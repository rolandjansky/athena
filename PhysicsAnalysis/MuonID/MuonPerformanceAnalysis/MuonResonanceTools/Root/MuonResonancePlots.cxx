/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonResonanceTools/MuonResonancePlots.h"
#include "TLorentzVector.h"
#include <iostream>
#ifndef ROOTCORE
#include "GaudiKernel/IToolSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#endif
#include "xAODEventInfo/EventInfo.h"

#include <iostream>

void MuonResonancePlots::initializePlots(){
  std::cout<<"booking MuonResonancePlots"<<std::endl;
  BookPlots();
}

void MuonResonancePlots::setBinning(std::map<std::string, std::vector<double> > bins ){ m_binning=bins;}

void MuonResonancePlots::Binning1D(TH1*& histo, std::string hname){

  if(histo) histo=0;
  std::map<std::string, std::vector<double> >::iterator it;
  for(it=m_binning.begin(); it!=m_binning.end(); it++){
    if(it->first != hname) continue;

    if(m_binning[hname].size()==3) 
      histo = Book1D(m_prefix+hname+m_suffix, m_prefix+hname+m_suffix,  m_binning[hname][0],m_binning[hname][1],m_binning[hname][2], false);
  }  
}

void MuonResonancePlots::Binning2D(TH2*& histo, std::string hname){

  if(histo) histo=0;
  std::map<std::string, std::vector<double> >::iterator it;
  for(it=m_binning.begin(); it!=m_binning.end(); it++){
    if(it->first != hname) continue;

    if(m_binning[hname].size()==6) 
      histo = Book2D(m_prefix+hname+m_suffix, m_prefix+hname+m_suffix, 
m_binning[hname][0],m_binning[hname][1],m_binning[hname][2],m_binning[hname][3],m_binning[hname][4],m_binning[hname][5], false);  
  }
}

void MuonResonancePlots::Fill1D(TH1*& histo, float x, float w){
 
  if(!histo) return;
  histo->Fill(x, w);
}

void MuonResonancePlots::Fill2D(TH2*& histo, float x, float y, float w){

  if(!histo) return;
  histo->Fill(x, y, w);
}

void MuonResonancePlots::FillTwice(TH1*& histo, float x1, float x2, float w){

  if(!histo) return;
  float x[2] = {x1, x2};
  for(int i=0; i<2; i++) histo->Fill(x[i], w);
}


