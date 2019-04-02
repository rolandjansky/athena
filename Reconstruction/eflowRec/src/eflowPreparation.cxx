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
  m_tracksName("InDetTrackParticles"),
  m_eflowCaloObjectsOutputName("eflowCaloObjects01"),
  m_caloObjectContainer(0),
  m_eflowRecTracksOutputName("eflowRecTracks01"),
  m_eflowRecClustersOutputName("eflowRecClusters01"),
  m_theTrackExtrapolatorTool("Trk::ParticleCaloExtensionTool",this),
  m_matchingTool("PFTrackClusterMatchingTool/CalObjBldMatchingTool", this),
  m_eflowMode("FullMode"),
  m_selectedElectrons(nullptr),
  m_selectedMuons(nullptr),
  m_leptonCellContainer(nullptr),
  m_egammaTrackMapName("GSFTrackAssociation"),
  m_electronsName("Electrons"),
  m_muonsName("Muons"),
  m_useLeptons(true),
  m_storeLeptonCells(false),
  m_eflowElectronsName("eflowRec_selectedElectrons_EM"),
  m_eflowLeptonCellsName("eflowRec_leptonCellContainer_EM"),
  m_nMatches(0),
  m_upperTrackPtCut(100.0),
  m_fixMuonLogic(false)
{
// The following properties can be specified at run-time
// (declared in jobOptions file)
  declareProperty("ClustersName", m_clustersName);
  declareProperty("CalClustersName", m_clustersCalName);
  declareProperty("TracksName", m_tracksName);
  declareProperty("EflowCaloObjectsOutputName", m_eflowCaloObjectsOutputName);
  declareProperty("EflowRecTracksOutputName", m_eflowRecTracksOutputName);
  declareProperty("EflowRecClustersOutputName", m_eflowRecClustersOutputName);
  declareProperty("PFTrackClusterMatchingTool", m_matchingTool, "The track-cluster matching tool");
  declareProperty("TrackExtrapolatorTool", m_theTrackExtrapolatorTool, "AlgTool to use for track extrapolation");
  declareProperty("EFlowMode", m_eflowMode);
  declareProperty("egammaTrackMapName", m_egammaTrackMapName);
  declareProperty("electronsName", m_electronsName);
  declareProperty("muonsName", m_muonsName);
  declareProperty("useLeptons", m_useLeptons);
  declareProperty("storeLeptonCells", m_storeLeptonCells);
  declareProperty("eflowElectronsName", m_eflowElectronsName);
  declareProperty("eflowLeptonCellsName", m_eflowLeptonCellsName);
  declareProperty("TrackSelectionTool", m_selTool);
  declareProperty("UpperTrackPtCut",m_upperTrackPtCut);
  declareProperty("FixMuonLogic",m_fixMuonLogic);
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

  if (m_useLeptons) m_selectedMuons = new xAOD::MuonContainer(SG::VIEW_ELEMENTS);

  ATH_CHECK(m_selTool.retrieve());

  return sc;
}

StatusCode eflowPreparation::finalize() {

  msg(MSG::INFO) << "Produced " << m_nMatches << " track-cluster matches." << endmsg;

  if (m_useLeptons && m_selectedMuons) delete m_selectedMuons;

  return StatusCode::SUCCESS;

}

