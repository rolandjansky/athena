/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_STANDALONEJETBTAGGERTOOL_HH
#define BTAGGING_STANDALONEJETBTAGGERTOOL_HH
///////////////////////////////////////////
///
/// \class StanAloneJetBTaggerAlg
/// StandAlone tool to run and add btagging information.
////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


namespace Analysis{

class IJetBTaggerTool;

class  StandAloneJetBTaggerAlg: 
  public AthAlgorithm
   { 
  public:
  
    /** Constructors and destructors */
    StandAloneJetBTaggerAlg(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~StandAloneJetBTaggerAlg();
    
    /** Main routines specific to an ATHENA algorithm */
    virtual StatusCode initialize();
    virtual StatusCode execute();

  private:
  
    std::string m_JetCollectionName;

    ToolHandle< IJetBTaggerTool > m_JetBTaggerTool; 
};

}

#endif
