// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMOMENTTOOLS_JETCALOENERGIES_H
#define JETMOMENTTOOLS_JETCALOENERGIES_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandle.h"
#include "xAODJet/JetContainer.h"
#include <vector>

class JetCaloEnergies : public asg::AsgTool,
                        virtual public IJetDecorator {
  ASG_TOOL_CLASS0(JetCaloEnergies)
public:
  
  JetCaloEnergies(const std::string & t);

  virtual StatusCode initialize() override;
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

protected:
  void fillEperSamplingCluster(const xAOD::Jet &jet, std::vector<float> & ePerSampling ) const ;
  void fillEperSamplingPFO(const xAOD::Jet &jet, std::vector<float> & ePerSampling ) const ;

private:
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_ePerSamplingKey{this, "EPerSamplingName", "EnergyPerSampling", "SG key for the EnergyPerSampling attribute"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_emFracKey{this, "EMFracName", "EMFrac", "SG key for the EMFrac attribute"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_hecFracKey{this, "HECFracName", "HECFrac", "SG key for the HECFrac attribute"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_psFracKey{this, "PSFracName", "PSFrac", "SG key for the PSFrac attribute"};
  
};


#undef ASG_DERIVED_TOOL_CLASS
#endif

