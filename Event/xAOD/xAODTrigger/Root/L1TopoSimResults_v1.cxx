/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/L1TopoSimResults_v1.h"

namespace xAOD {

  L1TopoSimResults_v1::L1TopoSimResults_v1() 
      : SG::AuxElement() {
  }

  
  // Name
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, unsigned int, connectionId,
					setConnectionId )

  // Raw data words
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, uint32_t, topoWord,
					setTopoWord )
  
  // Raw overflow words
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, uint32_t, topoWordOverflow,
					setTopoWordOverflow )

  // Raw data words
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, uint64_t, topoWord64,
					setTopoWord64 )
  
  // Raw overflow words
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, uint64_t, topoWord64Overflow,
					setTopoWord64Overflow )
  
  // Bit-length
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, unsigned int, bitWidth,
					setBitWidth )

  // Clock
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, unsigned int, clock,
					setClock )

}
