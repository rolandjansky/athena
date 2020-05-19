/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef VR_JET_OVERLAP_DECORATOR_H
#define VR_JET_OVERLAP_DECORATOR_H

#include "xAODJet/JetContainer.h"

enum class VRJetParameters {EDM,RHO30MIN02MAX4};

struct VRJetOverlapConfig {
  VRJetOverlapConfig(VRJetParameters = VRJetParameters::EDM);
  std::string relative_name;
  std::string absolute_name;
  VRJetParameters jet_parameters_source;
};

class VRJetOverlapDecorator
{
public:
  VRJetOverlapDecorator(const VRJetOverlapConfig& = VRJetOverlapConfig());
  void decorate(const xAOD::JetContainer& jets) const;
private:
  VRJetParameters m_param_source;
  SG::AuxElement::Decorator<float> m_rel_decorator;
  SG::AuxElement::Decorator<float> m_abs_decorator;
  SG::AuxElement::ConstAccessor<float> m_min_radius;
  SG::AuxElement::ConstAccessor<float> m_max_radius;
  SG::AuxElement::ConstAccessor<float> m_mass_scale;
};

#endif
