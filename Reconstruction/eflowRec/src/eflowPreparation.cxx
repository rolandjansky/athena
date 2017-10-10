/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowPreparation.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R. Tovey
CREATED:  8th November, 2001

********************************************************************/

#include "eflowRec/eflowUtil.h"

// INCLUDE HEADER FILES:

#include "eflowRec/eflowPreparation.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCellLink.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "Particle/TrackParticle.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>
#include <map>
#include <iostream>

//  END OF HEADER FILES INCLUDE

//  CONSTRUCTOR:
    
eflowPreparation::eflowPreparation(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_caloClusterReadHandle("CaloTopoCluster"),
  m_caloCalClusterReadHandle("CaloCalTopoCluster"),
  m_trackReadHandle("InDetTrackParticles"),
  m_electronReadHandle("Electrons"),
  m_muonReadHandle("Muons"),
  m_eflowCaloObjectContainerWriteHandle("eflowCaloObjects01"),
  m_eflowRecTrackContainerWriteHandle("eflowRecTracks01"),
  m_eflowRecClusterContainerWriteHandle("eflowRecClusters01"),
  m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool",this),
  m_matchingTool("PFTrackClusterMatchingTool/CalObjBldMatchingTool", this),
  m_eflowMode("FullMode"),
  m_selectedElectronsWriteHandle("eflowRec_selectedElectrons_EM"),
  m_leptonCaloCellContainerWriteHandle("eflowRec_leptonCellContainer_EM"),
  m_useLeptons(true),
  m_storeLeptonCells(false),
  m_nMatches(0),
  m_upperTrackPtCut(100.0)
{
// The following properties can be specified at run-time
// (declared in jobOptions file)
  declareProperty("ClustersName",m_caloClusterReadHandle );
  declareProperty("CalClustersName",m_caloCalClusterReadHandle  );
  declareProperty("TracksName", m_trackReadHandle);
  declareProperty("EflowCaloObjectsOutputName", m_eflowCaloObjectContainerWriteHandle );
  declareProperty("EflowRecTracksOutputName",  m_eflowRecTrackContainerWriteHandle);
  declareProperty("EflowRecClustersOutputName", m_eflowRecClusterContainerWriteHandle);
  declareProperty("PFTrackClusterMatchingTool", m_matchingTool, "The track-cluster matching tool");
  declareProperty("TrackExtrapolatorTool", m_theTrackExtrapolatorTool, "AlgTool to use for track extrapolation");
  declareProperty("EFlowMode", m_eflowMode);
  declareProperty("electronsName", m_electronReadHandle);
  declareProperty("muonsName",  m_muonReadHandle);
  declareProperty("useLeptons", m_useLeptons);
  declareProperty("storeLeptonCells", m_storeLeptonCells);
  declareProperty("eflowElectronsName", m_selectedElectronsWriteHandle);
  declareProperty("eflowLeptonCellsName", m_leptonCaloCellContainerWriteHandle);
  declareProperty("TrackSelectionTool", m_selTool);
  declareProperty("UpperTrackPtCut",m_upperTrackPtCut);
}

eflowPreparation::~eflowPreparation() { }

StatusCode eflowPreparation::initialize() {

  StatusCode sc;

  // tool service
  IToolSvc* myToolSvc;
  sc = service("ToolSvc", myToolSvc);

  if (sc.isFailure()) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_matchingTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find PFTrackClusterMatchingTool" << endmsg;
  }

  sc = m_theTrackExtrapolatorTool.retrieve();

  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Cannot find eflowTrackToCaloExtrapolatroTool " << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_useLeptons) m_selectedMuons = std::make_unique<xAOD::MuonContainer>(SG::VIEW_ELEMENTS);
  
  ATH_CHECK(m_selTool.retrieve());

  return sc;
}

StatusCode eflowPreparation::finalize() {

  ATH_MSG_INFO("Produced " << m_nMatches << " track-cluster matches.");

  return StatusCode::SUCCESS;

}

