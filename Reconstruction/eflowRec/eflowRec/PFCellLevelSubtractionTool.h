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

static const InterfaceID IID_PFCellLevelSubtractionTool("PFCellLevelSubtractionTool", 1, 0);

class PFCellLevelSubtractionTool : virtual public IPFSubtractionTool, public AthAlgTool {
public:

  PFCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFCellLevelSubtractionTool();

  static const InterfaceID& interfaceID();

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
  
  ToolHandle<PFTrackClusterMatchingTool> m_matchingTool;
  /* Matching tools for calculating the pull */
  ToolHandle<PFTrackClusterMatchingTool> m_matchingToolForPull_015;
  ToolHandle<PFTrackClusterMatchingTool> m_matchingToolForPull_02;
  
  /* Tools for "shower simulation" */
  std::unique_ptr<eflowEEtaBinnedParameters> m_binnedParameters;
  std::unique_ptr<eflowLayerIntegrator> m_integrator;
  ToolHandle<IEFlowCellEOverPTool> m_theEOverPTool;

  //double m_rCell;

  double m_subtractionSigmaCut;
  double m_consistencySigmaCut;

  //flag if we want to caluclate EoVerP from single particles we dont do eflow, so clusters are original unmodified ones.
  bool m_calcEOverP;

  // string flag to configure for running in golden e/p match mode
  std::string m_goldenModeString;

  // Number of clusters to match
  int m_nMatchesInCellLevelSubtraction;

  /** Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density regions  */
  bool m_useUpdated2015ChargedShowerSubtraction;

};

inline const InterfaceID& PFCellLevelSubtractionTool::interfaceID()
{ 
  return IID_PFCellLevelSubtractionTool; 
}

#endif 
