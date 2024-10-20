#include <string>
#include <vector>

class cSolutionSpaceExplorer
{
public:
    //////// Problem specification ///////////

    /// @brief set the variable names
    /// @param vsv vector of variable names

    void variables(
        const std::vector<std::string> &vsv);

    /// @brief add a constant
    /// @param name of constant
    /// @param v value of constant
    
    void consts(
        const std::string &name,
        double v);

    /// @brief add a constraint
    /// @param sc 
    /// ex "c1 * v1 + c2 * v2 <= 1"

    void constraint(
        const std::string &sc);

    /// @brief set the objective function to be maximized
    /// @param sf
    /// ex: "c1 * v1 + c2 * v2"
    /// note single spaces between tokens

    void objective(
        const std::string &sf);

    //////// Solving //////////////////////////

    /// @brief Parse the problem specification
    ///
    /// must be called after complete model specification
    /// and before any solution algorithm.
    /// This allows specification methods to be called in any order

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

    enum class eCompare
    {
        lt,
        le,
    };
    struct sConstraint
    {
        std::vector<int> vParams;
        eCompare compare;
        double value;
    };
    std::vector<std::string> vsConstraint; // constraint string specifiers
    std::vector<sConstraint> pConstraint;

    std::vector<double> vVarVals, vVarOptVals; // variable values
    double objectiveValue;

    /// @brief Parse the objective function string
    ///
    /// the result is stored in pObjective
    /// constant1 index, variable1 index, constant2 index, ...

    void parseObjective();

    void parseConstraints();

    std::vector<int> parseProductSum(
        const std::string &line,
        std::string &compare,
        double& value);

    /// @brief Calculate the objective function value

    double calcObjective();

    bool isFeasible();

    /// @brief odometer style iteration through test values
    /// @param[in,out] test vector of test values
    /// @param[in] max maximum test value
    /// @return true if more to come, false if iteration complete

    bool nextTestValues(
        std::vector<double> &test,
        int max,
        int rez);
};