StatusCode eflowPreparation::execute() {

  if (m_useLeptons) this->clearContainers();
  
  StatusCode sc;

  /* Create the eflowCaloObjectContainer and register it */


  sc = m_eflowCaloObjectContainerWriteHandle.record(std::make_unique<eflowCaloObjectContainer>());
  
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Could not record eflowCaloObjectContainer in TDS" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  /* Create the eflowRecTrackContainer and register it */
  sc =  m_eflowRecTrackContainerWriteHandle.record(std::make_unique<eflowRecTrackContainer>());
  
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Could not record eflowRecTrackContainer in TDS" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  /* Create the eflowRecClusterContainer and register it */

  sc = m_eflowRecClusterContainerWriteHandle.record(std::make_unique<eflowRecClusterContainer>());

  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Could not record eflowRecClusterContainer in TDS" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  if (m_useLeptons) {

    /* Put electron container and list of lepton cells into Storegate 
     By contrast muons do NOT go into storegate, because they are not needed downstream */
    if (recordLeptonContainers().isFailure()) {
      return StatusCode::SUCCESS;
    }

    /* Select some electrons */
    sc = this->selectElectrons();
    if (sc.isFailure()) {
      //won't mask out the tracks, but issue WARNING
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Did not select any electrons " << endmsg;
    }

    /* Select some muons */
    sc = this->selectMuons();
    if (sc.isFailure()) {
      //won't mask out the tracks, but issue WARNING
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Did not select any muons " << endmsg;
    }
  }

  /* Create eflowCaloObject static calo cluster container */
  eflowCaloObject::setClusterContainerPtr(new xAOD::CaloClusterContainer(), new xAOD::CaloClusterAuxContainer());
  
  /* Collect all calo clusters from all cluster containers to m_eflowRecClusterContainerWriteHandle */
  if (makeClusterContainer().isFailure()) {
    return StatusCode::SUCCESS;
  }

  /* Apply the track selection, extrapolate, and collect selected tracks to m_eflowRecTrackContainerWriteHandle */
  if (makeTrackContainer().isFailure()) {
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
void eflowPreparation::retrieveLCCalCellWeight(double energy, unsigned index, std::map<IdentifierHash,double>& cellsWeight) {
  
  /* match CaloCluster with CaloCalCluster to obtain cell weight */
  /* first try the position at 'index'. If we are lucky, the loop can be avoided. */
  /* Note the read handle has been tested to be valid prior to the call of this function */
  const xAOD::CaloCluster* matchedCalCluster = m_caloCalClusterReadHandle->at(index);
  if (!(fabs(energy - matchedCalCluster->rawE()) < 0.001)) {
    matchedCalCluster = nullptr;
    for (unsigned iCalCalCluster = 0; iCalCalCluster < m_caloCalClusterReadHandle->size();
        ++iCalCalCluster) {
      matchedCalCluster = m_caloCalClusterReadHandle->at(iCalCalCluster);
      if (fabs(energy - matchedCalCluster->rawE()) < 0.001) {
        break;
      }
    }
  }
  assert (matchedCalCluster);

  /* obtain cell index and cell weight */
  const CaloDetDescrManager*   calo_dd_man  = CaloDetDescrManager::instance();
  const CaloCell_ID*               calo_id  = calo_dd_man->getCaloCell_ID();
  xAOD::CaloCluster::const_cell_iterator itCell = matchedCalCluster->cell_begin();
  xAOD::CaloCluster::const_cell_iterator endCell = matchedCalCluster->cell_end();
  for (; itCell != endCell; ++itCell) {
    const CaloCell* pCell = *itCell;
    Identifier myId = pCell->ID();
    IdentifierHash myHashId = calo_id->calo_cell_hash(myId);
    cellsWeight[myHashId] = itCell.weight();
  }

  return ;
}

StatusCode eflowPreparation::makeClusterContainer() {

  /* Verify the read handle has a valid pointer, and if not return */
  if (!m_caloClusterReadHandle.isValid()){
    msg(MSG::WARNING) << " Can not retrieve xAOD::CaloClusterContainer with name: " <<  m_caloClusterReadHandle.key()  << endmsg;
    return StatusCode::SUCCESS;
  }
  
  /* Fill the vector of eflowRecClusters */
  unsigned int nClusters = m_caloClusterReadHandle->size();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    /* Create the eflowRecCluster and put it in the container */
    std::unique_ptr<eflowRecCluster> thisEFRecCluster  = std::make_unique<eflowRecCluster>(ElementLink<xAOD::CaloClusterContainer>(*m_caloClusterReadHandle, iCluster));
    
    if (m_caloCalClusterReadHandle.isValid()){
      std::map<IdentifierHash,double> cellsWeightMap;
      retrieveLCCalCellWeight(m_caloClusterReadHandle->at(iCluster)->e(), iCluster, cellsWeightMap);

      if (msgLvl(MSG::DEBUG)) {
        //zhangr
        std::map<IdentifierHash, double>::iterator it = cellsWeightMap.begin();
        for (; it != cellsWeightMap.end(); ++it) {
           msg(MSG::DEBUG) << "zhangrui eflowPreparation " << iCluster << "/" << nClusters << ": e="
                    << m_caloClusterReadHandle->at(iCluster)->e() << " (" << it->first << "  "
                    << it->second << ")" << endmsg;
        }
      }

      thisEFRecCluster->setCellsWeight(cellsWeightMap);
    }
    thisEFRecCluster->setClusterId(iCluster);
    m_eflowRecClusterContainerWriteHandle->push_back(std::move(thisEFRecCluster));

    if (msgLvl(MSG::DEBUG)) {
      const xAOD::CaloCluster* thisCluster = m_caloClusterReadHandle->at(iCluster);
      msg(MSG::DEBUG) << "eflowPreparation clus = " << thisCluster->eta() << " "
		      << thisCluster->phi() << " " << thisCluster->e()/cosh(thisCluster->eta()) << " " << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode eflowPreparation::makeTrackContainer() {

  /* Verify the read handle has a valid pointer, and if not return */
  if (!m_trackReadHandle.isValid()){
    if (msgLvl(MSG::WARNING)) { msg(MSG::WARNING) << "Can not retrieve xAOD::TrackParticleContainer with name: " << m_trackReadHandle.key() << endmsg; }
    return StatusCode::FAILURE;
  }

  /* Do the track selection for tracks to be used in all of the following steps: */
  /* TODO (tuning): Check if resize(0) might be faster than clear() */
  xAOD::TrackParticleContainer::const_iterator itTrackParticle = m_trackReadHandle->begin();
  int trackIndex = 0;
  for (; itTrackParticle != m_trackReadHandle->end(); ++itTrackParticle, ++trackIndex) {
    const xAOD::TrackParticle* track = (*itTrackParticle);
    if (!track) continue; // TODO: Print a WARNING here!

    ATH_MSG_DEBUG("Have track with E, pt, eta and phi of " << track->e() << ", " << track->pt() << ", " << track->eta() << " and " << track->phi());
    
    bool rejectTrack((m_eflowMode == "FullMode") && !selectTrack(track));

    if (m_useLeptons) {
      bool isElectron = this->isElectron(track);
      bool isMuon = this->isMuon(track);
      ATH_MSG_DEBUG("isElectron is " << isElectron << " and isMuon is " << isMuon);
      if (true == isElectron || true == isMuon) rejectTrack = true;
    }

    ATH_MSG_DEBUG("rejectTrack is " << rejectTrack);
    
    if (!rejectTrack) {
      /* Create the eflowRecCluster and put it in the container */
      std::unique_ptr<eflowRecTrack> thisEFRecTrack  = std::make_unique<eflowRecTrack>(ElementLink<xAOD::TrackParticleContainer>(*m_trackReadHandle, trackIndex), m_theTrackExtrapolatorTool);
      thisEFRecTrack->setTrackId(trackIndex);
      m_eflowRecTrackContainerWriteHandle->push_back(std::move(thisEFRecTrack));
    }
  }

  std::sort(m_eflowRecTrackContainerWriteHandle->begin(), m_eflowRecTrackContainerWriteHandle->end(), eflowRecTrack::SortDescendingPt());

  return StatusCode::SUCCESS;
}

bool eflowPreparation::selectTrack(const xAOD::TrackParticle* track) {
  if (track->pt()*0.001 < m_upperTrackPtCut) return m_selTool->accept(*track, track->vertex());
  else return false;
}


StatusCode eflowPreparation::recordLeptonContainers(){

  StatusCode sc = m_selectedElectronsWriteHandle.record(std::make_unique<xAOD::ElectronContainer>(SG::VIEW_ELEMENTS));
  
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
        << "Could not record egammaContainer in TDS"
        << endmsg;
    return sc;
  }

  if (true == m_storeLeptonCells) {

    //record the cell container
    sc =  m_leptonCaloCellContainerWriteHandle.record(std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS));

    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING))
        msg(MSG::WARNING) << "Could not record eflowRec LeptonCellContainer in TDS" << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode eflowPreparation::selectMuons() {

  const xAOD::MuonContainer* muonContainer = m_muonReadHandle.cptr();
  
  if (!muonContainer) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " No xAOD Muon container found in TDS with the name " << m_muonReadHandle.key() << endmsg;    
    return StatusCode::FAILURE;
  }

  xAOD::MuonContainer::const_iterator firstMuon = muonContainer->begin();
  xAOD::MuonContainer::const_iterator lastMuon = muonContainer->end();

  for (; firstMuon != lastMuon; ++firstMuon) {
    const xAOD::Muon* theMuon = *firstMuon;

    //Details of medium muons are here:
    //https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
    //No need to ask for combined muon, by construction other muons will not have ID track - we just ask for medium muons
    
    xAOD::Muon::Quality muonQuality = theMuon->quality();
    if( muonQuality <= xAOD::Muon::Medium) {   
      if (nullptr != m_selectedMuons.get()){
	m_selectedMuons->push_back(const_cast<xAOD::Muon*>(theMuon));
      } else if (msgLvl(MSG::WARNING)) {
	msg(MSG::WARNING) << " Invalid pointer to m_selectedMuons in selectMuons " << std::endl;
      }
      if (true == m_storeLeptonCells) this->storeMuonCells(theMuon);
    }//Medium muons
  } //muon loop

  return StatusCode::SUCCESS;
}

