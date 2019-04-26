// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id: TrackCaloClusterAuxContainer_v2.h $
#ifndef XAODPFLOW_VERSIONS_TRACKCALOCLUSTERAUXCONTAINER_v2_H
#define XAODPFLOW_VERSIONS_TRACKCALOCLUSTERAUXCONTAINER_v2_H
 
// System include(s):
// #include <stdint.h>
#include <vector>
 
// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {
 
   
   /// This class is meant to implement the TrackCaloCluster EDM.
   ///
   /// @author Noemi Calace
   ///
   class TrackCaloClusterAuxContainer_v2 : public AuxContainerBase {
 
   public:
      /// Default constructor
      TrackCaloClusterAuxContainer_v2();
      
      /// Dumps contents (for debugging)
      void dump() const;
       
   private:
     
     /// @name Defining parameters 
     /// @{
     std::vector< float > pt;
     std::vector< float > eta;
     std::vector< float > phi;
     std::vector< float > m;
     std::vector< int >   taste;
     /// @}
     
     /// @name Links
     /// @{
     std::vector<ElementLink< xAOD::TrackParticleContainer > >             trackParticleLink;
     std::vector<std::vector< ElementLink< xAOD::IParticleContainer > > >  iparticleLinks;

     /// @}
     

   }; // class TrackCaloClusterAuxContainer_v2
 
} // namespace xAOD
 
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrackCaloClusterAuxContainer_v2, xAOD::AuxContainerBase ); 
 
#endif // XAODPFLOW_VERSIONS_TRACKCALOCLUSTERAUXCONTAINER_v2_H
