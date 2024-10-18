#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

#include "cSolutionSpaceExplorer.h"

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          lb(wex::maker::make < wex::label >(fm))
    {
        lb.move(50, 50, 100, 30);
        lb.text("Hello World");

        show();
        run();
    }

private:
    wex::label &lb;
};

main()
{
    // a very simple employees to jobs assignment problem
    // one employess and two possible jobs

    // the solution explorer
    cSolutionSpaceExplorer ssex;

    // variables ( ti find values that optimize objective function )
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

    // parse the input
    ssex.parse();

    cGUI gui;

    return 0;
}
