// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTAU_VERSIONS_TAUTRACKAUXCONTAINER_V1_H
#define XAODTAU_VERSIONS_TAUTRACKAUXCONTAINER_V1_H

// System include(s):
#include <vector>
#include <cstdint>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace xAOD {
 
 
  class TauTrackAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    TauTrackAuxContainer_v1();
 
  private:
    std::vector< float > pt;
    std::vector< float > eta;
    std::vector< float > phi;
    //std::vector< float > m;

    typedef uint16_t TrackFlagType;
    std::vector< TrackFlagType > flagSet;
    
    std::vector< std::vector< float > > bdtScores;

    // std::vector< float > z0sinThetaTJVA;
    // std::vector< float > rConv;
    // std::vector< float > rConvII;
    // std::vector< float > dRJetSeedAxis;

    // std::vector< float > etaStrip;
    // std::vector< float > phiStrip;
      

    typedef std::vector< ElementLink< TrackParticleContainer > > TrackLinks_t;
    std::vector< TrackLinks_t > trackLinks;
  }; // class TauTrackAuxContainer_v1
 
} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TauTrackAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTAU_VERSIONS_TAUTRACKAUXCONTAINER_V1_H
