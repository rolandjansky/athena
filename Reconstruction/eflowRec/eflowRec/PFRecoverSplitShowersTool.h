/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFRECOVERSPLITSHOWERSTOOL_H
#define PFRECOVERSPLITSHOWERSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "eflowRec/IPFSubtractionTool.h"

#include <cassert>

class eflowCaloObjectContainer;
class eflowRecCluster;
class eflowRecTrack;
class eflowTrackCaloPoints;
class eflowLayerIntegrator;
class eflowEEtaBinnedParameters;
class IEFlowCellEOverPTool;
class PFTrackClusterMatchingTool;
class eflowRingSubtractionManager;
class eflowRecTrackContainer;
class eflowRecClusterContainer;
class eflowCaloObject;

class PFRecoverSplitShowersTool : public extends<AthAlgTool, IPFSubtractionTool> {
  
 public:

  PFRecoverSplitShowersTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFRecoverSplitShowersTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer*, eflowRecClusterContainer*,xAOD::CaloClusterContainer& theCaloClusterContainer);
  virtual StatusCode finalize();

 private:

  void recycleTracksClusters();
  void getClustersToConsider();
  void getTracksToRecover();
  int matchAndCreateEflowCaloObj();
  void performRecovery(int const nOriginalObj,xAOD::CaloClusterContainer& theCaloClusterContainer);
  void subtractTrackFromClusters(const eflowTrackCaloPoints& trackCalo, eflowRingSubtractionManager& ranking, eflowRecTrack* efRecTrack, std::vector<xAOD::CaloCluster*> clusterSubtractionList);
  double getSumEnergy(const std::vector<xAOD::CaloCluster*>& clusters);

  void printClusterList(std::vector<xAOD::CaloCluster*>& clusters, std::string prefix);
  void performSubtraction(eflowCaloObject* thisEflowCaloObject,xAOD::CaloClusterContainer& theCaloClusterContainer);

private:

  eflowCaloObjectContainer* m_eflowCaloObjectContainer;
  std::vector<eflowRecCluster*> m_clustersToConsider;
  std::vector<eflowRecTrack*> m_tracksToRecover;

  double m_rCell;
  double m_windowRms;

  /* Tool for getting e/p values and hadronic shower cell ordering principle parameters */
  ToolHandle<IEFlowCellEOverPTool> m_theEOverPTool;

  /** Track-Cluster matching tool */
  ToolHandle<PFTrackClusterMatchingTool> m_matchingTool;

  std::unique_ptr<eflowEEtaBinnedParameters> m_binnedParameters;
  std::unique_ptr<eflowLayerIntegrator> m_integrator;

  double m_subtractionSigmaCut;

  bool m_recoverIsolatedTracks;

  /** Count the number of track-cluster matches -- for the summary in finalize */
  unsigned int m_nTrackClusterMatches;

  /** Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density regions  */
  bool m_useUpdated2015ChargedShowerSubtraction;
  
};

#endif
