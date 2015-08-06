/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementAuxContainer_v2.cxx 679812 2015-07-02 14:57:02Z morrisj $

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v2.h"
        
namespace xAOD {

  JetElementAuxContainer_v2::JetElementAuxContainer_v2() :
    AuxContainerBase() 
  {
    AUX_VARIABLE( eta );
    AUX_VARIABLE( phi );    
    AUX_VARIABLE( key );
    AUX_VARIABLE( peak );
    AUX_VARIABLE( emJetElementETVec );
    AUX_VARIABLE( hadJetElementETVec );
    AUX_VARIABLE( emJetElementErrorVec );
    AUX_VARIABLE( hadJetElementErrorVec );
    AUX_VARIABLE( linkErrorVec );      
  }
  
} // namespace xAOD
