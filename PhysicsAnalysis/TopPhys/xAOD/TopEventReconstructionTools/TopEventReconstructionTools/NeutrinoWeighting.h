/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOWEIGHTING_H_
#define NEUTRINOWEIGHTING_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {
class Event;

/**
 * @brief Holds the two solutions from the quadratic equation as TLorentVectors.
 *
 * Filled when solving the quadratic equation to calculate the 4 Vectors for
 * the two neutrinos in the event.
 */
class NWSolution {
public:
    void setSolutions(int num, const TLorentzVector& a, const TLorentzVector& b) {
        m_v1 = a;
        m_v2 = b;
        m_solutions = num;
    }

    void setSolutions(int num) {
        m_solutions = num;
    }

    int getNumSolutions() const {
        return m_solutions;
    }

    TLorentzVector getv1() const {
        return m_v1;
    }

    TLorentzVector getv2() const {
        return m_v2;
    }

private:
    ///Number of solutions. 0 or 2, one solution isn't possible due to the use of floats.
    int m_solutions;

    ///First solution from the quadratic, check m_solutions is 2 first.
    TLorentzVector m_v1;

    ///Second solution from the quadratic, check m_solutions is 2 first.
    TLorentzVector m_v2;
};

/**
 * @brief Not complete. Work in progress
 */
class NeutrinoWeighting : public EventSelectorBase {
public:
    NeutrinoWeighting();
    virtual ~NeutrinoWeighting();

    bool apply(const top::Event&) const override;

    std::string name() const { return "RECO:NEUTRINOWEIGHTING"; }

private:
    /**
     * @param lepton  The lepton 4 vector.
     * @param bJet    The b jet 4 vector.
     * @param topMass The top mass is a constraint in the spin correlation calculation.
     * @param index   Gives the position in the neutrino eta array of this neutrino.
     */
    NWSolution solveForNeutrinoEta(const TLorentzVector& lepton, const TLorentzVector& bJet, double topMass, int index) const;

    /**
     * @brief Calculate the weight for this combination of particles by comparing with Met.
     *
     * Weight a solution based on the agreement with missing Et.
     *
     * @param neutrino1 The first neutrino (actually the order doesn't matter).
     * @param neutrino2 The second neutrino.
     * @param met_ex    The x component of missing Et.
     * @param met_ey    The y component of missing Et.
     *
     * @return The weight of this solution based on the input neutrinos.
     */
    double neutrino_weight(const TLorentzVector&, const TLorentzVector&, double, double) const;

    ///Sampling points
    double neutrinos[2000][3];

    ///Delta eta sampling size.  For the mass we don't need to sample so many points.
    int etaSize;

    ///Top Quark Mass constraint.
    double topMass;

    ///B jet mass constraint.
    double m_bmass;

    ///W jet mass constraint.
    double m_wmass;

    //For weighter.
    double sigmax;

    //For weighter.
    double sigmay;

};

}

#endif
