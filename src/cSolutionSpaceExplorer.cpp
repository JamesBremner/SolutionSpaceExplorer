#include <sstream>
#include <algorithm>
#include "cSolutionSpaceExplorer.h"

void cSolutionSpaceExplorer::objective(const std::string &sf)
{
    sObjective = sf;
}

void cSolutionSpaceExplorer::variables(
    const std::vector<std::string> &vsv)
{
    vVariableNames = vsv;
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
    parseObjective();
    parseConstraints();
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
    auto itv = vVariableNames.end();
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
            ret.push_back(itv - vVariableNames.begin());
            //itc = vConsts.end();
            break;

        case '<':
            ret.push_back(itc - vConsts.begin());
            ret.push_back(itv - vVariableNames.begin());
            //itc = vConsts.end();
            compare = token;
            fvalueExpected = true;
            break;

        default:
        {
            itc = vConsts.end() - 1;
            auto itvNow = std::find(
                vVariableNames.begin(), vVariableNames.end(),
                token);
            if (itvNow == vVariableNames.end())
            {
                // token is not a variable, look for constant
                itc = std::find_if(
                    vConsts.begin(), vConsts.end(),
                    [&](const std::pair<std::string, double> &c)
                    {
                        return c.first == token;
                    });
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
void cSolutionSpaceExplorer::search()
{
    objectiveValue = 0;
    vVarVals.clear();
    vVarVals.resize(vVariableNames.size(), 0);
    while (nextTestValues(vVarVals, 1, 1))
    {
        if (!isFeasible())
            continue;
        double o = calcObjective();
        if (o > objectiveValue)
        {
            objectiveValue = o;
            vVarOptVals = vVarVals;
        }
    }
}
double cSolutionSpaceExplorer::calcObjective()
{
    double ret = 0;
    for (int p = 0; p < pObjective.size(); p += 2)
    {
        ret +=
            vConsts[pObjective[p]].second *
            vVarVals[pObjective[p + 1]];
    }
    return ret;
}

bool cSolutionSpaceExplorer::isFeasible()
{
    for (auto &C : pConstraint)
    {
        double v = 0;
        for (int p = 0; p < C.vParams.size(); p += 2)
        {
            v +=
                vConsts[C.vParams[p]].second *
                vVarVals[C.vParams[p + 1]];
        }
        switch (C.compare)
        {
        case eCompare::le:
            if ( ! ( v <= C.value) )
                return false;
            break;
        case eCompare::lt:
            if ( ! (v < C.value))
                return false;
            break;
        }
    }
    return true;
}

bool cSolutionSpaceExplorer::nextTestValues(
    std::vector<double> &test,
    int max,
    int rez)
{
    int k = 0;
    while (true)
    {
        double *p = &test[k];
        *p += rez;
        if (*p <= max)
            break;
        // cary over
        *p = 0;
        k++;
        if (k == vVariableNames.size())
        {
            // search is complete
            return false;
        }
    }

    return true;
}
