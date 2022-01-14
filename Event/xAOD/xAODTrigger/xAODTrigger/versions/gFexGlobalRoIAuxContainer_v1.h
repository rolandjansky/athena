// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_GFEXGLOBALROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_GFEXGLOBALROIAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary store for the LVL1 GFEX TOB container
   ///
   /// This auxiliary container can describe the properties of a container
   /// of LVL1 GLOBAL TOB.

   class gFexGlobalRoIAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constuctor
      gFexGlobalRoIAuxContainer_v1();

   private:
      std::vector< uint32_t > word;
      std::vector< uint8_t >  saturated;
      std::vector< int > globalType;
      std::vector< uint8_t >  statusOne;
      std::vector< uint8_t >  statusTwo;
      std::vector< uint16_t >  quantityOne;
      std::vector< uint16_t >  quantityTwo;

   }; // class gFexGlobalRoIAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::gFexGlobalRoIAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_gFexGlobalRoIAuxContainer_V1_H
