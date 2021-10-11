/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/DebugComboHypoTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include <algorithm>
#include <cmath>

using namespace TrigCompositeUtils;

DebugComboHypoTool::DebugComboHypoTool(const std::string& type,
					       const std::string& name,
					       const IInterface* parent)
  : ComboHypoToolBase(type, name, parent)
{ }


StatusCode DebugComboHypoTool::initialize() {
  return StatusCode::SUCCESS;
}


bool DebugComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>& combination) const {
  ATH_MSG_ALWAYS("Next Combination:");
  using namespace TrigCompositeUtils;
  // Passes if *at least one* physics object with pT > 45 MeV is supplied on *each* leg.
  // NOTE: Chains with only one leg have legMultiplicity().size() == 0
  std::vector<uint8_t> legPasses(std::max((int) legMultiplicity().size(), 1), 0);
  for (const Combo::LegDecision& ld : combination) {
    DecisionID id = ld.first; // Specifies which leg this physics object is being supplied on for this combination
    ElementLink<DecisionContainer> d = ld.second; // The physics object's navigation Decision Object.
    ElementLink<xAOD::IParticleContainer> f = (*d)->objectLink<xAOD::IParticleContainer>(featureString()); // The actual physics object itself.
    ATH_MSG_ALWAYS("-- " << HLT::Identifier(id) << " f:" << f.dataID() << " i:" << f.index() << " pT:" << (*f)->pt());
    if ((*f)->pt() > 45.0) {
      const size_t index = (legMultiplicity().size() > 0 ? getIndexFromLeg(id) : 0);
      legPasses.at( index ) = 1;
    }
  }
  for (const uint8_t pass : legPasses) {
    if (!pass) {
      return false;
    }
  }
  return true;
}
