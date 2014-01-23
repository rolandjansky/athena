/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************
// NtupleReader.h
// This class reads and stores the n-tuples
// made by GetTrack, GetMuonSimple and
// GetVertex, and returns them to the user
// either in a single vector or
// event-by-event, within a ROOT session
// ****************************************
#ifndef NtupleReader_H
#define NtupleReader_H

#include "Vertex.h"
#include "Track.h"
#include "Muon.h"
#include "TriggerDecision.h"
#include "TFile.h"
#include "TTree.h"
#include<iostream>

typedef std::vector<Track> TrackBag;
typedef std::vector<Muon> MuonBag;
typedef std::vector<Vertex> VertexBag;
typedef std::vector<TriggerDecision> TriggerBag;

class NtupleReader {
 public:
  NtupleReader() { };
  void prepareTree(std::string, TTree*);
  TrackBag readTrackTree(Int_t, std::string, TTree*);
  VertexBag readVertexTree(Int_t, std::string, TTree*);
  MuonBag readMuonTree(Int_t, std::string, TTree*);
  TriggerBag readTriggerTree(Int_t, std::string, TTree*);
  std::vector<VertexBag> allVertices(std::string, TTree*);
  std::vector<TrackBag> allTracks(std::string, TTree*);
  std::vector<MuonBag> allMuons(std::string, TTree*);
  std::vector<TriggerBag> allTriggers(std::string, TTree*);
  VertexBag getVertices(Int_t, Int_t, std::string, TTree*);
  TrackBag getTracks(Int_t,  Int_t, std::string, TTree*);
  MuonBag getMuons(Int_t,  Int_t, std::string, TTree*);
  TriggerBag getTriggers(Int_t, Int_t, std::string, TTree*);
};

void NtupleReader::prepareTree(std::string key, TTree* theTree) {

  // Build the indices for navigating between events
  theTree->BuildIndex((key+"_runNumber").c_str(), (key+"_eventNumber").c_str());
  return;

}

TrackBag NtupleReader::getTracks(Int_t run,  Int_t event, std::string key, TTree* TRACKS){

  // Get the entry corresponding to the run and event
  Int_t index = TRACKS->GetEntryNumberWithIndex(run,event);
  if (index==-1) { // Trap in case wrong event/run
    std::cout << "ERROR: run/event pairing " << run << "/" << event << " is not found in the track ntuple with key " << key << std::endl;
    std::cout << "Please check! Returning an empty vector of tracks!" << std::endl;
    TrackBag blank; blank.clear();
    return blank;
  }

  // Get the tracks corresponding to this entry
  TrackBag theTracks = readTrackTree(index,key,TRACKS);

  // Return to the user
  return theTracks;

}

MuonBag NtupleReader::getMuons(Int_t run, Int_t event, std::string key, TTree* MUONS){

  // Get the entry corresponding to the run and event
  Int_t index = MUONS->GetEntryNumberWithIndex(run,event);
  if (index==-1) {
    std::cout << "ERROR: run/event pairing " << run << "/" << event << " is not found in the muon ntuple with key " << key << std::endl;
    std::cout << "Please check! Returning an empty vector of muons!" << std::endl;
    MuonBag blank; blank.clear();
    return blank;
  }

  // Get the tracks corresponding to this entry
  MuonBag theMuons = readMuonTree(index,key,MUONS);

  // Return to the user
  return theMuons;

}

VertexBag NtupleReader::getVertices(Int_t run,  Int_t event, std::string key, TTree* VERTICES){

  // Get the entry corresponding to the run and event
  Int_t index = VERTICES->GetEntryNumberWithIndex(run,event);
  if (index==-1) {
    std::cout << "ERROR: run/event pairing " << run << "/" << event << " is not found in the vertex ntuple with key " << key << std::endl;
    std::cout << "Please check! Returning an empty vector of vertices!" << std::endl;
    VertexBag blank; blank.clear();
    return blank;
  }

  // Get the tracks corresponding to this entry
  VertexBag theVertices = readVertexTree(index,key,VERTICES);

  // Return to the user
  return theVertices;

}

