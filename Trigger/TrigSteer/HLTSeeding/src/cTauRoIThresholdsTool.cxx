/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "cTauRoIThresholdsTool.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/exceptions.h"
#include "GaudiKernel/ThreadLocalContext.h"

StatusCode cTauRoIThresholdsTool::initialize() {
  ATH_CHECK(RoIThresholdsTool::initialize());
  ATH_CHECK(m_jTauLinkKey.initialize());
  return StatusCode::SUCCESS;
}

uint64_t cTauRoIThresholdsTool::getPattern(const xAOD::eFexTauRoI& eTau,
                                           const RoIThresholdsTool::ThrVec& /*menuThresholds*/,
                                           const TrigConf::L1ThrExtraInfoBase& /*menuExtraInfo*/) const {
  // Get the jTau matched to the eTau
  using jTauLink_t = ElementLink<xAOD::jFexTauRoIContainer>;
  SG::ReadDecorHandle<xAOD::eFexTauRoIContainer, jTauLink_t> jTauLinkAcc{m_jTauLinkKey, Gaudi::Hive::currentContext()};
  if (not jTauLinkAcc.isPresent()) {
    ATH_MSG_ERROR("Decoration " << m_jTauLinkKey.key() << " is missing, cannot create cTau threshold pattern");
    throw SG::ExcNullReadHandle(m_jTauLinkKey.clid(), m_jTauLinkKey.key(), m_jTauLinkKey.storeHandle().name());
  }
  jTauLink_t jTauLink = jTauLinkAcc(eTau);
  const xAOD::jFexTauRoI* jTau = *jTauLink;

  // Sanity check print-out
  ATH_MSG_DEBUG("eFex eta,phi = " << eTau.iEta() << ", " << eTau.iPhi()
                << ", jFex eta,phi = " << jTau->globalEta() << ", " << jTau->globalPhi());

  // GET SOME VARIABLES JUST AS AN EXAMPLE
  unsigned int eFexEt = eTau.etTOB();
  unsigned int jFexIso = jTau->iso();
  ATH_MSG_DEBUG("eFex et = " << eFexEt << ", jFex iso = " << jFexIso);

  // THIS IS CURRENTLY ONLY A PLACEHOLDER
  return 0xffffffffffffffff;
}