void MuonResonancePlots::BookPlots(){

  //general muon properties
  Binning1D(mu_1stAuthor, "mu_1stAuthor");                  //primary author of leading muon
  Binning1D(mu_2ndAuthor, "mu_2ndAuthor");                  //primary author of subleading muon
  
  Binning1D(mu_1stPt  ,"mu_1stPt");                         //pt of leading muon
  Binning1D(mu_2ndPt  ,"mu_2ndPt");                         //pt of subleading muon
  Binning1D(mu_avPt   ,"mu_avPt");                          //average muon pt
  Binning1D(mu_1stPhi ,"mu_1stPhi");                        //phi of leading muon
  Binning1D(mu_2ndPhi ,"mu_2ndPhi");                        //phi of subleading muon
  Binning1D(mu_1stEta ,"mu_1stEta");                        //eta of leading muon
  Binning1D(mu_2ndEta ,"mu_2ndEta");                        //eta of subleading muon
  	   
  Binning1D(Mmumu     ,"Mmumu");                            //Mmumu invariant mass
  Binning1D(Z_pt      ,"Z_pt");                             //vectorboson pt
  Binning1D(Z_phi     ,"Z_phi");                            //vectorboson phi

  Binning2D(h_Zpt_mu_1stPt     ,"h_Zpt_mu_1stPt");                   //Z pt vs. pt of leading muon
  Binning2D(h_mu_1stPt_1stPhi  ,"h_mu_1stPt_1stPhi");                //pt vs. phi of leading muon
  Binning2D(h_mu_2ndPt_2ndPhi  ,"h_mu_2ndPt_2ndPhi");                //pt vs. phi of subleading muon
  Binning2D(h_Zpt_mu_avPt      ,"h_Zpt_mu_avPt");                    //Z pt vs. average pt of muons
	 
  Binning2D(h_Zm_1stPhi        ,"h_Zm_1stPhi");                      //Mmumu vs. phi of leading muon
  Binning2D(h_Zm_2ndPhi        ,"h_Zm_2ndPhi");                      //Mmumu vs. phi of subleading muon
  Binning2D(h_Zm_1stEta        ,"h_Zm_1stEta");                      //Mmumu vs. eta of leading muon 
  Binning2D(h_Zm_2ndEta        ,"h_Zm_2ndEta");                      //Mmumu vs. eta of subleading muon

  Binning2D(h_Zm_Eta,"h_Zm_Eta");          //Mmumu vs. eta - both muons in the same eta bin 

  Binning2D(h_Zm_1stEta01,"h_Zm_1stEta01");

  Binning2D(h_Zm_1stPt         ,"h_Zm_1stPt");                       //Mmumu vs. pt of leading muon 
  Binning2D(h_Zm_2ndPt         ,"h_Zm_2ndPt");                       //Mmumu vs. pt of subleading muon 
  Binning2D(h_Zm_mu_avPt       ,"h_Zm_mu_avPt");                     //Mmumu vs. average pt of muons
  Binning2D(h_Zm_Pexp          ,"h_Zm_Pexp");                        //Mmumu vs. p* (as defined in ATL-COM-MUON 2014-001)
  
  //correlation plots (truth-info)
  Binning2D(h_Zm_1stPhi_truth    ,"h_Zm_1stPhi_truth");            //Mmumu(truth) vs. phi(truth) of leading muon 
  Binning2D(h_Zm_2ndPhi_truth    ,"h_Zm_2ndPhi_truth");            //Mmumu(truth) vs. phi(truth) of subleading muon
  Binning2D(h_Zm_1stEta_truth    ,"h_Zm_1stEta_truth");            //Mmumu(truth) vs. eta(truth) of leading muon
  Binning2D(h_Zm_2ndEta_truth    ,"h_Zm_2ndEta_truth");            //Mmumu(truth) vs. eta(truth) of subleading muon
  Binning2D(h_Zm_1stPt_truth     ,"h_Zm_1stPt_truth");             //Mmumu(truth) vs. pt(truth) of leading muon 
  Binning2D(h_Zm_2ndPt_truth     ,"h_Zm_2ndPt_truth");             //Mmumu(truth) vs. pt(truth) of subleading muon
  Binning2D(h_Zm_mu_avPt_truth   ,"h_Zm_mu_avPt_truth");           //Mmumu vs. average pt(truth) of muons
  Binning2D(h_Zm_Pexp_truth      ,"h_Zm_Pexp_truth");              //Mmumu(truth) vs. p*(truth)

  //resolution plots
  Binning2D(Res_mu_1stPt_2D      ,"Res_mu_1stPt_2D");             //pt-pt(truth) vs. pt for leading muon
  Binning2D(Res_mu_2ndPt_2D      ,"Res_mu_2ndPt_2D");             //pt-pt(truth) vs. pt for subleading muon
  Binning2D(Res_mu_1stPhi_2D     ,"Res_mu_1stPhi_2D");            //phi-phi(truth) vs. phi for leading muon
  Binning2D(Res_mu_2ndPhi_2D     ,"Res_mu_2ndPhi_2D");            //phi-phi(truth) vs. phi for subleading muon
  Binning2D(Res_mu_1stEta_2D     ,"Res_mu_1stEta_2D");            //eta-eta(truth) vs. eta for leading muon
  Binning2D(Res_mu_2ndEta_2D     ,"Res_mu_2ndEta_2D");            //eta-eta(truth) vs. eta for subleading muon
  
  Binning2D(Res_Zm_2D            ,"Res_Zm_2D");                 //Mmumu-Mmumu(truth) vs. Mmumu
  Binning2D(Res_Zm_Eta_2D        ,"Res_Zm_Eta_2D");             //Mmumu-Mmumu(truth) vs. eta of leading muon
  Binning2D(Res_Zm_Phi_2D        ,"Res_Zm_Phi_2D");             //Mmumu-Mmumu(truth) vs. phi of leading muon
  Binning2D(Res_Zm_Pt_2D         ,"Res_Zm_Pt_2D");              //Mmumu-Mmumu(truth) vs. pt of leading muon
  Binning2D(Res_Zm_Pexp_2D       ,"Res_Zm_Pexp_2D");            //Mmumu-Mmumu(truth) vs. p*
  Binning2D(Res_Zpt_2D           ,"Res_Zpt_2D");                //Zpt-Zpt(truth) vs. Zpt
  Binning2D(Res_Zphi_2D          ,"Res_Zphi_2D");               //Zphi-Zphi(truth) vs. Zphi 

  //Chi^2 of tracks
  Binning1D(ChiSquared, "ChiSquared");                           //Chi^2/DoF for combined track of leading muon 
 
  //momentum imbalance plots
  Binning1D(p_pTRUE,    "p_pTRUE");                              //pt-pt(truth)
  Binning1D(pID_pME,    "pID_pME");                              //momentum inbalance
  
  //eta-phi maps
  Binning2D(Z_m_etaphi           ,"Z_m_etaphi");               //eta-phi map of Mmumu
  Binning2D(DeltaZ_m_etaphi      ,"DeltaZ_m_etaphi");          //eta-phi map of DeltaM(Z)/M(Z) as in ATL-COM-MUON 2014-001 
  Binning2D(DeltaZ_m_q_etaphi    ,"DeltaZ_m_q_etaphi");        //eta-phi map of q*DeltaM(Z)/M(Z) (combined track)
  Binning2D(p_pTRUE_etapt        ,"p_pTRUE_etapt");            //eta-pt  map of pt-pt(truth)/pt(truth)
  Binning2D(p_pTRUE_etaphi       ,"p_pTRUE_etaphi");           //eta-phi map of pt-pt(truth)/pt(truth)   
  
  return;  
}

