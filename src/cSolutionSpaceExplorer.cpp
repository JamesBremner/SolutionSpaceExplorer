#include <iostream>
#include <sstream>
#include <algorithm>
#include "cRunWatch.h"
#include "cSolutionSpaceExplorer.h"

cSolutionSpaceExplorer::sVar::sVar( 
    const std::string& name, double max )
        : myName( name ),
        myMax( max ),
        myVal( 0 )
        {

        }

cSolutionSpaceExplorer::cSolutionSpaceExplorer()
    : myVariableMax(1)
{
}

void cSolutionSpaceExplorer::objective(const std::string &sf)
{
    sObjective = sf;
}

void cSolutionSpaceExplorer::variables(
    const std::vector<std::string> &vsv,
    double max)
{
    for (auto &name : vsv)
        vVars.emplace_back(name, max);
}

void cSolutionSpaceExplorer::consts(
    const std::string &name,
    double v)
{
    vConsts.push_back(std::make_pair(name, v));
}

void cSolutionSpaceExplorer::constraint(
    const std::string &sc)
{
    vsConstraint.push_back(sc);
}

void cSolutionSpaceExplorer::parse()
{
    raven::set::cRunWatch aWatcher("parse");
    parseObjective();
    parseConstraints();
    variableRanges();
}

std::vector<std::string> tokenize(const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

std::vector<int> cSolutionSpaceExplorer::parseProductSum(
    const std::string &line,
    std::string &compare,
    double &value)
{
    std::vector<int> ret;

    bool fvalueExpected = false;
    auto itv = vVars.end();
    auto itc = vConsts.end();

    for (auto &token : tokenize(line))
    {
        if (token.empty())
            continue;
        if (fvalueExpected)
        {
            value = atof(token.c_str());
            return ret;
        }
        switch (token[0])
        {

        case '*':
            break;

        case '+':
        case '\03':
            ret.push_back(itc - vConsts.begin());
            ret.push_back(itv - vVars.begin());
            itc = vConsts.end();
            break;

        case '<':
            ret.push_back(itc - vConsts.begin());
            ret.push_back(itv - vVars.begin());
            itc = vConsts.end();
            compare = token;
            fvalueExpected = true;
            break;

        default:
        {
            auto itvNow = std::find_if(
                vVars.begin(), vVars.end(),
                [&](const sVar &var)
                {
                    return var.myName == token;
                });
            if (itvNow == vVars.end())
            {
                // token is not a variable, look for constant
                itc = std::find_if(
                    vConsts.begin(), vConsts.end(),
                    [&](const std::pair<std::string, double> &c)
                    {
                        return c.first == token;
                    });
                if (itc == vConsts.end())
                    throw std::runtime_error(
                        "unrecognized token " + token);
            }
            else
            {
                // token is variable
                itv = itvNow;
            }
        }
        break;
        }
    }
    return ret;
}

void cSolutionSpaceExplorer::parseObjective()
{
    pObjective.clear();
    sObjective += " ";
    sObjective.push_back('\03'); // End of text
    vConsts.push_back(std::make_pair("One", 1));
    std::string null;
    double v;
    pObjective = parseProductSum(sObjective, null, v);
}

void cSolutionSpaceExplorer::parseConstraints()
{
    pConstraint.clear();
    sConstraint spc;
    for (auto &sc : vsConstraint)
    {
        sc += " ";
        sc.push_back('\03');
        std::string compareToken;
        spc.vParams = parseProductSum(
            sc,
            compareToken,
            spc.value);
        if (compareToken == "<")
            spc.compare = eCompare::lt;
        else if (compareToken == "<=")
            spc.compare = eCompare::le;
        else
            throw std::runtime_error(
                "unrecognized compare");

        pConstraint.push_back(spc);
    }
}

void cSolutionSpaceExplorer::variableRanges()
{
}
void cSolutionSpaceExplorer::search()
{
    raven::set::cRunWatch aWatcher("cSolutionSpaceExplorer::search");

    objectiveValue = 0;
    vTestVars = vVars;
    while (nextTestValues(vTestVars, myVariableMax, 1))
    {
        if (!isFeasible())
            continue;
        double o = calcObjective();
        if (o > objectiveValue)
        {
            std::cout << "objval " << o << " ";
            objectiveValue = o;
            vOptVars = vTestVars;
        }
    }
}
double cSolutionSpaceExplorer::calcObjective()
{
    // raven::set::cRunWatch aWatcher("calcObjective");
    double ret = 0;
    for (int p = 0; p < pObjective.size(); p += 2)
    {
        ret +=
            vConsts[pObjective[p]].second *
            vTestVars[pObjective[p + 1]].myVal;
    }
    return ret;
}

bool cSolutionSpaceExplorer::isFeasible()
{
    // raven::set::cRunWatch aWatcher("isFeasible");
    for (auto &C : pConstraint)
    {
        double v = 0;
        int *p = C.vParams.data();
        for (int i = 0; i < C.vParams.size(); i += 2)
        {
            v +=
                vConsts[*p].second *
                vTestVars[*(p + 1)].myVal;
            p++;
            p++;
        }
        switch (C.compare)
        {
        case eCompare::le:
            if (!(v <= C.value))
                return false;
            break;
        case eCompare::lt:
            if (!(v < C.value))
                return false;
            break;
        }
    }
    return true;
}

bool cSolutionSpaceExplorer::nextTestValues(
    std::vector<sVar> &test,
    int max,
    int rez)
{
    // raven::set::cRunWatch aWatcher("nextTestValues");
    int k = 0;
    while (true)
    {
        double *p = &test[k].myVal;
        *p += rez;
        if (*p <= test[k].myMax)
            break;
        // cary over
        *p = 0;
        k++;
        if (k == vVars.size())
        {
            // search is complete
            return false;
        }
    }

    return true;
}

double cSolutionSpaceExplorer::optVarValue(
    const std::string &varName) const
{
    auto it = std::find_if(
        vOptVars.begin(), vOptVars.end(),
        [&](const sVar &var)
        {
            return varName == var.myName;
        });
    if (it == vVars.end())
    {
        std::cout << "Cannot find var " << varName << "\n";
        return 0;
    }
    return it->myVal;
}

std::vector<std::pair<std::string, double>>
cSolutionSpaceExplorer::optVarNameValue() const
{
    std::vector<std::pair<std::string, double>> ret;
    for (auto &var : vOptVars)
    {
        ret.push_back(std::make_pair(
            var.myName,
            var.myVal));
    }
    return ret;
}