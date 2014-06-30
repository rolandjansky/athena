/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File:   TrackTools.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Track tools.
 *
 * Created in February 2013, based on TrackInCaloTools.
 */

#include "TrackTools.h"

/// DATABASE INCLUDES
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

/// TILECAL INCLUDES
// #include "TileGeoModel/TileDetectorTool.h"
// #include "TileGeoModel/TileDetectorFactory.h"
// #include "TileGeoModel/TileAtlasFactory.h"
// #include "TileGeoModel/TileTBFactory.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileCellDim.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileConditions/TileCablingService.h"

/// CALORIMETER INCLUDES
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloCellList.h"

/// CLHEP INCLUDES
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"

/// GEOMODEL INCLUDES
#include "FindVolAction.h"
#include "FindScintillatorAction.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include <cmath>

namespace KinematicUtils {

double deltaPhi(double phi1, double phi2) {
  double dPhi=std::fabs(phi1-phi2);
  if (dPhi>M_PI) dPhi=2*M_PI-dPhi;
  return dPhi;
}


double deltaR(double eta1, double eta2, double phi1, double phi2) {
  double dPhi=deltaPhi(phi1,phi2);
  double dEta=std::fabs(eta1-eta2);
  double dR=std::sqrt(std::pow(dEta,2)+std::pow(dPhi,2));
  return dR;
}



}






//=============================================
TrackTools::TrackTools(const std::string&type,
                       const std::string&name,
                       const IInterface*parent)
:AthAlgTool(type, name, parent),
m_toCalo("ExtrapolateToCaloTool"),
m_cellNoiseRMSCut(3.4),
m_cellNoiseRMSCutLoose(2.0),
m_useExtrapolation(true),
m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault"),
m_negativeCells(false),
m_calibCrossed(false){
//============================================
    
    // INTERFACING
    declareInterface<ITrackTools>(this);
    declareInterface<IEnergyDepositionTool>(this);
    
    m_eCoreDef.push_back("0.05");
    
    declareProperty ("useNegativeCells", m_negativeCells);
    declareProperty ("cellNoiseRMSCut", m_cellNoiseRMSCut);
    declareProperty ("cellNoiseRMSCutLoose", m_cellNoiseRMSCutLoose);
    declareProperty("ExtrapolateToCaloTool", m_toCalo);
    declareProperty ("useExtrapolation",m_useExtrapolation);
    declareProperty ("NoiseTool", m_noiseTool );
    declareProperty ("CaloCellContainer", m_cellContainerName="AllCalo");
    declareProperty ("CaloClusterContainer",  m_caloclusterContainerName="CaloCalTopoCluster");
    declareProperty ("EcoreDefinition", m_eCoreDef);
    declareProperty ("EcoreCalibWithPath", m_calibCrossed=false);
    
    m_currentTrackParameters = 0;
    m_eCore = 0;
} // TRACKTOOLS::TRACKTOOLS

//========================
TrackTools::~TrackTools(){
//=======================
} // TRACKTOOLS::~TRACKTOOLS

//==================================
StatusCode TrackTools::initialize(){
//=================================
    
    ATH_MSG_ERROR( "Initializing TrackTools" );
    m_noiseToolSc = m_noiseTool.retrieve();
    if(m_noiseToolSc.isFailure()){
        ATH_MSG_WARNING( "TrackTools::Unable to find CaloNoiseTool " );
    } // IF
    else{
        ATH_MSG_INFO( "TrackTools::Calo Noise Tool is selected" );
    } // ELSE
    
    CHECK( service("StoreGateSvc", m_storeGate) );
    
    // RETRIEVE TOOL SERVICE
    IToolSvc* toolsvc;
    CHECK( service("ToolSvc",toolsvc) );
    
    // TRACK EXTRAPOLATOR TOOLS
    if(toolsvc->retrieveTool("Extrapolator", m_trackExtrapolatorToCalo).isFailure()){
        ATH_MSG_FATAL( "Cannot get TrackExtrapolatorToCalo");
        return StatusCode::FAILURE;
    } // IF
    if (m_toCalo.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Can't get handle on " );
        return StatusCode::FAILURE;
    }
    
    // RETRIEVE TILE DESCRIPTOR
    if(detStore()->retrieve(m_tileDDM).isFailure()){
        ATH_MSG_FATAL( "Could not get TileDetDescrManager" );
        return StatusCode::FAILURE;
    } // IF
    
    if(m_cellNoiseRMSCut < m_cellNoiseRMSCutLoose){
        ATH_MSG_WARNING( "cellNoiseRMSCut cannot be smaller than cellNoiseRMSCutLoose" );
        ATH_MSG_INFO( "Setting cellNoiseRMSCutLoose to cellNoiseRMSCut" );
        m_cellNoiseRMSCutLoose = m_cellNoiseRMSCut;
    } // IF
    
    if(service("DetectorStore",m_detStoreSvc).isFailure()){
        ATH_MSG_FATAL("DETECTOR STORE COULD NOT BE RETRIEVED.");
        return StatusCode::FAILURE;
    } // IF
    
    // SETUP CALORIMETER DESCRIPTION MANAGERS
    m_calo_dd = CaloDetDescrManager::instance();
    m_caloCell_ID = m_calo_dd->getCaloCell_ID();
    
    m_subCalos.push_back(CaloCell_ID::LAREM);
    m_subCalos.push_back(CaloCell_ID::LARHEC);
    m_subCalos.push_back(CaloCell_ID::TILE);
    
    m_cone = 0;
    initializeListener(this, name());
    m_eCore = new ConeDefinition("Ecore", m_eCoreDef, this);
    
    return StatusCode::SUCCESS;
} // TrackTools::initialize

//================================
StatusCode TrackTools::finalize(){
//===============================
    delete m_eCore;
    return StatusCode::SUCCESS;
} // TRACKTOOLS::FINALIZE

//=========================
void TrackTools::m_clear(){
//========================
    ATH_MSG_DEBUG( "Calling m_clear" );
    m_currentTrackParameters = 0;
    m_cone = 0;
    m_cellsOnTrack.clear();
    m_cellsPerLayer.clear();
    m_extrapolations.clear();
    m_weightsForCellsCrossed.clear();
} // TRACKTOOLS::M_CLEAR

//===========================================================
bool TrackTools::newTrack(const Trk::TrackParameters* track){
//==========================================================
    if(!m_listener){
        ATH_MSG_DEBUG( "Incident Service not active: calling m_clear" );
        m_clear();
    } // IF
    if(!track)
        ATH_MSG_DEBUG( "Null pointer to TrackParameters! " );
    
    bool new_track = (!m_currentTrackParameters ||  track != m_currentTrackParameters);
    if(new_track){
        m_clear();
        m_currentTrackParameters = track;
    } // IF
    return new_track;
} // TRACKTOOLS::NEWTRACK

//====================================================================================================
StatusCode TrackTools::getExtrapolations(const Trk::TrackParameters* track, bool usePosition = false){
//===================================================================================================
    if(!track){
        ATH_MSG_DEBUG( "Null pointer to TrackParameters" );
        return StatusCode::FAILURE;
    } // IF
    
    if(usePosition){
        std::pair<double, double> position = std::make_pair(track->position().eta(), track->position().phi());
        for(int layer = ps; layer <= hec3; layer++)
            m_extrapolations.push_back(position);
    }
    else if(!m_useExtrapolation) // use track direction for all layers
    {
        std::pair<double, double> position = std::make_pair(track->eta(), track->parameters()[Trk::phi]);
        for(int layer = ps; layer <= hec3; layer++)
            m_extrapolations.push_back(position);
    }
    else
        m_extrapolations = m_trackExtrapolatorToCalo->getMidPointForExtrapolations(track);
    
    if(m_extrapolations.size() == 0)
        return StatusCode::FAILURE;
    ATH_MSG_DEBUG( "Size of extrapolations: " << m_extrapolations.size() );
    return StatusCode::SUCCESS;
}

//=============================================
int TrackTools::getLayer(const CaloCell *cell){
//============================================
    if(!cell || !cell->caloDDE()){
        ATH_MSG_WARNING( "INVALID CELL" );
        return -1;
    } // IF
    return getLayer(cell->caloDDE()->getSampling() );
} // TrackTools::getLayer

//=====================================================
int TrackTools::getLayer(CaloCell_ID::CaloSample calo){
//====================================================
    if(calo==CaloCell_ID::PreSamplerB || calo==CaloCell_ID::PreSamplerE)
        return ps;
    else if(calo==CaloCell_ID::EMB1 || calo==CaloCell_ID::EME1)
        return em1;
    else if(calo==CaloCell_ID::EMB2 || calo==CaloCell_ID::EME2)
        return em2;
    else if(calo==CaloCell_ID::EMB3 || calo==CaloCell_ID::EME3)
        return em3;
    else if(calo==CaloCell_ID::TileBar0 || calo==CaloCell_ID::TileExt0)
        return tile1;
    else if(calo==CaloCell_ID::TileBar1 || calo==CaloCell_ID::TileExt1 || calo==CaloCell_ID::TileGap1 || calo==CaloCell_ID::TileGap3)
        return tile2;
    else if(calo==CaloCell_ID::TileBar2 || calo==CaloCell_ID::TileExt2 || calo==CaloCell_ID::TileGap2)
        return tile3;
    else if(calo==CaloCell_ID::HEC0)
        return hec0;
    else if(calo==CaloCell_ID::HEC1)
        return hec1;
    else if(calo==CaloCell_ID::HEC2)
        return hec2;
    else if(calo==CaloCell_ID::HEC3)
        return hec3;
    else{
        ATH_MSG_DEBUG( "Invalid CaloCell_ID: " << calo );
        return -1;
    } // ELSE
} // TrackTools::getLayer

// ------------------------------------------------------------------
/** Store in m_cellsPerLayer the list of cells within deltaR = m_cone.
 Also stores m_cellsOnTrack (the cell touched by the extrapolation)
 Should be called only by getEnergyAndCells **/
StatusCode TrackTools::getCellsForCone(double cone)
{
    ATH_MSG_DEBUG( "In TrackTools::getCellsForCone. Given cone size: " << cone );
    
    if(cone == 0)
    {
        ATH_MSG_WARNING( "getCellsForCone got 0 as conesize" );
        return StatusCode::FAILURE;
    }
    
    //retrieve the cell container
    const CaloCellContainer* container;
    StatusCode sc = m_storeGate->retrieve(container, m_cellContainerName);
    if(sc.isFailure() )
    {
        ATH_MSG_WARNING( "Unable to retrieve the cell container  " << m_cellContainerName );
        return StatusCode::FAILURE;
    }
    m_cone = cone;
    // Prepare for getting the cells around and on track
    m_cellsPerLayer.clear(); // maybe this was called before for a smaller cone
    m_cellsPerLayer.resize(NLAYERS);
    m_cellsOnTrack.resize(NLAYERS);
    
    // Loop over SubCalo's (EM, Tile, HEC) and retrieve the cells within the cone
    for(unsigned int i = 0; i < m_subCalos.size(); i++)
    {
        CaloCell_ID::SUBCALO subCalo = m_subCalos[i];
        
        // Cell preselection is done mid-point between extrapolations on subcalo and the maximum distance as tolerance
        std::vector<double> v = getParametersForCellSelection(subCalo);
        ATH_MSG_DEBUG( "SubCalo: " << i << " eta: " << v[0] << " phi: " << v[1] << " cone: " << v[2] );
        CaloCellList *cellList = new CaloCellList(container,subCalo);
        cellList->select(v[0], v[1], m_cone + v[2] + 0.02);
        ATH_MSG_DEBUG( "SubCalo: " << i << " ncells: " << cellList->ncells() );
        
        // Loop over cells, fill m_cellsOnTrack and m_cellsPerLayer
        for(CaloCellList::list_iterator icell = cellList->begin(); icell != cellList->end(); icell++)
        {
            const CaloCell *cell = (*icell);
            int layer = (int) getLayer(cell);
            if(layer < 0) // invalid layer
                continue;
            
            // Is it above the noise threshold? (true is for loose cut --> negative cells also)
            if(!accept(cell, true) )
                continue;
            m_cellsPerLayer[layer].push_back(cell);
            // Is it the cell on track ?
            if(isCellOnTrack(cell, m_extrapolations[layer].first, m_extrapolations[layer].second) )
                m_cellsOnTrack[layer] = cell;
        } // end cell loop
        
        delete cellList;
    } // end subCalo loop
    
    for(unsigned int i=0; i < m_cellsPerLayer.size(); i++)
        ATH_MSG_DEBUG( "Cells on layer " << i << ": " << m_cellsPerLayer[i].size() );
    
    return StatusCode::SUCCESS;
}



//==============================================================================================================
bool TrackTools::isClusterAroundTrack(double cone, const CaloCluster* cluster, const Rec::TrackParticle* track){
//==============================================================================================================
    const CaloCluster* clusterPointer = cluster;
    if(!clusterPointer) ATH_MSG_ERROR("NULL CLUSTERPOINTER");
    
    const Rec::TrackParticle* trackPointer = track;
    if(!trackPointer) ATH_MSG_ERROR("NULL TRACKPOINTER");
    
    double deltaR = 0;
    double dif_eta = 0;
    double dif_phi = 0;

    for(int i=0;i<11;i++){
        dif_eta = clusterPointer->eta() - getXYZEtaPhiPerLayer(trackPointer)[i][3];
        dif_phi = phidiff(clusterPointer->phi(),getXYZEtaPhiPerLayer(trackPointer)[i][4]);
        double tmp = sqrt(dif_eta*dif_eta+dif_phi*dif_phi);
        deltaR = tmp<deltaR ? tmp : deltaR;
    } // FOR
    
    if(deltaR <= cone) return true;
    return false;
} // isClusterAroundTrack

//===============================================================================================================
std::vector<const CaloCluster*> TrackTools::getClustersAroundTrack(double cone, const Rec::TrackParticle* track){
//===============================================================================================================

    StatusCode sc = m_storeGate->retrieve(m_caloclusterContainer, m_caloclusterContainerName);
    if(sc.isFailure()) ATH_MSG_ERROR("NO ASSOCIATED CALOCLUSTERS RETRIEVED");
    
    const Rec::TrackParticle* trackPointer = track;
    if(!trackPointer) ATH_MSG_ERROR("NULL TRACKPOINTER");

    std::vector<const CaloCluster*> output;
    
    if(m_caloclusterContainer != NULL){
        CaloClusterContainer::const_iterator clusterItr = m_caloclusterContainer->begin();
        CaloClusterContainer::const_iterator clusterEnd = m_caloclusterContainer->end();
        while(clusterItr != clusterEnd){
            const CaloCluster* cluster = *clusterItr;
            if (isClusterAroundTrack(cone, cluster, trackPointer)){
                output.push_back(cluster);
            } // IF
            ++clusterItr;
        } // WHILE
    } // IF
    
    //ATH_MSG_ERROR("output Size:" << output.size());
    return output;
} // getClustersAroundTrack


//==========================================================================================
StatusCode TrackTools::getEnergyAndCells(const Trk::TrackParameters* track,
                                         std::vector<double> conesize,
                                         std::vector< std::vector<double> > &etot,
                                         std::vector< std::vector<ListOfCells> > &usedCells,
                                         bool useLooseCut, bool isEt){
//==========================================================================================
    
    ATH_MSG_DEBUG( "In TrackTools::getEnergyAndCells" );
    
    unsigned int ncones = conesize.size();
    if(ncones == 0){
        ATH_MSG_WARNING( "getEnergyAndCells: Cone vector is empty, give up  ");
        return StatusCode::FAILURE;
    } // IF
    
    // Sort the conesize vector and get the cells for the largest one
    std::vector<double> sorted_cones = conesize;
    std::sort(sorted_cones.begin(), sorted_cones.end());
    ATH_MSG_DEBUG( "Greatest cone: " << sorted_cones[ncones-1] );
    ATH_MSG_DEBUG( "Current cone: " << m_cone );
    if(sorted_cones[ncones-1] <= 0)
        return StatusCode::FAILURE;
    
    if(newTrack(track) || sorted_cones[ncones-1] > m_cone){
        if(getExtrapolations(track) == StatusCode::FAILURE ||
           getCellsForCone(sorted_cones[ncones-1]) == StatusCode::FAILURE){
            ATH_MSG_DEBUG( " TrackTools: Could not get cells for given cone " );
            return StatusCode::FAILURE;
        } // IF
        ATH_MSG_DEBUG( "Got cells for cone" );
    } // IF
    
    etot.clear();
    usedCells.clear();
    etot.resize(ncones);
    usedCells.resize(ncones);
    for(unsigned int i = 0 ;i < ncones; i++){
        usedCells[i].resize(NLAYERS);
        etot[i].resize(NLAYERS);
    } // FOR
    
    // Iterate backwards over the (sorted) cones removing the cells that are beyond the cone size
    // Gather total energies and cells for each cone
    std::vector<ListOfCells> *allCells = new std::vector<ListOfCells>(m_cellsPerLayer);
    if(useLooseCut == false){
        for(std::vector<ListOfCells>::iterator it = allCells->begin(); it != allCells->end(); it++){
            selectCells(*it, useLooseCut);
            ATH_MSG_VERBOSE( "Size: " << (*it).size() );
        } // FOR
    } // IF
    std::vector<ListOfCells> *lastCells = allCells;
    
    for(int icone = ncones -1; icone >= 0; icone--){
        // Find the corresponding element in the vector conesize (unsorted)
        int jcone = 0;
        for(; jcone < (int) ncones && conesize[jcone] != sorted_cones[icone] ; jcone++){};
        
        for(int ilayer = 0; ilayer < NLAYERS; ilayer++){
            for(ListOfCells::iterator it = (*lastCells)[ilayer].begin(); it != (*lastCells)[ilayer].end(); it++ ){
                const CaloCell *cell = *it;
                if(P4Helpers::deltaR( *cell, m_extrapolations[ilayer].first, m_extrapolations[ilayer].second ) < conesize[jcone] ){
                    usedCells[jcone][ilayer].push_back(cell);
                    double e = (isEt) ? cell->et() : cell->energy();
                    etot[jcone][ilayer] += e;
                } // IF
            } // FOR
        } // FOR
        lastCells = &usedCells[jcone];
    } // FOR
    delete allCells;
    ATH_MSG_DEBUG( "end of getEnergyAndCells" );
    return StatusCode::SUCCESS;
} // getEnergyAndCells

//===================================================================
bool TrackTools::getMeasuredEnergy(const Trk::TrackParameters* track,
                                   const std::vector<double>& dR,
                                   std::vector<double>& energies,
                                   CaloLayer first_layer,
                                   CaloLayer last_layer,
                                   bool useLooseCut, bool isEt){
//===================================================================
    
    ATH_MSG_DEBUG( "In TrackTools::getMeasuredEnergy (vector)" );
    if(last_layer < first_layer || first_layer < ps || last_layer > hec3){
        ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
        return false;
    } // IF
    
    // inputs
    std::vector< std::vector<double> > etot;
    std::vector< std::vector< std::vector<const CaloCell*> > > usedCells;//only dummy here
    
    StatusCode sc = getEnergyAndCells(track, dR, etot, usedCells, useLooseCut, isEt);
    if(sc.isFailure() ){
        ATH_MSG_DEBUG( "Calculation of isolation energy failed" );
        return false;
    } // IF
    energies.clear();
    for(unsigned int cone=0;cone<dR.size();cone++){
        double totalEnergy=0.;
        for(int layer = first_layer; layer <= last_layer; layer++){
            totalEnergy += etot[cone][layer];
        } // FOR
        energies.push_back(totalEnergy);
    } // FOR
    return true;
} // getMeasuredEnergy

//=====================================================================
double TrackTools::getMeasuredEnergy(const Trk::TrackParameters* track,
                                     const double dR,
                                     CaloLayer first_layer,
                                     CaloLayer last_layer,
                                     bool useLooseCut, bool isEt){
//=====================================================================
    
    std::vector<double> conesize;
    conesize.push_back(dR);
    std::vector<double> energies;
    if(!getMeasuredEnergy(track, conesize, energies, first_layer, last_layer, useLooseCut, isEt) || energies.size() == 0)
        return 0;
    return energies[0];
} // getMeasuredEnergy

