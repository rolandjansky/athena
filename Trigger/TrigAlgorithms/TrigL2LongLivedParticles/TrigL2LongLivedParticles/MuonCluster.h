/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MuonCluster_h__
#define __MuonCluster_h__
/*
  MuonCluster Algorithm:  Trigger/TrigAlgorithm/TrigMuonCluster

  Authors: Stefano.Giagu@cern.ch
           Antonio.Policicchio@cern.ch

  Algorithm for selection of Long-Lived neutral particle decays (Hidden Valley sector):
  Clustering of LVL1 muon RoIs, access to LVL2 jets and SITRACK tracks.
*/
#include <string>
#include <vector>
#include <set>
#include <map>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#define kMAX_ROI 20

class MuonCluster : public HLT::AllTEAlgo {
public:
  enum {
    ITIMER_INIT,
    ITIMER_CLUSTER,
    ITIMER_JETS,
    ITIMER_TRACKS,
    ITIMER_FINAL
  };

public:
  /** Constructor */
  MuonCluster(const std::string& name, ISvcLocator* svc);
  ~MuonCluster();

  /** hltBeginRun() */
  HLT::ErrorCode hltBeginRun();
  /** hltEndRun() */
  HLT::ErrorCode hltEndRun();

  /** hltInitialize() */
  HLT::ErrorCode hltInitialize();
  /** hltFinalize() */
  HLT::ErrorCode hltFinalize();
  /** hltExecute(), main code of the algorithm */
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			    unsigned int output);



  /** A function which clears internal data for a new event */
  void clearEvent();
  /**
   * @brief This method overwrites the default one (doing nothing) in the algo.h class
   *
   * This is used to reset the internal caching mechanism of this MuonCluster algorithm.
   */
  HLT::ErrorCode hltEndEvent() { m_useCachedResult = false; m_old_feature = 0; m_clu_feature = 0; m_cachedTE=0; return HLT::OK; }

  // monitored quantities
  std::vector<double> m_RoiEta;
  std::vector<double> m_RoiPhi;

protected:
  
  typedef struct  {
    float eta;
    float phi;
    int nroi;
  } lvl1_muclu_roi;
  

protected:
  // JobOption properties
  /** A property which specifies the radius of the cluster */
  float mDeltaR;
  /** A property which specifies the matching of the jet and of the cluster */
  float mDeltaRJet;

  /** Eta of the Center RoI cluster, for monitoring purpose */
  float mCluEta;
  /** Phi of the Center RoI cluster, for monitoring purpose */
  float mCluPhi;
  /** Numbers of Roi in cluster */
  int mCluNum;
  /** Number of Jet with Log(h/e)<0.5 **/
  int mNumJet;

  /** Minimum Et for jets in calorimeter based isolation. */
  double m_minJetEt;  

  /** Minimum Pt for tracks in ID based isolation. */
  double m_PtMinID;
  /** A property which specifies the matching of the track and of the cluster */
  float mDeltaRTrk;
  /** Cut on Tracks */
  int mNumTrk;

  /** calculcate the deltaR between two Rois */
  float DeltaR(std::vector<const LVL1::RecMuonRoI*>::const_iterator, lvl1_muclu_roi );
  float DeltaR(lvl1_muclu_roi , lvl1_muclu_roi );
  
  //Output parameters. Keep old container so we don't break Frozen Tier0. Should be removed in the future!
  std::string m_featureLabel; //!< label for the mucluster  feature in the HLT Navigation
  std::string m_featureLabelOld; //!< label for the mucluster  feature in the HLT Navigation
  bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode
  TrigMuonClusterFeature *m_old_feature;  //!< internal caching: m_old_feature of the first execution
  xAOD::TrigCompositeContainer *m_clu_feature;    //!< internal caching: m_clu_feature of the first execution
  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution

  // Services
  /** A service handle to StoreGate */
  ServiceHandle<StoreGateSvc> mStoreGate;
  /** Timers */
  std::vector<TrigTimer*> mTimers;
};

#endif // __MuonCluster_h__
