/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/TrackDepositInCaloTool.h"

// --- Tracking ---
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"

// --- Calorimeter ---
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "TileDetDescr/TileDetDescrManager.h"

#include "CaloDetDescr/CaloDetDescriptor.h"
#include "TileDetDescr/TileCellDim.h"
#include "CaloUtils/CaloCellList.h"

//#include "TrackToCalo/CaloCellHelpers.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "ParticleCaloExtension/ParticleCellAssociationCollection.h"
#include "ParticleCaloExtension/ParticleCellIntersection.h"
#include "ParticleCaloExtension/ParticleCaloAssociation.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

// --- ROOT ---
#include "TH1F.h"
#include "TH2F.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
TrackDepositInCaloTool::TrackDepositInCaloTool( const std::string& type, const std::string& name, const IInterface* pInterface ) :
  AthAlgTool(type,name,pInterface),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
  m_caloCellAssociationTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
  m_cellContainer(0),
  m_solenoidRadius(1280)
  //m_solenoidHalfLength(2900)
{
  declareInterface<ITrackDepositInCaloTool>(this);
  declareProperty("ExtrapolatorHandle", m_extrapolator );   
  declareProperty("doExtrapolation",m_doExtr = true);
  declareProperty("doEDeposHist",m_doHist = false);
  declareProperty("DebugMode", m_debugMode = false);
  declareProperty("ShowNeighbours", m_showNeighbours = false);
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////
TrackDepositInCaloTool::~TrackDepositInCaloTool(){}

///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::initialize() {

  if ( AlgTool::initialize().isFailure() ) {
    ATH_MSG_FATAL("AlgTool couldn't be initialized!");
    return StatusCode::FAILURE;
  } 
  
  if ( detStore()->retrieve(m_caloDDM).isFailure() ) {
    ATH_MSG_WARNING("Unable to retrieve CaloDetDescrManager from DetectorStore. Calling the CaloDetDescrManager::instance() method");
    m_caloDDM = CaloDetDescrManager::instance();
  }
  if ( !m_caloDDM ) {
    ATH_MSG_ERROR("Unfortunately that did not succeed. Sending StatusCode::FAILURE");
    return StatusCode::FAILURE;
  }
  
  if ( detStore()->retrieve(m_tileDDM).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve TileDetDescrManager from DetectorStore!");
    return StatusCode::FAILURE;
  }
  if ( !m_tileDDM ) {
    return StatusCode::FAILURE;
  }
    
  if( service("THistSvc", m_histSvc).isFailure() ){
    ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc.");
    return StatusCode::FAILURE;
  }

  if ( m_doHist ) {
    StatusCode sc = bookHistos();
    if(sc.isFailure() ){
      ATH_MSG_ERROR("Unable to book histograms.");
      return sc;
    }
  }
  
  if ( initializeDetectorInfo().isFailure() ) {
    ATH_MSG_ERROR("Could not initialize detector info.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;

  ATH_CHECK(m_extrapolator.retrieve());
  ATH_CHECK(m_caloExtensionTool.retrieve()   );
  ATH_CHECK(m_caloCellAssociationTool.retrieve());
}

///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::finalize() {

  if (AlgTool::finalize().isFailure()) {
    ATH_MSG_ERROR("AlgTool couldn't be finalized()!");
    return StatusCode::FAILURE;
  } 
  
  ATH_MSG_INFO("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// getDeposits
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::getDeposits(const Trk::TrackParameters* par, const CaloCellContainer* caloCellCont) const{

  ATH_MSG_DEBUG("In TrackDepositInCaloTool::getDeposits()");
  std::vector<DepositInCalo> result;
  
  // --- from now on, par can be assumed to exist ---
  if (!par) {
    return result;
  }

  if(!caloCellCont) return result;
  
  const Trk::ParticleHypothesis muonHypo = Trk::muon;

  // --- Preselection of track's crossing with detector elements ---
  std::vector<Amg::Vector3D> extrapolations;
  std::map<double, const CaloDetDescriptor*> caloInfo;
  const Trk::TrackParameters* currentPar = extrapolateToSolenoid(par);
  if (!currentPar) {
    ATH_MSG_WARNING("Extrapolation to solenoid did not succeed!");
    return result;
  }
  if (getTraversedLayers(currentPar, caloInfo, extrapolations).isFailure()) {
    ATH_MSG_WARNING("Failure in getTraversedLayers(). ");
    return result;
  }

  // --- Iterate over all the detector regions ---
  std::map<double, const CaloDetDescriptor*>::const_iterator detDescrIt = caloInfo.begin();
  while (detDescrIt!=caloInfo.end()) {
    const CaloDetDescriptor* descr = detDescrIt->second;
    // ---- extrapolate to entrance of layer ---
    const Trk::TrackParameters* parEntrance = extrapolateToEntranceOfLayer(currentPar, descr);
    if (!parEntrance) {
      ATH_MSG_DEBUG("Could not extrapolate to entrance.");
      detDescrIt++;
      continue;
    }
    // ---- extrapolate to exit of layer ---
    const Trk::TrackParameters* parExit = extrapolateToExitOfLayer(parEntrance, descr);
    if (!parExit) {
      ATH_MSG_DEBUG("Could not extrapolate to exit.");
      // delete parEntrance;
      delete currentPar;
      currentPar = parEntrance;
      detDescrIt++;
      continue;
    }
    // --- Calculate energy loss ---
    double energyLoss = calcEnergy(parEntrance, muonHypo) - calcEnergy(parExit, muonHypo);
    // --- Display some debug info ---
    // ATH_MSG_INFO("Energy loss = " << energyLoss << " for sample " << descr->getSampling());
    double distance = (parEntrance->position()-parExit->position()).mag();
    // if (distance)
    //   ATH_MSG_INFO("Distance = " << distance << " => Eloss/CLHEP::mm = " << energyLoss/distance);

    // --- Retrieve crossed cells ---  
    std::vector<const CaloCell*>* cells = getCaloCellsForLayer(descr, parEntrance, parExit, caloCellCont);

    // --- Add contributions ---
    double sumEnergy = 0;
    double sumEt     = 0;
    bool showNB = m_showNeighbours;
    if (cells) {
      std::vector<const CaloCell*>::const_iterator cellIt = cells->begin();
      while (cellIt!=cells->end()) {
        const CaloCell* cell = (*cellIt);
        if (cell) {
          if (showNB&&m_debugMode) {
            showNeighbours(cell);
          }
          // if (m_debugMode) {
          //   ATH_MSG_INFO("---------- energy = " << cell->energy());
          // }
          sumEnergy += cell->energy();
          sumEt     += cell->et();
        } 
        else {
          ATH_MSG_VERBOSE("Cell NULL pointer found!");
        }
        cellIt++;
      }
      // --- Write DepositInCalo ---
      if (m_debugMode) {
        ATH_MSG_INFO("Energy = " << sumEnergy << " for sample " << descr->getSampling() << " in " << cells->size() << " cells.");
      }
      if (distance) {
        // ATH_MSG_INFO("Eloss/CLHEP::mm = " << sumEnergy/distance);
        result.push_back(DepositInCalo(descr->getSampling(), sumEnergy, energyLoss, sumEt) );
      }
      delete cells;
    }
  
    // --- Free memory and prepare for next round ---
    delete parExit;
    delete currentPar;
    currentPar = parEntrance;
    detDescrIt++;
  }

  // --- Finalization ---
  delete currentPar;
  // ATH_MSG_INFO("Leaving getDeposits()...");
  return result;
}


///////////////////////////////////////////////////////////////////////////////
// - New getDeposits
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::getDeposits(const xAOD::TrackParticle* tp) const {

    ATH_MSG_DEBUG("In TrackDepositsInCaloTool::getDeposits() - new");
    std::vector<DepositInCalo> result;
    const unsigned int nSamples = CaloSampling::getNumberOfSamplings();

// - associate calocells to trackparticle, cone size 0.2, use cache

    const Rec::ParticleCellAssociation* association = 0;
    m_caloCellAssociationTool->particleCellAssociation(*tp,association,0.2,NULL,true);

    if(!association) return result;
       ATH_MSG_VERBOSE(" particleCellAssociation done  " << association );

// - pick up the cell intersections

    std::vector< std::pair<const CaloCell*,Rec::ParticleCellIntersection*> > cellIntersections = association->cellIntersections();

    const Trk::CaloExtension& caloExtension = association->caloExtension();

#if 0
    // This test can never be true for standard-compliant code
    // and is elided by compilers.
    if(!(&caloExtension)) {
      ATH_MSG_WARNING( " No caloExtension found ");
      return result;
    }
#endif

    if(!caloExtension.caloEntryLayerIntersection()) {
      ATH_MSG_WARNING( " No caloEntryLayerIntersection found ");
      return result;
    }

    ATH_MSG_DEBUG( " nr of cell intersections " << cellIntersections.size() );
    if( cellIntersections.size() < 3) ATH_MSG_WARNING( " Strange nr of calorimeter cell intersections " << cellIntersections.size() );

    CaloExtensionHelpers::EntryExitLayerMap  entryExitLayerMap;
    CaloExtensionHelpers::entryExitLayerMap( caloExtension, entryExitLayerMap );
    ATH_MSG_VERBOSE("EntryExitLayerMap " << entryExitLayerMap.size() );

    CaloExtensionHelpers::ScalarLayerMap  eLossLayerMap;
    CaloExtensionHelpers::eLossLayerMap( caloExtension, eLossLayerMap );
    ATH_MSG_VERBOSE("eLossLayerMap " << eLossLayerMap.size() );

    std::vector<float> exp_E(nSamples,0.0);
    std::vector<float> meas_E(nSamples,0.0);
    std::vector<float> meas_Et(nSamples,0.0);
    std::vector<int> LayerHit(nSamples,0);
    CaloSampling::CaloSample sample;
    Amg::Vector3D lEntry, lExit;

// loop over cellIntersections, there can be more than one cell hit in a layer

    for(auto it : cellIntersections){
      const CaloCell* curr_cell = it.first;
      if (curr_cell->badcell()) continue;
      int cellSampling = curr_cell->caloDDE()->getSampling();
      CaloSampling::CaloSample sample = static_cast<CaloSampling::CaloSample>(cellSampling);
      //bool badCell     = curr_cell->badcell();
      auto pos = entryExitLayerMap.find(sample);
      if (pos == entryExitLayerMap.end() ) continue;      
      lEntry = pos->second.first;
      lExit = pos->second.second;
      if (TrackDepositInCaloTool::inCell(curr_cell, lEntry) || TrackDepositInCaloTool::inCell(curr_cell,lExit)){
         meas_E[cellSampling] += Float_t(curr_cell->energy());
         meas_Et[cellSampling] += Float_t(curr_cell->energy() * curr_cell->sinTh());
         exp_E[cellSampling] = Float_t((it.second)->expectedEnergyLoss());
         LayerHit[cellSampling]++;
         ATH_MSG_VERBOSE( " Layer : " << cellSampling << "   Energy = " << curr_cell->energy() << " Exp : " << (it.second)->expectedEnergyLoss());
      }
    }
   
// sum cells per layer and fill samples per layer

    for(int i = CaloSampling::PreSamplerB ; i < CaloSampling::CaloSample::Unknown; i++) {
      if (LayerHit[i] > 0){
         sample = static_cast<CaloSampling::CaloSample>(i);
         result.push_back(DepositInCalo(sample, meas_E[i], exp_E[i], meas_Et[i])) ; 
         ATH_MSG_DEBUG( " Layer : " << sample << "   Energy = " << meas_E[i] << " nCells : " << LayerHit[i] << " Exp: " << exp_E[i]);
      }
    }

    return result;
}

// - end getDeposits


///////////////////////////////////////////////////////////////////////////////
// getCaloCellsForLayer
///////////////////////////////////////////////////////////////////////////////
std::vector<const CaloCell*>* TrackDepositInCaloTool::getCaloCellsForLayer(const CaloDetDescriptor* descr, const Trk::TrackParameters* parEntrance, const Trk::TrackParameters* parExit, const CaloCellContainer* caloCellCont) const {
  if (descr->is_tile()) {
    // --- Tile implemention is lengthy and therefore put in seperate function ---
    return getCaloCellsForTile(descr, parEntrance, parExit, caloCellCont);
  }
  else {
    // --- LAr implementation is short, quick and simple ---
    const CaloCell* cellEntrance = getClosestCellLAr(parEntrance, descr, caloCellCont);
    const CaloCell* cellExit     = getClosestCellLAr(parExit, descr, caloCellCont);
    std::vector<const CaloCell*>* result = new std::vector<const CaloCell*>();
    result->push_back(cellEntrance);
    if (cellEntrance!=cellExit) {
      result->push_back(cellExit);
      // ATH_MSG_INFO("Two cells for LAr retrieved.");
    }
    else {
      // ATH_MSG_INFO("One cell for LAr retrieved.");
    }
    return result;
  }
}


///////////////////////////////////////////////////////////////////////////////
// getCaloCellsForTile
///////////////////////////////////////////////////////////////////////////////
std::vector<const CaloCell*>* TrackDepositInCaloTool::getCaloCellsForTile(const CaloDetDescriptor* descr, const Trk::TrackParameters* parEntrance, const Trk::TrackParameters* parExit, const CaloCellContainer* caloCellCont) const {
/*
  ...to be written...
*/
  ATH_MSG_VERBOSE("in getCaloCellsForTileLayer()...");

  std::vector<const CaloCell*>* result = 0;
  // --- Get all the relevant kinematical variables ---  
  double phiPar         = parEntrance->position().phi();
  double phiParEntrance = parEntrance->position().phi();
  double zParEntrance   = parEntrance->position().z();
  double phiParExit     = parExit->position().phi();
  double zParExit       = parExit->position().z();
  double diffZPar       = zParExit - zParEntrance;
  double etaPar         = parEntrance->position().eta();
  // --- Determine granularity ---
  double etaWidth     = 2*(descr->calo_eta_max() - descr->calo_eta_min())/descr->n_eta();
  double etaMin       = etaPar - etaWidth;
  double etaMax       = etaPar + etaWidth;
  double phiWidth     = (descr->calo_phi_max() - descr->calo_phi_min())/descr->n_phi();
  // TODO: HOW TO DEAL WITH PHI ~ PI?
  double phiMin       = phiPar-phiWidth;
  double phiMax       = phiPar+phiWidth;
  // --- Fill vecHash ---
  CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();

  std::vector<IdentifierHash> vecHash;
  m_caloDDM->cellsInZone(etaMin, etaMax, phiMin, phiMax, sample, vecHash);
  std::vector<IdentifierHash>::iterator it = vecHash.begin();
  
  // const CaloCell* closestCell[2] = {0,0};
  result = new std::vector<const CaloCell*>();
  
  // --- Iterate and find closest to track (around 12-15 elements in loop) --- 
  std::map<double, const CaloCell*> neighbourMap0;
  std::map<double, const CaloCell*> neighbourMap1;

  while (it!=vecHash.end()) {
    const CaloCell* cell = caloCellCont->findCell(*it);
    if (cell) {
      const CaloDetDescrElement* dde = cell->caloDDE();
      if (dde) {
        // --- Get dimensions of TileCell ---
        // ATH_MSG_INFO("e = " << cell->energy());
        
        double dPhiCell   = dde->dphi();
        double phiCellMin = dde->phi() - dPhiCell/2;
        double phiCellMax = phiCellMin + dPhiCell;
        // ATH_MSG_INFO("phi[] = (" << phiCellMin << ", " << phiCellMax << "), phiPar = " << phiParEntrance);
        // --- Veto on phi acceptance (assuming no crossings of phi-boundaries in TileCal (dPhi ~ 0.1) ) ---
        if (!(phiParEntrance>phiCellMin && phiParExit<phiCellMax)) {
          if (!(phiParExit>phiCellMin && phiParExit<phiCellMax)) {
            it++;
            continue;
          }
        }
        
        // --- There are two z dimensions for the BC cells ---
        double zCellMin0  = 0; 
        double zCellMax0  = 0;
        double zCellMin1  = 0;
        double zCellMax1  = 0;
        TileCellDim* cellDim = m_tileDDM->get_cell_dim(cell->ID());
        if (cellDim) {
          // --- This is valid for the BC cells ---
          if (cellDim->getNRows()==6) {
            zCellMin0 = cellDim->getZMin(0);
            zCellMax0 = cellDim->getZMax(0);
            zCellMin1 = cellDim->getZMin(3);
            zCellMax1 = cellDim->getZMax(3);
          }
          else {
            zCellMin0 = cellDim->getZMin(0);
            zCellMax0 = cellDim->getZMax(0);
            zCellMin1 = zCellMin0;
            zCellMax1 = zCellMax0;
          }
        }
        else {
          ATH_MSG_WARNING("TileCellDim object not found for cell " << cell->ID());
        }
        // ATH_MSG_INFO("z0[] = (" << zCellMin0 << ", " << zCellMax0 << "), zPar = " << zParEntrance);
        // ATH_MSG_INFO("z1[] = (" << zCellMin1 << ", " << zCellMax1 << "), zPar = " << zParExit);
        
        // --- Check wether entrance parameters are within cell acceptance at entrance ---
        // --- The equal signs (>=, <=) are very important for the boundary cells ---
        if (zParEntrance>=zCellMin0 && zParEntrance<=zCellMax0) {
          if (phiParEntrance>phiCellMin && phiParEntrance<phiCellMax) {
            result->push_back(cell);
            // ATH_MSG_INFO("SELECTED");
            it++;
            continue;
          }
        }
        // --- Check wether exit parameters are within cell acceptance at exit ---
        if (zParExit>=zCellMin1 && zParExit<=zCellMax1) {
          if (phiParExit>phiCellMin && phiParExit<phiCellMax) {
            result->push_back(cell);
            // ATH_MSG_INFO("SELECTED");
            it++;
            continue;
          }
        }
        // --- Check wether it crosses a cell ---
        if (diffZPar>0) {
          if (zParEntrance<zCellMin0 && zParExit>zCellMax1) {
            result->push_back(cell);
            // ATH_MSG_INFO("SELECTED");
            it++;
            continue;
          }
        }
        else {
          if (zParEntrance>zCellMax0 && zParExit<zCellMin1) {
            result->push_back(cell);
            it++;
            continue;
          }
        }
      }
    }
    it++;
  }
  
/*  if (m_debugMode&&m_showNeighbours) {
    ATH_MSG_INFO("SAMPLE = " << sample);
    std::map<double, const CaloCell*>::const_iterator mapIt0 = neighbourMap0.begin();
    ATH_MSG_INFO("zEntr = " << zPar[0] << ", phiEntr = " << phiPar[0]);
    ATH_MSG_INFO("zExit = " << zPar[1] << ", phiExit = " << phiPar[1]);    
    while (mapIt0!=neighbourMap0.end()) {
      const CaloCell* cell = mapIt0->second;
      double distance = mapIt0->first;
      if (cell) {
        if (cell==closestCell[0])
          ATH_MSG_INFO("SELECTED CELL:");
        if (cell==closestCell[1]) 
          ATH_MSG_INFO("SELECTED CELL:");
        const CaloDetDescrElement* dde = cell->caloDDE();
        double dPhiCell = dde->dphi();
        double dZCell = dde->dz();
        double zCellMin = dde->z() - dZCell/2;
        double phiCellMin = dde->phi() - dPhiCell/2;
        double zCellMax = zCellMin + dZCell;
        double phiCellMax = phiCellMin + dPhiCell;
        ATH_MSG_INFO("energy = " << cell->energy() << ", ");
        ATH_MSG_INFO("(" << zCellMin << ", " << zCellMax << "), (" << phiCellMin << ", " << phiCellMax << ")");
      }
      else {
        ATH_MSG_INFO("dR2 = " << distance << ", NULL pointer!");
      }
      mapIt0++;
    }
  }
  
  
  result = new std::vector<const CaloCell*>();
  result->push_back(closestCell[0]);
  if (closestCell[1]!=closestCell[0]) {
    result->push_back(closestCell[1]);
    if (m_debugMode) {
      double e0 = -99;
      double e1 = -99;
      if (closestCell[0]) {
        e0 = closestCell[0]->energy();
      }
      if (closestCell[1]) {
        e1 = closestCell[1]->energy();
      }
      ATH_MSG_INFO("2 Cells: energy0 = " << e0 << ", energy1 = " << e1);
    }
  }
  else {
    if (m_debugMode) {
      ATH_MSG_INFO("Deposits contained in one cell");
    }
  }*/
  
  // ATH_MSG_INFO(result->size() << " cells for sample " << descr->getSampling());
  
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateToEntraceOfLayer
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackDepositInCaloTool::extrapolateToEntranceOfLayer(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const {

  // --- Initialization ---
  const Trk::TrackParameters* paramEntrance = 0;
  bool checkBoundary = true;
  Trk::ParticleHypothesis muonHypo = Trk::muon;

  Trk::Surface* surfEntrance = createSurface(descr, Entrance);
  if (surfEntrance) {
    // --- Try to extrapolate to entrance --- 
    paramEntrance = m_extrapolator->extrapolate(*par, *surfEntrance, Trk::alongMomentum, !checkBoundary, muonHypo);
    if (paramEntrance) {
      Amg::Vector3D posVecEntrance = paramEntrance->position();
      // --- If the parameters are not on surface, extrapolate to side of calorimeter ---
      if (!surfEntrance->isOnSurface(posVecEntrance, checkBoundary) ) {
        Trk::Surface* surfInside = createSurface(descr, Inside);
        if (surfInside) {
          const Trk::TrackParameters* paramOnInside = m_extrapolator->extrapolate(*paramEntrance, *surfInside, Trk::alongMomentum, checkBoundary, muonHypo);
          if (paramOnInside) {
            ATH_MSG_VERBOSE("Successfully extrapolated to inner side of calo for sample " << descr->getSampling());
            delete paramEntrance;
            paramEntrance = paramOnInside;
          }
          else {
            ATH_MSG_DEBUG("Extrapolation to entrance failed!");
          }
          delete surfInside;
        }
      }
      else {
        ATH_MSG_VERBOSE("Successfully extrapolated to entrance of calo for sample " << descr->getSampling());
      }
    }
    else {
      ATH_MSG_DEBUG("Extrapolation to entrance failed without boundary check.");
    }
    delete surfEntrance;
  }
  else {
    ATH_MSG_DEBUG("Error in extrapolateToBeginOfLayer()");
  }
  
  return paramEntrance;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateToExitOfLayer
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackDepositInCaloTool::extrapolateToExitOfLayer(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const {
  
  //  const Trk::TrackParameters* paramEntrance = 0;
  const Trk::TrackParameters* paramExit = 0;
  bool checkBoundary = true;
  Trk::ParticleHypothesis muonHypo = Trk::muon;

  // --- Create surface parallel to layer ---
  Trk::Surface* surfExit = createSurface(descr, Exit);
  if (surfExit) {
    // --- Try to extrapolate to exit of layer ---
    paramExit = m_extrapolator->extrapolate(*par, *surfExit, Trk::alongMomentum, checkBoundary, muonHypo);
    if (paramExit) {
      ATH_MSG_VERBOSE("Extrapolated to exit. ");
    }
    else {
      // --- Try to extrapolate to side ---
      Trk::Surface* surfOutside = createSurface(descr, Outside);
      if (surfOutside) {
        paramExit = m_extrapolator->extrapolate(*par, *surfOutside, Trk::alongMomentum, checkBoundary, muonHypo);
        if (paramExit) {
          ATH_MSG_VERBOSE("Succesfully extrapolated to outer side of calo for sample " << descr->getSampling());
        }
        else {
          ATH_MSG_VERBOSE("Could not extrapolate to exit of calo.");
        }
        delete surfOutside;
      }
      else {
        ATH_MSG_WARNING("Could not create surface for outside of calo for sample " << descr->getSampling());
      }
    }
    delete surfExit;
  }
  else {
    ATH_MSG_WARNING("Could not create exit surface.");
  }
  
  return paramExit;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateToSolenoid
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* TrackDepositInCaloTool::extrapolateToSolenoid(const Trk::TrackParameters* par, bool oppositeMomentum) const {
  
  const Trk::TrackParameters* parAtSolenoid = 0;
  Trk::PropDirection direction = Trk::alongMomentum;
  if (oppositeMomentum) {
    direction = Trk::oppositeMomentum;
  }
  
  // --- First extrapolate to solenoid radius or EndCap disk ---
  // --- Ownership of the HepTransform3Ds is passed to the Surfaces ---
  double halfLengthOfCylinder = 3700;
  bool checkBoundary = true;
  Trk::CylinderSurface solenoid(m_solenoidRadius, halfLengthOfCylinder);
  Trk::ParticleHypothesis muonHypo = Trk::muon;  
  
  parAtSolenoid = m_extrapolator->extrapolate(*par, solenoid, direction, checkBoundary, muonHypo);
  if (!parAtSolenoid) {
    // --- Guess EndCap side by direction ---
    double zTrans = par->eta()>0? halfLengthOfCylinder : -halfLengthOfCylinder;
    Trk::DiscSurface disc(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,zTrans))),
                          0, m_solenoidRadius);

    parAtSolenoid = m_extrapolator->extrapolate(*par, disc, direction, checkBoundary, muonHypo);

    if (!parAtSolenoid) {
      ATH_MSG_VERBOSE("extrapolateToSolenoid(): Extrapolation to cap of solenoid failed. Trying opposite side.");
      Trk::DiscSurface discOpp(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-zTrans))),
                               0, m_solenoidRadius);
      parAtSolenoid = m_extrapolator->extrapolate(*par, discOpp, direction, checkBoundary, muonHypo);
    }
    
    if (parAtSolenoid) {
      ATH_MSG_VERBOSE("extrapolateToSolenoid(): Extrapolation succeeded for disc-type surface.");
    }
    
  }
  else {
    ATH_MSG_VERBOSE("extrapolateToSolenoid(): Extrapolation succeeded for cylinder-type surface.");
  }
  
  return parAtSolenoid;
}

///////////////////////////////////////////////////////////////////////////////
// deposits
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::deposits(const Trk::TrackParameters* par, const double deltaR, const bool inCell) const {
/*
  This method retrieves a vector of DepositInCalo (sample, energy, energyloss, et) containing one entry for each
  calorimeter sample traversed. The method is written for muons which are MIPs, so they don't shower very much. This is the
  reason why only the one sample is returned that is closest to the muon track. 
  The algorithm for finding the deposits works as follows. First a preselection is done by getTraversedLayers(). This returns
  a vector of CaloDetDescriptors*. From these CaloDetDescriptors surfaces are built to which the extrapolator extrapolates
  the track parameters . The extrapolator first tries to hit the entrance surface, but if it fails it will try to hit the 
  sides. From there on, the track is extrapolated to the middle of the Calorimeter and the CaloCell that is closest to this point is retrieved
  using the function getClosestCell. Eventually the parameters are extrapolated to the exit, or outer side of the sample.
*/
  ATH_MSG_DEBUG("In TrackDepositInCaloTool::deposits()");
  std::vector<DepositInCalo> result;
  
  // --- Stupid workaround to get rid of compiler warnings, parameters kept for backwards compatibility ---
  if (!inCell) if (deltaR) {}
  
  // --- Possible crash prevention ---
  if (!par) {
    return result;
  }

  if (!m_cellContainer) {
    ATH_MSG_WARNING("Cell container retrieved, but pointer = 0");
    return result;
  }
  
  const Trk::ParticleHypothesis muonHypo = Trk::muon;
  bool checkBoundary   = true;

  // --- Preselection of track's crossing with detector elements ---
  std::vector<Amg::Vector3D> extrapolations;
  std::map<double, const CaloDetDescriptor*> caloInfo;
  const Trk::TrackParameters* parAtSolenoid = extrapolateToSolenoid(par);
  if (parAtSolenoid) {
    if (getTraversedLayers(parAtSolenoid, caloInfo, extrapolations).isFailure()) {
      delete parAtSolenoid; 
      ATH_MSG_WARNING("Failure in finding getTraversedLayers. ");
      return result;
    }
  }
  else {
    ATH_MSG_WARNING("Failure in extrapolating to solenoid surface.");
    return result;
  }

  delete parAtSolenoid;
    
  // --- Loop over the cells from preselection ---
  std::map<double, const CaloDetDescriptor*>::iterator it = caloInfo.begin();
  std::vector<Amg::Vector3D>::iterator itP = extrapolations.begin();
  while (it!=caloInfo.end() ) {
    // --- Initialization of variables to be determined below ---
    double energyEntrance  = 0;
    double energyExit      = 0;
    double energyDeposit   = 0;
    double ETDeposit       = 0;
    double energyLoss      = 0;
    bool eLossFound        = false;
    const CaloDetDescriptor* descr = it->second;
    CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();
    ATH_MSG_VERBOSE("Analysing crossing of calo sample " << sample << " ");

    // --- Extrapolate to entrance of layer ---
    const Trk::Surface* surfEntrance = createSurface(descr, Entrance);
    if (surfEntrance) {
      const Trk::TrackParameters* paramEntrance = m_extrapolator->extrapolate(*par, *surfEntrance, Trk::alongMomentum, !checkBoundary, muonHypo);
      if (paramEntrance) {
        Amg::Vector3D posVecEntrance = paramEntrance->position();
        if (!surfEntrance->isOnSurface(posVecEntrance, checkBoundary) ) {
          Trk::Surface* surfInside = createSurface(descr, Inside);
          if (surfInside) {
            const Trk::TrackParameters* paramOnInside = m_extrapolator->extrapolate(*paramEntrance, *surfInside, Trk::alongMomentum, checkBoundary, muonHypo);
            if (paramOnInside) {
              ATH_MSG_VERBOSE("Succesfully extrapolated to inner side of calo for sample " << sample);
              delete paramEntrance;
              paramEntrance = paramOnInside;
            }
            else {
              ATH_MSG_WARNING("Extrapolation failed to inner side of calo " << sample);
            }
            delete surfInside;
          }
          else {
            ATH_MSG_WARNING("Could not create surface for inside of calo for sample " << sample);
          }
        }
        energyEntrance = calcEnergy(paramEntrance, muonHypo);
        // --- Extrapolate to middle of layer ---
        Trk::Surface* surfMiddle = createSurface(descr, Middle);
        if (surfMiddle) {
          const Trk::TrackParameters* paramMiddle = m_extrapolator->extrapolate(*paramEntrance, *surfMiddle, Trk::alongMomentum, checkBoundary, muonHypo);
          if (paramMiddle) {
            // Get energy:
            // CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();
            const CaloCell* cell = getClosestCell(paramMiddle, descr);
            if (cell) {
              energyDeposit = cell->energy();
              ETDeposit     = cell->et();
            }
            // energyDeposit = DepositInCell(const_cast<CaloDetDescriptor*>(descr)->getSampling(), paramMiddle->eta(), paramMiddle->momentum().phi(), 0.1);
            // --- Extrapolate to exit of layer ---
            Trk::Surface* surfExit = createSurface(descr, Exit);
            const Trk::TrackParameters* paramExit;
            if (surfExit) {
              paramExit = m_extrapolator->extrapolate(*paramMiddle, *surfExit, Trk::alongMomentum, checkBoundary, muonHypo);
              if (paramExit) {
                ATH_MSG_VERBOSE("Extrapolated to exit. ");
                energyExit = calcEnergy(paramExit, muonHypo);
                eLossFound = true;
              }
              else {
                // Try to extrapolate to outside
                Trk::Surface* surfOutside = createSurface(descr, Outside);
                if (surfOutside) {
                  paramExit = m_extrapolator->extrapolate(*paramMiddle, *surfOutside, Trk::alongMomentum, checkBoundary, muonHypo);
                  if (paramExit) {
                    ATH_MSG_VERBOSE("Succesfully extrapolated to outer side of calo for sample " << sample);
                    energyExit = calcEnergy(paramExit, muonHypo);
                    eLossFound = true;
                  }
                  else {
                    ATH_MSG_VERBOSE("Could not extrapolate to exit of calo.");
                  }
                  delete surfOutside;
                }
                else {
                  ATH_MSG_WARNING("Could not create surface for outside of calo for sample " << sample);
                }
              }
              delete surfExit;
            }
            else {
              ATH_MSG_WARNING("Could not create exit surface.");
            }
            if (eLossFound)
              delete paramExit;
/*            if (eLossFound) {
              if (descr->is_tile()) {
                std::map<double, const CaloCell*> cells = getCellsTile(paramEntrance, descr);
                std::map<double, const CaloCell*>::iterator cellIt = cells.begin();
                ATH_MSG_INFO(" ----------------");
                while (cellIt!=cells.end()) {
                  const CaloCell* cell = cellIt->second;
                  ATH_MSG_INFO("Deposit of " << cell->energy() << ", sample " << sample << ", eta = " 
                    << cell->caloDDE()->eta() << ", phi = " << cell->caloDDE()->phi());
                  energyDeposit += cell->energy();
                  cellIt++;
                }
                ATH_MSG_INFO(" ----------------");
              }}  */

            delete paramMiddle;
          }
          else {
            ATH_MSG_VERBOSE("Could not extrapolate to middle of calo.");
          }
          delete surfMiddle;
        }
        else {
          ATH_MSG_VERBOSE("Could not create middle surface.");
        }
        delete paramEntrance;
      }
      else {
        ATH_MSG_VERBOSE("Could not extrapolate to entrance of calo.");
      }
      delete surfEntrance;
    }
    else {
      ATH_MSG_VERBOSE("Could not create entrance surface.");
    }
    
    energyLoss = eLossFound? -(energyExit-energyEntrance) : 0;
    result.push_back(DepositInCalo(sample, energyDeposit, energyLoss, ETDeposit) );
    if (m_debugMode)
      ATH_MSG_INFO("Sample: " << sample << "\tEnergyDeposit: " << energyDeposit << "\tEnergyLoss: " << energyLoss);

    if (m_doHist) {
      m_hParELossEta->Fill(energyLoss, itP->eta() ); 
      m_hParELossSample->Fill(energyLoss, sample ); 
    }

    itP++;
    it++;
  }
  
  ATH_MSG_VERBOSE("---TRACKDEPOSITINCALOTOOL---TRACKDEPOSITINCALOTOOL---TRACKDEPOSITINCALOTOOL");
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// calcEnergy
///////////////////////////////////////////////////////////////////////////////
double TrackDepositInCaloTool::calcEnergy(const Trk::TrackParameters* par, const Trk::ParticleHypothesis& particleHypo) const {
  static Trk::ParticleMasses masses;  
  double mass = masses.mass[particleHypo]; 
  if (par == NULL) return 0.;
  double pX = par->momentum().x();
  double pY = par->momentum().y();
  double pZ = par->momentum().z();
  double energy =  sqrt( mass*mass + pX*pX + pY*pY + pZ*pZ );  
  return energy; 
}

///////////////////////////////////////////////////////////////////////////////
// initializeDetectorInfo()
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::initializeDetectorInfo() {

  ATH_MSG_DEBUG("In CaloTrkMuIdDetStore::initialize()");
  // Initialize LAr
  for (const CaloDetDescriptor* descr : m_caloDDM->calo_descriptors_range()) {
    if (descr) {
      CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();      
      ATH_MSG_VERBOSE("Detector Description element for sample " << sample);
      if ( descr->is_lar_em_barrel() ) {
        ATH_MSG_VERBOSE("  this is a cylindrical detector element.");
        double thickness = descr->calo_r_max() - descr->calo_r_min();
        double r = descr->calo_r_min() + thickness/2.0;
        ATH_MSG_VERBOSE("  R = " << r);
        ATH_MSG_VERBOSE("  sign = " << descr->calo_sign());
        ATH_MSG_VERBOSE("  range_low  = " << descr->calo_z_min());
        ATH_MSG_VERBOSE("  range_high = " << descr->calo_z_max());      
        if (sample!=CaloCell_ID::PreSamplerB) 
          m_barrelLayerMap[r].push_back(descr);
      }
      else {
        ATH_MSG_VERBOSE("  this is a disk-like detector element.");
        double thickness = descr->calo_z_max() - descr->calo_z_min();
        double z = descr->calo_z_min() + thickness/2.0;
        ATH_MSG_VERBOSE("  Z = " << z);
        ATH_MSG_VERBOSE("  sign = " << descr->calo_sign());
        ATH_MSG_VERBOSE("  range_low  = " << descr->calo_r_min());
        ATH_MSG_VERBOSE("  range_high = " << descr->calo_r_max());  
        if (sample!=CaloCell_ID::PreSamplerE) {
          m_endCapLayerMap[z].push_back(descr);
        }
      }
    }
    else 
      ATH_MSG_VERBOSE("CaloDetDescriptor was not available!");
  }
  
  ATH_MSG_VERBOSE("Processing tiles... ");
  for (const CaloDetDescriptor* descr : m_caloDDM->tile_descriptors_range()) {
    if (descr) {
      ATH_MSG_VERBOSE("Detector Description element for sample " << const_cast<CaloDetDescriptor*>(descr)->getSampling());
      if (!descr->is_tile() ) {
        ATH_MSG_VERBOSE("This is not a isTile()==true element.");
      }
      CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();
      if (sample >= 15 && sample <= 17) {
        // --- Skip the TileGap detector elements ---
        continue;
      }
      ATH_MSG_VERBOSE("  this is a cylindrical detector element.");
      double thickness = descr->calo_r_max() - descr->calo_r_min();
      double r = descr->calo_r_min() + thickness/2.0;
      ATH_MSG_VERBOSE("  r = " << r);
      ATH_MSG_VERBOSE("  sign = " << descr->calo_sign());
      ATH_MSG_VERBOSE("  range_low  = " << descr->calo_z_min());
      ATH_MSG_VERBOSE("  range_high = " << descr->calo_z_max());      
      m_barrelLayerMap[r].push_back(descr);
    }
    else 
      ATH_MSG_VERBOSE("CaloDetDescriptor was not available!");
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateR
///////////////////////////////////////////////////////////////////////////////
Amg::Vector3D* TrackDepositInCaloTool::extrapolateR(const Amg::Vector3D& initialPosition, double phi0, double theta0, double r) const {
  double x0 = initialPosition.x();
  double y0 = initialPosition.y();
  double z0 = initialPosition.z();

  double b = 2*x0*cos(phi0)+2*y0*sin(phi0);
  double c = x0*x0 + y0*y0 - r*r;
  double det = b*b-4*c;
  double lsin = 0;
  if (det<0) {
    // ATH_MSG_WARNING("ExtrapoateR(): extrapolation did not succeed (determinant == 0)!");
    return 0;
  }
  else 
   lsin = (-b + sqrt(det))/2;

  double xe = x0 + lsin*cos(phi0);
  double ye = y0 + lsin*sin(phi0);
  double ze = z0 + lsin*cos(theta0)/sin(theta0);

  if (fabs(xe*xe+ye*ye - r*r)>10&&det>0)  {
    // ATH_MSG_WARNING("ExtrapoateR(): extrapolation did not succeed!");
    return 0;
  }
  // ATH_MSG_INFO("ExtrapolateR(): Extrapolation succeeded.");
  return new Amg::Vector3D(xe,ye,ze);
}

///////////////////////////////////////////////////////////////////////////////
// extrapolateZ
///////////////////////////////////////////////////////////////////////////////
Amg::Vector3D* TrackDepositInCaloTool::extrapolateZ(const Amg::Vector3D& initialPosition, double phi0, double theta0, double z) const {
  double x0 = initialPosition.x();
  double y0 = initialPosition.y();
  double z0 = initialPosition.z();
  double cosTheta0 = cos(theta0);
  if (cosTheta0) {
    double dist = z-z0;
    double lambda = dist/cos(theta0);
    if (lambda<0) {
      // ATH_MSG_WARNING("ExtrapolateZ(): extrapolation did not succeed!");
      return 0;
    }
    double xe = x0 + lambda*sin(theta0)*cos(phi0);
    double ye = y0 + lambda*sin(theta0)*sin(phi0);  
    double ze = z;
    // ATH_MSG_INFO("ExtrapolateZ(): Extrapolation succeeded.");
    return new Amg::Vector3D(xe, ye, ze);
  }
  else {
    // ATH_MSG_WARNING("ExtrapolateZ(): extrapolation did not succeed!");
    return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
// getTraversedLayers
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::getTraversedLayers(const Trk::TrackParameters* par, std::map<double, const CaloDetDescriptor*>& caloInfo , std::vector<Amg::Vector3D>& extrapolations) const {

  const Trk::TrackParameters* parAtSolenoid = 0;
  // --- To be replaced by a check, possibly extrapolating to solenoid surface if needed ---
  bool parIsAtSolenoid = true;
  if (parIsAtSolenoid)
    parAtSolenoid = par;
  
  // --- This performs a straight line extrapolation and determines wether it is in calorimeter layer acceptance. ---
  if (parAtSolenoid) {
    // ATH_MSG_INFO("Parameters at solenoid are well-defined.");

    double deltaR_solLast   = fabs( parAtSolenoid->position().perp() - par->position().perp() );
    double deltaEta_solLast = fabs( parAtSolenoid->position().eta() - par->position().eta() );
    if (m_doHist) {
      m_hDeltaEtaLastPar->Fill(deltaEta_solLast);
      m_hDeltaRadiusLastPar->Fill(deltaR_solLast);
    }
  
    const Amg::Vector3D positionAtSolenoid = parAtSolenoid->position();
    double theta0 = parAtSolenoid->momentum().theta();
    double phi0   = parAtSolenoid->momentum().phi();
    CaloLayerMapIt mapIt;

    // --- This Code fragment determines the Barrel crossings ---
    mapIt = m_barrelLayerMap.begin();
    while (mapIt!=m_barrelLayerMap.end()) {
      double radius = mapIt->first;
      Amg::Vector3D* extrapolation = extrapolateR(positionAtSolenoid, phi0, theta0, radius);
      if (extrapolation) {
        std::vector<const CaloDetDescriptor*>::const_iterator vecIt = mapIt->second.begin();
        while(vecIt!=mapIt->second.end()) {
          const CaloDetDescriptor* descr = *vecIt;
          double zSigned = extrapolation->z()*descr->calo_sign();
          if (zSigned>=descr->calo_z_min() && zSigned<=descr->calo_z_max()) {
            double distance = (*extrapolation-positionAtSolenoid).mag();
            caloInfo[distance] = descr;
            extrapolations.push_back(*extrapolation);
          }
          vecIt++;
        }
        delete extrapolation;
      }
      mapIt++;
    }
    
    // This code fragment determines the EndCap crossings
    mapIt = m_endCapLayerMap.begin();
    while (mapIt!=m_endCapLayerMap.end()) {
      double zCenter = mapIt->first;
      std::vector<const CaloDetDescriptor*>::const_iterator vecIt = mapIt->second.begin();
      while(vecIt!=mapIt->second.end()) {
        const CaloDetDescriptor* descr = *vecIt;
        double z = zCenter*descr->calo_sign();
        Amg::Vector3D* extrapolation = extrapolateZ(positionAtSolenoid, phi0, theta0, z);
        if (extrapolation) {
          double radius = extrapolation->perp();
          if (radius>=descr->calo_r_min() && radius<=descr->calo_r_max()) {
            double distance = (*extrapolation-positionAtSolenoid).mag();
            caloInfo[distance] = descr;
            extrapolations.push_back(*extrapolation);
          }
          delete extrapolation;
        }
        vecIt++;
      }
      mapIt++;
    }
    
  }
  else {
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// createSurface()
///////////////////////////////////////////////////////////////////////////////
Trk::Surface* TrackDepositInCaloTool::createSurface(const CaloDetDescriptor* descr, CaloSurfaceType type) const {
/*
Creates a Calorimeter surface for parameter descr. For barrel layers CaloSurfaceType Entrance, Middle and Exit yield 
a Trk::CylinderSurface. For CaloSurfaceTypes Inside and Outside the result will be a Trk::DiscSurface. Inside in
this case is the smallest abs(z) and Outside the largest abs(z) for the two possible sides. For EndCap layers,
Inside is the cylinder surface with smallest radius, and Outside the one with the largest radius. It all makes sense
when you realize that CaloTrkMuId extrapolates a track outwards from the Inner Detector. The ownership of the 
transforms is passed to the Trk::Surface result.
*/
  Trk::Surface* res = 0;
  if (descr->is_tile() || descr->is_lar_em_barrel() ) {
    // --- Cylindrical calo elements ---
    if (type>=Entrance&&type<=Exit) {
      double thickness  = descr->calo_r_max() - descr->calo_r_min();
      double halfLength = (descr->calo_z_max() - descr->calo_z_min())/2;
      double middle     = descr->calo_z_min() + halfLength;
      double radius = type/2.0*thickness + descr->calo_r_min();
      // ATH_MSG_INFO("r = " << radius << " for type " << type << " and sample " << descr->getSampling());
//    HepGeom::Transform3D* trans = new HepGeom::Translate3D(0,0,descr->calo_sign()*middle);
      res = new Trk::CylinderSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,descr->calo_sign()*middle)),
                                     radius, halfLength);
      return res;
    }
    else if (type==Inside||type==Outside) {
      double offset;
      if (type==Inside) {
        offset = descr->calo_z_min()*descr->calo_sign();
      }
      else {
        offset = descr->calo_z_max()*descr->calo_sign();
      }
      res = new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,offset)),
                          descr->calo_r_min(), descr->calo_r_max());
      return res;
    }
    else {
      ATH_MSG_WARNING("Type error in CaloTrkMuIdDetStore::createSurface().");
      return 0;
    }
  }
  else {
    // --- Disc-like calo elements (EndCap) ---
    if (type>=Entrance&&type<=Exit) {
      double thickness = descr->calo_z_max() - descr->calo_z_min();
      double offset = descr->calo_sign()*(thickness*type/2.0 + descr->calo_z_min());
      res = new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,offset)),
                                 descr->calo_r_min(), descr->calo_r_max() );
      return res;
    }
    else if (type==Inside||type==Outside) {
      double radius;
      if (type==Inside) {
        radius = descr->calo_r_min();
      }
      else {
        radius = descr->calo_r_max();
      }
      double halfLength = (descr->calo_z_max()-descr->calo_z_min())/2.0;
      double offset = descr->calo_sign()*(descr->calo_z_min()+halfLength);
      res = new Trk::CylinderSurface(new Amg::Transform3D(Amg::Translation3D(0.,0.,offset)),
                                     radius, halfLength);
      return res;
    }
    else {
      ATH_MSG_WARNING("Type error in CaloTrkMuIdDetStore::createSurface().");
      return 0;
    }
  }
  return 0; 
}