//============================================================================================
/** Return a vector with the cells inside the given definition of Ecore or Eloss set by the user **/
std::vector<const CaloCell*> TrackTools::getCellsForConeDef(const Trk::TrackParameters* track,
                                                            CaloLayer first_layer,
                                                            CaloLayer last_layer,
                                                            bool useLooseCut,
                                                            ConeDefinition *coneDef,
                                                            bool usePosition=false){
//============================================================================================
    
    ATH_MSG_DEBUG( "In TrackTools::getCellsForconeDef" );
    std::vector<const CaloCell*> cells_coneDef;
    if(last_layer < first_layer || first_layer < ps || last_layer > hec3){
        ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
        return cells_coneDef;
    } // IF
    
    // Collect the cells for the largest cone to speed-up
    double max_cone = getLargestConeNeeded(coneDef);
    ATH_MSG_DEBUG( "Cone to contain " << coneDef->name << ": " << max_cone );
    if(newTrack(track) || max_cone > m_cone){
        if(getExtrapolations(track, usePosition) == StatusCode::FAILURE ||
           getCellsForCone(max_cone) == StatusCode::FAILURE){
            ATH_MSG_DEBUG( "Could not get cells, returning empty vector for " << coneDef->name );
            return cells_coneDef;
        } // IF
    } // IF
    
    for(int ilayer = first_layer; ilayer <= last_layer; ilayer++){
        ATH_MSG_DEBUG( "Getting cells for " << coneDef->name << " on layer " << ilayer );
        CaloLayer clayer = (CaloLayer) ilayer;
        if(coneDef->m_Param.count(ilayer) ) // user wants parametrized eloss, no cell to include
            continue;
        if(coneDef->m_Crossed.count(ilayer) ){
            std::vector<const CaloCell*> cellsCrossed = getCellsCrossedByTrack(track, clayer, clayer, useLooseCut);
            cells_coneDef.insert(cells_coneDef.end(), cellsCrossed.begin(), cellsCrossed.end());
        } // IF
        else if(coneDef->m_Cone.count(ilayer) ){
            std::vector< std::vector<const CaloCell*> > cells = getCellsAroundTrack(track, coneDef->m_Cone[ilayer], clayer, clayer, useLooseCut);
            if(cells.size() )
                cells_coneDef.insert(cells_coneDef.end(), cells[0].begin(), cells[0].end());
        } // ELSE IF
        else if(coneDef->m_NxM.count(ilayer) ){
            std::vector<const CaloCell*> cells1 = getCellsAroundTrackOnLayer(track, clayer, coneDef->m_NxM[ilayer].first, coneDef->m_NxM[ilayer].second, 0, 0, useLooseCut);
            if(cells1.size())
                cells_coneDef.insert(cells_coneDef.end(), cells1.begin(), cells1.end());
        } // ELSE IF
        else if(coneDef->m_ElCone.count(ilayer) ){
            std::vector< std::vector<const CaloCell*> > cells2 = getCellsAroundTrack(track, coneDef->m_ElCone[ilayer].first, coneDef->m_ElCone[ilayer].second, clayer, clayer, useLooseCut);
            if(cells2.size())
                cells_coneDef.insert(cells_coneDef.end(), cells2[0].begin(), cells2[0].end());
        } // ELSE IF
        else // should never happen
            ATH_MSG_WARNING( "Wrong definition of " << coneDef->name << " for layer " << ilayer );
        ATH_MSG_DEBUG( "Cells on " << coneDef->name << ": " << cells_coneDef.size() );
    } // FOR
    return cells_coneDef;
} // TrackTools::getCellsForConeDef

//========================================================================================================
std::vector< std::pair<double, double> > TrackTools::getEtaPhiPerLayer(const Trk::TrackParameters* track){
//=======================================================================================================
    return m_trackExtrapolatorToCalo->getMidPointForExtrapolations(track);
} // TrackTools::getEtaPhiPerLayer

//=====================================================================================================
std::vector< std::vector<double> > TrackTools::getXYZEtaPhiPerLayer(const Trk::TrackParameters* track){
//====================================================================================================
    return m_trackExtrapolatorToCalo->getTrackPositions(track);
} // TrackTools::getEtaPhiPerLayer

//====================================================================================================
double TrackTools::getEcore(const Trk::TrackParameters* track,
                            CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut, bool isEt){
//===================================================================================================
    // GET ENERGY DEPOSITED BY MUON TRACK
    ATH_MSG_DEBUG( "In TrackTools::getEcore" );
    double total_energy = 0;
    if(m_eCore->m_Param.size() != NLAYERS){
        std::vector<const CaloCell*> cells = getCellsForEcore(track, first_layer, last_layer, useLooseCut);
        ATH_MSG_DEBUG( "Cells on eCore: " << cells.size() );
        for(std::vector<const CaloCell*>::iterator it = cells.begin(); it != cells.end(); it++){
            unsigned int layer = getLayer(*it);
            double calib = 1.;
            // If requested and using cells crossed, calibrate eCore according to L(cells_crossed)/L(layer), assuming dE/dx is roughly constant
            if(m_calibCrossed && m_eCore->m_Crossed.count(layer) && m_weightsForCellsCrossed[layer] != 0)
                calib = 1./m_weightsForCellsCrossed[layer];
            total_energy += (isEt ? (*it)->et() : (*it)->e() ) * calib;
        } // FOR
    } // IF
    
    // Add the parametrized eloss if requested. Change from e to et using eta from the track if needed
    if(m_eCore->m_Param.size() ){
        if(!m_useExtrapolation)
            ATH_MSG_WARNING( "Not using extrapolation, cannot get parametrized eloss" );
        else{
            std::vector<double> param_eloss = m_trackExtrapolatorToCalo->getParametrizedEloss(track);
            for(std::map<int, bool>::iterator im = m_eCore->m_Param.begin(); im != m_eCore->m_Param.end(); im++)
                //         if(im->first >= first_layer && im->first <= last_layer )
                total_energy += param_eloss[im->first] * (isEt ? cosh(track->eta()) : 1);
        } // ELSE
    } // IF
    return total_energy;
} // getEcore

//=======================================================================================================
std::vector<const CaloCell*> TrackTools::getAssociatedCells(const Trk::Track* track,
                                                            CaloLayer first_layer, CaloLayer last_layer){
//=======================================================================================================
    const CaloCellContainer* container;
    StatusCode sc = m_storeGate->retrieve(container, m_cellContainerName);
    if(sc.isFailure()) ATH_MSG_ERROR("NO ASSOCIATED CALOCELLS RETRIEVED");
    CaloCellContainer::const_iterator f_cell = container->begin();
    CaloCellContainer::const_iterator l_cell = container->end();
    std::vector<const CaloCell*> output;
    for( ; f_cell != l_cell; ++f_cell ){
        const CaloCell* cell = ( *f_cell );
        if(getLayer(cell)<first_layer || getLayer(cell)>last_layer) continue;
        if(getCBNTPathInsideCell(track,cell)>0.) output.push_back(cell);
    } // FOR
    return output;
} // TrackTools::getAssociatedCells


//======================================================================================================================================
std::vector<std::vector<const CaloCell*> > TrackTools::getCellsAroundTrack(const Trk::TrackParameters* track,
                                                                           const double dR, CaloLayer first_layer, CaloLayer last_layer,
                                                                           bool useLooseCut){
//======================================================================================================================================
    
    ATH_MSG_DEBUG( "In TrackTools::getCellsAroundTrack" );
    
    std::vector<double> conesize;
    conesize.push_back(dR);
    std::vector< std::vector<double> > etot;
    std::vector< std::vector< std::vector<const CaloCell*> > > theCellsVector;
    std::vector<std::vector<const CaloCell*> > output;
    
    StatusCode sc = getEnergyAndCells(track, conesize, etot, theCellsVector, useLooseCut);
    if(sc.isFailure() || theCellsVector.size() == 0){
        ATH_MSG_DEBUG( "No cells around track" );
        return output;
    } // IF
    for(int layer = first_layer; layer <= last_layer; layer++)
        output.push_back(theCellsVector[0][layer]);
    return output;
} // TrackTools::getCellsAroundTrack


//======================================================================================================================================
std::vector<std::vector<const CaloCell*> > TrackTools::getCellsAroundTrack(const Trk::TrackParameters* track,
                                                                           const double deta,const double dphi, CaloLayer first_layer, CaloLayer last_layer,
                                                                           bool useLooseCut){
//=====================================================================================================================================
    // Return a vector with the cells around track for the given "rectangular cone" (deta, dphi) divided by layer
    ATH_MSG_DEBUG( "In TrackTools::getCellsAroundTrack (rectangle)" );
    
    std::vector<double> conesize;
    conesize.push_back(sqrt(deta*deta + dphi*dphi));
    std::vector< std::vector<double> > etot;
    std::vector< std::vector< std::vector<const CaloCell*> > > theCellsVector;
    std::vector<std::vector<const CaloCell*> > output;
    
    if(deta <= 0 || dphi <= 0){
        ATH_MSG_WARNING( "Wrong values passed for deta and/or dphi" );
        return output;
    } // IF
    
    StatusCode sc = getEnergyAndCells(track, conesize, etot, theCellsVector, useLooseCut);
    if(sc.isFailure() || theCellsVector.size() == 0){
        ATH_MSG_DEBUG( "Could not get cells around track" );
        return output;
    } // IF
    output.resize(last_layer - first_layer + 1);
    for(int i=0, layer=first_layer; layer <= last_layer; layer++, i++){
        std::vector<const CaloCell*>::iterator it = theCellsVector[0][layer].begin();
        for(; it != theCellsVector[0][layer].end(); it++){
            const CaloCell *cell = *it;
            double delta_eta = (m_extrapolations[layer].first - cell->eta())/deta;
            double delta_phi = (m_extrapolations[layer].second - cell->phi())/dphi;
            if(fabs(delta_eta) < 1 && fabs(delta_phi) < 1)
                output[i].push_back(cell);
        } // FOR
    } // FOR
    ATH_MSG_DEBUG( "End of getCellsAroundTrack (elliptical cone)" );
    return output;
} // TrackTools::getCellsAroundTrack

//===================================================================================================
std::vector<const CaloCell*> TrackTools::getCellsOnTrack(const Trk::TrackParameters* track,
                                                         CaloLayer first_layer, CaloLayer last_layer,
                                                         bool useLooseCut){
//==================================================================================================
    ATH_MSG_DEBUG( "In TrackTools::getCellsOnTrack" );
    if(newTrack(track) || m_cellsOnTrack.size() == 0)
        getMeasuredEnergy(track, 0.5, ps, hec3);
    std::vector<const CaloCell*> cellsOnTrack;
    if(m_cellsOnTrack.size() ){
        cellsOnTrack.reserve(m_cellsOnTrack.size());
        for(int layer = first_layer; layer <= last_layer; layer++)
            if(m_cellsOnTrack[layer] && (useLooseCut || accept(m_cellsOnTrack[layer], useLooseCut) ) )
                cellsOnTrack.push_back(m_cellsOnTrack[layer]);
    } // IF
    return cellsOnTrack;
} // TrackTools::getCellsOnTrack

//===========================================================================
bool TrackTools::isCellOnTrack(const CaloCell *cell, double eta, double phi){
//==========================================================================
    // CHECK WHETHER EXTRAPOLATION IS INSIDE CELL
    
    return ( fabs(eta - cell->eta()) < cell->caloDDE()->deta()/2. &&
            fabs(phidiff(phi, cell->phi()) ) < cell->caloDDE()->dphi()/2.);
} // TrackTools::isCellOnTrack

//=================================================================================
bool TrackTools::isScintOnTrack(const Scintillator *scint, double eta, double phi){
//================================================================================
    // CHECK WHETHER EXTRAPOLATION IS INSIDE SCINTILLATOR
    
    return ( fabs(eta - scint->eta()) < scint->deta()/2. &&
            fabs(phidiff(phi, scint->phi()) ) < scint->dphi()/2.);
} // TrackTools::isCellOnTrack

//==============================================================
bool TrackTools::accept(const CaloCell *cell, bool useLooseCut){
//=============================================================
    // WHETHER CELL PASSED NOISE THRESHOLD
    double cellEn_temp = (m_negativeCells || useLooseCut) ? fabs(cell->energy()) : cell->energy();
    double cell_rms_noise = (m_noiseToolSc.isSuccess()) ? m_noiseTool->totalNoiseRMS(cell) : 0;
    if(useLooseCut)
        return (cellEn_temp > cell_rms_noise*m_cellNoiseRMSCutLoose);
    return (cellEn_temp > cell_rms_noise*m_cellNoiseRMSCut);
} // TrackTools::accept

namespace {
    struct RejectCell
    {
        RejectCell (TrackTools* tt, bool useloose) : m_tt(tt), m_useloose(useloose) {}
        bool operator() (const CaloCell* cell) { return !m_tt->accept(cell, m_useloose); }
        TrackTools* m_tt;
        bool m_useloose;
    };
}
//==================================================================================
void TrackTools::selectCells(std::vector<const CaloCell*> &cells, bool useLooseCut){
//==================================================================================
    std::vector<const CaloCell*>::iterator it =
    std::remove_if (cells.begin(), cells.end(), RejectCell (this, useLooseCut));
    cells.erase (it, cells.end());
} // TrackTools::selectCells

//================================================================================
/** Return a vector with all the cells crossed by the track (can be more than one per layer) **/
std::vector<const CaloCell*> TrackTools::getCellsCrossedByTrack(const Trk::TrackParameters* track,
                                                                CaloLayer first_layer,
                                                                CaloLayer last_layer,
                                                                bool useLooseCut){
//================================================================================
    
    ATH_MSG_DEBUG( "In TrackTools::getCellsCrossedByTrack" );
    std::vector<const CaloCell*> cellsCrossed;
    m_weightsForCellsCrossed = std::vector<double>(NLAYERS, 0.);
    if(!m_useExtrapolation)
    {
        ATH_MSG_WARNING( "Extrapolation not requested, cannot get cells crossed" );
        return cellsCrossed;
    }
    std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
    std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
    if(v_entrance.size() == 0 || v_exit.size() == 0)
        return cellsCrossed;
    
    // Define the largest cone that contains all crossed cells
    double max_cone = 0.5;
    for(int layer = first_layer; layer <= last_layer; layer++)
        if(v_entrance[layer] && v_exit[layer])
            max_cone = std::max(max_cone, 0.5*deltaR(v_entrance[layer], v_exit[layer]) );
    ATH_MSG_DEBUG( "Cone to contain cells crossed: " << max_cone );
    
    if(newTrack(track) || max_cone > m_cone)
        if(getExtrapolations(track) == StatusCode::FAILURE ||
           getCellsForCone(max_cone) == StatusCode::FAILURE)
        {
            ATH_MSG_DEBUG( " TrackTools: Could not get cells for given cone " );
            return cellsCrossed;
        }
    
    for(int layer = first_layer; layer <= last_layer; layer++)
    {
        unsigned int ncellsCrossed = cellsCrossed.size();
        if(!v_entrance[layer] || !v_exit[layer])
        {
            ATH_MSG_DEBUG( "No extrapolation for entrance or exit on layer " << layer );
            if(m_cellsOnTrack[layer] && accept(m_cellsOnTrack[layer], useLooseCut) )
                cellsCrossed.push_back(m_cellsOnTrack[layer]);
            else
                ATH_MSG_DEBUG( "No cell on track either" );
            continue;
        }
        
        std::vector<const CaloCell*>::iterator it = m_cellsPerLayer[layer].begin();
        for(; it != m_cellsPerLayer[layer].end(); it++)
        {
            const CaloCell *cell = *it;
            if(accept(cell, useLooseCut) && cellCrossedByTrack(cell, v_entrance[layer], v_exit[layer]) )
            {
                cellsCrossed.push_back(cell);
                ATH_MSG_DEBUG("Weight for cells crossed: " << m_weightsForCellsCrossed[layer]);
            }
        }
        
        ATH_MSG_DEBUG( "Track crossed " <<  cellsCrossed.size() - ncellsCrossed << " cells in layer " << layer );
        // For 2x2 (or more) the track could not cross all cells, but this should not happen often so I will ignore
        // The right way to detect that would be to check how many cells we have in eta X phi but this is too tedious
        if(cellsCrossed.size() - ncellsCrossed > 3)
            ATH_MSG_DEBUG( "getCellsCrossed might return more than the actual crossed cells" );
        ATH_MSG_DEBUG("Total weight for cells crossed: " << m_weightsForCellsCrossed[layer]);
        if(m_weightsForCellsCrossed[layer] > 1)
        {
            m_weightsForCellsCrossed[layer] = 1.;
            ATH_MSG_DEBUG("Weight reset to 1");
        }
    } // end of loop over layers
    return cellsCrossed;
}
//------------------------------------------------------------------
/** Return the fraction of the path crossed by the track which is covered by the cells collected **/
std::vector<double> TrackTools::getWeightsForCellsCrossed(const Trk::TrackParameters* track, bool useLooseCut)
{
    if(newTrack(track) || m_weightsForCellsCrossed.size() == 0)
        getCellsCrossedByTrack(track, ps, hec3, useLooseCut);
    return m_weightsForCellsCrossed;
}
//------------------------------------------------------------------
/** Return the cells around the track for the given cone, subtracting the cells used for eCore calculation. **/
std::vector< std::vector<const CaloCell*> > TrackTools::getCellsForIsolation(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
    ATH_MSG_DEBUG( "In TrackTools::getCellsForIsolation" );
    if(last_layer < first_layer || first_layer < ps || last_layer > hec3)
    {
        ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
        return std::vector< std::vector<const CaloCell* > >();
    }
    std::vector< std::vector<const CaloCell* > > cells = getCellsAroundTrack(track, dR, first_layer, last_layer, useLooseCut);
    if(cells.size() == 0)
    {
        ATH_MSG_DEBUG( "Returning 0 as isolation energy" );
        return cells;
    }
    // Using only parametrization for eCore?
    if(m_eCore->m_Param.count(true) == NLAYERS)
        return cells;
    
    std::vector<const CaloCell*> core_cells = getCellsForEcore(track, first_layer, last_layer, useLooseCut);
    if(core_cells.size() == 0)
        return cells;
    
    removeCoreFromCone(cells, core_cells, first_layer, last_layer);
    return cells;
}
//------------------------------------------------------------------
/** Return the isolation energy (Et) for the given cone, subtracting the cells used for eCore calculation. If the parametrization is requested in eCore, subtract the energy if there are cells on the layer. **/
double TrackTools::getIsolationEnergy(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
    ATH_MSG_DEBUG( "In TrackTools::getIsolationEnergy" );
    if(last_layer < first_layer || first_layer < ps || last_layer > hec3)
    {
        ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
        return 0;
    }
    // Add the parametrized eloss if requested. Change from e to et using eta from the track if needed
    std::vector<double> param_eloss;
    if(m_eCore->m_Param.size() )
    {
        if(!m_useExtrapolation)
            ATH_MSG_WARNING( "Not using extrapolation, cannot get parametrized eloss" );
        else
            param_eloss = m_trackExtrapolatorToCalo->getParametrizedEloss(track);
    }
    
    double isolationEnergy = 0.;
    std::vector< std::vector<const CaloCell*> > isolation_cells = getCellsForIsolation(track, dR, first_layer, last_layer, useLooseCut);
    if(isolation_cells.size() == 0)
        return isolationEnergy;
    for(unsigned int layer = (unsigned int) first_layer, ilayer = 0; layer <= (unsigned int) last_layer; layer++, ilayer++){
        if(isolation_cells[ilayer].size() == 0)
            continue;
        for(unsigned int i=0; i < isolation_cells[ilayer].size(); i++)
            isolationEnergy += isolation_cells[ilayer][i]->et();
        if(m_eCore->m_Param.count(layer) && param_eloss.size())
            isolationEnergy -= param_eloss[layer] * cosh(track->eta());
    }
    return isolationEnergy;
}
//-----------------------------------------------------------
/** Return a vector with the neta X phi (up to 3) neighbours of the given cell (+ itself).
 If n is even there are two options:
 - if the direction is given (positive or negative), take the cells in that direction
 - if not, define the direction that maximizes sumEt
 Warning: some cells might not be in the container, so you may get less than you requested **/
