#include <string>
#include <vector>

class cSolutionSpaceExplorer
{
public:

    cSolutionSpaceExplorer();

    //////// Problem specification ///////////

    /// @brief set some variable names
    /// @param vsv vector of variable names
    /// @param max maximum value of the variables

    void variables(
        const std::vector<std::string> &vsv,
        double max);

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

    double objective() const
    {
        return objectiveValue;
    }
    double optVarValue( const std::string& varName ) const;

    std::vector<std::pair<std::string,double>>
    optVarNameValue() const;

private:

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

    /// @brief A variable

    struct sVar
    {
        std::string myName;
        double myMax;
        double myVal;

        sVar( const std::string& name, double max );
    };
    std::string sObjective;      // string objective function
    std::vector<int> pObjective; // parsed Objective function
    std::vector<sVar> vVars;     // input variables
    std::vector<std::pair<std::string, double>> vConsts;
    int myVariableMax;


    std::vector<std::string> vsConstraint; // constraint string specifiers
    std::vector<sConstraint> pConstraint;

    std::vector<sVar> vTestVars;    // test variable values
    std::vector<sVar> vOptVars;     // optimal variable values
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

    void variableRanges();

    /// @brief Calculate the objective function value

    double calcObjective();

    bool isFeasible();

    /// @brief odometer style iteration through test values
    /// @param[in,out] test vector of test values
    /// @param[in] max maximum test value
    /// @return true if more to come, false if iteration complete

    bool nextTestValues(
        std::vector<sVar> &test,
        int max,
        int rez);
};