TriggerBag NtupleReader::getTriggers(Int_t run,  Int_t event, std::string key, TTree* TRIG){

  // Get the entry corresponding to the run and event
  Int_t index = TRIG->GetEntryNumberWithIndex(run,event);
  if (index==-1) {
    std::cout << "ERROR: run/event pairing " << run << "/" << event << " is not found in the trigger ntuple with key " << key << std::endl;
    std::cout << "Please check! Returning an empty vector of trigger decisions!" << std::endl;
    TriggerBag blank; blank.clear();
    return blank;
  }

  // Get the tracks corresponding to this entry
  TriggerBag theTriggers = readTriggerTree(index,key,TRIG);

  // Return to the user
  return theTriggers;

}

TrackBag NtupleReader::readTrackTree(Int_t index, std::string key, TTree* TRACKS) {

  // Declare the vectors into which the tree will be read
  Int_t eventNumber(0);
  Int_t lumiBlock(0);
  Int_t runNumber(0);
  std::vector<Double_t> *m_px(0);
  std::vector<Double_t> *m_py(0);
  std::vector<Double_t> *m_pz(0);
  std::vector<Double_t> *m_pt(0);
  std::vector<Double_t> *m_eta(0);
  std::vector<Double_t> *m_d0(0);
  std::vector<Double_t> *m_z0(0);
  std::vector<Double_t> *m_phi0(0);
  std::vector<Double_t> *m_theta(0);
  std::vector<Double_t> *m_qOverP(0);
  std::vector<Double_t> *m_d0Err(0);
  std::vector<Double_t> *m_z0Err(0);
  std::vector<Double_t> *m_phi0Err(0);
  std::vector<Double_t> *m_thetaErr(0);
  std::vector<Double_t> *m_qOverPErr(0);
  std::vector<Double_t> *m_chi2(0);
  std::vector<Int_t> * m_ndf(0);
  std::vector<Int_t> * m_nBLayer(0);
  std::vector<Int_t> * m_nPix(0);
  std::vector<Int_t> * m_nSCT(0);
  std::vector<Int_t> * m_nTRT(0);
  std::vector<Int_t> * m_index(0);
  std::vector<Int_t> * m_vtxIndex(0);

  // Set branch addresses
  std::string branchString = key+"_TRKS";
  TRACKS->SetBranchAddress( (key+"_eventNumber").c_str(), &eventNumber);
  TRACKS->SetBranchAddress( (key+"_lumiBlock").c_str(), &lumiBlock);
  TRACKS->SetBranchAddress( (key+"_runNumber").c_str(), &runNumber);
  TRACKS->SetBranchAddress( (branchString+"_px").c_str(), &m_px);
  TRACKS->SetBranchAddress( (branchString+"_py").c_str(), &m_py);
  TRACKS->SetBranchAddress( (branchString+"_pz").c_str(), &m_pz);
  TRACKS->SetBranchAddress( (branchString+"_pt").c_str(), &m_pt);
  TRACKS->SetBranchAddress( (branchString+"_eta").c_str(), &m_eta);
  TRACKS->SetBranchAddress( (branchString+"_d0").c_str(), &m_d0);
  TRACKS->SetBranchAddress( (branchString+"_z0").c_str(), &m_z0);
  TRACKS->SetBranchAddress( (branchString+"_phi0").c_str(), &m_phi0);
  TRACKS->SetBranchAddress( (branchString+"_theta").c_str(), &m_theta);
  TRACKS->SetBranchAddress( (branchString+"_qOverP").c_str(), &m_qOverP);

  TRACKS->SetBranchAddress( (branchString+"_d0Err").c_str(), &m_d0Err);
  TRACKS->SetBranchAddress( (branchString+"_z0Err").c_str(), &m_z0Err);
  TRACKS->SetBranchAddress( (branchString+"_phi0Err").c_str(), &m_phi0Err);
  TRACKS->SetBranchAddress( (branchString+"_thetaErr").c_str(), &m_thetaErr);
  TRACKS->SetBranchAddress( (branchString+"_qOverPErr").c_str(), &m_qOverPErr);

  TRACKS->SetBranchAddress( (branchString+"_chi2").c_str(), &m_chi2);
  TRACKS->SetBranchAddress( (branchString+"_ndf").c_str(), &m_ndf);
  TRACKS->SetBranchAddress( (branchString+"_nBLayer").c_str(), &m_nBLayer);
  TRACKS->SetBranchAddress( (branchString+"_nPix").c_str(), &m_nPix);
  TRACKS->SetBranchAddress( (branchString+"_nSCT").c_str(), &m_nSCT);
  TRACKS->SetBranchAddress( (branchString+"_nTRT").c_str(), &m_nTRT);
  TRACKS->SetBranchAddress( (branchString+"_index").c_str(), &m_index);
  TRACKS->SetBranchAddress( (branchString+"_vtxIndex").c_str(), &m_vtxIndex);

  // Looping variables
  Int_t j;

  TRACKS->GetEntry(index);
  Int_t nTrks = m_index->size();
  TrackBag eventTracks; eventTracks.clear();
  if (nTrks==0) return eventTracks;
  for (j=0; j<nTrks; ++j) {
    Track tmpTrack;
    tmpTrack.setEventNumber(eventNumber);
    tmpTrack.setLumiBlock(lumiBlock);
    tmpTrack.setRunNumber(runNumber);
    tmpTrack.setPx( (*m_px)[j]);
    tmpTrack.setPy( (*m_py)[j]);
    tmpTrack.setPz( (*m_pz)[j]);
    tmpTrack.setPt( (*m_pt)[j]);
    tmpTrack.setEta( (*m_eta)[j]);
    tmpTrack.setD0( (*m_d0)[j]);
    tmpTrack.setZ0( (*m_z0)[j]);
    tmpTrack.setPhi0( (*m_phi0)[j]);
    tmpTrack.setTheta( (*m_theta)[j]);
    tmpTrack.setQOverP( (*m_qOverP)[j]);

    tmpTrack.setD0Err( (*m_d0Err)[j]);
    tmpTrack.setZ0Err( (*m_z0Err)[j]);
    tmpTrack.setPhi0Err( (*m_phi0Err)[j]);
    tmpTrack.setThetaErr( (*m_thetaErr)[j]);
    tmpTrack.setQOverPErr( (*m_qOverPErr)[j]);

    tmpTrack.setChi2( (*m_chi2)[j]);
    tmpTrack.setNDF( (*m_ndf)[j]);
    tmpTrack.setNBLayer( (*m_nBLayer)[j]);
    tmpTrack.setNPix( (*m_nPix)[j]);
    tmpTrack.setNSCT( (*m_nSCT)[j]);
    tmpTrack.setNTRT( (*m_nTRT)[j]);
    tmpTrack.setIndex( (*m_index)[j]);
    tmpTrack.setVtxIndex( (*m_vtxIndex)[j]);
    eventTracks.push_back(tmpTrack);
  }

  return eventTracks;

}

