/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileFCSmStepToTileHitVec.cxx
//  Author   : Sergey Karpov <Sergey.Karpov@cern.ch>
//  Created  : Nov. 2013
//
//  DESCRIPTION:
//    Only implementation comments. Class level comments are in *.h file
//
//  HISTORY:
//    01 Nov 2013 - Created from TileCellIDCToCell.cxx
//    28 Nov 2013 - Work with U-shape was added (Sasha Solodkov)
//
//  BUGS:
//
//*****************************************************************************

// access to all containers
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CxxUtils/make_unique.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "TileGeoModel/TileDetectorTool.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCellContainer.h"

// Tile includes
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileInfo.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileFCSmStepToTileHitVec.h"
#include "TileG4Interfaces/ITileCalculator.h"

// ISF_FCS_Parametrization includes
#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"


//****************************************************************************
//* Constructor
//****************************************************************************
TileFCSmStepToTileHitVec::TileFCSmStepToTileHitVec(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_geoModSvc("GeoModelSvc",name)
  , m_tileID(nullptr)
  , m_tileInfo(nullptr)
  , m_tileMgr(nullptr)
  , m_calc("TileGeoG4SDCalc", name)
  , m_deltaT(0.5 * Gaudi::Units::nanosecond)
  , m_allHits(0)
  , m_Ushape(-1)
{
  m_FCS_StepInfo  = "MergedEventSteps";
  m_hitVec        = "TileHitVec_FCS";
  m_infoName      = "TileInfo";

  declareProperty( "GeoModelSvc", m_geoModSvc );
  declareProperty( "TileCalculator", m_calc);
  declareProperty("StepInfoCollection",   m_FCS_StepInfo, "Name of input container (default=TileHitCnt)");
  declareProperty("TileHitVector",        m_hitVec,       "Name of output hit vector (default=TileHitVec)");
  declareProperty("TileInfoName",         m_infoName,     "Name of TileInfo store (default=TileInfo");
  declareProperty("DeltaT",               m_deltaT,       "Minimal Time granularity in TileHit (default=0.5ns)");

}


//****************************************************************************
//* Destructor
//****************************************************************************
TileFCSmStepToTileHitVec::~TileFCSmStepToTileHitVec()
{}


//****************************************************************************
//* Initialization
//****************************************************************************
StatusCode TileFCSmStepToTileHitVec::initialize()
{
  ATH_CHECK( m_calc.retrieve() );
  // retrieve Tile detector manager, TileID helper and TileIfno from det store
  ATH_CHECK( detStore()->retrieve(m_tileMgr) );
  ATH_CHECK( detStore()->retrieve(m_tileID) );
  ATH_CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );
  ATH_CHECK(m_geoModSvc.retrieve());
  ATH_MSG_VERBOSE("GeoModelSvc initialized.");

  m_Ushape = this->getUshapeFromGM();
  if (m_Ushape < -2) {
    ATH_MSG_WARNING("Changing U-shape from " << m_Ushape << " to -2");
    m_Ushape = -2;
  }
  if (m_Ushape > 1 && m_Ushape < 10) {
    ATH_MSG_WARNING("Changing U-shape from " << m_Ushape << " to 1");
    m_Ushape = 1;
  }

  return StatusCode::SUCCESS;
}

int TileFCSmStepToTileHitVec::getUshapeFromGM() const {
  const TileDetectorTool* tileDetectorTool =
    dynamic_cast<const TileDetectorTool *>(m_geoModSvc->getTool("TileDetectorTool"));
  return (tileDetectorTool) ? tileDetectorTool->Ushape() : 0;
}


