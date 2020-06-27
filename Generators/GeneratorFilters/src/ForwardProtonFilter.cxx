/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/ForwardProtonFilter
//
// Allows the user to search for forward intact protons originating from
// elastic and diffractive interactions. Event passes filters if there forward
// proton with xi and pt in given range. If double tag option is enables, such
// protons are requested on both sides. single_tagA/single_tagC require protons
// on a given side, regardless what's happenning on the other
//
// Authors:
// Rafal Staszewski   Jul 2011

#include "GeneratorFilters/ForwardProtonFilter.h"


ForwardProtonFilter::ForwardProtonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("xi_min", m_xiMin = 0.01);
  declareProperty("xi_max", m_xiMax = 0.20);
  declareProperty("pt_min", m_ptMin = 0);     // [MeV]
  declareProperty("pt_max", m_ptMax = 10e3);  // [MeV]
  declareProperty("beam_energy", m_E0 = 7e6); // [MeV]
  declareProperty("double_tag", m_DoubleTag = false);
  declareProperty("single_tagA", m_Single_tagA = false);  // require proton on A, regardless of C
  declareProperty("single_tagC", m_Single_tagC = false);  // require proton on C, regardelss of A
}


StatusCode ForwardProtonFilter::filterEvent() {
  bool accepted_A = false;
  bool accepted_C = false;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;
    for (auto part: *genEvt) {
      // We're only interested in stable (status == 1) particles
      if ( part->status() != 1) continue;

      // We are specifically looking for protons
      const long pid = part->pdg_id();
      if (pid != 2212 ) continue;

      const double E = part->momentum().e();
      const double pz = part->momentum().pz();
      const double pt = part->momentum().perp();
      const double xi = (m_E0-E)/m_E0;
      if (m_xiMin <= xi && xi <= m_xiMax && m_ptMin <= pt && pt <= m_ptMax) {
        accepted_C = (pz > 0);
        accepted_A = (pz < 0);
      }

      if (accepted_A) ATH_MSG_DEBUG("Found a proton with xi=" << xi << " on side C");
      if (accepted_C) ATH_MSG_DEBUG("Found a proton with xi=" << xi << " on side A");

      if ( m_DoubleTag && (accepted_A && accepted_C) ) return StatusCode::SUCCESS;
      if (!m_DoubleTag ) {

         // if Single tag is not requested, do or
         if(!m_Single_tagA && !m_Single_tagC && (accepted_A || accepted_C) ) return StatusCode::SUCCESS;

         // if single tag request - check for presence on particular side
         if(m_Single_tagA && accepted_A) return StatusCode::SUCCESS;
         if(m_Single_tagC && accepted_C) return StatusCode::SUCCESS;

      }

    }
  }

  // If we get here the event doesn't pass the criteria
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
