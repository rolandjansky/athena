/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWCALOOBJECTBUILDER_H
#define EFLOWREC_EFLOWCALOOBJECTBUILDER_H
/********************************************************************

NAME:     eflowCaloObjectBuilder.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey
CREATED:  10th November, 2001

********************************************************************/

#include <vector>
#include <sstream>
#include "eflowRec/eflowBaseAlg.h"
#include "eflowRec/eflowTrackClusterMatcher.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "GaudiKernel/ToolHandle.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/ConstDataVector.h"

class eflowRecTrack;
class eflowRecCluster;
class eflowTrackClusterLink;
class eflowTrackToCaloTrackExtrapolatorTool;
class eflowCaloObject;
class eflowCaloObjectContainer;
class eflowCellEOverPTool;
class eflowBinnedParameters;
class eflowLayerIntegrator;

class egammaContainer;
class egamma;
class CaloCellContainer;
class CaloCell;


class eflowCaloObjectBuilder : public eflowBaseAlg {
 public:

  // constructor
  eflowCaloObjectBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  ~eflowCaloObjectBuilder();
  // Gaudi algorithm hooks

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  StatusCode makeClusterList();
  StatusCode makeTrackList();
  bool selectTrack(const xAOD::TrackParticle* track);
  void matchTracksWithClusters(int debug);
  std::vector<eflowRecCluster*>::iterator getBestClusterMatch(const eflowRecTrack* eflowTrack);
  void simulateShower(eflowTrackClusterLink* trackClusterLink);

  std::string printTrack(const xAOD::TrackParticle* track);
  std::string printCluster(const xAOD::CaloCluster* cluster);
  void printAllClusters();

  /* Name of the cluster container and track container to read in: */
  std::string m_clustersName;
  std::string m_tracksName;

  /* Name of and pointer to the eflowCaloObjectContainer to write out: */
  std::string m_eflowCaloObjectsOutputName;
  eflowCaloObjectContainer* m_caloObjectContainer;

  /* Tools for "shower simulation" */
  eflowBinnedParameters* m_binnedParameters;
  eflowLayerIntegrator* m_integrator;

  ToolHandle<eflowCellEOverPTool> m_theEOverPTool;

  /* Handle to interface on TrackToCalo tool. */
  ToolHandle<eflowTrackToCaloTrackExtrapolatorTool> m_theTrackExtrapolatorTool;

  /* Which eflow mode is in use - Tau, MET or Full */
  std::string m_eflowMode;

  std::vector<eflowRecTrack*> m_eflowTrackList;
  std::vector<eflowRecCluster*> m_eflowClusterList;

  /* Put electron container and list of lepton cells into Storegate */
  StatusCode recordLeptonContainers();

  /** select electrons to use */
  StatusCode selectElectrons();
  /** check if track belongs to an electron */
  bool isElectron(const xAOD::TrackParticle* track);
  /** store the cells of the electrons */
  void storeElectronCells(const xAOD::Egamma* electron);

  /** select third chain muons to use */
  StatusCode selectMuons();  
  /** select staco muons to use - fall back only for when third chain muons don't exist */
  StatusCode selectStacoMuons();

  /** check if track belongs to an muon */
  bool isMuon(const xAOD::TrackParticle* track);
  /** store the cells of the electrons */
  void storeMuonCells(const xAOD::Muon* muon);

  /** puts set of lepton cells into the lepton container */
  void storeLeptonCells(const xAOD::CaloCluster* theCluster);
  
  void clearContainers();

  /** container of electrons that we will select */
  xAOD::ElectronContainer* m_selectedElectrons;

  /** container of muons that we will select */
  xAOD::MuonContainer* m_selectedMuons;

  /** container to put the lepton cells into */
  ConstDataVector<CaloCellContainer>* m_leptonCellContainer;

  /** Name of egamma track map */
  std::string m_egammaTrackMapName;

  /** Name of electron container */
  std::string m_electronsName;

  /** Name of muon container */
  std::string m_muonsName;

  /** bool to toggle masking out of lepton tracks */
  bool m_useLeptons;

  /** bool to toggle storage of lepton CaloCells */
  bool m_storeLeptonCells;
  
  /** bool to assign name to container of electrons selected by eflowRec, to be used to mask out electron tracks */
  std::string m_eflowElectronsName;
  
  /** bool to assign mame to container of lepton cells, to be used to remove lepton cells */
  std::string m_eflowLeptonCellsName;

};
#endif
