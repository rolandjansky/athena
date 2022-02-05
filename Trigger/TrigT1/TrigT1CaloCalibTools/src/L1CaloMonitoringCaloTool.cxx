/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     L1CaloMonitoringCaloTool.cxx
// PACKAGE:  TrigT1CaloCalibTools
//
// AUTHOR:   Peter Faulkner
//
// ********************************************************************

#include <cmath>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

#include "TileEvent/TileCell.h"

#include "Identifier/Identifier.h"
#include "Identifier/Identifier32.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"

#include "TrigT1CaloCalibTools/L1CaloMonitoringCaloTool.h"

namespace LVL1 {

const int L1CaloMonitoringCaloTool::s_maxTowers;
const int L1CaloMonitoringCaloTool::s_nregions;
const int L1CaloMonitoringCaloTool::s_nsinThBins;


/*---------------------------------------------------------*/
L1CaloMonitoringCaloTool::L1CaloMonitoringCaloTool( const std::string & name ) :
    asg::AsgTool( name ),
    m_lvl1Helper(nullptr),
    m_sinTh(s_nsinThBins, 0.),
    m_events(0),
    m_lastRun(0),
    m_lastEvent(0),
    m_sideOffset(0),
    m_layerOffset(0),
    m_binOffset(s_nregions, 0),
    m_indexOffset(s_nregions, 0),
    m_etaShift(s_nregions, 0)
/*---------------------------------------------------*/
{
  declareProperty("CaloCellContainer", m_caloCellContainerName = "AllCalo");
  declareProperty("MappingVectorSize", m_maxCells = 188080);  
}

/*---------------------------------------------------------*/
StatusCode L1CaloMonitoringCaloTool:: initialize()
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO("Initializing " << name());

  ATH_CHECK( m_cellMatch.retrieve() );

  StatusCode sc;
  
  // Get LVL1 idhelper from detector store
  const CaloLVL1_ID* lvl1_id = 0;
  sc = detStore()->retrieve(lvl1_id, "CaloLVL1_ID");
  if (sc.isFailure() || !lvl1_id) {
    ATH_MSG_ERROR("Could not get CaloLVL1_ID helper !");
    return sc;
  }
  else {
    //msg(MSG::DEBUG) << " Found the CaloLVL1_ID helper. " << endmsg;
    m_lvl1Helper = (CaloLVL1_ID*) lvl1_id;
  }

  // Set up cell to tt mapping arrays
  const unsigned int maxCaloCells = 188080;
  if (m_maxCells > maxCaloCells) m_maxCells = maxCaloCells;
  if (m_maxCells > 0) {
    m_cellIds.reserve(m_maxCells);
    m_cellIds.assign(m_maxCells, 0);
    m_ttIdx.reserve(m_maxCells);
    m_ttIdx.assign(m_maxCells, 0);
  }
  m_energySums.reserve(s_maxTowers);
  m_quality.reserve(s_maxTowers);
  m_denom.reserve(s_maxTowers);

  // Set up arrays for indexing and sinTh
  int binsEta[s_nregions] = { 25, 3, 1, 4 };
  int binsPhi[s_nregions] = { 64, 32, 32, 16 };
  double grans[s_nregions] = { 0.1, 0.2, 0.1, 0.425 };
  double etaOffset[s_nregions] = { 0., 2.5, 3.1, 3.2 };
  int bin = 0;
  for (int r = 0; r < s_nregions; ++r) {
    if (r == 0) {
      m_binOffset[r]   = 0;
      m_indexOffset[r] = 0;
    } else {
      m_binOffset[r]   = binsEta[r-1] + m_binOffset[r-1];
      m_indexOffset[r] = binsEta[r-1]*binsPhi[r-1] + m_indexOffset[r-1];
    }
    m_etaShift[r] = 0;
    int shift = binsPhi[r] - 1;
    while (shift) {
      m_etaShift[r]++;
      shift>>=1;
    }
    for (int i = 0; i < binsEta[r]; ++i) {
      double eta = etaOffset[r] + (i+0.5)*grans[r];
      m_sinTh[bin++] = 1./(1000.*std::cosh(eta)); // include MeV->GeV conversion
    }
  }
  m_sideOffset = binsEta[s_nregions-1]*binsPhi[s_nregions-1] + m_indexOffset[s_nregions-1];
  m_layerOffset = 2*m_sideOffset;

