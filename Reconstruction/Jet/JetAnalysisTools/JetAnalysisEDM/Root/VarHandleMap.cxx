/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetAnalysisEDM/AttributeMap.h"
#include "JetAnalysisEDM/Particle.h"
#include "JetAnalysisEDM/VarHandleMap.icc"


namespace JetAnalysisEDM {

  string_t VarHandleMapBase::prefix(){return m_attributeMap->prefix();}
  const ::Long64_t* VarHandleMapBase::master(){return m_attributeMap->master();}
  TTree* VarHandleMapBase::inputTree(){return m_attributeMap->inputTree();}
  
  void VarHandleMapBase::forwardError(const char* f, const char* m){
    m_attributeMap->forwardError( f, m);
  }



}
