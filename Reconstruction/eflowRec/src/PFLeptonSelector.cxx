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
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Problem selecting electrons " << endmsg;
  }

  /* Select  muons */
  sc = this->selectMuons();
  if (sc.isFailure()) {
    //won't mask out the tracks, but issue WARNING
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Problem selecting muons " << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode PFLeptonSelector::finalize(){ return StatusCode::SUCCESS; }

StatusCode PFLeptonSelector::selectElectrons(){

  if (!m_electronsReadHandle.isValid()){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Invalid read handle to electron container with name: " <<   m_electronsReadHandle.key() << endmsg;
    return StatusCode::FAILURE;
  }

  xAOD::ElectronContainer::const_iterator firstElectron = m_electronsReadHandle->begin();
  xAOD::ElectronContainer::const_iterator lastElectron = m_electronsReadHandle->end();

  for (; firstElectron != lastElectron; ++firstElectron){

    const xAOD::Electron* theElectron = *firstElectron;
    if (theElectron){
      if (theElectron->pt() > 10000){
        bool val_med = false;
	bool gotID = theElectron->passSelection(val_med, "LHMedium");
	if (!gotID) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Electron ID " << endmsg;
	  continue;
	}
	if (true == val_med){
	  if (m_selectedElectronsWriteHandle.isValid()) m_selectedElectronsWriteHandle->push_back(const_cast<xAOD::Electron*>(theElectron));
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Do not have valid WriteHandle for ElectronContainer with name: " << m_selectedElectronsWriteHandle.key() << endmsg;
	  if (true == m_storeLeptonCells) this->storeElectronCells(*theElectron);
	}//mediumPP
      }//10GeV pt cut
    }//valid egamma pointer
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron is a NULL pointer " << endmsg;
    
  }//electron loop

  return StatusCode::SUCCESS;
}

void PFLeptonSelector::storeElectronCells(const xAOD::Egamma& electron){

  const xAOD::CaloCluster* electronCluster = electron.caloCluster(); 
  if (electronCluster){
    this->storeLeptonCells(*electronCluster);      
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron has an invalid pointer to its cluster " << endmsg;

}

StatusCode PFLeptonSelector::selectMuons() {

  if (!m_muonsReadHandle.isValid()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Invalid read handle to muon container with name: " << m_muonsReadHandle.key() << endmsg;    
    return StatusCode::FAILURE;
  }

  xAOD::MuonContainer::const_iterator firstMuon = m_muonsReadHandle->begin();
  xAOD::MuonContainer::const_iterator lastMuon = m_muonsReadHandle->end();

  for (; firstMuon != lastMuon; ++firstMuon) {
    const xAOD::Muon* theMuon = *firstMuon;

    //Details of medium muons are here:
    //https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
    //No need to ask for combined muon, by construction other muons will not have ID track - we just ask for medium muons
    
    xAOD::Muon::Quality muonQuality = theMuon->quality();
    if( muonQuality <= xAOD::Muon::Medium) {   
      if (m_selectedMuonsWriteHandle.isValid()) m_selectedMuonsWriteHandle->push_back(const_cast<xAOD::Muon*>(theMuon));
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Do not have valid WriteHandle for MuonContainer with name: " << m_selectedMuonsWriteHandle.key() << endmsg;
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
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid pointer to its cluster " << endmsg;
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid element link to its cluster " << endmsg;

}

void PFLeptonSelector::storeLeptonCells(const xAOD::CaloCluster& theCluster){

  const CaloClusterCellLink* theCellLink = theCluster.getCellLinks();

  if (theCellLink){
  
    CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
    
    for (; firstCell != lastCell; ++firstCell){
      if (m_leptonCaloCellsWriteHandle.isValid()) m_leptonCaloCellsWriteHandle->push_back(*firstCell);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Do not have valid WriteHandle for CaloCellContaienr with name: " << m_leptonCaloCellsWriteHandle.key() << endmsg;
    }//cell loop
  }
 else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This cluster has an invalid pointer to its cells, in storeLeptonCells " << endmsg;

}

StatusCode PFLeptonSelector::recordLeptonContainers(){

  StatusCode sc = m_selectedElectronsWriteHandle.record(std::make_unique<xAOD::ElectronContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not record electron WriteHandle with key: " << m_selectedElectronsWriteHandle.key() << endmsg;
    return sc;
  }

  sc = m_selectedMuonsWriteHandle.record(std::make_unique<xAOD::MuonContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not record muon WriteHandle with key: " << m_selectedMuonsWriteHandle.key() << endmsg;
    return sc;
  }

  if (true == m_storeLeptonCells) {

    //record the cell container
    sc =  m_leptonCaloCellsWriteHandle.record(std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS));

    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING))msg(MSG::WARNING) << "Could not record CaloCell WriteHandle with key: " << m_leptonCaloCellsWriteHandle << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}
