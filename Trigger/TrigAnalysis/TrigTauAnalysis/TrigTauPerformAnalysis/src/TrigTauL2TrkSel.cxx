/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2TrkSel.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L2 selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TrigTauPerformAnalysis/TrigTauL2TrkSel.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"
#include "TrigTauPerformAnalysis/TrigTauHistogram.h"


#include <math.h>
#include <iostream>
using namespace std;

//
// -----------------------------------------------------------------------
//

TrigTauL2TrkSel::TrigTauL2TrkSel()
  :  cut_NtracksMin         (0),         
     cut_NtracksMax         (0),         
     cut_NtracksIsolation(0),                            
     cut_NtracksSlow     (0),    
     cut_NtracksMatched  (0),      
     maxPtMin            (0),
     chargeCut           (0),
     sumPtRatio          (100),
     trkEmEtMin          (0),
     _debug(false),
     menu(TrigTauFlags::SECONDATTEMPT),
     m_histManager(0)
{}

TrigTauL2TrkSel::~TrigTauL2TrkSel()
{
  if( m_histManager ) { delete m_histManager; m_histManager=0;}
}

TrigTauL2TrkSel::TrigTauL2TrkSel(const TrigTauL2TrkSel& other)
  :  cut_NtracksMin         (other.cut_NtracksMin         ),        
     cut_NtracksMax         (other.cut_NtracksMax         ),        
     cut_NtracksIsolation(other.cut_NtracksIsolation),
     cut_NtracksSlow     (other.cut_NtracksSlow     ),    
     cut_NtracksMatched  (other.cut_NtracksMatched  ),      
     maxPtMin            (other.maxPtMin            ),
     chargeCut           (other.chargeCut           ),
     sumPtRatio          (other.sumPtRatio          ),
     trkEmEtMin          (other.trkEmEtMin          ),
     _debug              (other._debug              ),
     menu                (other.menu                ),
     m_histManager       (other.m_histManager       )
{}

void
TrigTauL2TrkSel::setCuts(int nTrMax, int nTrISO, int nTrSlow, int nTrkMatched,
			 float maPtSlow , float dRIso , float mPt , 
			 float dRMatch, float dRt, float maxPt, bool charge,
			 float ssumPtRatio, float trkEmEt, int nTrMin)
{
  cut_NtracksMin         = nTrMin;
  cut_NtracksMax         = nTrMax;
  cut_NtracksIsolation= nTrISO;
  cut_NtracksSlow     = nTrSlow;
  cut_NtracksMatched  = nTrkMatched;
  maxPtMin            = maxPt;
  chargeCut           = charge;
  sumPtRatio          = ssumPtRatio;
  trkEmEtMin          = trkEmEt;
  maPtSlow = maPtSlow;
  dRIso = dRIso;
  mPt = mPt;
  dRMatch = dRMatch;
  dRt = dRt;

  // Reset (these can no longer be set here)
  maPtSlow = 0;
  dRIso = 0.;
  mPt = 0.;
  dRMatch = 0.;
  dRt = 0.;
}

void
TrigTauL2TrkSel::print() const {

  if (menu != TrigTauFlags::SECONDATTEMPT) {
    cout << "LVL2 selection based on trigger decisision bits for " 
	 << TrigTauFlags::menuName(menu) << endl;
    return;
  }

  cout << " L2Track cuts :" << endl;

  if (cut_NtracksMax >= 0) { 
    cout <<  cut_NtracksMin << " <= Ntracks         <= " << cut_NtracksMax         << endl;
  } else {
    cout << " Ntracks:           not applied" << endl;
  }

  cout << " max(Pt_track)   >= " << maxPtMin            << endl;

  if (cut_NtracksIsolation >= 0) { 
    cout << " NtracksIsolation<= " << cut_NtracksIsolation<< endl;
  } else {
    cout << " NtracksIsolation:  not applied" << endl;
  }

  if (cut_NtracksSlow >= 0) { 
    cout << " NtracksSlow     <= " << cut_NtracksSlow     << endl;
  } else {
    cout << " NtracksSlow:       not applied" << endl;
  }

  if (cut_NtracksMatched >= 0) { 
    cout << " NtracksMatched  <= " << cut_NtracksMatched  << endl;
  } else {
    cout << " NtracksMatched:    not applied" << endl;
  }

  if (chargeCut) {
    cout << " Charge cut applied " << endl;
  } else {
    cout << " Charge cut not applied " << endl;
  }

  if (sumPtRatio < 100.) {
    cout << " sumPtRatio <= " << sumPtRatio << endl;
  } else {
    cout << " sumPtRatio:        not applied" << endl;
  } 

  if (trkEmEtMin > 0.) {
    cout << " Et(trk+EM) >  " << trkEmEtMin << endl;
  } else {
    cout << " trkEmEt:           not applied" << endl;
  }

}

void TrigTauL2TrkSel::setHistDebug(TrigTauHistogram::HISTTYPE type)
{
  if( type != TrigTauHistogram::NONE ){
    //gROOT->cd();
    if( m_histManager ) delete m_histManager;
    m_histManager = new TrigTauHistogram(type);
    m_histManager->book("baseL2Trk","Before any cut");
    m_histManager->book("ntrkExist","L2Trk: Ntrk>0 ");
    m_histManager->book("ntrk","L2Trk: Cut on Ntrk ");
    m_histManager->book("leadPt","L2Trk: Cut on leading Pt");
    m_histManager->book("nSlow","L2Trk: Cut on number of slow tracks");
    m_histManager->book("nIso","L2Trk: Cut on N isolation tracks");
    m_histManager->book("nMatched","L2Trk: Cut on N matched");
    m_histManager->book("Q","L2Trk: Cut on total charge");
    m_histManager->book("sumPtCore","L2Trk: Cut on sumPtCoree");
    m_histManager->book("Et_trkEM","L2Trk: Cut on Et(trk+EM)");

  }else{
      if( m_histManager ) { delete m_histManager; m_histManager=0;}
  } 
}


