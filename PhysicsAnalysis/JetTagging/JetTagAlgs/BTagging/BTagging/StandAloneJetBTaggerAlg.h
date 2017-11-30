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
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

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
  
    SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", ""};
    SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingCollectionName {this, "BTaggingCollectionName", "", ""};
    std::string m_suffix;

    ToolHandle< IJetBTaggerTool > m_JetBTaggerTool; 

};

}

#endif
