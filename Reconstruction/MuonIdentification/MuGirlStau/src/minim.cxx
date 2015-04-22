/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/minim.h"
#include <cmath>
#include <utility>

using namespace MuGirlNS;

#if defined(__GNUC__) && !defined(__clang__)
// constexpr sqrt is a nonstandard extension implemented by gcc
constexpr double GOLD2 = (sqrt(5.0) - 1.0) / 2.0;
constexpr double GOLD1 = 1.0 - GOLD2;
#else
static const double GOLD2 = (sqrt(5.0) - 1.0) / 2.0;
static const double GOLD1 = 1.0 - GOLD2;
#endif

std::pair<double, double> Minimizer::minimize(
        IObjective& obj,
        StauTechnology eTech,
        double& amin,
        double& amax,
        double tolerance)
{
    double a0 = amin;
    obj.function(a0, eTech);
    double a3 = amax;
    obj.function(a3, eTech);
    double a1 = GOLD2 * a0 + GOLD1 * a3, f1 = obj.function(a1, eTech);
    double a2 = GOLD1 * a0 + GOLD2 * a3, f2 = obj.function(a2, eTech);
    double tol = tolerance * (a3 - a0);

    while (a3 - a0 > tol)
    {
        if (f1 > f2)
        {
            a0 = a1;
            a1 = a2;
            f1 = f2;
            a2 = GOLD1 * a3 + GOLD2 * a1;
            f2 = obj.function(a2, eTech);
        }
        else
        {
            a3 = a2;
            a2 = a1;
            f2 = f1;
            a1 = GOLD2 * a2 + GOLD1 * a0;
            f1 = obj.function(a1, eTech);
        }
    }
    a1 = (a1 + a2) / 2.0;
    minF = obj.function(a1, eTech);
//     amin = a0;
//     amax = a3;
    return std::make_pair(a1, minF);
}

void Minimizer::createGrid(
        IObjective& obj,
        StauTechnology eTech,
        double& amin,
        double& amax,
        double tolerance)
{
//    double currentPoint = amin;
//
//    while (currentPoint <= amax)
//    {
//        obj.function(currentPoint, eTech);
//        currentPoint += tolerance;
//    }
    for (double currentPoint = amin; currentPoint <= amax; currentPoint += tolerance)
        obj.function(currentPoint, eTech);
}

// To use the following test code, compile this module with -DTESTING

#ifdef TESTING
#include <iostream>

class parabola : public IObjective
{
public:
    virtual double function(double a)
    {
        return 1.0 + 2.0 * a + 3.0 * a * a;
    }
};

int main(int argc, char* argv[])
{
    parabola p;
    Minimizer minim;
    double mina = minim.minimize(p, -2.0, 2.0, 0.0001);

    //std::cout << "Minimum is f=" << minim.minF << " at a=" << mina << std::endl;
}
#endif // TESTING

