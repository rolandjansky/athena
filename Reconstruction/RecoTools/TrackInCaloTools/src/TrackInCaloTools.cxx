/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
//
#include "CaloUtils/CaloCellList.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileCellDim.h"
//
#include "FourMomUtils/P4Helpers.h"
//
#include "TrackInCaloTools/TrackInCaloTools.h"
#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
//
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

#define CONE_NxM 0.35 // Cone to contain 3x3 cells of any layer. Largest cell is 0.2 x 0.1 --> sqrt(0.3**2 + 0.15**2)

/////////////////////////////////////////////////////////
TrackInCaloTools::TrackInCaloTools (const std::string&type, const std::string&name, const IInterface*parent)
  :AthAlgTool(type, name, parent),
   m_cellNoiseRMSCut(3.4),
   m_cellNoiseRMSCutLoose(2.0),
   m_useExtrapolation(true),
   m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault"),
   m_negativeCells(false),
   m_cosmics(false),
   m_calibCrossed(false)
{
  declareInterface<ITrackInCaloTools>(this);
  declareInterface<IEnergyDepositionTool>(this);

  m_eCoreDef.push_back("0.05");
  
  declareProperty ("useNegativeCells", m_negativeCells);
  declareProperty ("cellNoiseRMSCut", m_cellNoiseRMSCut);
  declareProperty ("cellNoiseRMSCutLoose", m_cellNoiseRMSCutLoose);
  declareProperty ("useExtrapolation",m_useExtrapolation);
  declareProperty ("NoiseTool", m_noiseTool );
  declareProperty ("CaloCellContainer", m_cellContainerName="AllCalo");
  declareProperty ("EcoreDefinition", m_eCoreDef);
  declareProperty ("EcoreCalibWithPath", m_calibCrossed=false);
  declareProperty ("Cosmics", m_cosmics=false);

  m_currentTrackParameters = 0;
  m_eCore = 0;
}
//
TrackInCaloTools::~TrackInCaloTools()
{
  delete m_eCore;
}
// ------------------------------------------------------------------ 
StatusCode TrackInCaloTools::initialize()
{
  // Retrieve the Messaging Service
  
  StatusCode sc = StatusCode::SUCCESS;
 
  ATH_MSG_INFO( "Initializing TrackInCaloTools" );
  m_noiseToolSc = m_noiseTool.retrieve();
  if (m_noiseToolSc.isFailure())
    {
      ATH_MSG_WARNING( "TrackInCaloTools::Unable to find CaloNoiseTool " );
    }
  else
    {
      ATH_MSG_INFO( "TrackInCaloTools::Calo Noise Tool is selected" );
    } 

  // Get pointer to StoreGateSvc and cache:
  sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()){
    ATH_MSG_FATAL( "TrackInCaloTools::Unable to retrieve pointer to StoreGateSvc" );
    return StatusCode::FAILURE;
  }

  //Get Tool service
  IToolSvc* toolsvc;
  if (service("ToolSvc",toolsvc) == StatusCode::FAILURE)
  {
    ATH_MSG_FATAL( "Could not get tool sevices " );
    return StatusCode::FAILURE;
  }
  
  // Get TrackExtrapolatorToCalo
  sc = toolsvc->retrieveTool("TrackExtrapolatorToCalo", m_trackExtrapolatorToCalo);
  if(sc.isFailure()){
    ATH_MSG_FATAL( "Cannot get TrackExtrapolatorToCalo");
    return StatusCode::FAILURE;
  }

  // Retrieve TileDetDescrManager from the DetectorStore
  StoreGateSvc* detStore = 0;
  if (service("DetectorStore", detStore)  == StatusCode::FAILURE)
  {
    ATH_MSG_FATAL( "Could not get DetectorStore " );
    return StatusCode::FAILURE;
  }
  if ( detStore->retrieve(m_tileDDM) == StatusCode::FAILURE)
  {
    ATH_MSG_FATAL( "Could not get TileDetDescrManager" );
    return StatusCode::FAILURE;
  }  
  
  if (m_cellNoiseRMSCut < m_cellNoiseRMSCutLoose)
  {
    ATH_MSG_WARNING( "cellNoiseRMSCut cannot be smaller than cellNoiseRMSCutLoose" );
    ATH_MSG_INFO( "Setting cellNoiseRMSCutLoose to cellNoiseRMSCut" );
    m_cellNoiseRMSCutLoose = m_cellNoiseRMSCut;
  }

  m_calo_dd = CaloDetDescrManager::instance();
  m_caloCell_ID = m_calo_dd->getCaloCell_ID();
  
  m_subCalos.push_back(CaloCell_ID::LAREM);
  m_subCalos.push_back(CaloCell_ID::LARHEC);
  m_subCalos.push_back(CaloCell_ID::TILE);
  
  m_cone = 0;
  initializeListener(this, name());
  m_eCore = new ConeDefinition("Ecore", m_eCoreDef, this);
  
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------- 
StatusCode  TrackInCaloTools::finalize()
{
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------
/** Clear vectors (cellsOnTrack, cellsPerLayer, extrapolations, P4), max_cone and pointer to current track. Called by listener when event finishes **/
void TrackInCaloTools::m_clear()
{
  ATH_MSG_DEBUG( "Calling m_clear" );
  m_currentTrackParameters = 0;
  m_cone = 0;
  m_cellsOnTrack.clear();
  m_cellsPerLayer.clear();
  m_extrapolations.clear();
  m_weightsForCellsCrossed.clear();
}
//-------------------------------------------------------------------- 
/** Check if given track is the one stored in m_currentTrackParameters. If not, store it **/
bool TrackInCaloTools::newTrack(const Trk::TrackParameters* track)
{
  if (!m_listener)
  {
    ATH_MSG_DEBUG( "Incident Service not active: calling m_clear" );
    m_clear();
  }
  if (!track)
    ATH_MSG_DEBUG( "Null pointer to TrackParameters! " );

  bool new_track = (!m_currentTrackParameters ||  track != m_currentTrackParameters);
  if ( new_track )
  {
    m_clear();
    m_currentTrackParameters = track;
  }
  return new_track;
}
//-------------------------------------------------------------------- 
/** Call TrackExtrapolatorToCalo to get the extrapolations and store it internally. If usePosition is set, use the position of the given TrackParameters (used by measurement) **/
StatusCode TrackInCaloTools::getExtrapolations(const Trk::TrackParameters* track, bool usePosition = false)
{
  if (!track)
  {
    ATH_MSG_DEBUG( "Null pointer to TrackParameters" );
    return StatusCode::FAILURE;
  }
  
  if (usePosition) // use position of given track parameters for all layers
  {
    std::pair<double, double> position = std::make_pair(track->position().eta(), track->position().phi());
    for (int layer = ps; layer <= hec3; layer++)
      m_extrapolations.push_back(position);
  }
  else if (!m_useExtrapolation) // use track direction for all layers
  {
    std::pair<double, double> position = std::make_pair(track->eta(), track->parameters()[Trk::phi]);
    for (int layer = ps; layer <= hec3; layer++)
      m_extrapolations.push_back(position);
  }
  else
    m_extrapolations = m_trackExtrapolatorToCalo->getMidPointForExtrapolations(track);
  
  if ( m_extrapolations.size() == 0)
    return StatusCode::FAILURE;
  ATH_MSG_DEBUG( "Size of extrapolations: " << m_extrapolations.size() );
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------- 
/** Convert the CaloCell_ID into a CaloLayer (integer) which is used internally **/
int TrackInCaloTools::getLayer(const CaloCell *cell)
{
  if (!cell || !cell->caloDDE() )
  {
      ATH_MSG_WARNING( "Invalid cell or caloDDE" );
    return -1;
  }
  return getLayer(cell->caloDDE()->getSampling() );
}
//-------------------------------------------------------------------- 
/** Convert the CaloCell_ID into a CaloLayer (integer) which is used internally **/
int TrackInCaloTools::getLayer(CaloCell_ID::CaloSample calo)
{
  if (calo==CaloCell_ID::PreSamplerB || calo==CaloCell_ID::PreSamplerE)
    return ps;
  else if (calo==CaloCell_ID::EMB1 || calo==CaloCell_ID::EME1)
    return em1;
  else if (calo==CaloCell_ID::EMB2 || calo==CaloCell_ID::EME2)
    return em2;
  else if (calo==CaloCell_ID::EMB3 || calo==CaloCell_ID::EME3)
    return em3;
  else if (calo==CaloCell_ID::TileBar0 || calo==CaloCell_ID::TileExt0)
    return tile1;
  else if (calo==CaloCell_ID::TileBar1 || calo==CaloCell_ID::TileExt1 || calo==CaloCell_ID::TileGap1)
   return tile2;
  else if (calo==CaloCell_ID::TileBar2 || calo==CaloCell_ID::TileExt2 || calo==CaloCell_ID::TileGap2)
    return tile3;
  else if (calo==CaloCell_ID::HEC0)
    return hec0;
  else if (calo==CaloCell_ID::HEC1)
    return hec1;
  else if (calo==CaloCell_ID::HEC2)
    return hec2;
  else if (calo==CaloCell_ID::HEC3)
    return hec3;
  else
  {
      ATH_MSG_DEBUG( "Invalid CaloCell_ID: " << calo );
    return -1;
  }
}
// ------------------------------------------------------------------
/** Store in m_cellsPerLayer the list of cells within deltaR = m_cone.
    Also stores m_cellsOnTrack (the cell touched by the extrapolation) 
    Should be called only by getEnergyAndCells **/
StatusCode TrackInCaloTools::getCellsForCone(double cone)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsForCone. Given cone size: " << cone );
  
  if (cone == 0)
  {
    ATH_MSG_WARNING( "getCellsForCone got 0 as conesize" );
    return StatusCode::FAILURE;
  }
  
  //retrieve the cell container
  const CaloCellContainer* container;
  StatusCode sc = m_storeGate->retrieve(container, m_cellContainerName);
  if ( sc.isFailure() )
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
  for (unsigned int i = 0; i < m_subCalos.size(); i++)
  {
    CaloCell_ID::SUBCALO subCalo = m_subCalos[i];
    
    // Cell preselection is done mid-point between extrapolations on subcalo and the maximum distance as tolerance
    std::vector<double> v = getParametersForCellSelection(subCalo);
    ATH_MSG_DEBUG( "SubCalo: " << i << " eta: " << v[0] << " phi: " << v[1] << " cone: " << v[2] );
    CaloCellList *cellList = new CaloCellList(container,subCalo);
    cellList->select(v[0], v[1], m_cone + v[2] + 0.02);
    ATH_MSG_DEBUG( "SubCalo: " << i << " ncells: " << cellList->ncells() );
    
    // Loop over cells, fill m_cellsOnTrack and m_cellsPerLayer
    for (CaloCellList::list_iterator icell = cellList->begin(); icell != cellList->end(); icell++)
    {
      const CaloCell *cell = (*icell);
      int layer = (int) getLayer(cell);
      if (layer < 0) // invalid layer
        continue;
      
      // Is it above the noise threshold? (true is for loose cut --> negative cells also)
      if ( !accept(cell, true) )
        continue;
      m_cellsPerLayer[layer].push_back(cell);
      // Is it the cell on track ?
      if ( isCellOnTrack(cell, m_extrapolations[layer].first, m_extrapolations[layer].second) )
        m_cellsOnTrack[layer] = cell;
    } // end cell loop
    
    delete cellList;
  } // end subCalo loop

  for (unsigned int i=0; i < m_cellsPerLayer.size(); i++)
    ATH_MSG_DEBUG( "Cells on layer " << i << ": " << m_cellsPerLayer[i].size() );

  return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------
StatusCode TrackInCaloTools::getEnergyAndCells(const Trk::TrackParameters* track,std::vector<double> conesize,std::vector< std::vector<double> > &etot,std::vector< std::vector<ListOfCells> > &usedCells, bool useLooseCut, bool isEt)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getEnergyAndCells" );
  
  unsigned int ncones = conesize.size();
  if (ncones == 0)
  {
    ATH_MSG_WARNING( "getEnergyAndCells: Cone vector is empty, give up  ");
    return StatusCode::FAILURE;
  }
  
  // Sort the conesize vector and get the cells for the largest one
  std::vector<double> sorted_cones = conesize;
  std::sort(sorted_cones.begin(), sorted_cones.end());
  ATH_MSG_DEBUG( "Greatest cone: " << sorted_cones[ncones-1] );
  ATH_MSG_DEBUG( "Current cone: " << m_cone );
  if (sorted_cones[ncones-1] <= 0)
    return StatusCode::FAILURE;

  if (newTrack(track) || sorted_cones[ncones-1] > m_cone)
  {
    if (getExtrapolations(track) == StatusCode::FAILURE ||
      getCellsForCone(sorted_cones[ncones-1]) == StatusCode::FAILURE)
    {
      ATH_MSG_DEBUG( " TrackInCaloTools: Could not get cells for given cone " );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Got cells for cone" );
  }

  etot.clear();
  usedCells.clear();
  etot.resize(ncones);
  usedCells.resize(ncones);
  for (unsigned int i = 0 ;i < ncones; i++)
  {
    usedCells[i].resize(NLAYERS);
    etot[i].resize(NLAYERS);
  }
  
  // Iterate backwards over the (sorted) cones removing the cells that are beyond the cone size
  // Gather total energies and cells for each cone
  std::vector<ListOfCells> *allCells = new std::vector<ListOfCells>(m_cellsPerLayer);
  if (useLooseCut == false)
    for (std::vector<ListOfCells>::iterator it = allCells->begin(); it != allCells->end(); it++)
    {
      selectCells(*it, useLooseCut);
      ATH_MSG_VERBOSE( "Size: " << (*it).size() );
    }
  std::vector<ListOfCells> *lastCells = allCells;

  for (int icone = ncones -1; icone >= 0; icone--)
  {
   // Find the corresponding element in the vector conesize (unsorted)
   int jcone = 0;
   for ( ; jcone < (int) ncones && conesize[jcone] != sorted_cones[icone] ; jcone++) {};
    
    for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
      for (ListOfCells::iterator it = (*lastCells)[ilayer].begin(); it != (*lastCells)[ilayer].end(); it++ )
      {
        const CaloCell *cell = *it;
        if ( P4Helpers::deltaR( *cell, m_extrapolations[ilayer].first, m_extrapolations[ilayer].second ) < conesize[jcone] )
        {
          usedCells[jcone][ilayer].push_back(cell);
          double e = (isEt) ? cell->et() : cell->energy();
          etot[jcone][ilayer] += e;
        }
      }
    lastCells = &usedCells[jcone];
  }
  delete allCells;
  ATH_MSG_DEBUG( "end of getEnergyAndCells" );
  return StatusCode::SUCCESS;
}
// ------------------------------------------------------------------
bool TrackInCaloTools::getMeasuredEnergy(const Trk::TrackParameters* track,const std::vector<double>& dR, std::vector<double>& energies, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut, bool isEt)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getMeasuredEnergy (vector)" );
  if (last_layer < first_layer || first_layer < ps || last_layer > hec3)
  {
    ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
    return false;
  }
  
//   inputs
  std::vector< std::vector<double> > etot;
  std::vector< std::vector< std::vector<const CaloCell*> > > usedCells;//only dummy here
  
  StatusCode sc = getEnergyAndCells(track, dR, etot, usedCells, useLooseCut, isEt);
  if ( sc.isFailure() )
  {
    ATH_MSG_DEBUG( "Calculation of isolation energy failed" );
    return false;
  }
  energies.clear();
  for (unsigned int cone=0;cone<dR.size();cone++)
  {
    double totalEnergy=0.;
    for (int layer = first_layer; layer <= last_layer; layer++)
    {
      totalEnergy += etot[cone][layer];
    }
    energies.push_back(totalEnergy);
  }
  return true;
}
//------------------------------------------------------------------
double TrackInCaloTools::getMeasuredEnergy(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut, bool isEt)
{
  std::vector<double> conesize;
  conesize.push_back(dR);
  std::vector<double> energies;
  if ( !getMeasuredEnergy(track, conesize, energies, first_layer, last_layer, useLooseCut, isEt) || energies.size() == 0)
    return 0;
  return energies[0];
}
// ------------------------------------------------------------------
/** Return a vector with the cells inside the given definition of Ecore or Eloss set by the user **/
std::vector<const CaloCell*> TrackInCaloTools::getCellsForConeDef(const Trk::TrackParameters* track, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut, ConeDefinition *coneDef, bool usePosition=false)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsForconeDef" );
  std::vector<const CaloCell*> cells_coneDef;
  if (last_layer < first_layer || first_layer < ps || last_layer > hec3)
  {
    ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
    return cells_coneDef;
  }

  // Collect the cells for the largest cone to speed-up
  double max_cone = getLargestConeNeeded(coneDef);
  ATH_MSG_DEBUG( "Cone to contain " << coneDef->name << ": " << max_cone );
  if (newTrack(track) || max_cone > m_cone)
    if (getExtrapolations(track, usePosition) == StatusCode::FAILURE ||
        getCellsForCone(max_cone) == StatusCode::FAILURE)
  {
    ATH_MSG_DEBUG( "Could not get cells, returning empty vector for " << coneDef->name );
    return cells_coneDef;
  }

  for (int ilayer = first_layer; ilayer <= last_layer; ilayer++)
  {
    ATH_MSG_DEBUG( "Getting cells for " << coneDef->name << " on layer " << ilayer );
    CaloLayer clayer = (CaloLayer) ilayer;
    if ( coneDef->m_Param.count(ilayer) ) // user wants parametrized eloss, no cell to include
      continue;
    if ( coneDef->m_Crossed.count(ilayer) )
    {
      std::vector<const CaloCell*> cellsCrossed = getCellsCrossedByTrack(track, clayer, clayer, useLooseCut);
      cells_coneDef.insert(cells_coneDef.end(), cellsCrossed.begin(), cellsCrossed.end());
    }
    else if ( coneDef->m_Cone.count(ilayer) )
    {
      std::vector< std::vector<const CaloCell*> > cells = getCellsAroundTrack(track, coneDef->m_Cone[ilayer], clayer, clayer, useLooseCut);
      if ( cells.size() )
        cells_coneDef.insert(cells_coneDef.end(), cells[0].begin(), cells[0].end());
    }
    else if ( coneDef->m_NxM.count(ilayer) )
    {
      std::vector<const CaloCell*> cells1 = getCellsAroundTrackOnLayer(track, clayer, coneDef->m_NxM[ilayer].first, coneDef->m_NxM[ilayer].second, 0, 0, useLooseCut);
      if (cells1.size())
        cells_coneDef.insert(cells_coneDef.end(), cells1.begin(), cells1.end());
    }
    else if ( coneDef->m_ElCone.count(ilayer) )
    {
      std::vector< std::vector<const CaloCell*> > cells2 = getCellsAroundTrack(track, coneDef->m_ElCone[ilayer].first, coneDef->m_ElCone[ilayer].second, clayer, clayer, useLooseCut);
      if (cells2.size())
        cells_coneDef.insert(cells_coneDef.end(), cells2[0].begin(), cells2[0].end());
    }
    else // should never happen
      ATH_MSG_WARNING( "Wrong definition of " << coneDef->name << " for layer " << ilayer );
    ATH_MSG_DEBUG( "Cells on " << coneDef->name << ": " << cells_coneDef.size() );
  }
  return cells_coneDef;
}
// ------------------------------------------------------------------      
/** Get the core energy, corresponding to the energy deposited by a muon track, using 
    the definition of Ecore set by the user **/
