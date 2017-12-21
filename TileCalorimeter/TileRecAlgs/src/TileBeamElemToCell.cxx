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


// Tile includes
#include "TileRecAlgs/TileBeamElemToCell.h"
#include "TileIdentifier/TileHWID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileCell.h"

// Calo includes
#include "CaloIdentifier/TileTBID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

// Atlas include
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "Identifier/IdentifierHash.h"
// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h" 


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
  m_infoName = "TileInfo";

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

  ATH_CHECK( m_beamElemContainerKey.initialize() );
  ATH_CHECK( m_cellContainerKey.initialize() );
  
  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}


//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileBeamElemToCell::execute()
{

  // create new container
  SG::WriteHandle<CaloCellContainer> cellContainer(m_cellContainerKey);

  /* Register the set of TileCells to the event store. */
  ATH_CHECK( cellContainer.record(std::make_unique<CaloCellContainer>()) );

  //**
  //* Get TileBeamElems
  //**
  SG::ReadHandle<TileBeamElemContainer> beamElemContainer(m_beamElemContainerKey);

  if(!beamElemContainer.isValid()) {
   ATH_MSG_WARNING( "No signal from beam elements; container '"
                    << m_beamElemContainerKey.key() << "' doesn't exist in StoreGate" );

  } else {

    //* Iterate over BeamElem, creating new TileCells (or incrementing
    //* existing ones). Add each new TileCell to the CaloCellContainer.
      
    SelectAllObject<TileBeamElemContainer> beamElems(beamElemContainer.cptr());

    for (const TileBeamElem* beamElem : beamElems) {
	
      HWIdentifier adc_id = beamElem->adc_HWID();
      std::vector<unsigned int> amp = beamElem->get_digits();
      float ener = amp[0] * m_tileInfo->BeamElemChannelCalib(adc_id);

      Identifier cell_ID = beamElem->pmt_ID();

      // CaloDDE is NULL pointer here
      // store cell in container
      std::unique_ptr<TileCell> cell = std::make_unique<TileCell>(nullptr, cell_ID, ener);
      cellContainer->push_back(cell.release());
    }
  }
 

 // Execution completed.

 ATH_MSG_VERBOSE( "TileCell container registered to the TES with name"
                  << m_cellContainerKey.key());
    
    
  return StatusCode::SUCCESS;
    
}  


//****************************************************************************
//* Finalize
//****************************************************************************
    
StatusCode TileBeamElemToCell::finalize()
{
  
  ATH_MSG_INFO( "finalize() end" );
  
  return StatusCode::SUCCESS;
}
    
