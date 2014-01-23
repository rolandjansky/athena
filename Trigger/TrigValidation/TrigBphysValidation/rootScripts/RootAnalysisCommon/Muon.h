/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************
// Muon.h
// Simple representation of a muon
// Can be created from the n-tuple tree
// made by GetMuonSimple in BPhysAnalysisTools
//********************************************

class Muon {
  
private:
  Int_t m_event;
  Int_t m_lumiBlock;
  Int_t m_run;
  Double_t m_px;
  Double_t m_py;
  Double_t m_pz;
  Double_t m_e;
  Double_t m_cotTheta;
  Double_t m_qOverP;
  Double_t m_pt;
  Double_t m_eta;
  Double_t m_phi;
  Int_t m_charge;
  Int_t m_author;
  bool m_isLoose;
  bool m_isMedium;
  bool m_isTight;
  Double_t m_fitChi2;
  Double_t m_matchChi2;
  Double_t m_matchChi2OverDoF;
  Double_t m_nucone20;
  Double_t m_etcone10;
  Double_t m_etcone20;
  Double_t m_etcone30;
  Double_t m_etcone40;
  bool m_bestMatch;
  bool m_isCombinedMuon;
  bool m_hasCombinedMuon;
  bool m_hasInDetTrackParticle;
  bool m_hasMuonSpectrometerTrackParticle;
  bool m_hasMuonExtrapolatedTrackParticle;
  bool m_hasCombinedMuonTrackParticle;
  bool m_hasCluster;
  bool m_isHighPt;
  bool m_isLowPt;
  Int_t m_numberOfBLayerHits;
  Int_t m_numberOfPixelHits;
  Int_t m_numberOfSCTHits;
  Int_t m_numberOfTRTHits;
  Int_t m_numberOfTRTHighThresholdHits;
  Int_t m_numberOfMDTHits;
  Int_t m_numberOfCSCEtaHits;
  Int_t m_numberOfCSCPhiHits;
  Int_t m_numberOfRPCEtaHits;
  Int_t m_numberOfRPCPhiHits;
  Int_t m_numberOfTGCEtaHits;
  Int_t m_numberOfTGCPhiHits;
  Double_t m_segmentDeltaEta;
  Double_t m_segmentDeltaPhi;
  Int_t m_index;
  Int_t m_inDetTrackIndex;

public:
  // Constructor
  Muon(void) {
      m_event = 0;
      m_lumiBlock = 0;
      m_run = 0;
      m_px = 0.0;
      m_py = 0.0;
      m_pz = 0.0;
      m_e = 0.0;
      m_cotTheta = 0.0;
      m_qOverP = 0.0;
      m_pt = 0.0;
      m_eta = 0.0;
      m_phi = 0.0;
      m_charge = 0;
      m_author = 0;
      m_isLoose = false;
      m_isMedium = false;
      m_isTight = false;
      m_fitChi2 = 0.0;
      m_matchChi2 = 0.0;
      m_matchChi2OverDoF = 0.0;
      m_nucone20 = 0.0;
      m_etcone10 = 0.0;
      m_etcone20 = 0.0;
      m_etcone30 = 0.0;
      m_etcone40 = 0.0;
      m_bestMatch = false;
      m_isCombinedMuon = false;
      m_hasCombinedMuon = false;
      m_hasInDetTrackParticle = false;
      m_hasMuonSpectrometerTrackParticle = false;
      m_hasMuonExtrapolatedTrackParticle = false;
      m_hasCombinedMuonTrackParticle = false;
      m_hasCluster = false;
      m_isHighPt = false;
      m_isLowPt = false;
      m_numberOfBLayerHits = 0;
      m_numberOfPixelHits = 0;
      m_numberOfSCTHits = 0;
      m_numberOfTRTHits = 0;
      m_numberOfTRTHighThresholdHits = 0;
      m_numberOfMDTHits = 0;
      m_numberOfCSCEtaHits = 0;
      m_numberOfCSCPhiHits = 0;
      m_numberOfRPCEtaHits = 0;
      m_numberOfRPCPhiHits = 0;
      m_numberOfTGCEtaHits = 0;
      m_numberOfTGCPhiHits = 0;
      m_segmentDeltaEta = 0.0;
      m_segmentDeltaPhi = 0.0;	
      m_index = 0;
      m_inDetTrackIndex = 0;
  }
  // Set methods
  void setEventNumber(Int_t input) {m_event = input;}
  void setLumiBlock(Int_t input) {m_lumiBlock = input;}
  void setRunNumber(Int_t input) {m_run = input;}
  void setPx(Double_t input) {m_px = input;}
  void setPy(Double_t input) {m_py = input;}
  void setPz(Double_t input) {m_pz = input;}
  void setE(Double_t input) {m_e = input;}
  void setCotTheta(Double_t input) {m_cotTheta = input;}
  void setQOverP(Double_t input) {m_qOverP = input;}
  void setPt(Double_t input) {m_pt = input;}
  void setEta(Double_t input) {m_eta = input;}
  void setPhi(Double_t input) {m_phi = input;}
  void setCharge(Int_t input) {m_charge = input;}
  void setAuthor(Int_t input) {m_author = input;}
  void setIsLoose(bool input) {m_isLoose = input;}
  void setIsMedium(bool input) {m_isMedium = input;}
  void setIsTight(bool input) {m_isTight = input;}
  void setFitChi2(Double_t input) {m_fitChi2 = input;}
  void setMatchChi2(Double_t input) {m_matchChi2 = input;}
  void setMatchChi2OverDoF(Double_t input) {m_matchChi2OverDoF = input;}
  void setNucone20(Double_t input) {m_nucone20 = input;}
  void setEtcone10(Double_t input) {m_etcone10 = input;}
  void setEtcone20(Double_t input) {m_etcone20 = input;}
  void setEtcone30(Double_t input) {m_etcone30 = input;}
  void setEtcone40(Double_t input) {m_etcone40 = input;}
  void setBestMatch(bool input) {m_bestMatch = input;}
  void setIsCombinedMuon(bool input) {m_isCombinedMuon = input;}
  void setHasCombinedMuon(bool input) {m_hasCombinedMuon = input;}
  void setHasInDetTrackParticle(bool input) {m_hasInDetTrackParticle = input;}
  void setHasMuonSpectrometerTrackParticle(bool input) {m_hasMuonSpectrometerTrackParticle = input;}
  void setHasMuonExtrapolatedTrackParticle(bool input) {m_hasMuonExtrapolatedTrackParticle = input;}
  void setHasCombinedMuonTrackParticle(bool input) {m_hasCombinedMuonTrackParticle = input;}
  void setHasCluster(bool input) {m_hasCluster = input;}
  void setIsHighPt(bool input) {m_isHighPt = input;}
  void setIsLowPt(bool input) {m_isLowPt = input;}
  void setNumberOfBLayerHits(Int_t input) {m_numberOfBLayerHits = input;}
  void setNumberOfPixelHits(Int_t input) {m_numberOfPixelHits = input;}
  void setNumberOfSCTHits(Int_t input) {m_numberOfSCTHits = input;}
  void setNumberOfTRTHits(Int_t input) {m_numberOfTRTHits = input;}
  void setNumberOfTRTHighThresholdHits(Int_t input) {m_numberOfTRTHighThresholdHits = input;}
  void setNumberOfMDTHits(Int_t input) {m_numberOfMDTHits = input;}
  void setNumberOfCSCEtaHits(Int_t input) {m_numberOfCSCEtaHits = input;}
  void setNumberOfCSCPhiHits(Int_t input) {m_numberOfCSCPhiHits = input;}
  void setNumberOfRPCEtaHits(Int_t input) {m_numberOfRPCEtaHits = input;}
  void setNumberOfRPCPhiHits(Int_t input) {m_numberOfCSCPhiHits = input;}
  void setNumberOfTGCEtaHits(Int_t input) {m_numberOfTGCEtaHits = input;}
  void setNumberOfTGCPhiHits(Int_t input) {m_numberOfTGCPhiHits = input;}
  void setSegmentDeltaEta(Double_t input) {m_segmentDeltaEta = input;}
  void setSegmentDeltaPhi(Double_t input) {m_segmentDeltaPhi = input;}