///////////////////////////////////////////////////////////////////////////////
// getClosestCell()
///////////////////////////////////////////////////////////////////////////////
const CaloCell* TrackDepositInCaloTool::getClosestCell(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const {
/* 
Get closest cell near the TrackParameters par. For LAr this can be done using the get_element function of the 
CaloDetDescrManager. This should be really fast since it is nothing more than a sequence of lookups. 
For the non-projective tile cells one has to select cells in a certain (eta,phi) region and then select the one
that is closest to the track.
*/

  // --- Determine cell type ---
  const CaloCell* cell = 0;
  if (descr->is_tile()) {
    cell = getClosestCellTile(par, descr);
  }
  else {
    cell = getClosestCellLAr(par, descr, m_cellContainer);
  }
  // ATH_MSG_INFO("cell = " << cell);
  return cell;
  
}

///////////////////////////////////////////////////////////////////////////////
// getClosestCellLAr()
///////////////////////////////////////////////////////////////////////////////
const CaloCell* TrackDepositInCaloTool::getClosestCellLAr(const Trk::TrackParameters* par, const CaloDetDescriptor* descr, const CaloCellContainer* caloCellCont) const {

  CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();
  // ATH_MSG_INFO("Sampling = " << sample);
  const CaloDetDescrElement* cellDescr = m_caloDDM->get_element(sample, par->position().eta(), par->position().phi());
  if (cellDescr) {
    IdentifierHash hash = cellDescr->calo_hash();
    const CaloCell* cell = caloCellCont->findCell(hash);
    if (cell) {
      // ATH_MSG_INFO("Energy = " << cell->energy());
      return cell;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// getClosestCellTile()
///////////////////////////////////////////////////////////////////////////////
const CaloCell* TrackDepositInCaloTool::getClosestCellTile(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const {
  std::map<double, const CaloCell*> neighbourMap;
  const CaloCell* result = 0;
  // --- Determine granularity ---
  double etaPar       = par->position().eta();
  double phiPar       = par->position().phi();
  double etaWidth     = 2*(descr->calo_eta_max() - descr->calo_eta_min())/descr->n_eta();
  double etaMin       = etaPar - etaWidth;
  double etaMax       = etaPar + etaWidth;
  double phiWidth     = (descr->calo_phi_max() - descr->calo_phi_min())/descr->n_phi();
  // TODO: HOW TO DEAL WITH PHI ~ PI?  
  double phiMin       = phiPar-phiWidth;
  double phiMax       = phiPar+phiWidth;
  // --- Fill vecHash ---
  CaloCell_ID::CaloSample sample = const_cast<CaloDetDescriptor*>(descr)->getSampling();

  std::vector<IdentifierHash> vecHash;
  m_caloDDM->cellsInZone(etaMin, etaMax, phiMin, phiMax, sample, vecHash);
  std::vector<IdentifierHash>::iterator it = vecHash.begin();
  
  // --- Iterate and find closest to track (around 12-15 elements in loop) --- 
  double dR2Min = 999;
  double etaCell = 999;
  double phiCell = 999;
  while (it!=vecHash.end()) {
    const CaloCell* cell = m_cellContainer->findCell(*it);
    if (cell) {
      const CaloDetDescrElement* dde = cell->caloDDE();
      if (dde) {
        etaCell = dde->eta();
        phiCell = dde->phi();
        double dEta = etaPar - etaCell;
        double dPhi = phiPar - phiCell;
        double dR2 = dEta*dEta + dPhi*dPhi;
        neighbourMap[sqrt(dR2)] = cell;
        if (dR2<dR2Min) {
          dR2Min = dR2;
          result = cell;
        }
      }
    }
    it++;
  }
  
  // --- Show deposits near this track (only if debugMode is on) ---
  if (m_debugMode&&m_showNeighbours) {
    ATH_MSG_INFO("SAMPLE = " << sample);
    std::map<double, const CaloCell*>::const_iterator mapIt = neighbourMap.begin();
    while (mapIt!=neighbourMap.end()) {
      const CaloCell* cell = mapIt->second;
      double distance = mapIt->first;
      if (cell) {
        ATH_MSG_INFO("dR2 = " << distance << ", energy = " << cell->energy());
      }
      else {
        ATH_MSG_INFO("dR2 = " << distance << ", NULL pointer!");
      }
      mapIt++;
    }
  }
  
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Book Histograms
///////////////////////////////////////////////////////////////////////////////
StatusCode TrackDepositInCaloTool::bookHistos() {

  ATH_MSG_DEBUG("Booking the ROOT Histos");
  StatusCode sc;

  m_hDepositLayer12     = new TH1F("hDepositLayer12", "hDepositLayer12", 40, 0, 4000);
  m_hDepositLayer13     = new TH1F("hDepositLayer13", "hDepositLayer13", 40, 0, 4000);
  m_hDepositLayer14     = new TH1F("hDepositLayer14", "hDepositLayer14", 40, 0, 4000);
  m_hParELossEta        = new TH2F("hParELossEta", "Parametrized eLoss vs eta", 40, 0, 4000, 30, -3, 3);
  m_hParELossSample     = new TH2F("hParELossSample", "Parametrized eLoss vs sample", 40, 0, 4000, 21, 0, 21);
  m_hDeltaEtaLastPar    = new TH1F("hDeltaEtaLastPar", "hDeltaEtaLastPar", 50, -2, 2);
  m_hDeltaRadiusLastPar = new TH1F("hDeltaRadiusLastPar", "hDeltaRadiusLastPar", 50, 0, 5000);
  m_hDepositsInCore     = new TH1F("hDepositsInCore", "hDepositsInCore", 50, 0, 5000);
  m_hDepositsInCone     = new TH1F("hDepositsInCone", "hDepositsInCone", 50, 0, 5000);
  m_hDistDepositsTile   = new TH2F("hDistDepositsTile", "hDistDepositsTile", 30, 0.0, 0.3, 30, 0, 4000);
  m_hDistDepositsHEC    = new TH2F("hDistDepositsHEC", "hDistDepositsHEC", 30, 0.0, 0.3, 30, 0, 4000);  
  
  m_hEMB1vsdPhi    = new TH2F("hEMB1vsdPhi", "hEMB1vsdPhi", 50, -3.14, 3.14, 50, 0, 500);  
  m_hEMB2vsdPhi    = new TH2F("hEMB2vsdPhi", "hEMB2vsdPhi", 50, -3.14, 3.14, 50, 0, 500);  
  m_hEMB3vsdPhi    = new TH2F("hEMB3vsdPhi", "hEMB3vsdPhi", 50, -3.14, 3.14, 50, 0, 500);  
  m_hEMB1vsdEta    = new TH2F("hEMB1vsdEta", "hEMB1vsdEta", 50, -3.14, 3.14, 50, 0, 500);  
  m_hEMB2vsdEta    = new TH2F("hEMB2vsdEta", "hEMB2vsdEta", 50, -3.14, 3.14, 50, 0, 500);  
  m_hEMB3vsdEta    = new TH2F("hEMB3vsdEta", "hEMB3vsdEta", 50, -3.14, 3.14, 50, 0, 500);  
  
  if (m_histSvc) {
    sc = m_histSvc->regHist("/AANT/CaloTrkMuId/hDepositLayer12", m_hDepositLayer12);
    sc = m_histSvc->regHist("/AANT/CaloTrkMuId/hDepositLayer13", m_hDepositLayer13);
    sc = m_histSvc->regHist("/AANT/CaloTrkMuId/hDepositLayer14", m_hDepositLayer14);
    sc = m_histSvc->regHist("/AANT/CaloTrkMuId/hParELossSample", m_hParELossSample);
    sc = m_histSvc->regHist("/AANT/CaloTrkMuId/hParELossEta", m_hParELossEta);
    sc = m_histSvc->regHist("/AANT/DetStore/hDeltaEtaLastPar", m_hDeltaEtaLastPar);
    sc = m_histSvc->regHist("/AANT/DetStore/hDeltaRadiusLastPar", m_hDeltaRadiusLastPar);
    sc = m_histSvc->regHist("/AANT/DetStore/hDepositsInCore", m_hDepositsInCore);
    sc = m_histSvc->regHist("/AANT/DetStore/hDepositsInCone", m_hDepositsInCone);
    sc = m_histSvc->regHist("/AANT/DetStore/hDistDepositsTile", m_hDistDepositsTile);
    sc = m_histSvc->regHist("/AANT/DetStore/hDistDepositsHEC", m_hDistDepositsHEC);
    
    sc = m_histSvc->regHist("/AANT/DetStore/hEMB1vsdPhi", m_hEMB1vsdPhi);
    sc = m_histSvc->regHist("/AANT/DetStore/hEMB2vsdPhi", m_hEMB2vsdPhi);
    sc = m_histSvc->regHist("/AANT/DetStore/hEMB3vsdPhi", m_hEMB3vsdPhi);    
    sc = m_histSvc->regHist("/AANT/DetStore/hEMB1vsdEta", m_hEMB1vsdEta);
    sc = m_histSvc->regHist("/AANT/DetStore/hEMB2vsdEta", m_hEMB2vsdEta);
    sc = m_histSvc->regHist("/AANT/DetStore/hEMB3vsdEta", m_hEMB3vsdEta);    
  }
  else {
    return StatusCode::FAILURE;
  }
  
  return sc;
}


/*
CaloCell_ID::get_neighbours	(	const IdentifierHash 	caloHash,
const LArNeighbours::neighbourOption & 	option,    = all2D, nextInEta, nextInPhi, prevInEta, prevInPhi
std::vector< IdentifierHash > & 	neighbourList	 
)			 const*/

///////////////////////////////////////////////////////////////////////////////
// Functions below are under development, these are not used yet.
///////////////////////////////////////////////////////////////////////////////

bool TrackDepositInCaloTool::isInsideDomain(double position, double domainCenter, double domainWidth, bool phiVariable) const {
  double halfWidth = domainWidth/2;
  if (phiVariable) {
    //double pi = 3.14159265358979312;
    if ( fabs(fabs(domainCenter)-CLHEP::pi) < domainWidth ) {
      position += CLHEP::pi;
      domainCenter += CLHEP::pi;
      if (position > CLHEP::pi) {
        position -= 2*CLHEP::pi;
      }
      if (domainCenter > CLHEP::pi){
        domainCenter -= 2*CLHEP::pi;
      }
    }
  }
  double boundLow  = domainCenter - halfWidth;
  double boundHigh = domainCenter + halfWidth;
  if (position<boundLow) 
    return false;
  if (position>boundHigh)
    return false;
  return true;
}

bool TrackDepositInCaloTool::isInsideCell(const Amg::Vector3D& position, const CaloCell* cell) const {
  const CaloDetDescrElement* dde = cell->caloDDE();
  if (!dde) 
    return false;
  if (dde->is_tile()) {
    if (!isInsideDomain(position.z(), dde->z(), dde->dz()))
      return false;
  }
  else {
    if (!isInsideDomain(position.eta(), dde->eta(), dde->eta()))
      return false;
  }
  if (!isInsideDomain(position.phi(), dde->phi(), dde->dphi(), true)) {
    return false;
  }
  // if (!isInsideDomain(position.r(), dde->r(), dde->dr())) 
  //   return false;
  return true;
}



// inCell
// an alternative version of crossedPhi from CaloCellHelpers, which has a bug

bool TrackDepositInCaloTool::inCell(const CaloCell* cell, const Amg::Vector3D& pos) const {
  bool result = fabs(CaloPhiRange::diff(pos.phi(),cell->phi())) < cell->caloDDE()->dphi()/2;
  if (cell->caloDDE()->getSubCalo() != CaloCell_ID::TILE)
     result &= fabs(pos.eta()-cell->eta()) < cell->caloDDE()->deta()/2 ;
  else if (cell->caloDDE()->getSampling() != CaloCell_ID::TileBar1)  // still need to deal with tilebar1
     result &= fabs(pos.z()-cell->z()) < cell->caloDDE()->dz()/2 ;
  return result;
}


double TrackDepositInCaloTool::distance(const Amg::Vector3D& p1, const Amg::Vector3D& p2) const {
  double diff_x = p1.x() - p2.x();
  double diff_y = p1.y() - p2.y();
  double diff_z = p1.z() - p2.z();
  return sqrt(diff_x*diff_x + diff_y*diff_y + diff_z*diff_z);
}


///////////////////////////////////////////////////////////////////////////////
// DEPRECATED FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
std::vector<DepositInCalo> TrackDepositInCaloTool::deposits( const Trk::Track* track, const double deltaR, const bool inCell) const {

  ATH_MSG_WARNING("This function will be deprecated soon.");

  std::vector<DepositInCalo> deposits;
  // Take eta of the last measured hit
  const DataVector <const Trk::TrackParameters>* paramvec = track->trackParameters();
  const Trk::TrackParameters* par=0;
  if (paramvec) { 
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
    --itEnd;
    par = *itEnd;
    return  this->deposits(par, deltaR, inCell);
  }

  return deposits;
}

void TrackDepositInCaloTool::showNeighbours(const CaloCell* cell) const {

  // --- Get helper class for finding neighbours ---
  const CaloCell_ID* cellId = m_caloDDM->getCaloCell_ID();
  if (cellId) {
    // --- vector to be filled with hashes that point to the neighbours ---
    std::vector<IdentifierHash> theNeighbours;
    // --- Option all2D corresponds to faces + corners of given cell ---       
    const LArNeighbours::neighbourOption option = LArNeighbours::all2D;
    double etaCell = cell->caloDDE()->eta();
    double phiCell = cell->caloDDE()->phi();
    // --- Get neighbours ---
    IdentifierHash hash = cell->caloDDE()->calo_hash();
    cellId->get_neighbours(hash, option, theNeighbours);
    ATH_MSG_INFO("Number of neighbours: " << theNeighbours.size());
    // --- Iterate over neighbours and display distance to primary cell and energy ---
    std::vector<IdentifierHash>::iterator it = theNeighbours.begin();
    while (it!=theNeighbours.end()) {
      const CaloCell* thisCell = m_cellContainer->findCell(*it);
      if (thisCell) {
        const CaloDetDescrElement* dde = thisCell->caloDDE();
        if (dde) {
          double dEta   = dde->eta() - etaCell;
          double dPhi   = dde->phi() - phiCell;
          double energy = thisCell->energy();
          ATH_MSG_INFO("(dEta, dPhi) = (" << dEta << ", " << dPhi << "), energy = " << energy);
        }
      }
      it++;
    }
  }
  else {
    ATH_MSG_WARNING("Could not retrieve CaloCell_ID");
  }
}