std::vector<const CaloCell*> TrackTools::getNeighbours(const CaloCell *cell, int neta, int nphi, int dir_eta, int dir_phi, bool useLooseCut)
{
    std::vector<const CaloCell*> cells;
    if(!cell)
    {
        ATH_MSG_WARNING( "Null pointer to cell!" );
        return cells;
    }
    cells.push_back(cell);
    if(neta > 3 || nphi > 3)
    {
        ATH_MSG_WARNING( "Cannot make clusters bigger than 3 in each direction, giving up" );
        return cells;
    }
    if(neta != 1 || nphi != 1)
    {
        //retrieve the cell container
        const CaloCellContainer* container;
        StatusCode sc = m_storeGate->retrieve(container, m_cellContainerName);
        if(sc.isFailure() )
        {
            ATH_MSG_WARNING( "Unable to retrieve the cell container  " << m_cellContainerName );
            return cells;
        }
        
        IdentifierHash id = cell->caloDDE()->calo_hash();
        // Use m_caloCell_ID->get_neighbours to get the identifiers of the neighbours
        // Should be faster than looping over the cells directly
        std::vector<IdentifierHash> ids;
        int option;
        if(neta == 1)
            option = (LArNeighbours::prevInPhi | LArNeighbours::nextInPhi);
        else if(nphi == 1)
            option = (LArNeighbours::prevInEta | LArNeighbours::nextInEta);
        else
            option = LArNeighbours::all2D;
        // Get the identifiers of the cells and the cells
        m_caloCell_ID->get_neighbours(id, (LArNeighbours::neighbourOption) option, ids);
        container->findCellVector(ids, cells);
        // Remove the cells which do not belong to the same layer (get_neighbours can retrieve cells in TileGap3
        selectCellsByLayer(cells, getLayer(cell));
    }
    // Apply the noise threshold
    selectCells(cells, useLooseCut);
    selectNeighbours(cells, neta, nphi, dir_eta, dir_phi);
    return cells;
}


namespace {
    struct SelectNeighbor
    {
        SelectNeighbor (int neta, int nphi, int dir_eta, int dir_phi, double eta, double phi)
        : m_neta(neta), m_nphi(nphi), m_dir_eta(dir_eta), m_dir_phi(dir_phi),
        m_eta(eta), m_phi(phi) {}
        // Remove if this is true.
        bool operator() (const CaloCell* cell2)
        {
            // Distance from reference cell or extrapolation divided by cell size
            // and according to the cluster direction
            int deta = (int) round( (cell2->eta() - m_eta)/cell2->caloDDE()->deta() )*m_dir_eta;

            // FEW CONSTANTS
            double phi_min = -2.*asin(1.);
            double twopi   = 4.*asin(1.);
            double phi_max = phi_min + twopi;
            double phi1    = cell2->phi();
            double phi2    = m_phi;

            // COMPUTE PHI DIFFERENCE
            double ph1     = phi1 < phi_min ? phi1 + twopi : ( phi1 > phi_max ? phi1 - twopi : phi1 );
            double ph2     = phi2 < phi_min ? phi2 + twopi : ( phi2 > phi_max ? phi2 - twopi : phi2 );
            double res     = ph1-ph2;
            double DIFF    = res  < phi_min ? res + twopi  : ( res  > phi_max ?  res - twopi : res );

            int dphi = (int) round( DIFF/cell2->caloDDE()->dphi() )*m_dir_phi;
            // if (cell is outside neta, nphi) or (2 were requested and this
            // is the "negative one") -> remove it
            // Actually this should work for n > 3
            return (abs(deta) > m_neta/2 || (m_neta%2 == 0 && -deta >= m_neta/2 ) ||
                    abs(dphi) > m_nphi/2 || (m_nphi%2 == 0 && -dphi >= m_nphi/2 ) );
        }
        int m_neta;
        int m_nphi;
        int m_dir_eta;
        int m_dir_phi;
        double m_eta;
        double m_phi;
    };
}
//-----------------------------------------------------------
/** For getNeighbours: define the directions if not given and remove cells outside neta, nphi **/
void TrackTools::selectNeighbours(std::vector<const CaloCell*> &cells, int neta, int nphi, int dir_eta, int dir_phi, double eta, double phi)
{
    ATH_MSG_DEBUG( "In selectNeighbours" );
    if(!cells.size())
        return;
    std::vector<const CaloCell*>::iterator it = cells.begin();
    // If eta, phi are not given by the extrapolation, get from the first cell (the one on the center)
    if(eta < -5)
    {
        const CaloCell *cell = *it;
        eta = cell->eta();
        phi = cell->phi();
        it++;
    }
    
    // If the directions are not given, set them to where sumEt is maximal (has to call in any case so they are not 0)
    getDirections(cells, neta, nphi, &dir_eta, &dir_phi, eta, phi);
    ATH_MSG_DEBUG( "Direction for cluster: " << dir_eta << " " << dir_phi );
    
    it = std::remove_if (it, cells.end(), SelectNeighbor (neta, nphi, dir_eta, dir_phi, eta, phi));
    cells.erase (it, cells.end());
    ATH_MSG_DEBUG( "selectNeighbours successful" );
}
//-----------------------------------------------------------
int TrackTools::sign(double x, double min)
{
    if(fabs(x) < min)
        return 0;
    else if(x>0)
        return 1;
    return -1;
}
//-----------------------------------------------------------
/*int TrackTools::round(double x)
 {
 return int(x > 0.0 ? x + 0.5 : x - 0.5);
 }*/
//-----------------------------------------------------------
/** For an even number of cells, define the directions to select the cells (if not given),
 according to where sumEt is maximal **/
void TrackTools::getDirections(std::vector<const CaloCell*> cells, int neta, int nphi, int *dir_eta, int *dir_phi, double eta, double phi)
{
    double sumEta = 0;
    double sumPhi = 0;
    if((neta == 2 && !(*dir_eta) ) || (nphi == 2 && !(*dir_phi) ) )
    {
        // loop over cells, sum sign(cell.eta - other_cell.eta)*other_cell.et (same for phi)
        // If result is positive, cluster direction is positive
        // Avoids cells in the same eta, phi (distance < 0.5) or further than neta, nphi
        std::vector<const CaloCell*>::iterator it = cells.begin();
        // If eta, phi are not given by the extrapolation, get from the first cell (the one on the center)
        if(eta < -5)
        {
            const CaloCell *cell = cells.front();
            eta = cell->eta();
            phi = cell->phi();
            it++;
        }
        for(; it != cells.end(); it++) // start from 2nd cell
        {
            const CaloCell *cell2 = *it;
            // Distance from eta, phi in number of cells
            int deta = (int) round( (cell2->eta() - eta)/cell2->caloDDE()->deta() );
            int dphi = (int) round(phidiff(cell2->phi(), phi)/cell2->caloDDE()->dphi() );
            if(deta != 0 && abs(deta) <= neta/2 )
                sumEta += sign( deta )*cell2->et();
            if(dphi != 0 && abs(dphi) <= nphi/2 )
                sumPhi += sign( dphi )*cell2->et();
        }
    }
    if(sumEta == 0) // Set the cluster direction to 1
        sumEta = 1.0;
    if(sumPhi == 0) // Set the cluster direction to 1
        sumPhi = 1.0;
    // Set the directions
    if(! (*dir_eta) )
        *dir_eta = sign(sumEta);
    if(! (*dir_phi) )
        *dir_phi = sign(sumPhi);
}
// ------------------------------------------------------------------
/** For IEnergyDepositionTool: given CaloSample, track parameters and a cone, return the energy and its error (rms noise) **/
std::pair<double,double> TrackTools::measurement(CaloCell_ID::CaloSample sample, const Trk::TrackParameters& track)
{
    
    int layer = getLayer(sample);
    if(layer < 0)
    {
        ATH_MSG_WARNING( "Invalid CaloSample" << sample );
        return std::make_pair(0., 0.);
    }
    std::vector<const CaloCell*> cells = getCellsForEcore(&track, (CaloLayer) layer, (CaloLayer) layer, false, true);
    selectCellsBySample(cells, sample);
    ATH_MSG_DEBUG( "Cells selected: " << cells.size() );
    double energy = 0, noise = 0;
    
    for(std::vector<const CaloCell*>::iterator it = cells.begin(); it != cells.end(); it++)
    {
        const CaloCell *cell = *it;
        double cell_rms_noise = (m_noiseToolSc.isSuccess()) ? m_noiseTool->totalNoiseRMS(cell) : 0;
        noise += cell_rms_noise*cell_rms_noise;
        energy += cell->energy();
    }
    return std::make_pair(energy, sqrt(noise) );
}

//====================================================================================
std::vector<const CaloCell*> TrackTools::getCellsAroundTrackOnLayer(const Trk::TrackParameters *track,
                                                                    CaloLayer layer,
                                                                    int neta, int nphi, int dir_eta, int dir_phi,
                                                                    bool useLooseCut){
//===================================================================================
    
    ATH_MSG_DEBUG("In getCellsAroundTrackOnLayer. Layer " << (int) layer << " cells: " << neta << "x" << nphi );
    if(layer < ps || layer > hec3)
        ATH_MSG_WARNING("Invalid layer: " << layer );
    else if(neta == 0 && nphi == 0)
        ATH_MSG_WARNING("No cell requested" );
    else{
        if(newTrack(track) || m_cellsOnTrack.size() == 0)
            getCellsOnTrack(track);
        if(m_cellsOnTrack.size() == 0)
            ATH_MSG_DEBUG("Could not retrieve cells on track, returning empty vector in getCellsAroundTrack" );
        else if(m_cellsOnTrack[layer])
            return getNeighbours(m_cellsOnTrack[layer], neta, nphi, dir_eta, dir_phi, useLooseCut);
        else{
            ATH_MSG_DEBUG("No cell on track for layer " << layer );
            if(m_cone < CONE_NxM)
                getMeasuredEnergy(track, CONE_NxM, ps, hec3);
            if(m_cellsPerLayer[layer].size() == 0)
                ATH_MSG_DEBUG("No cells on layer " << layer
                              << ", returning empty vector in getCellsAroundTrackOnLayer" );
            else{
                std::vector<const CaloCell*> cells = m_cellsPerLayer[layer];
                ATH_MSG_DEBUG("Cells on layer: " << cells.size() );
                if(!useLooseCut)
                    selectCells(cells, useLooseCut);
                if(cells.size() )
                    selectNeighbours(cells, neta, nphi, dir_eta, dir_phi,
                                     m_extrapolations[layer].first, m_extrapolations[layer].second);
                return cells;
            } // ELSE
        } // ELSE
    } // ELSE
    return std::vector<const CaloCell*>();
}

//============================================================================================
double TrackTools::getPathInsideCell(const Trk::TrackParameters *track, const CaloCell *cell){
//===========================================================================================
    
    
    ATH_MSG_DEBUG("in TrackInCaloTools::getPathInsideCell" );
    int layer;
    if(!cell)
        ATH_MSG_DEBUG("NULL POINTER TO CELL!" );
    else if(!m_useExtrapolation)
        ATH_MSG_DEBUG("TRACK IS NOT EXTRAPOLATED." );
    else if((layer = getLayer(cell)) >= 0){
        std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
        std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
        if(v_entrance.size() == 0 || v_exit.size() == 0 || !v_entrance[layer] || !v_exit[layer]){
            ATH_MSG_DEBUG("CANNOT GET EXTRAPOLATION POINTS IN LAYER." );
            if(newTrack(track) && getExtrapolations(track) == StatusCode::FAILURE){
                return 0;
            } // IF
            if(isCellOnTrack(cell, m_extrapolations[layer].first, m_extrapolations[layer].second) ){
                return (cell->caloDDE()->getSampling() == 17) ? 0 : 1;
            } // IF
        } // IF
        else{
            return pathInsideCell(cell, v_entrance[layer], v_exit[layer]);
        } // ELSE
    } // ELSE IF
    return 0;
} // TrackTools::getPathInsideCell

//======================================================================================
double TrackTools::getCBNTPathInsideCell(const Trk::Track *track, const CaloCell *cell){
//======================================================================================

    ATH_MSG_DEBUG("in TrackInCaloTools::getPathInsideCell" );

    // GET CELL DESCRIPTOR AND SAMPLING
    const CaloDetDescrElement* dde = cell->caloDDE();
    if(!dde) return 0.;
    int sampling = dde->getSampling();
    int sampling_entrance = 0;
    int sampling_exit     = 0;
    switch(sampling){
        case 12: sampling_entrance = 12; sampling_exit = 14; break;
        case 13: sampling_entrance = 12; sampling_exit = 14; break;
        case 14: sampling_entrance = 12; sampling_exit = 14; break;
        case 15: sampling_entrance = 13; sampling_exit = 14; break;
        case 16: sampling_entrance = 12; sampling_exit = 13; break;
        case 17: sampling_entrance = 18; sampling_exit = 19; break;
        case 18: sampling_entrance = 18; sampling_exit = 20; break;
        case 19: sampling_entrance = 18; sampling_exit = 20; break;
        case 20: sampling_entrance = 18; sampling_exit = 20; break;
        default: return 0.;
    } // SWITCH

    // DO EXTRAPOLATION
    const Trk::TrackParameters *par1_entrance = m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_entrance),
                                                                       0.0,Trk::alongMomentum );
    const Trk::TrackParameters *par2_entrance = (!par1_entrance) ? 
                                                m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_entrance),
                                                                       0.0,Trk::oppositeMomentum ) : par1_entrance;
    const Trk::TrackParameters *par1_exit =     m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_exit),
                                                                       0.0,Trk::alongMomentum );
    const Trk::TrackParameters *par2_exit =     (!par1_exit) ? 
                                                m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_exit),
                                                                       0.0,Trk::oppositeMomentum ) : par1_exit;

    if(!par1_entrance && !par2_entrance) return 0.;
    if(!par1_exit     && !par2_exit)     return 0.;
    const Trk::TrackParameters *pars_entrance = par1_entrance ? par1_entrance : par2_entrance;
    const Trk::TrackParameters *pars_exit     = par1_exit     ? par1_exit     : par2_exit;

    return getPathFromCBNTMethod(cell, pars_entrance, pars_exit);
} // TrackTools::getCBNTPathInsideCell

//==================================================================================================
std::vector<const Trk::TrackParameters*> TrackTools::getEntrance(const Trk::TrackParameters *track){
//=================================================================================================
    std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
    return v_entrance;
} // TrackTools::getEntrance

//==============================================================================================
std::vector<const Trk::TrackParameters*> TrackTools::getExit(const Trk::TrackParameters *track){
//=============================================================================================
    std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
    return v_exit;
} // TrackTools::getExit

//======================================================================================================
const Trk::TrackParameters* TrackTools::getEntrance(const Trk::TrackParameters *track, int layer){
//=====================================================================================================
    const Trk::TrackParameters* entrance = NULL;
    std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
    if(v_entrance.size() != 0 && v_entrance[layer])
        entrance = v_entrance[layer];
    return entrance;
} // TrackTools::getEntrance

//==================================================================================================
const Trk::TrackParameters* TrackTools::getExit(const Trk::TrackParameters *track, int layer){
//=================================================================================================
    const Trk::TrackParameters* exit = NULL;
    std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
    if(v_exit.size() != 0 && v_exit[layer])
        exit = v_exit[layer];
    return exit;
} // TrackTools::getExit

//==========================================================================================
double TrackTools::getEntrance(const Trk::TrackParameters *track, int layer, int pos){
//=========================================================================================
    std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
    if(v_entrance.size() != 0 && v_entrance[layer])
        switch(pos){
            case 0: return v_entrance[layer]->position().x();
            case 1: return v_entrance[layer]->position().y();
            case 2: return v_entrance[layer]->position().z();
            case 3: return v_entrance[layer]->position().eta();
            case 4: return v_entrance[layer]->position().phi();
            default: return 0.;
        } // SWITCH
    return 0.;
} // TrackTools::getEntrance

//======================================================================================
double TrackTools::getExit(const Trk::TrackParameters *track, int layer, int pos){
//=====================================================================================
    std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
    if(v_exit.size() != 0 && v_exit[layer])
        switch(pos){
            case 0: return v_exit[layer]->position().x();
            case 1: return v_exit[layer]->position().y();
            case 2: return v_exit[layer]->position().z();
            case 3: return v_exit[layer]->position().eta();
            case 4: return v_exit[layer]->position().phi();
            default: return 0.;
        } // SWITCH
    return 0.;
} // TrackTools::getExit

// //======================================================================================================
// double TrackTools::getCBNTActivePathInsideCell(const Trk::TrackParameters *track, const CaloCell *cell){
// //=====================================================================================================
//     
//     ATH_MSG_DEBUG("in TrackInCaloTools::getPathInsideCell" );
//     int layer;
//     if(!cell)
//         ATH_MSG_DEBUG("NULL POINTER TO CELL!" );
//     else if(!m_useExtrapolation)
//         ATH_MSG_DEBUG("TRACK IS NOT EXTRAPOLATED." );
//     else if((layer = getLayer(cell)) >= 0){
//         std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
//         std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
//         if(v_entrance.size() == 0 || v_exit.size() == 0 || !v_entrance[layer] || !v_exit[layer]){
//             ATH_MSG_DEBUG("CANNOT GET EXTRAPOLATION POINTS IN LAYER." );
//             if(newTrack(track) && getExtrapolations(track) == StatusCode::FAILURE){
//                 return 0;
//             } // IF
//             if(isCellOnTrack(cell, m_extrapolations[layer].first, m_extrapolations[layer].second) ){
//                 return (cell->caloDDE()->getSampling() == 17) ? 0 : 1;
//             } // IF
//         } // IF
//         else{
//             return getActivePathFromCBNTMethod(cell, v_entrance[layer], v_exit[layer]);
//         } // ELSE
//     } // ELSE IF
//     return 0;
// } // TrackTools::getCBNTPathInsideCell

//------------------------------------------------------------------
/** Return dR = sqrt(deta^2 + dphi^2) for the positions of TrackParameters* a and b **/
double TrackTools::deltaR(const Trk::TrackParameters *a, const Trk::TrackParameters *b)
{
    double deta = a->position().eta() - b->position().eta();
    double dphi = phidiff(a->position().phi(), b->position().phi());
    return sqrt( deta*deta + dphi*dphi );
}
//------------------------------------------------------------------
/** Return a vector with the mid-point (eta, phi) and the cone to be used on cell preselection for the given subcalo **/
std::vector<double> TrackTools::getParametersForCellSelection(CaloCell_ID::SUBCALO subCalo)
{
    if(subCalo == CaloCell_ID::LAREM)
        return getMidPointAndDistance(m_extrapolations[ps], m_extrapolations[em3]);
    else if(subCalo == CaloCell_ID::TILE)
        return getMidPointAndDistance(m_extrapolations[tile1], m_extrapolations[tile3]);
    return getMidPointAndDistance(m_extrapolations[hec0], m_extrapolations[hec3]);
}
//------------------------------------------------------------------
/** Given two pairs representing two points (eta, phi), return a vector with the mid-point (eta, phi) and the distance between the mid-point and the points **/
std::vector<double> TrackTools::getMidPointAndDistance(std::pair<double, double> p1, std::pair<double, double> p2)
{
    std::vector<double> x;
    x.push_back( 0.5*(p1.first + p2.first) ); // eta
    x.push_back( phiMean(p1.second, p2.second) ); // phi
    x.push_back( sqrt( square(x[0] - p1.first) + square( phidiff(x[1], p1.second) ) ) ); // distance between mid-point and p1 (or p2)
    return x;
}
//------------------------------------------------------------------
/** Return the largest cone needed to collect the cells for Ecore or Eloss **/
double TrackTools::getLargestConeNeeded(ConeDefinition *coneDef)
{
    double max_cone = m_cone;
    for(int layer=ps; layer <= hec3; layer++)
        max_cone = std::max(max_cone, getConeSizeForDef(layer, coneDef) );
    return max_cone;
}

//========================================================================
double TrackTools::getConeSizeForDef(int ilayer, ConeDefinition *coneDef){
//=======================================================================
    if(coneDef->m_Param.count(ilayer) )
        return 0;
    else if(coneDef->m_Crossed.count(ilayer) )
        return 0.5; // not the right value, but ok
    else if(coneDef->m_Cone.count(ilayer) )
        return coneDef->m_Cone[ilayer];
    else if(coneDef->m_NxM.count(ilayer) )
        return CONE_NxM;
    else if(coneDef->m_ElCone.count(ilayer) )
        return sqrt( square(coneDef->m_ElCone[ilayer].first) + square(coneDef->m_ElCone[ilayer].second) );
    // Should never happen
    ATH_MSG_WARNING( "Wrong definition of " <<  coneDef->name << " for layer " << ilayer );
    return 0;
}

