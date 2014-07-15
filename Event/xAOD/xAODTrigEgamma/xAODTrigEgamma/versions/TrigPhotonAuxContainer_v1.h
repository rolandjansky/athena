// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonAuxContainer_v1.h 602896 2014-06-20 12:48:42Z krasznaa $
#ifndef XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONAUXCONTAINTER_V1_H
#define XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONAUXCONTAINTER_V1_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

namespace xAOD {

   /// Class holding the payload of xAOD::TrigPhotonContainer_v1
   ///
   /// This is the class holding all the data that the HLT saves for
   /// <code>xAOD::TrigPhotonContainer_v1</code>.
   ///
   /// @author Fabrizio Salvatore <Fabrizio.Salvatore@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 602896 $
   /// $Date: 2014-06-20 14:48:42 +0200 (Fri, 20 Jun 2014) $
   ///
   class TrigPhotonAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TrigPhotonAuxContainer_v1();

   private:
      /// @name Simple photon properties
      /// @{
      std::vector< uint32_t > roiWord;
      std::vector< float > rcore;
      std::vector< float > eratio;
      std::vector< float > etHad;
      std::vector< float > etHad1;
      std::vector< float > f0;
      std::vector< float > f1;
      std::vector< float > f2;
      std::vector< float > f3;
      std::vector< float > fside;
      std::vector< float > weta2;
      /// @}

      /// @name 4-momentum properties
      /// @{
      std::vector< float > pt;
      std::vector< float > caloEta;
      std::vector< float > caloPhi;
      std::vector< float > dEta;
      std::vector< float > dPhi;
      /// @}

      /// @name Link(s) to the photon constituents
      /// @{
      std::vector< ElementLink< TrigEMClusterContainer > > emClusterLink;
      /// @}

   }; // class TrigPhotonAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPhotonAuxContainer_v1, 1313537382, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigPhotonAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // not XAODTRIGEGAMMA_VERSIONS_TRIGPHOTONAUXCONTAINTER_V1_H