MuonBag NtupleReader::readMuonTree(Int_t index, std::string key, TTree* MUONS) {

  // Declare the vectors into which the tree will be read
  Int_t eventNumber(0);
  Int_t lumiBlock(0);
  Int_t runNumber(0);
  std::vector<Double_t> *m_px(0);
  std::vector<Double_t> *m_py(0);
  std::vector<Double_t> *m_pz(0);
  std::vector<Double_t> *m_e(0);
  std::vector<Double_t> *m_cotTh(0);
  std::vector<Double_t> *m_pt(0);
  std::vector<Double_t> *m_eta(0);
  std::vector<Double_t> *m_phi(0);
  std::vector<Int_t> *m_charge(0);
  std::vector<Int_t> *m_author(0);
  std::vector<bool> *m_isLoose(0);
  std::vector<bool> *m_isMedium(0);
  std::vector<bool> *m_isTight(0);
  std::vector<Double_t> *m_fitChi2(0);
  std::vector<Double_t> *m_matchChi2(0);
  std::vector<Double_t> *m_matchChi2OverDoF(0);
  std::vector<Double_t> *m_nucone20(0);
  std::vector<Double_t> *m_etcone10(0);
  std::vector<Double_t> *m_etcone20(0);
  std::vector<Double_t> *m_etcone30(0);
  std::vector<Double_t> *m_etcone40(0);
  std::vector<bool> *m_bestMatch(0);
  std::vector<bool> *m_isCombinedMuon(0);
  std::vector<bool> *m_hasCombinedMuon(0);
  std::vector<bool> *m_hasInDetTrackParticle(0);
  std::vector<bool> *m_isStandAloneMuon(0);
  std::vector<bool> *m_hasMuonSpectrometerTrackParticle(0);
  std::vector<bool> *m_hasMuonExtrapolatedTrackParticle(0);
  std::vector<bool> *m_hasCombinedMuonTrackParticle(0);
  std::vector<bool> *m_hasCluster(0);
  std::vector<bool> *m_isHighPt(0);
  std::vector<bool> *m_isLowPt(0);
  std::vector<Int_t> *m_numberOfBLayerHits(0);
  std::vector<Int_t> *m_numberOfPixelHits(0);
  std::vector<Int_t> *m_numberOfSCTHits(0);
  std::vector<Int_t> *m_numberOfTRTHits(0);
  std::vector<Int_t> *m_numberOfTRTHighThresholdHits(0);
  std::vector<Int_t> *m_numberOfMDTHits(0);
  std::vector<Int_t> *m_numberOfCSCEtaHits(0);
  std::vector<Int_t> *m_numberOfCSCPhiHits(0);
  std::vector<Int_t> *m_numberOfRPCEtaHits(0);
  std::vector<Int_t> *m_numberOfRPCPhiHits(0);
  std::vector<Int_t> *m_numberOfTGCEtaHits(0);
  std::vector<Int_t> *m_numberOfTGCPhiHits(0);
  std::vector<Int_t> *m_index(0);
  std::vector<Int_t> *m_inDetTrackIndex(0);


  // Set branch addresses
  std::string branchString = key+"_MU";
  MUONS->SetBranchAddress( (key+"_eventNumber").c_str(), &eventNumber);
  MUONS->SetBranchAddress( (key+"_lumiBlock").c_str(), &lumiBlock);
  MUONS->SetBranchAddress( (key+"_runNumber").c_str(), &runNumber);
  MUONS->SetBranchAddress( (branchString+"_px").c_str(), &m_px);
  MUONS->SetBranchAddress( (branchString+"_py").c_str(), &m_py);
  MUONS->SetBranchAddress( (branchString+"_pz").c_str(), &m_pz);
  MUONS->SetBranchAddress( (branchString+"_e").c_str(), &m_e);
  MUONS->SetBranchAddress( (branchString+"_cotTh").c_str(), &m_cotTh);
  MUONS->SetBranchAddress( (branchString+"_pt").c_str(), &m_pt);
  MUONS->SetBranchAddress( (branchString+"_eta").c_str(), &m_eta);
  MUONS->SetBranchAddress( (branchString+"_phi").c_str(), &m_phi);
  MUONS->SetBranchAddress( (branchString+"_charge").c_str(), &m_charge);
  MUONS->SetBranchAddress( (branchString+"_author").c_str(), &m_author);
  MUONS->SetBranchAddress( (branchString+"_isLoose").c_str(), &m_isLoose);
  MUONS->SetBranchAddress( (branchString+"_isMedium").c_str(), & m_isMedium);
  MUONS->SetBranchAddress( (branchString+"_isTight").c_str(), &m_isTight);
  MUONS->SetBranchAddress( (branchString+"_fitChi2").c_str(), &m_fitChi2);
  MUONS->SetBranchAddress( (branchString+"_matchChi2").c_str(), &m_matchChi2);
  MUONS->SetBranchAddress( (branchString+"_matchChi2OverDoF").c_str(), &m_matchChi2OverDoF);
  MUONS->SetBranchAddress( (branchString+"_nucone20").c_str(), &m_nucone20);
  MUONS->SetBranchAddress( (branchString+"_etcone10").c_str(), &m_etcone10);
  MUONS->SetBranchAddress( (branchString+"_etcone20").c_str(), &m_etcone20);
  MUONS->SetBranchAddress( (branchString+"_etcone30").c_str(), &m_etcone30);
  MUONS->SetBranchAddress( (branchString+"_etcone40").c_str(), &m_etcone40);
  MUONS->SetBranchAddress( (branchString+"_bestMatch").c_str(), &m_bestMatch);
  MUONS->SetBranchAddress( (branchString+"_isCombinedMuon").c_str(), &m_isCombinedMuon);
  MUONS->SetBranchAddress( (branchString+"_hasCombinedMuon").c_str(), &m_hasCombinedMuon);
  MUONS->SetBranchAddress( (branchString+"_hasInDetTrackParticle").c_str(), &m_hasInDetTrackParticle);

  MUONS->SetBranchAddress( (branchString+"_isStandAloneMuon").c_str(), &m_isStandAloneMuon);

  MUONS->SetBranchAddress( (branchString+"_hasMuonSpectrometerTrackParticle").c_str(), &m_hasMuonSpectrometerTrackParticle);
  MUONS->SetBranchAddress( (branchString+"_hasMuonExtrapolatedTrackParticle").c_str(), &m_hasMuonExtrapolatedTrackParticle);
  MUONS->SetBranchAddress( (branchString+"_hasCombinedMuonTrackParticle").c_str(), &m_hasCombinedMuonTrackParticle);
  MUONS->SetBranchAddress( (branchString+"_hasCluster").c_str(), &m_hasCluster);
  MUONS->SetBranchAddress( (branchString+"_isHighPt").c_str(), &m_isHighPt);
  MUONS->SetBranchAddress( (branchString+"_isLowPt").c_str(), &m_isLowPt);
  MUONS->SetBranchAddress( (branchString+"_numberOfBLayerHits").c_str(), &m_numberOfBLayerHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfPixelHits").c_str(), &m_numberOfPixelHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfSCTHits").c_str(), &m_numberOfSCTHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfTRTHits").c_str(), &m_numberOfTRTHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfTRTHighThresholdHits").c_str(), &m_numberOfTRTHighThresholdHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfMDTHits").c_str(), &m_numberOfMDTHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfCSCEtaHits").c_str(), &m_numberOfCSCEtaHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfCSCPhiHits").c_str(), &m_numberOfCSCPhiHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfRPCEtaHits").c_str(), &m_numberOfRPCEtaHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfRPCPhiHits").c_str(), &m_numberOfRPCPhiHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfTGCEtaHits").c_str(), &m_numberOfTGCEtaHits);
  MUONS->SetBranchAddress( (branchString+"_numberOfTGCPhiHits").c_str(), &m_numberOfTGCPhiHits);
  MUONS->SetBranchAddress( (branchString+"_index").c_str(), &m_index);
  MUONS->SetBranchAddress( (branchString+"_inDetTrackIndex").c_str(), &m_inDetTrackIndex);

  // Looping variable
  Int_t j;

  // Loop over the tree and fill the objects
  MUONS->GetEntry(index);
  Int_t nMu = m_index->size();
  MuonBag eventMuons; eventMuons.clear();
  if (nMu==0) return eventMuons;
  for (j=0; j<nMu; ++j) {
    Muon tmpMuon;
    tmpMuon.setEventNumber(eventNumber);
    tmpMuon.setLumiBlock(lumiBlock);
    tmpMuon.setRunNumber(runNumber);
    tmpMuon.setPx( (*m_px)[j]);
    tmpMuon.setPy( (*m_py)[j]);
    tmpMuon.setPz( (*m_pz)[j]);
    tmpMuon.setE( (*m_e)[j]);
    tmpMuon.setCotTheta( (*m_cotTh)[j]);
    tmpMuon.setPt( (*m_pt)[j]);
    tmpMuon.setEta( (*m_eta)[j]);
    tmpMuon.setPhi( (*m_phi)[j]);
    tmpMuon.setCharge( (*m_charge)[j]);
    tmpMuon.setAuthor( (*m_author)[j]);
    tmpMuon.setIsLoose( (*m_isLoose)[j]);
    tmpMuon.setIsMedium( (*m_isMedium)[j]);
    tmpMuon.setIsTight( (*m_isTight)[j]);
    tmpMuon.setFitChi2( (*m_fitChi2)[j]);
    tmpMuon.setMatchChi2( (*m_matchChi2)[j]);
    tmpMuon.setMatchChi2OverDoF( (*m_matchChi2OverDoF)[j]);
    tmpMuon.setNucone20( (*m_nucone20)[j]);
    tmpMuon.setEtcone10( (*m_etcone10)[j]);
    tmpMuon.setEtcone20( (*m_etcone20)[j]);
    tmpMuon.setEtcone30( (*m_etcone30)[j]);
    tmpMuon.setEtcone40( (*m_etcone40)[j]);
    tmpMuon.setBestMatch( (*m_bestMatch)[j]);
    tmpMuon.setIsCombinedMuon( (*m_isCombinedMuon)[j]);
    tmpMuon.setHasCombinedMuon( (*m_hasCombinedMuon)[j]);
    tmpMuon.setHasInDetTrackParticle( (*m_hasInDetTrackParticle)[j]);

    tmpMuon.setHasInDetTrackParticle( (*m_isStandAloneMuon)[j]);

    tmpMuon.setHasMuonSpectrometerTrackParticle( (*m_hasMuonSpectrometerTrackParticle)[j]);
    tmpMuon.setHasMuonExtrapolatedTrackParticle( (*m_hasMuonExtrapolatedTrackParticle)[j]);
    tmpMuon.setHasCombinedMuonTrackParticle( (*m_hasCombinedMuonTrackParticle)[j]);
    tmpMuon.setHasCluster( (*m_hasCluster)[j]);
    tmpMuon.setIsHighPt( (*m_isHighPt)[j]);
    tmpMuon.setIsLowPt( (*m_isLowPt)[j]);
    tmpMuon.setNumberOfBLayerHits( (*m_numberOfBLayerHits)[j]);
    tmpMuon.setNumberOfPixelHits( (*m_numberOfPixelHits)[j]);
    tmpMuon.setNumberOfSCTHits( (*m_numberOfSCTHits)[j]);
    tmpMuon.setNumberOfTRTHits( (*m_numberOfTRTHits)[j]);
    tmpMuon.setNumberOfTRTHighThresholdHits( (*m_numberOfTRTHighThresholdHits)[j]);
    tmpMuon.setNumberOfMDTHits( (*m_numberOfMDTHits)[j]);
    tmpMuon.setNumberOfCSCEtaHits( (*m_numberOfCSCEtaHits)[j]);
    tmpMuon.setNumberOfCSCPhiHits( (*m_numberOfCSCPhiHits)[j]);
    tmpMuon.setNumberOfRPCEtaHits( (*m_numberOfRPCEtaHits)[j]);
    tmpMuon.setNumberOfRPCPhiHits( (*m_numberOfRPCPhiHits)[j]);
    tmpMuon.setNumberOfTGCEtaHits( (*m_numberOfTGCEtaHits)[j]);
    tmpMuon.setNumberOfTGCPhiHits( (*m_numberOfTGCPhiHits)[j]);
    tmpMuon.setIndex( (*m_index)[j]);
    tmpMuon.setInDetTrackIndex( (*m_inDetTrackIndex)[j]);
    eventMuons.push_back(tmpMuon);
  }

  return eventMuons;

}

