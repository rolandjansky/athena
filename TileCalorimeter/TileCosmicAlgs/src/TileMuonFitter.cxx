/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TileMuonFitter
 * PACKAGE:  offline/TileCalorimeter/TileCosmicAlgs
 *
 * AUTHOR :  J. Maneira
 * CREATED:  10-Jul-2006
 * REVISED:  25-Jan-2008 Added Hough Transform Method by Luciano M. de Andrade
 *
 *
 *           08-Apr-2009 Remove masking of bad cells (now done by 
 *                                                    TileCellBuilder)
 *                       Optimized loop over CaloCellContainer to read only
 Tile Cells
 *
 *
 *
 * PURPOSE:  Fit a straight muon track through a selected subset
 *           of Tile Cells, calculate horizontal plane(or vertical for 
 *           single beam) crossing time. Use track parameters to 
 *           estimate energy and path length.
 *
 *  Input: TileCellContainer 
 *  Output: ComissioningEvent/ComTime object
 *          TileEvent/TileCosmicMuon object
 *  Parameters:
 *    
 *   
 ********************************************************************/
// Tile includes
#include "TileCosmicAlgs/TileMuonFitter.h"
#include "TileIdentifier/TileHWID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileEvent/TileCosmicMuon.h"

// Calo includes
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloID.h"

// Athena includes
#include "EventContainers/SelectAllObject.h" 
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "Identifier/HWIdentifier.h"

// Gaudi includes
#include "GaudiKernel/ISvcLocator.h"

// External includes
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/FunctionMinimum.h"

// ROOT includes
#include "CLHEP/Vector/ThreeVector.h"

#include <cmath>
#ifndef __APPLE__
#define pi M_PIl
#else
#define pi M_PI
#endif

using CLHEP::Hep3Vector;
using CLHEP::c_light;

// Hough Transform Constants --------------------------------------------------

#define BIN_RES_RXY 100.0          // bin resolution for Hough parameter rxy (in milimeters)
#define BIN_RES_RZY 100.0          // bin resolution for Hough parameter rzy (in milimeters)
#define BIN_RES_AXY (2.0*pi/128.0) // bin resolution for Hough parameter axy (in     radian)
#define BIN_RES_AZY (2.0*pi/128.0) // bin resolution for Hough parameter azy (in     radian)
#define MAX_NCELLS 500U             // max number of cells to compute HT
#define MIN_NCELLS 3U               // min number of cells in track
#define MIN_ENERGY 1.0F             // minimal track energy (in GeV)
#define MIN_ENERGY_MEV 1000.0F      // minimal track energy (in MeV)
#define SHIFT_X 5000.0             // displacement in X
#define SHIFT_Z 8000.0             // displacement in Z

// -----------------------------------------------------------------------------

using namespace ROOT::Minuit2;

CaloCell_ID::SUBCALO TileMuonFitter::m_caloIndex = CaloCell_ID::TILE;

