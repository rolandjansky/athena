/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// method to find leading track closest to the tau passes
inline const xAOD::TrackParticle* FastTrackSelection::findLeadingTrack(const xAOD::TauJet *hlt_tau, const DataVector<xAOD::TrackParticle>* tracks) const {
  //  Lead Track Finding                                                                                                              
  const xAOD::TrackParticle* lead_trk(0);

  for (DataVector<xAOD::TrackParticle>::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr){
    const double lead_dR = Utils::DeltaR((*tr)->eta(), (*tr)->phi(), hlt_tau->etaDetectorAxis(), hlt_tau->phiDetectorAxis());
    //const double lead_dR = Utils::DeltaR((*tr)->eta(), (*tr)->phi(), hlt_tau->eta(), hlt_tau->phi() );
    if (!(lead_dR < 0.1)) {
      std::cout << "findLeadingTrack(): skipping track with pT=" << (*tr)->pt() << " eta=" << (*tr)->eta() << " phi=" << (*tr)->phi() << " for dR > 0.1" << std::endl;
      continue;
    }
    
    if (not lead_trk) {
      lead_trk = (*tr);
    }
    
    if (lead_trk->pt() < (*tr)->pt()) {
      lead_trk = (*tr);
    }
  }

  return lead_trk;
}

bool FastTrackSelection::trackSelection(const xAOD::TauJet *hlt_tau, const xAOD::TrackParticle *leadingTrack, const DataVector<xAOD::TrackParticle> *tracks) const {
  //NB: we could call the leading track finder oursevles, but then we don't know whether we're using TauCore or TauIso tracks
 
  float lead_trk_z0, lead_trk_phi, lead_trk_eta;
  bool usePileupSuppCut;

  if(leadingTrack) {
    lead_trk_z0 =  leadingTrack->z0();
    lead_trk_phi = leadingTrack->phi();
    lead_trk_eta = leadingTrack->eta();
    usePileupSuppCut = true;
    std::cout << "leading track pT = " << leadingTrack->pt() << std::endl;
  } else {
    lead_trk_z0 = 0.0;
    lead_trk_eta = hlt_tau->etaDetectorAxis();
    lead_trk_phi = hlt_tau->phiDetectorAxis();
    usePileupSuppCut = false;
  }
 
  int nCore = 0;
  int nIso = 0;
  for(auto t: *tracks){
      
    const float deltaz0 = fabs(t->z0() - lead_trk_z0);
    if (usePileupSuppCut && deltaz0 >= m_delta_z0) { 
      continue;
    }
    
    const double dR = Utils::DeltaR_Square(t->eta(), t->phi(), lead_trk_eta, lead_trk_phi);
    
    if (dR < m_core * m_core) {
      std::cout << "TauIso: nCore++: dR = " << dR << " for track " << t << " with pT=" << t->pt() << " eta=" << t->eta()  << " phi=" << t->phi() << std::endl;
      ++nCore;
    } else if (dR < m_iso * m_iso) { 
      ++nIso;
      std::cout << "TauIso: nIso++: dR = " << sqrt(dR) << " for track " << t << " with pT=" << t->pt() << " eta=" << t->eta()  << " phi=" << t->phi() << std::endl;
    }
  }
 
  //if (nCore > 0 && nCore < m_ncore_bound && nIso < m_niso_bound) {
  if (nCore < m_ncore_bound && nIso < m_niso_bound) {
    std::cout << "ACCEPT nCore = " << nCore << " nIso = " << nIso << std::endl; 
    return true;
  } 

  std::cout << "REJECT nCore = " << nCore << " nIso = " << nIso << std::endl; 
  return false;
}

const asg::AcceptInfo& FastTrackSelection::getAcceptInfo() const
{
  return m_accept;
}


