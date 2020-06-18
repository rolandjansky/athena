// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuAuxContainer_v1.h 636406 2014-12-17 05:19:48Z lheinric $
#ifndef XAODTRIGGER_VERSIONS_MENUJSONAUXCONTAINER_v1_H
#define XAODTRIGGER_VERSIONS_MENUJSONAUXCONTAINER_v1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

//class xAODTriggerMenuAuxContainerCnv;

namespace xAOD {

   /// Auxiliary store object for TriggerMenuJsonContainer_v1
   ///
   /// Holder for a JSON file.
   ///
   class TriggerMenuJsonAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TriggerMenuJsonAuxContainer_v1();

   private:
      //friend class ::xAODTriggerMenuAuxContainerCnv;

      /// @name Configuration identifier variables
      /// @{
      std::vector< uint32_t > key;
      std::vector< std::string > name;
      /// @}

      /// @name Stored JSON data
      /// @{
      std::vector< std::string > payload;
      /// @}

   }; // class TriggerMenuJsonAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h" 
SG_BASE( xAOD::TriggerMenuJsonAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODTRIGGER_VERSIONS_MENUJSONAUXCONTAINER_v1_H
