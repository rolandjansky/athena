/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsAuxContainer_v1.cxx 707323 2015-11-12 02:45:01Z wsfreund $ 

// Local include(s):
#include "xAODCaloRings/versions/CaloRingsAuxContainer_v1.h"

namespace xAOD {
  
CaloRingsAuxContainer_v1::CaloRingsAuxContainer_v1()
  : AuxContainerBase() {

  ///@name The RingSet ElementLink vector
  AUX_VARIABLE(ringSetLinks);             
 
}
  
} // namespace xAOD
