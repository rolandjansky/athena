/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstitFourMomTool.h

#ifndef JetMomentTools_JetConstitFourMomTool_H
#define JetMomentTools_JetConstitFourMomTool_H

/// Russell Smith
/// May 2015
///
/// Tool to attach the LC constituent level 4-vector to EM Jets

#include "AsgDataHandles/ReadHandleKeyArray.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include <vector>

class JetConstitFourMomTool : public asg::AsgTool,
                              public IJetModifier {

  ASG_TOOL_CLASS(JetConstitFourMomTool, IJetModifier)

 public:

  // Constructor from tool name.
  JetConstitFourMomTool(std::string myname);

  StatusCode initialize();

  StatusCode modify(xAOD::JetContainer& jets) const;

 private:
  
  int m_constitScale;
  std::vector<std::string> m_jetScaleNames;
  std::vector<std::string> m_altColls;
  std::vector<int>         m_altConstitScales;
  std::vector<std::string> m_altJetScales;

  std::vector<bool> m_isDetectorEtaPhi;

  SG::ReadHandleKeyArray<xAOD::CaloClusterContainer> m_altColls_keys;

};

#endif