//
// Constructor
//
TileMuonFitter::TileMuonFitter(std::string name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_tileID(nullptr)
  , m_tileHWID(nullptr)
  , m_tileMgr(nullptr)
  , m_caloCells(nullptr)
  , m_theTrack(nullptr)
  , m_nCells(0)
  , m_meanX(0.0)
  , m_meanY(0.0)
  , m_meanZ(0.0)
  , m_weightedMeanX(0.0)
  , m_weightedMeanY(0.0)
  , m_weightedMeanZ(0.0)
  , m_maxBottomIndex(0)
  , m_maxTopIndex(0)
  , m_reg1to2(false)
{
  declareProperty("DoHoughTransform", m_doHoughTransform = true);
  declareProperty("EThreshold", m_eThreshold = 250.);
  declareProperty("DeltaTimeCut", m_deltaTimeCut = 6.);
  declareProperty("MinimumCells", m_minimumCells = 2);
  declareProperty("DoWeighted", m_doWeighted = true);
  declareProperty("DoDensityWeighting", m_doDensity = true);
  declareProperty("BeamType", m_beamType = "cosmics");
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
TileMuonFitter::~TileMuonFitter() {
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
StatusCode TileMuonFitter::initialize() {

  CHECK( detStore()->retrieve(m_tileMgr) );
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );

  if (m_doHoughTransform) {
    m_cosmicMuonContainerKey = "TileCosmicMuonHT";
  }

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_cosmicMuonContainerKey.initialize() );
  ATH_CHECK( m_comTimeKey.initialize() );

  m_cellEnergy.resize(m_tileID->cell_hash_max());
  m_cellWeight.resize(m_tileID->cell_hash_max());
  m_cellTime.resize(m_tileID->cell_hash_max());
  m_cellDeltaTime.resize(m_tileID->cell_hash_max());
  m_cellHash.resize(m_tileID->cell_hash_max());
  m_cellPosition.resize(m_tileID->cell_hash_max());

  if (m_minimumCells < 2) m_minimumCells = 2;

  ATH_MSG_INFO( "Loading:   " << m_cellContainerKey.key() );
  ATH_MSG_INFO( "Recording: " << m_cosmicMuonContainerKey.key() << " and " << m_comTimeKey.key() );
  ATH_MSG_INFO( "Cell Energy Threshold: " << m_eThreshold << " (MeV)" );
  ATH_MSG_INFO( "Delta Time Cell Cut: " << m_deltaTimeCut << " (ns)" );
  ATH_MSG_INFO( "Minimum Number of cells for fit: " << m_minimumCells );

  if (m_doWeighted && m_doDensity)
    ATH_MSG_INFO( "Weighting cell position and time with energy density." );

  if (m_doWeighted && !m_doDensity)
    ATH_MSG_INFO( "Weighting cell position and time with energy." );

  if (!m_doWeighted)
    ATH_MSG_INFO( "Not weighting cell position and time." );

  if (!m_beamType.compare("collisions")
      || !m_beamType.compare("singlebeam")
      || !m_beamType.compare("cosmics")) {

    ATH_MSG_INFO( "Setting up TMF for beam type = " << m_beamType );
  } else {
    ATH_MSG_WARNING( "BeamType " << m_beamType
                     << " from jobproperties not recognized. Setting to cosmics for TMF only." );

    m_beamType = "cosmics";
  }

  m_tileDD_radiusLB.resize(4);
  m_tileDD_radiusEB.resize(4);
  m_tileDD_zEBA.resize(2);
  m_tileDD_zEBC.resize(3);
  m_tileDD_zLB.resize(2);
  bool hack = false;

  std::vector<TileDetDescriptor*>::const_iterator first = m_tileMgr->tile_descriptors_begin();
  std::vector<TileDetDescriptor*>::const_iterator last = m_tileMgr->tile_descriptors_end();
  for (; first != last; ++first) {
    const TileDetDescriptor * tileDD = (*first);
    //tileDD->print();
    if (tileDD->n_samp() != 3) break;
    if (tileDD->n_eta(0) == 10 && tileDD->sign_eta() == 1) { //LBA
      hack = (tileDD->dr(0) > 300);
      m_tileDD_zLB[1] = tileDD->zcenter(0) + tileDD->dz(0) / 2.;
      for (int is = 0; is < 3; is++)
        m_tileDD_radiusLB[is] = tileDD->rcenter(is) - tileDD->dr(is) / 2.;
      m_tileDD_radiusLB[3] = tileDD->rcenter(2) + tileDD->dr(2) / 2.;
    } else if (tileDD->n_eta(0) == 10 && tileDD->sign_eta() == -1) { //LBC
      m_tileDD_zLB[0] = tileDD->zcenter(0) - tileDD->dz(0) / 2.;
    } else if (tileDD->n_eta(0) == 5 && tileDD->sign_eta() == 1) { //EBA
      m_tileDD_zEBA[0] = tileDD->zcenter(0) - tileDD->dz(0) / 2.;
      m_tileDD_zEBA[1] = tileDD->zcenter(0) + tileDD->dz(0) / 2.;
      for (int is = 0; is < 3; is++)
        m_tileDD_radiusEB[is] = tileDD->rcenter(is) - tileDD->dr(is) / 2.;
      m_tileDD_radiusEB[3] = tileDD->rcenter(2) + tileDD->dr(2) / 2.;
    } else if (tileDD->n_eta(0) == 5 && tileDD->sign_eta() == -1) { //EBC
      m_tileDD_zEBC[0] = tileDD->zcenter(0) - tileDD->dz(0) / 2.;
      m_tileDD_zEBC[1] = tileDD->zcenter(0) + tileDD->dz(0) / 2.;
    }

  }
// WARNING: ugly hack! Remove when Geo-Model is updated
// this is needed to remove from the cell lengths the spacer
// plate after tilerow 11 and the front plate before tilerow 1
// see http://indico.cern.ch/conferenceDisplay.py?confId=59722
  if (hack) {
    ATH_MSG_INFO( "Old geometry detected: moving inner/outer radius by +10/-40 mm " );
    m_tileDD_radiusLB[3] -= 40; // reduce outer radius by 40 mm
    m_tileDD_radiusEB[3] -= 40; // reduce outer radius by 40 mm
    m_tileDD_radiusLB[0] += 10; // increase inner radius by 10 mm
    m_tileDD_radiusEB[0] += 10; // increase inner radius by 10 mm
  }
// end of hack
  ATH_MSG_INFO( "Geometry read from TileDetDescr for track length calculation:" );
  for (int is = 0; is < 4; is++)
    ATH_MSG_INFO( "Radius sampling " << is
                 << " LB: " << m_tileDD_radiusLB[is]
                 << " EB: " << m_tileDD_radiusEB[is] );

  ATH_MSG_INFO( "LB  z between " << m_tileDD_zLB[0] << " and " << m_tileDD_zLB[1] );
  ATH_MSG_INFO( "EBA z between " << m_tileDD_zEBA[0] << " and " << m_tileDD_zEBA[1] );
  ATH_MSG_INFO( "EBC z between " << m_tileDD_zEBC[0] << " and " << m_tileDD_zEBC[1] );
  ATH_MSG_INFO( "TileMuonFitter initialization completed" );

  return StatusCode::SUCCESS;
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
StatusCode TileMuonFitter::finalize() {

  ATH_MSG_INFO( "Finalizing" );

  return StatusCode::SUCCESS;
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
StatusCode TileMuonFitter::execute() {

  ATH_MSG_DEBUG( " start execute " );

  int fitStatus = 2; //not even try
  setEventDefaults();

  SG::ReadHandle<CaloCellContainer> cellContainer(m_cellContainerKey);
  if (!cellContainer.isValid()) {
    ATH_MSG_WARNING( " Could not find container " << m_cellContainerKey.key() );
  } else {
    ATH_MSG_DEBUG( "Container " << m_cellContainerKey.key() << " with CaloCells found " );

    m_caloCells = cellContainer.cptr();

    // check on number of tile cells
    if (m_caloCells->nCellsCalo(m_caloIndex) == 0) {
      ATH_MSG_DEBUG( "no TileCells in CellContainer " << m_cellContainerKey.key() << "> for this event!" );
    } else {
      buildCells();
    }

    if (eventSelection()) {
      if (!m_doHoughTransform)
        fitStatus = fitTrack();
      else
        fitStatus = houghTrack();
      if (fitStatus) calculateTime();
    }

    buildTileCosmicMuon(fitStatus);
    buildComTime(fitStatus);
  }

  // Execution completed.
  ATH_MSG_DEBUG( "TileMuonFitter execution completed." );

  return StatusCode::SUCCESS;
}

// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::setEventDefaults() {
  m_nCells = 0;
  for (unsigned int i = 0; i < m_tileID->cell_hash_max(); i++) {
    m_cellEnergy[i] = 0.0;
    m_cellWeight[i] = 0.0;
    m_cellTime[i] = 0.0;
    m_cellDeltaTime[i] = 999.;
    m_cellHash[i] = 0;
    m_cellPosition[i].setX(0.0);
    m_cellPosition[i].setY(0.0);
    m_cellPosition[i].setZ(0.0);
  }

  m_linePar.clear();
  m_zeroCrossingTime.clear();
  m_fitMinimum.clear();
}

// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildCells() {

  // CaloCellContainer::const_iterator collItr = celcoll->begin();
  // CaloCellContainer::const_iterator lastColl = celcoll->end();

  size_t collItr = m_caloCells->indexFirstCellCalo(m_caloIndex);
  size_t lastColl = m_caloCells->indexLastCellCalo(m_caloIndex);

  for (; collItr != lastColl; ++collItr) {

    const CaloCell* cell = (*m_caloCells)[collItr];

    const TileCell* tilecell = dynamic_cast<const TileCell*>(cell);
    if (tilecell == 0) continue;

    Identifier cell_id = cell->ID();
    IdentifierHash cell_idhash = m_tileID->cell_hash(cell_id);
    CaloDetDescrElement *caloDDE = m_tileMgr->get_cell_element(cell_id);

    double ener = cell->energy();
    double time = cell->time();
    double deltatime = tilecell->timeDiff();

    if (ener < m_eThreshold) continue;	   	// below threshold removal
    if (fabs(caloDDE->eta()) < 0.05 && caloDDE->r() > 3500) {
      ATH_MSG_DEBUG( "Cell eta: " << caloDDE->eta()
                    << " Cell r: " << caloDDE->r()
                    << " Tile diff: " << tilecell->timeDiff() );

      deltatime = 0;
    }
    if (fabs(deltatime) > m_deltaTimeCut) continue;	// cut high time imbalance cells

    double volume = caloDDE->volume();
    if (volume == 0.0) {
      ATH_MSG_DEBUG( "Warning: Skipping cell with zero volume!" );
      continue;
    }

    if (m_doDensity)
      m_cellWeight[m_nCells] = ener / volume;
    else
      m_cellWeight[m_nCells] = ener;
    m_cellEnergy[m_nCells] = ener;
    m_cellTime[m_nCells] = time;
    m_cellDeltaTime[m_nCells] = deltatime;
    m_cellHash[m_nCells] = cell_idhash;
    m_cellPosition[m_nCells].setX(caloDDE->x());
    m_cellPosition[m_nCells].setY(caloDDE->y());
    m_cellPosition[m_nCells].setZ(caloDDE->z());
    m_nCells++;

  }	//end of CaloCellContainer cycle
}

// ********************************************************************
// ********************************************************************
// ********************************************************************

bool TileMuonFitter::eventSelection() {
  // simple check if there are cells on top and bottom halfs
  // returns 1 if both top and bottom have energy, 
  // JM, Aug08: Change to allow for single beam halo muon
  // looks for A/C side instead of top/bottom

  if (m_nCells < m_minimumCells) return false;

  double maxReg1Energy;
  double maxReg2Energy;
  double cellPosition;
  int maxReg1Index;
  int maxReg2Index;
  bool selection = false;
  maxReg1Energy = maxReg2Energy = 0.;
  maxReg1Index = maxReg2Index = -1;

  for (int i = 0; i < m_nCells; i++) {
    if (!m_beamType.compare("singlebeam")) {
      cellPosition = m_cellPosition[i].getZ();
    } else if (!m_beamType.compare("collisions")) {
      cellPosition = 1.;
    } else {
      cellPosition = m_cellPosition[i].getY();
    }

    if (cellPosition > 0) {
      if (m_cellEnergy[i] > maxReg1Energy) {
        maxReg1Energy = m_cellEnergy[i];
        maxReg1Index = i;
      }
    } else {
      if (m_cellEnergy[i] > maxReg2Energy) {
        maxReg2Energy = m_cellEnergy[i];
        maxReg2Index = i;
      }
    }
  }

  m_reg1to2 = 1;
  if (maxReg1Index >= 0 && maxReg2Index >= 0) {
    if (m_cellTime[maxReg2Index] < m_cellTime[maxReg1Index]) m_reg1to2 = 0;
  }

  if (!m_beamType.compare("collisions")) {
    selection = (maxReg1Energy > m_eThreshold);
  } else {
    selection = (maxReg2Energy > m_eThreshold && maxReg1Energy > m_eThreshold);
  }

  return selection;
}

// ********************************************************************
// ********************************************************************
// ********************************************************************

int TileMuonFitter::fitTrack() {

  ATH_MSG_DEBUG( "Fitting with Std method" << m_nCells << " cells." );

  std::vector<double> X;
  std::vector<double> Y;
  std::vector<double> Z;
  X.resize(m_nCells);
  Y.resize(m_nCells);
  Z.resize(m_nCells);
  for (int i = 0; i < m_nCells; i++) {

    X[i] = m_cellPosition[i].getX();
    Y[i] = m_cellPosition[i].getY();
    Z[i] = m_cellPosition[i].getZ();
    double rr = sqrt(X[i] * X[i] + Y[i] * Y[i]);

    ATH_MSG_DEBUG( "Cell " << i
                  << "   Energy " << m_cellEnergy[i]
                  << " Weight " << m_cellWeight[i]
                  << "   Time " << m_cellTime[i]
                  << "   r " << rr
                  << "   X " << X[i]
                  << "   Y " << Y[i]
                  << "   Z " << Z[i] );

  }

  // Minuit Method ------------------------------------------------------------

  m_theTrack = new TileMuonTrackDistance(X, Y, Z, m_cellWeight);
  m_theTrack->SetWeighted(m_doWeighted);
  m_theTrack->Means();

  double aa, bb, cc, dd;
  double erraa, errbb, errcc, errdd;
  aa = bb = cc = dd = erraa = errbb = errcc = errdd = 0.0;
  if (m_cellPosition[m_maxTopIndex].getX() == m_cellPosition[m_maxBottomIndex].getX()) {
    bb = 0.;
    dd = 0.;
  } else {
    bb = (m_cellPosition[m_maxTopIndex].getY() - m_cellPosition[m_maxBottomIndex].getY())
        / (m_cellPosition[m_maxTopIndex].getX() - m_cellPosition[m_maxBottomIndex].getX());
    dd = (m_cellPosition[m_maxTopIndex].getZ() - m_cellPosition[m_maxBottomIndex].getZ())
        / (m_cellPosition[m_maxTopIndex].getX() - m_cellPosition[m_maxBottomIndex].getX());
  }

  MnUserParameters upar;
  upar.Add("bb", bb, 0.1);
  upar.Add("dd", dd, 0.1);

  MnMigrad migrad(*m_theTrack, upar);
  FunctionMinimum min = migrad();

  bool myIsValid = min.IsValid();

  m_fitMinimum.push_back(0);

  if (myIsValid) {
    bb = min.UserState().Value("bb");
    dd = min.UserState().Value("dd");
    aa = m_theTrack->GetMeanY() - bb * m_theTrack->GetMeanX();
    cc = m_theTrack->GetMeanZ() - dd * m_theTrack->GetMeanX();
    errbb = min.UserState().Error("bb");
    errdd = min.UserState().Error("dd");
    erraa = fabs(errbb * m_theTrack->GetMeanX());
    errcc = fabs(errdd * m_theTrack->GetMeanX());

    addTrack(aa, bb, cc, dd);

    m_fitMinimum[m_fitMinimum.size() - 1] = min.UserState().Fval();
  }

  if (myIsValid) {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Minimum Value  aa: " << aa
                      << "   bb: " << bb
                      << "   cc: " << cc
                      << "   dd " << dd << endmsg;
      msg(MSG::DEBUG) << "Minimum Error  aa: " << erraa
                      << "   bb: " << errbb
                      << "   cc: " << errcc
                      << "   dd " << errdd << endmsg;
    }
    return 1;
  } else {
    ATH_MSG_DEBUG( "Could not minimize" );
    return 0;
  }

}
// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::calculateTime() {

  if (!m_beamType.compare("singlebeam") || !m_beamType.compare("collisions"))
    calculateTimeAtZequal0();
  else
    calculateTimeAtYequal0();

}
// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::calculateTimeAtYequal0() {

  // Uses the track parameters to calculate:
  //   time, position  at horizontal plane
  // Since y = a + b*x and z = c + d*x
  //   y == 0 => x = -a/b    &&   z = c - d*a/b
  // Time is average from each corrected with tof along track


  for (unsigned int n = 0; n < m_linePar.size(); n++) {
    double p0 = m_linePar[n][0];
    double p1 = m_linePar[n][1];
    double p2 = m_linePar[n][2];
    double p3 = m_linePar[n][3];

    ATH_MSG_DEBUG( "Starting CalculateTime at Y=0 m_linePar: "
                  << p0 << " " << p1 << " " << p2 << " " << p3  );

    double correction = 0, weight = 1.0, weightsum = 0, time = 0;

    if (p1 == 0) {
      m_zeroCrossingTime.push_back(0.0);
    } else {
      Hep3Vector theZeroCrossingPosition(-1.0 * p0 / p1, 0.0, p2 - 1.0 * p3 * p0 / p1);

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using cells to calculate time at Y=0: ";

      for (int i = 0; i < m_nCells; i++) {
        if (m_doHoughTransform && m_cellInfo[i].track_index != (int) n) continue; // use only cells that belong to the current track

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << i << "/ ";
        Hep3Vector dataP(m_cellPosition[i].getX(), m_cellPosition[i].getY(),
            m_cellPosition[i].getZ());
        Hep3Vector lineP(m_theTrack->ClosestPoint(&dataP, m_linePar[n]));
        correction = (theZeroCrossingPosition - lineP).mag() / c_light;

        if (m_doWeighted) weight = m_cellEnergy[i];
        if ((m_reg1to2 && (m_cellPosition[i].getY() > 0))
            || (!m_reg1to2 && (m_cellPosition[i].getY() < 0))) {
          correction = 1.0 * correction;
        } else {
          correction = -1.0 * correction;
        }

        time += weight * (m_cellTime[i] + correction);
        weightsum += weight;
      }

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;

      if (weightsum > 0)
        m_zeroCrossingTime.push_back(time / weightsum);
      else
        m_zeroCrossingTime.push_back(0.0);

      ATH_MSG_DEBUG( "Time at Y=0: " << m_zeroCrossingTime[n] );
    }
  }
}
// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::calculateTimeAtZequal0() {

  // Uses the track parameters to calculate:
  //   time, position  at vertical plane
  // Since y = a + b*x and z = c + d*x
  //   z == 0 => x = -c/d    &&   y = a - b*c/d
  // Time is average from each corrected with tof along track


  for (unsigned int n = 0; n < m_linePar.size(); n++) {
    double p0 = m_linePar[n][0];
    double p1 = m_linePar[n][1];
    double p2 = m_linePar[n][2];
    double p3 = m_linePar[n][3];

    ATH_MSG_DEBUG( "Starting CalculateTime at Z=0 m_linePar: "
                  << p0 << " " << p1 << " " << p2  << " " << p3 );

    double correction = 0, weight = 1.0, weightsum = 0, time = 0;

    if (p3 == 0) {
      m_zeroCrossingTime.push_back(0.0);
    } else {
      Hep3Vector theZeroCrossingPosition(-1.0 * p2 / p3, p0 - 1.0 * p1 * p2 / p3, 0.0);

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using cells to calculate time at Z=0: ";

      for (int i = 0; i < m_nCells; i++) {
        if (m_doHoughTransform && m_cellInfo[i].track_index != (int) n) continue; // use only cells that belong to the current track

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << i << "/ ";
        Hep3Vector dataP(m_cellPosition[i].getX(), m_cellPosition[i].getY(),
            m_cellPosition[i].getZ());
        Hep3Vector lineP(m_theTrack->ClosestPoint(&dataP, m_linePar[n]));
        correction = (theZeroCrossingPosition - lineP).mag() / c_light;

        if (m_doWeighted) weight = m_cellEnergy[i];
        if ((m_reg1to2 && (m_cellPosition[i].getZ() > 0))
            || (!m_reg1to2 && (m_cellPosition[i].getZ() < 0)))
          correction = 1.0 * correction;
        else
          correction = -1.0 * correction;

        time += weight * (m_cellTime[i] + correction);
        weightsum += weight;
      }

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endmsg;

      if (weightsum > 0)
        m_zeroCrossingTime.push_back(time / weightsum);
      else
        m_zeroCrossingTime.push_back(0.0);

      ATH_MSG_DEBUG( "Time: " << m_zeroCrossingTime[n] );
    }
  }
}

// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildTileCosmicMuon(int fitok) {

  if (!m_beamType.compare("singlebeam") || !m_beamType.compare("collisions"))
    buildTileCosmicMuonAtZequal0(fitok);
  else
    buildTileCosmicMuonAtYequal0(fitok);

}

// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildTileCosmicMuonAtYequal0(int fitok) {

  // Uses the track parameters to calculate:
  //   position and direction at horizontal at horizontal plane
  // Since y = a + b*x and z = c + d*x
  //   y == 0 => x = -a/b    &&   z = c - d*a/b
  // Direction is (1,b,d)/sqrt(1 + b*b + d*d)
  //
  // Then put the result in a ComTime object

  SG::WriteHandle<TileCosmicMuonContainer> cosmicMuonContainer (m_cosmicMuonContainerKey);
  StatusCode sc = cosmicMuonContainer.record(std::make_unique<TileCosmicMuonContainer>());
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot record TileCosmicMuonContainer" << cosmicMuonContainer.key() );
  }

  for (unsigned int n = 0; n < m_linePar.size(); n++) {
    double p0 = m_linePar[n][0];
    double p1 = m_linePar[n][1];
    double p2 = m_linePar[n][2];
    double p3 = m_linePar[n][3];

    ATH_MSG_DEBUG( "Starting BuildTileCosmicMuon at y=0 m_linePar: "
                  << p0 << " " << p1 << " " << p2 << " " << p3 );

    TileCosmicMuon *theTileCosmicMuon = new TileCosmicMuon();

    if (!(fitok == 1) || p1 == 0.0) {
      cosmicMuonContainer->push_back(theTileCosmicMuon);
    } else {
      Hep3Vector theDirection(1.0, p1, p3);
      theDirection = theDirection.unit();
      if ((m_reg1to2 && p1 > 0.0) || (!m_reg1to2 && p1 < 0.0)) theDirection *= -1.0;

      std::vector<double> ltop, lbot, etop, ebot;
      std::vector<IdentifierHash> cells;
      std::vector<double> segPath;
      std::vector<int> segPartition, segModule, segSampling;

      if (m_linePar.size() > 0) trackIntersection(ltop, lbot, n);
      if (m_linePar.size() > 0)
        trackSegmentIntersection(segPath, segPartition, segModule, segSampling, n);
      if (m_linePar.size() > 0) energyInTrack(etop, ebot, cells, n);

      theTileCosmicMuon->SetTime(m_zeroCrossingTime[n]);
      theTileCosmicMuon->SetPositionX(-1.0 * p0 / p1);
      theTileCosmicMuon->SetPositionY(0.);
      theTileCosmicMuon->SetPositionZ(p2 - 1.0 * p3 * p0 / p1);
      theTileCosmicMuon->SetDirectionPhi(theDirection.phi());
      theTileCosmicMuon->SetDirectionTheta(theDirection.theta());
      theTileCosmicMuon->SetFitQuality(m_fitMinimum[n]);
      theTileCosmicMuon->SetFitNCells(m_nCells);
      theTileCosmicMuon->SetPathTop(ltop);
      theTileCosmicMuon->SetPathBottom(lbot);
      theTileCosmicMuon->SetSegmentPath(segPath);
      theTileCosmicMuon->SetSegmentPartition(segPartition);
      theTileCosmicMuon->SetSegmentModule(segModule);
      theTileCosmicMuon->SetSegmentSampling(segSampling);
      theTileCosmicMuon->SetEnergyTop(etop);
      theTileCosmicMuon->SetEnergyBottom(ebot);
      theTileCosmicMuon->SetTrackCellHash(cells);

      cosmicMuonContainer->push_back(theTileCosmicMuon);
    }

  }

  if (fitok != 2) delete m_theTrack;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildTileCosmicMuonAtZequal0(int fitok) {

  // Uses the track parameters to calculate:
  //   position and direction at horizontal at horizontal plane
  // Since y = a + b*x and z = c + d*x
  //   z == 0 => x = -c/d    &&   y = a - b*c/d
  // Direction is (1,b,d)/sqrt(1 + b*b + d*d)
  //
  // Then put the result in a ComTime object

  SG::WriteHandle<TileCosmicMuonContainer> cosmicMuonContainer (m_cosmicMuonContainerKey);
  StatusCode sc = cosmicMuonContainer.record(std::make_unique<TileCosmicMuonContainer>());
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot record TileCosmicMuonContainer" << cosmicMuonContainer.key() );
  }

  for (unsigned int n = 0; n < m_linePar.size(); n++) {
    double p0 = m_linePar[n][0];
    double p1 = m_linePar[n][1];
    double p2 = m_linePar[n][2];
    double p3 = m_linePar[n][3];

    ATH_MSG_DEBUG( "Starting BuildTileCosmicMuon at z=0 m_linePar: "
                  << p0 << " " << p1 << " " << p2 << " " << p3 );

    TileCosmicMuon *theTileCosmicMuon = new TileCosmicMuon();

    if (!(fitok == 1) || p3 == 0.0) {
      cosmicMuonContainer->push_back(theTileCosmicMuon);
    } else {
      Hep3Vector theDirection(1.0, p1, p3);
      theDirection = theDirection.unit();
      if ((m_reg1to2 && p3 > 0.0) || (!m_reg1to2 && p3 < 0.0)) theDirection *= -1.0;

      std::vector<double> ltop, lbot, etop, ebot;
      std::vector<IdentifierHash> cells;
      std::vector<double> segPath;
      std::vector<int> segPartition, segModule, segSampling;

      if (m_linePar.size() > 0) trackIntersection(ltop, lbot, n);
      if (m_linePar.size() > 0)
        trackSegmentIntersection(segPath, segPartition, segModule, segSampling, n);
      if (m_linePar.size() > 0) energyInTrack(etop, ebot, cells, n);

      theTileCosmicMuon->SetTime(m_zeroCrossingTime[n]);
      theTileCosmicMuon->SetPositionX(-1.0 * p2 / p3);
      theTileCosmicMuon->SetPositionY(p0 - 1.0 * p1 * p2 / p3);
      theTileCosmicMuon->SetPositionZ(0.);
      theTileCosmicMuon->SetDirectionPhi(theDirection.phi());
      theTileCosmicMuon->SetDirectionTheta(theDirection.theta());
      theTileCosmicMuon->SetFitQuality(m_fitMinimum[n]);
      theTileCosmicMuon->SetFitNCells(m_nCells);
      theTileCosmicMuon->SetPathTop(ltop);
      theTileCosmicMuon->SetPathBottom(lbot);
      theTileCosmicMuon->SetSegmentPath(segPath);
      theTileCosmicMuon->SetSegmentPartition(segPartition);
      theTileCosmicMuon->SetSegmentModule(segModule);
      theTileCosmicMuon->SetSegmentSampling(segSampling);
      theTileCosmicMuon->SetEnergyTop(etop);
      theTileCosmicMuon->SetEnergyBottom(ebot);
      theTileCosmicMuon->SetTrackCellHash(cells);

      cosmicMuonContainer->push_back(theTileCosmicMuon);
    }

  }

  if (fitok != 2) delete m_theTrack;
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
void TileMuonFitter::trackIntersection(std::vector<double> & ltop, std::vector<double> & lbot,
    int index) {

  const double p0 = m_linePar[index][0];
  const double p1 = m_linePar[index][1];
  const double p2 = m_linePar[index][2];
  const double p3 = m_linePar[index][3];

  ATH_MSG_DEBUG( "Starting TrackIntersection: " );
  int is, ip;
  std::vector<Hep3Vector> intPoint; //intersection points
  std::vector<Hep3Vector> topIPO;   //intersection points on top hemisphere, order according to y
  std::vector<Hep3Vector> bottomIPO; //intersection points on bottom hemisphere, order according to y
  std::vector<Hep3Vector> temp3v;
  std::vector<Hep3Vector> plane;
  Hep3Vector horizontalPlane;

  plane.resize(6);
  lbot.resize(3);
  ltop.resize(3);
  temp3v.resize(2);
  for (is = 0; is < 3; is++) {
    ltop[is] = 0.0;
    lbot[is] = 0.0;
  }
  /** check intersection with cylinder */
  for (is = 0; is < 4; is++) {

    /** Make the list of valid LB intersection points*/
    double aux_squared = m_tileDD_radiusLB[is] * m_tileDD_radiusLB[is] * (1 + p1 * p1) - p0 * p0;
    if (aux_squared > 0) {
      double x[2] = { (-1.0 * p0 * p1 + sqrt(aux_squared)) / (1 + p1 * p1), (-1.0 * p0 * p1
          - sqrt(aux_squared)) / (1 + p1 * p1) };
      for (uint8_t i = 0; i < 2; i++) {
        temp3v[i].set(x[i], p0 + p1 * x[i], p2 + p3 * x[i]);
        if (checkLBz(temp3v[i].z())) intPoint.push_back(temp3v[i]);
      }
    }

    /** Now let's make the list of valid EB intersection points*/
    aux_squared = m_tileDD_radiusEB[is] * m_tileDD_radiusEB[is] * (1 + p1 * p1) - p0 * p0;
    if (aux_squared > 0) {
      double x[2] = { (-1.0 * p0 * p1 + sqrt(aux_squared)) / (1 + p1 * p1), (-1.0 * p0 * p1
          - sqrt(aux_squared)) / (1 + p1 * p1) };
      for (uint8_t i = 0; i < 2; i++) {
        temp3v[i].set(x[i], p0 + p1 * x[i], p2 + p3 * x[i]);
        if (checkEBz(temp3v[i].z())) intPoint.push_back(temp3v[i]);
      }
    }

  }
  uint8_t ncylint = intPoint.size();

  /** check intersection with vertical planes */
  for (ip = 0; ip < 6; ip++)
    plane[ip].set(0, 0, 0);
  if (p3 != 0) {
    const double inv_p3 = 1. / p3;
    for (ip = 0; ip < 2; ip++) {
      plane[ip].set((m_tileDD_zLB[ip] - p2) * inv_p3, p0 + p1 * (m_tileDD_zLB[ip] - p2) * inv_p3,
          m_tileDD_zLB[ip]);

      plane[ip + 2].set((m_tileDD_zEBA[ip] - p2) * inv_p3, p0 + p1 * (m_tileDD_zEBA[ip] - p2) * inv_p3,
          m_tileDD_zEBA[ip]);

      plane[ip + 4].set((m_tileDD_zEBC[ip] - p2) * inv_p3, p0 + p1 * (m_tileDD_zEBC[ip] - p2) * inv_p3,
          m_tileDD_zEBC[ip]);

      for (is = 0; is < 3; is++) {
        if (checkLBr(plane[ip].perp(), is)) intPoint.push_back(plane[ip]);
        if (checkEBr(plane[ip + 2].perp(), is)) intPoint.push_back(plane[ip + 2]);
        if (checkEBr(plane[ip + 4].perp(), is)) intPoint.push_back(plane[ip + 4]);
      }
    }
  }

  ATH_MSG_DEBUG( "Intersection with cylinders: " << ncylint
                << " with planes: " << intPoint.size() - ncylint );

  /** We need to check if we need to add the horizontal point.
   The criterion is: is the horizontal plane crossing point
   within TileCal or not? */
  horizontalPlane.set(0, 0, 0);
  uint16_t idx_hor = 99;
  uint16_t i, j, k, jmax;
  if (p1 != 0) {
    const double inv_p1 = 1. / p1;
    /**check intersection with horizontal plane*/
    horizontalPlane.set(-1.0 * p0 * inv_p1, 0, p2 - p3 * p0 * inv_p1);
    if (checkLBz(horizontalPlane.z()) && checkLBr(horizontalPlane.x())) {
      for (i = 0; i < 3; i++)
        if (checkLBr(horizontalPlane.x(), i)) idx_hor = i;
    }
    if (checkEBz(horizontalPlane.z()) && checkEBr(horizontalPlane.x())) {
      for (i = 0; i < 3; i++)
        if (checkEBr(horizontalPlane.x(), i)) idx_hor = i;
    }
  }

  ATH_MSG_DEBUG( "Horizontal Plane idx: " << idx_hor
                << " X: " << horizontalPlane.x()
                << " Z: " << horizontalPlane.z() );

  /** Order points according to the y coordinate, and create two
   different vectors, one for bottom, one for top */
  uint16_t npoints = intPoint.size();
  float ymax;
  bool filled;
  std::vector<uint8_t> neworder;
  neworder.resize(0);
  for (i = 0; i < npoints; i++) {
    ymax = -99999999.0;
    jmax = 9999;
    for (j = 0; j < npoints; j++) {
      filled = false;
      for (k = 0; k < neworder.size(); k++) {
        if (j == neworder[k]) filled = true;
      }
      if (filled) continue;
      if (intPoint[j].y() > ymax) {
        ymax = intPoint[j].y();
        jmax = j;
      }
    }
    if (jmax < 9999) neworder.push_back(jmax);
  }

  if (neworder.size() != npoints)
    ATH_MSG_ERROR( " Npoints " << npoints
                  << " New order: " << neworder.size() );

  /** Now create ordered vector, adding in the horizontal plane point
   if needed. This must go at the top of the bottom list and at
   the bottom of the top list, since they are ordered according to y.*/

  if (idx_hor < 99) bottomIPO.push_back(horizontalPlane);

  for (i = 0; i < npoints; i++) {
    if (intPoint[neworder[i]].y() > 0)
      topIPO.push_back(intPoint[neworder[i]]);
    else
      bottomIPO.push_back(intPoint[neworder[i]]);
  }
  if (idx_hor < 99) topIPO.push_back(horizontalPlane);

  ATH_MSG_DEBUG( "Number of points on top: " << topIPO.size()
                << " and on bottom: " << bottomIPO.size() );

  if (topIPO.size() > 1 && bottomIPO.size()) {

    /** Now let's turn points into segment lengths. We take
     two successive points from the top or bottom stack.
     The segment is simply the difference between them. The region
     indices are checked with the midpoint. We discard
     this segment if its midpoint is in the LB/EB gap.*/

    Hep3Vector temp_midpoint;
    int temp_idx;
    for (i = 0; i < topIPO.size() - 1; i++) {
      ATH_MSG_DEBUG( "Top points : " << i
                    << " X: " << topIPO[i].x()
                    << " Y: " << topIPO[i].y()
                    << " Z: " << topIPO[i].z() );

      temp_midpoint = (topIPO[i] + topIPO[i + 1]) / 2.;
      ATH_MSG_DEBUG( "Temp_midz top: " << temp_midpoint.z() );

      if (checkLBz(temp_midpoint.z()))
        temp_idx = whichLBr(temp_midpoint.perp());
      else if (checkEBz(temp_midpoint.z()))
        temp_idx = whichEBr(temp_midpoint.perp());
      else
        continue;

      ATH_MSG_DEBUG( "Temp_idx : " << temp_idx );

      if (temp_idx < 0 || temp_idx > 2) continue;
      ltop[temp_idx] += (topIPO[i] - topIPO[i + 1]).mag();

      ATH_MSG_DEBUG( "ltop : " << ltop[temp_idx] );

    }

    ATH_MSG_DEBUG( "Top points : " << topIPO.size() - 1
                  << " X: " << topIPO[topIPO.size() - 1].x()
                  << " Y: " << topIPO[topIPO.size() - 1].y()
                  << " Z: " << topIPO[topIPO.size() - 1].z() );


    for (i = 0; i < bottomIPO.size() - 1; i++) {
      ATH_MSG_DEBUG( "Bot points : " << i
                    << " X: " << bottomIPO[i].x()
                    << " Y: " << bottomIPO[i].y()
                    << " Z: " << bottomIPO[i].z() );


      temp_midpoint = (bottomIPO[i] + bottomIPO[i + 1]) / 2.;
      ATH_MSG_DEBUG( "Temp_midz bottom: " << temp_midpoint.z() );

      if (checkLBz(temp_midpoint.z()))
        temp_idx = whichLBr(temp_midpoint.perp());
      else if (checkEBz(temp_midpoint.z()))
        temp_idx = whichEBr(temp_midpoint.perp());
      else
        continue;

      ATH_MSG_DEBUG( "Temp_idx : " << temp_idx );

      if (temp_idx < 0 || temp_idx > 2) continue;
      lbot[temp_idx] += (bottomIPO[i] - bottomIPO[i + 1]).mag();

      ATH_MSG_DEBUG( "lbot : " << lbot[temp_idx] );
    }

    ATH_MSG_DEBUG( "Bot points : " << bottomIPO.size() - 1
                  << " X: " << bottomIPO[bottomIPO.size() - 1].x()
                  << " Y: " << bottomIPO[bottomIPO.size() - 1].y()
                  << " Z: " << bottomIPO[bottomIPO.size() - 1].z() );

  }
  ATH_MSG_DEBUG( "Leaving TrackIntersection" );
}

