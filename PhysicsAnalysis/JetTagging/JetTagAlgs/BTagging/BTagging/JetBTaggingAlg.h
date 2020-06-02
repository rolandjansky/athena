/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ReadDecorHandleKeyArray.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"

#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "BTagging/IBTagTool.h"
#include "BTagging/IBTagLightSecVertexing.h"

// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

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
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:
  
    SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
    Gaudi::Property<SG::ReadDecorHandleKeyArray<xAOD::JetContainer>> m_jetParticleLinkNameList{ this, "TrackToJetAssociatorNames", {""}, "Element Link vector from jet to particle container"};
    //SG::ReadHandleKey<xAOD::VertexContainer> m_VertexCollectionName {this, "vxPrimaryCollectionName", "", "Input primary vertex container"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_BTagSVCollectionName {this, "BTagSVCollectionName", "", "Input BTagging secondary vertex container"};
    SG::ReadHandleKey<xAOD::BTagVertexContainer> m_BTagJFVtxCollectionName {this, "BTagJFVtxCollectionName", "", "Input BTagging Jet Fitter container"};
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_jetBTaggingLinkName {this, "BTaggingLinkName", "", "Element link from jet to BTagging container"};
    SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingCollectionName {this, "BTaggingCollectionName", "", "Output BTagging container"};
    SG::WriteDecorHandleKey<xAOD::BTaggingContainer> m_bTagJetDecorLinkName {this, "JetLinkName", "", "Element Link from BTagging to Jet container"};

    std::string m_JetName;

    ToolHandle< IBTagTool > m_bTagTool;
    ToolHandle< IBTagLightSecVertexing > m_bTagSecVtxTool;

};

}

#endif
