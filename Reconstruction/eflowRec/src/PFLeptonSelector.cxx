#include "eflowRec/PFLeptonSelector.h"

PFLeptonSelector::PFLeptonSelector(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_electronsReadHandle("Electrons"),
  m_muonsReadHandle("Muons"),
  m_selectedElectronsWriteHandle("eflowRec_selectedElectrons"),
  m_selectedMuonsWriteHandle("eflowRec_selectedMuons"),
  m_leptonCaloCellsWriteHandle("eflowRec_leptonCellContainer"),
  m_storeLeptonCells(false)
{
  declareProperty("inputElectronsName", m_electronsReadHandle);
  declareProperty("inputMuonsName",  m_muonsReadHandle);
  declareProperty("outputElectronsName", m_selectedElectronsWriteHandle);
  declareProperty("outputMuonsName", m_selectedMuonsWriteHandle);
  declareProperty("outputLeptonCellsName", m_leptonCaloCellsWriteHandle);
  declareProperty("storeLeptonCells", m_storeLeptonCells);
}

StatusCode PFLeptonSelector::initialize(){
  ATH_CHECK( m_electronsReadHandle.initialize() );
  ATH_CHECK( m_muonsReadHandle.initialize() );

  ATH_CHECK( m_selectedElectronsWriteHandle.initialize() );
  ATH_CHECK( m_selectedMuonsWriteHandle.initialize() );

  ATH_CHECK( m_leptonCaloCellsWriteHandle.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode PFLeptonSelector::execute(){

  if (recordLeptonContainers().isFailure()) {
      return StatusCode::SUCCESS;
    }

  /* Select electrons */
  StatusCode sc = this->selectElectrons();
  //if fail to select electrons issue warning, but carry on processing event
  if (sc.isFailure()) ATH_MSG_WARNING(" Problem selecting electrons");

  /* Select  muons */
  sc = this->selectMuons();
   //if fail to select muons issue warning, but carry on processing event
  if (sc.isFailure()) ATH_MSG_WARNING("Problem selecting muons ");

  return StatusCode::SUCCESS;
}

StatusCode PFLeptonSelector::finalize(){ return StatusCode::SUCCESS; }

StatusCode PFLeptonSelector::selectElectrons(){

  if (!m_electronsReadHandle.isValid()){
    ATH_MSG_WARNING("Invalid read handle to electron container with name: " << m_electronsReadHandle.key());
    return StatusCode::FAILURE;
  }

  for (auto theElectron : *m_electronsReadHandle){
    
    if (theElectron){
      if (theElectron->pt() > 10000){
        bool val_med = false;
	bool gotID = theElectron->passSelection(val_med, "LHMedium");
	if (!gotID) {
	  ATH_MSG_WARNING("Could not get Electron ID");
	  continue;
	}
	if (true == val_med){
	  if (m_selectedElectronsWriteHandle.isValid()) m_selectedElectronsWriteHandle->push_back(const_cast<xAOD::Electron*>(theElectron));
	  else ATH_MSG_WARNING("Do not have valid WriteHandle for ElectronContainer with name: " << m_selectedElectronsWriteHandle.key());
	  if (true == m_storeLeptonCells) this->storeElectronCells(*theElectron);
	}//mediumPP
      }//10GeV pt cut
    }//valid egamma pointer
    else ATH_MSG_WARNING("This electron is a NULL pointer");
    
  }//electron loop

  return StatusCode::SUCCESS;
}

void PFLeptonSelector::storeElectronCells(const xAOD::Egamma& electron){

  const xAOD::CaloCluster* electronCluster = electron.caloCluster(); 
  if (electronCluster){
    this->storeLeptonCells(*electronCluster);      
  }
  else ATH_MSG_WARNING("This electron has an invalid pointer to its cluster");

}

StatusCode PFLeptonSelector::selectMuons() {

  if (!m_muonsReadHandle.isValid()) {
    ATH_MSG_WARNING("Invalid read handle to muon container with name: " << m_muonsReadHandle.key());    
    return StatusCode::FAILURE;
  }

  for (auto theMuon : *m_muonsReadHandle){  
    
    //Details of medium muons are here:
    //https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
    //No need to ask for combined muon, by construction other muons will not have ID track - we just ask for medium muons
    
    xAOD::Muon::Quality muonQuality = theMuon->quality();
    if( muonQuality <= xAOD::Muon::Medium) {   
      if (m_selectedMuonsWriteHandle.isValid()) m_selectedMuonsWriteHandle->push_back(const_cast<xAOD::Muon*>(theMuon));
      else ATH_MSG_WARNING("Do not have valid WriteHandle for MuonContainer with name: " << m_selectedMuonsWriteHandle.key());
      if (true == m_storeLeptonCells) this->storeMuonCells(*theMuon);
    }//Medium muons
  } //muon loop

  return StatusCode::SUCCESS;
}

void PFLeptonSelector::storeMuonCells(const xAOD::Muon& muon){

  const ElementLink<xAOD::CaloClusterContainer> theLink = muon.clusterLink(); 
  if (theLink.isValid()){
    const xAOD::CaloCluster* muonCluster = *theLink;
    if (muonCluster){
      this->storeLeptonCells(*muonCluster);
    }
    else ATH_MSG_WARNING("This muon has an invalid pointer to its cluster ");
  }
  else ATH_MSG_WARNING("This muon has an invalid element link to its cluster");

}

void PFLeptonSelector::storeLeptonCells(const xAOD::CaloCluster& theCluster){

  const CaloClusterCellLink* theCellLink = theCluster.getCellLinks();

  if (theCellLink){
    for (auto theCaloCell : *theCellLink){
      if (m_leptonCaloCellsWriteHandle.isValid()) m_leptonCaloCellsWriteHandle->push_back(theCaloCell);
      else ATH_MSG_WARNING(" Do not have valid WriteHandle for CaloCellContaienr with name: " << m_leptonCaloCellsWriteHandle.key());
    }//cell loop
  }
  else ATH_MSG_WARNING("This cluster has an invalid pointer to its cells, in storeLeptonCells");
}

StatusCode PFLeptonSelector::recordLeptonContainers(){

  StatusCode sc = m_selectedElectronsWriteHandle.record(std::make_unique<xAOD::ElectronContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not record electron WriteHandle with key: " << m_selectedElectronsWriteHandle.key());
    return sc;
  }

  sc = m_selectedMuonsWriteHandle.record(std::make_unique<xAOD::MuonContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not record muon WriteHandle with key: " << m_selectedMuonsWriteHandle.key());
    return sc;
  }

  if (true == m_storeLeptonCells) {

    //record the cell container
    sc =  m_leptonCaloCellsWriteHandle.record(std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS));

    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not record CaloCell WriteHandle with key: " << m_leptonCaloCellsWriteHandle);
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}
