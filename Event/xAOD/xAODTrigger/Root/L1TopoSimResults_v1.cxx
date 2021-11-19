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
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, std::string, boardName,
					setName )

  // Raw data words
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, uint32_t, word32,
					setWord32 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, uint64_t, word64,
					setWord64 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, std::bitset<128>, wordOptical,
					setWordOptical )

  // Clock
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( L1TopoSimResults_v1, unsigned int, clock,
					setClock )

}