void MuonResonancePlots::fill(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd, int trk){

  if(trk==0 && ( !mu1st.trackParticle(xAOD::Muon::CombinedTrackParticle) 
		 || !mu2nd.trackParticle(xAOD::Muon::CombinedTrackParticle) ) ) return;
  
  if(trk==1 && ( !mu1st.trackParticle(xAOD::Muon::InnerDetectorTrackParticle) 
		 || !mu2nd.trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ) ) return;

  if(trk==2 && ( !mu1st.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) 
		 || !mu2nd.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle) ) ) return;
  
  TLorentzVector V,l1,l2;
  l1 = sortedPair(getTrackTLV(mu1st, trk), getTrackTLV(mu2nd, trk)).first;
  l2 = sortedPair(getTrackTLV(mu1st, trk), getTrackTLV(mu2nd, trk)).second;
  V = l1 + l2;

  TLorentzVector V_truth,l1_truth,l2_truth;
  if(findTruthMuon(mu1st) && findTruthMuon(mu2nd)){
    l1_truth = sortedPair(findTruthMuon(mu1st)->p4(), findTruthMuon(mu2nd)->p4()).first;
    l2_truth = sortedPair(findTruthMuon(mu1st)->p4(), findTruthMuon(mu2nd)->p4()).second;
  }
  V_truth = l1_truth + l2_truth;

  float w = effWeight(mu1st) * effWeight(mu2nd);

  // bool do_print = false;
  // if(do_print) PrintVectors(l1, l2, V, l1_truth, l2_truth, V_truth, trk, w);

  // Filling histos 
  Fill1D( mu_1stAuthor, int(mu1st.author()), w);
  Fill1D( mu_2ndAuthor, int(mu2nd.author()), w);

  Fill1D( mu_1stPt,  l1.Pt()/m_fGeV, w);
  Fill1D( mu_2ndPt,  l2.Pt()/m_fGeV, w);
  Fill1D( mu_avPt,   (l1.Pt()+l2.Pt())*0.5/m_fGeV, w);
  Fill1D( mu_1stPhi, l1.Phi(), w);
  Fill1D( mu_2ndPhi, l2.Phi(), w);
  Fill1D( mu_1stEta, l1.Eta(), w);
  Fill1D( mu_2ndEta, l2.Eta(), w);

  Fill1D( Mmumu, V.M()/m_fGeV, w);
  Fill1D( Z_pt,  V.Pt()/m_fGeV, w);
  Fill1D( Z_phi, V.Phi(), w);

  Fill2D( h_Zpt_mu_1stPt,    l1.Pt()/m_fGeV,  V.Pt()/m_fGeV, w);
  Fill2D( h_mu_1stPt_1stPhi, l1.Phi(),      l1.Pt()/m_fGeV, w);
  Fill2D( h_mu_2ndPt_2ndPhi, l2.Phi(),      l2.Pt()/m_fGeV, w);    
  Fill2D( h_Zpt_mu_avPt,     (l1.Pt()+l2.Pt())*0.5/m_fGeV, V.Pt()/m_fGeV, w); 

  Fill2D( h_Zm_1stPhi,  l1.Phi(), V.M()/m_fGeV, w);
  Fill2D( h_Zm_2ndPhi,  l2.Phi(), V.M()/m_fGeV, w);
  Fill2D( h_Zm_1stEta,  l1.Eta(), V.M()/m_fGeV, w);
  Fill2D( h_Zm_2ndEta,  l2.Eta(), V.M()/m_fGeV, w);