bool eflowPreparation::isMuon(const xAOD::TrackParticle* track){

  if (m_selectedMuons){

    xAOD::MuonContainer::const_iterator firstMuon = m_selectedMuons->begin();
    xAOD::MuonContainer::const_iterator lastMuon = m_selectedMuons->end();
    
    for (; firstMuon != lastMuon ; ++firstMuon){
      const xAOD::Muon* theMuon = *firstMuon;
      if (theMuon){
	ATH_MSG_DEBUG("Considering muon in isMuon with e,pt, eta and phi of " << theMuon->e() << ", " << theMuon->pt() << ", " << theMuon->eta() << " and " << theMuon->phi());
	const ElementLink< xAOD::TrackParticleContainer > theLink = theMuon->inDetTrackParticleLink();
	if (theLink.isValid()){
	  const xAOD::TrackParticle* ID_track = *theLink;
	  if (ID_track){
	    if (track == ID_track) return true;
	    return false;
	  }
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has a NULL pointer to the track " << endmsg;
	}
	else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid link to the track " << endmsg;
      }//if muon pointer is valid
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon is a NULL pointer " << endmsg;
    }//muon loop
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedMuons in isMuon " << std::endl;

  return false;
}


void eflowPreparation::storeMuonCells(const xAOD::Muon* muon){

  const ElementLink<xAOD::CaloClusterContainer> theLink =  muon->clusterLink(); 
  if (theLink.isValid()){
    const xAOD::CaloCluster* muonCluster = *theLink;
    if (muonCluster){
      this->storeLeptonCells(muonCluster);
      const CaloClusterCellLink* theCellLink = muonCluster->getCellLinks();
      CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
      CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
      
      for (; firstCell != lastCell; ++firstCell){
      }//cell loop
      
    }
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid pointer to its cluster " << endmsg;
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This muon has an invalid element link to its cluster " << endmsg;

}


StatusCode eflowPreparation::selectElectrons(){

  
  const xAOD::ElectronContainer* egammaContainer = m_electronReadHandle.cptr();
  if (!egammaContainer){
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " No Electron container found in TDS" << endmsg;
    return StatusCode::FAILURE;
  }

  xAOD::ElectronContainer::const_iterator firstElectron = egammaContainer->begin();
  xAOD::ElectronContainer::const_iterator lastElectron = egammaContainer->end();

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
	  if (true == m_storeLeptonCells) this->storeElectronCells(theElectron);
	}//mediumPP
      }//10GeV pt cut
    }//valid egamma pointer
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron is a NULL pointer " << endmsg;
    
  }//electron loop

  return StatusCode::SUCCESS;

}

