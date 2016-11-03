/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTILECORRECTION_JETTILECORRECTIONALG_H
#define JETTILECORRECTION_JETTILECORRECTIONALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "AsgTools/ToolHandle.h"

//#include "JetTileCorrection/IJetTileCorrectionTool.h"
#include "JetCPInterfaces/IJetTileCorrectionTool.h"

namespace CP{

  class JetTileCorrectionAlg: public ::AthAnalysisAlgorithm { 
  public: 
    JetTileCorrectionAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~JetTileCorrectionAlg(); 
    
    virtual StatusCode  initialize();
    virtual StatusCode  firstExecute();
    virtual StatusCode  execute();
    virtual StatusCode  finalize();
    
  private: 

    std::string m_jetKey;
    
    ToolHandle< IJetTileCorrectionTool > m_jtcTool;
    
  }; 
}

#endif //> !JETTILECORRECTION_JETTILECORRECTIONALG_H