bool
TrigTauL2TrkSel::passed(const TrigTauL2Object& tau) {
  if (menu != TrigTauFlags::SECONDATTEMPT) return tau.m_tags.passedL2(menu);
  return (cutWord(tau) == 0);
}


unsigned long
TrigTauL2TrkSel::cutWord(const TrigTauL2Object& tau) {
  float varHist = 0;
  if( m_histManager)
    {
      if( m_histManager->type() == TrigTauHistogram::PTHIST ) varHist = tau.Pt();
      else if( m_histManager->type() == TrigTauHistogram::ETAHIST ) varHist = tau.Eta();
      else if( m_histManager->type() == TrigTauHistogram::PHIHIST ) varHist = tau.Phi();
    }

  if( m_histManager) m_histManager->fill(0, varHist);

  if(  tau.tracks.begin() !=  tau.tracks.end() )
    if( m_histManager) m_histManager->fill(1, varHist);

  if (_debug) cout << "L2Trk:  eta="<<tau.Eta() <<", phi="<<tau.Phi() << " RoIWord="
		   <<  std::hex <<tau.m_roiWord << std::dec <<endl;

  unsigned long word = 0;
  if (_debug) cout << "nTrk="<< tau.m_nCoreTracks << " ";
  if( cut_NtracksMin         >=0 &&  tau.m_nCoreTracks    < cut_NtracksMin          ) {
    if (_debug) cout << " - reject nTrMin" << endl;
    word |= L2CUT_NTRK;
  }
  if( cut_NtracksMax         >=0 &&  tau.m_nCoreTracks     > cut_NtracksMax          ) {
    if (_debug) cout << " - reject nTrMax" << endl;
    word |= L2CUT_NTRK;
  }
  if( m_histManager && word==0) m_histManager->fill(2, varHist);

  if (_debug) cout << "LeadPt="<<tau.m_leadingTrackPt << " ";
  if ( tau.m_leadingTrackPt < maxPtMin) {
    if (_debug) cout << " - reject maxPt" << endl;
    word |= L2CUT_LEADPT;
  }
  if( m_histManager && word==0) m_histManager->fill(3, varHist);

  if (_debug) cout <<"nSlow="<< tau.m_nSlowTracks << " ";
  if( cut_NtracksSlow     >=0 &&  tau.m_nSlowTracks       > cut_NtracksSlow      ) {
    if (_debug) cout << " - reject nSlow" << endl;
    word |= L2CUT_NSLOW;
  }
  if( m_histManager && word==0) m_histManager->fill(4, varHist);

  if (_debug) cout << "nIso="<< tau.m_nIsoTracks << " ";
  if( cut_NtracksIsolation>=0 &&  tau.m_nIsoTracks     > cut_NtracksIsolation ) {
    if (_debug) cout << " - reject nIso" << endl;
    word |= L2CUT_NISO;
  }
  if( m_histManager && word==0) m_histManager->fill(5, varHist);

  if (_debug) cout << "nMatched="<< tau.m_nMatchedTracks << " ";
  if( cut_NtracksMatched  >=0 &&  tau.m_nMatchedTracks > cut_NtracksMatched       ) {
    if (_debug) cout << " - reject nMatch" << endl;
    word |= L2CUT_NMATCHED;
  }
  if( m_histManager && word==0) m_histManager->fill(6, varHist);

  if (_debug) cout << "Q="<< tau.m_charge << " ";
  if ( chargeCut && fabs(tau.m_charge) > 2) {
    if (_debug) cout << " - reject totCh" << endl;
    word |= L2CUT_CHARGE;
  }
  if( m_histManager && word==0) m_histManager->fill(7, varHist);

  float ratioIsoCore = 0;
  if( tau.m_scalarPtSumCore > 0 ) ratioIsoCore = tau.m_scalarPtSumIso/tau.m_scalarPtSumCore;

  if (_debug) cout << "sumPtRatio="<< ratioIsoCore << " ";
  if (sumPtRatio < 100.) {
    if ( ratioIsoCore > sumPtRatio) {
      if (_debug) cout << " - reject sumPtRatio" << endl;
      word |= L2CUT_RISO;
    }
  }
  if( m_histManager && word==0) m_histManager->fill(8, varHist);

  float emEt = tau.m_EMEnergyWide[0]+tau.m_EMEnergyWide[1]+tau.m_EMEnergyWide[2];
  emEt /= TMath::CosH(tau.m_caloEta);

  TLorentzVector trkEMSum;
  trkEMSum.SetPtEtaPhiM(emEt, tau.m_caloEta, tau.m_caloPhi, 0.);

  trkEMSum += tau.m_3fastest;
  float trkEM2Et = trkEMSum.Et();


  if (_debug) cout << "Et(trk+EM)=" << trkEM2Et << " ";
  if (tau.m_nCoreTracks > 1 && trkEM2Et <= trkEmEtMin) {
    if (_debug) cout << " - reject Et(trk+EM)" << endl;
    word |= L2CUT_ETTRKEM;
  }
  if( m_histManager && word==0) m_histManager->fill(9, varHist);

  if (_debug && word==0) cout << " accepted" << endl;
  return word;
}
