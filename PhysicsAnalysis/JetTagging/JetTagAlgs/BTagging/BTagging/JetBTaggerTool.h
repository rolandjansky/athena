/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "MagFieldInterfaces/IMagFieldSvc.h"

namespace Analysis{

class IBTagTool;
class IBTagTrackAssociation;
class IBTagSecVertexing;
class IBTagJetPtScaling;

class  JetBTaggerTool: 
  virtual public asg::AsgTool, 
  virtual public IJetBTaggerTool,
  virtual public IJetModifier {
  ASG_TOOL_CLASS2(JetBTaggerTool, IJetBTaggerTool, IJetModifier)
  public:
  
  virtual StatusCode initialize();

  JetBTaggerTool(const std::string & n);
  //JetBTaggerTool(const std::string&, const IInterface*, const std::string&);
  virtual ~JetBTaggerTool();
  virtual int modify(xAOD::JetContainer& jets) const;


 private:
  
  std::string m_BTagName;
  std::string m_BTagSVName; 
  std::string m_BTagJFVtxName; 

  // FIXME: mutable
  mutable ToolHandle< IBTagTool > m_bTagTool; 
  ToolHandle< IBTagTrackAssociation > m_BTagTrackAssocTool;
  ToolHandle< IBTagSecVertexing > m_bTagSecVtxTool;
  ToolHandle< IBTagJetPtScaling> m_PtRescalingTool;
  bool m_augment;
  bool m_PtRescale;
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  int btag_container(xAOD::JetContainer& jets, xAOD::BTaggingContainer *, bool) const;
  // Utility functions to be used in case of container overwriting.
  // Note that they do not need to be defined here, as they can only be called from code in JetBTaggerTool.cxx.
  template< class CONTAINER, class AUXSTORE >
    StatusCode overwrite(const std::string& key, bool doCopy = true) const;
  template< class CONTAINER, class AUXSTORE >
    StatusCode overwriteImp(const std::string& key, bool doCopy = true) const;
};

}

#endif