//===============================================================================
void TrackTools::removeCoreFromCone(std::vector< std::vector< const CaloCell*> > &cells,
                                    std::vector< const CaloCell*> core_cells,
                                    CaloLayer first_layer, CaloLayer last_layer){
//==============================================================================
    
    ATH_MSG_DEBUG("TrackTools::removeCoreFromCone" );
    if(last_layer < first_layer || first_layer < ps || last_layer > hec3){
        ATH_MSG_WARNING("Invalid layers: " << first_layer << " " << last_layer );
        return;
    } // IF
    
    std::vector<const CaloCell*>::iterator core_cellIt = core_cells.begin();
    for(; core_cellIt != core_cells.end(); ++core_cellIt){
        int layer = getLayer(*core_cellIt);
        int ilayer = layer - first_layer;
        if(layer < 0 || ilayer < 0)
            continue;
        if((unsigned int) ilayer >= cells.size() )
            break;
        std::vector<const CaloCell*>::iterator cellIt = cells[ilayer].begin();
        for(; cellIt != cells[ilayer].end(); ++cellIt)
            if((*cellIt) == (*core_cellIt)){
                cells[ilayer].erase(cellIt);
                break;
            } // IF
    } // FOR
}

//==================================================================================
void TrackTools::selectCellsByLayer(std::vector<const CaloCell*> &cells, int layer){
//=================================================================================
    std::vector<const CaloCell*>::iterator it = cells.begin();
    while (it != cells.end())
        if(getLayer(*it) != layer)
            cells.erase(it);
        else
            ++it;
} // TrackTools::selectCellsByLayer

//==============================================================================================================================
double TrackTools::pathInsideCell(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit){
//=============================================================================================================================
    
    ATH_MSG_DEBUG("in TrackInCaloTools::pathInsideCell" );
    if(!crossedPhi(cell, entrance->position().phi(), exit->position().phi()))
        return 0;
    double pathCrossed = 0;
    
    // Z PLANES
    if(cell->caloDDE()->getSampling() == CaloCell_ID::TileBar1)
        pathCrossed = getPathLengthInZTileBar1(cell, entrance->position().z(), exit->position().z());
    else
        pathCrossed = getPathLengthInZ(cell, entrance->position().z(), exit->position().z());
    if(pathCrossed > 0) return pathCrossed;
    
    // PHI PLANES
    pathCrossed = getPathLengthInPhi(cell, entrance->position().phi(), exit->position().phi());
    if(pathCrossed > 0) return pathCrossed;
    
    // ETA PLANES
    pathCrossed = getPathLengthInEta(cell, entrance->position().eta(), exit->position().eta());
    if(pathCrossed > 0) return pathCrossed;
    
    // SANITY CHECKS
    if(pathCrossed <= 0)
        return 0;
    return pathCrossed;
} // TrackTools::pathInsideCell

//================================================================================================================================
bool TrackTools::cellCrossedByTrack(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit){
//===============================================================================================================================
    int layer = getLayer(cell);
    if(layer < 0)
        return false;
    double pathCrossed = pathInsideCell(cell, entrance, exit);
    if(pathCrossed <= 0)
        return false;
    m_weightsForCellsCrossed[layer] += pathCrossed;
    return true;
} // TrackTools::cellCrossedByTrack

//======================================================================================
bool TrackTools::crossedPhi(const CaloCell *cell, double phi_entrance, double phi_exit){
//=====================================================================================
    double mean_phi = phiMean(phi_entrance, phi_exit);
    double dphi = fabs( phidiff(phi_entrance, phi_exit) );
    double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
    
    return (phidiff(cell->phi() + cell->caloDDE()->dphi()/2., phi_min) > 0 &&
            phidiff(phi_max, cell->phi() - cell->caloDDE()->dphi()/2.) > 0);
} // TrackTools::crossedPhi

//===========================================================================================
bool TrackTools::crossedPhi(const Scintillator *scint, double phi_entrance, double phi_exit){
//==========================================================================================
    double mean_phi = phiMean(phi_entrance, phi_exit);
    double dphi = fabs( phidiff(phi_entrance, phi_exit) );
    double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
    
    return (phidiff(scint->phi() + scint->dphi()/2., phi_min) > 0 &&
            phidiff(phi_max, scint->phi() - scint->dphi()/2.) > 0);
} // TrackTools::crossedPhi

//================================================================================================
double TrackTools::getPathLengthInPhi(const CaloCell *cell, double phi_entrance, double phi_exit){
//===============================================================================================
    double mean_phi = phiMean(phi_entrance, phi_exit);
    double dphi = fabs( phidiff(phi_entrance, phi_exit) );
    double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
    
    if(not crossedPhi(cell,phi_entrance,phi_exit))
        return 0;
    
    double phi_L = phidiff(phi_max, cell->phi() + 0.5*cell->caloDDE()->dphi()) > 0 ? cell->phi() + 0.5*cell->caloDDE()->dphi() : phi_max;
    double phi_R = phidiff(phi_min, cell->phi() - 0.5*cell->caloDDE()->dphi()) > 0 ? phi_min : cell->phi() + 0.5*cell->caloDDE()->dphi();
    return phidiff(phi_L, phi_R)/dphi;
} // TrackTools::getPathLengthInPhi

//=====================================================================================================
double TrackTools::getPathLengthInPhi(const Scintillator* scint, double phi_entrance, double phi_exit){
//====================================================================================================
    double mean_phi = phiMean(phi_entrance, phi_exit);
    double dphi = fabs( phidiff(phi_entrance, phi_exit) );
    double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
    
    if(not crossedPhi(scint,phi_entrance,phi_exit))
        return 0;
    
    double phi_L = phidiff(phi_max, scint->phi() + 0.5*scint->dphi()) > 0 ? scint->phi() + 0.5*scint->dphi() : phi_max;
    double phi_R = phidiff(phi_min, scint->phi() - 0.5*scint->dphi()) > 0 ? phi_min : scint->phi() + 0.5*scint->dphi();
    return phidiff(phi_L, phi_R)/dphi;
} // TrackTools::getPathLengthInPhi

//================================================================================================
double TrackTools::getPathLengthInEta(const CaloCell *cell, double eta_entrance, double eta_exit){
//===============================================================================================
    double etaMin = cell->eta() - 0.5*cell->caloDDE()->deta(), etaMax = cell->eta() + 0.5*cell->caloDDE()->deta();
    if(fabs(eta_entrance  - eta_exit) < 1e-6 ) // TO AVOID FLOATING POINT ERROR
        return eta_entrance > etaMin && eta_entrance < etaMax;
    
    double etaMinTrack = std::min(eta_entrance, eta_exit);
    double etaMaxTrack = std::max(eta_entrance, eta_exit);
    return (std::min(etaMax, etaMaxTrack) - std::max(etaMin, etaMinTrack))/(etaMaxTrack - etaMinTrack);
} // TrackTools::getPathLengthInEta

//=====================================================================================================
double TrackTools::getPathLengthInEta(const Scintillator* scint, double eta_entrance, double eta_exit){
//====================================================================================================
    double etaMin = scint->eta() - 0.5*scint->deta(), etaMax = scint->eta() + 0.5*scint->deta();
    if(fabs(eta_entrance  - eta_exit) < 1e-6 ) // TO AVOID FLOATING POINT ERROR
        return eta_entrance > etaMin && eta_entrance < etaMax;
    
    double etaMinTrack = std::min(eta_entrance, eta_exit);
    double etaMaxTrack = std::max(eta_entrance, eta_exit);
    return (std::min(etaMax, etaMaxTrack) - std::max(etaMin, etaMinTrack))/(etaMaxTrack - etaMinTrack);
} // TrackTools::getPathLengthInEta

//==========================================================================================
double TrackTools::getPathLengthInZ(const CaloCell *cell, double z_entrance, double z_exit){
//=========================================================================================
    return getPathLengthInZ(cell->z() - 0.5*cell->caloDDE()->dz(), cell->z() + 0.5*cell->caloDDE()->dz(), z_entrance, z_exit);
} // TrackTools::getPathLengthInZ

//===============================================================================================
double TrackTools::getPathLengthInZ(const Scintillator* scint, double z_entrance, double z_exit){
//==============================================================================================
    
    ATH_MSG_DEBUG("SCINTILLATOR FOUND: " << scint->z() << " | " << scint->dz()
                  << " | " << z_entrance << " | " << z_exit );
    return getPathLengthInZ(scint->z() - 0.5*scint->dz(), scint->z() + 0.5*scint->dz(), z_entrance, z_exit);
} // TrackTools::getPathLengthInZ

//==============================================================================================
double TrackTools::getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit){
//==============================================================================================
    if(fabs(z_entrance  - z_exit) < 1e-6 ) // TO AVOID FLOATING POINT ERROR
        return z_entrance > zMin && z_entrance < zMax;
    
    double zMinTrack = std::min(z_entrance, z_exit);
    double zMaxTrack = std::max(z_entrance, z_exit);
    
    return (std::min(zMax, zMaxTrack) - std::max(zMin, zMinTrack))/(zMaxTrack - zMinTrack);
} // TrackTools::getPathLengthInZ

//==================================================================================================
double TrackTools::getPathLengthInZTileBar1(const CaloCell *cell, double z_entrance, double z_exit){
//=================================================================================================
    // LADDER SHAPED CELLS: TWO PARTS ARE FIRST CALCULATED SEPARATELY
    
    
    ATH_MSG_DEBUG("in TrackTools::getPathLengthInZTileBar1" );
    TileCellDim *cell_dim = m_tileDDM->get_cell_dim(cell->caloDDE()->identify());
    if(!cell_dim || cell_dim->getNRows() != 6) // should always be the case for this sampling
        return 0;
    
    // BOTTOM PART
    double R           = (cell_dim->getRMax(2) - cell_dim->getRMin(0))/(cell_dim->getRMax(5) - cell_dim->getRMin(0));
    double z_middle    = z_entrance + R*(z_exit - z_entrance);
    double zBottom_min = cell_dim->getZMin(0), zBottom_max = cell_dim->getZMax(0);
    double zTop_min    = cell_dim->getZMin(3), zTop_max = cell_dim->getZMax(3);
    double pathBottom  = getPathLengthInZ(zBottom_min, zBottom_max, z_entrance, z_middle);
    
    // TOP PART
    if(zTop_min == zTop_max){
        if(fabs(z_middle) > fabs(zBottom_max) && pathBottom > 0){
            ATH_MSG_DEBUG("B9 CELL." );
            return 1;
        } // IF
        return pathBottom;
    } // IF
    double pathTop = getPathLengthInZ(zTop_min, zTop_max, z_middle, z_exit);
    return R*pathBottom + (1.-R)*pathTop;
} // TrackTools::getPathLengthInZTileBar1

//===================================================
void TrackTools::setNoiseThreshold(double threshold){
//==================================================
    
    if(threshold < m_cellNoiseRMSCutLoose)
        ATH_MSG_WARNING("CANNOT SET NOISE THRESHOLD BELOW LOOSE ONE, KEEPING ORIGINAL: "
                        << m_cellNoiseRMSCut );
    else
        m_cellNoiseRMSCut = threshold;
} // TrackTools::setNoiseThreshold

//========================================================================================================
void TrackTools::selectCellsBySample(std::vector<const CaloCell*> &cells, CaloCell_ID::CaloSample sample){
//=======================================================================================================
    std::vector<const CaloCell*>::iterator it = cells.begin();
    while (it != cells.end())
        if((*it) && (*it)->caloDDE()->getSampling() != sample)
            cells.erase(it);
        else
            ++it;
} // TrackTools::selectCellsBySample

//================================================================================================================
std::vector<double> TrackTools::getIsolationEnergy(std::vector<const INavigable4Momentum *> tracks, double dR,
                                                   CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut){
//===============================================================================================================
    std::vector<double> energies(tracks.size(), 0.);
    std::vector<const CaloCell*> core_cells;
    std::vector< std::vector< std::vector<const CaloCell*> > > cells;
    for(std::vector<const INavigable4Momentum *>::iterator it = tracks.begin(); it != tracks.end(); ++it){
        const Trk::TrackParameters *par = m_trackExtrapolatorToCalo->getTrackParameters(*it);
        cells.push_back( getCellsAroundTrack(par, dR, first_layer, last_layer, useLooseCut) );
        std::vector<const CaloCell*> c = getCellsForEcore(par, first_layer, last_layer, useLooseCut);
        core_cells.insert(core_cells.end(), c.begin(), c.end());
    } // FOR
    for(unsigned int i = 0; i < tracks.size(); ++i){
        removeCoreFromCone(cells[i], core_cells, first_layer, last_layer);
        for(int j = 0; j <= last_layer - first_layer; ++j){ // layer
            for(std::vector<const CaloCell*>::iterator cell_it = cells[i][j].begin(); cell_it != cells[i][j].end(); ++cell_it)
                energies[i] += (*cell_it)->et();
        } // FOR
    } // FOR
    return energies;
} // TrackTools::getIsolationEnergy

//===================================================================================================================
double TrackTools::getIsolationWidth(const Trk::TrackParameters* track,
                                     const double dR, const double EtExp, const double dRExp,
                                     bool negRemoval, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut){
//==================================================================================================================
    
    
    ATH_MSG_DEBUG("in TrackTools::getIsolationWidth" );
    
    if(last_layer < first_layer || first_layer < ps || last_layer > hec3){
        ATH_MSG_DEBUG("INVALID LAYERS: " << first_layer << " " << last_layer );
        return 0;
    } // IF
    
    double isolationSumWidth = 0.;
    double isolationSumEt = 0.;
    std::vector< std::vector<const CaloCell*> > isolation_cells = getCellsForIsolation(track, dR, first_layer, last_layer, useLooseCut);
    if(isolation_cells.size() == 0)
        return isolationSumWidth;
    for(unsigned int layer = (unsigned int) first_layer, ilayer = 0; layer <= (unsigned int) last_layer; layer++, ilayer++){
        for(unsigned int i=0; i < isolation_cells[ilayer].size(); i++){
            double Etcell=isolation_cells[ilayer][i]->et();
            if(negRemoval && Etcell<0) continue;
            double dRcell=P4Helpers::deltaR( *isolation_cells[ilayer][i], m_extrapolations[ilayer].first, m_extrapolations[ilayer].second );
            double EtTerm=pow( Etcell ,EtExp);
            double dRTerm=pow( dRcell ,dRExp);
            isolationSumWidth += EtTerm*dRTerm;
            isolationSumEt += EtTerm;
        } // FOR
    } // FOR
    if(isolationSumEt) return isolationSumWidth/isolationSumEt;
    else return 0.;
} // TrackTools::getIsolationWidth

//=====================
// SCINTILLATOR METHODS
//=====================

//=============================================================================
std::pair<float,float> TrackTools::getLadderZ(const CaloCell* cell, bool ladd){
//============================================================================
    
    TileCellDim *cell_dim = m_tileDDM->get_cell_dim(cell->caloDDE()->identify());
    if(cell->caloDDE()->getSampling() != 13 || ladd){
        return std::make_pair(cell_dim->getZMin(0),cell_dim->getZMax(0));
    } // IF
    else if(cell->caloDDE()->getSampling() == 13){
        return std::make_pair(cell_dim->getZMin(3),cell_dim->getZMax(3));
    } // ELSE IF
    return std::make_pair(0.,0.);
    
} // TRACKTOOLS::GETLADDERZ

//================================================================================
std::pair<float,float> TrackTools::getLadderZ(int sampling, float eta, bool ladd){
//===============================================================================
    
    // STORE MINIMAL AND MAXIMAL Z VALUES FOR LADDER SHAPED CELLS
    float ZMinPosBLB[9] = {   0.,    282.99,  565.99,   848.98,  1150.23,  1451.48,  1780.12,  2117.88,  2483.04};
    float ZMinNegBLB[9] = {-282.99, -565.99, -848.98, -1150.23, -1451.48, -1780.12, -2117.88, -2483.04, -2820.};
    float ZMaxPosBLB[9] = { 282.99,  565.99,  848.98,  1150.23,  1451.48,  1780.12,  2117.88,  2483.04,  2820.};
    float ZMaxNegBLB[9] = {   0.,   -282.99, -565.99,  -848.98, -1150.23, -1451.48, -1780.12, -2117.88, -2483.04};
    float ZMinPosCLB[8] = {   0.,    319.51,  648.15,   976.78,  1323.68,  1670.57,  2044.85,  2436.39};
    float ZMinNegCLB[8] = {-319.51, -648.15, -976.78, -1323.68, -1670.57, -2044.85, -2437.39,  2820.};
    float ZMaxPosCLB[8] = { 319.51,  648.15,  976.78,  1323.68,  1670.57,  2044.85,  2437.39,  2820.};
    float ZMaxNegCLB[8] = {   0.,   -319.51, -648.15,  -976.78, -1323.68, -1670.57, -2044.85, -2437.39};
    
    // RETURN APPROPRIATE Z VALUE PAIR (MIN, MAX)
    int cpos = fabs(eta)/0.1;
    if(sampling == 13 && eta > 0 && !ladd ) return std::make_pair(ZMinPosBLB[cpos],ZMaxPosBLB[cpos]);
    if(sampling == 13 && eta < 0 && !ladd ) return std::make_pair(ZMinNegBLB[cpos],ZMaxNegBLB[cpos]);
    if(sampling == 13 && eta > 0 &&  ladd ) return std::make_pair(ZMinPosCLB[cpos],ZMaxPosCLB[cpos]);
    if(sampling == 13 && eta < 0 &&  ladd ) return std::make_pair(ZMinNegCLB[cpos],ZMaxNegCLB[cpos]);
    return std::make_pair(0.,0.);
} // TRACKTOOLS::GETLADDERZ

//=======================================================================================================================
float TrackTools::getPathInsideScint(const Trk::TrackParameters *track, const CaloCell *cell, const Scintillator* scint){
//======================================================================================================================
    
    
    ATH_MSG_DEBUG("in TrackTools::getPathInsideScint" );
    
    int layer;
    if(!cell)
        ATH_MSG_DEBUG("NULL POINTER TO CELL!" );
    else if(!m_useExtrapolation)
        ATH_MSG_DEBUG("EXTRAPOLATION NOT REQUESTED." );
    else if((layer = getLayer(cell)) >= 0){
        std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
        std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
        if(v_entrance.size() == 0 || v_exit.size() == 0 || !v_entrance[layer] || !v_exit[layer]){
            ATH_MSG_DEBUG("EXTRAPOLATION RESOLUTION IS NOT SUFFICIENT" );
            if(newTrack(track) && getExtrapolations(track).isFailure()){
                ATH_MSG_DEBUG("EXTRAPOLATION FAILED." );
                return 0;
            } // IF
            if(isCellOnTrack(cell, m_extrapolations[layer].first, m_extrapolations[layer].second)){
                ATH_MSG_DEBUG("CELL IS NOT ON TRACK." );
                return 0;
            } // IF
        } // IF
        else
            return pathInsideScint(cell, scint, v_entrance[layer], v_exit[layer]);
    } // ELSE IF
    return 0;
    
} // TRACKTOOLS::GETPATHINSIDESCINT

//==================================================================
float TrackTools::pathInsideScint(const CaloCell *cell,
                                  const Scintillator *scint,
                                  const Trk::TrackParameters *entrance,
                                  const Trk::TrackParameters *exit){
//=================================================================
    
    
    ATH_MSG_DEBUG("in TrackTools::pathInsideScint" );
    
    if(!crossedPhi(scint, entrance->position().phi(), exit->position().phi())){
        ATH_MSG_DEBUG("NOT CROSSED IN PHI." );
        return 0;
    } // IF
    float pathCrossed = 0;
    if(cell->caloDDE()->getSubCalo() != CaloCell_ID::TILE){
        ATH_MSG_DEBUG("DOING ETA CALCULATION." );
        pathCrossed = getPathLengthInEta(scint, entrance->position().eta(), exit->position().eta());
    } // IF
    else{
        ATH_MSG_DEBUG("DOING Z CALCULATION." );
        pathCrossed = getPathLengthInZ(scint, entrance->position().z(), exit->position().z());
    } // ELSE
    if(pathCrossed <= 0){
        ATH_MSG_DEBUG("NEGATIVE PATH FOUND." );
        return 0;
    } // IF
    return pathCrossed;
    
} // TRACKTOOLS::PATHINSIDESCINT