bool eflowPreparation::isElectron(const xAOD::TrackParticle* track){

  if (m_selectedElectronsWriteHandle.isValid()){

    xAOD::ElectronContainer::iterator firstElectron = m_selectedElectronsWriteHandle->begin();
    xAOD::ElectronContainer::iterator lastElectron = m_selectedElectronsWriteHandle->end();
    
    for (; firstElectron != lastElectron; ++firstElectron){
      const xAOD::Electron* this_egamma = *firstElectron;
      if (this_egamma){
	unsigned int nTrack = this_egamma->nTrackParticles();
	
	if (0 != nTrack){
	  
	  const xAOD::TrackParticle* origTrack = xAOD::EgammaHelpers::getOriginalTrackParticle(this_egamma);
	  
	  if (origTrack){
	    if (track == origTrack) {
	      return true;
	    }
	  }//if valid track pointer
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Electron object map has NULL pointer to original TrackParticle " << endmsg;
	}//if has a track
	else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Electron object has " << nTrack << " tracks " << endmsg;
      }//if valid pointer
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Electron is a NULL pointer " << endmsg;
    }//electron loop    
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedElectrons in isElectron " << endmsg;

  return false;

}

void eflowPreparation::storeElectronCells(const xAOD::Egamma* electron){

  const xAOD::CaloCluster* electronCluster = electron->caloCluster(); 
  if (electronCluster){
    this->storeLeptonCells(electronCluster);      
  }
  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron has an invalid pointer to its cluster " << endmsg;

}

void eflowPreparation::storeLeptonCells(const xAOD::CaloCluster* theCluster){

  const CaloClusterCellLink* theCellLink = theCluster->getCellLinks();

  if (theCellLink){
  
    CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator lastCell = theCellLink->end();
    
    for (; firstCell != lastCell; ++firstCell){
      if (m_leptonCaloCellContainerWriteHandle.isValid()) m_leptonCaloCellContainerWriteHandle->push_back(*firstCell);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_leptonCellContainer in storeLeptonCells" << endmsg;
    }//cell loop
  }
 else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This cluster has an invalid pointer to its cells, in storeLeptonCells " << endmsg;

}

void eflowPreparation::clearContainers(){
  if (m_useLeptons && m_selectedMuons) m_selectedMuons->clear();
}
