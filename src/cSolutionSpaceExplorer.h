#include <string>
#include <vector>

class cSolutionSpaceExplorer
{
public:
    //////// Problem specification ///////////

    /// @brief set the objective function to be maximized
    /// @param sf
    /// ex: "c1 * v1 + c2 * v2"

    void objective(
        const std::string &sf);

    void variables(
        const std::vector<std::string> &vsv);
    void consts(
        const std::string &name,
        double v);
    void constraint(
        const std::string &sc);

    //////// Solving //////////////////////////

    /// @brief Parse the problem specification
    ///
    /// must be called after complete model specification
    /// and before any solution alghorithm
    
    void parse();

    /// @brief Exhaustive search of entire solution space

    void search();

    //////// Results /////////////////////////

    double ObjectiveValue() const
    {
        return objectiveValue;
    }
    std::vector<double> variableValues() const
    {
        return vVarVals;
    }

private:
    std::string sObjective;      // string objective function
    std::vector<int> pObjective; // parsed Objective function
    std::vector<std::string> vVariableNames;
    std::vector<std::pair<std::string, double>> vConsts;

    std::vector<double> vVarVals, vVarOptVals; // variable values
    double objectiveValue;

    /// @brief Parse the objective function string
    ///
    /// the result is stored in pObjective
    /// constant1 index, variable1 index, constant2 index, ...

    void parseObjective();

    /// @brief Calculate the objective function value

    double calcObjective();

    /// @brief odometer style iteration through test values
    /// @param[in,out] test vector of test values
    /// @param[in] max maximum test value
    /// @return true if more to come, false if iteration complete

    bool nextTestValues(
        std::vector<double> &test,
        int max,
        int rez);
};