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

void cSolutionSpaceExplorer::parse()
{
    parseObjective();
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

void cSolutionSpaceExplorer::parseObjective()
{
    pObjective.clear();
    sObjective += " ";
    sObjective.push_back('\03');      // End of text
    vConsts.push_back(std::make_pair("One", 1));
    auto itv = vVariableNames.end();
    auto itc = vConsts.end();

    for (auto &token : tokenize(sObjective))
    {
        if( token.empty() )
            continue;
        switch (token[0])
        {

        case '*':
            break;

        case '+':
        case '\03':
        {
            int ic = vConsts.size() - 1;
            if (itc != vConsts.end())
                ic = itc - vConsts.begin();
            pObjective.push_back(ic);
            pObjective.push_back(itv - vVariableNames.begin());
        }
        break;

        default:
        {
            itc = vConsts.end();
            auto itvNow = std::find(
                vVariableNames.begin(), vVariableNames.end(),
                token);
            if (itvNow == vVariableNames.end()) {
                // token is not a variable, look for constant
                itc = std::find_if(
                    vConsts.begin(), vConsts.end(),
                    [&](const std::pair<std::string, double> &c)
                    {
                        return c.first == token;
                    });
            } else {
                // token is variable
                itv = itvNow;
            }
        }
        break;
        }
    }
}
