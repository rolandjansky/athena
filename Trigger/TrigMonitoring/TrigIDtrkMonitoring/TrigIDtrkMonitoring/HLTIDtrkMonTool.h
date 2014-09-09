/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *  @file HLTIDtrkMonTool.h
 *
 *  Contact:
 *  @author Erkcan.Ozcan (AT cern DOT ch) 
 *  @author Stefan.Ask (AT cern DOT ch) 
 *  
 *  Comment: Produce Tier0 histograms for DQM display
 *           L2: Basic Online Track Param. Histograms (IDSCAN/SiTrack)
 *           EF: Basic Online Track Param. Histograms 
 *           Combined L2/EF/Offline: A few Efficiency and Resolution Histrograms 
 *
 *  Todo: Make plots aware of warm period, through DCS info (?)
 *        
 */

#ifndef HLTIDTRKMONTOOL_H
#define HLTIDTRKMONTOOL_H

#include <string>
#include <vector>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

// ROOT
#include "TLorentzVector.h" 

// Forward declarations
class IInterface;
class StoreGateSvc;
class TH1I;
class TH1F;
class StatusCode;

class simpleTrack : public TLorentzVector { public: float d0, z0, nPixHits, nSctHits, nTrtHits; };

class HLTIDtrkMonTool : public IHLTMonTool
{

 public:
  HLTIDtrkMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~HLTIDtrkMonTool();

  StatusCode init();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); 
#endif

  StatusCode fill();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
#endif
  
  
 private:
  std::vector< simpleTrack > GetEFVectors(const DataHandle<Rec::TrackParticleContainer>);
  std::vector< simpleTrack > GetL2Vectors(const DataHandle<TrigInDetTrackCollection>);
  template <class track1, class track2>
    std::map<int, std::vector<int> >  MatchTracks(const std::vector< track1 > &, const std::vector< track2 > & , double );
  template <class tTrack>
    void TrackHoles(int, 
		    const std::vector< const Rec::TrackParticle* >, 
		    const std::vector< tTrack* >, 
		    std::map<int, std::vector<int> > &, 
		    std::string);
  //  bool* getPattern(const Rec::TrackParticle& track);
  void getPattern(const Rec::TrackParticle& track, bool oLayers[19]);
  //  bool* getPattern(const TrigInDetTrack& track);
  void getPattern(const TrigInDetTrack& track, bool tLayers[19]);
  template <class track1, class track2>
    void EfficiencyResidual(int, 
			    const std::vector< track1 > &, 
			    const std::vector< track2 > &, 
			    std::map<int, std::vector<int> > &, 
			    std::string);
  void EfficiencyOR(int);


 private:
  //+++ Configuration
  bool                     m_doMonCommon;
  std::string              m_EFType; 
  std::string              m_IDSCANType;
  std::string              m_SiTrackType;
  std::string              m_TRTType;
  std::vector<std::string> m_EFSliceName;
  std::vector<std::string> m_MatchPairs;
  std::string              m_OfflineCollection;
  std::string              m_CombEFSlice;
  std::string              m_CombIDSCANSlice;
  std::string              m_CombTRTSlice;
  std::string              m_CombSiTrackSlice;
  double                   m_DeltaR;
  double                   m_PtMin;
  double                   m_EtaMax;
  double                   m_Z0Max;
  double                   m_D0Max;
  int                      m_SiHitMin;

  //+++ L2/EF/Offline Comparison
  std::vector< simpleTrack > m_EFTrackVectors;
  std::vector< simpleTrack > m_OfflineTrackVectors;
  std::vector< simpleTrack > m_IDSCANTrackVectors;
  std::vector< simpleTrack > m_SiTrackTrackVectors;
  std::vector< simpleTrack > m_TRTTrackVectors;
  //  std::vector< Rec::TrackParticle* > m_EFRawTrackVectors;
  //  std::vector< Rec::TrackParticle* > m_OfflineRawTrackVectors;
  std::vector< const Rec::TrackParticle* > m_EFRawTrackVectors;
  std::vector< const Rec::TrackParticle* > m_OfflineRawTrackVectors;
  std::vector< TrigInDetTrack* > m_IDSCANRawTrackVectors;
  std::vector< TrigInDetTrack* > m_SiTrackRawTrackVectors;
  std::map<int,std::vector<int> > m_OfflineToEF;
  std::map<int,std::vector<int> > m_OfflineToIDSCAN;
  std::map<int,std::vector<int> > m_OfflineToSiTrack;
  std::map<int,std::vector<int> > m_OfflineToTRT;
  std::vector<int> totalMatchedTracks;

  //+++ Histograms
  std::string m_TrigEFTrack_pt;
  std::string m_TrigEFTrack_eta;
  std::string m_TrigEFTrack_phi;
  std::string m_TrigEFTrack_z0;
  std::string m_TrigEFTrack_d0;
  std::string m_TrigEFTrack_qoverp;
  std::string m_TrigEFTrack_NPixelHits;
  std::string m_TrigEFTrack_NSctHits;
  std::string m_TrigEFTrack_NTRTHits;
  std::string m_TrigEFTrack_ChiNdof;
  std::string m_TrigEFROI_NTracks;
  std::string m_TrigEFROI_NVertices;
  std::string m_TrigEffEta;
  std::string m_TrigEffPhi;
  std::string m_TrigEffZ0;
  std::string m_TrigEffD0;

  std::string m_TrigOfflHoles;
  std::string m_TrigTrigHoles;
  std::string m_TrigNOfflHoles;
  std::string m_TrigNTrigHoles;

};

#endif
