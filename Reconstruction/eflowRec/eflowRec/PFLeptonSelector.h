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
  StatusCode selectElectrons();

  /** store the cells of the electrons */
  void storeElectronCells(const xAOD::Egamma& electron);

  /** select  muons to use */
  StatusCode selectMuons();

  /** store the cells of the muons */
  void storeMuonCells(const xAOD::Muon& muon);

  /** puts set of lepton cells into the lepton container */
  void storeLeptonCells(const xAOD::CaloCluster& theCluster);

  /** Put lepton containers and list of lepton cells into Storegate */
  StatusCode recordLeptonContainers();
  
  /** ReadHandle for the ElectronContainer to be used as input */
  SG::ReadHandle<xAOD::ElectronContainer> m_electronsReadHandle;

  /** ReadHandle for the MuonContainer to be used as input */
  SG::ReadHandle<xAOD::MuonContainer> m_muonsReadHandle;

  /** WriteHandle for the ElectronContainer, that will be filled with electrons passing the electron ID in PFLeptonSelector::selectElectrons */
  SG::WriteHandle<xAOD::ElectronContainer> m_selectedElectronsWriteHandle;

  /** WriteHandle for the MuonContainer, that will be filled with muons passing the muon ID in PFLeptonSelector::selectMuons */
  SG::WriteHandle<xAOD::MuonContainer> m_selectedMuonsWriteHandle;

  /** WriteHandle for the CaloCellContainer, that will store calorimeter cells associated to leptons */
  SG::WriteHandle<ConstDataVector<CaloCellContainer> > m_leptonCaloCellsWriteHandle;

  /** bool to toggle storage of lepton CaloCells */
  bool m_storeLeptonCells;

  
};
#endif
