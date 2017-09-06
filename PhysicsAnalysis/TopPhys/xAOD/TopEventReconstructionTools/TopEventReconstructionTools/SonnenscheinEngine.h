/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SONNENSCEINENGINE_H_
#define SONNENSCEINENGINE_H_

#include <vector>
#include <string>
#include <utility>

#include "TLorentzVector.h"

namespace top {

/**
 * @brief Rewrite the top and anti-top decays as a quartic polynomial using top
 * and W mass constraints. And solve.
 *
 * Implements http://arxiv.org/abs/hep-ph/0510100 and
 * http://arxiv.org/abs/hep-ph/0603011.  Solves exactly the 4-th order polynomial
 * and returns the neutrino Lorentz vectors for both neutrinos in the event.
 *
 */
class SonnenscheinEngine {
public:
    /**
     * @brief Does nothing except sets the default flag to false.
     */
    SonnenscheinEngine();

    /**
     * @brief Does nothing.
     */
    ~SonnenscheinEngine();

    /**
     * @brief Does the real work.
     *
     * Supply it with the Lorentz vectors of the (lepton, b and a top and W mass)
     * * 2 and the MET ex and ey components. Note that if you give it 'truth'
     * particles and the truth W and t masses then it should reproduce the neutrinos
     * exactly in one of the solutions.
     *
     * @return A vector of up-to 4 solutions.  Note that when you called this
     * function you assumed a lepton-b pairing, so if you want to try the other
     * one you'll have to call it again with the b and bbar swapped or something.
     */
    std::vector<std::pair<TLorentzVector, TLorentzVector> > solve(const TLorentzVector& me_t_lp, const TLorentzVector& me_t_b, double mass_t, double mass_wp, const TLorentzVector& me_tbar_lm, const TLorentzVector& me_tbar_bbar, double mass_tbar, double mass_wm, double me_mex, double me_mey) const;

private:
    /**
     * @brief Solve a quartic.
     *
     * You're probably confused.  It says quartic, but takes five parameters as
     * input.  First it divides everything by h0 - so that gives four parameters.
     *
     * In the original code they have lots of cases to test when something == 0.
     * Not sure this would ever run - so I deleted it :).
     */
    std::vector<double> solveQuartic(double h0, double h1, double h2, double h3, double h4) const;

    /**
     * @brief Solves a cubic. No, really!
     *
     * Similar cases to check something == 0 left out.
     * z^3 + s1z^2 + s2z + s3
     *
     * @return Either 3 solutions, or a single one.
     */
    std::vector<double> solveCubic(double s1, double s2, double s3) const;

    /**
     * @brief Another Ronseal (google it) of a function.
     *
     * @param a Coefficient of x^2
     * @param b Coefficient of x^1
     * @param c Coefficient of x^0
     * @return A vector of (up-to) two solutions.
     */
    std::vector<double> solveQuadratic(double a, double b, double c) const;

    /**
     * @brief Is a number positive or negative.  This little gem will let you know.
     *
     * @param a The number in question.
     * @return Plus or minus one.
     */
    double sign(double a) const;

    ///Print a load of debug information (probably not by default, right)?
    bool m_debug;
};

}

#endif
