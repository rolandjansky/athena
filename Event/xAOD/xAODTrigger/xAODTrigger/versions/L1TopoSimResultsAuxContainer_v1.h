/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResultsAuxContainer_v1.h 2021-10-06 @asonay $
#ifndef XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTSAUXCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTSAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   // Auxiliary store for the L1Topo decision container
   //
   // @author Anil Sonay <anil.sonay@cern.ch>
   //
   // $Revision: 0 $
   // $Date: 2021-10-06 $
   //
   class L1TopoSimResultsAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constuctor
      L1TopoSimResultsAuxContainer_v1();

   private:
      std::vector< unsigned int > connectionId;
      std::vector< uint32_t > topoWord;
      std::vector< uint32_t > topoWordOverflow;
      std::vector< uint64_t > topoWord64;
      std::vector< uint64_t > topoWord64Overflow;
      std::vector< unsigned int > bitWidth;
      std::vector< unsigned int > clock;

   }; // class L1TopoSimResultsAuxContainer_v1

} // namespace xAOD

// Declare the inheritance of the container:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::L1TopoSimResultsAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTSAUXCONTAINER_V1_H
