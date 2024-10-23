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

    void searchHiLoRez();

    //////// Results /////////////////////////

    double objective() const
    {
        return objectiveValue;
    }
    double optVarValue( const std::string& varName ) const;

    std::vector<std::pair<std::string,double>>
    optVarNameValue() const;

private:

    /// A pair of a vector variable indices ( first )
    /// and a vector of constant indices ( second )
    /// the are to be all multiplied together
    /// to form part of a product sum

    typedef std::pair<std::vector<int>,std::vector<int>> product_t;

    /// A vector of products to be summed

    typedef std::vector<product_t> productSum_t;

    /// A constraint

    enum class eCompare
    {
        lt,
        le,
    };
    struct sConstraint
    {
        productSum_t vParams;   // product sum
        eCompare compare;       // comparison operator
        double value;           // value to be compared to the product sum
    };

    /// @brief A variable

    struct sVar
    {
        std::string myName;     // user name
        double myMax;           // maximum allowed value
        double myVal;           // value

        sVar( const std::string& name, double max );
    };

    std::string sObjective;      // string specifying objective function
    productSum_t pObjective;     // parsed Objective function
    std::vector<sVar> vVars;     // string specifying variables
    std::vector<std::pair<std::string, double>> vConsts;    // string specifying constant
    //int myVariableMax;


    std::vector<std::string> vsConstraint; // strings specifying constraints
    std::vector<sConstraint> pConstraint;  // parsed constraints

    std::vector<sVar> vTestVars;    // test variable values
    std::vector<sVar> vOptVars;     // optimal variable values
    double objectiveValue;          // value of objective function

    /// @brief Parse the objective function string
    ///
    /// the result is stored in pObjective
    /// constant1 index, variable1 index, constant2 index, ...

    void parseObjective();

    void parseConstraints();

    productSum_t parseProductSum(
        const std::string &line,
        std::string &compare,
        double& value);

    void variableRanges();

    /// @brief Calculate the objective function value

    double calcObjective();

    bool isFeasible();

    double calcProductSum( const productSum_t& ps );

    /// @brief odometer style iteration through test values
    /// @param[in,out] test vector of test values
    /// @return true if more to come, false if iteration complete

    bool nextTestValues(
        std::vector<sVar> &test,
        int rez);
};