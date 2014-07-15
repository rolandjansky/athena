// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronAuxContainer_v1.h 602876 2014-06-20 11:21:26Z krasznaa $
#ifndef XAODTRIGEGAMMA_VERSIONS_TRIGELECTRONAUXCONTAINTER_V1_H
#define XAODTRIGEGAMMA_VERSIONS_TRIGELECTRONAUXCONTAINTER_V1_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace xAOD {

   /// Class holding the payload of xAOD::TrigElectronContainer_v1
   ///
   /// This is the class holding all the data that the HLT saves for
   /// <code>xAOD::TrigElectronContainer_v1</code>.
   ///
   /// @author Fabrizio Salvatore <Fabrizio.Salvatore@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 602876 $
   /// $Date: 2014-06-20 13:21:26 +0200 (Fri, 20 Jun 2014) $
   ///
   class TrigElectronAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TrigElectronAuxContainer_v1();

   private:
      /// @name Simple electron properties
      /// @{
      std::vector< uint32_t > roiWord;
      std::vector< float > zvtx;
      std::vector< float > charge;
      std::vector< int >   nTRTHits;
      std::vector< int >   nTRTHiThresholdHits;
      std::vector< float > rcore;
      std::vector< float > eratio;
      std::vector< float > etHad;
      std::vector< float > etHad1;
      std::vector< float > f0;
      std::vector< float > f1;
      std::vector< float > f2;
      std::vector< float > f3;
      /// @}

      /// @name 4-momentum properties
      /// @{
      std::vector< float > trkEtaAtCalo;
      std::vector< float > trkPhiAtCalo;
      std::vector< float > etOverPt;
      std::vector< float > pt;
      std::vector< float > caloEta;
      std::vector< float > caloPhi;
      /// @}

      /// @name Links to the electron constituents
      /// @{
      std::vector< ElementLink< TrigEMClusterContainer > > emClusterLink;
      std::vector< ElementLink< TrackParticleContainer > > trackParticleLink;
      /// @}

   }; // class TrigElectronAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigElectronAuxContainer_v1, 1180743572, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TrigElectronAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // not XAODTRIGEGAMMA_VERSIONS_TRIGELECTRONAUXCONTAINTER_V1_H
