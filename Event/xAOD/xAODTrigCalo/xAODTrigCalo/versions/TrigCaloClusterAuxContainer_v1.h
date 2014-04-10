// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterAuxContainer_v1.h 592214 2014-04-09 19:27:48Z krasznaa $
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
   /// $Date: 2014-04-09 21:27:48 +0200 (Wed, 09 Apr 2014) $
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

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCaloClusterAuxContainer_v1, 1130546378, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigCaloClusterAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTERAUXCONTAINER_V1_H
