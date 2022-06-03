/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "cTauRoIThresholdsTool.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/exceptions.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "L1TopoAlgorithms/cTauMultiplicity.h"


namespace 
{
  bool isocut(TrigConf::Selection::WP WP, const unsigned int bit) {
    // ctauWp will take values 0 (None)/ 1 (Loose)/ 2 (Medium)/ 3 (Tight)
    unsigned int value = 0;
    if ( WP == TrigConf::Selection::WP::NONE ) {value = 0;}
    else if ( WP == TrigConf::Selection::WP::LOOSE ) {value = 1;}
    else if ( WP == TrigConf::Selection::WP::MEDIUM ) {value = 2;}
    else if ( WP == TrigConf::Selection::WP::TIGHT ) {value = 3;}
    // TODO Add a printout in case the WP is not found                                                                                                                          
    if (bit >= value) {return true;}
    else {return false;}
  };
}

StatusCode cTauRoIThresholdsTool::initialize() {
  ATH_CHECK(RoIThresholdsTool::initialize());
  ATH_CHECK(m_jTauLinkKey.initialize());
  return StatusCode::SUCCESS;
}

uint64_t cTauRoIThresholdsTool::getPattern(const xAOD::eFexTauRoI& eTau,
                                           const RoIThresholdsTool::ThrVec& menuThresholds,
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

  // Variables needed to form a ctau 
  // pT in units of 100 MeV 
  unsigned int eFexEt = eTau.etTOB();
  unsigned int eFexEta = eTau.iEta();
  // isolation in units of 200 MeV
  unsigned int jFexIso = jTau->tobIso();
  bool isIsolated  = false;
  uint64_t thresholdMask = 0;

  ATH_MSG_DEBUG("eFex tau eta,phi = " << eTau.iEta() << ", " << eTau.iPhi()
                << ", jFex tau eta,phi = " << jTau->globalEta() << ", " << jTau->globalPhi() 
		<< ", eFex et (100 MeV/counts) = " << eFexEt << ", jFex iso (200 MeV/counts) = " << jFexIso);

  // Iterate through thresholds and see which ones are passed
  for (const std::shared_ptr<TrigConf::L1Threshold>& thrBase : menuThresholds) {
    
    std::shared_ptr<TrigConf::L1Threshold_cTAU> thr = std::static_pointer_cast<TrigConf::L1Threshold_cTAU>(thrBase);
    // Checking et and isolation thresholds
    // The isolation value is multiplied by 2 to normalise to 100 MeV/counts units
    unsigned int isolation_score = TCS::cTauMultiplicity::convertIsoToBit( 2*float(jFexIso), float(eFexEt) );
    isIsolated = isocut(TrigConf::Selection::WP(thr->isolation()), isolation_score );
    // Using iEta coordinate for the eFEX ensures a 0.1 granularity of the eta coordinate, as expected from the menu method thrValue100MeV. 
    bool passPt = eFexEt > thr->thrValue100MeV(eFexEta);
    
    if ( isIsolated && passPt ) {
      thresholdMask |= (1<<thr->mapping());
    }
    
  } // loop over thr
  
  return thresholdMask;    

}
