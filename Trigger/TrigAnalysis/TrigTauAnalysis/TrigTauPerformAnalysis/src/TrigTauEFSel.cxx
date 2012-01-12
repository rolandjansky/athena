/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauEFSel.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply EF selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TrigTauPerformAnalysis/TrigTauEFSel.h"

#include "TrigTauPerformNtuple/TrigTauRecEFObject.h"
#include "TrigTauPerformNtuple/TrigTauTrackCollection.h"

#include <math.h>
#include <iostream>
using namespace std;

// Need to add some code here...

TrigTauEFSel::TrigTauEFSel() :
  m_NTrackMin     (0),
  m_NTrackMax     (0),
  m_EMRadiusMax   (0),
  m_IsoFracMax    (0),
  m_EMFracMin     (0),
  m_PtMaxTrackMin (0),
  m_EtCalibMin    (0),
  m_EMEtMin       (0),
  m_EMRadiusMaxMultiProng   (0),
  m_IsoFracMaxMultiProng    (0),
  m_EMFracMinMultiProng     (0),
  m_PtMaxTrackMinMultiProng (0),
  m_EtCalibMinMultiProng    (0),
  m_EMEtMinMultiProng       (0),
  m_MassMax(0),
  m_DrTrkAvgMax(0),
  m_DrTrkAvgMaxMultiProng(0),
  m_LowEtOverPtLeadTrkSP(0),
  m_LowEtOverPtLeadTrkMP(0),
  m_HighEtOverPtLeadTrkSP(0),
  m_HighEtOverPtLeadTrkMP(0),
  m_ETThresh(-1),
  _debug(false),
  menu(TrigTauFlags::SECONDATTEMPT),
  m_histManager(0)

{}

TrigTauEFSel::TrigTauEFSel( const TrigTauEFSel& other) :
  m_NTrackMin(               other.m_NTrackMin),
  m_NTrackMax(               other.m_NTrackMax),
  m_EMRadiusMax(             other.m_EMRadiusMax),
  m_IsoFracMax(              other.m_IsoFracMax),
  m_EMFracMin(               other.m_EMFracMin),
  m_PtMaxTrackMin(           other.m_PtMaxTrackMin),
  m_EtCalibMin(              other.m_EtCalibMin),
  m_EMEtMin(                 other.m_EMEtMin),
  m_EMRadiusMaxMultiProng(   other.m_EMRadiusMaxMultiProng),
  m_IsoFracMaxMultiProng(    other.m_IsoFracMaxMultiProng),
  m_EMFracMinMultiProng(     other.m_EMFracMinMultiProng),
  m_PtMaxTrackMinMultiProng( other.m_PtMaxTrackMinMultiProng),
  m_EtCalibMinMultiProng(    other.m_EtCalibMinMultiProng),
  m_EMEtMinMultiProng(       other.m_EMEtMinMultiProng),
  m_MassMax(                 other.m_MassMax),
  m_DrTrkAvgMax(             other.m_DrTrkAvgMax),
  m_DrTrkAvgMaxMultiProng(   other.m_DrTrkAvgMaxMultiProng),
  m_LowEMRadSP(              other.m_LowEMRadSP),
  m_LowEMRadMP(              other.m_LowEMRadMP),
  m_LowTrkAvgDistSP(         other.m_LowTrkAvgDistSP),
  m_LowTrkAvgDistMP(         other.m_LowTrkAvgDistMP),
  m_LowEtOverPtLeadTrkSP(    other.m_LowEtOverPtLeadTrkSP),
  m_LowEtOverPtLeadTrkMP(    other.m_LowEtOverPtLeadTrkMP),
  m_HighEtOverPtLeadTrkSP(   other.m_HighEtOverPtLeadTrkSP),
  m_HighEtOverPtLeadTrkMP(   other.m_HighEtOverPtLeadTrkMP),
  m_ETThresh(                other.m_ETThresh),
  _debug(                    other._debug),
  menu(                      other.menu),
  m_histManager(             new TrigTauHistogram( *m_histManager))
{}

TrigTauEFSel::~TrigTauEFSel()
{
  if( m_histManager ) { delete m_histManager; m_histManager=0;}
}


