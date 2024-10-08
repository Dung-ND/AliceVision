// This file is part of the AliceVision project.
// Copyright (c) 2016 AliceVision contributors.
// Copyright (c) 2012 openMVG contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <aliceVision/system/Logger.hpp>
#include <aliceVision/linearProgramming/ISolver.hpp>

#include <iostream>
#include <iterator>
#include <vector>

namespace aliceVision {
namespace linearProgramming {

/// Generic Bisection algorithm via Linear Programming.
/// Use dichotomy or mid-point best parameter that fit the solution.
/// http://en.wikipedia.org/wiki/Bisection_method
/// The bisection algorithm continue as long as
///  precision or max iteration number is not reach.
///
template<typename ConstraintBuilder, typename ConstraintType>
bool BisectionLP(ISolver& solver,
                 ConstraintBuilder& cstraintBuilder,
                 std::vector<double>* parameters,
                 double gammaUp = 1.0,                 // Upper bound
                 double gammaLow = 0.0,                // lower bound
                 double eps = 1e-8,                    // precision that stop dichotomy
                 const int maxIteration = 20,          // max number of iteration
                 double* bestFeasibleGamma = nullptr,  // value of best bisection found value
                 bool bVerbose = false)
{
    int k = 0;
    bool bModelFound = false;
    ConstraintType constraint;
    do
    {
        ++k;  // One more iteration

        double gamma = (gammaLow + gammaUp) / 2.0;

        //-- Setup constraint and solver
        cstraintBuilder.Build(gamma, constraint);
        solver.setup(constraint);
        //--
        // Solving
        bool bFeasible = solver.solve();
        //--

        if (bFeasible)
        {
            gammaUp = gamma;
            if (bestFeasibleGamma)
                *bestFeasibleGamma = gamma;
            solver.getSolution(*parameters);
            bModelFound = true;

            if (bVerbose)
                ALICEVISION_LOG_DEBUG(k << "/" << maxIteration << "\t gamma " << gamma << "\t gammaUp-gammaLow " << gammaUp - gammaLow);
        }
        else
        {
            gammaLow = gamma;
            if (bVerbose)
                ALICEVISION_LOG_DEBUG("Not feasible with gamma: " << gamma);
        }
    } while (k < maxIteration && gammaUp - gammaLow > eps);

    return bModelFound;
}

}  // namespace linearProgramming
}  // namespace aliceVision
