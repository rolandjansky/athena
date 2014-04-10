// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterAuxContainer_v1.h 592214 2014-04-09 19:27:48Z krasznaa $
#ifndef XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERAUXCONTAINER_V1_H
#define XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloClusterAuxContainer_v1.h"

namespace xAOD {

   /// Auxiliary container for trigger EM cluster containers
   ///
   /// This is a fixed implementation of the trigger EM cluster
   /// auxiliary store. Defining every possible property that trigger
   /// EM clusters can have.
   ///
   /// @author Aranzazu Ruiz Martinez <Aranzazu.Ruiz.Martinez@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revsision$
   /// $Date: 2014-04-09 21:27:48 +0200 (Wed, 09 Apr 2014) $
   ///
   class TrigEMClusterAuxContainer_v1 : public TrigCaloClusterAuxContainer_v1 {

   public:
      /// Default constructor
      TrigEMClusterAuxContainer_v1();

   private:
      std::vector< float > energy;
      std::vector< std::vector< float > > energySample;
      std::vector< float > et;
      std::vector< float > eta;
      std::vector< float > phi;
      std::vector< float > e237;
      std::vector< float > e277;
      std::vector< float > fracs1;
      std::vector< float > weta2;
      std::vector< float > ehad1;
      std::vector< float > eta1;
      std::vector< float > emaxs1;
      std::vector< float > e2tsts1;
      std::vector< float > e233;
      std::vector< float > wstot;

   }; // class TrigEMClusterAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigEMClusterAuxContainer_v1, 1111649561, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigEMClusterAuxContainer_v1, xAOD::TrigCaloClusterAuxContainer_v1 );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERAUXCONTAINER_V1_H
