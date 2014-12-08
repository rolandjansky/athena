/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSMINIM_H
#define MUGIRLNSMINIM_H

#include "MuGirlStau/StauToolDefinitions.h"

namespace MuGirlNS
{
/**
 * Interface to the objective function
 */
class IObjective
{
public:
    /**
     * A function of one parameter
     * @param a The value of the parameter
     * @return The objective function evaluated at the given parameter
     */
    virtual double function(double a, StauTechnology eTech) = 0;
}; // class IObjective

/**
 * A minimizer of a function of one variable, using the golden section line
 * search method
 */
class Minimizer
{
public:
    /**
     * The main minimization routine
     * @param obj       An instance of the objective function object
     * @param amin      The lower bound of the search interval
     * @param amax      The upper bound of the search interval
     * @param tolerance The fraction of the origina interval where the
     *                  minimization stops. The default is 0.0001.
     * @return The values of the parameter and the function at the minimum
     */
    std::pair<double, double> minimize(IObjective& obj, StauTechnology eTech, double& amin, double& amax, double tolerance = 0.0001);
    void createGrid(IObjective& obj, StauTechnology eTech, double& amin, double& amax, double tolerance);
    double minF; /**< The last value of the objective function */
}; // class Minimizer
} // namespace MuGirlNS

#endif // MUGIRLNSMINIM_H
