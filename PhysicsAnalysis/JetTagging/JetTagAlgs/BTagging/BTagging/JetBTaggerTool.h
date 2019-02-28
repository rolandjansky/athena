/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_JETBTAGGERTOOL_HH
#define BTAGGING_JETBTAGGERTOOL_HH
///////////////////////////////////////////
///
/// \class JetBTaggerTool
/// jet modfier tool to run and add btagging information.
////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "BTagging/IJetBTaggerTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

namespace Analysis{

class IBTagTool;
class IBTagTrackAssociation;
class IBTagSecVertexing;

class  JetBTaggerTool: 
  virtual public asg::AsgTool, 
  virtual public IJetBTaggerTool,
  virtual public IJetModifier {
  ASG_TOOL_CLASS2(JetBTaggerTool, IJetBTaggerTool, IJetModifier)
  public:
  
  virtual StatusCode initialize();
  StatusCode execute();

  JetBTaggerTool(const std::string & n);
  virtual ~JetBTaggerTool();
  virtual int modify(xAOD::JetContainer& jets) const;


 private:

  SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName { this, "JetCollectionName", "", "Input jet container"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::JetContainer> >m_jetBTaggingLinkName{this,"JetContainerName","","Element link form jet to BTagging container"};
  SG::WriteHandleKey<xAOD::BTaggingContainer> m_BTaggingCollectionName { this, "BTaggingCollectionName", "", "Output BTagging container"} ;

  ToolHandle< IBTagTool > m_bTagTool; 
  ToolHandle< IBTagTrackAssociation > m_BTagTrackAssocTool;
  ToolHandle< IBTagSecVertexing > m_bTagSecVtxTool;
  bool m_augment;
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

};

}

#endif
