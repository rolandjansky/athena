// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterAuxContainer_v1.h 631412 2014-11-26 21:59:17Z gwatts $
#ifndef XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTERAUXCONTAINER_V1_H
#define XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary container for trigger calorimeter cluster containers
   ///
   /// This is a fixed implementation of the trigger calorimeter cluster
   /// auxiliary store. Defining every possible property that trigger
   /// calorimeter clusters can have.
   ///
   /// @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revsision$
   /// $Date: 2014-11-26 22:59:17 +0100 (Wed, 26 Nov 2014) $
   ///
   class TrigCaloClusterAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TrigCaloClusterAuxContainer_v1();

   private:
      std::vector< float > rawEnergy;
      std::vector< std::vector< float > > rawEnergySample;
      std::vector< float > rawEt;
      std::vector< float > rawEta;
      std::vector< float > rawPhi;
      std::vector< long > RoIword;
      std::vector< int > nCells;
      std::vector< unsigned int > clusterQuality;

   }; // class TrigCaloClusterAuxContainer_v1

} // namespace xAOD

SG_BASE( xAOD::TrigCaloClusterAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTERAUXCONTAINER_V1_H
