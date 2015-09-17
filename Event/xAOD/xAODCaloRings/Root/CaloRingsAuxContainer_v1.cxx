/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsAuxContainer_v1.cxx 659538 2015-04-08 17:29:18Z wsfreund $ 

// Local include(s):
#include "xAODCaloRings/versions/CaloRingsAuxContainer_v1.h"

namespace xAOD {
  
CaloRingsAuxContainer_v1::CaloRingsAuxContainer_v1()
  : AuxContainerBase() {

  ///@name The RingSet ElementLink vector
  AUX_VARIABLE(ringSetELVec);             
 
}
  
} // namespace xAOD
