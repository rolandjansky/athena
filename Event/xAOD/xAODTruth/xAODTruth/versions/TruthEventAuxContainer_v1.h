// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTAUXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTAUXCONTAINER_V1_H

#include <vector>

#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {


   /// Auxiliary store for the truth signal event
   ///
   /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
   ///
   class TruthEventAuxContainer_v1 : public AuxContainerBase {
   public:

      /// Default constructor
      TruthEventAuxContainer_v1();

   private:

     /// @todo Reinstate?
     // std::vector< int > signalProcessId;
     // std::vector< int > eventNumber;

     std::vector< ElementLink< TruthVertexContainer > > signalProcessVertexLink;
     std::vector< ElementLink< TruthParticleContainer > > beamParticle1Link;
     std::vector< ElementLink< TruthParticleContainer > > beamParticle2Link;

     /// @todo Needs to be a map or similar (perhaps two linked vectors?)
     std::vector< std::vector<float> > weights;

     std::vector< float > crossSection;
     std::vector< float > crossSectionError;

     std::vector< std::vector< ElementLink< TruthParticleContainer > > > truthParticleLinks;
     std::vector< std::vector< ElementLink< TruthVertexContainer > > > truthVertexLinks;


   }; // class TruthEventAuxContainer_v1

} // namespace xAOD


// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthEventAuxContainer_v1 , 1274265725 , 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthEventAuxContainer_v1, xAOD::AuxContainerBase );


#endif // XAODTRUTH_VERSIONS_TRUTHEVENTAUXCONTAINER_V1_H
