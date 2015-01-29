/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementAuxContainer_v2.cxx 642659 2015-01-29 12:41:06Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v2.h"
        
namespace xAOD {

  JetElementAuxContainer_v2::JetElementAuxContainer_v2() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( phi );
    AUX_VARIABLE( eta );
    AUX_VARIABLE( key );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( emEnergyVec );
    AUX_VARIABLE( hadEnergyVec );
    AUX_VARIABLE( emErrorVec );
    AUX_VARIABLE( hadErrorVec );
    AUX_VARIABLE( linkErrorVec );      
  }
  
} // namespace xAOD
