/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFRECOVERSPLITSHOWERSTOOL_H
#define PFRECOVERSPLITSHOWERSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <unordered_set>

#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "eflowRec/IPFSubtractionTool.h"

class eflowCaloObjectContainer;
class eflowRecCluster;
class eflowRecTrack;
class eflowTrackCaloPoints;
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
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer*, eflowRecClusterContainer*) const;
  virtual StatusCode finalize();

 private:

  struct eflowData {
    eflowCaloObjectContainer* caloObjects;
    std::unordered_set<eflowRecCluster*> clustersToConsider;
    std::vector<eflowRecTrack*> tracksToRecover;
  };

  void fillClustersToConsider(eflowData&) const;
  void fillTracksToRecover(eflowData&) const;

  unsigned int matchAndCreateEflowCaloObj(eflowData&) const;
  void performRecovery(unsigned int const nOriginalObj, eflowData&) const;
  void subtractTrackFromClusters(const eflowTrackCaloPoints& trackCalo, eflowRingSubtractionManager& ranking, eflowRecTrack* efRecTrack, std::vector<xAOD::CaloCluster*> clusterSubtractionList) const;
  double getSumEnergy(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusters) const;

  void printClusterList(std::vector<xAOD::CaloCluster*>& clusters, std::string prefix) const;
  void performSubtraction(eflowCaloObject* thisEflowCaloObject) const;

  /* Tool for getting e/p values and hadronic shower cell ordering principle parameters */
  ToolHandle<IEFlowCellEOverPTool> m_theEOverPTool{this,"eflowCellEOverPTool","eflowCellEOverPTool","Energy Flow E/P Values and Shower Parameters Tool"};

  std::unique_ptr<eflowEEtaBinnedParameters> m_binnedParameters;

  /** Parameter that controls whether to use retain remaining calorimeter energy in track-cluster system, after charged shower subtraction */
  Gaudi::Property<double> m_subtractionSigmaCut{this,"SubtractionSigmaCut",1.5,"Parameter that controls whether to use retain remaining calorimeter energy in track-cluster system, after charged shower subtraction"};

  /** Toggle whether to recover isolated tracks */
  Gaudi::Property<bool> m_recoverIsolatedTracks{this,"RecoverIsolatedTracks",false,"Toggle whether to recover isolated tracks"};
  
  /** Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density regions  */
  Gaudi::Property<bool> m_useUpdated2015ChargedShowerSubtraction{this,"useUpdated2015ChargedShowerSubtraction",true,"Toggle whether to use updated 2015 charged shower subtraction, which disables the shower subtraction in high calorimeter energy density region"};
  
  /** Toggle whether we have the HLLHC setup */
  Gaudi::Property<bool> m_isHLLHC{this,"isHLLHC",false,"Toggle whether we have the HLLHC setup"};
  
};

#endif