StatusCode eflowPreparation::execute() {

  if (m_useLeptons) this->clearContainers();

  StatusCode sc;

  /* Create the eflowCaloObjectContainer and register it */
  m_caloObjectContainer = new eflowCaloObjectContainer();
  sc = evtStore()->record(m_caloObjectContainer, m_eflowCaloObjectsOutputName,false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Could not record eflowCaloObjectContainer in TDS" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  /* Create the eflowRecTrackContainer and register it */
  m_recTrackContainer = new eflowRecTrackContainer();
  sc = evtStore()->record(m_recTrackContainer, m_eflowRecTracksOutputName,false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) {
      msg(MSG::WARNING) << "Could not record eflowRecTrackContainer in TDS" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  /* Create the eflowRecClusterContainer and register it */
  m_recClusterContainer = new eflowRecClusterContainer();
  sc = evtStore()->record(m_recClusterContainer, m_eflowRecClustersOutputName,false);
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

  /* Collect all calo clusters from all cluster containers to m_recClusterContainer */
  if (makeClusterContainer().isFailure()) {
    return StatusCode::SUCCESS;
  }

  /* Apply the track selection, extrapolate, and collect selected tracks to m_recTrackContainer */
  if (makeTrackContainer().isFailure()) {
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
void eflowPreparation::retrieveLCCalCellWeight(double energy, unsigned index, std::map<IdentifierHash,double>& cellsWeight) {
  /* Retrieve the CaloCal cluster container */
  const xAOD::CaloClusterContainer* CaloCalClusterContainer;
  if (evtStore()->retrieve(CaloCalClusterContainer, m_clustersCalName).isFailure() || !CaloCalClusterContainer) {
    msg(MSG::WARNING) << " In retrieveLCCalCellWeight Can not retrieve cal cluster Container: " << m_clustersCalName << endmsg;
    return ;
  }

  /* match CaloCluster with CaloCalCluster to obtain cell weight */
  /* first try the position at 'index'. If we are lucky, the loop can be avoided. */
  const xAOD::CaloCluster* matchedCalCluster = CaloCalClusterContainer->at(index);
  if (!(fabs(energy - matchedCalCluster->rawE()) < 0.001)) {
    matchedCalCluster = 0;
    for (unsigned iCalCalCluster = 0; iCalCalCluster < CaloCalClusterContainer->size();
        ++iCalCalCluster) {
      matchedCalCluster = CaloCalClusterContainer->at(iCalCalCluster);
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

  /* Retrieve the cluster container */
  const xAOD::CaloClusterContainer* thisCaloClusterContainer;
  StatusCode code = evtStore()->retrieve(thisCaloClusterContainer, m_clustersName);
  if (evtStore()->retrieve(thisCaloClusterContainer, m_clustersName).isFailure() || !thisCaloClusterContainer) {
    msg(MSG::WARNING) << " Can not retrieve cluster Container: " << m_clustersName << endmsg;
    return StatusCode::SUCCESS;
  }

  /* Fill the vector of eflowRecClusters */
  unsigned int nClusters = thisCaloClusterContainer->size();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    /* Create the eflowRecCluster and put it in the container */
    eflowRecCluster* thisEFRecCluster = new eflowRecCluster(ElementLink<xAOD::CaloClusterContainer>(*thisCaloClusterContainer, iCluster));

    if (m_clustersCalName != "") {
      std::map<IdentifierHash,double> cellsWeightMap;
      retrieveLCCalCellWeight(thisCaloClusterContainer->at(iCluster)->e(), iCluster, cellsWeightMap);

      if (false) {
        //zhangr
        std::map<IdentifierHash, double>::iterator it = cellsWeightMap.begin();
        for (; it != cellsWeightMap.end(); ++it) {
          std::cout << "zhangrui eflowPreparation " << iCluster << "/" << nClusters << ": e="
                    << thisCaloClusterContainer->at(iCluster)->e() << " (" << it->first << "  "
                    << it->second << ")" << std::endl;
        }
      }

      thisEFRecCluster->setCellsWeight(cellsWeightMap);
    }
    thisEFRecCluster->setClusterId(iCluster);
    m_recClusterContainer->push_back(thisEFRecCluster);

    if (msgLvl(MSG::DEBUG)) {
      const xAOD::CaloCluster* thisCluster = thisCaloClusterContainer->at(iCluster);
      msg(MSG::DEBUG) << "eflowPreparation clus = " << thisCluster->eta() << " "
		      << thisCluster->phi() << " " << thisCluster->e()/cosh(thisCluster->eta()) << " " << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode eflowPreparation::makeTrackContainer() {
  /* Retrieve xAOD::TrackParticle Container, return 'failure' if not existing */
  const xAOD::TrackParticleContainer* trackContainer;
  StatusCode sc = evtStore()->retrieve(trackContainer, m_tracksName);
  if (sc.isFailure() || !trackContainer) {
    if (msgLvl(MSG::WARNING)) { msg(MSG::WARNING) << " No track container found in TDS" << endmsg; }
    return StatusCode::FAILURE;
  }

  /* Do the track selection for tracks to be used in all of the following steps: */
  /* TODO (tuning): Check if resize(0) might be faster than clear() */
  xAOD::TrackParticleContainer::const_iterator itTrackParticle = trackContainer->begin();
  int trackIndex = 0;
  for (; itTrackParticle != trackContainer->end(); ++itTrackParticle, ++trackIndex) {
    const xAOD::TrackParticle* track = (*itTrackParticle);
    if (!track) continue; // TODO: Print a WARNING here!

    bool rejectTrack((m_eflowMode == "FullMode") && !selectTrack(track));

    if (m_useLeptons) {
      bool isElectron = this->isElectron(track);
      bool isMuon = this->isMuon(track);
      if (true == isElectron || true == isMuon) rejectTrack = true;
    }

    if (!rejectTrack) {
      /* Create the eflowRecCluster and put it in the container */
      eflowRecTrack* thisEFRecTrack = new eflowRecTrack(ElementLink<xAOD::TrackParticleContainer>(*trackContainer, trackIndex), m_theTrackExtrapolatorTool);
      thisEFRecTrack->setTrackId(trackIndex);
      m_recTrackContainer->push_back(thisEFRecTrack);
    }
  }

  std::sort(m_recTrackContainer->begin(), m_recTrackContainer->end(), eflowRecTrack::SortDescendingPt());

  return StatusCode::SUCCESS;
}

bool eflowPreparation::selectTrack(const xAOD::TrackParticle* track) {
  if (track->pt()*0.001 < m_upperTrackPtCut) return m_selTool->accept(*track, track->vertex());
  else return false;
}


StatusCode eflowPreparation::recordLeptonContainers(){

  m_selectedElectrons = new xAOD::ElectronContainer(SG::VIEW_ELEMENTS);

  StatusCode sc = evtStore()->record(m_selectedElectrons,m_eflowElectronsName,false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
        << "Could not record egammaContainer in TDS"
        << endmsg;
    return sc;
  }

  if (true == m_storeLeptonCells) {
    m_leptonCellContainer = new ConstDataVector<CaloCellContainer>(SG::VIEW_ELEMENTS);

    //record the cell container
    sc = evtStore()->record(m_leptonCellContainer, m_eflowLeptonCellsName, false);

    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING))
        msg(MSG::WARNING) << "Could not record eflowRec LeptonCellContainer in TDS" << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode eflowPreparation::selectMuons() {

  const xAOD::MuonContainer* muonContainer(NULL);
  StatusCode sc = evtStore()->retrieve(muonContainer, m_muonsName);
  if (sc.isFailure() || !muonContainer) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " No xAOD Muon container found in TDS with the name " << m_muonsName << endmsg;    
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
      if (m_selectedMuons) {
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
	const ElementLink< xAOD::TrackParticleContainer > theLink = theMuon->inDetTrackParticleLink();
	if (theLink.isValid()){
	  const xAOD::TrackParticle* ID_track = *theLink;
	  if (ID_track){
	    if (track == ID_track) return true;
	    if (false == m_fixMuonLogic) return false;
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

  const xAOD::ElectronContainer* egammaContainer(0);
  StatusCode sc = evtStore()->retrieve(egammaContainer, m_electronsName);
  if (sc.isFailure() || !egammaContainer){
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
	  if (m_selectedElectrons) m_selectedElectrons->push_back(const_cast<xAOD::Electron*>(theElectron));
	  else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_selectedElectrons in selectElectrons " << std::endl;
	  if (true == m_storeLeptonCells) this->storeElectronCells(theElectron);
	}//mediumPP
      }//10GeV pt cut
    }//valid egamma pointer
    else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This electron is a NULL pointer " << endmsg;
    
  }//electron loop

  return StatusCode::SUCCESS;

}

bool eflowPreparation::isElectron(const xAOD::TrackParticle* track){

  if (m_selectedElectrons){

    xAOD::ElectronContainer::iterator firstElectron = m_selectedElectrons->begin();
    xAOD::ElectronContainer::iterator lastElectron = m_selectedElectrons->end();
    
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
      if (m_leptonCellContainer) m_leptonCellContainer->push_back(*firstCell);
      else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " Invalid pointer to m_leptonCellContainer in storeLeptonCells" << endmsg;
    }//cell loop
  }
 else if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This cluster has an invalid pointer to its cells, in storeLeptonCells " << endmsg;

}

void eflowPreparation::clearContainers(){
  if (m_useLeptons && m_selectedMuons) m_selectedMuons->clear();
}