//=====================================================
int TrackTools::retrieveIndex(int sampling, float eta){
//====================================================
    
    // STORE ETA MAP
    float etamap[81] = { -0.95,-0.85,-0.75,-0.65,-0.55,-0.45,-0.35,-0.25,-0.15,-0.05,  // CELLS A-10 TO A-1  (SAMPLING 12) INDICES  0:9
        0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95,  // CELLS A1 TO A10    (SAMPLING 12) INDICES 10:19
        -0.85,-0.75,-0.65,-0.55,-0.45,-0.35,-0.25,-0.15,-0.05,  // CELLS BC-9 TO BC-1 (SAMPLING 13) INDICES 20:28
        0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85,  // CELLS BC1 TO BC9   (SAMPLING 13) INDICES 29:37
        -0.60,-0.40,-0.20,  // CELLS D-3 TO D-1   (SAMPLING 14) INDICES 38:40
        0.00, 0.20, 0.40, 0.60,  // CELLS D0 TO D3     (SAMPLING 14) INDICES 41:44
        -0.956279,0.9583722,  // CELLS C-10 TO C10  (SAMPLING 15) INDICES 45:46
        -0.855940,0.8579205,  // CELLS D-4 TO D4    (SAMPLING 16) INDICES 47:48
        -1.507772,-1.307385,-1.156978,-1.056676,  // CELLS E-4 TO E-1   (SAMPLING 17) INDICES 49:52
        1.0589020,1.1593041,1.3098471,1.5103633,  // CELLS E1 TO E4     (SAMPLING 17) INDICES 53:56
        -1.554988,-1.455460,-1.355965,-1.256501,-1.157065,  // CELLS A-16 TO A-12 (SAMPLING 18) INDICES 57:61
        1.1594202,1.258668,1.3579534,1.4572804,1.5566510,  // CELLS A12 TO A16   (SAMPLING 18) INDICES 62:66
        -1.454651,-1.355081,-1.255538,-1.156018,-1.056519,  // CELLS B-15 TO B-11 (SAMPLING 19) INDICES 67:71
        1.0586925,1.1580252,1.2573844,1.3567756,1.4562022,  // CELLS B11 TO B15   (SAMPLING 19) INDICES 72:76
        -1.204743,-1.005559,  // CELLS D-6 TO D-5   (SAMPLING 20) INDICES 77:78
        1.0074122,1.2063241}; // CELLS D5 TO D6     (SAMPLING 20) INDICES 79:80
    // CALCULATE INDEX
    int index(-1),i_start(-1),i_end(-1);
    switch(sampling){
        case 12: i_start = 0;  i_end = 19; break;
        case 13: i_start = 20; i_end = 37; break;
        case 14: i_start = 38; i_end = 44; break;
        case 15: i_start = 45; i_end = 46; break;
        case 16: i_start = 47; i_end = 48; break;
        case 17: i_start = 49; i_end = 56; break;
        case 18: i_start = 57; i_end = 66; break;
        case 19: i_start = 67; i_end = 76; break;
        case 20: i_start = 77; i_end = 80; break;
        default: i_start = -1; i_end = -1; break;
    } // SWITCH
    
    index = i_start;
    
    for(int i=i_start;i<=i_end;++i) index = fabs(eta-etamap[i]) <= fabs(eta-etamap[index]) ? i : index;
    return index;
    
} // TRACKTOOLS::RETRIEVEINDEX


//=================================================================================================================
double TrackTools::getPathFromExtra(const CaloCell* cell, std::vector<Trk::TrackParameters*> track_extrapolations){
//================================================================================================================
    
    // SANITY TEST
    //     ATH_MSG_ERROR("STARTING METHOD...");
    if(track_extrapolations.size()!=21) return 0.;
    
    // OBTAIN LAYER INDICES FOR LINEAR INTERPOLATION
    unsigned int SampleID = cell->caloDDE()->getSampling();
    
    unsigned int inner(99);
    unsigned int outer(99);
    switch(SampleID){
        case 12: inner = 12; outer = 14; break;
        case 13: inner = 12; outer = 14; break;
        case 14: inner = 12; outer = 14; break;
        case 15: inner = 13; outer = 14; break;
        case 16: inner = 12; outer = 13; break;
        case 17: inner = 18; outer = 19; break;
        case 18: inner = 18; outer = 20; break;
        case 19: inner = 18; outer = 20; break;
        case 20: inner = 18; outer = 20; break;
        default: break;
    } // SWITCH
    if(inner == 99 || outer == 99) return 0.;
    
    if( not track_extrapolations[outer] || not track_extrapolations[inner] ) return 0.;
    
    //     ATH_MSG_ERROR("FINISHED SANITY CHECKS...: " << inner << " " << outer);
    
    // OBTAIN TRACK AND CELL PARAMETERS
    double pathl = 0.;
    double Layer1X(track_extrapolations[outer]->position().x()),
    Layer1Y(track_extrapolations[outer]->position().y()),
    Layer1Z(track_extrapolations[outer]->position().z());
    double Layer2X(track_extrapolations[inner]->position().x()),
    Layer2Y(track_extrapolations[inner]->position().y()),
    Layer2Z(track_extrapolations[inner]->position().z());
    
    //     ATH_MSG_ERROR("EXTRA: " << Layer1X << " , " << Layer1Y << " , " << Layer1Z << " | " << Layer2X << " , " << Layer2Y << " , " << Layer2Z);
    
    double CellPhi(cell->caloDDE()->phi()),CellDPhi(cell->caloDDE()->dphi());
    double CellPhimin = CellPhi-CellDPhi/2.;
    double CellPhimax = CellPhi+CellDPhi/2.;
    double CellEta(cell->caloDDE()->eta());
    double CellZ(cell->caloDDE()->z()),CellDZ(cell->caloDDE()->dz());
    double CellZmin = CellZ-CellDZ/2.;
    double CellZmax = CellZ+CellDZ/2.;
    double CellR(cell->caloDDE()->r()),CellDR(cell->caloDDE()->dr());
    double CellRmin = CellR-CellDR/2.;
    double CellRmax = CellR+CellDR/2.;
    //     ATH_MSG_ERROR("OBTAINED PARAMETERS...");
    
    // TILECAL GEOMETRY
    double RLBAmin = 2300;
    double RLBAmax = 2600;
    double RLBBmin = 2600;
    double RLBBmax = 2990;
    double RLBCmin = 2990;
    double RLBCmax = 3440;
    double RLBDmin = 3440;
    double RLBDmax = 3820;
    
    double REBAmin = 2300;
    double REBAmax = 2600;
    double REBBmin = 2600;
    double REBBmax = 3140;
    double REBDmin = 3140;
    double REBDmax = 3820;
    
    double RITC_D4_min  = 3630 - 380./2.;
    double RITC_C10_min = 3215 - 450./2.;
    double RITC_E1_min  = 2803 - 313./2.;
    double RITC_E2_min  = 2476 - 341./2.;
    double RITC_E3_min  = 2066 - 478./2.;
    double RITC_E4_min  = 1646 - 362./2.;
    
    double RITC_D4_max  = 3630 + 380./2.;
    double RITC_C10_max = 3215 + 450./2.;
    double RITC_E1_max  = 2803 + 313./2.;
    double RITC_E2_max  = 2476 + 341./2.;
    double RITC_E3_max  = 2066 + 478./2.;
    double RITC_E4_max  = 1646 + 362./2.;
    
    double ZITC_D4_a  = 3405;
    double ZITC_C10_a = 3511;
    double ZITC_E1_a  = 3552;
    //     double ZITC_E2_a  = 3552;
    double ZITC_E3_a  = 3536;
    //     double ZITC_E4_a  = 3536;
    
    double ZITC_D4_c  = 3395;
    double ZITC_C10_c = 3501;
    double ZITC_E1_c  = 3542;
    //     double ZITC_E2_c  = 3542;
    double ZITC_E3_c  = 3526;
    //     double ZITC_E4_c  = 3526;
    
    double ZDITC_D4  = 309.;
    double ZDITC_C10 = 95.;
    double ZDITC_E1  = 12.;
    //     double ZDITC_E2  = 12.;
    double ZDITC_E3  = 6.;
    //     double ZDITC_E4  = 6.;
    
    double CellZB[9]  = {141.495, 424.49, 707.485, 999.605, 1300.855, 1615.8, 1949., 2300.46, 2651.52};
    double CellDZB[9] = {282.99, 283., 282.99, 301.25, 301.25, 328.64, 337.76, 365.16, 336.96};
    double CellZC[9]  = {159.755, 483.83, 812.465, 1150.23, 1497.125, 1857.71, 2241.12, 2628.695,0};
    double CellDZC[9] = {319.51, 328.64, 328.63, 346.9, 346.89, 374.28, 392.54, 382.61,0};
    
    double CellXimp[2], CellYimp[2], CellZimp[2];
    //     double Sxz(0), Syz(0), Sxy(0);
    double X(0), Y(0), Z(0), R(0), Phi(0);
    double DeltaPhi;
    
    int phiInd(-1), etaInd(-1);
    
    // GET INDEX
    float phitilemodulemap[64] = {
        -0.0490874, -0.147262, -0.245437, -0.343612, -0.441786, -0.539961, -0.638136, -0.736311,
        -0.834486, -0.93266, -1.03084, -1.12901, -1.22718, -1.32536, -1.42353, -1.52171, -1.61988,
        -1.71806, -1.81623, -1.91441, -2.01258, -2.11076, -2.20893, -2.30711, -2.40528, -2.50346,
        -2.60163, -2.69981, -2.79798, -2.89616, -2.99433, -3.09251, 0.0490874, 0.147262, 0.245437,
        0.343612, 0.441786, 0.539961, 0.638136, 0.736311, 0.834486, 0.93266, 1.03084, 1.12901,
        1.22718, 1.32536, 1.42353, 1.52171, 1.61988, 1.71806, 1.81623, 1.91441, 2.01258, 2.11076,
        2.20893, 2.30711, 2.40528, 2.50346, 2.60163, 2.69981, 2.79798, 2.89616, 2.99433, 3.09251
    };
    float samplecellmap[81] = {
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20
    };
    float etacellmap[81] = { 0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95,
        0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,
        0.0, 0.2,0.4,0.6,
        0.8579205,0.9583722,1.0589020,1.1593041,1.3098471,1.5103633,
        1.1594202,1.2586680,1.3579534,1.4572804,1.556651,
        1.0586925,1.1580252,1.2573844,1.3567756,1.4562022,
        1.0074122,1.2063241,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,-0.95,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,
        -0.2,-0.4,-0.6,
        -0.855940, -0.956279, -1.056676, -1.156978,-1.307385,-1.507772,
        -1.157065,-1.256501,-1.355965,-1.455460,-1.554988,
        -1.056519,-1.156018,-1.255538,-1.355081,-1.454651,
        -1.005559,-1.204743
    };
    //     ATH_MSG_ERROR("PLAYED NUMBER GAMES...");
    
    if(CellPhi<0.05){
        for(int i=0;i<=32;i++){
            if(fabs(CellPhi - phitilemodulemap[i]) <0.001) phiInd=i;
        } // FOR
    } // IF
    else{
        for(int i=33;i<64;i++){
            if(fabs(CellPhi - phitilemodulemap[i]) <0.001) phiInd=i;
        } // FOR
    } // ELSE
    
    for(int i=0; i<81; i++){
        if(SampleID==samplecellmap[i] && fabs(CellEta-etacellmap[i])<0.001) etaInd = i;
    } // FOR
    
    if(phiInd == -1 || etaInd == -1) return 0.;
    //     ATH_MSG_ERROR("RETRIEVED INDICES...");
    
    // COMPUTE PATH
    bool compute = true;
    int lBC(0);
    while(compute){
        int Np = 0;
        //         ATH_MSG_ERROR(SampleID << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np);
        if(sqrt((Layer1X-Layer2X)*(Layer1X-Layer2X)+(Layer1Y-Layer2Y)*(Layer1Y-Layer2Y)) < 3818.5){
            if(SampleID == 15){
                CellRmin = RITC_C10_min;
                CellRmax = RITC_C10_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_C10_a-ZDITC_C10/2.) CellZmin = ZITC_C10_a-ZDITC_C10/2.;
                    if(CellZmax > ZITC_C10_a+ZDITC_C10/2.) CellZmax = ZITC_C10_a+ZDITC_C10/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_C10_c-ZDITC_C10/2.) CellZmin = -ZITC_C10_c-ZDITC_C10/2.;
                    if(CellZmax > -ZITC_C10_c+ZDITC_C10/2.) CellZmax = -ZITC_C10_c+ZDITC_C10/2.;
                } // ELSE IF
            } // IF
            else if(SampleID == 16){
                CellRmin=RITC_D4_min;
                CellRmax=RITC_D4_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_D4_a-ZDITC_D4/2.) CellZmin = ZITC_D4_a-ZDITC_D4/2.;
                    if(CellZmax > ZITC_D4_a+ZDITC_D4/2.) CellZmax = ZITC_D4_a+ZDITC_D4/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_D4_c-ZDITC_D4/2.) CellZmin = -ZITC_D4_c-ZDITC_D4/2.;
                    if(CellZmax > -ZITC_D4_c+ZDITC_D4/2.) CellZmax = -ZITC_D4_c+ZDITC_D4/2.;
                } // ELSE IF
            } // ELSE IF
            else if(SampleID == 17){
                if( etaInd==25 || etaInd==65 ){
                    CellRmax = RITC_E1_max;
                    CellRmin = RITC_E1_min;
                } // IF
                else if( etaInd==26 || etaInd==66 ){
                    CellRmax = RITC_E2_max;
                    CellRmin = RITC_E2_min;
                } // ELSE IF
                else if( etaInd==27 || etaInd==67 ){
                    CellRmax = RITC_E3_max;
                    CellRmin = RITC_E3_min;
                } // ELSE IF
                else if( etaInd==28 || etaInd==68 ){
                    CellRmax = RITC_E4_max;
                    CellRmin = RITC_E4_min;
                } // ELSE IF
                if( (etaInd>=25 && etaInd<=26) || (etaInd>=65 && etaInd<=66) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E1_a-ZDITC_E1/2.) CellZmin=ZITC_E1_a-ZDITC_E1/2.;
                        if(CellZmax > ZITC_E1_a+ZDITC_E1/2.) CellZmax=ZITC_E1_a+ZDITC_E1/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E1_c-ZDITC_E1/2.) CellZmin= -ZITC_E1_c-ZDITC_E1/2.;
                        if(CellZmax > -ZITC_E1_c+ZDITC_E1/2.) CellZmax= -ZITC_E1_c+ZDITC_E1/2.;
                    } // ELSE
                } // IF
                else if( (etaInd>=27 && etaInd<=28) || (etaInd>=67 && etaInd<=68) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E3_a-ZDITC_E3/2.) CellZmin=ZITC_E3_a-ZDITC_E3/2.;
                        if(CellZmax > ZITC_E3_a+ZDITC_E3/2.) CellZmax=ZITC_E3_a+ZDITC_E3/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E3_c-ZDITC_E3/2.) CellZmin= -ZITC_E3_c-ZDITC_E3/2.;
                        if(CellZmax > -ZITC_E3_c+ZDITC_E3/2.) CellZmax= -ZITC_E3_c+ZDITC_E3/2.;
                    } // ELSE IF
                } // ELSE
            } // ELSE IF
            else if(SampleID == 12){
                CellRmin = RLBAmin;
                CellRmax = RLBAmax;
                
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 0){
                CellRmin = RLBBmin;
                CellRmax = RLBBmax;
                int cpos = fabs(CellEta)/0.1;
                CellZ = CellZB[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZB[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 1){
                CellRmin = RLBCmin;
                CellRmax = RLBCmax;
                int cpos = fabs(CellEta)/0.1;
                if(cpos >= 8) return 0;
                CellZ = CellZC[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZC[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 14){
                CellRmin = RLBDmin;
                CellRmax = RLBDmax;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            if(SampleID == 18){
                CellRmin = REBAmin;
                CellRmax = REBAmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // IF
            } // IF
            if(SampleID == 19){
                CellRmin = REBBmin;
                CellRmax = REBBmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            if(SampleID == 20){
                CellRmin = REBDmin;
                CellRmax = REBDmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            
            // CALCULATE POINTS OF INTERSECTION
            // INTERSECTIONS R PLANES
            //             ATH_MSG_ERROR("INTERSECTION R PLANES " << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np << endl);
            double Radius(CellRmin);
            
            double x0int(track_extrapolations[outer]->position().x()), x1int(track_extrapolations[inner]->position().x()),
            y0int(track_extrapolations[outer]->position().y()), y1int(track_extrapolations[inner]->position().y()),
            z0int(track_extrapolations[outer]->position().z()), z1int(track_extrapolations[inner]->position().z());
            double S((y1int-y0int)/(x1int-x0int));
            double a(1+S*S), b(2*S*y0int-2*S*S*x0int), c(y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int);
            double x1((-b+sqrt(b*b-4*a*c))/(2*a)), x2((-b-sqrt(b*b-4*a*c))/(2*a));
            double y1(y0int+S*(x1-x0int)), y2(y0int+S*(x2-x0int));
            double S1 = ((z1int-z0int)/(x1int-x0int)); //, R1(sqrt(x1*x1+y1*y1)), R2(sqrt(x2*x2+y2*y2));
            
            double z1(z0int+S1*(x1-x0int)), z2(z0int+S1*(x2-x0int));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X=x2;
                Y=y2;
                Z=z2;
            } // IF
            
            Phi = acos(X/sqrt(X*X+Y*Y));
            if(Y <= 0) Phi = -Phi;
            R = CellRmin;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X;
                CellYimp[Np]=Y;
                CellZimp[Np]=Z;
                Np=Np+1;
            } // IF
            
            Radius = CellRmax;
            
            c  = y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int;
            x1 = ((-b+sqrt(b*b-4*a*c))/(2*a));
            x2 = ((-b-sqrt(b*b-4*a*c))/(2*a));
            y1 = (y0int+S*(x1-x0int));
            y2 = (y0int+S*(x2-x0int));
            z1 = (z0int+S1*(x1-x0int));
            z2 = (z0int+S1*(x2-x0int));
            S1 = ((z1int-z0int)/(x1int-x0int));
            //             R1 = (sqrt(x1*x1+y1*y1));
            //             R2 = (sqrt(x2*x2+y2*y2));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X = x2;
                Y = y2;
                Z = z2;
            } // IF
            
            Phi=acos(X/sqrt(X*X+Y*Y));
            if (Y <= 0) Phi=-Phi;
            R=CellRmax;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                Np=Np+1;
            } // IF
            
            // INTERSECTIONS Z PLANES
            //             ATH_MSG_ERROR("INTERSECTION Z PLANES " << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np << endl);
            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmin;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmax;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            // INTERSECTIONS PHI PLANES
            //             ATH_MSG_ERROR("INTERSECTION PHI PLANES " << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np << endl);
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimin));
                Y = X*tan(CellPhimin);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                Phi = acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimax));
                Y = X*tan(CellPhimax);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            // CALCULATE PATH IF TWO INTERSECTIONS WERE FOUND
            if(Np == 2){
                pathl += sqrt( (CellXimp[0]-CellXimp[1])*(CellXimp[0]-CellXimp[1]) +
                              (CellYimp[0]-CellYimp[1])*(CellYimp[0]-CellYimp[1]) +
                              (CellZimp[0]-CellZimp[1])*(CellZimp[0]-CellZimp[1]) );
            } // IF
        } // IF
        if(SampleID == 13 && lBC == 0) ++lBC;
        else compute = false;
    } // WHILE (FOR LBBC LAYER)
    
    return pathl;
} // TrackTools::getPathFromExtra