  ATH_CHECK( m_cablingKey.initialize() );

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode L1CaloMonitoringCaloTool:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloMonitoringCaloTool::loadCaloCells()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) {
    ATH_MSG_DEBUG("in loadCaloCells()");
  }

  StatusCode sc;

  // Don't run more than once per event

  int eventNumber = 0;
  int runNumber   = 0;
  const EventInfo* evInfo = 0;
  sc = evtStore()->retrieve(evInfo);
  if (sc.isFailure()) {
    if (debug) {
    ATH_MSG_DEBUG("No EventInfo found");
    }
  } else {
    const EventID* evID = evInfo->event_ID();
    if (evID) {
      eventNumber = evID->event_number();
      runNumber   = evID->run_number();
      if (eventNumber == m_lastEvent && runNumber == m_lastRun) {
        return StatusCode::SUCCESS;
      }
    }
  }
  m_lastEvent = eventNumber;
  m_lastRun   = runNumber;

  // Retreive Calo Cell collection from SG

  const CaloCellContainer* caloCellContainer = 0;
  sc = evtStore()->retrieve(caloCellContainer, m_caloCellContainerName); 
  if(!sc.isSuccess() || !caloCellContainer) {
    ATH_MSG_WARNING("No CaloCellContainer found at AllCalo");
    return StatusCode::SUCCESS;
  }
  ++m_events;

  // Get Et and CaloQuality corresponding to TTs
  
  CaloCellContainer::const_iterator CaloCellIterator    = caloCellContainer->begin();
  CaloCellContainer::const_iterator CaloCellIteratorEnd = caloCellContainer->end();
  m_energySums.assign(s_maxTowers, 0.0);
  m_quality.assign(s_maxTowers, 0.0);
  m_denom.assign(s_maxTowers, 0.0);
  unsigned int cellIdsIndex = 0;

  SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey);
  
  for (; CaloCellIterator != CaloCellIteratorEnd; ++CaloCellIterator) {
      
    const CaloCell* caloCell(*CaloCellIterator);
    const Identifier cellId(caloCell->ID());
    const Identifier invalidId(0);
    Identifier ttId1(0);
    Identifier ttId2(0);

    // Use cell ordering to avoid unnecessary calls to mapping tools

    int index1 = s_maxTowers;
    int index2 = s_maxTowers;
    const unsigned int cellId32 = cellId.get_identifier32().get_compact();
    if (m_events == 1) {
      m_cellMatch->matchCell2Tower(**cabling, caloCell, ttId1, ttId2);
      if (ttId1 != invalidId) index1 = towerIndex(ttId1);
      if (ttId2 != invalidId) index2 = towerIndex(ttId2);
      if (cellIdsIndex < m_maxCells-1) {
        m_cellIds[cellIdsIndex] = cellId32;
        m_ttIdx[cellIdsIndex++] = index1;
        if (ttId2 != invalidId) {
          m_cellIds[cellIdsIndex] = cellId32;
          m_ttIdx[cellIdsIndex++] = index2;
        }
      }
    } else {
      if ((cellIdsIndex < m_maxCells-1) && (m_cellIds[cellIdsIndex] == cellId32)) {
        index1 = m_ttIdx[cellIdsIndex++];
        if (m_cellIds[cellIdsIndex] == cellId32) index2 = m_ttIdx[cellIdsIndex++];
      } else {
        m_cellMatch->matchCell2Tower(**cabling, caloCell, ttId1, ttId2);
        if (ttId1 != invalidId) index1 = towerIndex(ttId1);
        if (ttId2 != invalidId) index2 = towerIndex(ttId2);
      }
    }

    // Get Et/Energy and CaloQuality

    if (index1 < s_maxTowers) {
      const float energy = caloCell->energy();
      const CaloDetDescrElement* caloDDE = caloCell->caloDDE();
      if (index2 < s_maxTowers) { // Tile D cells
        const float energy2 = energy*0.5;
        m_energySums[index1] += energy2;
        m_energySums[index2] += energy2;
	const TileCell* tileCell = dynamic_cast<const TileCell*>(caloCell);
	if (tileCell && (tileCell->provenance() & 0x8080)) {
	  const float qual = energy2 * std::max(tileCell->qual1(), tileCell->qual2());
	  m_quality[index1] += qual;
	  m_quality[index2] += qual;
	  m_denom[index1] += energy2;
	  m_denom[index2] += energy2;
        }
      } else {
        if (caloDDE->is_lar_fcal()) {
          m_energySums[index1] += energy*caloDDE->sinTh();
        } else m_energySums[index1] += energy;
        if (caloDDE->is_tile()) {
	  const TileCell* tileCell = dynamic_cast<const TileCell*>(caloCell);
	  if (tileCell && (tileCell->provenance() & 0x8080)) {
	    m_quality[index1] += energy * std::max(tileCell->qual1(), tileCell->qual2());
	    m_denom[index1] += energy;
          }
	} else {
	  if (caloCell->provenance() & 0x2000) {
	    m_quality[index1] += (energy * caloCell->quality());
	    m_denom[index1] += energy;
          }
        }
      }
    }
  }

  // Compute final quantities
  
  for (int index = 0; index < s_maxTowers; ++index) {
    if (region(index) == 3) { // FCAL
      m_energySums[index] *= 1e-3;
    } else {
      const int bin = etaBin(index);
      m_energySums[index] *= m_sinTh[bin];
    }
    if ( m_denom[index] != 0.) {
      m_quality[index] /= m_denom[index];
    } else {
      m_quality[index] = -1e6; // error value
    }
  }

  return StatusCode::SUCCESS;     
}

