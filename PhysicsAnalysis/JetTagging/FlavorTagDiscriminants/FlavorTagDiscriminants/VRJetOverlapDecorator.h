#ifndef VR_JET_OVERLAP_DECORATOR_H
#define VR_JET_OVERLAP_DECORATOR_H

#include "xAODJet/JetContainer.h"

class VRJetOverlapDecorator
{
public:
  VRJetOverlapDecorator(const std::string& name = "relativeDeltaRToVRJet");
  void decorate(const xAOD::JetContainer& jets) const;
private:
  SG::AuxElement::Decorator<float> m_decorator;
};

#endif
