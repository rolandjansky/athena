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
  StatusCode selectElectrons(SG::WriteHandle<xAOD::ElectronContainer>& selectedElectronsWriteHandle, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** store the cells of the electrons */
  void storeElectronCells(const xAOD::Egamma& electron, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** select  muons to use */
  StatusCode selectMuons(SG::WriteHandle<xAOD::MuonContainer>& selectedMuonsWriteHandle, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** store the cells of the muons */
  void storeMuonCells(const xAOD::Muon& muon, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** puts set of lepton cells into the lepton container */
  void storeLeptonCells(const xAOD::CaloCluster& theCluster, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle);

  /** Put lepton containers and list of lepton cells into Storegate */
  StatusCode recordLeptonContainers(SG::WriteHandle<xAOD::ElectronContainer>& selectedElectronsWriteHandle, SG::WriteHandle<xAOD::MuonContainer>& selectedMuonsWriteHandle, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle );
  
  /** ReadHandle for the ElectronContainer to be used as input */
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsReadHandleKey{this,"inputElectronsName","Electrons","ReadHandle for the ElectronContainer to be used as input"};

  /** ReadHandle for the MuonContainer to be used as input */
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsReadHandleKey{this,"inputMuonsName","Muons","ReadHandle for the MuonContainer to be used as input"};

  /** WriteHandle for the ElectronContainer, that will be filled with electrons passing the electron ID in PFLeptonSelector::selectElectrons */
  SG::WriteHandleKey<xAOD::ElectronContainer> m_selectedElectronsWriteHandleKey{this,"outputElectronsName","eflowRec_selectedElectrons","WriteHandle for the ElectronContainer, that will be filled with electrons passing the electron ID in PFLeptonSelector::selectElectrons"};

  /** WriteHandle for the MuonContainer, that will be filled with muons passing the muon ID in PFLeptonSelector::selectMuons */
  SG::WriteHandleKey<xAOD::MuonContainer> m_selectedMuonsWriteHandleKey{this,"outputMuonsName","eflowRec_selectedMuons","WriteHandle for the MuonContainer, that will be filled with muons passing the muon ID in PFLeptonSelector::selectMuons"};

  /** WriteHandle for the CaloCellContainer, that will store calorimeter cells associated to leptons */
  SG::WriteHandleKey<ConstDataVector<CaloCellContainer> > m_leptonCaloCellsWriteHandleKey{this,"outputLeptonCellsName","eflowRec_leptonCellContainer","WriteHandle for the CaloCellContainer, that will store calorimeter cells associated to leptons"};

  /** Toggle storage of lepton CaloCells */
  Gaudi::Property<bool> m_storeLeptonCells{this,"storeLeptonCells",false,"Toggle storage of lepton CaloCells"};

  
};
#endif
