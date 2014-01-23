/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// CutsJpsi class
///---------------------------------------------------
/// Implements candidate selection for B2JpsiV0
/// candidates
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#ifndef CutsJpsi_H
#define CutsJpsi_H

#include "CutsBase.h"
#include "Chain.h"
#include "GoodRunsLists/DQHelperFunctions.h"

// main class
class CutsJpsi : public CutsBase {

  public:
    // costructor
    CutsJpsi(OutputLevel olIn, std::string settings);

    // public methods
    bool selectTrigDecision(TriggerBag& trigDecisions,
                            std::string chain);                 // select event based on trigger decision information
    bool selectTrigPassThrough(std::vector<std::string>* pt,
                               std::string chain);              // select pass-through chains
    bool selectLevel1(TriggerBag& trigDecisions,
                      std::vector<std::string>* pt);            // select level-1 trigger decision
    bool selectTrigger(TriggerBag& trigDecisions,
                       std::vector<std::string>* pt);           // select trigger decision (level-1 && level-2)

    bool selectJpsi(BCompositeParticle* jpsi,
                    TrackBag tracks,
                    MuonBag muons);                 // select Jpsi
    bool selectSimpleJpsi(SimpleJpsi* jpsi);        // select Jpsi

    bool selectFEX(Chain* chain,
                   int i,
                   bool analyzeMatched);            // selection of i-th TrigL2Bphys objects FEX selecton

    bool selectHypo(Chain* chain,
                    int i,
                    bool analyzeMatched,
                    double overrideChi2 = -1,
                    int overrideNMuHits = -1);       // selection of i-th TrigL2Bphys objects simulating Hypo selection

    bool selectFEX(Chain* chain,
                   bool analyzeMatched);            // loop through TrigL2Bphys objects and apply FEX selecton

    bool selectHypo(Chain* chain,
                    bool analyzeMatched,
                    double overrideChi2 = -1,
                    int overrideNMuHits = -1);      // loop through TrigL2Bphys objects and apply Hypo selection

    bool selectHypo(TrigDiMuonExtrapolator* propagator,
                    TrigHits* trig,
                    int nHits = -1,
                    double chi2 = -1);
    bool selectHypo(TrigDiMuonExtrapolator* propagator,
                    std::vector<int>* trigIndex,
                    TrigHits* trig,
                    int nHits = -1,
                    double chi2 = -1);              // loop through entries listed in trigIndex
    bool selectHypo(int entry,
                    TrigDiMuonExtrapolator* propagator,
                    TrigHits* trig,
                    int nHits = -1,
                    double chi2 = -1);              // loop through all entries in one event

    bool selectGRL(int run, int lumiBlock);         // select event using GRL

    // public attributes
    Muon      jpsiMu0;                              // Jpsi muons
    Muon      jpsiMu1;

    Track     jpsiTrk0;                             // Jpsi tracks
    Track     jpsiTrk1;

    double    jpsiDPhi;                             // opening angle in phi

    double    E0;                                   // moun track energy
    double    E1;                                   // moun track energy

    int    mu0_pixHits;                             // number of hits
    int    mu1_pixHits;
    int    mu0_sctHits;
    int    mu1_sctHits;
    int    mu0_mdtHits;
    int    mu1_mdtHits;

    // cut variables
    // Jpsi
    DoubleRangeCut  cut_jpsiMass;           // jpsi invariant mass cut
    DoubleCut       cut_jpsiChi2Prob;       // chi2 probability cut
    DoubleCut       cut_jpsiDPhi;           // opening angle of two muons in phi
    DoubleCut       cut_jpsiDEta;           // opening angle of two muons in eta

    DoubleCut       cut_trkChi2OnNdf;       // Track fit chi2/ndf cut
    DoubleCut       cut_muonPt;             // pt cut on muons
    DoubleCut       cut_muonPt2;            // pt cut on leading muon
    DoubleCut       cut_muonE;              // Enery cut on muons
    DoubleCut       cut_muonEta;            // eta of the jpsi
    IntCut          cut_muonPixHits;        // cut on no. of b-layer + pixel hits
    IntCut          cut_muonSctHits;        // cut on no. of SCT hits
    IntCut          cut_muonMdtHits;        // cut on no. of MDT hits
    StringCut       cut_muonTypes;          // which types of muons are used for the J/psi selection
    StringCut       cut_detPart;            // Detector part: Barrel, Endcap