// ********************************************************************
// ********************************************************************
// ********************************************************************
void TileMuonFitter::trackSegmentIntersection(std::vector<double> & segPath,
    std::vector<int> & segPartition, std::vector<int> & segModule, std::vector<int> & segSampling,
    int index) {

  double p0 = m_linePar[index][0];
  double p1 = m_linePar[index][1];
  double p2 = m_linePar[index][2];
  double p3 = m_linePar[index][3];

  ATH_MSG_DEBUG( "Starting TrackIntersection: " );

  std::vector<Hep3Vector> intPoint; //intersection points
  std::vector<Hep3Vector> ordPoint;   //ordered intersection points (according to y)

  std::vector<Hep3Vector> temp3v;
  std::vector<Hep3Vector> plane;
  std::vector<Hep3Vector> modPlane;

  plane.resize(6);
  modPlane.clear();
  modPlane.resize(32);
  temp3v.resize(2);
  segPath.clear();
  segPartition.clear();
  segModule.clear();
  segSampling.clear();

  /** check intersection with cylinder */
  int is, ip;
  uint16_t i, j, k, jmax;
  for (is = 0; is < 4; is++) {

    /** Make the list of valid LB intersection points*/
    double aux_squared = m_tileDD_radiusLB[is] * m_tileDD_radiusLB[is] * (1 + p1 * p1) - p0 * p0;
    if (aux_squared > 0) {
      double x[2] = { (-1.0 * p0 * p1 + sqrt(aux_squared)) / (1 + p1 * p1), (-1.0 * p0 * p1
          - sqrt(aux_squared)) / (1 + p1 * p1) };
      for (i = 0; i < 2; i++) {
        temp3v[i].set(x[i], p0 + p1 * x[i], p2 + p3 * x[i]);
        if (checkLBz(temp3v[i].z())) intPoint.push_back(temp3v[i]);
      }
    }

    /** Now let's make the list of valid EB intersection points*/
    aux_squared = m_tileDD_radiusEB[is] * m_tileDD_radiusEB[is] * (1 + p1 * p1) - p0 * p0;
    if (aux_squared > 0) {
      double x[2] = { (-1.0 * p0 * p1 + sqrt(aux_squared)) / (1 + p1 * p1), (-1.0 * p0 * p1
          - sqrt(aux_squared)) / (1 + p1 * p1) };
      for (i = 0; i < 2; i++) {
        temp3v[i].set(x[i], p0 + p1 * x[i], p2 + p3 * x[i]);
        if (checkEBz(temp3v[i].z())) intPoint.push_back(temp3v[i]);
      }
    }

  }
  uint16_t ncylint = intPoint.size();

  /** check intersection with vertical planes */
  for (ip = 0; ip < 6; ip++)
    plane[ip].set(0, 0, 0);
  if (p3 != 0) {
    for (ip = 0; ip < 2; ip++) {
      plane[ip].set((m_tileDD_zLB[ip] - p2) / p3, p0 + p1 * (m_tileDD_zLB[ip] - p2) / p3,
          m_tileDD_zLB[ip]);

      plane[ip + 2].set((m_tileDD_zEBA[ip] - p2) / p3, p0 + p1 * (m_tileDD_zEBA[ip] - p2) / p3,
          m_tileDD_zEBA[ip]);

      plane[ip + 4].set((m_tileDD_zEBC[ip] - p2) / p3, p0 + p1 * (m_tileDD_zEBC[ip] - p2) / p3,
          m_tileDD_zEBC[ip]);

      for (is = 0; is < 3; is++) {
        if (checkLBr(plane[ip].perp(), is)) intPoint.push_back(plane[ip]);
        if (checkEBr(plane[ip + 2].perp(), is)) intPoint.push_back(plane[ip + 2]);
        if (checkEBr(plane[ip + 4].perp(), is)) intPoint.push_back(plane[ip + 4]);
      }
    }
  }
  uint16_t nvpint = intPoint.size() - ncylint;

  /** check intersection with module planes */
  double tanTheta, tempX;
  for (ip = 0; ip < 32; ip++) {
    tanTheta = tan(pi / 32. * double(ip));
    if ((tanTheta - p1) != 0) {
      tempX = p0 / (tanTheta - p1);
      modPlane[ip].set(tempX, p0 + p1 * tempX, p2 + p3 * tempX);

      if (checkLBz(modPlane[ip].z()) && checkLBr(modPlane[ip].perp())) {
        intPoint.push_back(modPlane[ip]);
      }

      if (checkEBz(modPlane[ip].z()) && checkEBr(modPlane[ip].perp())) {
        intPoint.push_back(modPlane[ip]);
      }
    }
  }
  uint16_t nmpint = intPoint.size() - ncylint - nvpint;
  uint16_t npoints = intPoint.size();

  ATH_MSG_DEBUG( "Intersections: " << npoints
                << " cylinders: " << ncylint
                << " vertical planes: " << nvpint
                << " module planes: " << nmpint );

  /** Order points according to the y coordinate. */
  double ymax;
  bool filled;
  std::vector<int> neworder;
  neworder.resize(0);
  for (i = 0; i < npoints; i++) {
    ymax = -999999.0;
    jmax = 9999;
    for (j = 0; j < npoints; j++) {
      filled = false;
      for (k = 0; k < neworder.size(); k++) {
        if (j == neworder[k]) filled = true;
      }
      if (filled) continue;
      if (intPoint[j].y() > ymax) {
        ymax = intPoint[j].y();
        jmax = j;
      }
    }
    if (jmax < 9999) neworder.push_back(jmax);
  }

  if (neworder.size() != npoints)
    ATH_MSG_DEBUG( " Npoints " << npoints << " New order: " << neworder.size() );

  for (i = 0; i < npoints; i++)
    ordPoint.push_back(intPoint[neworder[i]]);

  ATH_MSG_DEBUG( "Number of points: " << ordPoint.size() );

  /** Now let's turn points into segment lengths. We take
   two successive points from the stack.
   The segment is simply the difference between them. The region
   indices are checked with the midpoint. We discard
   this segment if its midpoint is in the LB/EB gap.*/

  if (ordPoint.size() > 1) {
    Hep3Vector temp_midpoint;
    int temp_idr, temp_idp, temp_idm;
    bool extra_debug = false;
    for (i = 0; i < ordPoint.size(); i++) {
      ATH_MSG_DEBUG( "Ordered points : " << i
                    << " X: " << ordPoint[i].x()
                    << " Y: " << ordPoint[i].y()
                    << " Z: " << ordPoint[i].z() );

      if (i == ordPoint.size() - 1) break;

      temp_midpoint = (ordPoint[i] + ordPoint[i + 1]) / 2.;

      if (checkLBz(temp_midpoint.z())) {
        temp_idr = whichLBr(temp_midpoint.perp());
        temp_idp = (temp_midpoint.z() > 0) ? 1 : 2;
      } else if (checkEBz(temp_midpoint.z())) {
        temp_idr = whichEBr(temp_midpoint.perp());
        temp_idp = (temp_midpoint.z() > 0) ? 3 : 4;
      } else
        continue;

      temp_idm = whichModule(temp_midpoint);
      if (temp_idr < 0 || temp_idr > 2) continue;
      if (temp_idm < 0 || temp_idm > 63) continue;

      segPath.push_back((ordPoint[i] - ordPoint[i + 1]).mag());
      segPartition.push_back(temp_idp);
      segModule.push_back(temp_idm);
      segSampling.push_back(temp_idr);

    }

    ATH_MSG_DEBUG( "Number of segments : " << segPath.size() );

    for (i = 0; i < segPath.size(); i++) {
      if (segPath[i] > 1100) extra_debug = true;
      ATH_MSG_DEBUG( "Segment " << i
                    << " Path : " << segPath[i]
                    << " Partition : " << segPartition[i]
                    << " Module : " << segModule[i]
                    << " Sampling : " << segSampling[i] );

    }

    if (msgLvl(MSG::DEBUG) && extra_debug) {
      msg(MSG::DEBUG) << "Large segment p0: " << p0
                        << " p1 " << p1
                        << " p2 " << p2
                        << " p3 " << p3 << endmsg;

      msg(MSG::DEBUG) << "Intersections: " << npoints
                        << " cylinders: " << ncylint
                        << " vertical planes: " << nvpint
                        << " module planes: " << nmpint << endmsg;

      for (i = 0; i < ordPoint.size(); i++) {
        msg(MSG::DEBUG) << "Ordered points : " << i
                         << " X: " << ordPoint[i].x()
                         << " Y: " << ordPoint[i].y()
                         << " Z: " << ordPoint[i].z() << endmsg;
      }

      for (i = 0; i < segPath.size(); i++) {
        msg(MSG::DEBUG) << "Segment " << i
                          << " Path : " << segPath[i]
                          << " Partition : " << segPartition[i]
                          << " Module : " << segModule[i]
                          << " Sampling : " << segSampling[i] << endmsg;
      }
    }

  }

  ATH_MSG_DEBUG( "Leaving TrackSegmentIntersection" );
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkLBz(double x1) {
  if (x1 > m_tileDD_zLB[0] && x1 < m_tileDD_zLB[1])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkEBz(double x1) {
  return (checkEBAz(x1) || checkEBCz(x1));
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkEBAz(double x1) {
  if (x1 > m_tileDD_zEBA[0] && x1 < m_tileDD_zEBA[1])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkEBCz(double x1) {
  if (x1 > m_tileDD_zEBC[0] && x1 < m_tileDD_zEBC[1])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkLBr(double x1) {
  if (x1 < 0) x1 = -1.0 * x1;
  if (x1 > m_tileDD_radiusLB[0] && x1 < m_tileDD_radiusLB[3])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkEBr(double x1) {
  if (x1 < 0.0) x1 = -1.0 * x1;
  if (x1 > m_tileDD_radiusEB[0] && x1 < m_tileDD_radiusEB[3])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkLBr(double x1, uint8_t s1) {
  if (s1 > 3U) return false;
  if (x1 < 0.0) x1 = -1.0 * x1;
  if (x1 > m_tileDD_radiusLB[s1] && x1 < m_tileDD_radiusLB[s1 + 1])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
bool TileMuonFitter::checkEBr(double x1, uint8_t s1) {
  if (s1 > 3U) return false;
  if (x1 < 0.0) x1 = -1.0 * x1;
  if (x1 > m_tileDD_radiusEB[s1] && x1 < m_tileDD_radiusEB[s1 + 1])
    return true;
  else
    return false;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
int TileMuonFitter::whichEBr(double x1) {
  int radidx = -1;
  for (int i = 0; i < 3; i++) {
    if (checkEBr(x1, i)) radidx = i;
  }
  return radidx;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
int TileMuonFitter::whichLBr(double x1) {
  int radidx = -1;
  for (int i = 0; i < 3; i++) {
    if (checkLBr(x1, i)) radidx = i;
  }
  return radidx;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
int TileMuonFitter::whichModule(Hep3Vector tempvec) {
  double phi = tempvec.phi();
  int mod;
  if (phi < 0.0) phi += 2.0 * pi;
  if (phi > 2.0 * pi)
    mod = -1;
  else
    mod = int(phi * (64.0 / (2.0 * pi)));
  return mod;
}
// ********************************************************************
// ********************************************************************
// ********************************************************************
void TileMuonFitter::energyInTrack(std::vector<double> & etop, std::vector<double> & ebot
    , std::vector<IdentifierHash> & cells, int index) {

  int is;
  double distance_cut[2][3] = { { 300., 375., 860. }, { 750., 750., 1700. } };
  etop.resize(3);
  ebot.resize(3);
  for (is = 0; is < 3; is++) {
    etop[is] = 0.;
    ebot[is] = 0.;
  }
  Hep3Vector dataP;
  Hep3Vector lineP;

  size_t collItr = m_caloCells->indexFirstCellCalo(m_caloIndex);
  size_t lastColl = m_caloCells->indexLastCellCalo(m_caloIndex);

  for (; collItr != lastColl; ++collItr) {

    const CaloCell* cell = (*m_caloCells)[collItr];
    const TileCell* tilecell = dynamic_cast<const TileCell*>(cell);
    if (tilecell == 0) continue;

    Identifier cell_id = cell->ID();
    IdentifierHash cell_idhash = m_tileID->cell_hash(cell_id);
    CaloDetDescrElement *caloDDE = m_tileMgr->get_cell_element(cell_id);

    double volume = caloDDE->volume();
    if (volume == 0.0) {
      ATH_MSG_DEBUG( "Warning: Skipping cell with zero volume!" );
      continue;
    }

    int sample = m_tileID->sample(cell_id);
    int section = m_tileID->section(cell_id);
    if (section < 1 || section > 2 || sample < 0 || sample >= 3) break;
    dataP.set(caloDDE->x(), caloDDE->y(), caloDDE->z());
    lineP = m_theTrack->ClosestPoint(&dataP, m_linePar[index]);
    double distance = (dataP - lineP).mag();
    if (distance < distance_cut[section - 1][sample]) {
      cells.push_back(cell_idhash);
      if (caloDDE->y() > 0.0) etop[sample] += cell->energy();
      else ebot[sample] += cell->energy();
    }

  }   //end of CaloCellContainer cycle

}

// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildComTime(int fitok) {

  if (!m_beamType.compare("singlebeam") || !m_beamType.compare("collisions"))
    buildComTimeAtZequal0(fitok);
  else
    buildComTimeAtYequal0(fitok);

}
// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildComTimeAtYequal0(int fitok) {

  // Uses the track parameters to calculate:
  //   position and direction at horizontal at horizontal plane
  //  Since y = a + b*x and z = c + d*x
  // 		y == 0 => x = -a/b    &&   z = c - d*a/b
  //  Direction is (1,b,d)/sqrt(1 + b*b + d*d) 
  //
  // Then put the result in a ComTime object


  double p0, p1, p2, p3;
  double ztime;

  if (m_linePar.size() == 0) {
    p0 = 0.0;
    p1 = 0.0;
    p2 = 0.0;
    p3 = 0.0;
    ztime = 0.0;
  } else {
    p0 = m_linePar[0][0];
    p1 = m_linePar[0][1];
    p2 = m_linePar[0][2];
    p3 = m_linePar[0][3];
    ztime = m_zeroCrossingTime[0];
  }

  ATH_MSG_DEBUG( "Starting BuildComTime at y=0 m_linePar: "
                << p0 << " " << p1 << " " << p2 << " " << p3 );

  SG::WriteHandle<ComTime> theComTime (m_comTimeKey);
  StatusCode sc;
  if (!(fitok == 1) || p1 == 0.0) {
    sc = theComTime.record(std::make_unique<ComTime>());
  } else {
    Hep3Vector theZeroCrossingPosition(-1.0 * p0 / p1, 0.0, p2 - 1.0 * p3 * p0 / p1);
    Hep3Vector theDirection(1.0, p1, p3);
    theDirection = theDirection.unit();
    if ((m_reg1to2 && p1 > 0.0) || (!m_reg1to2 && p1 < 0.0)) theDirection *= -1.0;

    sc = theComTime.record(std::make_unique<ComTime>(0.0, ztime, theZeroCrossingPosition, theDirection));

    if (msgLvl(MSG::DEBUG)) {

      msg(MSG::DEBUG) << "Time: " << ztime
                      << " Position X: " << theZeroCrossingPosition.getX()
                      << " Position Y: " << theZeroCrossingPosition.getY()
                      << " Position Z: " << theZeroCrossingPosition.getZ() << endmsg;

      msg(MSG::DEBUG) << "Direction u: " << theDirection.getX()
                      << " Direction v: " << theDirection.getY()
                      << " Direction w: " << theDirection.getZ() << endmsg;
    }
  }

  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot record ComTime" );
  }
}

// ********************************************************************
// ********************************************************************
// ********************************************************************

void TileMuonFitter::buildComTimeAtZequal0(int fitok) {

  // Uses the track parameters to calculate:
  //   position and direction at vertical plane
  //  Since y = a + b*x and z = c + d*x
  // 		z == 0 => x = -c/d    &&   y = a - b*c/d
  //  Direction is (1,b,d)/sqrt(1 + b*b + d*d) 
  //
  // Then put the result in a ComTime object

  double p0, p1, p2, p3;
  double ztime;

  if (m_linePar.size() == 0) {
    p0 = 0.0;
    p1 = 0.0;
    p2 = 0.0;
    p3 = 0.0;
    ztime = 0.0;
  } else {
    p0 = m_linePar[0][0];
    p1 = m_linePar[0][1];
    p2 = m_linePar[0][2];
    p3 = m_linePar[0][3];
    ztime = m_zeroCrossingTime[0];
  }

  ATH_MSG_DEBUG( "Starting BuildComTime at z=0 m_linePar: "
                << p0 << " " << p1 << " " << p2 << " " << p3 );

  SG::WriteHandle<ComTime> theComTime (m_comTimeKey);
  StatusCode sc;
  if (!(fitok == 1) || p3 == 0.0) {
    sc = theComTime.record(std::make_unique<ComTime>());
  } else {
    Hep3Vector theZeroCrossingPosition(-1.0 * p2 / p3, p0 - 1.0 * p1 * p2 / p3, 0.0);
    Hep3Vector theDirection(1.0, p1, p3);
    theDirection = theDirection.unit();
    if ((m_reg1to2 && p3 > 0.0) || (!m_reg1to2 && p3 < 0.0)) theDirection *= -1.0;

    sc = theComTime.record(std::make_unique<ComTime>(0.0, ztime, theZeroCrossingPosition, theDirection));

    if (msgLvl(MSG::DEBUG)) {

      msg(MSG::DEBUG) << "Time: " << ztime
                      << " Position X: " << theZeroCrossingPosition.getX()
                      << " Position Y: " << theZeroCrossingPosition.getY()
                      << " Position Z: " << theZeroCrossingPosition.getZ() << endmsg;

      msg(MSG::DEBUG) << "Direction u: " << theDirection.getX()
                      << " Direction v: " << theDirection.getY()
                      << " Direction w: " << theDirection.getZ() << endmsg;
    }
  }

  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Cannot record ComTime" );
  }
}

// *****************************************************************************
// ************************ Hough Transform Routines ***************************
// *****************************************************************************

// Cartesian to Hough space
void TileMuonFitter::cart2hough(float x1, float y1, float x2, float y2, double &raio,
    double &angu) {

  // track should have at least a very small angle
  if (x1 == x2) x1 += 0.001F;
  if (y1 == y2) y1 += 0.001F;

  double a = (y1 - y2) / (x1 - x2);
  double b = y1 - a * x1;

  raio = fabs(b) / sqrt(a * a + 1.0);

  if (a < 0.0 && b > 0.0)
    angu = atan(a) + pi / 2.0;
  else if (a > 0.0 && b < 0.0)
    angu = atan(a) - pi / 2.0;
  else if (a < 0.0 && b < 0.0)
    angu = atan(a) - pi / 2.0;
  else
    angu = atan(a) + pi / 2.0;

  angu = angu - pi;
  if (angu < 0.0) angu = 2.0 * pi + angu;
}

// Hough to Cartesian space
void TileMuonFitter::hough2cart(double r, double a, double offset, double &aa, double &bb) {
  double x1, y1;
  double x2, y2;

  x1 = (r - sin(a + pi)) / cos(a + pi) - offset; // to translate back
  y1 = 1.0;

  x2 = (r + sin(a + pi)) / cos(a + pi) - offset;
  y2 = -1.0;

  aa = (y2 - y1) / (x2 - x1);
  bb = y1 - aa * x1;
}

float TileMuonFitter::dist2line(CellInfo &ci, float *pos, float *w) {
  float v0, v1, v2, d0, d1, d2;

  v0 = ci.x - pos[0];
  v1 = ci.y - pos[1];
  v2 = ci.z - pos[2];

  d0 = v1 * w[2] - w[1] * v2;
  d1 = w[0] * v2 - v0 * w[2];
  d2 = v0 * w[1] - w[0] * v1;

  return sqrt(d0 * d0 + d1 * d1 + d2 * d2);
}

void TileMuonFitter::points2dir(CellInfo &ci1, CellInfo &ci2, float *w) {
  w[0] = ci1.x - ci2.x;
  w[1] = ci1.y - ci2.y;
  w[2] = ci1.z - ci2.z;

  float invmod = 1.0F / sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
  w[0] *= invmod;
  w[1] *= invmod;
  w[2] *= invmod;
}

// count number of cells inside RoI (line between two cells)
unsigned int TileMuonFitter::CntCells(unsigned int index1, unsigned int index2, double &skew) {

  unsigned int size = m_cellInfo.size();
  double dist;

  // compute direction -------------------------------------------------------

  float p[3] = { m_cellInfo[index1].x, m_cellInfo[index1].y, m_cellInfo[index1].z };
  float w[3];
  points2dir(m_cellInfo[index1], m_cellInfo[index2], w);

  // find number of cells inside RoI -----------------------------------------

  unsigned int cnt = 0U;
  skew = 0.0;
  for (unsigned int i = 0U; i < size; i++) {
    if (!m_cellInfo[i].use || i == index1 || i == index2) continue;

    dist = dist2line(m_cellInfo[i], p, w);
    if (dist < m_cellInfo[i].dist * 0.5F) {
      cnt++;
      skew += dist;
    }
  }
  if (cnt > 0U) skew /= (int) cnt;

  return cnt;
}

// get the line between two cells with the highest number of cells inside RoI
bool TileMuonFitter::guessTrack(unsigned int &index1, unsigned int &index2) {
  unsigned int i, j;
  unsigned int NPoints = m_cellInfo.size();

  double skew, skew_min = 999999999.9;
  unsigned int cnt, cnt_max = 0U;

  for (i = 1; i < NPoints; i++) {
    for (j = 0; j < i; j++) {
      if (m_cellInfo[i].use && m_cellInfo[j].use) {
        cnt = CntCells(i, j, skew);
        if (cnt > cnt_max || (cnt == cnt_max && skew < skew_min)) {
          cnt_max = cnt;
          skew_min = skew;
          index1 = i;
          index2 = j;
        }
      }
    }
  }

  return (cnt_max >= MIN_NCELLS);
}

// build vector of CellInfo with all activated cells
unsigned int TileMuonFitter::buildCellInfoVector() {
  const float distance_cut[2][3] = { { 300.0, 820.0, 470.0 }, { 350.0, 540.0, 740.0 } };

  m_cellInfo.clear();
  m_linePar.clear();

  for (int i = 0; i < m_nCells; i++) {
    CellInfo cell;

    cell.x = m_cellPosition[i].getX() + SHIFT_X;
    cell.y = m_cellPosition[i].getY();
    cell.z = m_cellPosition[i].getZ() + SHIFT_Z;

    cell.e = m_cellEnergy[i];
    cell.ev = m_cellWeight[i];

    cell.use = true;
    cell.is_out = true;
    cell.track_index = -1;

    int sample = m_tileID->sample(m_tileID->cell_id(m_cellHash[i]));
    int section = m_tileID->section(m_tileID->cell_id(m_cellHash[i]));
    if (section < 1 || section > 2 || sample < 0 || sample >= 3)
      cell.dist = 500.0;
    else
      cell.dist = distance_cut[section - 1][sample];

    m_cellInfo.push_back(cell);
  }

  return m_cellInfo.size();
}

// select cells that are inside RoI
float TileMuonFitter::selectCells(float *p, float *w) {
  unsigned int NPoints = m_cellInfo.size();
  float toten = 0.0;
  unsigned int ntrack = m_linePar.size();

  for (unsigned int i = 0; i < NPoints; i++) {
    if (m_cellInfo[i].use == false || dist2line(m_cellInfo[i], p, w) > m_cellInfo[i].dist) {
      m_cellInfo[i].is_out = true;
    } else {
      m_cellInfo[i].is_out = false;
      m_cellInfo[i].use = false;
      m_cellInfo[i].track_index = ntrack;

      toten += m_cellInfo[i].e;
    }
  }

  return toten;
}

// if line is close to horizontal in plane zy, it should be halo event
bool TileMuonFitter::isHaloMuon(double azy) {
  if ((azy > 1.0 * pi / 2.0 - 6.0 * BIN_RES_AZY && azy < 1.0 * pi / 2.0 + 6.0 * BIN_RES_AZY)||
  (azy > 3.0*pi/2.0 - 6.0*BIN_RES_AZY && azy < 3.0*pi/2.0 + 6.0*BIN_RES_AZY))return true;
  else return false;
}

void TileMuonFitter::doHough(double &rxy, double &axy, double &rzy, double &azy) {
  unsigned int i, j;

  // dont need to compute other iteration for Halo events
  if (isHaloMuon(azy)) return;

  float nminrxy = rxy - 4.0 * BIN_RES_RXY;
  float nmaxrxy = rxy + 4.0 * BIN_RES_RXY;
  float nminaxy = axy - 4.0 * BIN_RES_AXY;
  float nmaxaxy = axy + 4.0 * BIN_RES_AXY;
  float nminrzy = rzy - 5.0 * BIN_RES_RZY;
  float nmaxrzy = rzy + 5.0 * BIN_RES_RZY;
  float nminazy = azy - 5.0 * BIN_RES_AZY;
  float nmaxazy = azy + 5.0 * BIN_RES_AZY;

  float weight;
  float aw = 0.0F;
  float arxy = 0.0F;
  float aaxy = 0.0F;
  float arzy = 0.0F;
  float aazy = 0.0F;

  unsigned int NPoints = m_cellInfo.size();
  for (i = 1; i < NPoints; i++) {
    for (j = 0; j < i; j++) {
      cart2hough(m_cellInfo[i].x, m_cellInfo[i].y, m_cellInfo[j].x, m_cellInfo[j].y, rxy, axy);
      cart2hough(m_cellInfo[i].z, m_cellInfo[i].y, m_cellInfo[j].z, m_cellInfo[j].y, rzy, azy);

      if (rxy < nminrxy || rxy > nmaxrxy || axy < nminaxy || axy > nmaxaxy || rzy < nminrzy
          || rzy > nmaxrzy || azy < nminazy || azy > nmaxazy) continue;

      weight = m_cellInfo[i].ev * m_cellInfo[j].ev;

      arxy += rxy * weight;
      aaxy += axy * weight;
      arzy += rzy * weight;
      aazy += azy * weight;
      aw += weight;
    }
  }

  const double inv_aw = 1. / aw;
  rxy = arxy * inv_aw;
  axy = aaxy * inv_aw;
  rzy = arzy * inv_aw;
  azy = aazy * inv_aw;
}

int TileMuonFitter::houghTrack() {

  ATH_MSG_DEBUG( "Fitting with Hough method" << m_nCells << " cells." );

  // Build vector with cell Infos --------------------------------------------
  bool compute = true;

  unsigned int NPoints = buildCellInfoVector();
  if (NPoints > MAX_NCELLS) compute = false;

  // loop to find tracks -----------------------------------------------------

  while (compute) {

    // Get direction with maximum number of cells --------------------------

    unsigned int index1, index2;
    if (!guessTrack(index1, index2)) break;

    // select cells which belong to the track ------------------------------

    float p[3] = { m_cellInfo[index1].x, m_cellInfo[index1].y, m_cellInfo[index1].z };
    float w[3];
    points2dir(m_cellInfo[index1], m_cellInfo[index2], w);

    float en = selectCells(p, w);
    if (en < MIN_ENERGY_MEV) continue;

    // compute reference direction -----------------------------------------

    double rxy, axy, rzy, azy;
    cart2hough(m_cellInfo[index1].x, m_cellInfo[index1].y, m_cellInfo[index2].x, m_cellInfo[index2].y, rxy, axy);
    cart2hough(m_cellInfo[index1].z, m_cellInfo[index1].y, m_cellInfo[index2].z, m_cellInfo[index2].y, rzy, azy);

    // compute Hough Transform ---------------------------------------------

    doHough(rxy, axy, rzy, azy);

    // compute track parameters --------------------------------------------

    double aa, bb, cc, dd;

    hough2cart(rxy, axy, SHIFT_X, bb, aa);
    hough2cart(rzy, azy, SHIFT_Z, dd, cc);

    addTrack(aa, bb, (aa - cc) / dd, bb / dd);

    ATH_MSG_DEBUG( "Minimum Value  aa: " << aa
                  << "   bb: " << bb
                  << "   cc: " << (aa - cc) / dd
                  << "   dd " << bb / dd );

    m_fitMinimum.push_back(1.0);
  }

  // prepare comtime ---------------------------------------------------------

  std::vector<double> X;
  std::vector<double> Y;
  std::vector<double> Z;

  X.resize(m_nCells);
  Y.resize(m_nCells);
  Z.resize(m_nCells);

  for (int i = 0; i < m_nCells; i++) {
    X[i] = m_cellPosition[i].getX();
    Y[i] = m_cellPosition[i].getY();
    Z[i] = m_cellPosition[i].getZ();
  }

  m_theTrack = new TileMuonTrackDistance(X, Y, Z, m_cellWeight);
  m_theTrack->SetWeighted(m_doWeighted);

  if (m_linePar.size() > 0) return 1;
  else return 0;
}

void TileMuonFitter::addTrack(double aa, double bb, double cc, double dd) {
  std::vector<double> par;
  par.resize(4);

  par[0] = aa;
  par[1] = bb;
  par[2] = cc;
  par[3] = dd;

  m_linePar.push_back(par);
}
