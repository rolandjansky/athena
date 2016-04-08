// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterAuxContainer_v1.h 639154 2015-01-13 14:35:02Z wigleswt $
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
   /// $Date: 2015-01-13 15:35:02 +0100 (Tue, 13 Jan 2015) $
   ///
   class TrigCaloClusterAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TrigCaloClusterAuxContainer_v1();

   private:
     /// Include the TrigCaloCluster variables:
#define TRIGCALOCLUSTER_AUX_VARIABLE( TYPE, NAME ) TYPE NAME
#include "xAODTrigCalo/versions/TrigCaloClusterAuxVariables_v1.def"
#undef TRIGCALOCLUSTER_AUX_VARIABLE

   }; // class TrigCaloClusterAuxContainer_v1

} // namespace xAOD

SG_BASE( xAOD::TrigCaloClusterAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGCALO_VERSIONS_TRIGCALOCLUSTERAUXCONTAINER_V1_H
