/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// #include "TrigTauEmulation/FastTrackSelection.h"
#include "TrigTauEmulation/HltTauSelection.h"
#include "TrigTauEmulation/Utils.h"

// Default constructor
FastTrackSelection::FastTrackSelection() :
  m_use_fasttracking(true),
  m_d0(2.0),
  m_z0(150.0),
  m_delta_z0(2.0),
  m_core(0.2),
  m_iso(0.4),
  m_ncore_bound(4),
  m_niso_bound(2)
{

  m_accept.addCut("FastTrackSel", "FastTrackSel");

}


// Accept method
const Root::TAccept& FastTrackSelection::accept(const xAOD::TauJet * hltau, const DataVector<xAOD::TrackParticle> * tracks) const

{
  m_accept.clear();
  m_accept.setCutResult("FastTrackSel", false);

  if (not m_use_fasttracking) {
    // passthrough mode
    m_accept.setCutResult("FastTrackSel", true);
    return m_accept;
  }

  //  Lead Track Finding                                                                                                              
  const xAOD::TrackParticle * lead_trk(0);

  std::cout << hltau << " nTracks = " << tracks->size() << std::endl;

  //if (tracks->size() == 0){
    //std::cout << "NO TRACKS ASSOCIATED" << std::endl;
  //}

  for (DataVector<xAOD::TrackParticle>::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr){
    //if (!((*tr)->pt() > 1000.))  {
      //continue;
    //}
    // if (!(fabs((*tr)->d0() < m_d0)))
    //   continue;
    // if (!(fabs((*tr)->z0() < m_z0)))
    //   continue;

    //const auto tp = (*tr)->perigeeParameters();

    const double lead_dR = Utils::DeltaR((*tr)->eta(), (*tr)->phi(), hltau->eta(), hltau->phi());
    if (!(lead_dR < 0.1)) { 
      continue;
    }
    
    if (not lead_trk) {
      lead_trk = (*tr);
    }
    
    if (lead_trk->pt() < (*tr)->pt()) {
      lead_trk = (*tr);
    }
  }


  double lead_trk_z0, lead_trk_eta, lead_trk_phi;
  bool usePileupSuppCut = true;
  if(lead_trk) {
    lead_trk_z0 = lead_trk->z0();
    lead_trk_phi = lead_trk->phi();
    lead_trk_eta = lead_trk->eta();
    usePileupSuppCut = true;
  } else {
    lead_trk_z0 = 0.0;
    lead_trk_eta = hltau->etaDetectorAxis();
    lead_trk_phi = hltau->phiDetectorAxis();
    usePileupSuppCut = false;
  }

  std::cout << "Trying tau " << hltau << " with usePileupSuppCut = " << usePileupSuppCut << std::endl;
  //std::cout << "lead_trk_z0 = " << lead_trk_z0 << " lead_trk_eta = " << lead_trk_eta << " lead_trk_phi = " << lead_trk_phi << std::endl;

  // Track Counting
  int Ncore = 0;   
  int Niso  = 0;
  //if (lead_trk) {
    for (DataVector<xAOD::TrackParticle>::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr) {
      //if (!((*tr)->pt() > 1000.))  {
        //continue;
      //}
      
      // if (!(fabs((*tr)->d0() < m_d0)))
      //   continue;
      // if (!(fabs((*tr)->z0() < m_z0)))
      //   continue;
      
      float deltaz0 = fabs((*tr)->z0() - lead_trk_z0);
      
      //if( !((deltaz0 < m_delta_z0) || !usePileupSuppCut) ){
      if (usePileupSuppCut && deltaz0 >= m_delta_z0) { 
        //std::cout << "\t deltaz0 = " << deltaz0 << " -> NEXT TRACK" << std::endl;
        continue;
      }
     
      //std::cout << "\t tr->eta = " << (*tr)->eta() << " tr->phi() = " << (*tr)->phi() << std::endl; 
      const double dR = Utils::DeltaR_Square((*tr)->eta(), (*tr)->phi(), lead_trk_eta, lead_trk_phi);
      
      //std::cout << "\tdR = " << dR << std::endl;
      if (dR < m_core * m_core) {
        //std::cout << "\t\t => nCore++" << std::endl;
        Ncore++;
      } else if (dR < m_iso * m_iso) { 
        Niso++;
        //std::cout << "\t\t => nIso++" << std::endl;
      }
  }
 
  //std::cout << "m_core * m_core = " << m_core*m_core << " m_iso*m_iso = " << m_iso*m_iso << std::endl;
  std::cout << "Ncore = " << Ncore << " Niso = " << Niso << std::endl;
  //std::cout << "NcoreBound = " << m_ncore_bound << " NisoBound = " << m_niso_bound << std::endl;

  if (Ncore > 0 && Ncore < m_ncore_bound && Niso < m_niso_bound) {
    m_accept.setCutResult("FastTrackSel", true);
    std::cout << "ACCEPTING FastTrackSel CUT" << std::endl;
  } else {
    m_accept.setCutResult("FastTrackSel", false);
    std::cout << "REJECTING FastTrackSel CUT" << std::endl;
  }
  
  return m_accept;
}

const Root::TAccept& FastTrackSelection::accept(const xAOD::TauJet * presel_tau) const

{
  m_accept.clear();
  m_accept.setCutResult("FastTrackSel", false);

  if (not m_use_fasttracking) {
    // passthrough mode
    m_accept.setCutResult("FastTrackSel", true);
    return m_accept;
  }

  // Track Counting
  int Ncore = presel_tau->nTracks();   
  int Niso  = presel_tau->nWideTracks();
  
  //std::cout << "\t \t \t \t Presel tau " << presel_tau->index() 
        //<< ": pt / eta / phi / Ncore / Niso = " 
        //<< presel_tau->pt() << " / " << presel_tau->eta() << " / "
        //<< presel_tau->phi() << " / " << Ncore << " / " << Niso << std::endl;

  if (Ncore > 0 && Ncore < m_ncore_bound && Niso < m_niso_bound) {
    m_accept.setCutResult("FastTrackSel", true);
  }

  return m_accept;
}



