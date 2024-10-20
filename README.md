# SolutionSpaceExplorer

Solution Space Explorer for mathematical optimization.

# Problem Specification
```
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
```

These methods can be called in any order.  Once specification is complete, method `parse()` must be called.

# Solution

Currently, one algorithm is implemented - and exhaustive seach through the solution space

# Example

```
    // a very simple employees to jobs assignment problem
    // one employess and two possible jobs

    // the solution explorer
    cSolutionSpaceExplorer ssex;

    // variables ( to find values that optimize objective function )
    // e1toj1 means employee 1 is assigned to job 1
    ssex.variables( { "e1toj1", "e1toj2"});

    // constants
    // e1fj1  the effectivenes of assigning employee 1 to job 1
    ssex.consts("e1fj1",10);
    ssex.consts("e1fj2",100);

    // objective function
    ssex.objective(
        "e1toj1 * e1fj1 + "
        "e1toj2 * e1fj2 "    );

    // employee can only be assigned one job
    ssex.constraint(
        "e1toj1 + e1toj2 <= 1"    );

    // parse the input
    ssex.parse();

    ssex.search();
```
Output is
```
Employee assigned to job 2  for total value 100
```

