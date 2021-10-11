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
#include "AsgTools/PropertyWrapper.h"
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
  
  Gaudi::Property<int>                       m_constitScale{this, "ConstitScale", 0, "Cluster signal state to use for the jet constituents"};
  Gaudi::Property<std::vector<std::string> > m_jetScaleNames{this, "JetScaleNames", {}, "The names of the jet scale"};
  Gaudi::Property<std::vector<std::string> > m_altColls{this, "AltConstitColls", {}, "Alternate collections to use (e.g. origin-corrected or not)"};
  Gaudi::Property<std::vector<int> >         m_altConstitScales{this, "AltConstitScales", {}, "Scales to use for the alternate constituent collections"};
  Gaudi::Property<std::vector<std::string> > m_altJetScales{this, "AltJetScales", {}, "Existing momentum scales that should just be copied"};

  std::vector<bool> m_isDetectorEtaPhi;

  SG::ReadHandleKeyArray<xAOD::CaloClusterContainer> m_altColls_keys{this, "AltConstitCollsKeys", {}, "Do not configure this manually"};

};

#endif
