/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileCellToNtuple.cxx
//  Author   : Zhifang
//  Created  : Jan, 2003
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi Includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h"

// Calo include
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "CaloEvent/CaloCellContainer.h"

//TileCalo include
#include "TileEvent/TileCell.h"
#include "TileEvent/TileCellContainer.h"
#include "TileRec/TileCellToNtuple.h"

const int max_chan=5216;

// Constructor & deconstructor
TileCellToNtuple::TileCellToNtuple(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_tileID(0)
  , m_tileTBID(0)
{
  m_cellContainer="MBTSContainer"; // "TileCellContainerKey";
  m_ntupleLoc="/FILE1/TileRec";
  m_ntupleID="h110";
  m_scinCells = true;
  
  declareProperty("TileCellContainer", m_cellContainer);    
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("ScinCells", m_scinCells);
}

TileCellToNtuple::~TileCellToNtuple()
{
}

// Alg standard interface function
StatusCode TileCellToNtuple::initialize() 
{ 
  ATH_MSG_INFO( "Initialization started" );

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileTBID) );

  m_ntupleLoc="/NTUPLES" + m_ntupleLoc;

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);
  if(!DirPtr) DirPtr = ntupleSvc()->createDirectory(m_ntupleLoc);
  if(!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
    return StatusCode::FAILURE;
  }
  m_ntuplePtr = ntupleSvc()->book(DirPtr.ptr(), m_ntupleID,
                                    CLID_ColumnWiseTuple, "TileCell-Ntuple");
  if(!m_ntuplePtr) {
    ATH_MSG_ERROR( "Failed to book ntuple: TileCellNtuple" );
    return StatusCode::FAILURE;
  }

  CHECK( m_ntuplePtr->addItem("TileCell/ncell", m_nchan, 0, max_chan) );
  CHECK( m_ntuplePtr->addItem("TileCell/totalE", m_tolE) );
  CHECK( m_ntuplePtr->addItem("TileCell/energy", m_nchan, m_energy) );

  if (!m_scinCells)
    CHECK( m_ntuplePtr->addItem("TileCell/enediff", m_nchan, m_enediff) );

  CHECK( m_ntuplePtr->addItem("TileCell/time", m_nchan, m_time) );
  CHECK( m_ntuplePtr->addItem("TileCell/quality", m_nchan, m_quality) );

  if (m_scinCells) {
    CHECK( m_ntuplePtr->addItem("TileCell/type", m_nchan, m_type, -1, 6) );
    CHECK( m_ntuplePtr->addItem("TileCell/module", m_nchan, m_module, 0, 7) );
    CHECK( m_ntuplePtr->addItem("TileCell/channel", m_nchan, m_channel, 0, 15) );
  } else {
    CHECK( m_ntuplePtr->addItem("TileCell/detector", m_nchan, m_detector, 0, 3) );
    CHECK( m_ntuplePtr->addItem("TileCell/side", m_nchan, m_side, -1, 1) );
    CHECK( m_ntuplePtr->addItem("TileCell/sample", m_nchan, m_sample, 0, 3) );
    CHECK( m_ntuplePtr->addItem("TileCell/eta", m_nchan, m_eta, 0, 15) );
    CHECK( m_ntuplePtr->addItem("TileCell/phi", m_nchan, m_phi, 0, 63) );
  }

  ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileCellToNtuple::execute()
{
  m_nchan=0;
  m_tolE=0.0;

  if (m_scinCells) {

    // step1: read Cells from TDS
    const TileCellContainer* CellCnt;
    CHECK( evtStore()->retrieve(CellCnt, m_cellContainer) );

    // step2: to fill items in ntuple
    TileCellContainer::const_iterator it = CellCnt->begin();
    TileCellContainer::const_iterator end = CellCnt->end();
    for(; it != end; ++it) {
      const TileCell *cell = (const TileCell *)(*it);
      m_tolE+=cell->energy();
      m_energy[m_nchan]=cell->energy();
      m_time[m_nchan]=cell->time();
      m_quality[m_nchan]=cell->quality();
    
      Identifier id=cell->ID();
      m_type[m_nchan]=m_tileTBID->type(id);
      m_channel[m_nchan]=m_tileTBID->channel(id);
      m_module[m_nchan]=m_tileTBID->module(id);
    
      m_nchan++;

      if (m_nchan >= max_chan) break;
    }
      
  } else {




    // step1: read Cells from TDS
    const CaloCellContainer* CellCnt;
    CHECK( evtStore()->retrieve(CellCnt, m_cellContainer) );

    // step2: to fill items in ntuple
    CaloCellContainer::const_iterator it = CellCnt->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator end = CellCnt->endConstCalo(CaloCell_ID::TILE);
    for(; it != end; ++it) {
      const TileCell *cell = (const TileCell *)(*it);
      m_tolE+=cell->energy();
      m_energy[m_nchan]=cell->energy();
      m_enediff[m_nchan]=cell->eneDiff();
      m_time[m_nchan]=cell->time();
      m_quality[m_nchan]=cell->quality();
    
      Identifier id=m_tileID->cell_id(cell->subcalo_hash());
      m_detector[m_nchan]=m_tileID->section(id);
      m_side[m_nchan]=m_tileID->side(id);
      m_sample[m_nchan]=m_tileID->sample(id);
      m_eta[m_nchan]=m_tileID->tower(id);
      m_phi[m_nchan]=m_tileID->module(id);
    
      m_nchan++;

      if (m_nchan >= max_chan) break;
    }      
  }
  
  // step3: commit ntuple
  CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}

StatusCode TileCellToNtuple::finalize()
{
  ATH_MSG_INFO( "finalize() successfully" );
  return StatusCode::SUCCESS;
}