// Return Et

float L1CaloMonitoringCaloTool::et(const Identifier& ttId) const
{
  float e = 0.;
  const int index = towerIndex(ttId);
  if (index < s_maxTowers) e = m_energySums[index];
  return e;
}

// Return Calo Quality

float L1CaloMonitoringCaloTool::caloQuality(const Identifier& ttId) const
{
  float q = -1e6;
  const int index = towerIndex(ttId);
  if (index < s_maxTowers) q = m_quality[index];
  return q;
}

// Return index to TT-Calo energy sum vector

int L1CaloMonitoringCaloTool::towerIndex(const Identifier& ttId) const
{
  const int side   = m_lvl1Helper->pos_neg_z(ttId);
  const int layer  = m_lvl1Helper->sampling(ttId);
  const int region = m_lvl1Helper->region(ttId);
  const int ieta   = m_lvl1Helper->eta(ttId);
  const int iphi   = m_lvl1Helper->phi(ttId);
  int index = 0;
  index = m_indexOffset[region] + (ieta<<(m_etaShift[region])) + iphi;
  if (side < 0)  index += m_sideOffset;
  if (layer > 0) index += m_layerOffset;
  return index;
}

// Return the region corresponding to index

int L1CaloMonitoringCaloTool::region(const int index) const
{
  int rg = 0;
  int ix = index;
  if (ix >= m_layerOffset) ix -= m_layerOffset;
  if (ix >= m_sideOffset)  ix -= m_sideOffset;
  for (int r = s_nregions-1; r >= 0; --r) {
    if (ix >= m_indexOffset[r]) {
      rg = r;
      break;
    }
  }
  return rg;
}

// Return the eta bin corresponding to index

int L1CaloMonitoringCaloTool::etaBin(const int index) const
{
  int bin = 0;
  int ix = index;
  if (ix >= m_layerOffset) ix -= m_layerOffset;
  if (ix >= m_sideOffset)  ix -= m_sideOffset;
  for (int r = s_nregions-1; r >= 0; --r) {
    if (ix >= m_indexOffset[r]) {
      const int ieta = (ix - m_indexOffset[r])>>(m_etaShift[r]);
      bin = m_binOffset[r] + ieta;
      break;
    }
  }
  return bin;
}

} // end namespace
