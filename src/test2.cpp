#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "cRunWatch.h"
#include "cSolutionSpaceExplorer.h"

main()
{
    raven::set::cRunWatch::Start();

    {
        raven::set::cRunWatch aWatcher("main");

        cSolutionSpaceExplorer ssex;

        {
            /*
            Employees are willing to push crates for money. They have certain attributes:
Each employee pushes crates at a certain rate. 
For example, Alice might push crates at 3 feet per dollar,
and Bob might push them at 2 feet per dollar

Each employee has a certain set of crate types that they're capable of pushing.
 For example, Alice might only be able to push crates that are labeled with an A, B, or C, 
 while Bob might only be able to push crates that are labeled with an A or a D

Each employee might have a limit to the total number of crates they're willing to push.
 For example, even though Alice is capable of pushing crates A, B or C,
  she might have a policy where she only pushes a maximum of 2 crates per job assignment

Each employee might also have a limit to the number of dollars they're willing to accept.
 For example, Alice might have a policy where she only accepts up to $35 per job assignment.

I have a certain budget for each crate. 
For example, let's say I have 5 crates, labeled A through E, and I'm willing to spend $20 on each crate,
 but my budget for each crate can only be used to push one specific crate.

My goal is to maximize the sum of the distance pushed for all the crates.

https://stackoverflow.com/q/79058180/16582
            */

            raven::set::cRunWatch aWatcher("specify");

            // employee assignments to crates
            ssex.variables(
                {"aaA", "aaB", "aaC", "aaD"},
                1);

            // employee payments for each crate
            ssex.variables(
                {"apA", "apB", "apC", "apD",
                 "bpA", "bpB", "bpC", "bpD"},
                20);

            // crate budgets
            ssex.constraint("apA + bpA <= 20");
            ssex.constraint("apB + bpB <= 20");
            ssex.constraint("apC + bpC <= 20");
            ssex.constraint("apD + bpD <= 20");

            // pay limit
            ssex.constraint("apA + apB + apC + apD < 35");

            // crate limit
            ssex.constraint("aaA + aaB + aaC + aaD < 2");

            // efficiencies
            ssex.consts("af", 3);
            ssex.consts("bf", 2);

            // objective to maximize distance pushed
            ssex.objective(
                "aaA * af * apA + aaB *af * apB + aaC * af * apC + aaD * af * apD + "
                "bf * bpA + bf * bpB + bf * bpC + bf * bpD");
        }

        ssex.parse();

        ssex.searchHiLoRez();

        for (auto &p : ssex.optVarNameValue())
            std::cout << p.first << ": " << p.second << " ";
        std::cout << "Total Distance " << ssex.objective() << "\n";
    }

    raven::set::cRunWatch::Report();
}
