#include <iostream>
#include <sstream>
#include <algorithm>
#include "cRunWatch.h"
#include "cSolutionSpaceExplorer.h"

cSolutionSpaceExplorer::sVar::sVar(
    const std::string &name, double max)
    : myName(name),
      myMax(max),
      myVal(0)
{
}

cSolutionSpaceExplorer::cSolutionSpaceExplorer()
// : myVariableMax(1)
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

cSolutionSpaceExplorer::productSum_t
cSolutionSpaceExplorer::parseProductSum(
    const std::string &line,
    std::string &compare,
    double &value)
{
    productSum_t ret;
    product_t product;

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
            ret.push_back(product);
            product.first.clear();
            product.second.clear();
            break;

        case '<':
            ret.push_back(product);
            product.first.clear();
            product.second.clear();
            
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
                product.first.push_back(itc - vConsts.begin());
            }
            else
            {
                // token is variable
                product.second.push_back(itvNow - vVars.begin());
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
    while (nextTestValues(vTestVars, 1))
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
double cSolutionSpaceExplorer::calcProductSum(const productSum_t &ps)
{
    double ret = 0;
    for (int p = 0; p < ps.size(); p += 2)
    {
        double prod = 1;
        for (int i : ps[p].first)
            prod *= vConsts[i].second;
        for (int i : ps[p].second)
            prod *= vTestVars[i].myVal;
        ret += prod;
    }
    return ret;
}

double cSolutionSpaceExplorer::calcObjective()
{
    // raven::set::cRunWatch aWatcher("calcObjective");
    return calcProductSum(pObjective);
}

bool cSolutionSpaceExplorer::isFeasible()
{
    // raven::set::cRunWatch aWatcher("isFeasible");
    for (auto &C : pConstraint)
    {
        double v = calcProductSum(C.vParams);

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