// BOTH MUONS IN SAME ETABIN: 20 BINS OF 0.25 FROM -2.5 TO 2.5
  for (int ie=0; ie<20; ie++){
    float xe = -2.5+0.25*ie;
    if ((l1.Eta()>xe && l1.Eta()<(xe+0.25)) && (l2.Eta()>xe && l2.Eta()<(xe+0.25))){
      Fill2D( h_Zm_Eta,  l2.Eta(), V.M()/m_fGeV, w);
    }
  }

// MASS VS ETA OF PT LEADING IN SMALL ETABIN, FROM -2.5 TO 2.5, STEP OF 0.1, 50 BINS

  Fill2D( h_Zm_1stEta01,   l1.Eta(),  V.M()/m_fGeV, w);

// MASS VS PT LEADING IN LARGE ETABIN [-2.5, -2.0, -1.0, 0.0, 1.0, 2.0, 2.5]

  Fill2D( h_Zm_1stPt,   l1.Pt()/m_fGeV,  V.M()/m_fGeV, w);

  Fill2D( h_Zm_2ndPt,   l2.Pt()/m_fGeV,  V.M()/m_fGeV, w);
  Fill2D( h_Zm_mu_avPt, (l1.Pt()+l2.Pt())*0.5/m_fGeV, V.M()/m_fGeV, w);
  Fill2D( h_Zm_Pexp,    p_star(l1, l2),             V.M()/m_fGeV, w);

  Fill2D( h_Zm_1stPhi_truth,  l1.Phi(), V_truth.M()/m_fGeV, w);
  Fill2D( h_Zm_2ndPhi_truth,  l2.Phi(), V_truth.M()/m_fGeV, w);
  Fill2D( h_Zm_1stEta_truth,  l1.Eta(), V_truth.M()/m_fGeV, w);
  Fill2D( h_Zm_2ndEta_truth,  l2.Eta(), V_truth.M()/m_fGeV, w);
  Fill2D( h_Zm_1stPt_truth,   l1.Pt()/m_fGeV,               V_truth.M()/m_fGeV, w);
  Fill2D( h_Zm_2ndPt_truth,   l2.Pt()/m_fGeV,               V_truth.M()/m_fGeV, w);  
  Fill2D( h_Zm_mu_avPt_truth, (l1.Pt()+l2.Pt())*0.5/m_fGeV, V_truth.M()/m_fGeV, w);
  Fill2D( h_Zm_Pexp_truth,    p_star(l1,l2),              V_truth.M()/m_fGeV, w); 

  Fill2D( Res_mu_1stPt_2D,  l1_truth.Pt()/m_fGeV,  l1.Pt()-l1_truth.Pt()/m_fGeV, w); 
  Fill2D( Res_mu_2ndPt_2D,  l2_truth.Pt()/m_fGeV,  l2.Pt()-l2_truth.Pt()/m_fGeV, w);
  Fill2D( Res_mu_1stPhi_2D, l1_truth.Phi(),      l1.Phi()-l1_truth.Phi(), w);
  Fill2D( Res_mu_2ndPhi_2D, l2_truth.Phi(),      l2.Phi()-l2_truth.Phi(), w);
  Fill2D( Res_mu_1stEta_2D, l1_truth.Eta(),      l1.Eta()-l1_truth.Eta(), w);
  Fill2D( Res_mu_2ndEta_2D, l2_truth.Eta(),      l2.Eta()-l2_truth.Eta(), w);

  Fill2D( Res_Zm_2D,      V_truth.M()/m_fGeV,   (V.M()-V_truth.M())/m_fGeV, w);
  Fill2D( Res_Zm_Eta_2D,  l1.Eta(),           (V.M()-V_truth.M())/m_fGeV, w);
  Fill2D( Res_Zm_Phi_2D,  l1.Phi(),           (V.M()-V_truth.M())/m_fGeV, w);
  Fill2D( Res_Zm_Pt_2D,   l1.Pt()/m_fGeV,       (V.M()-V_truth.M())/m_fGeV, w);
  Fill2D( Res_Zm_Pexp_2D, p_star(l1,l2),      (V.M()-V_truth.M())/m_fGeV, w);
  Fill2D( Res_Zpt_2D,     V_truth.Pt()/m_fGeV,  (V.Pt()-V_truth.Pt())/m_fGeV, w); 
  Fill2D( Res_Zphi_2D,    V_truth.Phi(),      V.Phi()-V_truth.Phi(), w);

  FillTwice( ChiSquared, getChiSquared(mu1st,trk), getChiSquared(mu2nd,trk), w); 
  FillTwice( p_pTRUE,    deltaPt(l1, l1_truth),    deltaPt(l2, l2_truth), w); 
  FillTwice( pID_pME,    deltaPt(getTrackTLV(mu1st, 1), getTrackTLV(mu1st, 2)), deltaPt(getTrackTLV(mu2nd, 1), getTrackTLV(mu2nd, 2)), w); 

  Fill2D( Z_m_etaphi,        l1.Eta(), l1.Phi(),        V.M()/m_fGeV);
  Fill2D( DeltaZ_m_etaphi,   l1.Eta(), l1.Phi(),        (V.M()-M0())/M0());
  Fill2D( DeltaZ_m_q_etaphi, l1.Eta(), l1.Phi(),        mu1st.charge()*(V.M()-M0())/M0()); 
  Fill2D( p_pTRUE_etapt,     l1.Eta(), l1.Pt()/m_fGeV,    deltaPt(l1, l1_truth) );
  Fill2D( p_pTRUE_etapt,     l2.Eta(), l2.Pt()/m_fGeV,    deltaPt(l2, l2_truth) ); 
  Fill2D( p_pTRUE_etaphi,    l1.Eta(), l1.Phi(),        deltaPt(l1, l1_truth) );
  Fill2D( p_pTRUE_etaphi,    l2.Eta(), l2.Phi(),        deltaPt(l2, l2_truth) );
  
  return;
}


