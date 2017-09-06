/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*************************************************************************************
 * @package : TrigMinBias
 * @author  : Regina Kwee <Regina.Kwee@cern.ch> - CERN, Humboldt University of Berlin
 *************************************************************************************/

#ifndef TRIGMINBIAS_TRIGTRACKCOUNTER_H
#define TRIGMINBIAS_TRIGTRACKCOUNTER_H

#include "TrigInterfaces/FexAlgo.h"
#include <string>
#include "TrkTrack/TrackCollection.h"
#include "xAODTrigMinBias/TrigHisto2D.h"

class StoreGateSvc;

/** @class TrigTrackCounter

A feature extraction algorithm to extract online low pt reconstructed
track distributions: z0 vs pt and eta vs phi.

*/
class TrigTrackCounter: public HLT::FexAlgo {
 public:
  
  TrigTrackCounter(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorithm constructor
  ~TrigTrackCounter(void);

  HLT::ErrorCode hltInitialize();  //!< hlt initialize
  HLT::ErrorCode hltFinalize();    //!< hlt finalize
  HLT::ErrorCode hltExecute(const HLT::TriggerElement *, HLT::TriggerElement* outputTE);
  
 private:
  /** A histogram of the online reconstructed low pt tracks: z0 vs pt.*/
  xAOD::TrigHisto2D* m_z0_pt;
  
  /** A histogram of the online reconstructed low pt tracks: eta vs phi*/
  xAOD::TrigHisto2D* m_eta_phi;
  /** thresholds for filling m_eta_phi histo */
  float m_pt_min;
  float m_z0_max;


  /** TrigHisto input parameters */
  unsigned int m_hZ0Bins;
  float m_hZ0Min, m_hZ0Max;
  unsigned int m_hPtBins;
  float m_hPtMin, m_hPtMax;
  unsigned int m_hEtaBins;
  float m_hEtaMin, m_hEtaMax;
  unsigned int m_hPhiBins;
  float m_hPhiMin, m_hPhiMax;

  /** InputTrackContainerName */
  std::string m_trkContainerName;

  /** Variables used to monitor data stored in TrigTrackCounts */ 
  std::vector<float> m_trkZ0; 
  std::vector<float> m_trkPt; 
  std::vector<float> m_trkEta; 
  std::vector<float> m_trkPhi; 

  /** Other monitoring variables */
  int m_ntrks;

  /** Timers */
  TrigTimer                   *m_formFeaturesTimer;
  TrigTimer                   *m_getTracksTimer;

  /** A pointer to the track collection to be retrieved */
  const TrackCollection       *m_recoTracks;
};

#endif
