/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/// Interface to work with truth trajectories.
/// This interface implies that a GenParticle can belong to no more than one TruthTrajectory.
/// 
/// @author Andrei Gaponenko <agaponenko@lbl.gov>

#ifndef ITRUTHTRAJECTORYBUILDER_H
#define ITRUTHTRAJECTORYBUILDER_H

#include "GaudiKernel/IAlgTool.h"

// Forard declarations
class TruthTrajectory;
#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
  
  static const InterfaceID IID_ITruthTrajectoryBuilder("Trk::ITruthTrajectoryBuilder",1,0);

  class ITruthTrajectoryBuilder : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() { return IID_ITruthTrajectoryBuilder; }

    /** Build a TruthTrajectory this particle belongs to.  
     *  The result may be an empty TruthTrajectory (in case input did not pass the cuts).
     */
    virtual void buildTruthTrajectory(TruthTrajectory *result, const HepMC::GenParticle *input) const = 0;

    /** Previous particle on the truth trajectory or 0 */
    virtual const HepMC::GenParticle *getMother(const HepMC::GenParticle *part) const = 0;

    /** Next particle on the truth trajectory or 0 */
    virtual const HepMC::GenParticle *getDaughter(const HepMC::GenParticle *part) const = 0;
  };
  
} // namespace Trk

#endif/*ITRUTHTRAJECTORYBUILDER_H*/
