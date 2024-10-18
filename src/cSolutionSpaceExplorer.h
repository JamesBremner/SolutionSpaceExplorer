#include <string>
#include <vector>

class cSolutionSpaceExplorer
{
    public:

    /// @brief set the objective function to be maximized
    /// @param sf 
    /// ex: "c1 * v1 + c2 * v2"

    void objective(
        const std::string& sf    );


    void variables(
        const std::vector<std::string>& vsv    );
    void consts(
        const std::string& name,
        double v    );
    void constraint(
        const std::string& sc    );
    void parse();
    void search();

    private:
    std::string sObjective;                     // string objective function
    std::vector<int> pObjective;                // parsed Objective function
    std::vector<std::string> vVariableNames;    
    std::vector<std::pair<std::string,double>> vConsts;

    /// @brief Parse the objective function string
    ///
    /// the result is stored in pObjective
    /// constant1 index, variable1 index, constant2 index, ...

    void parseObjective();
};