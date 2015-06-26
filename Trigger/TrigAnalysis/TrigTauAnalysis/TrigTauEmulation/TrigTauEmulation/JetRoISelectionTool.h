/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETROISELECTIONTOOL_H
#define JETROISELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"
/* #include "PATCore/IAsgSelectionTool.h" */


// Local includes
#include "TrigTauEmulation/IJetRoISelectionTool.h"

class JetRoISelectionTool : public virtual IJetRoISelectionTool, public asg::AsgTool

{

  ASG_TOOL_CLASS(JetRoISelectionTool, IJetRoISelectionTool)


 public:

  // Default Constructor 
  JetRoISelectionTool(const std::string& name);

  // Copy Constructor 
  JetRoISelectionTool(const JetRoISelectionTool& other);

  // Destructor
  virtual ~JetRoISelectionTool() {};

  // Tool initialization
  virtual StatusCode initialize();

  // Get the decision for a specific JetRoI
  virtual const Root::TAccept& accept(const xAOD::JetRoI& l1jet) const;


 private:

  mutable Root::TAccept m_accept;
  
  double m_roi_pt;
  double m_roi_eta;



};
#endif