// function to return pointer to associated truth particle
const xAOD::TruthParticle* MuonResonancePlots::findTruthMuon(const xAOD::Muon& mu){
  
  const xAOD::TruthParticle *truthMu = 0;
  if(!mu.isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) return truthMu;
  ElementLink<xAOD::TruthParticleContainer> link = mu.auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
  if(!link.isValid()) return truthMu;
  truthMu = (*link);  
  return truthMu;
}

// returns corrected TLV of track particle
TLorentzVector MuonResonancePlots::getTrackTLV(const xAOD::Muon& mu, int type){
  
  TLorentzVector v;
  const xAOD::TrackParticle *cb_ = mu.trackParticle(xAOD::Muon::CombinedTrackParticle);
  const xAOD::TrackParticle *id_ = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle *me_ = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
  if(type>-1 && (!cb_ || !id_ || !me_)) return v;

  if(id_ && me_) {
    float pt_id = id_->pt();
    float pt_me = me_->pt();
    if(mu.isAvailable< float >("InnerDetectorPt")) pt_id = mu.auxdata< float >("InnerDetectorPt");
    if(mu.isAvailable< float >("MuonSpectrometerPt")) pt_me = mu.auxdata< float >("MuonSpectrometerPt"); 

    if(type==1)  v.SetPtEtaPhiM( pt_id,     id_->eta(), id_->phi(), id_->m() );
    if(type==2)  v.SetPtEtaPhiM( pt_me,     me_->eta(), me_->phi(), me_->m() );
  }

  if(type==-1) v.SetPtEtaPhiM( mu.pt(),   mu.eta(),   mu.phi(),   mu.m()   );
  if(type==0)  v.SetPtEtaPhiM( cb_->pt(), cb_->eta(), cb_->phi(), cb_->m() );

  return v;
}

// returns efficiency scale factor of muon;
float MuonResonancePlots::effWeight(const xAOD::Muon& mu){

  float sf = 1.;
  if(mu.isAvailable< float >("EfficiencyScaleFactor") ) sf = mu.auxdata< float >("EfficiencyScaleFactor");
  return sf;
}