    // Trigger
    StringCut       cut_level1;             // apply level-1 trigger
    StringCut       cut_level2;             // apply level-2 trigger
    StringCut       cut_passThrough;        // pass-through chains, coma separated list. Only part of the name can be specified, e.g. _mu4_,_MU4_

    // fex cuts
    DoubleCut       cut_fexChi2;            // cut on trigger vertexing chi2
    DoubleRangeCut  cut_fexMass;            // cut on invariant mass of TrigL2Bphys object
    DoubleCut       cut_fexTrkPt;           // cut on trigger track pT
    IntCut          cut_fexCheckOS;         // check tracks have opposite sign

    // hypo cuts
    DoubleCut       cut_hypoChi2;           // cut on trigger vertexing chi2
    DoubleRangeCut  cut_hypoMass;           // cut on invariant mass of TrigL2Bphys object
    IntCut          cut_hypoNMuHits;        // no. of associated muon hits requested in Hypo algorithm
    IntCut          cut_hypoCheckOS;        // check tracks have opposite sign

    // some other cuts
    DoubleCut       cut_trigTrkDR;          // matching between offline and trigger track
    IntCut          cut_doChi2Scan;         // perform scan of different chi2 cuts at Hypo step
    IntCut          cut_doNMuHitsScan;      // perform scan of different NMuHits cuts at Hypo step (only applicable for TrigDiMuon)
    DoubleCut       cut_extraChi2;          // chi2 cut to be applied separately

    // Good Runs List
    StringCut       cut_GRL;                // good run list

    // list of pass-through chains
    std::vector<std::string>  passThrough;
    std::vector<std::string>  level1;
    std::vector<std::string>  level2;

};

///===================================================
/// Begin of definitions
///===================================================

