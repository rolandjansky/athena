/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/LeptonFilter
//
// Allows the user to search for electrons or positrons. Event passes filters if
// there is a stable e or mu with p_t and eta in the specified range default is
// p_t >= 10 GeV and unlimited eta.
//
// Authors:
// Ian Hinchliffe  Dec 2001         
// Andy Buckley    Apr 2009

#include "GeneratorFilters/LeptonFilter.h"
#include <cmath>


LeptonFilter::LeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator) 
{
  declareProperty("Ptcut", m_Ptmin = 10000.0);
  declareProperty("Etacut", m_EtaRange = 10.0);
  declareProperty("PtcutMax", m_Ptmax = 1e99);
}


StatusCode LeptonFilter::filterEvent() {
  // Loop over all events in McEventCollection and extract the leading lepton pt
  double leading_lepton_pt = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

    // Loop over all particles in event
    for (auto part: *genEvt) {

      // We're only interested in stable (status == 1) particles
      if ( part->status() != 1) continue;

      // We are specifically looking for electrons (+-11) and muons (+-13)
      const long pid = part->pdg_id();
      const long apid = std::abs(pid);
      if (apid == 11 || apid == 13) {
        const double pT = part->momentum().perp();
        const double eta = part->momentum().pseudoRapidity();
        const std::string pname = ((apid == 11) ? "electron" : "muon");
        ATH_MSG_DEBUG( "Found " << pname
		       << ": pT, eta = " << pT << ", " << eta );
        
        // If we've found a stable electron or muon, check eta and pt
        if (pT > leading_lepton_pt && std::abs(eta) <= m_EtaRange) {
	  leading_lepton_pt = pT;
        }
      }
    }
  }

  ATH_MSG_DEBUG ( "Leading lepton pt = " << leading_lepton_pt << "within |eta| <= " << m_EtaRange);

  if (leading_lepton_pt < m_Ptmin) {
    setFilterPassed(false);
    ATH_MSG_DEBUG( "Fail: no e or mu found "
		   << " with pT >= " << m_Ptmin);
  } else if (leading_lepton_pt >= m_Ptmax) {
    setFilterPassed(false);
    ATH_MSG_DEBUG ( "Fail: high pt lepton veto "
		    << " pT < " << m_Ptmax );
  } else {
    setFilterPassed(true);
    ATH_MSG_DEBUG ( "Within min and max pt cuts " << m_Ptmin << ", " 
		    << m_Ptmax );
  }
  return StatusCode::SUCCESS;
}
