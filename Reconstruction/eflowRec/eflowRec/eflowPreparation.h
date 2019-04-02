/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWPREPARATION_H
#define EFLOWREC_EFLOWPREPARATION_H
/********************************************************************

NAME:     eflowPreparation.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey
CREATED:  10th November, 2001

********************************************************************/

#include <vector>
#include <sstream>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Particle/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "AthContainers/ConstDataVector.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

class eflowTrackExtrapolatorBaseAlgTool;
class eflowCaloObjectContainer;
class PFTrackClusterMatchingTool;

class CaloCellContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;

class eflowPreparation : public AthAlgorithm {
 public:

  // constructor
  eflowPreparation(const std::string& name, ISvcLocator* pSvcLocator);

  ~eflowPreparation();

  // Gaudi algorithm hooks
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  StatusCode makeClusterContainer();
  StatusCode makeTrackContainer();

  /* for EM mode, LC weight for cells are retrieved before doing any subtraction; they will be used after subtraction */
  void retrieveLCCalCellWeight(double energy, unsigned index, std::map<IdentifierHash, double>& cellWeights);

  bool selectTrack(const xAOD::TrackParticle* track);

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

  /** check if track belongs to an muon */
  bool isMuon(const xAOD::TrackParticle* track);
  /** store the cells of the electrons */
  void storeMuonCells(const xAOD::Muon* muon);

  /** puts set of lepton cells into the lepton container */
  void storeLeptonCells(const xAOD::CaloCluster* theCluster);

  void clearContainers();

 private:

  /* Name of the cluster container and track container to read in: */
  std::string m_clustersName;
  std::string m_clustersCalName;
  std::string m_tracksName;

  /* Name of and pointer to the eflowCaloObjectContainer to write out: */
  std::string m_eflowCaloObjectsOutputName;
  eflowCaloObjectContainer* m_caloObjectContainer;

  /* Name of and pointer to the eflowRecTrackContainer to write out: */
  std::string m_eflowRecTracksOutputName;
  eflowRecTrackContainer* m_recTrackContainer;

  /* Name of and pointer to the eflowRecClusterContainer to write out: */
  std::string m_eflowRecClustersOutputName;
  eflowRecClusterContainer* m_recClusterContainer;

  /* Handle to interface on TrackToCalo tool. */
  ToolHandle<eflowTrackExtrapolatorBaseAlgTool> m_theTrackExtrapolatorTool;

  ToolHandle<PFTrackClusterMatchingTool> m_matchingTool;

  /* Which eflow mode is in use - Tau, MET or Full */
  std::string m_eflowMode;

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

  /** Count the number of track-cluster matches -- for the summary in finalize() */
  unsigned int m_nMatches;

  /** New track selection tool */
    ToolHandle<InDet::IInDetTrackSelectionTool> m_selTool;

  /** Upper limit on track Pt for input tracks */
  float m_upperTrackPtCut;

  /** Bool to toggle muons bug fix */
  bool m_fixMuonLogic;
  
};
#endif
