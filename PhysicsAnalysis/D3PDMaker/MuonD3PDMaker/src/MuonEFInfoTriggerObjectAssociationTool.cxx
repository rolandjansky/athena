/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonEFInfoTriggerObjectAssociationTool.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Associate from an Analysis::Muon to matching Event Filter trigger object.
 */


#include "MuonEFInfoTriggerObjectAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonEFInfoTriggerObjectAssociationTool::MuonEFInfoTriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{
  declareProperty("EFTrigMatchType",  m_efTrigMatchType); // CB or MS or ME or MG
}

///const HepMC::GenParticle*
///MuonGenParticleAssociationTool::get (const Analysis::Muon& p)
///{

const TrigMuonEFInfo* MuonEFInfoTriggerObjectAssociationTool::get(const Analysis::Muon& p) {
  if(m_efTrigMatchType=="CB") TrigMatch::TrigMuonEFInfoHelper::setTrackToUse(TrigMatch::useCombinedTrack);
  else if(m_efTrigMatchType=="MS") TrigMatch::TrigMuonEFInfoHelper::setTrackToUse(TrigMatch::useSpectrometerTrack);
  else if(m_efTrigMatchType=="ME") TrigMatch::TrigMuonEFInfoHelper::setTrackToUse(TrigMatch::useExtrapolatedTrack);
  else if(m_efTrigMatchType=="MG") TrigMatch::TrigMuonEFInfoHelper::setTrackToUse(TrigMatch::useMuGirlTrack);
  else REPORT_MESSAGE(MSG::ERROR) << "MuonD3PDMaker::MuonEFInfoTriggerObjectAssociationTool: bad trigger match type";

  return Base::get(p);

}

} //  namespace D3PD
