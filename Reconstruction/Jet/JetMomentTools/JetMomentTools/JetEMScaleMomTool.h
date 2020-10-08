// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMOMENTTOOLS_JETEMSCALEMOMTOOL_H
#define JETMOMENTTOOLS_JETEMSCALEMOMTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "xAODJet/JetContainer.h"
#include <vector>

class JetEMScaleMomTool : public asg::AsgTool,
                        virtual public IJetDecorator {
  ASG_TOOL_CLASS0(JetEMScaleMomTool)
public:
  
  JetEMScaleMomTool(const std::string & t);

  virtual StatusCode initialize() override;
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

private:
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};
  // Should be false except if running on topoclusters except for the (legacy/cosmics?) case where
  // jets were built from the CaloCalTopoClusters at EM scale
  Gaudi::Property<bool> m_useUncalibConstits{this, "UseUncalibConstits", "", "Toggle for extracting the EMScale momentum from uncalibrated topoclusters"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_emscalePtKey{this,   "EMScalePtName",   "JetEMScaleMomentum_pt",  "SG key for the EMScale pt attribute"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_emscaleEtaKey{this,  "EMScaleEtaName",  "JetEMScaleMomentum_eta", "SG key for the EMScale eta attribute"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_emscalePhiKey{this,  "EMScalePhiName",  "JetEMScaleMomentum_phi", "SG key for the EMScale phi attribute"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_emscaleMassKey{this, "EMScaleMassName", "JetEMScaleMomentum_m",   "SG key for the EMScale mass attribute"};

};


#undef ASG_DERIVED_TOOL_CLASS
#endif

