/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingSetAuxContainer_v1.cxx 659538 2015-04-08 17:29:18Z wsfreund $ 
// Local include(s):
#include "xAODCaloRings/versions/RingSetAuxContainer_v1.h"

namespace xAOD {
  
RingSetAuxContainer_v1::RingSetAuxContainer_v1()
  : AuxContainerBase() {

  ///@name The rings energy representation for this RingSet
  AUX_VARIABLE(ringsE);             
 
}
  
} // namespace xAOD
