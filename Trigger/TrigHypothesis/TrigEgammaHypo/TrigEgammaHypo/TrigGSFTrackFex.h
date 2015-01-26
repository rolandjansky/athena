// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigGSFTrackFex.h
 **
 **   Description: Fex algo to refit tracks with GSF. Mostly taken from offline egamma:
 **                egammaTools/EMBremCollectionBuilder
 **
 **   Authors: R. M. White     <ryan.white@cern.ch>
 **
 **   Created: January 2015 
 **
 **************************************************************************/ 

#ifndef TRIG_TRIGGSFTRACKFEX_H 
#define TRIG_TRIGGSFTRACKFEX_H

// standard stuff
#include <vector>
#include <cmath> 

// general athena stuff
#include "GaudiKernel/IToolSvc.h"

// trigger includes
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrkTrack/TrackCollection.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

class IegammaTrkRefitterTool;
class IegammaCheckEnergyDepositTool;
class IEMExtrapolationTools;

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace Trk
{
    class ITrackParticleCreatorTool;
    class ITrackSlimmingTool;
    class ITrackSummaryTool;
}

class CaloCluster;
class TrigGSFTrackFex : public HLT::FexAlgo  {
  
 public:

  TrigGSFTrackFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigGSFTrackFex();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
 private:
  
  
  // to set Accept-All mode 
  bool m_acceptAll;

  /** @brief Refit of track */
  StatusCode refitTrack(const xAOD::TrackParticle* tmpTrkPart);
  /** @brief broad track selection */
  bool Select(const xAOD::CaloCluster* cluster,const xAOD::TrackParticle* track) const;
  /** @brief CandidateMatchHelper */
  double CorrectedEta(double clusterEta,double z_first,bool isEndCap) const;
   /** @brief CandidateMatchHelper */
  double PhiROT(double Et,double Eta, int charge, double r_first ,bool isEndCap) const;
  /** @brief The track refitter */
  ToolHandle<IegammaTrkRefitterTool>  m_trkRefitTool;
  /** @brief Tool to create track particle */
  ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
  /** @brief Tool to slim tracks  */
  ToolHandle<Trk::ITrackSlimmingTool>     m_slimTool;
  /** @brief Tool for Track summary  */
  ToolHandle<Trk::ITrackSummaryTool>     m_summaryTool;
  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool;

  /** @Cut on minimum silicon hits*/
  int                               m_MinNoSiHits;
  /** @brief broad cut on deltaEta*/
  double                m_broadDeltaEta;
  /** @brief broad cut on deltaPhi*/
  double                m_broadDeltaPhi;
  /** @narrow windows*/
  double                m_narrowDeltaEta;
  double                m_narrowDeltaPhi;
  double                m_narrowDeltaPhiBrem;
  double                m_narrowRescale;
  double                m_narrowRescaleBrem;
  /** @brief helper for */
  CaloPhiRange          m_phiHelper;
  //collections
  const xAOD::TrackParticleContainer*  m_trackTES;
  //      const TrackParticleTruthCollection* m_trackTruthTES;
  TrackCollection*                    m_finalTracks;
  xAOD::TrackParticleContainer*        m_finalTrkPartContainer;

  // Monitoring, counters
  unsigned int m_AllClusters;
  unsigned int m_AllTracks;
  unsigned int m_AllTRTTracks;
  unsigned int m_AllSiTracks;
  unsigned int m_SelectedTracks;
  unsigned int m_FailedFitTracks;
  unsigned int m_FailedSiliconRequirFit;
  unsigned int m_RefittedTracks;

  // Timers
  TrigTimer* m_totalTimer;
  TrigTimer* m_toolTimer;
};

#endif
