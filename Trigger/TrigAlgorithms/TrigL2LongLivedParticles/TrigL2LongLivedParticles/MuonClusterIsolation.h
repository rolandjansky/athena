/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2LONGLIVEDPARTICLES_MUONCLUSTERISOLATION_H
#define TRIGL2LONGLIVEDPARTICLES_MUONCLUSTERISOLATION_H
/*
  MuonClusterIsolation Algorithm:  Trigger/TrigAlgorithm/TrigMuonClusterIsolation

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
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#define kMAX_ROI 20

class MuonClusterIsolation : public HLT::AllTEAlgo {
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
  MuonClusterIsolation(const std::string& name, ISvcLocator* svc);
  ~MuonClusterIsolation();

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
   * This is used to reset the internal caching mechanism of this MuonClusterIsolation algorithm.
   */
  HLT::ErrorCode hltEndEvent() { m_useCachedResult = false; m_clu_feature = 0; m_cachedTE=0; return HLT::OK; }

  // monitored quantities
  std::vector<double> m_RoiEta;
  std::vector<double> m_RoiPhi;
  

protected:
  // JobOption properties
  /** A property which specifies the radius of the cluster */
  float m_DeltaR;
  /** A property which specifies the matching of the jet and of the cluster */
  float m_DeltaRJet;

  /** Eta of the Center RoI cluster, for monitoring purpose */
  float m_CluEta;
  /** Phi of the Center RoI cluster, for monitoring purpose */
  float m_CluPhi;
  /** Numbers of Roi in cluster */
  int m_CluNum;
  
  int m_NumTrk;
  int m_NumJet;
  
  double m_PtMinID;
  float m_DeltaRTrk;
  double m_minJetEt;
    
  
  //Output parameters. Keep old container so we don't break Frozen Tier0. Should be removed in the future!
  std::string m_featureLabel; //!< label for the mucluster  feature in the HLT Navigation
  bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode
  xAOD::TrigCompositeContainer *m_clu_feature;    //!< internal caching: m_clu_feature of the first execution
  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution

  // Services
  /** Timers */
  std::vector<TrigTimer*> m_Timers;
};

#endif // TRIGL2LONGLIVEDPARTICLES_MUONCLUSTERISOLATION_H