void
TrigTauEFSel::setCuts(int NTrackMin, int NTrackMax, float EMRadiusMax,
		      float IsoFracMax, float EMFracMin, float PtMaxTrackMin,
		      float EtCalibMin, float EMEtMin,
		      float EMRadiusMaxMultiProng,
		      float IsoFracMaxMultiProng, float EMFracMinMultiProng, float PtMaxTrackMinMultiProng,
		      float EtCalibMinMultiProng, float EMEtMinMultiProng, float MassMax,
                      float DrTrkAvgMax, float DrTrkAvgMaxMultiProng) {

  m_NTrackMin = NTrackMin;
  m_NTrackMax = NTrackMax;
  m_EMRadiusMax = EMRadiusMax;
  m_IsoFracMax = IsoFracMax;
  m_EMFracMin = EMFracMin;
  m_PtMaxTrackMin = PtMaxTrackMin;
  m_EtCalibMin = EtCalibMin;
  m_EMEtMin = EMEtMin;
  
  // this will switch between the old and the new pt parametrized cuts
  m_ETThresh = -1;
  
  if(EMRadiusMaxMultiProng<0)   m_EMRadiusMaxMultiProng = EMRadiusMax; 
  else                          m_EMRadiusMaxMultiProng = EMRadiusMaxMultiProng;
  if(IsoFracMaxMultiProng<0)    m_IsoFracMaxMultiProng = IsoFracMax; 
  else                          m_IsoFracMaxMultiProng = IsoFracMaxMultiProng;
  if(EMFracMinMultiProng<0)     m_EMFracMinMultiProng = EMFracMin; 
  else                          m_EMFracMinMultiProng = EMFracMinMultiProng;
  if(PtMaxTrackMinMultiProng<0) m_PtMaxTrackMinMultiProng = PtMaxTrackMin; 
  else                          m_PtMaxTrackMinMultiProng = PtMaxTrackMinMultiProng;
  if(EtCalibMinMultiProng<0)    m_EtCalibMinMultiProng = EtCalibMin;
  else                          m_EtCalibMinMultiProng = EtCalibMinMultiProng;
  if(EMEtMinMultiProng<0)       m_EMEtMinMultiProng = EMEtMin;
  else                          m_EMEtMinMultiProng = EMEtMinMultiProng;

  m_MassMax = MassMax;

  m_DrTrkAvgMax = DrTrkAvgMax;
  if(DrTrkAvgMaxMultiProng<0)   m_DrTrkAvgMaxMultiProng = DrTrkAvgMax;
  else                          m_DrTrkAvgMaxMultiProng = DrTrkAvgMaxMultiProng;
}

// takes ownership of TrigTauSafeCutFunction objects
void
TrigTauEFSel::setParamSafeCuts( const CutFunctor& LowEMRadSP,
                                const CutFunctor& LowEMRadMP,
                                const CutFunctor& LowTrkAvgDistSP,
                                const CutFunctor& LowTrkAvgDistMP,
                                float LowEtOverPtLeadTrkSP,
                                float LowEtOverPtLeadTrkMP,
                                float HighEMRadSP,
                                float HighEMRadMP,
                                float HighTrkAvgDistSP,
                                float HighTrkAvgDistMP,
                                float HighEtOverPtLeadTrkSP,
                                float HighEtOverPtLeadTrkMP,
                                int NTrackMin,
                                int NTrackMax,
                                float EtCalibMinSP,
                                float EtCalibMinMP,
                                float ETThresh)
{
  m_LowEMRadSP            = LowEMRadSP;
  m_LowEMRadMP            = LowEMRadMP;
  m_LowTrkAvgDistSP       = LowTrkAvgDistSP;
  m_LowTrkAvgDistMP       = LowTrkAvgDistMP;
  m_LowEtOverPtLeadTrkSP  = LowEtOverPtLeadTrkSP;
  m_LowEtOverPtLeadTrkMP  = LowEtOverPtLeadTrkMP;
  m_EMRadiusMax           = HighEMRadSP;
  m_EMRadiusMaxMultiProng = HighEMRadMP;
  m_DrTrkAvgMax           = HighTrkAvgDistSP;
  m_DrTrkAvgMaxMultiProng = HighTrkAvgDistMP;
  m_HighEtOverPtLeadTrkSP = HighEtOverPtLeadTrkSP;
  m_HighEtOverPtLeadTrkMP = HighEtOverPtLeadTrkMP;
  m_NTrackMin             = NTrackMin;
  m_NTrackMax             = NTrackMax;
  m_EtCalibMin            = EtCalibMinSP;
  if( EtCalibMinMP < 0)
      m_EtCalibMinMultiProng = EtCalibMinSP;
  else
      m_EtCalibMinMultiProng = EtCalibMinMP;
  m_ETThresh              = ETThresh;
}

                           