double TrackInCaloTools::getEcore(const Trk::TrackParameters* track, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut, bool isEt)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getEcore" );
  double total_energy = 0;
  if (m_eCore->m_Param.size() != NLAYERS)
  {
    std::vector<const CaloCell*> cells = getCellsForEcore(track, first_layer, last_layer, useLooseCut);
    ATH_MSG_DEBUG( "Cells on eCore: " << cells.size() );
    for (std::vector<const CaloCell*>::iterator it = cells.begin(); it != cells.end(); it++)
    {
      unsigned int layer = getLayer(*it);
      double calib = 1.;
      // If requested and using cells crossed, calibrate eCore according to L(cells_crossed)/L(layer), assuming dE/dx is roughly constant
      if (m_calibCrossed && m_eCore->m_Crossed.count(layer) && m_weightsForCellsCrossed[layer] != 0)
        calib = 1./m_weightsForCellsCrossed[layer];
      total_energy += (isEt ? (*it)->et() : (*it)->e() ) * calib;
    }
  }
  
  // Add the parametrized eloss if requested. Change from e to et using eta from the track if needed
  if ( m_eCore->m_Param.size() )
  {
    if (!m_useExtrapolation)
      ATH_MSG_WARNING( "Not using extrapolation, cannot get parametrized eloss" );
    else
    {
      std::vector<double> param_eloss = m_trackExtrapolatorToCalo->getParametrizedEloss(track);
      for (std::map<int, bool>::iterator im = m_eCore->m_Param.begin(); im != m_eCore->m_Param.end(); im++)
//         if ( im->first >= first_layer && im->first <= last_layer )
          total_energy += param_eloss[im->first] * (isEt ? cosh(track->eta()) : 1);
    }
  }
  return total_energy;
}
// ------------------------------------------------------------------      
/** Return a vector with the cells around track for the given cone (dR) divided by layer **/
std::vector<std::vector<const CaloCell*> > TrackInCaloTools::getCellsAroundTrack(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsAroundTrack" );
  //inputs
  std::vector<double> conesize;
  conesize.push_back(dR);
  std::vector< std::vector<double> > etot;
  std::vector< std::vector< std::vector<const CaloCell*> > > theCellsVector;
  std::vector<std::vector<const CaloCell*> > output;

  StatusCode sc = getEnergyAndCells(track, conesize, etot, theCellsVector, useLooseCut);
   if ( sc.isFailure() || theCellsVector.size() == 0)
    {
      ATH_MSG_DEBUG( "No cells around track" );
      return output;
    }
  for (int layer = first_layer; layer <= last_layer; layer++)
    output.push_back(theCellsVector[0][layer]);
  return output;
}
// ------------------------------------------------------------------      
/** Return a vector with the cells around track for the given "rectangular cone" (deta, dphi) divided by layer **/
std::vector<std::vector<const CaloCell*> > TrackInCaloTools::getCellsAroundTrack(const Trk::TrackParameters* track,const double deta,const double dphi, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsAroundTrack (rectangle)" );
  //inputs
  std::vector<double> conesize;
  conesize.push_back(sqrt(deta*deta + dphi*dphi));
  std::vector< std::vector<double> > etot;
  std::vector< std::vector< std::vector<const CaloCell*> > > theCellsVector;
  std::vector<std::vector<const CaloCell*> > output;
  
  if (deta <= 0 || dphi <= 0)
   {
      ATH_MSG_WARNING( "Wrong values passed for deta and/or dphi" );
      return output;
    }
  
  StatusCode sc = getEnergyAndCells(track, conesize, etot, theCellsVector, useLooseCut);
   if ( sc.isFailure() || theCellsVector.size() == 0)
    {
      ATH_MSG_DEBUG( "Could not get cells around track" );
      return output;
    }
  output.resize(last_layer - first_layer + 1);
  for (int i=0, layer=first_layer; layer <= last_layer; layer++, i++) {
    std::vector<const CaloCell*>::iterator it = theCellsVector[0][layer].begin();
    for (; it != theCellsVector[0][layer].end(); it++) {
      const CaloCell *cell = *it;
      double delta_eta = (m_extrapolations[layer].first - cell->eta())/deta;
      double delta_phi = (m_extrapolations[layer].second - cell->phi())/dphi;
//       if (delta_eta*delta_eta + delta_phi*delta_phi < 1)
      if (fabs(delta_eta) < 1 && fabs(delta_phi) < 1)
        output[i].push_back(cell);
    }
  }
  ATH_MSG_DEBUG( "End of getCellsAroundTrack (elliptical cone)" );
  return output;
}
//--------------------------------------------------------------------------------------------
/** Return std::vector<const CaloCell*> with the cells crossed by the track (one per layer) **/
std::vector<const CaloCell*> TrackInCaloTools::getCellsOnTrack(const Trk::TrackParameters* track, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsOnTrack" );
  if ( newTrack(track) || m_cellsOnTrack.size() == 0)
    getMeasuredEnergy(track, 0.5, ps, hec3);
  std::vector<const CaloCell*> cellsOnTrack;
  if (m_cellsOnTrack.size() )
  {
    cellsOnTrack.reserve(m_cellsOnTrack.size());
    for (int layer = first_layer; layer <= last_layer; layer++)
      if ( m_cellsOnTrack[layer] && (useLooseCut || accept(m_cellsOnTrack[layer], useLooseCut) ) )
        cellsOnTrack.push_back(m_cellsOnTrack[layer]);
  }
  return cellsOnTrack;
}
// ------------------------------------------------------------------ 
/** Given a cell and the position of the extrapolation (eta, phi), return true if the extrapolation is inside the cell **/
bool TrackInCaloTools::isCellOnTrack(const CaloCell *cell, double eta, double phi)
{
  return ( fabs(eta - cell->eta()) < cell->caloDDE()->deta()/2. &&
       fabs(CaloPhiRange::diff(phi, cell->phi()) ) < cell->caloDDE()->dphi()/2.);
}
// ------------------------------------------------------------------ 
/** Return true if cell passes the noise threshold **/
bool TrackInCaloTools::accept(const CaloCell *cell, bool useLooseCut)
{ 
  double cellEn_temp = (m_negativeCells || useLooseCut) ? fabs(cell->energy()) : cell->energy();
  double cell_rms_noise = (m_noiseToolSc == StatusCode::SUCCESS) ? m_noiseTool->totalNoiseRMS(cell) : 0; 
  if (useLooseCut)
    return (cellEn_temp > cell_rms_noise*m_cellNoiseRMSCutLoose);
  return (cellEn_temp > cell_rms_noise*m_cellNoiseRMSCut);
}
//-----------------------------------------------------------
/** Given a vector with CaloCells, remove those below the noise threshold **/
void TrackInCaloTools::selectCells(std::vector<const CaloCell*> &cells, bool useLooseCut)
{
  std::vector<const CaloCell*>::iterator it = cells.begin();
  while ( it != cells.end() )
    if ( !accept(*it, useLooseCut) )
      cells.erase(it);
    else
      it++;
}
//-----------------------------------------------------------
/** Return a vector with all the cells crossed by the track (can be more than one per layer) **/
std::vector<const CaloCell*> TrackInCaloTools::getCellsCrossedByTrack(const Trk::TrackParameters* track, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsCrossedByTrack" );
  std::vector<const CaloCell*> cellsCrossed;
  m_weightsForCellsCrossed = std::vector<double>(NLAYERS, 0.);
  if (!m_useExtrapolation)
  {
    ATH_MSG_WARNING( "Extrapolation not requested, cannot get cells crossed" );
    return cellsCrossed;
  }
  std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
  std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
  if (v_entrance.size() == 0 || v_exit.size() == 0)
    return cellsCrossed;
  
  // Define the largest cone that contains all crossed cells
  double max_cone = 0.5;
  for (int layer = first_layer; layer <= last_layer; layer++)
    if (v_entrance[layer] && v_exit[layer])
      max_cone = std::max(max_cone, 0.5*deltaR(v_entrance[layer], v_exit[layer]) );
  ATH_MSG_DEBUG( "Cone to contain cells crossed: " << max_cone ); 
  
  if (newTrack(track) || max_cone > m_cone)
    if (getExtrapolations(track) == StatusCode::FAILURE ||
        getCellsForCone(max_cone) == StatusCode::FAILURE)
  {
    ATH_MSG_DEBUG( " TrackInCaloTools: Could not get cells for given cone " );
    return cellsCrossed;
  }
  
  for (int layer = first_layer; layer <= last_layer; layer++)
  {
    unsigned int ncellsCrossed = cellsCrossed.size();
    if (!v_entrance[layer] || !v_exit[layer])
    {
      ATH_MSG_DEBUG( "No extrapolation for entrance or exit on layer " << layer ); 
      if (m_cellsOnTrack[layer] && accept(m_cellsOnTrack[layer], useLooseCut) )
        cellsCrossed.push_back(m_cellsOnTrack[layer]);
      else
        ATH_MSG_DEBUG( "No cell on track either" );
      continue;
    }
    
    std::vector<const CaloCell*>::iterator it = m_cellsPerLayer[layer].begin();
    for ( ; it != m_cellsPerLayer[layer].end(); it++)
    {
      const CaloCell *cell = *it;
      if (accept(cell, useLooseCut) && cellCrossedByTrack(cell, v_entrance[layer], v_exit[layer]) )
      {
        cellsCrossed.push_back(cell);
        ATH_MSG_DEBUG("Weight for cells crossed: " << m_weightsForCellsCrossed[layer]);
      }
    }
    
    ATH_MSG_DEBUG( "Track crossed " <<  cellsCrossed.size() - ncellsCrossed << " cells in layer " << layer );
    // For 2x2 (or more) the track could not cross all cells, but this should not happen often so I will ignore
    // The right way to detect that would be to check how many cells we have in eta X phi but this is too tedious
    if (cellsCrossed.size() - ncellsCrossed > 3) 
      ATH_MSG_DEBUG( "getCellsCrossed might return more than the actual crossed cells" );
    ATH_MSG_DEBUG("Total weight for cells crossed: " << m_weightsForCellsCrossed[layer]);
    if (m_weightsForCellsCrossed[layer] > 1)
    {
      m_weightsForCellsCrossed[layer] = 1.;
      ATH_MSG_DEBUG("Weight reset to 1");
    }
  } // end of loop over layers
  return cellsCrossed;
}
//------------------------------------------------------------------ 
/** Return the fraction of the path crossed by the track which is covered by the cells collected **/
std::vector<double> TrackInCaloTools::getWeightsForCellsCrossed(const Trk::TrackParameters* track, bool useLooseCut)
{
  if (newTrack(track) || m_weightsForCellsCrossed.size() == 0)
    getCellsCrossedByTrack(track, ps, hec3, useLooseCut);
  return m_weightsForCellsCrossed;
}
//------------------------------------------------------------------ 
/** Return the cells around the track for the given cone, subtracting the cells used for eCore calculation. **/
std::vector< std::vector<const CaloCell*> > TrackInCaloTools::getCellsForIsolation(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getCellsForIsolation" );
  if (last_layer < first_layer || first_layer < ps || last_layer > hec3)
  {
    ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
    return std::vector< std::vector<const CaloCell* > >();
  }
  std::vector< std::vector<const CaloCell* > > cells = getCellsAroundTrack(track, dR, first_layer, last_layer, useLooseCut);
  if (cells.size() == 0)
  {
    ATH_MSG_DEBUG( "Returning 0 as isolation energy" );
    return cells;
  }
  // Using only parametrization for eCore?
  if (m_eCore->m_Param.count(true) == NLAYERS)
    return cells;

  std::vector<const CaloCell*> core_cells = getCellsForEcore(track, first_layer, last_layer, useLooseCut);
  if (core_cells.size() == 0)
    return cells;
  
  removeCoreFromCone(cells, core_cells, first_layer, last_layer);
  return cells;
}
//------------------------------------------------------------------ 
/** Return the isolation energy (Et) for the given cone, subtracting the cells used for eCore calculation. If the parametrization is requested in eCore, subtract the energy if there are cells on the layer. **/
double TrackInCaloTools::getIsolationEnergy(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getIsolationEnergy" );
  if (last_layer < first_layer || first_layer < ps || last_layer > hec3)
  {
    ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
    return 0;
  }
  // Add the parametrized eloss if requested. Change from e to et using eta from the track if needed
  std::vector<double> param_eloss;
  if ( m_eCore->m_Param.size() )
  {
    if (!m_useExtrapolation)
      ATH_MSG_WARNING( "Not using extrapolation, cannot get parametrized eloss" );
    else
      param_eloss = m_trackExtrapolatorToCalo->getParametrizedEloss(track);
  }
  
  double isolationEnergy = 0.;
  std::vector< std::vector<const CaloCell*> > isolation_cells = getCellsForIsolation(track, dR, first_layer, last_layer, useLooseCut);
  if (isolation_cells.size() == 0)
    return isolationEnergy;
  for (unsigned int layer = (unsigned int) first_layer, ilayer = 0; layer <= (unsigned int) last_layer; layer++, ilayer++) {
    if (isolation_cells[ilayer].size() == 0)
      continue;
    for (unsigned int i=0; i < isolation_cells[ilayer].size(); i++)
      isolationEnergy += isolation_cells[ilayer][i]->et();
    if (m_eCore->m_Param.count(layer) && param_eloss.size())
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
std::vector<const CaloCell*> TrackInCaloTools::getNeighbours(const CaloCell *cell, int neta, int nphi, int dir_eta, int dir_phi, bool useLooseCut)
{
  std::vector<const CaloCell*> cells;
  if (!cell)
  {
    ATH_MSG_WARNING( "Null pointer to cell!" );
    return cells;
  }
  cells.push_back(cell);
  if (neta > 3 || nphi > 3)
  {
    ATH_MSG_WARNING( "Cannot make clusters bigger than 3 in each direction, giving up" );
    return cells;
  }
  if (neta != 1 || nphi != 1)
  {
    //retrieve the cell container
    const CaloCellContainer* container;
    StatusCode sc = m_storeGate->retrieve(container, m_cellContainerName);
    if ( sc.isFailure() )
    {
      ATH_MSG_WARNING( "Unable to retrieve the cell container  " << m_cellContainerName );
      return cells;
    }
    
    IdentifierHash id = cell->caloDDE()->calo_hash();
    // Use m_caloCell_ID->get_neighbours to get the identifiers of the neighbours
    // Should be faster than looping over the cells directly
    std::vector<IdentifierHash> ids;
    int option;
    if (neta == 1)
      option = (LArNeighbours::prevInPhi | LArNeighbours::nextInPhi); 
    else if (nphi == 1)
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
//-----------------------------------------------------------
/** For getNeighbours: define the directions if not given and remove cells outside neta, nphi **/
void TrackInCaloTools::selectNeighbours(std::vector<const CaloCell*> &cells, int neta, int nphi, int dir_eta, int dir_phi, double eta, double phi)
{
  ATH_MSG_DEBUG( "In selectNeighbours" );
  if (!cells.size())
    return;
  std::vector<const CaloCell*>::iterator it = cells.begin();
  // If eta, phi are not given by the extrapolation, get from the first cell (the one on the center)
  if (eta < -5) 
  {
    const CaloCell *cell = *it; 
    eta = cell->eta();
    phi = cell->phi();
    it++;
  }
  
  // If the directions are not given, set them to where sumEt is maximal (has to call in any case so they are not 0)
  getDirections(cells, neta, nphi, &dir_eta, &dir_phi, eta, phi);
  ATH_MSG_DEBUG( "Direction for cluster: " << dir_eta << " " << dir_phi );
  
  // Select the cells
  while ( it != cells.end() )
  {
    const CaloCell *cell2 = *it;
    // Distance from reference cell or extrapolation divided by cell size and according to the cluster direction
    int deta = (int) round( (cell2->eta() - eta)/cell2->caloDDE()->deta() )*dir_eta;
    int dphi = (int) round( CaloPhiRange::diff(cell2->phi(), phi )/cell2->caloDDE()->dphi() )*dir_phi;
    // If (cell is outside neta, nphi) or (2 were requested and this is the "negative one") -> remove it
    // Actually this should work for n > 3
    if ( abs(deta) > neta/2 || (neta%2 == 0 && -deta >= neta/2 ) || 
         abs(dphi) > nphi/2 || (nphi%2 == 0 && -dphi >= nphi/2 ) )
      cells.erase(it);
    else
      it++;
  }
  ATH_MSG_DEBUG( "selectNeighbours successful" );
}
//-----------------------------------------------------------
int TrackInCaloTools::sign(double x, double min)
{
  if (fabs(x) < min)
    return 0;
  else if (x>0)
    return 1;
  return -1;
}
//-----------------------------------------------------------
/*int TrackInCaloTools::round(double x)
{
  return int(x > 0.0 ? x + 0.5 : x - 0.5);
}*/
//-----------------------------------------------------------
/** For an even number of cells, define the directions to select the cells (if not given), 
    according to where sumEt is maximal **/
void TrackInCaloTools::getDirections(std::vector<const CaloCell*> cells, int neta, int nphi, int *dir_eta, int *dir_phi, double eta, double phi)
{
  double sumEta = 0;
  double sumPhi = 0;
  if ( (neta == 2 && !(*dir_eta) ) || (nphi == 2 && !(*dir_phi) ) )
  {
    // loop over cells, sum sign(cell.eta - other_cell.eta)*other_cell.et (same for phi)
    // If result is positive, cluster direction is positive
    // Avoids cells in the same eta, phi (distance < 0.5) or further than neta, nphi
    std::vector<const CaloCell*>::iterator it = cells.begin();
    // If eta, phi are not given by the extrapolation, get from the first cell (the one on the center)
    if (eta < -5)
    {
      const CaloCell *cell = cells.front();
      eta = cell->eta();
      phi = cell->phi();
      it++;
    }
    for ( ; it != cells.end(); it++) // start from 2nd cell
    {
      const CaloCell *cell2 = *it;
      // Distance from eta, phi in number of cells
      int deta = (int) round( (cell2->eta() - eta)/cell2->caloDDE()->deta() );
      int dphi = (int) round(CaloPhiRange::diff(cell2->phi(), phi)/cell2->caloDDE()->dphi() );
      if ( deta != 0 && abs(deta) <= neta/2 )
        sumEta += sign( deta )*cell2->et();
      if ( dphi != 0 && abs(dphi) <= nphi/2 ) 
        sumPhi += sign( dphi )*cell2->et();
    }
  }
  if (sumEta == 0) // Set the cluster direction to 1
    sumEta = 1.0;
  if (sumPhi == 0) // Set the cluster direction to 1
    sumPhi = 1.0;
  // Set the directions
  if (! (*dir_eta) )
    *dir_eta = sign(sumEta);
  if (! (*dir_phi) )
    *dir_phi = sign(sumPhi);
}
// ------------------------------------------------------------------ 
/** For IEnergyDepositionTool: given CaloSample, track parameters and a cone, return the energy and its error (rms noise) **/
std::pair<double,double> TrackInCaloTools::measurement(CaloCell_ID::CaloSample sample, const Trk::TrackParameters& track)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::measurement" );
  if (m_cosmics)
  {
    ATH_MSG_DEBUG( "TrackInCaloTools::measurement is not ready for cosmics, returning 0" );
    std::make_pair(0., 0.);
  }
  
  int layer = getLayer(sample);
  if (layer < 0)
  {
    ATH_MSG_WARNING( "Invalid CaloSample" << sample );
    return std::make_pair(0., 0.);
  }
  std::vector<const CaloCell*> cells = getCellsForEcore(&track, (CaloLayer) layer, (CaloLayer) layer, false, true);
  selectCellsBySample(cells, sample);
  ATH_MSG_DEBUG( "Cells selected: " << cells.size() );
  double energy = 0, noise = 0;
  
  for (std::vector<const CaloCell*>::iterator it = cells.begin(); it != cells.end(); it++)
  {
    const CaloCell *cell = *it;
    double cell_rms_noise = (m_noiseToolSc == StatusCode::SUCCESS) ? m_noiseTool->totalNoiseRMS(cell) : 0; 
    noise += cell_rms_noise*cell_rms_noise;
    energy += cell->energy();
  }
  return std::make_pair(energy, sqrt(noise) );
}
//------------------------------------------------------------------ 
/** Return a vector with the neta x nphi cells around the track for given layer.
Try to start from cell on track, otherwise relies that the cell sizes are not very different.
For even clusters the direction has to be given, or it is defined as the one for which sumEt is maximal **/
std::vector<const CaloCell*> TrackInCaloTools::getCellsAroundTrackOnLayer(const Trk::TrackParameters *track, CaloLayer layer, int neta, int nphi, int dir_eta, int dir_phi, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In getCellsAroundTrackOnLayer. Layer " << (int) layer << " cells: " << neta << "x" << nphi );
  if (layer < ps || layer > hec3)
    ATH_MSG_WARNING( "Invalid layer: " << layer );
  else if (neta == 0 && nphi == 0)
    ATH_MSG_WARNING( "No cell requested" );
  else
  {
    if ( newTrack(track) || m_cellsOnTrack.size() == 0)
      getCellsOnTrack(track);
    if (m_cellsOnTrack.size() == 0)
      ATH_MSG_DEBUG( "Could not retrieve cells on track, returning empty vector in getCellsAroundTrack" );
    else if(m_cellsOnTrack[layer])
      return getNeighbours(m_cellsOnTrack[layer], neta, nphi, dir_eta, dir_phi, useLooseCut);
    else // no cell on track for this layer
    {
      ATH_MSG_DEBUG( "No cell on track for layer " << layer );
      // The best solution would be to have the identifiers on track and apply the same method as when the cell is present
      // For the moment it relies on the distance between the cell and the extrapolation divided by the cell size
      // This number is rounded so that [-0.5, 0.5] --> 0, [0.5, 1.5] --> 1, [-0.5, -1.5] --> -1, etc
      // First, estimate the cone size to contain all cells
      if (m_cone < CONE_NxM)
        getMeasuredEnergy(track, CONE_NxM, ps, hec3);
      if (m_cellsPerLayer[layer].size() == 0)
        ATH_MSG_DEBUG( "No cells on layer " << layer 
            << ", returning empty vector in getCellsAroundTrackOnLayer" );
      else
      {
        std::vector<const CaloCell*> cells = m_cellsPerLayer[layer];
        ATH_MSG_DEBUG( "Cells on layer: " << cells.size() );
        if (!useLooseCut)
          selectCells(cells, useLooseCut);
        if ( cells.size() )
          selectNeighbours(cells, neta, nphi, dir_eta, dir_phi, m_extrapolations[layer].first, m_extrapolations[layer].second);
        return cells;
      }
    }
//       ATH_MSG_DEBUG( "No cell on track, returning empty vector" );
  }
  return std::vector<const CaloCell*>();
}
//------------------------------------------------------------------ 
/** Return the % of the path crossed inside the given cell. If one or both the extrapolation fails, return 1 if it is the cell on track **/
double TrackInCaloTools::getPathInsideCell(const Trk::TrackParameters *track, const CaloCell *cell)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getPathInsideCell" );
  int layer;
  if (!cell)
    ATH_MSG_WARNING("Null pointer to cell!");
  else if (!m_useExtrapolation)
    ATH_MSG_WARNING( "Extrapolation not requested, cannot get path inside cell" );
  else if ((layer = getLayer(cell)) >= 0)
  {
    std::vector<const Trk::TrackParameters*> v_entrance = m_trackExtrapolatorToCalo->getExtrapolationsToLayerEntrance(track);
    std::vector<const Trk::TrackParameters*> v_exit = m_trackExtrapolatorToCalo->getExtrapolationsToLayerExit(track);
    if (v_entrance.size() == 0 || v_exit.size() == 0 || !v_entrance[layer] || !v_exit[layer])
    {
      ATH_MSG_DEBUG( "Cannot get extrapolation points in layer" );
      if (newTrack(track) && getExtrapolations(track) == StatusCode::FAILURE)
        return 0;
      if ( isCellOnTrack(cell, m_extrapolations[layer].first, m_extrapolations[layer].second) )
        return 1;
    }
    else
      return pathInsideCell(cell, v_entrance[layer], v_exit[layer]);
  }
  return 0;
}
//------------------------------------------------------------------
/** Return dR = sqrt(deta^2 + dphi^2) for the positions of TrackParameters* a and b **/
double TrackInCaloTools::deltaR(const Trk::TrackParameters *a, const Trk::TrackParameters *b)
{
  double deta = a->position().eta() - b->position().eta();
  double dphi = CaloPhiRange::diff(a->position().phi(), b->position().phi());
  return sqrt( deta*deta + dphi*dphi );
}
//------------------------------------------------------------------ 
/** Return a vector with the mid-point (eta, phi) and the cone to be used on cell preselection for the given subcalo **/
std::vector<double> TrackInCaloTools::getParametersForCellSelection(CaloCell_ID::SUBCALO subCalo)
{
  if (subCalo == CaloCell_ID::LAREM)
    return getMidPointAndDistance(m_extrapolations[ps], m_extrapolations[em3]);
  else if (subCalo == CaloCell_ID::TILE)
    return getMidPointAndDistance(m_extrapolations[tile1], m_extrapolations[tile3]);
  return getMidPointAndDistance(m_extrapolations[hec0], m_extrapolations[hec3]);
}
//------------------------------------------------------------------ 
/** Given two pairs representing two points (eta, phi), return a vector with the mid-point (eta, phi) and the distance between the mid-point and the points **/
std::vector<double> TrackInCaloTools::getMidPointAndDistance(std::pair<double, double> p1, std::pair<double, double> p2)
{
  std::vector<double> x;
  x.push_back( 0.5*(p1.first + p2.first) ); // eta
  x.push_back( phiMean(p1.second, p2.second) ); // phi
  x.push_back( sqrt( square(x[0] - p1.first) + square( CaloPhiRange::diff(x[1], p1.second) ) ) ); // distance between mid-point and p1 (or p2)
  return x;
}
//------------------------------------------------------------------ 
/** Return the largest cone needed to collect the cells for Ecore or Eloss **/
double TrackInCaloTools::getLargestConeNeeded(ConeDefinition *coneDef)
{
  double max_cone = m_cone;
  for (int layer=ps; layer <= hec3; layer++) 
    max_cone = std::max(max_cone, getConeSizeForDef(layer, coneDef) );
  return max_cone;
}
//------------------------------------------------------------------ 
/** Return the cone needed to collect the cells for Ecore or Eloss on given layer **/
double TrackInCaloTools::getConeSizeForDef(int ilayer, ConeDefinition *coneDef)
{
  if ( coneDef->m_Param.count(ilayer) )
    return 0;
  else if ( coneDef->m_Crossed.count(ilayer) )
    return 0.5; // not the right value, but ok
  else if ( coneDef->m_Cone.count(ilayer) )
    return coneDef->m_Cone[ilayer];
  else if ( coneDef->m_NxM.count(ilayer) )
    return CONE_NxM;
  else if ( coneDef->m_ElCone.count(ilayer) )
    return sqrt( square(coneDef->m_ElCone[ilayer].first) + square(coneDef->m_ElCone[ilayer].second) );
  // Should never happen
  ATH_MSG_WARNING( "Wrong definition of " <<  coneDef->name << " for layer " << ilayer );
  return 0;
}
//------------------------------------------------------------------ 
/** Given the cells on a cone (per layer) and the cells on the core, remove the latter from the former **/
void TrackInCaloTools::removeCoreFromCone(std::vector< std::vector< const CaloCell*> > &cells, std::vector< const CaloCell*> core_cells, CaloLayer first_layer, CaloLayer last_layer)
{
  ATH_MSG_DEBUG("In TrackInCaloTools::removeCoreFromCone" );
  if (last_layer < first_layer || first_layer < ps || last_layer > hec3)
  {
    ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
    return;
  }
  // Loop over the selected layers and remove the cells that apper on Etcore
  std::vector<const CaloCell*>::iterator core_cellIt = core_cells.begin();
  for ( ; core_cellIt != core_cells.end(); ++core_cellIt)
  {
    int layer = getLayer(*core_cellIt); // core_cell layer
    int ilayer = layer - first_layer; // index of core_cell layer in the cells vector
    if (layer < 0 || ilayer < 0)
      continue;
    if ( (unsigned int) ilayer >= cells.size() ) 
      break;
    std::vector<const CaloCell*>::iterator cellIt = cells[ilayer].begin();
    for ( ; cellIt != cells[ilayer].end(); ++cellIt)
      if ( (*cellIt) == (*core_cellIt) ) {
        cells[ilayer].erase(cellIt);
        break;
      }
  }
}
//------------------------------------------------------------------
/** Remove from the given vector, cells which do not belong to the given layer **/
void TrackInCaloTools::selectCellsByLayer(std::vector<const CaloCell*> &cells, int layer)
{
  std::vector<const CaloCell*>::iterator it = cells.begin();
  while (it != cells.end())
    if (getLayer(*it) != layer)
      cells.erase(it);
    else 
      ++it;
}
//------------------------------------------------------------------
/** Return the % of the path crossed inside the cell, given the parameters for the extrapolation at entrance and exit of the layer **/
double TrackInCaloTools::pathInsideCell(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit)
{
  if (!crossedPhi(cell, entrance->position().phi(), exit->position().phi()))
    return 0;
  double pathCrossed = 0;
  if (cell->caloDDE()->getSubCalo() != CaloCell_ID::TILE)
    pathCrossed = getPathLengthInEta(cell, entrance->position().eta(), exit->position().eta());
  else if (cell->caloDDE()->getSampling() == CaloCell_ID::TileBar1) // ladder shape cells, divide the problem in 2
    pathCrossed = getPathLengthInZTileBar1(cell, entrance->position().z(), exit->position().z());
  else
    pathCrossed = getPathLengthInZ(cell, entrance->position().z(), exit->position().z());
  if (pathCrossed <= 0)
    return 0;
  return pathCrossed;
}
//------------------------------------------------------------------
/** Return true if the cell was crossed by the track, and calculate the % of the path inside the cell **/
bool TrackInCaloTools::cellCrossedByTrack(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit)
{
  int layer = getLayer(cell);
  if (layer < 0)  
    return false;
  double pathCrossed = pathInsideCell(cell, entrance, exit);
  if (pathCrossed <= 0)
    return false;
  m_weightsForCellsCrossed[layer] += pathCrossed;
  return true;
}
//------------------------------------------------------------------
/** Return true if the cell crossed was crossed by the track in phi **/
bool TrackInCaloTools::crossedPhi(const CaloCell *cell, double phi_entrance, double phi_exit)
{
  double mean_phi = phiMean(phi_entrance, phi_exit);
  double dphi = fabs( CaloPhiRange::diff(phi_entrance, phi_exit) );
  double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
  
  return (CaloPhiRange::diff(cell->phi() + cell->caloDDE()->dphi()/2., phi_min) > 0 &&
        CaloPhiRange::diff(phi_max, cell->phi() - cell->caloDDE()->dphi()/2.) > 0);
}
//------------------------------------------------------------------
// /** Return the % of path length crossed by the track inside a cell in phi **/
// double TrackInCaloTools::getPathInPhi(const CaloCell *cell, double phi_entrance, double phi_exit)
// {
//   double mean_phi = phiMean(phi_entrance, phi_exit);
//   double dphi = fabs( CaloPhiRange::diff(phi_entrance, phi_exit);
//   double phi_min = mean_phi - dphi, phi_max = mean_phi + dphi;
//   
//   if (!(CaloPhiRange::diff(cell->phi() + cell->caloDDE()->dphi()/2., phi_min) > 0 &&
//         CaloPhiRange::diff(phi_max, cell->phi() - cell->caloDDE()->dphi()/2.) > 0))
//     return 0;
//   
//   double phi_L = CaloPhiRange::diff(phi_max, cell->phi() + 0.5*cell->caloDDE()->dphi()) > 0? cell->phi() + 0.5*cell->caloDDE()->dphi(), phi_max;
//   double phi_R = CaloPhiRange::diff(phi_min, cell->phi() - 0.5*cell->caloDDE()->dphi()) > 0? phi_min, cell->phi() + 0.5*cell->caloDDE()->dphi();
//   return CaloPhiRange::diff(phi_L, phi_R)/dphi;
// }
//------------------------------------------------------------------
/** Return the % of path length crossed by the track inside a cell in eta **/
double TrackInCaloTools::getPathLengthInEta(const CaloCell *cell, double eta_entrance, double eta_exit)
{
  double etaMin = cell->eta() - 0.5*cell->caloDDE()->deta(), etaMax = cell->eta() + 0.5*cell->caloDDE()->deta();
  if ( fabs(eta_entrance  - eta_exit) < 1e-6 ) // to avoid FPE
    return eta_entrance > etaMin && eta_entrance < etaMax;
    
  double etaMinTrack = std::min(eta_entrance, eta_exit);
  double etaMaxTrack = std::max(eta_entrance, eta_exit);
  return (std::min(etaMax, etaMaxTrack) - std::max(etaMin, etaMinTrack))/(etaMaxTrack - etaMinTrack);
}
//------------------------------------------------------------------
/** Return the % of path length crossed by the track inside a cell in Z **/
double TrackInCaloTools::getPathLengthInZ(const CaloCell *cell, double z_entrance, double z_exit)
{
  return getPathLengthInZ(cell->z() - 0.5*cell->caloDDE()->dz(), cell->z() + 0.5*cell->caloDDE()->dz(), z_entrance, z_exit);
}
//------------------------------------------------------------------
/** Return the % of path length crossed by the track inside a cell in Z **/
double TrackInCaloTools::getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit)
{
  if ( fabs(z_entrance  - z_exit) < 1e-6 ) // to avoid FPE
    return z_entrance > zMin && z_entrance < zMax;
 
  double zMinTrack = std::min(z_entrance, z_exit);
  double zMaxTrack = std::max(z_entrance, z_exit);
  return (std::min(zMax, zMaxTrack) - std::max(zMin, zMinTrack))/(zMaxTrack - zMinTrack);
}
//------------------------------------------------------------------
/** Return the % of path length crossed by the track inside a cell in Z for a ladder shaped cell **/
double TrackInCaloTools::getPathLengthInZTileBar1(const CaloCell *cell, double z_entrance, double z_exit)
{
  // Divide the problem in 2 rectangles: the cell contains 6 tile rows, 3 in the bottom part, 3 in the top
  // Calculate the point where the track crossed the boundary between the bottom and top parts
  // and determine the path lenght inside each part
  ATH_MSG_DEBUG("In getPathLengthInZTileBar1");
  TileCellDim *cell_dim = m_tileDDM->get_cell_dim(cell->caloDDE()->identify());
  if (!cell_dim || cell_dim->getNRows() != 6) // should always be the case for this sampling
    return 0;
  // Get the percentage in depth covered by the bottom part of the cell
  double R = (cell_dim->getRMax(2) - cell_dim->getRMin(0))/(cell_dim->getRMax(5) - cell_dim->getRMin(0));
  ATH_MSG_DEBUG("Cell depth ratio: " << R );
  // The point where the track crossed the boundary
  double z_middle = z_entrance + R*(z_exit - z_entrance);
  ATH_MSG_DEBUG("Z entrance, middle, exit: " << z_entrance << " " << z_middle << " " << z_exit);
  // Get the boundaries of the 2 half-cells
  double zBottom_min = cell_dim->getZMin(0), zBottom_max = cell_dim->getZMax(0);
  double zTop_min = cell_dim->getZMin(3), zTop_max = cell_dim->getZMax(3);
  ATH_MSG_DEBUG("Z bottom min, bottom max: " << zBottom_min << " " << zBottom_max);
  ATH_MSG_DEBUG("Z top min, top max: " << zTop_min << " " << zTop_max);
  double pathBottom = getPathLengthInZ(zBottom_min, zBottom_max, z_entrance, z_middle);
  // Calculate the path traversed in the bottom and top parts
  if (zTop_min == zTop_max) // top part of B9 cell has 0 width. Don't take the mean in this case
  {
    if ( fabs(z_middle) > fabs(zBottom_max) && pathBottom > 0)
    {
      ATH_MSG_DEBUG("B9 cell, track left the sampling, will return 1");
      return 1;
    }
    ATH_MSG_DEBUG("B9 cell, only bottom part: " << pathBottom);
    return pathBottom;
  }
  double pathTop = getPathLengthInZ(zTop_min, zTop_max, z_middle, z_exit);
  ATH_MSG_DEBUG("Path in bottom / top: " << pathBottom << " " << pathTop);
  // Take the weighted mean using the depth ratio since the method gives the %
  return R*pathBottom + (1.-R)*pathTop;
}
//------------------------------------------------------------------
/** Set the noise threshold for cell selection **/
void TrackInCaloTools::setNoiseThreshold(double threshold)
{
  if (threshold < m_cellNoiseRMSCutLoose)
    ATH_MSG_WARNING( "Cannot set noise threshold below loose one, keeping it at " << m_cellNoiseRMSCut );
  else
    m_cellNoiseRMSCut = threshold;
}
//------------------------------------------------------------------
/** Remove from the given vector, cells which do not belong to the given sample (even if they belong to the same layer) **/
void TrackInCaloTools::selectCellsBySample(std::vector<const CaloCell*> &cells, CaloCell_ID::CaloSample sample)
{
  std::vector<const CaloCell*>::iterator it = cells.begin();
  while (it != cells.end())
    if ( (*it) && (*it)->caloDDE()->getSampling() != sample)
      cells.erase(it);
    else 
      ++it;
}
//------------------------------------------------------------------
/** Calculate the isolation energy for a set of tracks, removing the core region of each from the isolation cone **/
std::vector<double> TrackInCaloTools::getIsolationEnergy(std::vector<const INavigable4Momentum *> tracks, double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  std::vector<double> energies(tracks.size(), 0.);
  std::vector<const CaloCell*> core_cells;
  std::vector< std::vector< std::vector<const CaloCell*> > > cells;
  for (std::vector<const INavigable4Momentum *>::iterator it = tracks.begin(); it != tracks.end(); ++it)
  {
    const Trk::TrackParameters *par = m_trackExtrapolatorToCalo->getTrackParameters(*it); 
    cells.push_back( getCellsAroundTrack(par, dR, first_layer, last_layer, useLooseCut) );
    std::vector<const CaloCell*> c = getCellsForEcore(par, first_layer, last_layer, useLooseCut);
    core_cells.insert(core_cells.end(), c.begin(), c.end());
  }
  for (unsigned int i = 0; i < tracks.size(); ++i)
  {
    removeCoreFromCone(cells[i], core_cells, first_layer, last_layer);
    for (int j = 0; j <= last_layer - first_layer; ++j) { // layer
      for (std::vector<const CaloCell*>::iterator cell_it = cells[i][j].begin(); cell_it != cells[i][j].end(); ++cell_it)
        energies[i] += (*cell_it)->et();
    }
  }
  return energies;
}
//------------------------------------------------------------------
/** Return sum( Et**EtExp * dR**dRExp )/sum(Et) within a cone of dR, removing negative cells if negRemoval = true **/ 
double TrackInCaloTools::getIsolationWidth(const Trk::TrackParameters* track,const double dR, const double EtExp, const double dRExp, bool negRemoval, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
{
  ATH_MSG_DEBUG( "In TrackInCaloTools::getIsolationWidth" );
  if (last_layer < first_layer || first_layer < ps || last_layer > hec3)
  {
    ATH_MSG_WARNING( "Invalid layers: " << first_layer << " " << last_layer );
    return 0;
  }
 
  double isolationSumWidth = 0.;
  double isolationSumEt = 0.;
  std::vector< std::vector<const CaloCell*> > isolation_cells = getCellsForIsolation(track, dR, first_layer, last_layer, useLooseCut);
  if (isolation_cells.size() == 0)
    return isolationSumWidth;
  for (unsigned int layer = (unsigned int) first_layer, ilayer = 0; layer <= (unsigned int) last_layer; layer++, ilayer++) {
    for (unsigned int i=0; i < isolation_cells[ilayer].size(); i++){
      double Etcell=isolation_cells[ilayer][i]->et();
      if (negRemoval && Etcell<0) continue;
      double dRcell=P4Helpers::deltaR( *isolation_cells[ilayer][i], m_extrapolations[ilayer].first, m_extrapolations[ilayer].second );
      double EtTerm=pow( Etcell ,EtExp);
      double dRTerm=pow( dRcell ,dRExp);
      isolationSumWidth += EtTerm*dRTerm;
      isolationSumEt += EtTerm;
    }
  }
  if (isolationSumEt) return isolationSumWidth/isolationSumEt;
  else return 0.;
}
