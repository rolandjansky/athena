/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstitFourMomTool.cxx

#include "JetMomentTools/JetConstitFourMomTool.h"

#include "xAODJet/JetConstituentVector.h"
#include "xAODJet/JetTypes.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

//**********************************************************************

JetConstitFourMomTool::JetConstitFourMomTool(std::string myname)
  : JetModifierBase(myname),
    m_constitScale(0),
    m_jetScaleNames({}),
    m_altColls({}),
    m_altConstitScales({}),
    m_altJetScales({})
{
  // What cluster signal state to use for the jet constituents
  declareProperty("ConstitScale",     m_constitScale     );
  // The names of the jet scale
  declareProperty("JetScaleNames",    m_jetScaleNames    );
  // If an alternate collection should be used (e.g. origin-corrected or not)
  declareProperty("AltConstitColls",  m_altColls         );
  // What scales to use for the alternate constituent collections
  declareProperty("AltConstitScales", m_altConstitScales );
  // If an existing momentum scale should just be copied
  declareProperty("AltJetScales",     m_altJetScales     );
}

//**********************************************************************

StatusCode JetConstitFourMomTool::initialize() {
  // Check configuration consistency
  if( m_jetScaleNames.empty() ||
      (m_jetScaleNames.size() != m_altColls.size()) ||
      (m_jetScaleNames.size() != m_altConstitScales.size()) ||
      (m_jetScaleNames.size() != m_altJetScales.size())
      ) {
    ATH_MSG_FATAL("Inconsistency in configuration -- all vector properties must have the same (nonzero) length!");
    return StatusCode::FAILURE;
  }

  m_isDetectorEtaPhi.resize(m_jetScaleNames.size());
  for(size_t iScale=0; iScale<m_jetScaleNames.size(); ++iScale) {
    if(m_jetScaleNames[iScale]=="DetectorEtaPhi") {
      m_isDetectorEtaPhi[iScale] = true;
    }
    if(!m_altJetScales[iScale].empty() && !m_altColls[iScale].empty()) {
      ATH_MSG_FATAL("Inconsistency in configuration -- Both AltJetScale and AltConstitColl set for scale " 
		    << m_jetScaleNames[iScale] << "!");
      return StatusCode::FAILURE;      
    }
  }

  return StatusCode::SUCCESS;
}

int JetConstitFourMomTool::modify(xAOD::JetContainer& jets) const {
  if(jets.empty()) return 0;

  const size_t nScales=m_jetScaleNames.size();
  // This only really makes sense for clusters now, as signal states don't exist for other types
  std::vector<const xAOD::CaloClusterContainer*> altCollections(nScales,NULL);
  // Do some setup that doesn't have to be repeated for each jet
  for(size_t iScale=0; iScale<nScales; ++iScale) {
    if(!m_altColls[iScale].empty()) { // retrieve alternate constituent collections
      const xAOD::Jet& leadjet = *jets.front();
      if(leadjet.getInputType()==xAOD::JetInput::LCTopo || leadjet.getInputType()==xAOD::JetInput::EMTopo
	 || leadjet.getInputType()==xAOD::JetInput::LCTopoOrigin || leadjet.getInputType()==xAOD::JetInput::EMTopoOrigin) {
	const xAOD::CaloClusterContainer* altclusters(0);
	ATH_CHECK( evtStore()->retrieve(altclusters,m_altColls[iScale]) );
	if(!altclusters) {
	  ATH_MSG_WARNING("Failed to retrieve alt cluster collection " << m_altColls[iScale]);
	  return 1;
	} else {
	  altCollections[iScale] = altclusters;
	}
      } else {
	ATH_MSG_WARNING("Alt collection " << m_altColls[iScale] << " and jet type " << leadjet.getInputType() << " not supported yet!");
	return 1;
      } // check that jet type/alt collection are implemented
    } // have an alt collection for this scale
  }

  // Limit ourselves to one loop over the constituents
  for(const auto& jet : jets) {
    std::vector<xAOD::JetFourMom_t> constitFourVecs(nScales);
    // Get the constituents of the jet
    const xAOD::JetConstituentVector constituents = jet->getConstituents();
    // Gets the constituent iter at the appropriate scale
    xAOD::JetConstitScale constscale = (xAOD::JetConstitScale) m_constitScale;
    for (auto citer = constituents.begin(constscale);
	 citer != constituents.end(constscale); ++citer) {
      for (size_t iScale=0; iScale<nScales; ++iScale) {
	// If altJetScales is set, do nothing in the constituent loop.
	if(m_altJetScales[iScale].empty()) {
	  if(altCollections[iScale]) { // get the index-parallel alternative constituent
	    const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>((*altCollections[iScale])[(*citer)->rawConstituent()->index()]);
	    CaloClusterChangeSignalState(cluster, (xAOD::CaloCluster::State) m_altConstitScales[iScale]);
	    ATH_MSG_VERBOSE("Add cluster with pt " << cluster->pt() << " to constit fourvec for scale " << m_jetScaleNames[iScale] );
	    constitFourVecs[iScale] += xAOD::JetFourMom_t( cluster->pt(), cluster->eta(), cluster->phi(), cluster->m() );
	  } else { // add the constituent 4-mom
	    ATH_MSG_VERBOSE("Add raw constituent with pt " << (*citer)->pt() << " to constit fourvec for scale " << m_jetScaleNames[iScale] );
	    constitFourVecs[iScale] += **citer;
	  }
	}
      } // loop over scales
    } // loop over clusters

    // Now we can set the scales from the various four-vecs we have calculated
    ATH_MSG_VERBOSE("Jet has "
		 << " pt: " << jet->pt()
		 << ", eta: " << jet->eta()
		 << ", phi: " << jet->phi());
    for (size_t iScale=0; iScale<nScales; ++iScale) {
      if(!m_altJetScales[iScale].empty()) {
	// Easy case first -- just copy the momentum state
	constitFourVecs[iScale] = jet->jetP4(m_altJetScales[iScale]);
      }
      ATH_MSG_VERBOSE("Scale " << m_jetScaleNames[iScale] << " has "
		   << " pt: " << constitFourVecs[iScale].Pt()
		   << ", eta: " << constitFourVecs[iScale].Eta()
		   << ", phi: " << constitFourVecs[iScale].Phi());
      if(m_isDetectorEtaPhi[iScale]) {
	const static SG::AuxElement::Accessor<float> acc_modEta("DetectorEta");
	const static SG::AuxElement::Accessor<float> acc_modPhi("DetectorPhi");
	acc_modEta(*jet) = constitFourVecs[iScale].Eta();
	acc_modPhi(*jet) = constitFourVecs[iScale].Phi();
      ATH_MSG_VERBOSE("Detector eta: " << constitFourVecs[iScale].Eta()
		   << ", phi: " << constitFourVecs[iScale].Phi());
      } else {
	jet->setJetP4(m_jetScaleNames[iScale], constitFourVecs[iScale]);
      }
    } // loop over scales
  } // loop over jets

  return 0;
}
