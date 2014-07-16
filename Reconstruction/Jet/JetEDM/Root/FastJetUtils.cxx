/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEDM/FastJetUtils.h"

namespace xAOD {
  
  namespace JetAlgorithmType {

    const std::string& algName(fastjet::JetAlgorithm a){
      return algName( algId(a) );
    }
    
    ID algId(fastjet::JetAlgorithm a){
      // for now there is a 1-to-1 relation
      return (ID) a ;
    }

    fastjet::JetAlgorithm fastJetDef(ID id){
      // for now there is a 1-to-1 relation
      return (fastjet::JetAlgorithm) id;
    }
    fastjet::JetAlgorithm fastJetDef(const std::string& name){
      return fastJetDef( algId( name) );
    }


  }
}