// Accept method for EDM tau and TauIso and TauCore tracks from FTF
asg::AcceptData FastTrackSelection::accept(const xAOD::TauJet *hlt_tau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const
{
  // NOTE: this methods wants to be passed a CaloOnly tau
  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("FastTrackSel", false);

  if (not m_use_fasttracking) {
    // passthrough mode
    acceptData.setCutResult("FastTrackSel", true);
    return acceptData;
  }

  std::cout << "Checking tracks for HLT tau with pT=" << hlt_tau->pt() << " eta=" << hlt_tau->eta() << " phi=" << hlt_tau->phi() << std::endl;

  const xAOD::TrackParticle* coreLeadingTrack = findLeadingTrack(hlt_tau, preselTracksCore);
  if(!coreLeadingTrack){
    // TauCore container doesn't have a leading track -> means it's empty for this tau
    std::cout << "no core leading track found" << std::endl;
    return acceptData;
  }

  bool passCoreCut = trackSelection(hlt_tau, coreLeadingTrack, preselTracksCore);
  if(!passCoreCut){
    std::cout << "did not pass trackSelection() for core" << std::endl;
    return acceptData;
  }
  std::cout << "passed trackSelection() for core" << std::endl;

  const xAOD::TrackParticle* isoLeadingTrack = findLeadingTrack(hlt_tau, preselTracksIso);
  if(isoLeadingTrack){
    std::cout << "iso leading track pT = " << isoLeadingTrack->pt() << std::endl;
  } else {
    std::cout << "no iso leading track" << std::endl;
  }

  bool passIsoCut = trackSelection(hlt_tau, isoLeadingTrack, preselTracksIso);
  if(passIsoCut){
    acceptData.setCutResult("FastTrackSel", true);
    std::cout << "passed trackSelection() for core & iso" << std::endl;
  } else {
    std::cout << "did not pass trackSelection() for iso" << std::endl;
  }

  return acceptData;
}

// Accept method for EDM tau and associated track container
asg::AcceptData FastTrackSelection::accept(const xAOD::TauJet * hltau, const DataVector<xAOD::TrackParticle> * tracks) const
{
  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("FastTrackSel", false);

  if (not m_use_fasttracking) {
    // passthrough mode
    acceptData.setCutResult("FastTrackSel", true);
    return acceptData;
  }

  //  Lead Track Finding                                                                                                              
  const xAOD::TrackParticle * lead_trk(0);

  // std::cout << hltau << " nTracks = " << tracks->size() << std::endl;

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

    //const double lead_dR = Utils::DeltaR((*tr)->eta(), (*tr)->phi(), hltau->eta(), hltau->phi());
    const double lead_dR = Utils::DeltaR((*tr)->eta(), (*tr)->phi(), hltau->etaDetectorAxis(), hltau->phiDetectorAxis());
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

  // std::cout << "Trying tau " << hltau << " with usePileupSuppCut = " << usePileupSuppCut << std::endl;
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
  // std::cout << "Ncore = " << Ncore << " Niso = " << Niso << std::endl;
  //std::cout << "NcoreBound = " << m_ncore_bound << " NisoBound = " << m_niso_bound << std::endl;

  if (Ncore > 0 && Ncore < m_ncore_bound && Niso < m_niso_bound) {
    acceptData.setCutResult("FastTrackSel", true);
    // std::cout << "ACCEPTING FastTrackSel CUT" << std::endl;
  } else {
    acceptData.setCutResult("FastTrackSel", false);
    // std::cout << "REJECTING FastTrackSel CUT" << std::endl;
  }
  
  return acceptData;
}

asg::AcceptData FastTrackSelection::accept(const xAOD::TauJet * presel_tau) const {
  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("FastTrackSel", false);

  if (not m_use_fasttracking) {
    // passthrough mode
    //std::cout << "passthrough mode - accept candidate regardless of tracks" << std::endl;
    acceptData.setCutResult("FastTrackSel", true);
    return acceptData;
  }

  // Track Counting
  int Ncore = presel_tau->nTracks();
#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
  int Niso  = presel_tau->nTracksIsolation();
#else
  int Niso  = presel_tau->nWideTracks();
#endif

  //std::cout << "\t \t \t \t Presel tau " << presel_tau->index() 
        //<< ": pt / eta / phi / Ncore / Niso = " 
        //<< presel_tau->pt() << " / " << presel_tau->eta() << " / "
        //<< presel_tau->phi() << " / " << Ncore << " / " << Niso << std::endl;

  if (Ncore > 0 && Ncore < m_ncore_bound && Niso < m_niso_bound) {
    //std::cout << "accepting nCore = " << Ncore << " and nIso = " << Niso << " (bounds " << m_ncore_bound << " and " << m_niso_bound << ")" << std::endl;
    acceptData.setCutResult("FastTrackSel", true);
  } else {
    //std::cout << "NOT accepting nCore = " << Ncore << " and nIso = " << Niso << " (bounds " << m_ncore_bound << " and " << m_niso_bound << ")" << std::endl;
  }

  return acceptData;
}

