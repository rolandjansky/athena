/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCELLLEVELSUBTRACTIONTOOL_H
#define PFCELLLEVELSUBTRACTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/IPFSubtractionTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/PFMatchPositions.h"
#include "eflowRec/EtaPhiLUT.h"

#include <vector>

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;
class IEFlowCellEOverPTool;
class PFTrackClusterMatchingTool;
class eflowRecTrack;

namespace PFMatch {
  class TrackEtaPhiInFixedLayersProvider;
}

namespace eflowRec {
  class EtaPhiLUT;
}

class PFCellLevelSubtractionTool : public extends<AthAlgTool, IPFSubtractionTool> {
public:

  PFCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFCellLevelSubtractionTool();

  StatusCode initialize();
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer* recTrackContainer, eflowRecClusterContainer* recClusterContainer) const;
  StatusCode finalize();

 private:

  struct eflowData {
    eflowCaloObjectContainer* caloObjects;
    eflowRecTrackContainer* tracks;
    eflowRecClusterContainer* clusters;
    eflowRec::EtaPhiLUT clusterLUT;
  };

  void calculateRadialEnergyProfiles(eflowData& data) const;
  void performSubtraction(eflowData& data) const;
  bool isEOverPFail(double expectedEnergy, double sigma, double clusterEnergy, bool consistencySigmaCut, bool useGoldenMode) const;
  bool canAnnihilated(double expectedEnergy, double sigma, double clusterEnergy) const;

  unsigned int matchAndCreateEflowCaloObj(unsigned int n, eflowData& data) const;
  std::string printTrack(const xAOD::TrackParticle* track) const;
  std::string printCluster(const xAOD::CaloCluster* cluster) const;
  void printAllClusters(const eflowRecClusterContainer& recClusterContainer) const;

  // Need a track position provider to preselect clusters
  std::unique_ptr<PFMatch::TrackEtaPhiInFixedLayersProvider> m_trkpos;

  /** Default track-cluster matching tool */
  ToolHandle<PFTrackClusterMatchingTool> m_matchingTool{this,"PFTrackClusterMatchingTool","PFTrackClusterMatchingTool/CalObjBldMatchingTool","The track-cluster matching tool"};
  /* Track-cluster matching tools for calculating the pull */
  ToolHandle<PFTrackClusterMatchingTool> m_matchingToolForPull_015{this,"PFTrackClusterMatchingTool_015","PFTrackClusterMatchingTool/PFPullMatchingTool_015","The 0.15 track-cluster matching tool to calculate the pull"};
  ToolHandle<PFTrackClusterMatchingTool> m_matchingToolForPull_02{this,"PFTrackClusterMatchingTool_02","PFTrackClusterMatchingTool/PFPullMatchingTool_02","The 0.2 track-cluster matching tool to calculate the pull"};
  
  /* Tools for "shower simulation" */
  std::unique_ptr<eflowEEtaBinnedParameters> m_binnedParameters;
  ToolHandle<IEFlowCellEOverPTool> m_theEOverPTool{this,"eflowCellEOverPTool","eflowCellEOverPTool","Energy Flow E/P Values and Shower Paremeters Tool"};

  /** Parameter that controls whether to use retain remaining calorimeter energy in track-cluster system, after charged shower subtraction */
  Gaudi::Property<double> m_subtractionSigmaCut{this,"SubtractionSigmaCut",1.5,"Parameter that controls whether to use retain remaining calorimeter energy in track-cluster system, after charged shower subtraction"};
  /** Parameter that controls whether a track, in a track-cluster system, will be processed by the split shower recovery algorithm */
  Gaudi::Property<double> m_consistencySigmaCut{this,"ConsistencySigmaCut",1.0,"Parameter that controls whether a track, in a track-cluster system, will be processed by the split shower recovery algorithm"};

  /** Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed */
  Gaudi::Property<bool> m_calcEOverP{this,"CalcEOverP",false,"Toggle EOverP algorithm mode, whereby no charged shower subtraction is performed"};

  /** "Number of clusters to match to each track" */
  Gaudi::Property<int> m_nMatchesInCellLevelSubtraction{this,"nMatchesInCellLevelSubtraction",1,"Number of clusters to match to each track"};

  /** Toggle whether to use golden mode, whereby we only use idealised track-cluster matches within +- N sigma of expected mean e/p */
  Gaudi::Property<std::string> m_goldenModeString{this,"goldenModeString","","Toggle whether to use golden mode, whereby we only use idealised track-cluster matches within +- N sigma of expected mean e/p"};
  bool m_runInGoldenMode{false};
  
  /** Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density regions  */
  Gaudi::Property<bool> m_useUpdated2015ChargedShowerSubtraction{this,"useUpdated2015ChargedShowerSubtraction",true,"Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density region"};

  /** Toggle whether we have the HLLHC setup */
  Gaudi::Property<bool> m_isHLLHC{this,"isHLLHC",false,"Toggle whether we have the HLLHC setup"};

};

#endif 
