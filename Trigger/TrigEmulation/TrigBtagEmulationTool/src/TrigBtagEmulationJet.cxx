/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#include "src/TrigBtagEmulationJet.h"
#include "xAODBTagging/BTaggingUtilities.h"
#include "FlavorTagDiscriminants/DL2HighLevel.h" 

//**********************************************************************

namespace Trig {

TrigBtagEmulationJet::TrigBtagEmulationJet(const std::string& name,
					   const xAOD::Jet *jet)
  : AthMessaging(name),
    m_jet(jet),
    m_pt(jet->pt()),
    m_eta(jet->eta()),
    m_phi(jet->phi())
{}

TrigBtagEmulationJet::TrigBtagEmulationJet(const std::string& name,
					   const xAOD::JetRoI *jetRoI, 
					   bool isJJ) 
  : AthMessaging(name),
    m_pt(isJJ ? jetRoI->et4x4() * 0.001 : jetRoI->et8x8() * 0.001),
    m_eta(jetRoI->eta()),
    m_phi(jetRoI->phi())
{}

bool TrigBtagEmulationJet::isPassedBTagger(const std::string& btagger, 
					   double workingPoint) const 
{
  if( btagger.substr(0, 7) == "offperf" ) { // 7 -> strlen("offperf")
    ATH_MSG_DEBUG( "offperf tagger always passes");
    return true;
  }
  
  bool res = false;
  const xAOD::BTagging *btag = xAOD::BTaggingUtilities::getBTagging( *m_jet );
  if (not btag) {
    ATH_MSG_DEBUG( "Couldn't retrieve BTagging for jet pt:" << pt() << ", eta:" << eta() << ", so not a bjet" );
    return false;
  }

  if( btagger.substr(0, 4) == "dl1r" ) { // 4 -> strlen("dl1r")
    res = satisfy("DL1r", btag, workingPoint);
  } else if( btagger.substr(0, 4) == "dl1d" ) { // 4 -> strlen("dl1d")
    res = satisfy("DL1d20211216", btag, workingPoint);
  } else if( btagger.substr(0, 9) == "newTagger" ) { // 9 -> strlen("newTagger")
    static const std::map<std::string, std::string> remapping
      = {{"DL1d20210519r22_pu","DL1dEMUL_pu"},
	 {"DL1d20210519r22_pc","DL1dEMUL_pc"},
	 {"DL1d20210519r22_pb","DL1dEMUL_pb"}};
    static const FlavorTagDiscriminants::DL2HighLevel dl2
      = FlavorTagDiscriminants::DL2HighLevel("BTagging/20210519r22/dl1d/antikt4empflow/network.json",
					     FlavorTagDiscriminants::FlipTagConfig::STANDARD,
					     remapping);
    dl2.decorate(*btag);
    
    res = satisfy("DL1dEMUL", btag, workingPoint);
  } else {
    ATH_MSG_WARNING( "Tagger " << btagger << " not supported." );
  }
  
  return res;
}

bool TrigBtagEmulationJet::satisfy(const std::string& tagger_name,
				   const xAOD::BTagging *btag,
				   double workingPoint) const 
{
  double pu = -1;
  double pb = -1;
  double pc = -1;

  btag->pu(tagger_name, pu); 
  btag->pb(tagger_name, pb);
  btag->pc(tagger_name, pc);

  double tagger_weight = dl1r_weight(pu,pb,pc);
  ATH_MSG_DEBUG( "jet pt:" << (pt() / Gaudi::Units::GeV) << ", " << tagger_name << " weight: " << tagger_weight << " (req " << workingPoint << ", pu:" << pu << " pb:" << pb << " pc:" << pc << ")" );

  return tagger_weight > workingPoint;
}

}

//**********************************************************************

