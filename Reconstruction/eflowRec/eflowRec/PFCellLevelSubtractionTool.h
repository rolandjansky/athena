/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCELLLEVELSUBTRACTIONTOOL_H
#define PFCELLLEVELSUBTRACTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/IPFSubtractionTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"

#include <vector>
#include <cassert>

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;
class IEFlowCellEOverPTool;
class PFTrackClusterMatchingTool;
class eflowRecTrack;

class PFCellLevelSubtractionTool : public extends<AthAlgTool, IPFSubtractionTool> {
public:

  PFCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFCellLevelSubtractionTool();

  StatusCode initialize();
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer* recTrackContainer, eflowRecClusterContainer* recClusterContainer,xAOD::CaloClusterContainer& theCaloClusterContainer);
  StatusCode finalize();

 private:

  void calculateRadialEnergyProfiles(xAOD::CaloClusterContainer& theCaloClusterContainer);
  void calculateAverageEnergyDensity();
  void performSubtraction(xAOD::CaloClusterContainer& theCaloClusterContainer);
  bool runInGoldenMode() { return ((m_goldenModeString == "golden1") || (m_goldenModeString == "golden2")); }
  bool isEOverPFail(double expectedEnergy, double sigma, double clusterEnergy, bool consistencySigmaCut, bool useGoldenMode);
  bool canAnnihilated(double expectedEnergy, double sigma, double clusterEnergy);

  int matchAndCreateEflowCaloObj(int n);
  std::string printTrack(const xAOD::TrackParticle* track);
  std::string printCluster(const xAOD::CaloCluster* cluster);
  void printAllClusters(const eflowRecClusterContainer& recClusterContainer);

 private:

  eflowCaloObjectContainer* m_eflowCaloObjectContainer;
  eflowRecTrackContainer* m_eflowTrackContainer;
  eflowRecClusterContainer* m_eflowClusterContainer;

  /** Default track-cluster matching tool */
  ToolHandle<PFTrackClusterMatchingTool> m_matchingTool{this,"PFTrackClusterMatchingTool","PFTrackClusterMatchingTool/CalObjBldMatchingTool","The track-cluster matching tool"};
  /* Track-cluster matching tools for calculating the pull */
  ToolHandle<PFTrackClusterMatchingTool> m_matchingToolForPull_015{this,"PFTrackClusterMatchingTool_015","PFTrackClusterMatchingTool/PFPullMatchingTool_015","The 0.15 track-cluster matching tool to calculate the pull"};
  ToolHandle<PFTrackClusterMatchingTool> m_matchingToolForPull_02{this,"PFTrackClusterMatchingTool_02","PFTrackClusterMatchingTool/PFPullMatchingTool_02","The 0.2 track-cluster matching tool to calculate the pull"};
  
  /* Tools for "shower simulation" */
  std::unique_ptr<eflowEEtaBinnedParameters> m_binnedParameters;
  std::unique_ptr<eflowLayerIntegrator> m_integrator;
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
  
  /** Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density regions  */
  Gaudi::Property<bool> m_useUpdated2015ChargedShowerSubtraction{this,"useUpdated2015ChargedShowerSubtraction",true,"Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density region"};

};

#endif 
