// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuAuxContainer_v1.h 636406 2014-12-17 05:19:48Z lheinric $
#ifndef XAODTRIGGER_VERSIONS_TRIGGERMENUAUXCONTAINER_v1_H
#define XAODTRIGGER_VERSIONS_TRIGGERMENUAUXCONTAINER_v1_H

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
   /// $Revision: 636406 $
   /// $Date: 2014-12-17 06:19:48 +0100 (Wed, 17 Dec 2014) $
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
      std::vector< std::vector< std::vector< uint32_t > > > chainSignatureCounters;
      std::vector< std::vector< std::vector< int > > > chainSignatureLogics;
      std::vector< std::vector< std::vector< std::vector< std::string > > > > chainSignatureOutputTEs;
      std::vector< std::vector< std::vector< std::string > > > chainSignatureLabels;
      /// @}

      /// @name HLT sequence info
      /// @{
      std::vector< std::vector< std::vector< std::string > > > sequenceInputTEs;
      std::vector< std::vector< std::string > > sequenceOutputTEs;
      std::vector< std::vector< std::vector< std::string > > > sequenceAlgorithms;
      /// @}


      /// @name Bunch group variables
      /// @{
      std::vector< std::vector< std::vector< uint16_t > > > bunchGroupBunches;
      /// @}

   }; // class TriggerMenuAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container: 
#include "xAODCore/BaseInfo.h" 
SG_BASE( xAOD::TriggerMenuAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODTRIGGER_VERSIONS_TRIGGERMENUAUXCONTAINER_v1_H
