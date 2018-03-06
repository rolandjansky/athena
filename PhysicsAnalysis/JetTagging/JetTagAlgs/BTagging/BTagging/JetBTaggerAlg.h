/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_JETBTAGGERALG_HH
#define BTAGGING_JETBTAGGERALG_HH
///////////////////////////////////////////
///
/// \class JetTaggerAlg
/// StandAlone tool to run and add btagging information.
////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "BTagging/IBTagTool.h"
#include "BTagging/IBTagTrackAssociation.h"
#include "BTagging/IBTagSecVertexing.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

namespace Analysis{

//class IBTagTool;
//class IBTagTrackAssociation;
//class IBTagSecVertexing;

class  JetBTaggerAlg: 
  public AthAlgorithm
   { 
  public:
  
    /** Constructors and destructors */
    JetBTaggerAlg(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~JetBTaggerAlg();
    
    /** Main routines specific to an ATHENA algorithm */
    virtual StatusCode initialize();
    virtual StatusCode execute();

  private:
  
    SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::JetContainer> >m_jetBTaggingLinkName{this,"JetContainerName","","Element link form jet to BTagging container"};
    SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingCollectionName {this, "BTaggingCollectionName", "", "Output BTagging container"};

    ToolHandle< IBTagTool > m_bTagTool;
    ToolHandle< IBTagTrackAssociation > m_BTagTrackAssocTool;
    ToolHandle< IBTagSecVertexing > m_bTagSecVtxTool;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

};

}

#endif