//===================================================================================
double TrackTools::getPathFromExtra(const Trk::Track* i_trk, const CaloCell* i_cell){
//==================================================================================
    
    // OBTAIN LAYER INDICES FOR LINEAR INTERPOLATION
    const CaloCell* cell = i_cell;
    unsigned int SampleID = cell->caloDDE()->getSampling();
    
    unsigned int inner(99);
    unsigned int outer(99);
    switch(SampleID){
        case 12: inner = 12; outer = 14; break;
        case 13: inner = 12; outer = 14; break;
        case 14: inner = 12; outer = 14; break;
        case 15: inner = 13; outer = 14; break;
        case 16: inner = 12; outer = 13; break;
        case 17: inner = 18; outer = 19; break;
        case 18: inner = 18; outer = 20; break;
        case 19: inner = 18; outer = 20; break;
        case 20: inner = 18; outer = 20; break;
        default: break;
    } // SWITCH
    if(inner == 99 || outer == 99) return 0.;
    
    // EXTRAPOLATION AND SANITY TEST
    std::vector<Trk::TrackParameters*> track_extrapolations;
    for(unsigned int l=inner; l<=outer; ++l){
        const Trk::TrackParameters *par1 = m_toCalo->extrapolate( *i_trk,
                                                                 static_cast<CaloCell_ID::CaloSample>(l),
                                                                 0.0,Trk::alongMomentum );
        const Trk::TrackParameters *par2 = m_toCalo->extrapolate( *i_trk,
                                                                 static_cast<CaloCell_ID::CaloSample>(l),
                                                                 0.0,Trk::oppositeMomentum );
        if(par1) track_extrapolations.push_back(const_cast<Trk::TrackParameters*>(par1));
        else if(par2) track_extrapolations.push_back(const_cast<Trk::TrackParameters*>(par2));
        else track_extrapolations.push_back(0);
    } // FOR
    
    outer = outer - inner;
    inner = 0;
    if( not track_extrapolations[outer] || not track_extrapolations[inner] ) return 0.;
    
    //     ATH_MSG_ERROR("FINISHED SANITY CHECKS...: " << inner << " " << outer);
    
    // OBTAIN TRACK AND CELL PARAMETERS
    double pathl = 0.;
    double Layer1X(track_extrapolations[outer]->position().x()),
    Layer1Y(track_extrapolations[outer]->position().y()),
    Layer1Z(track_extrapolations[outer]->position().z());
    double Layer2X(track_extrapolations[inner]->position().x()),
    Layer2Y(track_extrapolations[inner]->position().y()),
    Layer2Z(track_extrapolations[inner]->position().z());
    
    //     ATH_MSG_ERROR("EXTRA: " << Layer1X << " , " << Layer1Y << " , " << Layer1Z << " | " << Layer2X << " , " << Layer2Y << " , " << Layer2Z);
    
    double CellPhi(cell->caloDDE()->phi()),CellDPhi(cell->caloDDE()->dphi());
    double CellPhimin = CellPhi-CellDPhi/2.;
    double CellPhimax = CellPhi+CellDPhi/2.;
    double CellEta(cell->caloDDE()->eta());
    double CellZ(cell->caloDDE()->z()),CellDZ(cell->caloDDE()->dz());
    double CellZmin = CellZ-CellDZ/2.;
    double CellZmax = CellZ+CellDZ/2.;
    double CellR(cell->caloDDE()->r()),CellDR(cell->caloDDE()->dr());
    double CellRmin = CellR-CellDR/2.;
    double CellRmax = CellR+CellDR/2.;
    //     ATH_MSG_ERROR("OBTAINED PARAMETERS...");
    
    // TILECAL GEOMETRY
    double RLBAmin = 2300;
    double RLBAmax = 2600;
    double RLBBmin = 2600;
    double RLBBmax = 2990;
    double RLBCmin = 2990;
    double RLBCmax = 3440;
    double RLBDmin = 3440;
    double RLBDmax = 3820;
    
    double REBAmin = 2300;
    double REBAmax = 2600;
    double REBBmin = 2600;
    double REBBmax = 3140;
    double REBDmin = 3140;
    double REBDmax = 3820;
    
    double RITC_D4_min  = 3630 - 380./2.;
    double RITC_C10_min = 3215 - 450./2.;
    double RITC_E1_min  = 2803 - 313./2.;
    double RITC_E2_min  = 2476 - 341./2.;
    double RITC_E3_min  = 2066 - 478./2.;
    double RITC_E4_min  = 1646 - 362./2.;
    
    double RITC_D4_max  = 3630 + 380./2.;
    double RITC_C10_max = 3215 + 450./2.;
    double RITC_E1_max  = 2803 + 313./2.;
    double RITC_E2_max  = 2476 + 341./2.;
    double RITC_E3_max  = 2066 + 478./2.;
    double RITC_E4_max  = 1646 + 362./2.;
    
    double ZITC_D4_a  = 3405;
    double ZITC_C10_a = 3511;
    double ZITC_E1_a  = 3552;
    //     double ZITC_E2_a  = 3552;
    double ZITC_E3_a  = 3536;
    //     double ZITC_E4_a  = 3536;
    
    double ZITC_D4_c  = 3395;
    double ZITC_C10_c = 3501;
    double ZITC_E1_c  = 3542;
    //     double ZITC_E2_c  = 3542;
    double ZITC_E3_c  = 3526;
    //     double ZITC_E4_c  = 3526;
    
    double ZDITC_D4  = 309.;
    double ZDITC_C10 = 95.;
    double ZDITC_E1  = 12.;
    //     double ZDITC_E2  = 12.;
    double ZDITC_E3  = 6.;
    //     double ZDITC_E4  = 6.;
    
    double CellZB[9]  = {141.495, 424.49, 707.485, 999.605, 1300.855, 1615.8, 1949., 2300.46, 2651.52};
    double CellDZB[9] = {282.99, 283., 282.99, 301.25, 301.25, 328.64, 337.76, 365.16, 336.96};
    double CellZC[9]  = {159.755, 483.83, 812.465, 1150.23, 1497.125, 1857.71, 2241.12, 2628.695,0};
    double CellDZC[9] = {319.51, 328.64, 328.63, 346.9, 346.89, 374.28, 392.54, 382.61,0};
    
    double CellXimp[2], CellYimp[2], CellZimp[2];
    //     double Sxz(0), Syz(0), Sxy(0);
    double X(0), Y(0), Z(0), R(0), Phi(0);
    double DeltaPhi;
    
    int phiInd(-1), etaInd(-1);
    
    // GET INDEX
    float phitilemodulemap[64] = {
        -0.0490874, -0.147262, -0.245437, -0.343612, -0.441786, -0.539961, -0.638136, -0.736311,
        -0.834486, -0.93266, -1.03084, -1.12901, -1.22718, -1.32536, -1.42353, -1.52171, -1.61988,
        -1.71806, -1.81623, -1.91441, -2.01258, -2.11076, -2.20893, -2.30711, -2.40528, -2.50346,
        -2.60163, -2.69981, -2.79798, -2.89616, -2.99433, -3.09251, 0.0490874, 0.147262, 0.245437,
        0.343612, 0.441786, 0.539961, 0.638136, 0.736311, 0.834486, 0.93266, 1.03084, 1.12901,
        1.22718, 1.32536, 1.42353, 1.52171, 1.61988, 1.71806, 1.81623, 1.91441, 2.01258, 2.11076,
        2.20893, 2.30711, 2.40528, 2.50346, 2.60163, 2.69981, 2.79798, 2.89616, 2.99433, 3.09251
    };
    float samplecellmap[81] = {
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20
    };
    float etacellmap[81] = { 0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95,
        0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,
        0.0, 0.2,0.4,0.6,
        0.8579205,0.9583722,1.0589020,1.1593041,1.3098471,1.5103633,
        1.1594202,1.2586680,1.3579534,1.4572804,1.556651,
        1.0586925,1.1580252,1.2573844,1.3567756,1.4562022,
        1.0074122,1.2063241,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,-0.95,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,
        -0.2,-0.4,-0.6,
        -0.855940, -0.956279, -1.056676, -1.156978,-1.307385,-1.507772,
        -1.157065,-1.256501,-1.355965,-1.455460,-1.554988,
        -1.056519,-1.156018,-1.255538,-1.355081,-1.454651,
        -1.005559,-1.204743
    };
    //     ATH_MSG_ERROR("PLAYED NUMBER GAMES...");
    
    if(CellPhi<0.05){
        for(int i=0;i<=32;i++){
            if(fabs(CellPhi - phitilemodulemap[i]) <0.001) phiInd=i;
        } // FOR
    } // IF
    else{
        for(int i=33;i<64;i++){
            if(fabs(CellPhi - phitilemodulemap[i]) <0.001) phiInd=i;
        } // FOR
    } // ELSE
    
    for(int i=0; i<81; i++){
        if(SampleID==samplecellmap[i] && fabs(CellEta-etacellmap[i])<0.001) etaInd = i;
    } // FOR
    
    if(phiInd == -1 || etaInd == -1) return 0.;
    //     ATH_MSG_ERROR("RETRIEVED INDICES...");
    
    // COMPUTE PATH
    bool compute = true;
    int lBC(0);
    while(compute){
        int Np = 0;
        //         ATH_MSG_ERROR(SampleID << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np);
        if(sqrt((Layer1X-Layer2X)*(Layer1X-Layer2X)+(Layer1Y-Layer2Y)*(Layer1Y-Layer2Y)) < 3818.5){
            if(SampleID == 15){
                CellRmin = RITC_C10_min;
                CellRmax = RITC_C10_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_C10_a-ZDITC_C10/2.) CellZmin = ZITC_C10_a-ZDITC_C10/2.;
                    if(CellZmax > ZITC_C10_a+ZDITC_C10/2.) CellZmax = ZITC_C10_a+ZDITC_C10/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_C10_c-ZDITC_C10/2.) CellZmin = -ZITC_C10_c-ZDITC_C10/2.;
                    if(CellZmax > -ZITC_C10_c+ZDITC_C10/2.) CellZmax = -ZITC_C10_c+ZDITC_C10/2.;
                } // ELSE IF
            } // IF
            else if(SampleID == 16){
                CellRmin=RITC_D4_min;
                CellRmax=RITC_D4_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_D4_a-ZDITC_D4/2.) CellZmin = ZITC_D4_a-ZDITC_D4/2.;
                    if(CellZmax > ZITC_D4_a+ZDITC_D4/2.) CellZmax = ZITC_D4_a+ZDITC_D4/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_D4_c-ZDITC_D4/2.) CellZmin = -ZITC_D4_c-ZDITC_D4/2.;
                    if(CellZmax > -ZITC_D4_c+ZDITC_D4/2.) CellZmax = -ZITC_D4_c+ZDITC_D4/2.;
                } // ELSE IF
            } // ELSE IF
            else if(SampleID == 17){
                if( etaInd==25 || etaInd==65 ){
                    CellRmax = RITC_E1_max;
                    CellRmin = RITC_E1_min;
                } // IF
                else if( etaInd==26 || etaInd==66 ){
                    CellRmax = RITC_E2_max;
                    CellRmin = RITC_E2_min;
                } // ELSE IF
                else if( etaInd==27 || etaInd==67 ){
                    CellRmax = RITC_E3_max;
                    CellRmin = RITC_E3_min;
                } // ELSE IF
                else if( etaInd==28 || etaInd==68 ){
                    CellRmax = RITC_E4_max;
                    CellRmin = RITC_E4_min;
                } // ELSE IF
                if( (etaInd>=25 && etaInd<=26) || (etaInd>=65 && etaInd<=66) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E1_a-ZDITC_E1/2.) CellZmin=ZITC_E1_a-ZDITC_E1/2.;
                        if(CellZmax > ZITC_E1_a+ZDITC_E1/2.) CellZmax=ZITC_E1_a+ZDITC_E1/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E1_c-ZDITC_E1/2.) CellZmin= -ZITC_E1_c-ZDITC_E1/2.;
                        if(CellZmax > -ZITC_E1_c+ZDITC_E1/2.) CellZmax= -ZITC_E1_c+ZDITC_E1/2.;
                    } // ELSE
                } // IF
                else if( (etaInd>=27 && etaInd<=28) || (etaInd>=67 && etaInd<=68) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E3_a-ZDITC_E3/2.) CellZmin=ZITC_E3_a-ZDITC_E3/2.;
                        if(CellZmax > ZITC_E3_a+ZDITC_E3/2.) CellZmax=ZITC_E3_a+ZDITC_E3/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E3_c-ZDITC_E3/2.) CellZmin= -ZITC_E3_c-ZDITC_E3/2.;
                        if(CellZmax > -ZITC_E3_c+ZDITC_E3/2.) CellZmax= -ZITC_E3_c+ZDITC_E3/2.;
                    } // ELSE IF
                } // ELSE
            } // ELSE IF
            else if(SampleID == 12){
                CellRmin = RLBAmin;
                CellRmax = RLBAmax;
                
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 0){
                CellRmin = RLBBmin;
                CellRmax = RLBBmax;
                int cpos = fabs(CellEta)/0.1;
                CellZ = CellZB[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZB[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 1){
                CellRmin = RLBCmin;
                CellRmax = RLBCmax;
                int cpos = fabs(CellEta)/0.1;
                if(cpos >= 8) return 0;
                CellZ = CellZC[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZC[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 14){
                CellRmin = RLBDmin;
                CellRmax = RLBDmax;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            if(SampleID == 18){
                CellRmin = REBAmin;
                CellRmax = REBAmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // IF
            } // IF
            if(SampleID == 19){
                CellRmin = REBBmin;
                CellRmax = REBBmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            if(SampleID == 20){
                CellRmin = REBDmin;
                CellRmax = REBDmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            
            // CALCULATE POINTS OF INTERSECTION
            // INTERSECTIONS R PLANES
            //             ATH_MSG_ERROR("INTERSECTION R PLANES " << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np << endl);
            double Radius(CellRmin);
            
            double x0int(track_extrapolations[outer]->position().x()), x1int(track_extrapolations[inner]->position().x()),
            y0int(track_extrapolations[outer]->position().y()), y1int(track_extrapolations[inner]->position().y()),
            z0int(track_extrapolations[outer]->position().z()), z1int(track_extrapolations[inner]->position().z());
            double S((y1int-y0int)/(x1int-x0int));
            double a(1+S*S), b(2*S*y0int-2*S*S*x0int), c(y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int);
            double x1((-b+sqrt(b*b-4*a*c))/(2*a)), x2((-b-sqrt(b*b-4*a*c))/(2*a));
            double y1(y0int+S*(x1-x0int)), y2(y0int+S*(x2-x0int));
            double S1 = ((z1int-z0int)/(x1int-x0int));//, R1(sqrt(x1*x1+y1*y1)), R2(sqrt(x2*x2+y2*y2));
            double z1(z0int+S1*(x1-x0int)), z2(z0int+S1*(x2-x0int));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X=x2;
                Y=y2;
                Z=z2;
            } // IF
            
            Phi = acos(X/sqrt(X*X+Y*Y));
            if(Y <= 0) Phi = -Phi;
            R = CellRmin;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X;
                CellYimp[Np]=Y;
                CellZimp[Np]=Z;
                Np=Np+1;
            } // IF
            
            Radius = CellRmax;
            
            c  = y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int;
            x1 = ((-b+sqrt(b*b-4*a*c))/(2*a));
            x2 = ((-b-sqrt(b*b-4*a*c))/(2*a));
            y1 = (y0int+S*(x1-x0int));
            y2 = (y0int+S*(x2-x0int));
            z1 = (z0int+S1*(x1-x0int));
            z2 = (z0int+S1*(x2-x0int));
            S1 = ((z1int-z0int)/(x1int-x0int));
            //R1 = (sqrt(x1*x1+y1*y1));
            //R2 = (sqrt(x2*x2+y2*y2));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X = x2;
                Y = y2;
                Z = z2;
            } // IF
            
            Phi=acos(X/sqrt(X*X+Y*Y));
            if (Y <= 0) Phi=-Phi;
            R=CellRmax;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                Np=Np+1;
            } // IF
            
            // INTERSECTIONS Z PLANES
            //             ATH_MSG_ERROR("INTERSECTION Z PLANES " << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np << endl);
            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmin;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmax;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            // INTERSECTIONS PHI PLANES
            //             ATH_MSG_ERROR("INTERSECTION PHI PLANES " << " | IN WHILE LOOP, NUMBER OF INTERACTION POINTS: " << Np << endl);
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimin));
                Y = X*tan(CellPhimin);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                Phi = acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimax));
                Y = X*tan(CellPhimax);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            
            // CALCULATE PATH IF TWO INTERSECTIONS WERE FOUND
            if(Np == 2){
                pathl += sqrt( (CellXimp[0]-CellXimp[1])*(CellXimp[0]-CellXimp[1]) +
                              (CellYimp[0]-CellYimp[1])*(CellYimp[0]-CellYimp[1]) +
                              (CellZimp[0]-CellZimp[1])*(CellZimp[0]-CellZimp[1]) );
            } // IF
        } // IF
        if(SampleID == 13 && lBC == 0) ++lBC;
        else compute = false;
    } // WHILE (FOR LBBC LAYER)
    
    return pathl;
} // TrackTools::getPathFromExtra