VertexBag NtupleReader::readVertexTree(Int_t index, std::string key, TTree* PRIVTX) {

  // Declare the vectors into which the tree will be read
  Int_t eventNumber(0);
  Int_t lumiBlock(0);
  Int_t runNumber(0);
  Double_t m_bsX(0);
  Double_t m_bsY(0);
  Double_t m_bsZ(0);
  Double_t m_bsSigX(0);
  Double_t m_bsSigY(0);
  Double_t m_bsSigZ(0);
  std::vector<int> * m_vtxType(0);
  std::vector<double> * m_vtxCovXX(0);
  std::vector<double> *m_vtxCovXY(0);
  std::vector<double> *m_vtxCovXZ(0);
  std::vector<double> *m_vtxCovYZ(0);
  std::vector<double> *m_vtxCovYY(0);
  std::vector<double> *m_vtxCovZZ(0);
  std::vector<double> *m_vtxX(0);
  std::vector<double> *m_vtxY(0);
  std::vector<double> *m_vtxZ(0);
  std::vector<double> *m_vtxChi2(0);
  std::vector<int> * m_vtxNDF(0);
  std::vector<int> *m_index(0);

  // Set branch addresses
  PRIVTX->SetBranchAddress((key+"_eventNumber").c_str(),&eventNumber);
  PRIVTX->SetBranchAddress((key+"_runNumber").c_str(),&runNumber);
  PRIVTX->SetBranchAddress((key+"_lumiBlock").c_str(),&lumiBlock);
  PRIVTX->SetBranchAddress("BS_POS_X",&m_bsX);
  PRIVTX->SetBranchAddress("BS_POS_Y",&m_bsY);
  PRIVTX->SetBranchAddress("BS_POS_Z",&m_bsZ);
  PRIVTX->SetBranchAddress("BS_SIGMA_X",&m_bsSigX);
  PRIVTX->SetBranchAddress("BS_SIGMA_Y",&m_bsSigY);
  PRIVTX->SetBranchAddress("BS_SIGMA_Z",&m_bsSigZ);
  PRIVTX->SetBranchAddress((key+"_xposition").c_str(),&m_vtxX);
  PRIVTX->SetBranchAddress((key+"_yposition").c_str(),&m_vtxY);
  PRIVTX->SetBranchAddress((key+"_zposition").c_str(),&m_vtxZ);
  PRIVTX->SetBranchAddress((key+"_xxPosCov").c_str(),&m_vtxCovXX);
  PRIVTX->SetBranchAddress((key+"_yyPosCov").c_str(),&m_vtxCovYY);
  PRIVTX->SetBranchAddress((key+"_zzPosCov").c_str(),&m_vtxCovZZ);
  PRIVTX->SetBranchAddress((key+"_xyPosCov").c_str(),&m_vtxCovXY);
  PRIVTX->SetBranchAddress((key+"_yzPosCov").c_str(),&m_vtxCovYZ);
  PRIVTX->SetBranchAddress((key+"_xzPosCov").c_str(),&m_vtxCovXZ);
  PRIVTX->SetBranchAddress((key+"_chi2").c_str(),&m_vtxChi2);
  PRIVTX->SetBranchAddress((key+"_ndf").c_str(),&m_vtxNDF);
  PRIVTX->SetBranchAddress((key+"_type").c_str(),&m_vtxType);
  PRIVTX->SetBranchAddress((key+"_index").c_str(),&m_index);

  // Looping variable
  Int_t j;

  // Loop over the tree and fill the objects
  PRIVTX->GetEntry(index);
  Int_t nPriVtx = m_index->size();
  VertexBag eventVertices; eventVertices.clear();
  if (nPriVtx==0) return eventVertices;
  for (j=0; j<nPriVtx; ++j) {
    Vertex tmpPriVtx;
    tmpPriVtx.setEvent(eventNumber);
    tmpPriVtx.setLumiBlock(lumiBlock);
    tmpPriVtx.setRun(runNumber);
    tmpPriVtx.setBSX( m_bsX );
    tmpPriVtx.setBSY( m_bsY );
    tmpPriVtx.setBSZ( m_bsZ );
    tmpPriVtx.setBSSigX( m_bsSigX );
    tmpPriVtx.setBSSigY( m_bsSigY );
    tmpPriVtx.setBSSigZ( m_bsSigZ );
    tmpPriVtx.setType( (*m_vtxType)[j]);
    tmpPriVtx.setCovXX( (*m_vtxCovXX)[j]);
    tmpPriVtx.setCovXY( (*m_vtxCovXY)[j]);
    tmpPriVtx.setCovXZ( (*m_vtxCovXZ)[j]);
    tmpPriVtx.setCovYZ( (*m_vtxCovYZ)[j]);
    tmpPriVtx.setCovYY( (*m_vtxCovYY)[j]);
    tmpPriVtx.setCovZZ( (*m_vtxCovZZ)[j]);
    tmpPriVtx.setX( (*m_vtxX)[j]);
    tmpPriVtx.setY( (*m_vtxY)[j]);
    tmpPriVtx.setZ( (*m_vtxZ)[j]);
    tmpPriVtx.setChi2( (*m_vtxChi2)[j]);
    tmpPriVtx.setNDF( (*m_vtxNDF)[j]);
    tmpPriVtx.setIndex( (*m_index)[j]);
    eventVertices.push_back(tmpPriVtx);
  }

  return eventVertices;

}

