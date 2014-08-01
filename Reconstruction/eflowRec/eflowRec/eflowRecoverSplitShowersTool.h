/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWRECOVERSPLITSHOWERSTOOL_H
#define EFLOWREC_EFLOWRECOVERSPLITSHOWERSTOOL_H

/********************************************************************

NAME: eflowRecoverSplitShowersTool
PACKAGE: offline/Reconstruction/eflowRec

AUTHOR: Thomas Velz, Mark Hodgkinson
CREATED: 16 January 2014

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloClusterCorrection/CaloClusterLocalCalib.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "eflowRec/eflowBaseAlgTool.h"

class eflowCaloObjectContainer;
class eflowCaloObject;
class eflowRecCluster;
class eflowRecTrack;
class eflowTrackCaloPoints;
class eflowCellList;
class eflowTrackToCaloTrackExtrapolatorTool;
class eflowLayerIntegrator;
class eflowBinnedParameters;
class eflowCellEOverPTool;

static const InterfaceID IID_eflowRecoverSplitShowersTool("eflowRecoverSplitShowersTool", 1, 0);

class eflowRecoverSplitShowersTool : virtual public eflowBaseAlgTool, public AthAlgTool {
  
 public:

  eflowRecoverSplitShowersTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowRecoverSplitShowersTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer);
  virtual StatusCode finalize();

 private:
  void getClustersToConsider();
  void getTracksToRecover();
  void performRecovery();
  std::vector<eflowRecCluster*> getMatchedClusterList(eflowRecTrack* efRecTrack);
  void subtractTrackFromClusters(eflowRecTrack* efRecTrack,
                                 std::vector<eflowRecCluster*> matchedClusters);
  void makeOrderedCellList(const eflowTrackCaloPoints& trackCalo, const std::vector<xAOD::CaloCluster*>& clusters, eflowCellList& orderedCells);
  double getSumEnergy(const std::vector<xAOD::CaloCluster*>& clusters);
  void clearClusters(std::vector<xAOD::CaloCluster*>& clusters);

  void printClusterList(std::vector<xAOD::CaloCluster*>& clusters, std::string prefix);

  int m_debug;

  eflowCaloObjectContainer* m_eflowCaloObjectContainer;
  std::vector<eflowRecCluster*> m_clustersToConsider;
  std::vector<eflowRecTrack*> m_tracksToRecover;

  double m_rCell;
  double m_windowRms;

  /* Tool for getting e/p values and hadronic shower cell ordering principle parameters */
  ToolHandle<eflowCellEOverPTool> m_theEOverPTool;
  
  eflowBinnedParameters* m_binnedParameters;
  eflowLayerIntegrator* m_integrator;

  double m_subtractionSigmaCut;

  bool m_recoverIsolatedTracks;

};

inline const InterfaceID& eflowRecoverSplitShowersTool::interfaceID()
{
  return IID_eflowRecoverSplitShowersTool;
}

#endif
