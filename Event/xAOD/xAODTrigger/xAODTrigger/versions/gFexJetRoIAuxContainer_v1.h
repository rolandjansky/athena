// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_GFEXJETROIAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_GFEXJETROIAUXCONTAINER_V1_H

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
   /// of LVL1 JET TOB.

   class gFexJetRoIAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constuctor
      gFexJetRoIAuxContainer_v1();

   private:
      std::vector< uint32_t > word;
      std::vector< int > gFexType;
      std::vector< uint16_t > tobEt;
      std::vector< uint8_t >  iEta;
      std::vector< uint8_t >  iPhi;
      std::vector< uint8_t >  gFexJetStatus;
      std::vector< uint8_t >  saturated;

   }; // class gFexJetRoIAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::gFexJetRoIAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_gFexJetRoIAuxContainer_V1_H
