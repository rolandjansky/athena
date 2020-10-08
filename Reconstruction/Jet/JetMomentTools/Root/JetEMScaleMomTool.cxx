/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <functional>
#include "JetMomentTools/JetEMScaleMomTool.h"

#include "AsgDataHandles/WriteDecorHandle.h"
#include "xAODJet/JetAccessorMap.h"

struct p4Decorator {
  
  SG::WriteDecorHandle<xAOD::JetContainer, float > ptHandle;
  SG::WriteDecorHandle<xAOD::JetContainer, float > etaHandle;
  SG::WriteDecorHandle<xAOD::JetContainer, float > phiHandle;
  SG::WriteDecorHandle<xAOD::JetContainer, float > massHandle;

  p4Decorator(const SG::WriteDecorHandleKey<xAOD::JetContainer>& ptKey,
	      const SG::WriteDecorHandleKey<xAOD::JetContainer>& etaKey,
	      const SG::WriteDecorHandleKey<xAOD::JetContainer>& phiKey,
	      const SG::WriteDecorHandleKey<xAOD::JetContainer>& massKey) :
    ptHandle(ptKey),
    etaHandle(etaKey),
    phiHandle(phiKey),
    massHandle(massKey)
 {}

  void decorateP4(const xAOD::Jet& jet, const xAOD::JetFourMom_t& p4) {
    ptHandle(jet) = p4.Pt();
    etaHandle(jet) = p4.Eta();
    phiHandle(jet) = p4.Phi();
    massHandle(jet) = p4.M();
  }

};

xAOD::JetFourMom_t getEMScaleP4(xAOD::JetConstituentVector& constits) {

  xAOD::JetFourMom_t emscaleSum;
  // just sum 4-vectors:
  for (auto iconstit=constits.begin(xAOD::UncalibratedJetConstituent); iconstit!=constits.end(xAOD::UncalibratedJetConstituent); ++iconstit) {
    emscaleSum += **iconstit;
  }
  return emscaleSum;
}

//**********************************************************************

JetEMScaleMomTool::JetEMScaleMomTool(const std::string& name)
: AsgTool(name) { }

//**********************************************************************

StatusCode JetEMScaleMomTool::initialize() {
  ATH_MSG_INFO("Initializing JetEMScaleMomTool " << name());
  
  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetEMScaleMomTool needs to have its input jet container configured!");
    return StatusCode::FAILURE;
  }

  std::vector<std::reference_wrapper<SG::WriteDecorHandleKey<xAOD::JetContainer> > > keys = {m_emscalePtKey, m_emscaleEtaKey, m_emscalePhiKey, m_emscaleMassKey};
  for(SG::WriteDecorHandleKey<xAOD::JetContainer>& key : keys) {
    key = m_jetContainerName + "." + key.key();
    ATH_CHECK( key.initialize() );
  }
  
  ATH_MSG_DEBUG("Operating on jets with " << (m_useUncalibConstits ? "" : "un") << "calibrated constituents." );

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetEMScaleMomTool::decorate(const xAOD::JetContainer& jets) const {
  ATH_MSG_VERBOSE("Begin decorating jets.");
  p4Decorator emscale_decor(m_emscalePtKey, m_emscaleEtaKey, m_emscalePhiKey, m_emscaleMassKey);

  for(const xAOD::Jet* jet : jets) {
    if(m_useUncalibConstits) {
      // Need to loop and sum the constituents
      xAOD::JetConstituentVector constits = jet->getConstituents();
      if(! constits.isValid() ) {
	ATH_MSG_WARNING("Jet constituent vector is invalid. Can't set EM scale momentum");
	return StatusCode::FAILURE;
      }
      xAOD::JetFourMom_t em_p4 = getEMScaleP4(constits);
      emscale_decor.decorateP4(*jet,em_p4);
    } else {
      // Simply add the constituent-level p4
      xAOD::JetFourMom_t constit_p4 = jet->jetP4(xAOD::JetConstitScaleMomentum);
      emscale_decor.decorateP4(*jet,constit_p4);
    }

  }

  return StatusCode::SUCCESS;
}


//**********************************************************************
