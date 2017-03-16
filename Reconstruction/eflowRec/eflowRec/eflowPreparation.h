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

#include "StoreGate/DataHandle.h"

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

  /** for EM mode, LC weight for cells are retrieved before doing any subtraction; they will be used after subtraction */
  void retrieveLCCalCellWeight(double energy, unsigned index, std::map<IdentifierHash, double>& cellWeights);

  bool selectTrack(const xAOD::TrackParticle* track);

  /** Put electron container and list of lepton cells into Storegate */
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


  /// ReadHandle for the CaloClusterContainer to be used as input */
  SG::ReadHandle<xAOD::CaloClusterContainer> m_caloClusterReadHandle;

  /// ReadHandle for the CaloClusterContainer, at LC scale, to be used as input*/
  SG::ReadHandle<xAOD::CaloClusterContainer> m_caloCalClusterReadHandle;

  /// ReadHandle for the TrackParticleContainer to be used as input */
  SG::ReadHandle<xAOD::TrackParticleContainer> m_trackReadHandle;

  /// ReadHandle for the ElectronContainer to be used as input */
  SG::ReadHandle<xAOD::ElectronContainer> m_electronReadHandle;

  /// ReadHandle for the MuonContainer to be used as input */
  SG::ReadHandle<xAOD::MuonContainer> m_muonReadHandle;
  
  /// WriteHandle for the eflowCaloObjectContainer to write out: */
  SG::WriteHandle<eflowCaloObjectContainer> m_eflowCaloObjectContainerWriteHandle;
  
  /// WriteHandle for the eflowRecTrackContainer to write out: */
  SG::WriteHandle<eflowRecTrackContainer> m_eflowRecTrackContainerWriteHandle;

  /// WriteHandle for the eflowRecClusterContainer to write out: */
  SG::WriteHandle<eflowRecClusterContainer> m_eflowRecClusterContainerWriteHandle;

  /// Handle to interface on TrackToCalo tool. */
  ToolHandle<eflowTrackExtrapolatorBaseAlgTool> m_theTrackExtrapolatorTool;

  ToolHandle<PFTrackClusterMatchingTool> m_matchingTool;

  /// Which eflow mode is in use - Tau, MET or Full */
  std::string m_eflowMode;

  /// WriteHandle for the ElectronContainer, that will be filled with electrons passing the electron ID in eflowPreparation::selectElectrons */
  SG::WriteHandle<xAOD::ElectronContainer> m_selectedElectronsWriteHandle;

  /// container of muons that we will select */
  std::unique_ptr<xAOD::MuonContainer> m_selectedMuons;

  /// WriteHandle for the CaloCellContainer, that will store calorimeter cells associated to leptons (electrons and muons) */
  SG::WriteHandle<ConstDataVector<CaloCellContainer> > m_leptonCaloCellContainerWriteHandle;

  /// bool to toggle masking out of lepton tracks */
  bool m_useLeptons;

  /// bool to toggle storage of lepton CaloCells */
  bool m_storeLeptonCells;

  /// Count the number of track-cluster matches -- for the summary in finalize() */
  unsigned int m_nMatches;

  /// New track selection tool */
  ToolHandle<InDet::IInDetTrackSelectionTool> m_selTool;

  /// Upper limit on track Pt for input tracks */
  float m_upperTrackPtCut;
  
};
#endif
