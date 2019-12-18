/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_JETBTAGGINGALG_HH
#define BTAGGING_JETBTAGGINGALG_HH
///////////////////////////////////////////
///
/// \class JetBTaggingAlg
/// Algorithm to run and add btagging information.
////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "BTagging/IBTagTool.h"
#include "BTagging/IBTagLightSecVertexing.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

namespace Analysis{
class IJetFitterVariablesFactory;

class  JetBTaggingAlg: 
  public AthAlgorithm
   { 
  public:
  
    /** Constructors and destructors */
    JetBTaggingAlg(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~JetBTaggingAlg();
    
    /** Main routines specific to an ATHENA algorithm */
    virtual StatusCode initialize();
    virtual StatusCode execute();

  private:
  
    SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
    Gaudi::Property<SG::ReadDecorHandleKey<xAOD::JetContainer>> m_jetParticleLinkName{ this, "TrackToJetAssociatorName", "", "Element Link vector form jet to particle container"};
    //SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Input primary vertex container"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_BTagSVCollectionName {this, "BTagSVCollectionName", "", "Input BTagging secondary vertex container"};
    SG::ReadHandleKey<xAOD::BTagVertexContainer> m_BTagJFVtxCollectionName {this, "BTagJFVtxCollectionName", "", "Input BTagging Jet Fitter container"};
    Gaudi::Property<SG::WriteDecorHandleKey<xAOD::JetContainer> >m_jetBTaggingLinkName{this,"JetContainerName","","Element link form jet to BTagging container"};
    SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingCollectionName {this, "BTaggingCollectionName", "", "Output BTagging container"};

    std::string m_JetName;
    std::string m_BTagLink;
    std::vector<std::string> m_TrackToJetAssocNameList;

    ToolHandle< IBTagTool > m_bTagTool;
    ToolHandle< IBTagLightSecVertexing > m_bTagSecVtxTool;
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

};

}

#endif
