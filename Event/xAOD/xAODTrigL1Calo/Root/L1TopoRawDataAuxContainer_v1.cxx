/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoRawDataAuxContainer_v1.cxx 652824 2015-03-09 23:24:32Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/L1TopoRawDataAuxContainer_v1.h"
        
namespace xAOD {

  L1TopoRawDataAuxContainer_v1::L1TopoRawDataAuxContainer_v1() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( dataWords );
    AUX_VARIABLE( statusWords );
    AUX_VARIABLE( errorWord ); 
    AUX_VARIABLE( sourceID );   
  } 
} // namespace xAOD