void
TrigTauEFSel::print() const {

  if (menu != TrigTauFlags::SECONDATTEMPT) {
    cout << "LVL2 selection based on trigger decisision bits for " 
	 << TrigTauFlags::menuName(menu) << endl;
    return;
  }

  cout << " EF cuts :"    << endl;

  if (m_NTrackMin > 1.e-10) {
    cout <<" NTrack  >= "   << m_NTrackMin     << endl;
  } else {
    cout <<" NTrackMin: not applied" << endl;
  }

  if (m_NTrackMax > 1.e-10) {
  cout <<" NTrack  <= "   << m_NTrackMax     << endl;
  } else {
    cout <<" NTrackMax: not applied" << endl;
  }

  if (m_EMRadiusMax > 1.e-10) {
    cout <<" EMRadius <= "   << m_EMRadiusMax   << " (MultiProng : "<<m_EMRadiusMaxMultiProng <<")"<< endl;
  } else {
    cout <<" EMRadius:  not applied" << endl;
  }

  if (m_IsoFracMax > 1.e-10) {
    cout <<" IsoFrac  <= "   << m_IsoFracMax     << " (MultiProng : "<< m_IsoFracMaxMultiProng<<")"<< endl;
  } else {
    cout <<" IsoFrac:   not applied" << endl;
  }

  if (m_EMFracMin > 1.e-10) {
    cout <<" EMFrac   >= "   << m_EMFracMin      << " (MultiProng : "<< m_EMFracMinMultiProng<<")"<< endl;
  } else {
    cout <<" EMFrac:    not applied" << endl;
  }

  if (m_PtMaxTrackMin > 1.e-10) {
    cout <<" PtMaxTrack >= " << m_PtMaxTrackMin  << " (MultiProng : "<< m_PtMaxTrackMinMultiProng <<")"<< endl;
  } else {
    cout <<" PtMaxTrack not applied" << endl;
  }

  if (m_EtCalibMin > 1.e-10) {
    cout <<" EtCalib  >= "   << m_EtCalibMin     << " (MultiProng : "<< m_EtCalibMinMultiProng<<")"<< endl;
  } else {
    cout <<" EtCalib:   not applied" << endl;
  }

  if (m_EMEtMin > 1.e-10) {
    cout <<" EMEt     >= "   << m_EMEtMin       << " (MultiProng : "<< m_EMEtMinMultiProng<<")" << endl; 
  } else {
    cout <<" EMEt:      not applied" << endl;
  }

  if( m_MassMax > 1.e-10 ){
    cout <<" MassMax     <= "   << m_MassMax        << endl; 
  } else {
    cout <<" MassMax:      not applied" << endl;
  }

  if( m_DrTrkAvgMax > 0)
  {
    cout <<" DrTrkAvgMax <= "   << m_DrTrkAvgMax    << endl;
  }
  else
  {
    cout <<" DrTrkAvgMax:  not applied" << endl;
  }
}



void TrigTauEFSel::setHistDebug(TrigTauHistogram::HISTTYPE type)
{
  if( type != TrigTauHistogram::NONE ){
    //gROOT->cd();
    if( m_histManager ) delete m_histManager;
    m_histManager = new TrigTauHistogram(type);
    m_histManager->book("baseEF","Before any cut");
    m_histManager->book("etCalibEF","EF: Cut on  EtCalib");
    m_histManager->book("ntrkMinEFID","EF: Cut on  NtrkMin reconstructed by EFID");
    m_histManager->book("ntrkMinEF","EF: Cut on  NtrkMin ");
    m_histManager->book("ntrkMaxEF","EF: Cut on  NtrkMax");
    m_histManager->book("emradEF","EF: Cut on EMRadius ");
    m_histManager->book("isoFracEF","EF: Cut on IsoFraction ");
    m_histManager->book("emFracEF","EF: Cut on  EMFraction");
    m_histManager->book("leadPtEF","EF: Cut on  leading track pT");
    m_histManager->book("emEtEF","EF: Cut on  EM Et");
    m_histManager->book("massEF","EF: Cut on  Mass");
    m_histManager->book("DrTrkAvgEF","EF: Cut on  trk avg dist");
    m_histManager->book("EtOverPtTrk","EF: Cut on  EtOverPtTrk");

  }else{
      if( m_histManager ) { delete m_histManager; m_histManager=0;}
  } 
}