CutsJpsi::CutsJpsi(OutputLevel olIn, std::string settings) : CutsBase(olIn, settings) {
  Log log("CutsJpsi::CutsJpsi", ol() );

  // initialize cut variables
  //jpsi
  AddCutVariable("JpsiMass"       , cut_jpsiMass      , 2700, 3500);
  AddCutVariable("JpsiChi2Prob"   , cut_jpsiChi2Prob  , 0.05    );
  AddCutVariable("JpsiDPhi"       , cut_jpsiDPhi      , 100     );
  AddCutVariable("JpsiDEta"       , cut_jpsiDEta      , 100     );

  AddCutVariable("TrkChi2OnNdf"   , cut_trkChi2OnNdf  , 3.5     );
  AddCutVariable("MuonPt"         , cut_muonPt        , 2.800    );
  AddCutVariable("MuonPt2"        , cut_muonPt2       , 2.800    );
  AddCutVariable("MuonE"          , cut_muonE         , 3000    );
  AddCutVariable("MuonEta"        , cut_muonEta       , 100     );
  AddCutVariable("MuonPixHits"    , cut_muonPixHits   , 1       );
  AddCutVariable("MuonSctHits"    , cut_muonSctHits   , 6       );
  AddCutVariable("MuonMdtHits"    , cut_muonMdtHits   , 5       );

  AddCutVariable("MuonTypes"      , cut_muonTypes     , "ALL"   );
  AddCutVariable("DetPart"        , cut_detPart       , "Barrel");

  //trigger
  AddCutVariable("Level1"         , cut_level1        , "L1_MU0" );
  AddCutVariable("Level2"         , cut_level2        , "" );
  AddCutVariable("PassThrough"    , cut_passThrough   , "_MU4_,_mu4_" );

  log.info("Level1: ");
  level1 = explode(cut_level1.val, ",");
  std::vector<std::string>::iterator level1Itr = level1.begin();
  for(; level1Itr!=level1.end(); ++level1Itr) {
    log.info("  " + (*level1Itr));
  }

  log.info("Level2: ");
  level2 = explode(cut_level2.val, ",");
  std::vector<std::string>::iterator level2Itr = level2.begin();
  for(; level2Itr!=level2.end(); ++level2Itr) {
    log.info("  " + (*level2Itr));
  }

  log.info("Pass through: ");
  passThrough = explode(cut_passThrough.val, ",");
  std::vector<std::string>::iterator passThroughItr = passThrough.begin();
  for(; passThroughItr!=passThrough.end(); ++passThroughItr) {
    log.info("  " + (*passThroughItr));
  }

  // fex
  AddCutVariable("FexChi2"       , cut_fexChi2        , 10000.   );
  AddCutVariable("FexMass"       , cut_fexMass        , 500, 1e20);
  AddCutVariable("FexTrkPt"      , cut_fexTrkPt       , 2800     );
  AddCutVariable("FexCheckOS"    , cut_fexCheckOS     , 0        );

  // hypo
  AddCutVariable("HypoChi2"       , cut_hypoChi2      , 10000.   );
  AddCutVariable("HypoMass"       , cut_hypoMass      , 500, 1e20);
  AddCutVariable("HypoNMuHits"    , cut_hypoNMuHits   , 3        );
  AddCutVariable("HypoCheckOS"    , cut_hypoCheckOS   , 0        );

  // some other cuts
  AddCutVariable("TrigTrkDR"      , cut_trigTrkDR     , 0.1      );
  AddCutVariable("DoChi2Scan"     , cut_doChi2Scan    , 0        );
  AddCutVariable("DoNMuHitsScan"  , cut_doNMuHitsScan , 0        );
  AddCutVariable("ExtraChi2"      , cut_extraChi2     , 30       );

  //GRL
  AddCutVariable("GRL"            , cut_GRL           , ""       );
  if( cut_GRL.enabled ) {
    DQ::SetXMLFile(cut_GRL.val.c_str());
  }

}
///===================================================
bool CutsJpsi::selectTrigDecision(TriggerBag& trigDecisions, std::string chain) {
  Log log("CutsJpsi::selectLevel1", ol() );

  // loop through decisions
  TriggerBag::iterator trigItr = trigDecisions.begin();
  for(; trigItr!=trigDecisions.end(); ++trigItr) {
    if( chain == (*trigItr).name() ) {
      return true;
    }
  }

  log.debug("Candidate not accepted by level-1 trigger");

  return false;

}
///===================================================
bool CutsJpsi::selectTrigPassThrough(std::vector<std::string>* pt, std::string chain) {
  Log log("CutsJpsi::selectTrigPassThrough", ol() );

  // loop through decisions
  std::vector<std::string>::iterator ptItr = pt->begin();
  for(; ptItr!=pt->end(); ++ptItr) {
    if( ptItr->find(chain) != std::string::npos) return true;
  }

  log.debug("Candidate not accepted by pass-through trigger");

  return false;

}
///===================================================
bool CutsJpsi::selectLevel1(TriggerBag& trigDecisions, std::vector<std::string>* pt) {
  Log log("CutsJpsi::selectLevel1", ol() );

  // level1
  if(cut_level1.enabled) {

    std::vector<std::string>::iterator level1Itr = level1.begin();
    for(; level1Itr!=level1.end(); ++level1Itr) {
      if( selectTrigDecision(trigDecisions, *level1Itr) ) return true;
    }
  }

  // pass through chains
  if(cut_passThrough.enabled) {

    std::vector<std::string>::iterator passThroughItr = passThrough.begin();
    for(; passThroughItr!=passThrough.end(); ++passThroughItr) {
      if( selectTrigPassThrough(pt, *passThroughItr) ) return true;
    }
  }

  if(!cut_level1.enabled && !cut_passThrough.enabled) return true;

  return false;

}
///===================================================
bool CutsJpsi::selectTrigger(TriggerBag& trigDecisions, std::vector<std::string>* pt) {
  Log log("CutsJpsi::selectTrigger", ol() );

  if( !selectLevel1(trigDecisions, pt) ) return false;

  if(cut_level2.enabled) {

    std::vector<std::string>::iterator level2Itr = level2.begin();
    for(; level2Itr!=level2.end(); ++level2Itr) {
      if( selectTrigDecision(trigDecisions, *level2Itr) ) return true;
    }
  } else
    return true;

    return false;

}
///===================================================
bool CutsJpsi::selectJpsi(BCompositeParticle* jpsi,
                          TrackBag tracks,
                          MuonBag muons)
{
  Log log("CutsJpsi::selectJpsi", ol() );

  bool success;

  //get Jpsi tracks
  success =
      jpsi->findTrack(0, tracks, jpsiTrk0) &&
      jpsi->findTrack(1, tracks, jpsiTrk1);

  if(!success) {
    log.error("Jpsi doesn't have tracks");
    return false;
  }

  // get Jpsi muons
  success =
      jpsi->findMuon(0, muons, jpsiMu0) &&
      jpsi->findMuon(1, muons, jpsiMu1);

  if(!success) {
    log.error("Jpsi doesn't have muons");
    return false;
  }

  // calculate muon energy
  E0 = jpsi->getTrackLorentzMom(0, 105.658).E();
  E1 = jpsi->getTrackLorentzMom(1, 105.658).E();

  // retrieve number of hits
  mu0_pixHits = jpsiTrk0.nBLayer() + jpsiTrk0.nPix();
  mu1_pixHits = jpsiTrk1.nBLayer() + jpsiTrk1.nPix();

  mu0_sctHits = jpsiTrk0.nSCT();
  mu1_sctHits = jpsiTrk1.nSCT();

  mu0_mdtHits = jpsiMu0.numberOfMDTHits();
  mu1_mdtHits = jpsiMu1.numberOfMDTHits();

  // calculate opening angle
  jpsiDPhi = dAngle(jpsi->getTrackMom(0).Phi(), jpsi->getTrackMom(1).Phi());

  // apply Jpsi cuts
  if(cut_jpsiMass.enabled       && cut_jpsiMass != jpsi->VTX_mass) return false;
  if(cut_jpsiChi2Prob.enabled   && cut_jpsiChi2Prob > TMath::Prob(jpsi->VTX_chi2, jpsi->VTX_NDF) ) return false;
  if(cut_jpsiDPhi.enabled       && cut_jpsiDPhi < fabs( dAngle(jpsi->getTrackMom(0).Phi(), jpsi->getTrackMom(1).Phi()) ) ) return false;
  if(cut_jpsiDEta.enabled       && cut_jpsiDEta < fabs( jpsi->getTrackMom(0).Eta() - jpsi->getTrackMom(1).Eta() ) ) return false;

  // aply muon cuts
  if( cut_trkChi2OnNdf.enabled  && (cut_trkChi2OnNdf < jpsiTrk0.chi2()/jpsiTrk0.ndf() || cut_trkChi2OnNdf < jpsiTrk1.chi2()/jpsiTrk1.ndf()) ) return false;

  if(cut_muonPt.enabled         && (cut_muonPt > jpsi->getTrackMom(0).Perp() || cut_muonPt > jpsi->getTrackMom(1).Perp()) ) return false;
  if(cut_muonE.enabled          && (cut_muonE  > E0 || cut_muonE  > E1  ) ) return false;
  if(cut_muonPixHits.enabled    && (cut_muonPixHits > mu0_pixHits || cut_muonPixHits > mu1_pixHits ) ) return false;
  if(cut_muonSctHits.enabled    && (cut_muonSctHits > mu0_sctHits || cut_muonSctHits > mu1_sctHits ) ) return false;
  if(cut_muonMdtHits.enabled    && (cut_muonMdtHits > mu0_mdtHits || cut_muonMdtHits > mu1_mdtHits ) ) return false;

  if(cut_muonPt2.enabled        &&  cut_muonPt2 > TMath::Max(jpsi->getTrackMom(0).Perp(), jpsi->getTrackMom(1).Perp()) ) return false;
  if(cut_muonEta.enabled        && (cut_muonEta < fabs(jpsi->getTrackMom(0).Eta()) || cut_muonEta < fabs(jpsi->getTrackMom(1).Eta())) ) return false;

  if(cut_muonTypes.enabled ) {
    if(cut_muonTypes == "ONE_COMB"  &&  !jpsiMu0.isCombinedMuon() && !jpsiMu1.isCombinedMuon()  ) return false;
    if(cut_muonTypes == "BOTH_COMB" && (!jpsiMu0.isCombinedMuon() || !jpsiMu1.isCombinedMuon()) ) return false;
  }

  if(cut_detPart.enabled) {
    if(cut_detPart == "BB" && (fabs(jpsi->getTrackMom(0).Eta()) >   1. || fabs(jpsi->getTrackMom(1).Eta()) >   1.) ) return false;
    if(cut_detPart == "EE" && (fabs(jpsi->getTrackMom(0).Eta()) <=  1. || fabs(jpsi->getTrackMom(1).Eta()) <=  1.) ) return false;
    if(cut_detPart == "BE" && ((fabs(jpsi->getTrackMom(0).Eta()) >  1. && fabs(jpsi->getTrackMom(1).Eta()) >  1.) ||
       (fabs(jpsi->getTrackMom(0).Eta()) <=  1. && fabs(jpsi->getTrackMom(1).Eta()) <=  1.)) ) return false;
  }

  // all cuts passed
  return true;

}
///===================================================
bool CutsJpsi::selectSimpleJpsi(SimpleJpsi* jpsi) {
  Log log("CutsJpsi::selectSimpleJpsi", ol() );

  if(cut_jpsiDPhi.enabled && cut_jpsiDPhi < fabs( dAngle(jpsi->trk0_phi, jpsi->trk1_phi) ) ) return false;
  if(cut_jpsiDEta.enabled && cut_jpsiDEta < fabs( jpsi->trk0_eta - jpsi->trk1_eta ) ) return false;

  if(cut_detPart.enabled) {
    if(cut_detPart == "BB" && (fabs(jpsi->trk0_eta) >   1. || fabs(jpsi->trk1_eta) >   1.) ) return false;
    if(cut_detPart == "EE" && (fabs(jpsi->trk0_eta) <=  1. || fabs(jpsi->trk1_eta) <=  1.) ) return false;
    if(cut_detPart == "BE" && ((fabs(jpsi->trk0_eta) >  1. && fabs(jpsi->trk1_eta) >  1.) || (fabs(jpsi->trk0_eta) <=  1. && fabs(jpsi->trk1_eta) <=  1.)) ) return false;
  }

  return true;
}
///===================================================
bool CutsJpsi::selectFEX(Chain* chain, int i, bool analyzeMatched) {
  Log log("CutsJpsi::selectFEX", ol() );

  double chi2, pt1, pt2, mass, ch1, ch2;

  // get vaues
  if(analyzeMatched) {
    chi2 = chain->Matched_chi2->at(i);
    mass = chain->Matched_mass->at(i);
    pt1  = chain->Matched_trk1_pt->at(i);
    pt2  = chain->Matched_trk2_pt->at(i);
    ch1  = chain->Matched_trk1_charge->at(i);
    ch2  = chain->Matched_trk2_charge->at(i);
  }else{
    chi2 = chain->All_chi2->at(i);
    mass = chain->All_mass->at(i);
    pt1  = chain->All_trk1_pt->at(i);
    pt2  = chain->All_trk2_pt->at(i);
    ch1  = chain->All_trk1_charge->at(i);
    ch2  = chain->All_trk2_charge->at(i);
  }

  //apply cuts
  if(cut_fexChi2.enabled && cut_fexChi2 < chi2 ) return false;
  if(cut_fexMass.enabled && cut_fexMass != mass ) return false;
  if(cut_fexTrkPt.enabled && ( cut_fexTrkPt > fabs(pt1) || cut_fexTrkPt > fabs(pt2))) return false;
  if(cut_fexCheckOS.enabled && cut_fexCheckOS.val && ch1*ch2 >=0 ) return false;

  // all cuts passed
  return true;

}
///===================================================
bool CutsJpsi::selectHypo(Chain* chain, int i, bool analyzeMatched,
                          double overrideChi2,
                          int overrideNMuHits)
{
  Log log("CutsJpsi::selectHypo", ol() );

  double chi2, pt1, pt2, mass, ch1, ch2;
  double mdt1, mdt2, rpc1, rpc2, tgc1, tgc2;

  // get vaues
  if(analyzeMatched) {
    chi2 = chain->Matched_chi2->at(i);
    mass = chain->Matched_mass->at(i);
    pt1  = chain->Matched_trk1_pt->at(i);
    pt2  = chain->Matched_trk2_pt->at(i);
    ch1  = chain->Matched_trk1_charge->at(i);
    ch2  = chain->Matched_trk2_charge->at(i);
    mdt1 = chain->Matched_trk1_mdtHits->at(i);
    rpc1 = chain->Matched_trk1_rpcHits->at(i);
    tgc1 = chain->Matched_trk1_tgcHits->at(i);
    mdt2 = chain->Matched_trk2_mdtHits->at(i);
    rpc2 = chain->Matched_trk2_rpcHits->at(i);
    tgc2 = chain->Matched_trk2_tgcHits->at(i);
  }else{
    chi2 = chain->All_chi2->at(i);
    mass = chain->All_mass->at(i);
    pt1  = chain->All_trk1_pt->at(i);
    pt2  = chain->All_trk2_pt->at(i);
    ch1  = chain->All_trk1_charge->at(i);
    ch2  = chain->All_trk2_charge->at(i);
    mdt1 = chain->All_trk1_mdtHits->at(i);
    rpc1 = chain->All_trk1_rpcHits->at(i);
    tgc1 = chain->All_trk1_tgcHits->at(i);
    mdt2 = chain->All_trk2_mdtHits->at(i);
    rpc2 = chain->All_trk2_rpcHits->at(i);
    tgc2 = chain->All_trk2_tgcHits->at(i);
  }

  log.debug("ch1*pt1 = " + fToStr(ch1*pt1) + ", ch2*pt2 = " + fToStr(ch2*pt2));

  // chi2 and nMuHits cuts
  bool doChi2Cut    = cut_hypoChi2.enabled;
  bool doNMuHitsCut = cut_hypoNMuHits.enabled;

  double cutChi2    = cut_hypoChi2.val;
  double cutNMuHits = cut_hypoNMuHits.val;

  if(overrideChi2 >= 0) {
    cutChi2 = overrideChi2;
    doChi2Cut = true;
  }

  if(overrideNMuHits>= 0) {
    cutNMuHits = overrideNMuHits;
    doNMuHitsCut = true;
  }

  //apply cuts
  if(doChi2Cut && cutChi2 < chi2 ) return false;
  if(cut_hypoMass.enabled && cut_hypoMass != mass ) return false;
  if(cut_hypoCheckOS.enabled && cut_hypoCheckOS.val && ch1*ch2 >=0 ) return false;
  if(doNMuHitsCut && ( cutNMuHits > mdt1+rpc1+tgc1 || cutNMuHits > mdt2+rpc2+tgc2 )) return false;

  // all cuts passed
  return true;

}
///===================================================
bool CutsJpsi::selectFEX(Chain* chain, bool analyzeMatched) {
  Log log("CutsJpsi::selectFEX", ol() );

  int nCand = (int)chain->All_fitMass->size();
  if(analyzeMatched)
    nCand = (int)chain->Matched_fitMass->size();

  for(int i=0; i < nCand; ++i) {
    // apply FEX cuts on TrigL2Bphys candidate
    if( selectFEX(chain, i, analyzeMatched) ) {
      return true;
    }
  }

  return false;

}
///===================================================
bool CutsJpsi::selectHypo(Chain* chain, bool analyzeMatched,
                          double overrideChi2,
                          int overrideNMuHits)
{
  Log log("CutsJpsi::selectHypo", ol() );

  int nCand = (int)chain->All_fitMass->size();
  if(analyzeMatched)
    nCand = (int)chain->Matched_fitMass->size();

  for(int i=0; i < nCand; ++i) {
    // apply Hypo cuts on TrigL2Bphys candidate
    if( selectHypo(chain, i, analyzeMatched, overrideChi2, overrideNMuHits) ) {
      return true;
    }
  }

  return false;

}
///===================================================
bool CutsJpsi::selectHypo(TrigDiMuonExtrapolator* propagator, TrigHits* trig, int nHits, double chi2) {
  Log log("CutsJpsi::selectHypo", ol() );

  if(cut_hypoMass.enabled && cut_hypoMass != trig->trigMass ) return false;
  if(cut_hypoCheckOS.enabled && cut_hypoCheckOS.val && trig->trk0_charge * trig->trk1_charge >=0 ) return false;

  // chi2 and nMuHits cuts
  bool doChi2Cut    = cut_hypoChi2.enabled;
  bool doNMuHitsCut = cut_hypoNMuHits.enabled;

  double cutChi2    = cut_hypoChi2.val;
  double cutNMuHits = cut_hypoNMuHits.val;

  if(chi2 >= 0) {
    cutChi2 = chi2;
    doChi2Cut = true;
  }

  if(nHits>= 0) {
    cutNMuHits = nHits;
    doNMuHitsCut = true;
  }

  if(nHits == -2) doNMuHitsCut = false;

  // apply chi2 cut
  if(doChi2Cut && cutChi2 < trig->chi2 ) return false;

  if(doNMuHitsCut) {
    // no of hits
    int nMdt[2] = {0, 0};
    int nRpc[2] = {0, 0};
    int nTgc[2] = {0, 0};

    // apply hit cut
    for(int j=0; j<2; ++j) {
      double ptId   = trig->pt(j);
      double etaId  = trig->eta(j);
      double phiId  = trig->phi(j);
      double charge = trig->charge(j);

      // MDT  -------------------------------------
      for(int i=0; i<(int)trig->innerMdt_r(j)->size(); ++i) {
        if(propagator->matchMdtInner(trig->innerMdt_r(j)->at(i), trig->innerMdt_z(j)->at(i), ptId, etaId, phiId, charge))
          ++nMdt[j];
      }

      for(int i=0; i<(int)trig->middleMdt_r(j)->size(); ++i) {
        if(propagator->matchMdtMiddle(trig->middleMdt_r(j)->at(i), trig->middleMdt_z(j)->at(i), ptId, etaId, phiId, charge))
          ++nMdt[j];
      }

      // RPC ---------------------------------------
      for(int i=0; i<(int)trig->etaRpc_r(j)->size(); ++i) {
        if(propagator->matchRpcEtaPad(trig->etaRpc_r(j)->at(i), trig->etaRpc_z(j)->at(i), ptId, etaId, phiId, charge))
          ++nRpc[j];
      }

      for(int i=0; i<(int)trig->phiRpc_x(j)->size(); ++i) {
        if(propagator->matchRpcPhiPad(trig->phiRpc_x(j)->at(i), trig->phiRpc_y(j)->at(i), ptId, etaId, phiId, charge))
          ++nRpc[j];
      }

      // TGC ---------------------------------------
      // wire
      for(int i=0; i<(int)trig->innerEtaTgc_r(j)->size(); ++i) {
        if(propagator->matchInnerTgcRhoHit(trig->innerEtaTgc_r(j)->at(i), trig->innerEtaTgc_z(j)->at(i), trig->innerEtaTgc_phi(j)->at(i), ptId, etaId, phiId, charge))
          ++nTgc[j];
      }
      for(int i=0; i<(int)trig->middleEtaTgc_r(j)->size(); ++i) {
        if(propagator->matchMiddleTgcRhoHit(trig->middleEtaTgc_r(j)->at(i), trig->middleEtaTgc_z(j)->at(i), trig->middleEtaTgc_phi(j)->at(i), ptId, etaId, phiId, charge))
          ++nTgc[j];
      }

      // strip
      for(int i=0; i<(int)trig->innerPhiTgc(j)->size(); ++i) {
        if(propagator->matchInnerTgcPhiHit(trig->innerPhiTgc(j)->at(i), ptId, etaId, phiId, charge))
          ++nTgc[j];
      }

      for(int i=0; i<(int)trig->middlePhiTgc(j)->size(); ++i) {
        if(propagator->matchMiddleTgcPhiHit(trig->middlePhiTgc(j)->at(i), ptId, etaId, phiId, charge))
          ++nTgc[j];
      }
    }

//     if(nMdt[0] != trig->trk0_nMdt)
//       log.info("MDT0: "+iToStr(nMdt[0])+" x "+fToStr( trig->trk0_nMdt ));
//     if(nMdt[1] != trig->trk1_nMdt)
//       log.info("MDT1: "+iToStr(nMdt[1])+" x "+fToStr( trig->trk1_nMdt ));
    //
//     if(nTgc[0] != trig->trk0_nTgc && trig->trk0_nTgc!=15)
//       log.info("TGC0: "+iToStr(nTgc[0])+" x "+fToStr( trig->trk0_nTgc ) + ", eta = "+fToStr(trig->eta(0)));
//     if(nTgc[1] != trig->trk1_nTgc && trig->trk1_nTgc!=15)
//       log.info("TGC1: "+iToStr(nTgc[1])+" x "+fToStr( trig->trk1_nTgc ) + ", eta = "+fToStr(trig->eta(1)));
    //
//     if(nRpc[0] != trig->trk0_nRpc) {
//       log.info("RPC0: "+iToStr(nRpc[0])+" x "+fToStr( trig->trk0_nRpc ) + ", eta = "+fToStr(trig->eta(0)));
//     }
//     if(nRpc[1] != trig->trk1_nRpc)
//       log.info("RPC1: "+iToStr(nRpc[1])+" x "+fToStr( trig->trk1_nRpc ) + ", eta = "+fToStr(trig->eta(1)));

    if(nMdt[0] + nRpc[0] + nTgc[0] >= cutNMuHits && nMdt[1] + nRpc[1] + nTgc[1] >= cutNMuHits )
      return true;

//     if(trig->trk0_nMdt +  trig->trk0_nRpc + trig->trk0_nTgc >= cutNMuHits && trig->trk1_nMdt +  trig->trk1_nRpc + trig->trk1_nTgc >= cutNMuHits )
//       return true;

    return false;
  }

  return true;
}
///===================================================
bool CutsJpsi::selectHypo(TrigDiMuonExtrapolator* propagator, std::vector<int>* trigIndex, TrigHits* trig, int nHits, double chi2) {
  Log log("CutsJpsi::selectHypo", ol() );

  bool passHypo = false;
  std::vector<int>::iterator indexItr = trigIndex->begin();
  for(; indexItr!=trigIndex->end(); ++indexItr) {
    // load trigger object
    trig->GetEntry(*indexItr);

    // apply hypo selection
    if( selectHypo(propagator, trig, nHits, chi2) ) passHypo = true;
  }

  return passHypo;
}
///===================================================
bool CutsJpsi::selectHypo(int entry, TrigDiMuonExtrapolator* propagator, TrigHits* trig, int nHits, double chi2) {
  Log log("CutsJpsi::selectHypo", ol() );

  trig->GetEntry(entry);
  int run   = trig->runNumber;
  int event = trig->eventNumber;

  bool passHypo = false;
  for(; run == trig->runNumber && event == trig->eventNumber && entry < trig->fChain->GetEntries(); trig->GetEntry(++entry)) {
    //log.info("entry "+iToStr(entry)+" run "+iToStr(trig->runNumber)+" event "+iToStr(trig->eventNumber));
    // apply hypo selection
    if( selectHypo(propagator, trig, nHits, chi2) ) passHypo = true;
  }

  return passHypo;

}
///===================================================
bool CutsJpsi::selectGRL(int run, int lumiBlock) {
  Log log("CutsJpsi::selectGRL", ol() );

  if( cut_GRL.enabled && !DQ::PassRunLB(run,lumiBlock) ) return false;

  return true;

}
///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
