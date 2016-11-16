// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJetAuxContainer_v2.h 631921 2015-09-23 23:30:59Z dkirchme $
#ifndef XAODDITAU_VERSIONS_DITAUJETAUXCONTAINER_V1_H
#define XAODDITAU_VERSIONS_DITAUJETAUXCONTAINER_V1_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}


// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/PFOContainer.h"

namespace xAOD {
 
 
    class DiTauJetAuxContainer_v1 : public AuxContainerBase {

    public:
     /// Default constructor
     DiTauJetAuxContainer_v1();
 
    private:
     std::vector< float > pt;
     std::vector< float > eta;
     std::vector< float > phi;
     std::vector< float > m;


    typedef ElementLink< xAOD::JetContainer > JetLink_t;
    std::vector< JetLink_t > jetLink;

    typedef ElementLink< xAOD::VertexContainer > VertexLink_t;
    std::vector< VertexLink_t > vertexLink;
    // typedef std::vector< ElementLink< xAOD::VertexContainer > >  SecondaryVertexLinks_t;
    // std::vector< VertexLink_t > secVertexLinks;

    std::vector< float > R_jet;
    std::vector< float > R_subjet;
    std::vector< float > R_core;
    std::vector< float > BDT;

    std::vector< std::vector< float > > subjet_pt;
    std::vector< std::vector< float > > subjet_eta;
    std::vector< std::vector< float > > subjet_phi;
    std::vector< std::vector< float > > subjet_e;

    std::vector< std::vector< float > > subjet_f_core;

    std::vector< float > TauJetVtxFraction;

    typedef std::vector< ElementLink< TrackParticleContainer > > TrackLink_t;
    std::vector< TrackLink_t > trackLinks;
    std::vector< TrackLink_t > isoTrackLinks;
    std::vector< TrackLink_t > otherTrackLinks;
 
       
   }; // class DiTauJetAuxContainer_v1
 
} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::DiTauJetAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODDITAU_VERSIONS_DITAUJETAUXCONTAINER_V1_H
