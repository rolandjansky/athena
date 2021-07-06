/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetFilterTool.h

#ifndef JETREC_JETFILTER_H
#define JETREC_JETFILTER_H

// Pierre-Antoine Delsart
// January 2014
//
// ASG tool to select jets.

#include <string>
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"

class JetFilterTool
: public asg::AsgTool,
  virtual public IJetModifier {

ASG_TOOL_CLASS(JetFilterTool, IJetModifier)

public:
    
  JetFilterTool(const std::string& t);
  
  StatusCode modify(xAOD::JetContainer& jets) const;

protected:

  double m_ptMin; // pT threshold [MeV]
    
};

#endif