//****************************************************************************
//* Execution
//****************************************************************************
StatusCode TileFCSmStepToTileHitVec::execute()
{
  ATH_MSG_DEBUG( "Execution beginning" );

  m_allHits.clear();
  m_allHits.resize(m_tileID->pmt_hash_max(),0);
  int nHit = 0;
  float Etot = 0.0;
  int newHits = 0;
  int sum_size = 0;
  const double tile_radius[13] = { 2300.,
                                   2400., 2500., 2600.,
                                   2730., 2860., 2990.,
                                   3140., 3290., 3440.,
                                   3640., 3820.,
                                   3820.01 };

  const double M_PI_32 = M_PI/32.;
  const double M_PI_64 = M_PI/64.;
  const double TAN_PI_64 = tan(M_PI/64.);

  // size is amaller than master plate and there is a gap between modules
  const double size_correction = 2.75 + 1.5/2.;

  const ISF_FCS_Parametrization::FCS_StepInfoCollection* inCollect;
  std::unique_ptr<TileHitVector> FCS_hits = CxxUtils::make_unique<TileHitVector>();

  // Get FCS_StepInfo from FCS_StepInfoCollection
  ATH_CHECK(evtStore()->retrieve(inCollect,m_FCS_StepInfo)); //FIXME use a ReadHandle

  IdContext pmt_context = m_tileID->pmt_context();

  // Iterating over Steps, creating new TileHits
  for(const ISF_FCS_Parametrization::FCS_StepInfo* stepInfo : *inCollect) {
    Identifier hit_id = stepInfo->identify();
    if (m_tileID->is_tile(hit_id)) {
      double ene  = stepInfo->energy();
      double time = stepInfo->time();

      IdentifierHash hit_idhash;
      m_tileID->get_hash(hit_id, hit_idhash, &pmt_context);

      if (m_Ushape < 0) { // do not change anything for negative Ushape values

        if ( ! m_allHits[hit_idhash] ) {
          m_allHits[hit_idhash] = new TileHit(hit_id,ene,time);
          ++newHits;
          // ATH_MSG_INFO(
          ATH_MSG_VERBOSE(
                          "Iterating over Steps:   new hit with id " << m_tileID->to_string(hit_id,-1)
                          << " nHit=" << nHit << "  energy=" << ene << "  time=" << time);
        } else {
          m_allHits[hit_idhash]->add(ene,time,m_deltaT);
          // ATH_MSG_INFO(
          ATH_MSG_VERBOSE(
                          "Iterating over Steps: extra hit with id " << m_tileID->to_string(hit_id,-1)
                          << " nHit=" << nHit << "  energy=" << ene << "  time=" << time);
        }

      } else {

        int section = m_tileID->section(hit_id);
        int side    = m_tileID->side(hit_id);
        int module  = m_tileID->module(hit_id);
        int sample  = m_tileID->sample(hit_id);

        bool Ecell = (sample == 3);
        bool spC10 = (section == 3 && sample == 1 &&
                      ((module >= 38 && module <= 41) || (module >= 54 && module <= 57)));

        if ( Ecell || spC10 ) {
          if ( ! m_allHits[hit_idhash] ) {
            m_allHits[hit_idhash] = new TileHit(hit_id,ene,time);
            // ATH_MSG_INFO(
            ATH_MSG_VERBOSE(
                            "Iterating over Steps:   new E/C10 hit with id " << m_tileID->to_string(hit_id,-1)
                            << "  nHit=" << nHit << "  energy=" << ene );
            ++newHits;
          } else {
            m_allHits[hit_idhash]->add(ene,time,m_deltaT);
            // ATH_MSG_INFO(
            ATH_MSG_VERBOSE(
                            "Iterating over Steps: extra E/C10 hit with id " << m_tileID->to_string(hit_id,-1)
                            << "  nHit=" << nHit << "  energy=" << ene );
          }

        } else {  //cell have two pmt

          double x    = stepInfo->x(); // coordinates of the hit
          double y    = stepInfo->y(); // coordinates of the hit
          int pmt     = m_tileID->pmt(hit_id); // original pmt index
          double      edep[2];
          Identifier  cell_id = m_tileID->cell_id(hit_id);

          double phi_module=(module + 0.5) * M_PI_32;
          double phi_hit = atan2(y,x);
          if (phi_hit<0) phi_hit += 2*M_PI;
          double dphi = phi_hit - phi_module; // should be in the range [-pi/64, pi/64]
          if (dphi < -M_PI_64 || dphi > M_PI_64) {
            ATH_MSG_ERROR( "Displaced hit with id " << m_tileID->to_string(hit_id,-1)
                           << " x " << x << " y " << y
                           << " phi_module " << phi_module  << " phi_hit " << phi_hit
                           << " dphi " << dphi );
            // correcting dphi
            dphi -= trunc(dphi/M_PI_64) * M_PI_64;
          }

          double radius = sqrt(x*x+y*y);
          double radius_corrected = radius * cos(dphi);
          double yLocal = radius * sin(dphi); // if negative, we are closer to pmt with index 0
          double zLocal = radius * cos(dphi);
          double halfYLocal = radius_corrected * TAN_PI_64 - size_correction;
          if (fabs(yLocal) > halfYLocal) {
            ATH_MSG_ERROR( "Displaced hit with id " << m_tileID->to_string(hit_id,-1)
                           << " x " << x << " y " << y
                           << " radius_corr " << radius_corrected
                           << " yLocal " << yLocal  << " halfYlocal_corr " << halfYLocal
                           << " delta " << fabs(yLocal) - halfYLocal );
            // correcting yLocal
            yLocal = copysign(halfYLocal,yLocal);
          }

          // tile row or tile index - number from 0 to 10
          int tile_ind = std::lower_bound (tile_radius, tile_radius+12,radius_corrected)-tile_radius-1;
          if (tile_ind < 0 || tile_ind > 10 ) {
            ATH_MSG_ERROR( "Displaced hit with id " << m_tileID->to_string(hit_id,-1)
                           << " x " << x << " y " << y
                           << " radius " << radius << " corrected " << radius_corrected
                           << " tile_index " << tile_ind );
            // correcting tile index
            if (tile_ind<0) tile_ind = 0;
            if (tile_ind>10) tile_ind = 10;
          }
          TileHitData hitData;
          hitData.tileSize = tile_ind;
          hitData.nDetector = section;
          hitData.nSide = side;
          m_calc->pmtEdepFromFCS_StepInfo(hitData, ene, yLocal, halfYLocal, zLocal, m_Ushape);
          edep[0] = hitData.edep_down;
          edep[1] = hitData.edep_up;

          for (int pm=0; pm<2; ++pm) {
            // changing hit_id, - use proper pm index (0 or 1) for down/up pmt
            hit_id = m_tileID->pmt_id(cell_id,pm);
            m_tileID->get_hash(hit_id, hit_idhash, &pmt_context);

            double tim=time;
            if (pm != pmt) { // need to correct time
              const double ref_ind_tile = 1.59;
              const double inv_speed = ref_ind_tile / CLHEP::c_light;
              if (pmt) tim -= 2 * yLocal * inv_speed;
              else tim += 2 * yLocal * inv_speed;
            }

            if ( ! m_allHits[hit_idhash] ) {
              m_allHits[hit_idhash] = new TileHit(hit_id,edep[pm],tim,m_deltaT);
              ++newHits;
              // ATH_MSG_INFO(
              ATH_MSG_VERBOSE(
                              "Iterating over Steps:  new  hit with id " << m_tileID->to_string(hit_id,-1)
                              << "  nHit=" << nHit << "  pmt " << pmt << "  energy=" << edep[pm] << "  time="
                              << time << " + " << tim-time << " = " << tim);
            } else {
              m_allHits[hit_idhash]->add(edep[pm],tim,m_deltaT);
              // ATH_MSG_INFO(
              ATH_MSG_VERBOSE(
                              "Iterating over Steps: extra hit with id " << m_tileID->to_string(hit_id,-1)
                              << "  nHit=" << nHit << "  pmt " << pmt << "  energy=" << edep[pm] << "  time="
                              << time << " + " << tim-time << " = " << tim);
            }
          }

        }//cell have two pmt

      }//m_UshapeType >= 0

      ++nHit;
      Etot += ene;
    }//is_tile(hit_id)

  }//Iterating over Steps

  ATH_MSG_DEBUG( "End of Iterating over Steps:  nHit=" << nHit << "  newHits=" << newHits << "  Etot=" << Etot );
  // ATH_MSG_INFO( "End of Iterating over Steps:  nHit=" << nHit << "  newHits=" << newHits << "  Etot=" << Etot );
  nHit = 0;
  Etot = 0.0;

  // Addition of each TileHit to the TileHitVector
  std::vector<TileHit*>::const_iterator curr = m_allHits.begin();
  std::vector<TileHit*>::const_iterator iend = m_allHits.end();
  for ( ; curr != iend; ++curr) {
    if (*curr) {
      TileHit *pHit = (*curr);

      // ATH_MSG_INFO(
      ATH_MSG_VERBOSE(
                      "Iterating over Hits:  nHit=" << nHit << "  size="
                      << pHit->size() << "  energy=" << pHit->energy() );
      FCS_hits->push_back(*pHit);
      ++nHit;
      for (int ii=0; ii<pHit->size(); ++ii ) Etot += pHit->energy(ii);
      sum_size += pHit->size();
    }
  }

  // Register the set of TileHits to the event store
  CHECK( evtStore()->record(std::move(FCS_hits), m_hitVec, false ) );

  ATH_MSG_DEBUG( "Execution completed,  nHit=" << nHit << "  sum_size=" << sum_size << "  Etot=" << Etot );
  // ATH_MSG_INFO( "Execution completed,  nHit=" << nHit << "  sum_size=" << sum_size << "  Etot=" << Etot );

  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Finalize
//****************************************************************************
StatusCode TileFCSmStepToTileHitVec::finalize()
{
  ATH_MSG_INFO( "Finalized successfully" );

  return StatusCode::SUCCESS;
}

