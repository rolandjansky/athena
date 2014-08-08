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

#include "GaudiKernel/ToolHandle.h"


namespace Analysis{

class IBTagTool;
class IBTagTrackAssociation;
class IBTagSecVertexing;

class  JetBTaggerTool: 
  virtual public asg::AsgTool, 
  virtual public IJetModifier {
  ASG_TOOL_CLASS(JetBTaggerTool, IJetModifier)
  public:
  
  virtual StatusCode initialize();

  JetBTaggerTool(const std::string & n);
  virtual ~JetBTaggerTool();
  virtual int modify(xAOD::JetContainer& jets) const;


 private:
  

  std::string m_BTagName;
  std::string m_BTagSVName; 
  std::string m_BTagJFVtxName; 

  ToolHandle< IBTagTool > m_bTagTool; 
  ToolHandle< IBTagTrackAssociation > m_BTagTrackAssocTool;
  ToolHandle< IBTagSecVertexing > m_bTagSecVtxTool;
  
};

}

#endif
