/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PFLEPTONSELECTOR_H
#define PFLEPTONSELECTOR_H

/** Algorithm which will select leptons for downstream algorithms to use within eflowRec */

#include "AthContainers/ConstDataVector.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

class CaloCellContainer;

class PFLeptonSelector : public AthAlgorithm {

public:
  /** Default constructor */
  PFLeptonSelector(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default destructor */
  ~PFLeptonSelector() {};

  /** Gaudi AthAlgorithm hooks */
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  /** Select electrons to use */
  StatusCode selectElectrons(SG::WriteHandle<ConstDataVector<xAOD::ElectronContainer >>& selectedElectronsWriteHandle, const SG::WriteHandle<ConstDataVector<CaloCellContainer> >& leptonCaloCellsWriteHandle);

  /** store the cells of the electrons */
  void storeElectronCells(const xAOD::Egamma& electron, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** select  muons to use */
  StatusCode selectMuons(SG::WriteHandle<ConstDataVector<xAOD::MuonContainer> >& selectedMuonsWriteHandle, const SG::WriteHandle<ConstDataVector<CaloCellContainer> >& leptonCaloCellsWriteHandle);

  /** store the cells of the muons */
  void storeMuonCells(const xAOD::Muon& muon, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** puts set of lepton cells into the lepton container */
  void storeLeptonCells(const xAOD::CaloCluster& theCluster, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** Put lepton containers and list of lepton cells into Storegate */
  StatusCode recordLeptonContainers(SG::WriteHandle<ConstDataVector<xAOD::ElectronContainer >>& selectedElectronsWriteHandle, SG::WriteHandle<ConstDataVector<xAOD::MuonContainer> >& selectedMuonsWriteHandle, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle );
  
  /** ReadHandle for the ElectronContainer to be used as input */
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsReadHandleKey{this,"inputElectronsName","Electrons","ReadHandle for the ElectronContainer to be used as input"};

  /** ReadHandle for the MuonContainer to be used as input */
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsReadHandleKey{this,"inputMuonsName","Muons","ReadHandle for the MuonContainer to be used as input"};

  /** WriteHandle for the ElectronContainer, that will be filled with electrons passing the electron ID in PFLeptonSelector::selectElectrons */
  SG::WriteHandleKey<ConstDataVector<xAOD::ElectronContainer >> m_selectedElectronsWriteHandleKey{this,"outputElectronsName","eflowRec_selectedElectrons","WriteHandle for the ElectronContainer, that will be filled with electrons passing the electron ID in PFLeptonSelector::selectElectrons"};

  /** WriteHandle for the MuonContainer, that will be filled with muons passing the muon ID in PFLeptonSelector::selectMuons */
  SG::WriteHandleKey<ConstDataVector<xAOD::MuonContainer> > m_selectedMuonsWriteHandleKey{this,"outputMuonsName","eflowRec_selectedMuons","WriteHandle for the MuonContainer, that will be filled with muons passing the muon ID in PFLeptonSelector::selectMuons"};
  
  /** WriteHandle for the CaloCellContainer, that will store calorimeter cells associated to leptons */
  SG::WriteHandleKey<ConstDataVector<CaloCellContainer> > m_leptonCaloCellsWriteHandleKey{this,"outputLeptonCellsName","eflowRec_leptonCellContainer","WriteHandle for the CaloCellContainer, that will store calorimeter cells associated to leptons"};

  /** Toggle storage of lepton CaloCells */
  Gaudi::Property<bool> m_storeLeptonCells{this,"storeLeptonCells",false,"Toggle storage of lepton CaloCells"};

  Gaudi::Property<std::string> m_electronID{this,"electronID","LHMedium","Select electron ID"};

  /** Toggle to determine whether we select any electrons or not - if selected then tracks matched to those electrons
  * in PFTrackSelector are not used in particle flow. If not selected then an empty electron container is created,
  * and hence PFTrackSelector won't find any electrons to match its selected tracks to.
  */
  Gaudi::Property<bool> m_selectElectrons{this,"selectElectrons",true,"Toggle usage of electron ID"};

  /** Toggle to determine whether we select any muons or not - if selected then tracks matched to those muons
  * in PFTrackSelector are not used in particle flow. If not selected then an empty muon container is created,
  * and hence PFTrackSelector won't find any muons to match its selected tracks to.
  */
  Gaudi::Property<bool> m_selectMuons{this,"selectMuons",true,"Toggle usage of muon ID"};
};
#endif