bool
TrigTauEFSel::passed(const TrigTauRecEFObject& tau) const {
  if (menu != TrigTauFlags::SECONDATTEMPT) return tau.m_tags.passedL2(menu);
  
  if( m_ETThresh > 0) return cutWordParamSafeCuts(tau) == 0;
  
  return (cutWord(tau) == 0);
}

unsigned long
TrigTauEFSel::cutWord(const TrigTauRecEFObject& tau) const {
  // return new cuts if Et threshold was set
  if( m_ETThresh > 0)
      return cutWordParamSafeCuts( tau);
  
  if( _debug ) cout << " In TrigTauEFSel::cutWord \n";
  unsigned long word = 0;
  float varHist = 0;
  if( m_histManager)
    {
      if( m_histManager->type() == TrigTauHistogram::PTHIST ) varHist = tau.Pt();
      else if( m_histManager->type() == TrigTauHistogram::ETAHIST ) varHist = tau.Eta();
      else if( m_histManager->type() == TrigTauHistogram::PHIHIST ) varHist = tau.Phi();
    }
  if( m_histManager) m_histManager->fill(0, varHist);

  if( _debug ) cout << "    hist are prepared \n";
  float emFrac = 0.;
  float ptMax = 999999.;
  float emEt = 0.;

  if (tau.m_etEMCalib+tau.m_etHadCalib>0.)
    emFrac = tau.m_etEMCalib/(tau.m_etEMCalib + tau.m_etHadCalib);
  if( _debug) cout << " emFrac = " << emFrac << endl;
  if( _debug ) cout << " Ntrk = " << tau.nLooseTracks() << " recorded " << tau.m_nLooseTracksRecord << endl;
  if( _debug ) cout << " leading track " << tau.m_leadPt << endl;
  if ( tau.m_nLooseTracksRecord > 0 )  ptMax = tau.m_leadPt;

  emEt = tau.m_energy / cosh(tau.m_caloEta);

  if (_debug) cout << "TauEF:  eta="<<tau.Eta() <<", phi="<<tau.Phi() << " RoIWord="
		   <<  std::hex <<tau.m_roiWord << std::dec <<endl;
  if( _debug ) { cout <<" EF : nTrk=" <<  tau.m_nLooseTracksRecord << ", EmRad="<< tau.m_EMRadius<<", isoFrac="
                      <<tau.m_isoFrac << ", EMFrac="<<emFrac<<"\n LeadPt="<<ptMax<<", Et"<< tau.Et()
                      <<", emET="<<emEt<<endl;}

  //unsigned int nTrk = tau.nLooseTracks();
  unsigned int nTrk = tau.m_nLooseTracksRecord;
  const TrigTauTrackCollection  * EFID = (const TrigTauTrackCollection  *) tau.r_EFIDtrackCollection.GetObject(); 
  unsigned int nTrkEFID = 1;

  // no check if EFID info is available and if the links are recorded (not available if avoidTrigDecision was set in TTPAThena
  if( EFID &&  int(tau.nLooseTracks()) == tau.m_nLooseTracksRecord ) 
    {
      nTrkEFID = EFID->nTracks();
    }else{
    if( _debug) cout << "No EFID track info recorded, proceed without \n";
  }
  

  /*
  static bool first = true;
  if( first ){
    cout << "WARNING Use all EFID tracks instead of selected tracks\n";
    first = false;
  }
  nTrk = tau.nLooseTracks()+ tau.nRejectedEFTracks();
  */


  float etcorr = tau.Et(); 
  // float etcorr = tauRec_et_corr(tau.m_etHadCalib + tau.m_etEMCalib,
  //			tau.Eta(), tau.nLooseTracks() );
  if (_debug) cout << "Et="<<etcorr << " ";
  if( m_EtCalibMin  > 1.e-10 && nTrk <= 1 && m_EtCalibMin > etcorr ) {
    if (_debug) cout << " - reject etcorr" << endl;
    word |= EFCUT_ETCORR;
  }
  if( m_EtCalibMinMultiProng  > 1.e-10  && nTrk > 1 && m_EtCalibMinMultiProng > etcorr ) {
    if (_debug) cout << " - reject etcorr MultiProng" << endl;
    word |= EFCUT_ETCORR;
  }
  if( m_histManager && word==0) m_histManager->fill(1, varHist);

  

  if (_debug) cout << "Ntrk="<< nTrk << " EFID Ntrk = " << nTrkEFID << " ";
  if( m_NTrackMin   > 1.e-10 && m_NTrackMin   >  nTrkEFID ) {
      if (_debug) cout << " - reject min nTr due to EFID" << endl;
      word |= EFCUT_EFIDNONZERO;
    }    
  if( m_histManager && word==0) m_histManager->fill(2, varHist);

  if(  m_NTrackMin   > 1.e-10 && m_NTrackMin >nTrk ) {
    if (_debug) cout << " - reject min nTr" << endl;
    word |= EFCUT_NTRKMIN;    
  }

  if( m_histManager && word==0) m_histManager->fill(3, varHist);

  if( m_NTrackMax   > 1.e-10 && m_NTrackMax < nTrk ) {
    if (_debug) cout << " - reject max nTr" << endl;
    word |= EFCUT_NTRKMAX;
  }
  if( m_histManager && word==0) m_histManager->fill(4, varHist);


  if (_debug) cout << "Emrad="<<tau.m_EMRadius << " ";
  if( m_EMRadiusMax > 1.e-10 && nTrk <= 1 && m_EMRadiusMax < tau.m_EMRadius ) {
    if (_debug) cout << " - reject EMRadius" << endl;
    word |= EFCUT_EMRAD;
  }
  if( m_EMRadiusMaxMultiProng > 1.e-10 && nTrk > 1 && m_EMRadiusMaxMultiProng < tau.m_EMRadius ) {
    if (_debug) cout << " - reject EMRadius MultiProng" << endl;
    word |= EFCUT_EMRAD;
  }



  if( m_histManager && word==0) m_histManager->fill(5, varHist);

  if (_debug) cout << "IsoFrac="<<tau.m_isoFrac << " ";
  if( m_IsoFracMax  > 1.e-10 && nTrk <= 1&& m_IsoFracMax < tau.m_isoFrac ) {
    if (_debug) cout << " - reject IsoFrac" << endl;
    word |= EFCUT_ISOFRAC;
  }
  if( m_IsoFracMaxMultiProng  > 1.e-10  && nTrk > 1 && m_IsoFracMaxMultiProng < tau.m_isoFrac ) {
    if (_debug) cout << " - reject IsoFrac MultiProng" << endl;
    word |= EFCUT_ISOFRAC;
  }

  if( m_histManager && word==0) m_histManager->fill(6, varHist);

  if (_debug) cout << "EmFrac="<<emFrac << " ";
  if( m_EMFracMin   > 1.e-10 && nTrk <= 1 && m_EMFracMin > emFrac ) {
    if (_debug) cout << " - reject emFrac" << endl;
    word |= EFCUT_EMFRAC;
  }
  if( m_EMFracMinMultiProng   > 1.e-10 && nTrk > 1 && m_EMFracMinMultiProng > emFrac ) {
    if (_debug) cout << " - reject emFrac MultiProng" << endl;
    word |= EFCUT_EMFRAC;
  }


  if( m_histManager && word==0) m_histManager->fill(7, varHist);

  if (_debug) cout << "LeadPt="<<ptMax << " ";
  if( m_PtMaxTrackMin > 1.e-10 && nTrk <= 1 && m_PtMaxTrackMin > ptMax ) {
    if (_debug) cout << " - reject ptMax" << endl;
    word |= EFCUT_LEADPT;
  }
  if( m_PtMaxTrackMinMultiProng > 1.e-10  && nTrk > 1 && m_PtMaxTrackMinMultiProng > ptMax ) {
    if (_debug) cout << " - reject ptMax MultiProng" << endl;
    word |= EFCUT_LEADPT;
  }
  if( m_histManager && word==0) m_histManager->fill(8, varHist);


  //if( m_EtCalibMin  > 1.e-10 && m_EtCalibMin > tau.Et() ) return false;
  if (_debug) cout << "EMEt="<< emEt << " ";
  if( m_EMEtMin     > 1.e-10  && nTrk <= 1 && m_EMEtMin > emEt ) {
    if (_debug) cout << " - reject emEt" << endl;
    word |= EFCUT_EMET;
  }
  if( m_EMEtMinMultiProng     > 1.e-10  && nTrk > 1 && m_EMEtMinMultiProng > emEt ) {
    if (_debug) cout << " - reject emEt MultiProng" << endl;
    word |= EFCUT_EMET;
  }

  if( m_histManager && word==0) m_histManager->fill(9, varHist);

  if( m_MassMax     > 1.e-10  && nTrk > 1 ){

    TLorentzVector p1(0,0,0,0), p2(0,0,0,0), p3(0,0,0,0);
    for (unsigned int itrk = 0; itrk < tau.nLooseTracks(); ++itrk){
      const TrigTauTrack * itr = &tau.looseTrack(itrk);
      float pt = itr->Pt();
      if(pt > p1.Pt() ){
	p3 = p2;
	p2 = p1;
	p1 = *itr;
      }else if( pt > p2.Pt() ){
	p3 = p2;
	p2 = *itr;
      }else if( pt > p3.Pt() ){
	p3 = *itr;
      }
    }
    p1 += p2;
    p1 += p3;

    if( (nTrk == 2 && 0.75*m_MassMax < p1.M()) || ( m_MassMax < p1.M() ) ) {
      if (_debug) cout << " - reject Mass" << endl;
      word |= EFCUT_MASS;
    }
  }
  
  if( m_histManager && word==0) m_histManager->fill(10, varHist);

  if (_debug) cout << "DrTrkAvg="<< tau.m_trkAvgDist << " ";
  if( m_DrTrkAvgMax > 1.e-10 && nTrk <= 1 && tau.m_trkAvgDist > m_DrTrkAvgMax) {
    if (_debug) cout << " - reject DrTrkAvg" << endl;
    word |= EFCUT_DRTRKAVG;
  }
  if( m_DrTrkAvgMaxMultiProng > 1.e-10  && nTrk > 1 && tau.m_trkAvgDist > m_DrTrkAvgMaxMultiProng) {
    if (_debug) cout << " - reject DrTrkAvg MultiProng" << endl;
    word |= EFCUT_DRTRKAVG;
  }

  if( m_histManager && word==0) m_histManager->fill(11, varHist);

  if (_debug && (word==0)) cout << " accepted" << endl;
  return word;
}

