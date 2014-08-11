// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuAuxContainer_v1.h 582798 2014-02-12 15:53:47Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGGERMENUAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGGERMENUAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store object for TriggerMenuContainer_v1
   ///
   /// This is a fully defined auxiliary container for TriggerMenuContainer_v1.
   /// Meant to provide efficient I/O for reading/writing the full trigger
   /// configuration.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 582798 $
   /// $Date: 2014-02-12 16:53:47 +0100 (Wed, 12 Feb 2014) $
   ///
   class TriggerMenuAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TriggerMenuAuxContainer_v1();

   private:
      /// @name Configuration identifier variables
      /// @{
      std::vector< uint32_t > smk;
      std::vector< uint32_t > l1psk;
      std::vector< uint32_t > hltpsk;
      /// @}

      /// @name LVL1 configuration variables
      /// @{
      std::vector< std::vector< uint16_t > > itemCtpIds;
      std::vector< std::vector< std::string > > itemNames;
      std::vector< std::vector< float > > itemPrescales;
      /// @}

      /// @name HLT configuration variables
      /// @{
      std::vector< std::vector< uint16_t > > chainIds;
      std::vector< std::vector< std::string > > chainNames;
      std::vector< std::vector< std::string > > chainParentNames;
      std::vector< std::vector< float > > chainPrescales;
      std::vector< std::vector< float > > chainRerunPrescales;
      std::vector< std::vector< float > > chainPassthroughPrescales;
      /// @}

      /// @name Bunch group variables
      /// @{
      std::vector< std::vector< std::vector< uint16_t > > > bunchGroupBunches;
      /// @}

   }; // class TriggerMenuAuxContainer_v1

} // namespace xAOD

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuAuxContainer_v1, 1212409402, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::TriggerMenuAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_TRIGGERMENUAUXCONTAINER_V1_H
