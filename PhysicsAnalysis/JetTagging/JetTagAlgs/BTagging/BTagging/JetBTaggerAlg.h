/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// For magneticfield
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

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
    SG::WriteDecorHandleKey<xAOD::JetContainer> m_jetBTaggingLinkName{this, "BTaggingLinkName","","Element link form jet to BTagging container"};
    SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingCollectionName {this, "BTaggingCollectionName", "", "Output BTagging container"};
    SG::WriteDecorHandleKey<xAOD::BTaggingContainer> m_bTagJetDecorLinkName {this, "JetLinkName", "", "Element Link from BTagging to Jet container"};

    std::string m_JetName;
    std::string m_BTagLink;

    ToolHandle< IBTagTool > m_bTagTool;
    ToolHandle< IBTagTrackAssociation > m_BTagTrackAssocTool;
    ToolHandle< IBTagSecVertexing > m_bTagSecVtxTool;
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

};

}

#endif