unsigned long
TrigTauEFSel::cutWordParamSafeCuts(const TrigTauRecEFObject& tau) const
{
  if( _debug ) cout << " In TrigTauEFSel::cutWordParamSafeCuts \n";
  unsigned long word = 0;
  float varHist = 0;
  if( m_histManager)
  {
    if( m_histManager->type() == TrigTauHistogram::PTHIST )
        varHist = tau.Pt();
    else if( m_histManager->type() == TrigTauHistogram::ETAHIST )
        varHist = tau.Eta();
    else if( m_histManager->type() == TrigTauHistogram::PHIHIST )
        varHist = tau.Phi();
  }
  if( m_histManager) m_histManager->fill(0, varHist);
  
  if( _debug ) cout << "    hist are prepared \n";
  
  float emFrac = 0.;
  float emEt = tau.m_energy / cosh(tau.m_caloEta);
  unsigned int nTrk = tau.m_nSeedCaloTracks;
  float et = tau.Et();
  bool singleProng = (nTrk <= 1);
  
  if (tau.m_etEMCalib+tau.m_etHadCalib>0.)
      emFrac = tau.m_etEMCalib/(tau.m_etEMCalib + tau.m_etHadCalib);
  
  if( _debug )
      cout << " EF: nTrk=" << nTrk << ","
           << " EmRad=" << tau.m_EMRadius << ","
           << " isoFrac=" << tau.m_isoFrac << ","
           << " EMFrac=" << emFrac << endl
           << " Et=" << et << ","
           << " emET=" << emEt << endl
           << " dRTrkAvg=" << tau.m_trkAvgDist << ","
           << " eta=" << tau.Eta() <<", phi=" << tau.Phi()
           << " RoIWord=" <<  std::hex << tau.m_roiWord << std::dec << endl;
  
  // Cut1: EtCalib
  if( singleProng)
  {
    if( m_EtCalibMin > 0 && m_EtCalibMin > et )
    {
      if (_debug) cout << " - reject etcorr" << endl;
      word |= EFCUT_ETCORR;
    }
  }
  else
  {
    if( m_EtCalibMinMultiProng  > 0 && m_EtCalibMinMultiProng > et)
    {
      if (_debug) cout << " - reject etcorr MultiProng" << endl;
      word |= EFCUT_ETCORR;
    }
  }
  if( m_histManager && word==0) m_histManager->fill(1, varHist);

  // Cut2: nTrkEFID NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(2, varHist);

  // Cut3: nTrkMin
  if( m_NTrackMin > 0 && m_NTrackMin > nTrk)
  {
    if (_debug) cout << " - reject min nTr" << endl;
    word |= EFCUT_NTRKMIN;    
  }
  if( m_histManager && word==0) m_histManager->fill(3, varHist);
  
  // Cut4: nTrkMax
  if( m_NTrackMax > 0 && m_NTrackMax < nTrk)
  {
    if (_debug) cout << " - reject max nTr" << endl;
    word |= EFCUT_NTRKMAX;
  }
  if( m_histManager && word==0) m_histManager->fill(4, varHist);

  // Cut5: EMRadius
  float EMRadCut = -1;
  if( singleProng)
  {
    if( et > m_ETThresh)
        EMRadCut = m_EMRadiusMax;
    else
        EMRadCut = m_LowEMRadSP.eval( et/1000.);
  }
  else
  {
    if( et > m_ETThresh)
        EMRadCut = m_EMRadiusMaxMultiProng;
    else
        EMRadCut = m_LowEMRadMP.eval( et/1000.);
  }
  if( EMRadCut > 0 && EMRadCut < tau.m_EMRadius)
  {
    if (_debug) cout << " - reject EMRadius" << endl;
    word |= EFCUT_EMRAD;
  }
  if( m_histManager && word==0) m_histManager->fill(5, varHist);
  
  // Cut6: isoFrac NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(6, varHist);

  // Cut7: emFrac NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(7, varHist);

  // Cut8: leadPt NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(8, varHist);

  // Cut9: emEt NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(9, varHist);

  // Cut10: mass NOT IMPLEMENTED
  if( m_histManager && word==0) m_histManager->fill(10, varHist);

  // Cut11: DrTrkAvg
  float DrTrkAvgDistCut = -1;
  if( singleProng)
  {
    if( et > m_ETThresh)
        DrTrkAvgDistCut = m_DrTrkAvgMax;
    else
        DrTrkAvgDistCut = m_LowTrkAvgDistSP.eval( et/1000.);
  }
  else
  {
    if( et > m_ETThresh)
        DrTrkAvgDistCut = m_DrTrkAvgMaxMultiProng;
    else
        DrTrkAvgDistCut = m_LowTrkAvgDistMP.eval( et/1000.);
  }
  if( DrTrkAvgDistCut > 0 && tau.m_trkAvgDist > DrTrkAvgDistCut)
  {
    if (_debug) cout << " - reject DrTrkAvg" << endl;
    word |= EFCUT_DRTRKAVG;
  }
  if( m_histManager && word==0) m_histManager->fill(11, varHist);

  // Cut12: EtOverPtTrk
  float EtOverPtTrkCut = -1;
  if( singleProng)
  {
    if( et > m_ETThresh)
        EtOverPtTrkCut = m_HighEtOverPtLeadTrkSP;
    else
        EtOverPtTrkCut = m_LowEtOverPtLeadTrkSP;
  }
  else
  {
    if( et > m_ETThresh)
        EtOverPtTrkCut = m_HighEtOverPtLeadTrkMP;
    else
        EtOverPtTrkCut = m_LowEtOverPtLeadTrkMP;
  }
  if( EtOverPtTrkCut > 0 && tau.m_etOverPtLeadTrk > EtOverPtTrkCut)
  {
    if (_debug) cout << " - reject EtOverPtTrk" << endl;
    word |= EFCUT_ETOVERPTTRK;
  }
  if( m_histManager && word==0) m_histManager->fill(12, varHist);
  
  // end
  if (_debug && (word==0)) cout << " accepted" << endl;
  return word;
}