TriggerBag NtupleReader::readTriggerTree(Int_t index, std::string key, TTree* TRIG) {

  // Declare the vectors into which the tree will be read
  Int_t eventNumber(0);
  Int_t runNumber(0);
  std::vector<int> * m_l1prescale(0);
  std::vector<int> * m_l2prescale(0);
  std::vector<int> * m_efprescale(0);
  std::vector<std::string> * m_l1name(0);
  std::vector<std::string> * m_l2name(0);
  std::vector<std::string> * m_efname(0);

  // Set branch addresses
  TRIG->SetBranchAddress((key+"_eventNumber").c_str(),&eventNumber);
  TRIG->SetBranchAddress((key+"_runNumber").c_str(),&runNumber);
  TRIG->SetBranchAddress((key+"_L1_trigger_name").c_str(),&m_l1name);
  TRIG->SetBranchAddress((key+"_L2_trigger_name").c_str(),&m_l2name);
  TRIG->SetBranchAddress((key+"_EF_trigger_name").c_str(),&m_efname);
  TRIG->SetBranchAddress((key+"_L1_prescale").c_str(),&m_l1prescale);
  TRIG->SetBranchAddress((key+"_L2_prescale").c_str(),&m_l2prescale);
  TRIG->SetBranchAddress((key+"_EF_prescale").c_str(),&m_efprescale);

  // Looping variable
  Int_t j;

  // Loop over the tree and fill the objects
  TRIG->GetEntry(index);
  Int_t nL1 = m_l1name->size();
  Int_t nL2 = m_l2name->size();
  Int_t nEF = m_efname->size();
  TriggerBag eventTriggers; eventTriggers.clear();
  if (nL1==0 && nL2==0 && nEF==0) return eventTriggers;
  for (j=0; j<nL1; ++j) {
    TriggerDecision tmpTrig;
    tmpTrig.setEvent(eventNumber);
    tmpTrig.setRun(runNumber);
    tmpTrig.setPrescale( (*m_l1prescale)[j]);
    tmpTrig.setName( (*m_l1name)[j]);
    eventTriggers.push_back(tmpTrig);
  }
  for (j=0; j<nL2; ++j) {
    TriggerDecision tmpTrig;
    tmpTrig.setEvent(eventNumber);
    tmpTrig.setRun(runNumber);
    tmpTrig.setPrescale( (*m_l2prescale)[j]);
    tmpTrig.setName( (*m_l2name)[j]);
    eventTriggers.push_back(tmpTrig);
  }
  for (j=0; j<nEF; ++j) {
    TriggerDecision tmpTrig;
    tmpTrig.setEvent(eventNumber);
    tmpTrig.setRun(runNumber);
    tmpTrig.setPrescale( (*m_efprescale)[j]);
    tmpTrig.setName( (*m_efname)[j]);
    eventTriggers.push_back(tmpTrig);
  }

  return eventTriggers;

}


