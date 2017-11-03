#include "eflowRec/PFLeptonSelector.h"

PFLeptonSelector::PFLeptonSelector(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode PFLeptonSelector::initialize(){
  ATH_CHECK( m_electronsReadHandleKey.initialize() );
  ATH_CHECK( m_muonsReadHandleKey.initialize() );

  ATH_CHECK( m_selectedElectronsWriteHandleKey.initialize() );
  ATH_CHECK( m_selectedMuonsWriteHandleKey.initialize() );

  ATH_CHECK( m_leptonCaloCellsWriteHandleKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode PFLeptonSelector::execute(){

  SG::WriteHandle<xAOD::ElectronContainer> selectedElectronsWriteHandle(m_selectedElectronsWriteHandleKey);
  SG::WriteHandle<xAOD::MuonContainer> selectedMuonsWriteHandle(m_selectedMuonsWriteHandleKey);
  SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle(m_leptonCaloCellsWriteHandleKey);
  
  if (recordLeptonContainers(selectedElectronsWriteHandle,selectedMuonsWriteHandle,leptonCaloCellsWriteHandle).isFailure()) {
      return StatusCode::SUCCESS;
    }

  /* Select electrons */
  StatusCode sc = this->selectElectrons(selectedElectronsWriteHandle,leptonCaloCellsWriteHandle);
  //if fail to select electrons issue warning, but carry on processing event
  if (sc.isFailure()) ATH_MSG_WARNING(" Problem selecting electrons");

  /* Select  muons */
  sc = this->selectMuons(selectedMuonsWriteHandle,leptonCaloCellsWriteHandle);
   //if fail to select muons issue warning, but carry on processing event
  if (sc.isFailure()) ATH_MSG_WARNING("Problem selecting muons ");

  return StatusCode::SUCCESS;
}

StatusCode PFLeptonSelector::finalize(){ return StatusCode::SUCCESS; }

StatusCode PFLeptonSelector::selectElectrons(SG::WriteHandle<xAOD::ElectronContainer>& selectedElectronsWriteHandle, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle ){

  SG::ReadHandle<xAOD::ElectronContainer>  electronsReadHandle(m_electronsReadHandleKey);
  
  if (!electronsReadHandle.isValid()){
    ATH_MSG_WARNING("Invalid read handle to electron container with name: " << electronsReadHandle.key());
    return StatusCode::FAILURE;
  }

  for (auto theElectron : *electronsReadHandle){
    
    if (theElectron){
      if (theElectron->pt() > 10000){
        bool val_med = false;
	bool gotID = theElectron->passSelection(val_med, "LHMedium");
	if (!gotID) {
	  ATH_MSG_WARNING("Could not get Electron ID");
	  continue;
	}
	if (true == val_med){
	  if (selectedElectronsWriteHandle.isValid()) selectedElectronsWriteHandle->push_back(const_cast<xAOD::Electron*>(theElectron));
	  else ATH_MSG_WARNING("Do not have valid WriteHandle for ElectronContainer with name: " << selectedElectronsWriteHandle.key());
	  if (true == m_storeLeptonCells) this->storeElectronCells(*theElectron,leptonCaloCellsWriteHandle);
	}//mediumPP
      }//10GeV pt cut
    }//valid egamma pointer
    else ATH_MSG_WARNING("This electron is a NULL pointer");
    
  }//electron loop

  return StatusCode::SUCCESS;
}

void PFLeptonSelector::storeElectronCells(const xAOD::Egamma& electron, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle){

  const xAOD::CaloCluster* electronCluster = electron.caloCluster(); 
  if (electronCluster){
    this->storeLeptonCells(*electronCluster,leptonCaloCellsWriteHandle);      
  }
  else ATH_MSG_WARNING("This electron has an invalid pointer to its cluster");

}

StatusCode PFLeptonSelector::selectMuons(SG::WriteHandle<xAOD::MuonContainer>& selectedMuonsWriteHandle, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle) {

  SG::ReadHandle<xAOD::MuonContainer> muonsReadHandle(m_muonsReadHandleKey);
  
  if (!muonsReadHandle.isValid()) {
    ATH_MSG_WARNING("Invalid read handle to muon container with name: " << muonsReadHandle.key());    
    return StatusCode::FAILURE;
  }

  for (auto theMuon : *muonsReadHandle){  
    
    //Details of medium muons are here:
    //https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
    //No need to ask for combined muon, by construction other muons will not have ID track - we just ask for medium muons
    
    xAOD::Muon::Quality muonQuality = theMuon->quality();
    if( muonQuality <= xAOD::Muon::Medium) {   
      
      if (selectedMuonsWriteHandle.isValid()) selectedMuonsWriteHandle->push_back(const_cast<xAOD::Muon*>(theMuon));
      else ATH_MSG_WARNING("Do not have valid WriteHandle for MuonContainer with name: " << selectedMuonsWriteHandle.key());
      if (true == m_storeLeptonCells) this->storeMuonCells(*theMuon,leptonCaloCellsWriteHandle);
    }//Medium muons
  } //muon loop

  return StatusCode::SUCCESS;
}

void PFLeptonSelector::storeMuonCells(const xAOD::Muon& muon, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle){

  const ElementLink<xAOD::CaloClusterContainer> theLink = muon.clusterLink(); 
  if (theLink.isValid()){
    const xAOD::CaloCluster* muonCluster = *theLink;
    if (muonCluster){
      this->storeLeptonCells(*muonCluster,leptonCaloCellsWriteHandle);
    }
    else ATH_MSG_WARNING("This muon has an invalid pointer to its cluster ");
  }
  else ATH_MSG_WARNING("This muon has an invalid element link to its cluster");

}

void PFLeptonSelector::storeLeptonCells(const xAOD::CaloCluster& theCluster, SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle){

  const CaloClusterCellLink* theCellLink = theCluster.getCellLinks();

  if (theCellLink){
    for (auto theCaloCell : *theCellLink){
      if (leptonCaloCellsWriteHandle.isValid()) leptonCaloCellsWriteHandle->push_back(theCaloCell);
      else ATH_MSG_WARNING(" Do not have valid WriteHandle for CaloCellContaienr with name: " << leptonCaloCellsWriteHandle.key());
    }//cell loop
  }
  else ATH_MSG_WARNING("This cluster has an invalid pointer to its cells, in storeLeptonCells");
}

StatusCode PFLeptonSelector::recordLeptonContainers(SG::WriteHandle<xAOD::ElectronContainer>& selectedElectronsWriteHandle, SG::WriteHandle<xAOD::MuonContainer>& selectedMuonsWriteHandle,SG::WriteHandle<ConstDataVector<CaloCellContainer> > leptonCaloCellsWriteHandle){

  StatusCode sc = selectedElectronsWriteHandle.record(std::make_unique<xAOD::ElectronContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not record electron WriteHandle with key: " << selectedElectronsWriteHandle.key());
    return sc;
  }
    
  sc = selectedMuonsWriteHandle.record(std::make_unique<xAOD::MuonContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not record muon WriteHandle with key: " << selectedMuonsWriteHandle.key());
    return sc;
  }

  if (true == m_storeLeptonCells) {

    //record the cell container
    sc =  leptonCaloCellsWriteHandle.record(std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS));

    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not record CaloCell WriteHandle with key: " << leptonCaloCellsWriteHandle);
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}