//=====================================================================================================================================
double TrackTools::getPathFromCBNTMethod(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit){
//====================================================================================================================================
    ATH_MSG_DEBUG("IN getPathFromCBNTMethod...");

    // OBTAIN LAYER INDICES FOR LINEAR INTERPOLATION
    unsigned int SampleID = cell->caloDDE()->getSampling();
    
    // OBTAIN TRACK AND CELL PARAMETERS
    double pathl = 0.;
    double Layer1X(exit->position().x()),Layer1Y(exit->position().y()),Layer1Z(exit->position().z());
    double Layer2X(entrance->position().x()),Layer2Y(entrance->position().y()),Layer2Z(entrance->position().z());

//    double Layer1X(-2180.56),Layer1Y(2660.6),Layer1Z(-3275.18);
//    double Layer2X(-1459.79),Layer2Y(1777.36),Layer2Z(-2193.83);

    //     ATH_MSG_ERROR("CBNT: " << Layer1X << " , " << Layer1Y << " , " << Layer1Z << " | " << Layer2X << " , " << Layer2Y << " , " << Layer2Z);
    double CellPhi(cell->caloDDE()->phi()),CellDPhi(cell->caloDDE()->dphi());
    double CellPhimin = CellPhi-CellDPhi/2.;
    double CellPhimax = CellPhi+CellDPhi/2.;
    double CellEta(cell->caloDDE()->eta());
    double CellZ(cell->caloDDE()->z()),CellDZ(cell->caloDDE()->dz());
    double CellZmin = CellZ-CellDZ/2.;
    double CellZmax = CellZ+CellDZ/2.;
    double CellR(cell->caloDDE()->r()),CellDR(cell->caloDDE()->dr());
    double CellRmin = CellR-CellDR/2.;
    double CellRmax = CellR+CellDR/2.;
    
    // TILECAL GEOMETRY
    double RLBAmin = 2300;
    double RLBAmax = 2600;
    double RLBBmin = 2600;
    double RLBBmax = 2990;
    double RLBCmin = 2990;
    double RLBCmax = 3440;
    double RLBDmin = 3440;
    double RLBDmax = 3820;
    
    double REBAmin = 2300;
    double REBAmax = 2600;
    double REBBmin = 2600;
    double REBBmax = 3140;
    double REBDmin = 3140;
    double REBDmax = 3820;
    
    double RITC_D4_min  = 3630 - 380./2.;
    double RITC_C10_min = 3215 - 450./2.;
    double RITC_E1_min  = 2803 - 313./2.;
    double RITC_E2_min  = 2476 - 341./2.;
    double RITC_E3_min  = 2066 - 478./2.;
    double RITC_E4_min  = 1646 - 362./2.;
    
    double RITC_D4_max  = 3630 + 380./2.;
    double RITC_C10_max = 3215 + 450./2.;
    double RITC_E1_max  = 2803 + 313./2.;
    double RITC_E2_max  = 2476 + 341./2.;
    double RITC_E3_max  = 2066 + 478./2.;
    double RITC_E4_max  = 1646 + 362./2.;
    
    double ZITC_D4_a  = 3405;
    double ZITC_C10_a = 3511;
    double ZITC_E1_a  = 3552;
    //     double ZITC_E2_a  = 3552;
    double ZITC_E3_a  = 3536;
    //     double ZITC_E4_a  = 3536;
    
    double ZITC_D4_c  = 3395;
    double ZITC_C10_c = 3501;
    double ZITC_E1_c  = 3542;
    //     double ZITC_E2_c  = 3542;
    double ZITC_E3_c  = 3526;
    //     double ZITC_E4_c  = 3526;
    
    double ZDITC_D4  = 309.;
    double ZDITC_C10 = 95.;
    double ZDITC_E1  = 12.;
    //     double ZDITC_E2  = 12.;
    double ZDITC_E3  = 6.;
    //     double ZDITC_E4  = 6.;
    
    double CellZB[9]  = {141.495, 424.49, 707.485, 999.605, 1300.855, 1615.8, 1949., 2300.46, 2651.52};
    double CellDZB[9] = {282.99, 283., 282.99, 301.25, 301.25, 328.64, 337.76, 365.16, 336.96};
    double CellZC[9]  = {159.755, 483.83, 812.465, 1150.23, 1497.125, 1857.71, 2241.12, 2628.695,0};
    double CellDZC[9] = {319.51, 328.64, 328.63, 346.9, 346.89, 374.28, 392.54, 382.61,0};
    
    double CellXimp[2], CellYimp[2], CellZimp[2];
    //     double Sxz(0), Syz(0), Sxy(0);
    double X(0), Y(0), Z(0), R(0), Phi(0);
    double DeltaPhi;
    
    int etaInd(-1);

    float samplecellmap[81] = {
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        13, 13, 13, 13, 13, 13, 13, 13, 13,
        14, 14, 14,
        16, 15, 17, 17, 17, 17,            //** ITC
        18, 18, 18, 18, 18,
        19, 19, 19, 19, 19,
        20, 20
    };
    float etacellmap[81] = { 0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95,
        0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,
        0.0, 0.2,0.4,0.6,
        0.8579205,0.9583722,1.0589020,1.1593041,1.3098471,1.5103633,
        1.1594202,1.2586680,1.3579534,1.4572804,1.556651,
        1.0586925,1.1580252,1.2573844,1.3567756,1.4562022,
        1.0074122,1.2063241,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,-0.95,
        -0.05,-0.15,-0.25,-0.35,-0.45,-0.55,-0.65,-0.75,-0.85,
        -0.2,-0.4,-0.6,
        -0.855940, -0.956279, -1.056676, -1.156978,-1.307385,-1.507772,
        -1.157065,-1.256501,-1.355965,-1.455460,-1.554988,
        -1.056519,-1.156018,-1.255538,-1.355081,-1.454651,
        -1.005559,-1.204743
    };
    
    for(int i=0; i<81; i++){
        if(SampleID==samplecellmap[i] && fabs(CellEta-etacellmap[i])<0.001) etaInd = i;
    } // FOR
    
    // COMPUTE PATH
    bool compute = true;
    int lBC(0);
    while(compute){
        //=============Added by Archil==================
        if(lBC==1 && (etaInd==18 || etaInd==59) ) break;
        //==============================================
        int Np = 0;
        if(sqrt((Layer1X-Layer2X)*(Layer1X-Layer2X)+(Layer1Y-Layer2Y)*(Layer1Y-Layer2Y)) < 3818.5){
            if(SampleID == 15){
                CellRmin = RITC_C10_min;
                CellRmax = RITC_C10_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_C10_a-ZDITC_C10/2.) CellZmin = ZITC_C10_a-ZDITC_C10/2.;
                    if(CellZmax > ZITC_C10_a+ZDITC_C10/2.) CellZmax = ZITC_C10_a+ZDITC_C10/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_C10_c-ZDITC_C10/2.) CellZmin = -ZITC_C10_c-ZDITC_C10/2.;
                    if(CellZmax > -ZITC_C10_c+ZDITC_C10/2.) CellZmax = -ZITC_C10_c+ZDITC_C10/2.;
                } // ELSE IF
            } // IF
            else if(SampleID == 16){
                CellRmin=RITC_D4_min;
                CellRmax=RITC_D4_max;
                if(CellEta > 0){
                    if(CellZmin < ZITC_D4_a-ZDITC_D4/2.) CellZmin = ZITC_D4_a-ZDITC_D4/2.;
                    if(CellZmax > ZITC_D4_a+ZDITC_D4/2.) CellZmax = ZITC_D4_a+ZDITC_D4/2.;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin < -ZITC_D4_c-ZDITC_D4/2.) CellZmin = -ZITC_D4_c-ZDITC_D4/2.;
                    if(CellZmax > -ZITC_D4_c+ZDITC_D4/2.) CellZmax = -ZITC_D4_c+ZDITC_D4/2.;
                } // ELSE IF
            } // ELSE IF
            else if(SampleID == 17){
                if( etaInd==25 || etaInd==65 ){
                    CellRmax = RITC_E1_max;
                    CellRmin = RITC_E1_min;
                } // IF
                else if( etaInd==26 || etaInd==66 ){
                    CellRmax = RITC_E2_max;
                    CellRmin = RITC_E2_min;
                } // ELSE IF
                else if( etaInd==27 || etaInd==67 ){
                    CellRmax = RITC_E3_max;
                    CellRmin = RITC_E3_min;
                } // ELSE IF
                else if( etaInd==28 || etaInd==68 ){
                    CellRmax = RITC_E4_max;
                    CellRmin = RITC_E4_min;
                } // ELSE IF
                if( (etaInd>=25 && etaInd<=26) || (etaInd>=65 && etaInd<=66) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E1_a-ZDITC_E1/2.) CellZmin=ZITC_E1_a-ZDITC_E1/2.;
                        if(CellZmax > ZITC_E1_a+ZDITC_E1/2.) CellZmax=ZITC_E1_a+ZDITC_E1/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E1_c-ZDITC_E1/2.) CellZmin= -ZITC_E1_c-ZDITC_E1/2.;
                        if(CellZmax > -ZITC_E1_c+ZDITC_E1/2.) CellZmax= -ZITC_E1_c+ZDITC_E1/2.;
                    } // ELSE
                } // IF
                else if( (etaInd>=27 && etaInd<=28) || (etaInd>=67 && etaInd<=68) ){
                    if(CellEta > 0){
                        if(CellZmin < ZITC_E3_a-ZDITC_E3/2.) CellZmin=ZITC_E3_a-ZDITC_E3/2.;
                        if(CellZmax > ZITC_E3_a+ZDITC_E3/2.) CellZmax=ZITC_E3_a+ZDITC_E3/2.;
                    } // IF
                    else if(CellEta < 0){
                        if(CellZmin < -ZITC_E3_c-ZDITC_E3/2.) CellZmin= -ZITC_E3_c-ZDITC_E3/2.;
                        if(CellZmax > -ZITC_E3_c+ZDITC_E3/2.) CellZmax= -ZITC_E3_c+ZDITC_E3/2.;
                    } // ELSE IF
                } // ELSE
            } // ELSE IF
            else if(SampleID == 12){
                CellRmin = RLBAmin;
                CellRmax = RLBAmax;
                
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 0){
                CellRmin = RLBBmin;
                CellRmax = RLBBmax;
                int cpos = fabs(CellEta)/0.1;
                CellZ = CellZB[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZB[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax > 2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 13 && lBC == 1){
                CellRmin = RLBCmin;
                CellRmax = RLBCmax;
                int cpos = fabs(CellEta)/0.1;
                if(cpos >= 8) return 0;
                CellZ = CellZC[cpos];
                if(CellEta < 0) CellZ=-CellZ;
                CellDZ = CellDZC[cpos];
                CellZmin = CellZ-CellDZ/2;
                CellZmax = CellZ+CellDZ/2;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            else if(SampleID == 14){
                CellRmin = RLBDmin;
                CellRmax = RLBDmax;
                if(CellZmin <-2802.5439) CellZmin=-2802.5439;
                if(CellZmax >2802.5454) CellZmax=2802.5454;
            } // ELSE IF
            if(SampleID == 18){
                CellRmin = REBAmin;
                CellRmax = REBAmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // IF
            } // IF
            if(SampleID == 19){
                CellRmin = REBBmin;
                CellRmax = REBBmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            if(SampleID == 20){
                CellRmin = REBDmin;
                CellRmax = REBDmax;
                if(CellEta > 0){
                    if(CellZmin <3574.5027) CellZmin=3574.4978;
                    if(CellZmax >6130.0039) CellZmax=6130.0039;
                } // IF
                else if(CellEta < 0){
                    if(CellZmin <-6120.0018) CellZmin=-6120.0018;
                    if(CellZmax >-3564.5006) CellZmax=-3564.5006;
                } // ELSE IF
            } // IF
            
            // CALCULATE POINTS OF INTERSECTION
            // INTERSECTIONS R PLANES
            double Radius(CellRmin);
            
            double x0int(exit->position().x()), x1int(entrance->position().x()),
            y0int(exit->position().y()), y1int(entrance->position().y()),
            z0int(exit->position().z()), z1int(entrance->position().z());
            double S((y1int-y0int)/(x1int-x0int));
            double a(1+S*S), b(2*S*y0int-2*S*S*x0int), c(y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int);
            double x1((-b+sqrt(b*b-4*a*c))/(2*a)), x2((-b-sqrt(b*b-4*a*c))/(2*a));
            double y1(y0int+S*(x1-x0int)), y2(y0int+S*(x2-x0int));
            double S1 = ((z1int-z0int)/(x1int-x0int));//, R1(sqrt(x1*x1+y1*y1)), R2(sqrt(x2*x2+y2*y2));
            double z1(z0int+S1*(x1-x0int)), z2(z0int+S1*(x2-x0int));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X=x2;
                Y=y2;
                Z=z2;
            } // IF

            Phi = acos(X/sqrt(X*X+Y*Y));
            if(Y <= 0) Phi = -Phi;
            R = CellRmin;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X;
                CellYimp[Np]=Y;
                CellZimp[Np]=Z;
                Np=Np+1;
               
            } // IF
            
            Radius = CellRmax;
            
            c  = y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int;
            x1 = ((-b+sqrt(b*b-4*a*c))/(2*a));
            x2 = ((-b-sqrt(b*b-4*a*c))/(2*a));
            y1 = (y0int+S*(x1-x0int));
            y2 = (y0int+S*(x2-x0int));
            z1 = (z0int+S1*(x1-x0int));
            z2 = (z0int+S1*(x2-x0int));
            S1 = ((z1int-z0int)/(x1int-x0int));

            X = x1;
            Y = y1;
            Z = z1;

            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X = x2;
                Y = y2;
                Z = z2;
            } // IF

            Phi=std::acos(X/sqrt(X*X+Y*Y));
            if (Y <= 0) Phi=-Phi;
            R=CellRmax;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                Np=Np+1;
               
            } // IF
            
            // INTERSECTIONS Z PLANES
            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmin;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                   
                } // IF
            } // IF

            if(Np < 2){
                double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);
                Z = CellZmax;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                   
                } // IF
            } // IF

            // INTERSECTIONS PHI PLANES
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimin));
                Y = X*std::tan(CellPhimin);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;                  
                } // IF
            } // IF


//================================This block is changed by Archil======================================
            if(Np < 2){
                double Sxy=(Layer2X-Layer1X)/(Layer2Y-Layer1Y);
                double Sxz=(Layer2X-Layer1X)/(Layer2Z-Layer1Z);
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimax));
                Y = X*std::tan(CellPhimax);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                R = sqrt(X*X+Y*Y);
                Phi=acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimax);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimax);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
//======================================================================================================

            // CALCULATE PATH IF TWO INTERSECTIONS WERE FOUND
            if(Np == 2){
                
                //     float temp_pathl =   sqrt( (CellXimp[0]-CellXimp[1])*(CellXimp[0]-CellXimp[1]) +
                //                                (CellYimp[0]-CellYimp[1])*(CellYimp[0]-CellYimp[1]) +
                //                                (CellZimp[0]-CellZimp[1])*(CellZimp[0]-CellZimp[1]) );
                //     if(temp_pathl>0.){
                //             ATH_MSG_ERROR("IN: " << Layer1X << " , " << Layer1Y << " , " << Layer1Z << " | CELL: " << cell->caloDDE()->x() << " , "  << cell->caloDDE()->y() << " , " << (CellZmin+CellZmax)/2. << " +/- " << CellDZ << " | OUT: " << Layer2X << " , " << Layer2Y << " , " << Layer2Z << " ===> " << temp_pathl);
                //     } // IF
                
                pathl += sqrt( (CellXimp[0]-CellXimp[1])*(CellXimp[0]-CellXimp[1]) +
                               (CellYimp[0]-CellYimp[1])*(CellYimp[0]-CellYimp[1]) +
                               (CellZimp[0]-CellZimp[1])*(CellZimp[0]-CellZimp[1]) );
            } // IF
            
        } // IF
        if(SampleID == 13 && lBC == 0) ++lBC;
        else compute = false;
    } // WHILE (FOR LBBC LAYER)

    return pathl;
} // TrackTools::getPathFromCBNTMethod

// //===========================================================================================================================================
// double TrackTools::getActivePathFromCBNTMethod(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit){
// //==========================================================================================================================================
//     
//     // EXTRAPOLATE MUON TRACKS
//     
//     // CELL VARIABLES
//     double CellEta = cell->caloDDE()->eta();
//     double CellZ = cell->caloDDE()->z();
//     double CellDZ = cell->caloDDE()->dz();
//     double CellZmin = CellZ - CellDZ/2. < -6120.0018 ? -6120.0018 : CellZ - CellDZ/2.;
//     double Sample = cell->caloDDE()->getSampling();
//     double Phi(cell->caloDDE()->phi());
//     
//     // TRACK VARIABLES
//     double X1(exit->position().x()),Y1(exit->position().y()),Z1(exit->position().z());
//     double X2(entrance->position().x()),Y2(entrance->position().y()),Z2(entrance->position().z());
//     double X1p(X1*cos(Phi)+Y1*sin(Phi)), Y1p(Y1*cos(Phi)-X1*sin(Phi)), Z1p(Z1);
//     double X2p(X2*cos(Phi)+Y2*sin(Phi)), Y2p(Y2*cos(Phi)-X2*sin(Phi)), Z2p(Z2);
//     double Xmin(Z1p<Z2p?X1p:X2p), Ymin(Z1p<Z2p?Y1p:Y2p), Zmin(Z1p<Z2p?Z1p:Z2p);
//     double Xmax(Z1p<Z2p?X2p:X1p), Ymax(Z1p<Z2p?Y2p:Y1p), Zmax(Z1p<Z2p?Z2p:Z1p);
//     
//     // TILECAL GEOMETRY
//     double HPerLB=9.12881;
//     double MasterLB=5.043541436;
//     double SpacerLB=4.085268564;
//     double HPerEB=9.12679;
//     double MasterEB=5.042425414;
//     double SpacerEB=4.084364586;
//     
//     double HPer(0), Master(0), Spacer(0);
//     double Scint=3.0;
//     
//     if(Sample < 15){
//         HPer = HPerLB;
//         Master = MasterLB;
//         Spacer = SpacerLB;
//     } // IF
//     else{
//         HPer = HPerEB;
//         Master = MasterEB;
//         Spacer = SpacerEB;
//     } // ELSE
//     
//     int CellIntPosALB[10] = {307, 334, 361, 389, 417, 337, 478, 519, 545, 582};
//     int CellIntNegALB[10] = {280, 253, 225, 197, 167, 136, 104, 69, 32, 0};
//     int CellIntPosBLB[9] = {307, 338, 369, 400, 433, 466, 502, 539, 579};
//     int CellIntNegBLB[9] = {276, 245, 214, 181, 148, 112, 75, 35, 0};
//     int CellIntPosCLB[8] = {307, 342, 378, 414, 452, 490, 531, 574};
//     int CellIntNegCLB[8] = {272, 236, 200, 162, 124, 83, 40, 0};
//     int CellIntPosAEB[5] = {0, 18, 68, 124, 184};
//     int CellIntNegAEB[5] = {262, 212, 156, 96, 0};
//     int CellIntPosBEB[5] = {0, 32, 86, 146, 210};
//     int CellIntNegBEB[5] = {248, 194, 134, 70, 0};
//     int CellIntPosDEB[2] = {0, 130};
//     int CellIntNegDEB[2] = {150, 0};
//     
//     unsigned int NTile(0);
//     double ZTile(0);
//     double PathSc(0), PathScInt(0);
//     
//     double SXmin[11]={2301.5, 2401.5, 2501.5, 2601.5, 2731.5, 2861.5, 2991.5, 3141.5, 3291.5, 3441.5, 3631.5};
//     double SXmax[11]={2398.5, 2498.5, 2598.5, 2728.5, 2858.5, 2988.5, 3138.5, 3288.5, 3438.5, 3628.5, 3818.5};
//     double SYmin[11]={109.565, 114.478, 119.391, 124.303, 130.690, 137.076, 143.463, 150.832, 158.201, 165.570, 174.900};
//     double SYmax[11]={114.331, 119.243, 124.156, 130.543, 136.929, 143.316, 150.685, 158.054, 165.423, 174.757, 184.091};
//     double SZmin, SZmax;
//     double Syx, Szx, Sxz, Syz, S;
//     double x, y, z;
//     double xlow, xhigh, ylow, yhigh, zlow, zhigh;
//     double Ximp[2] = {0., 0.};
//     double Yimp[2] = {0., 0.};
//     double Zimp[2] = {0., 0.};
//     
//     if(Sample == 18 || Sample == 19 || Sample == 20 ){
//         if(CellEta > 0 && CellZmin < 3574.4978) CellZmin=3574.4978;
//         ZTile=CellZmin+(Spacer-Scint)/2;
//     } // IF
//     else if(Sample == 12 || Sample == 13 || Sample == 14 ){
//         if(CellZmin < -2802.5439) CellZmin=-2802.5439;
//         ZTile=CellZmin+Master/2+(Spacer-Scint)/2;
//     } // ELSE IF
//     
//     // COMPUTE ACTIVE PATH
//     bool compute = true;
//     int lBC = 0;
//     while(compute){
//         
//         // DETERMINE NUMBER OF TILES
//         if(Sample < 14){
//             int cpos=(fabs(CellEta))/0.1;
//             if(Sample == 12){
//                 if(CellEta > 0) NTile=CellIntPosALB[cpos];
//                 if(CellEta < 0) NTile=CellIntNegALB[cpos];
//             } // IF
//             else if(Sample == 13 && lBC == 0){
//                 if(CellEta > 0) NTile=CellIntPosBLB[cpos];
//                 if(CellEta < 0) NTile=CellIntNegBLB[cpos];
//             } // ELSE
//             if(Sample == 13 && lBC == 1){
//                 if(CellEta > 0) NTile=CellIntPosCLB[cpos];
//                 if(CellEta < 0) NTile=CellIntNegCLB[cpos];
//             } // IF
//         } // IF
//         else if(Sample == 14){
//             if(CellEta < -0.5) NTile=0;
//             if(CellEta > -0.5 && CellEta < -0.3) NTile=100;
//             if(CellEta > -0.3 && CellEta < -0.1) NTile=186;
//             if(CellEta > -0.1 && CellEta < 0.1) NTile=267;
//             if(CellEta > 0.1 && CellEta < 0.3) NTile=347;
//             if(CellEta > 0.3 && CellEta < 0.5) NTile=428;
//             if(CellEta > 0.5) NTile=514;
//         } // ELSE IF
//         else if(Sample == 18){
//             int cpos=(fabs(CellEta)-1.1)/0.1;
//             if(CellEta > 0) NTile=CellIntPosAEB[cpos];
//             if(CellEta < 0) NTile=CellIntNegAEB[cpos];
//         } // ELSE IF
//         else if(Sample == 19){
//             int cpos=(fabs(CellEta)-1.0)/0.1;
//             if(CellEta > 0) NTile=CellIntPosBEB[cpos];
//             if(CellEta < 0) NTile=CellIntNegBEB[cpos];
//         } // ELSE IF
//         else if(Sample == 20){
//             int cpos=(fabs(CellEta)-0.9)/0.2;
//             if(CellEta > 0) NTile=CellIntPosDEB[cpos];
//             if(CellEta < 0) NTile=CellIntNegDEB[cpos];
//         } // ELSE IF
//         
//         // DETERMINE THE FIRST TILE THAT IS CROSSED
//         while(ZTile+Scint < Zmin){
//             ++NTile;
//             ZTile = ZTile + HPer;
//         } // WHILE
//         
//         while(ZTile < Zmax){
//             SZmin=ZTile;
//             SZmax=SZmin+3.;
//             int nmin(0),nmax(0),nstep(0);
//             Syx=(Ymax-Ymin)/(Xmax-Xmin);
//             Szx=(Zmax-Zmin)/(Xmax-Xmin);
//             switch(int(NTile%2)){
//                 case 0:
//                     switch(int(Sample)){
//                         case 12: nmin=0;  nmax=3;  nstep=2; break;
//                         case 13: if(lBC==0){nmin=4;  nmax=5;  nstep=2;}
//                             if(lBC==1){nmin=6;  nmax=9;  nstep=2;}
//                             break;
//                         case 14: nmin=10; nmax=11; nstep=2; break;
//                         case 18: if(CellZmin<0){ nmin=0; nmax=3;  nstep=2;}
//                             if(CellZmin>0){ nmin=1; nmax=2;  nstep=2;}
//                             break;
//                         case 19: if(CellZmin<0){ nmin=4; nmax=7;  nstep=2;}
//                             if(CellZmin>0){ nmin=3; nmax=6;  nstep=2;}
//                             break;
//                         case 20: if(CellZmin<0){ nmin=8; nmax=11; nstep=2;}
//                             if(CellZmin>0){ nmin=7; nmax=10; nstep=2;}
//                             break;
//                     } // SWITCH
//                     break;
//                 case 1:
//                     switch(int(Sample)){
//                         case 12: nmin=1;  nmax=2;  nstep=2; break;
//                         case 13: if(lBC==0){nmin=3;  nmax=6;  nstep=2;}
//                             if(lBC==1){nmin=7;  nmax=9;  nstep=2;}
//                             break;
//                         case 14: nmin=9;  nmax=10; nstep=2; break;
//                         case 18: if(CellZmin<0){ nmin=1; nmax=2;  nstep=2;}
//                             if(CellZmin>0){ nmin=0; nmax=3;  nstep=2;}
//                             break;
//                         case 19: if(CellZmin<0){ nmin=3; nmax=6;  nstep=2;}
//                             if(CellZmin>0){ nmin=4; nmax=7;  nstep=2;}
//                             break;
//                         case 20: if(CellZmin<0){ nmin=7; nmax=10; nstep=2;}
//                             if(CellZmin>0){ nmin=8; nmax=11; nstep=2;}
//                             break;
//                     } // SWITCH
//                     break;
//             } // SWITCH
//             
//             PathSc = 0;
//             // DETERMINE FOR EACH SCINTILLATOR INTERSECTIONS WITH THE TRACK
//             for(int n=nmin;n<nmax;n+=nstep){
//                 int Np = 0;
//                 
//                 // INTERSECTION WITH MAX X PLANE
//                 x = SXmax[n];
//                 y = Ymin + Syx*(x-Xmin);
//                 z = Zmin + Szx*(x-Xmin);
//                 ylow = -SYmax[n];
//                 yhigh = SYmax[n];
//                 zlow = SZmax;
//                 zhigh = SZmax;
//                 if(y >= ylow && y <= yhigh && z >= zlow && z <= zhigh){
//                     Ximp[Np]=x;
//                     Yimp[Np]=y;
//                     Zimp[Np]=z;
//                     Np=Np+1;
//                 } // IF
//                 
//                 // INTERSECTION WITH MIN X PLANE
//                 x=SXmin[n];
//                 ylow = -SYmin[n];
//                 yhigh = SYmin[n];
//                 zlow = SZmin;
//                 zhigh = SZmin;
//                 if(y >= ylow && y <= yhigh && z >= zlow && z <= zhigh){
//                     Ximp[Np]=x;
//                     Yimp[Np]=y;
//                     Zimp[Np]=z;
//                     Np=Np+1;
//                 } // IF
//                 
//                 // INTERSECTION WITH MAX Z PLANE
//                 if(Np<2){
//                     Syz = (Ymax-Ymin)/(Zmax-Zmin);
//                     Sxz = 1/Szx;
//                     z = SZmax;
//                     y = Ymin + Syz*(z-Zmin);
//                     x = Xmin + Sxz*(z-Zmin);
//                     xlow = SXmin[n];
//                     xhigh = SXmax[n];
//                     ylow = -SYmin[n]+(-SYmax[n]+SYmin[n])/(SXmax[n]-SXmin[n])*(x-SXmin[n]);
//                     yhigh = SYmin[n]+(SYmax[n]-SYmin[n])/(SXmax[n]-SXmin[n])*(x-SXmin[n]);
//                     if(x >= xlow && x <= xhigh && y >= ylow && y <= yhigh){
//                         Ximp[Np]=x;
//                         Yimp[Np]=y;
//                         Zimp[Np]=z;
//                         Np=Np+1;
//                     } // IF
//                 } // IF
//                 
//                 // INTERSECTION WITH MIN Z PLANE
//                 if(Np<2){
//                     z = SZmin;
//                     y = Ymin + Syz*(z-Zmin);
//                     x = Xmin + Sxz*(z-Zmin);
//                     xlow = SXmin[n];
//                     xhigh = SXmax[n];
//                     ylow = -SYmin[n]+(-SYmax[n]+SYmin[n])/(SXmax[n]-SXmin[n])*(x-SXmin[n]);
//                     yhigh = SYmin[n]+(SYmax[n]-SYmin[n])/(SXmax[n]-SXmin[n])*(x-SXmin[n]);
//                     if(x >= xlow && x <= xhigh && y >= ylow && y <= yhigh){
//                         Ximp[Np]=x;
//                         Yimp[Np]=y;
//                         Zimp[Np]=z;
//                         Np=Np+1;
//                     } // IF
//                 } // IF
//                 
//                 // INTERSECTION WITH MAX PHI PLANE
//                 if(Np<2){
//                     S = (SXmax[n]-SXmin[n])/(SYmax[n]-SYmin[n]);
//                     x = (SXmin[n]+S*Ymin-S*Syx*Xmin-S*SYmin[n])/(1-S*Syx);
//                     y = Ymin+Syx*(x-Xmin);
//                     z = Zmin+Szx*(x-Xmin);
//                     xlow = SXmin[n];
//                     xhigh = SXmax[n];
//                     zlow = SZmin;
//                     zhigh = SZmax;
//                     
//                     if(x >= xlow && x <= xhigh && z >= zlow  && z <= zhigh){
//                         Ximp[Np]=x;
//                         Yimp[Np]=y;
//                         Zimp[Np]=z;
//                         Np=Np+1;
//                     } // IF
//                 } // IF
//                 
//                 // INTERSECTION WITH MIN PHI PLANE
//                 if(Np<2){
//                     S = (SXmax[n]-SXmin[n])/(-SYmax[n]+SYmin[n]);
//                     x = (SXmin[n]+S*Ymin-S*Syx*Xmin+S*SYmin[n])/(1-S*Syx);
//                     y = Ymin+Syx*(x-Xmin);
//                     z = Zmin+Szx*(x-Xmin);
//                     xlow = SXmin[n];
//                     xhigh = SXmax[n];
//                     zlow = SZmin;
//                     zhigh = SZmax;
//                     
//                     if(x >= xlow && x <= xhigh && z >= zlow  && z <= zhigh){
//                         Ximp[Np]=x;
//                         Yimp[Np]=y;
//                         Zimp[Np]=z;
//                         Np=Np+1;
//                     } // IF
//                 } // IF
//                 
//                 if(Np==2){
//                     PathSc += sqrt( (Ximp[0]-Ximp[1])*(Ximp[0]-Ximp[1]) +
//                                    (Yimp[0]-Yimp[1])*(Yimp[0]-Yimp[1]) +
//                                    (Zimp[0]-Zimp[1])*(Zimp[0]-Zimp[1]) );
//                 } // IF
//             } // FOR
//             
//             PathScInt += PathSc;
//             PathSc = 0.;
//             ++NTile;
//             ZTile = ZTile + HPer;
//         } // WHILE
//         if(Sample == 13 && lBC == 0) ++lBC;
//         else compute = false;
//     } // WHILE (FOR LBBC LAYER)
//     
//     return PathScInt;
//     
// } // TrackTools::getActivePathFromCBNTMethod