std::vector<TrackBag> NtupleReader::allTracks(std::string key, TTree* TRACKS) {

  // Get the number of entries
  Int_t nEvTRACKS = (Int_t)TRACKS->GetEntries();

  // Start loop
  std::vector<TrackBag> allTracks;
  for (Int_t i=0; i<nEvTRACKS; ++i) {
    TrackBag theTracks = readTrackTree(i,key,TRACKS);
    allTracks.push_back(theTracks);
  }

  return allTracks;

}

std::vector<MuonBag> NtupleReader::allMuons(std::string key, TTree* MUONS) {

  // Get the number of entries
  Int_t nEvMUONS = (Int_t)MUONS->GetEntries();

  // Start loop
  std::vector<MuonBag> allMuons;
  for (Int_t i=0; i<nEvMUONS; ++i) {
    MuonBag theMuons = readMuonTree(i,key,MUONS);
    allMuons.push_back(theMuons);
  }

  return allMuons;

}

std::vector<VertexBag> NtupleReader::allVertices(std::string key, TTree* PRIVTX) {

  // Get the number of entries
  Int_t nEvPRIVTX = (Int_t)PRIVTX->GetEntries();

  // Start loop
  std::vector<VertexBag> allVertices;
  for (Int_t i=0; i<nEvPRIVTX; ++i) {
    VertexBag theVertices = readVertexTree(i,key,PRIVTX);
    allVertices.push_back(theVertices);
  }

  return allVertices;

}

std::vector<TriggerBag> NtupleReader::allTriggers(std::string key, TTree* TRIG) {

  // Get the number of entries
  Int_t nEvTRIG = (Int_t)TRIG->GetEntries();

  // Start loop
  std::vector<TriggerBag> allTriggers;
  for (Int_t i=0; i<nEvTRIG; ++i) {
    TriggerBag theTriggers = readTriggerTree(i,key,TRIG);
    allTriggers.push_back(theTriggers);
  }

  return allTriggers;

}

#endif

