/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileBeamElemToCell.cxx
//  Author   : Anna Lupi
//  Created  : July 2004
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Atlas include
#include "AthenaKernel/errorcheck.h"

// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h" 

// Calo includes
#include "CaloIdentifier/TileTBID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "Identifier/IdentifierHash.h"

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileCell.h"
#include "TileRecAlgs/TileBeamElemToCell.h"

// C++ STL includes
#include <map>

//
// Constructor
//
TileBeamElemToCell::TileBeamElemToCell(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileTBID(0)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_tileMgr(0)
{
  m_beamElemContainer = "TileBeamElemCnt";
  m_cellContainer = "TileTBCellCnt";
  m_infoName = "TileInfo";

  declareProperty("TileBeamElemContainer", m_beamElemContainer);   // Name of input container
  declareProperty("TileTBCellContainer", m_cellContainer);         // Name of output container
  declareProperty("TileInfoName", m_infoName);                     // name of TileInfo store
}

TileBeamElemToCell::~TileBeamElemToCell()
{}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileBeamElemToCell::initialize() 
{
  
  // retrieve Tile detector manager, TileID helper and TileIfno from det store
  CHECK( detStore()->retrieve(m_tileMgr) );
  CHECK( detStore()->retrieve(m_tileTBID) );
  CHECK( detStore()->retrieve(m_tileHWID) );
  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );
  
  ATH_MSG_INFO( "TileBeamElemToCell initialization completed" );

  return StatusCode::SUCCESS;
}


//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileBeamElemToCell::execute()
{

  // create new container
  CaloCellContainer* cells = new CaloCellContainer;

  //**
  //* Get TileBeamElems
  //**
    
  const TileBeamElemContainer* beamElemCnt;

 if(evtStore()->retrieve(beamElemCnt, m_beamElemContainer).isFailure()) {
   ATH_MSG_WARNING( "No signal from beam elements; container '"
       << m_beamElemContainer << "' doesn't exist in StoreGate" );

  } else {

    //* Iterate over BeamElem, creating new TileCells (or incrementing
    //* existing ones). Add each new TileCell to the CaloCellContainer.
      
    SelectAllObject<TileBeamElemContainer> beamElems(beamElemCnt);
    SelectAllObject<TileBeamElemContainer>::const_iterator itr=beamElems.begin(); 
    SelectAllObject<TileBeamElemContainer>::const_iterator end=beamElems.end(); 

    for( ; itr != end; ++itr) {
	
      HWIdentifier adc_id = (*itr)->adc_HWID();
      std::vector<unsigned int> amp = (*itr)->get_digits();
      float ener = amp[0] * m_tileInfo->BeamElemChannelCalib(adc_id);

      Identifier cell_ID = (*itr)->pmt_ID();
      TileCell * pCell = new TileCell(NULL,cell_ID,ener); // CaloDDE is NULL pointer here
	
      cells->push_back(pCell); // store cell in container
    }
  }
 
  /* Register the set of TileCells to the event store. */
    
 CHECK( evtStore()->record(cells, m_cellContainer, false) );

 // Execution completed.

 ATH_MSG_VERBOSE( "TileCell container registered to the TES with name"
      << m_cellContainer);
    
    
  return StatusCode::SUCCESS;
    
}  


//****************************************************************************
//* Finalize
//****************************************************************************
    
StatusCode TileBeamElemToCell::finalize()
{
  
  ATH_MSG_INFO( "TileBeamElemToCell::finalize() end" );
  
  return StatusCode::SUCCESS;
}
    