//===================================================
void TrackTools::dumpGrandKids(const PVConstLink pv){
//==================================================
    
    
    if(pv ){
        const GeoLogVol* gv = pv->getLogVol();
        if(gv != NULL ){
            ATH_MSG_ERROR("=> DUMPING GRAND KIDS OF " << gv->getName() );
            for(uint i=0;i<pv->getNChildVols();++i){
                dumpKids(pv->getChildVol(i));
            } // FOR
        } // IF
    } // IF
} // TrackTools::dumpGrandKids

//==============================================
void TrackTools::dumpKids(const PVConstLink pv){
//=============================================
    
    
    if(pv ){
        const GeoLogVol* gv = pv->getLogVol();
        if(gv != NULL ){
            ATH_MSG_ERROR("===> DUMPING KIDS OF " << gv->getName() );
            for(uint i=0;i<pv->getNChildVols();++i){
                dump(pv->getChildVol(i));
            } // FOR
        } // IF
    } // IF
} // TrackTools::dumpKids

//==========================================
void TrackTools::dump(const PVConstLink pv){
//=========================================
    
    
    if(pv ){
        const GeoLogVol* gv = pv->getLogVol();
        if(gv ){
            FindVolAction fva(pv,gv->getName(),-1);
            pv->apply(&fva);
            Transform3D geoTransform = fva.getGlobalTransform();
            double geoX     = geoTransform.getTranslation().x();
            double geoY     = geoTransform.getTranslation().y();
            double geoZ     = geoTransform.getTranslation().z();
            double geoTheta = geoTransform.getTranslation().theta();
            double geoPhi   = geoTransform.getTranslation().phi();
            ATH_MSG_ERROR("======>>> " << gv->getName() << " INFORMATION: " << geoX     << " , " << geoY   << " , " << geoZ << " , " 
                          << geoTheta << " , " << geoPhi );
        } // IF
    } // IF
} // TrackTools::dump

//====================================================
bool TrackTools::check(double a, double A, double dA){
//===================================================
    return ( ( a < A + dA/2. ) && ( a > A - dA/2. ) );
} // TrackTools::insideInterval


//======================================================================
void TrackTools::getCellsWithinConeAroundTrack(const Trk::Track* track, 
                                               const CaloCellContainer* input,
                                               ConstDataVector<CaloCellContainer>* output, 
                                               double cone,
                                               bool includelar){
//======================================================================

    // EXTRAPOLATION AND SANITY TEST
    if(cone>0 && track && input){
    std::vector<Trk::TrackParameters*> track_extrapolations;
    for(unsigned int l=0 ; l<21; ++l){
        if(l<12 && !includelar){
            track_extrapolations.push_back(0);
            continue;
        } // IF
        else{
            const Trk::TrackParameters *par1 = m_toCalo->extrapolate( *track,
                                                                     static_cast<CaloCell_ID::CaloSample>(l),
                                                                     0.0,Trk::alongMomentum );
            const Trk::TrackParameters *par2 = (!par1) ? m_toCalo->extrapolate( *track,
                                                                               static_cast<CaloCell_ID::CaloSample>(l),
                                                                               0.0,Trk::oppositeMomentum ) : par1;
            if(par1) track_extrapolations.push_back(const_cast<Trk::TrackParameters*>(par1));
            else if(par2) track_extrapolations.push_back(const_cast<Trk::TrackParameters*>(par2));
            else track_extrapolations.push_back(0);
        } // ELSE 
    } // FOR

    // CELLS WITHIN CONE AROUND TRACK
    CaloCellContainer::const_iterator icell = input->begin();
    CaloCellContainer::const_iterator end   = input->end();
    for(;icell!=end;++icell){
        const CaloCell* cell = (*icell);
        const CaloDetDescrElement* dde = cell->caloDDE();
        int sampling = dde->getSampling();

        // REASONS TO SKIP THIS CELL OR BREAK THE LOOP
        if(!dde) continue;
        if(!track_extrapolations[sampling]) continue;
        if(!includelar && dde->getSubCalo() == CaloCell_ID::LAREM) continue;
        if(dde->getSubCalo() != CaloCell_ID::LAREM && dde->getSubCalo() != CaloCell_ID::TILE) continue;

        // COMPUTE DIFFERENCE IN ETA/PHI
//        double deltaphi = fabs( cell->phi()-track_extrapolations[sampling]->position().phi() );//fabs(phidiff(cell->phi(),track_extrapolations[sampling]->position().phi()));
//        double deltaeta = fabs(cell->eta() - track_extrapolations[sampling]->position().eta());
//        double deltaR   = sqrt(deltaphi*deltaphi+deltaeta*deltaeta);

        double deltaR = KinematicUtils::deltaR(cell->eta(),track_extrapolations[sampling]->position().eta(), cell->phi(),track_extrapolations[sampling]->position().phi() );

        if(deltaR<=cone) output->push_back(cell);
    } // FOR
    } // IF

} // TrackTools::getCellsWithinConeAroundTrack





//==========================================================================================================
std::vector< double > TrackTools::getXYZEtaPhiInCellSampling( double exit[], const Trk::Track* track, const CaloCell *cell){
//==========================================================================================================
    std::vector<double> coordinates;

    // GET CELL DESCRIPTOR AND SAMPLING
    if(!cell || !track) return coordinates;
    const CaloDetDescrElement* dde = cell->caloDDE();
    if(!dde) return coordinates;
    int sampling = dde->getSampling();

    int sampling_entrance = 0;
    int sampling_exit     = 0;
    switch(sampling){
        case 12: sampling_entrance = 12; sampling_exit = 14; break;
        case 13: sampling_entrance = 12; sampling_exit = 14; break;
        case 14: sampling_entrance = 12; sampling_exit = 14; break;
        case 15: sampling_entrance = 13; sampling_exit = 14; break;
        case 16: sampling_entrance = 12; sampling_exit = 13; break;
        case 17: sampling_entrance = 18; sampling_exit = 19; break;
        case 18: sampling_entrance = 18; sampling_exit = 20; break;
        case 19: sampling_entrance = 18; sampling_exit = 20; break;
        case 20: sampling_entrance = 18; sampling_exit = 20; break;
        default: return coordinates;
    } // SWITCH


    // DO EXTRAPOLATION
    const Trk::TrackParameters *par1_entrance = m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_entrance),
                                                                       0.0,Trk::alongMomentum );
    const Trk::TrackParameters *par2_entrance = (!par1_entrance) ?
                                                m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_entrance),
                                                                       0.0,Trk::oppositeMomentum ) : par1_entrance;
    const Trk::TrackParameters *par1_exit =     m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_exit),
                                                                       0.0,Trk::alongMomentum );
    const Trk::TrackParameters *par2_exit =     (!par1_exit) ?
                                                m_toCalo->extrapolate( *track,
                                                                       static_cast<CaloCell_ID::CaloSample>(sampling_exit),
                                                                       0.0,Trk::oppositeMomentum ) : par1_exit;

    if(!par1_entrance && !par2_entrance) return coordinates;
    if(!par1_exit     && !par2_exit)     return coordinates;
    const Trk::TrackParameters *pars_entrance = par1_entrance ? par1_entrance : par2_entrance;
    const Trk::TrackParameters *pars_exit     = par1_exit     ? par1_exit     : par2_exit;

    // STORE TRACK PARAMETERS AND RETURN
    coordinates.push_back(pars_entrance->position().x());
    coordinates.push_back(pars_entrance->position().y());
    coordinates.push_back(pars_entrance->position().z());
    coordinates.push_back(pars_entrance->position().eta());
    coordinates.push_back(pars_entrance->position().phi());

    exit[0]=pars_exit->position().x();
    exit[1]=pars_exit->position().y();
    exit[2]=pars_exit->position().z();
    exit[3]=pars_exit->position().eta();
    exit[4]=pars_exit->position().phi();
    return coordinates;
    
} // getXYZEtaPhiInCellSampling








//==========================================================================================================
std::vector< double > TrackTools::getXYZEtaPhiInCellSampling(const Trk::Track* track, const CaloCell *cell){
//==========================================================================================================
    std::vector<double> coordinates;

    // GET CELL DESCRIPTOR AND SAMPLING
    if(!cell || !track) return coordinates;
    const CaloDetDescrElement* dde = cell->caloDDE();
    if(!dde) return coordinates;
    int sampling = dde->getSampling();

    // DO EXTRAPOLATION
    const Trk::TrackParameters *par1 = m_toCalo->extrapolate( *track,
                                                              static_cast<CaloCell_ID::CaloSample>(sampling),
                                                              0.0,Trk::alongMomentum );
    const Trk::TrackParameters *par2 = (!par1) ? m_toCalo->extrapolate( *track,
                                                                        static_cast<CaloCell_ID::CaloSample>(sampling),
                                                                        0.0,Trk::oppositeMomentum ) : par1;
    if(!par1 && !par2) return coordinates;
    const Trk::TrackParameters *pars = par1 ? par1 : par2;

    // STORE TRACK PARAMETERS AND RETURN
    coordinates.push_back(pars->position().x());
    coordinates.push_back(pars->position().y());
    coordinates.push_back(pars->position().z());
    coordinates.push_back(pars->position().eta());
    coordinates.push_back(pars->position().phi());
    return coordinates;
    
} // getXYZEtaPhiInCellSampling

//==================================================================================================
std::vector< double > TrackTools::getXYZEtaPhiInCellSampling(const Trk::Track* track, int sampling){
//==================================================================================================
    std::vector<double> coordinates;

    // DO EXTRAPOLATION
    const Trk::TrackParameters *par1 = m_toCalo->extrapolate( *track,
                                                              static_cast<CaloCell_ID::CaloSample>(sampling),
                                                              0.0,Trk::alongMomentum );
    const Trk::TrackParameters *par2 = (!par1) ? m_toCalo->extrapolate( *track,
                                                                        static_cast<CaloCell_ID::CaloSample>(sampling),
                                                                        0.0,Trk::oppositeMomentum ) : par1;
    if(!par1 && !par2) return coordinates;
    const Trk::TrackParameters *pars = par1 ? par1 : par2;

    // STORE TRACK PARAMETERS AND RETURN
    coordinates.push_back(pars->position().x());
    coordinates.push_back(pars->position().y());
    coordinates.push_back(pars->position().z());
    coordinates.push_back(pars->position().eta());
    coordinates.push_back(pars->position().phi());
    return coordinates;

} // getXYZEtaPhiInCellSampling






//======================================================================
double TrackTools::getEnergyLArCone(const Trk::Track* track, double cone){
//======================================================================

    //retrieve the cell container
    const CaloCellContainer* cellcoll;
    StatusCode sc = m_storeGate->retrieve(cellcoll, m_cellContainerName);
    if(sc.isFailure() )
    {
     	ATH_MSG_WARNING( "Unable to retrieve the cell container  " << m_cellContainerName );
        return -1;
    }


    double Energy_In_LAr_cone=0;
    
    if(cone>0 && track){
    std::vector<Trk::TrackParameters*> track_extrapolations;
    for(unsigned int l=0 ; l<12; ++l){
      const Trk::TrackParameters *par1 = m_toCalo->extrapolate( *track,
                                                               static_cast<CaloCell_ID::CaloSample>(l),
                                                                     0.0,Trk::alongMomentum );
      const Trk::TrackParameters *par2 = (!par1) ? m_toCalo->extrapolate( *track,
                                                                               static_cast<CaloCell_ID::CaloSample>(l),
                                                                               0.0,Trk::oppositeMomentum ) : par1;
      if(par1) track_extrapolations.push_back(const_cast<Trk::TrackParameters*>(par1));
      else if(par2) track_extrapolations.push_back(const_cast<Trk::TrackParameters*>(par2));
      else track_extrapolations.push_back(0);
    } // FOR

    // CELLS WITHIN CONE AROUND TRACK
    CaloCellContainer::const_iterator icell = cellcoll->begin();
    CaloCellContainer::const_iterator end   = cellcoll->end();
    for(;icell!=end;++icell){
        const CaloCell* cell = (*icell);
        const CaloDetDescrElement* dde = cell->caloDDE();
        int sampling = dde->getSampling();

        // REASONS TO SKIP THIS CELL OR BREAK THE LOOP
        if(!dde) continue;
        if(dde->getSubCalo() != CaloCell_ID::LAREM)continue;
        if(!track_extrapolations[sampling]) continue;
        
        if ( KinematicUtils::deltaR( track_extrapolations[sampling]->position().eta(),dde->eta(),
                                     track_extrapolations[sampling]->position().phi(),dde->phi()) < cone )
           Energy_In_LAr_cone+=cell->energy();
    } // FOR
    } // IF



    return Energy_In_LAr_cone; 

}



double TrackTools::phidiff(double phi1, double phi2){
     // FEW CONSTANTS
     double phi_min = -2.*asin(1.);
     double twopi   = 4.*asin(1.);
     double phi_max = phi_min + twopi;

     // COMPUTE PHI DIFFERENCE
     double ph1     = phi1 < phi_min ? phi1 + twopi : ( phi1 > phi_max ? phi1 - twopi : phi1 );
     double ph2     = phi2 < phi_min ? phi2 + twopi : ( phi2 > phi_max ? phi2 - twopi : phi2 );
     double res     = ph1-ph2;
     return           res  < phi_min ? res + twopi  : ( res  > phi_max ?  res - twopi : res );
} // TrackTools::phidiff

