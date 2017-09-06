/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuFineEtaPhiEfficiencyPlots.cxx
 *
 *  Created on: Apr 8, 2015
 *      Author: goblirsc
 */

#include "MuonPerformanceHistUtils/ZmumuFineEtaPhiEfficiencyPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/Muon.h"

ZmumuFineEtaPhiEfficiencyPlots::ZmumuFineEtaPhiEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched, ApplySF),
    m_isMatched(isMatched) {}

void ZmumuFineEtaPhiEfficiencyPlots::initializePlots()
{
  m_fineEtaPhi  = Book1D("fineEtaPhi" ," fineEtaPhi; fine (#eta, #phi) bin; Entries ",m_fepb.nbins(),m_fepb.firstbin()-0.5, m_fepb.lastbin()+0.5);
  m_fineEtaPhi_posq  = Book1D("fineEtaPhi_posq" ," fineEtaPhi_posq; fine (#eta, #phi) bin; Entries ",m_fepb.nbins(),m_fepb.firstbin()-0.5, m_fepb.lastbin()+0.5);
  m_fineEtaPhi_negq  = Book1D("fineEtaPhi_negq" ," fineEtaPhi_negq; fine (#eta, #phi) bin; Entries ",m_fepb.nbins(),m_fepb.firstbin()-0.5, m_fepb.lastbin()+0.5);
}

void ZmumuFineEtaPhiEfficiencyPlots::fill(Probe& probe)
{
  if(m_isMatched && !probe.isMatched()) return;
  float sfweight = (m_isMatched && m_apply_SF ? probe.sfweight() : 1.);
  //  case 1: ID track
  const xAOD::TrackParticle* trk = dynamic_cast<const xAOD::TrackParticle*>(&(probe.probeTrack()));
    
  //  in case that didn't work,  try a muon! 
  if (!trk) {
    const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*> (&(probe.probeTrack()));
    if (mu && mu->primaryTrackParticle()) trk = mu->primaryTrackParticle();
    if (!trk && mu && *(mu->inDetTrackParticleLink())) trk = *(mu->inDetTrackParticleLink());
  }
  if (trk) {
    if(trk->charge() > 0) m_fineEtaPhi_posq->Fill(m_fepb.bin(probe.probeTrack().p4()),sfweight);
    if(trk->charge() < 0) m_fineEtaPhi_negq->Fill(m_fepb.bin(probe.probeTrack().p4()),sfweight);
  }
  else {
    // truth probes: rely on the sign of the pdg ID
    const xAOD::TruthParticle* truthtrk = dynamic_cast<const xAOD::TruthParticle*>(&(probe.probeTrack()));
    if (truthtrk){
      if (truthtrk->charge() > 0) m_fineEtaPhi_posq->Fill(m_fepb.bin(probe.probeTrack().p4()),sfweight);
      if (truthtrk->charge() < 0) m_fineEtaPhi_negq->Fill(m_fepb.bin(probe.probeTrack().p4()),sfweight);
    }
  }

  m_fineEtaPhi->Fill(m_fepb.bin(probe.probeTrack().p4()),sfweight);
    

}