  void setIndex(Int_t input) {m_index = input;}
  void setInDetTrackIndex(Int_t input) {m_inDetTrackIndex = input;}

  // Get methods
  Int_t eventNumber(void) {return m_event;}
  Int_t lumiBlock(void) {return m_lumiBlock;}
  Int_t runNumber(void) {return m_run;}
  Double_t px(void) {return m_px;}
  Double_t py(void) {return m_py;}
  Double_t pz(void) {return m_pz;}
  Double_t e(void) {return m_e;}
  Double_t cotTheta(void) {return m_cotTheta;}
  Double_t qOverP(void) {return m_qOverP;}
  Double_t pt(void) {return m_pt;}
  Double_t eta(void) {return m_eta;}
  Double_t phi(void) {return m_phi;}
  Int_t charge(void) {return m_charge;}
  Int_t author(void) {return m_author;}
  bool isLoose(void) {return m_isLoose;}
  bool isMedium(void) {return m_isMedium;}
  bool isTight(void) {return m_isTight;}
  Double_t fitChi2(void) {return m_fitChi2;}
  Double_t matchChi2(void) {return m_matchChi2;}
  Double_t matchChi2OverDoF(void) {return m_matchChi2OverDoF;}
  Double_t nucone20(void) {return m_nucone20;}
  Double_t etcone10(void) {return m_etcone10;}
  Double_t etcone20(void) {return m_etcone20;}
  Double_t etcone30(void) {return m_etcone30;}
  Double_t etcone40(void) {return m_etcone40;}
  bool bestMatch(void) {return m_bestMatch;}
  bool isCombinedMuon(void) {return m_isCombinedMuon;}
  bool hasCombinedMuon(void) {return m_hasCombinedMuon;}
  bool hasInDetTrackParticle(void) {return m_hasInDetTrackParticle;}
  bool hasMuonSpectrometerTrackParticle(void) {return m_hasMuonSpectrometerTrackParticle;}
  bool hasMuonExtrapolatedTrackParticle(void) {return m_hasMuonExtrapolatedTrackParticle;}
  bool hasCombinedMuonTrackParticle(void) {return m_hasCombinedMuonTrackParticle;}
  bool hasCluster(void) {return m_hasCluster;}
  bool isHighPt(void) {return m_isHighPt;}
  bool isLowPt(void) {return m_isLowPt;}
  Int_t numberOfBLayerHits(void) {return m_numberOfBLayerHits;}
  Int_t numberOfPixelHits(void) {return m_numberOfPixelHits;}
  Int_t numberOfSCTHits(void) {return m_numberOfSCTHits;}
  Int_t numberOfTRTHits(void) {return m_numberOfTRTHits;}
  Int_t numberOfTRTHighThresholdHits(void) {return m_numberOfTRTHighThresholdHits;}
  Int_t numberOfMDTHits(void) {return m_numberOfMDTHits;}
  Int_t numberOfCSCEtaHits(void) {return m_numberOfCSCEtaHits;}
  Int_t numberOfCSCPhiHits(void) {return m_numberOfCSCPhiHits;}
  Int_t numberOfRPCEtaHits(void) {return m_numberOfRPCEtaHits;}
  Int_t numberOfRPCPhiHits(void) {return m_numberOfCSCPhiHits;}
  Int_t numberOfTGCEtaHits(void) {return m_numberOfTGCEtaHits;}
  Int_t numberOfTGCPhiHits(void) {return m_numberOfTGCPhiHits;}
  Double_t segmentDeltaEta(void) {return m_segmentDeltaEta;}
  Double_t segmentDeltaPhi(void) {return m_segmentDeltaPhi;} 
  Int_t index(void) {return m_index;}
  Int_t inDetTrackIndex(void) {return m_inDetTrackIndex;}

}; // End of class definition
