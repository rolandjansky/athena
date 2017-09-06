/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTOOLS_H_
#define EVENTTOOLS_H_

#include <string>

#include "xAODMissingET/MissingET.h"

/**
 * @file EventTools.h
 *
 * @brief A few functions for doing operations on particles / events.  Currently
 * holds code for dR, dPhi, MWT, HT, invariantMass, isSimulation and sorting
 * containers in descending pT order.  Note that the xAOD currently doesn't know
 * if it's fast sim or full sim.
 */

namespace xAOD {
class IParticle;
}

namespace top {
class Event;

/**
 * @brief Print an error message and terminate if thingToCheck is false.
 *
 * Lots of the xAOD functions need checking to see if they work or not.  I guess
 * we take the approach that if it didn't work, we should not really try to
 * recover but just print a message and quit as soon as we can.  So, this will
 * either return nothing or quit your program.
 *
 * @param thingToCheck The thing to check.  If it's false then print the error
 * message and quit.
 */
void check(bool thingToCheck, const std::string& usefulFailureMessage);

/**
 * @brief Calculate the delta-r distance between two particles (e.g. an Electron
 * and a Muon).
 *
 * You could do this directly with the xAOD interface, but like invariant mass,
 * I think it's nicer like this.
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 * @return delta-R metric.
 */
double deltaR(const xAOD::IParticle& p1, const xAOD::IParticle& p2);

/**
 * @brief Calculate delta-phi between two particles (e.g. an Electron and a
 * Muon)
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 * @return Signed delta-phi in radians.
 */
double deltaPhi(const xAOD::IParticle& p1, const xAOD::IParticle& p2);

/**
 * @brief Calculate the transverse mass of the W from the charged lepton and
 * the missing ET.  The maths looks something a bit like this:
 *
 * sqrt(2. * lepton.pt() * met.met() * (1. - cos(lepton.phi() - met.phi())))
 *
 * @return The calculated value, obviously.
 */
double mwt(const xAOD::IParticle& lepton, const xAOD::MissingET& met);

/**
 * @brief HT calculation.
 *
 * Uses the pT of the electrons, muons and jets.  If they are present.  It loops
 * through those three containers and adds up the pT.
 *
 * @return The HT value.
 */
double ht(const top::Event& event);

/**
 * @brief Calculate the invariant mass of two particles.
 *
 * This can be done using the xAOD objects directly, but I think this type
 * of interface is prettier.
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 *
 * @return The invariant mass of the pair.
 */
double invariantMass(const xAOD::IParticle& p1, const xAOD::IParticle& p2);

/**
 * @brief Is this event MC simulation (True) or data (False)?
 *
 * @return True if the event is MC simulation.
 */
bool isSimulation(const top::Event& event);

/**
 * @brief Used when sorting the e, mu, jet, tau containers after CP corrections.
 *
 * In case they have changed order. Containers have pointers in, so this should
 * take pointers as arguments.
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 *
 */
bool descendingPtSorter(const xAOD::IParticle* p0, const xAOD::IParticle* p1);

}

#endif