// returns Pt-sorted TLV pair
std::pair<TLorentzVector, TLorentzVector>  MuonResonancePlots::sortedPair(TLorentzVector v1, TLorentzVector v2){

  if(v1.Pt() >= v2.Pt()) return std::make_pair(v1, v2);
  else return std::make_pair(v2, v1);
}

//sets PDG mass 
float MuonResonancePlots::M0(){
  
  unsigned int res = 10;
  if(m_prefix == "Zmm_")    res = 0;
  if(m_prefix == "Jpsimm_") res = 1;
  if(m_prefix == "Ymm_")    res = 2;
  if(res == 10){
    std::cout << "There is no resonance mass value associated to " << m_prefix << " . Exiting." << std::endl;
    throw;
  }
  else return M_pdg[res];
}

// computes p* (ATL-COM-MUON 2014-001)
float MuonResonancePlots::p_star(TLorentzVector v1, TLorentzVector v2){

  float p = 0;
  if(v1.Pt()==0 || v2.Pt()==0) return p;
  p = M0() * std::sqrt( ( std::sin(v1.Theta()) * std::sin(v2.Theta()) )/( 2*(1-std::cos( std::abs(v1.Theta()-v2.Theta()) )) ) );
  return p;
}

// computes momentum inbalance
float MuonResonancePlots::deltaPt(TLorentzVector v1, TLorentzVector v2){

  float dp = -99.;
  if(v2.Pt()>0) dp = (v1.Pt() - v2.Pt())/v2.Pt();
  return dp;
}


// function to return Chi^2 / DoF
float MuonResonancePlots::getChiSquared(const xAOD::Muon& mu, int type){

  const xAOD::TrackParticle *cb_ = mu.trackParticle(xAOD::Muon::CombinedTrackParticle);
  const xAOD::TrackParticle *id_ = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle* me_ = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
  if(type>-1 && (!cb_ || !id_ || !me_)) return 0;

  if(type==-1 && mu.primaryTrackParticle()) return mu.primaryTrackParticle()->chiSquared()/mu.primaryTrackParticle()->numberDoF();
  if(type==0)  return cb_->chiSquared()/cb_->numberDoF();
  if(type==1)  return id_->chiSquared()/id_->numberDoF();
  if(type==2)  return me_->chiSquared()/me_->numberDoF();
  else return 0;
}

//***** only for debugging *****
void MuonResonancePlots::PrintVectors(TLorentzVector l1, TLorentzVector l2, TLorentzVector V,
				      TLorentzVector l1_truth, TLorentzVector l2_truth, TLorentzVector V_truth, int type, float weight){

  std::cout <<"Event weight " << weight << std::endl;  
  std::cout << "Reco muons, tracktype " << type << std::endl;
  std::cout <<"l1 Pt|Eta|Phi|E  " << l1.Pt() << " | " <<  l1.Eta() << " | " << l1.Phi() << " | " << l1.E() << std::endl;
  std::cout <<"l2 Pt|Eta|Phi|E  " << l2.Pt() << " | " <<  l2.Eta() << " | " << l2.Phi() << " | " << l2.E() << std::endl; 
  std::cout <<"V  Pt|Eta|Phi|E  " <<  V.Pt() << " | " <<   V.Eta() << " | " <<  V.Phi() << " | " <<  V.E() << std::endl;
  std::cout << "Truth muons " << std::endl;
  std::cout <<"l1_truth Pt|Eta|Phi|E  " << l1_truth.Pt() << " | " <<  l1_truth.Eta() << " | " << l1_truth.Phi() << " | " << l1_truth.E() << std::endl;
  std::cout <<"l2_truth Pt|Eta|Phi|E  " << l2_truth.Pt() << " | " <<  l2_truth.Eta() << " | " << l2_truth.Phi() << " | " << l2_truth.E() << std::endl; 
  std::cout <<"V_truth  Pt|Eta|Phi|E  " <<  V_truth.Pt() << " | " <<   V_truth.Eta() << " | " <<  V_truth.Phi() << " | " <<  V_truth.E() << std::endl;

  std::cout << "M_pdg   " <<  M0() << std::endl;
  std::cout << "M_truth " <<  V_truth.M() << std::endl;
  std::cout << "M_reco  " <<  V.M() << std::endl; 
}



