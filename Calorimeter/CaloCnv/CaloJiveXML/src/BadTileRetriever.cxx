/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/BadTileRetriever.h"

#include "AthenaKernel/Units.h"

#include "EventContainers/SelectAllObject.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileCellContainer.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

using Athena::Units::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  BadTileRetriever::BadTileRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("BadTILE"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_calo_id_man  = CaloIdManager::instance();
    m_calocell_id  = m_calo_id_man->getCaloCell_ID();
    m_sgKey = "AllCalo"; 
     
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("CellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveTILE" , m_tile = true);
    declareProperty("DoBadTile",     m_doBadTile = false);
  
    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode BadTileRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endmsg;

    return StatusCode::SUCCESS;	
  }
  
  /**
   * Tile data retrieval from chosen collection
   */
  StatusCode BadTileRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endmsg;

    const CaloCellContainer* cellContainer;
    if (!evtStore()->retrieve(cellContainer,m_sgKey))
      {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << 
	  "Could not retrieve Calorimeter Cells for Tile " << endmsg;
//        return StatusCode::SUCCESS;
      }

    if(m_tile){
      DataMap data = getBadTileData(cellContainer);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Error reading Tile data" << endmsg;
//        return StatusCode::SUCCESS;
      } else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Tile retrieved" << endmsg;
      }
    }
    //Tile cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve Tile bad cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap BadTileRetriever::getBadTileData(const CaloCellContainer* cellContainer) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getBadTileData()" << endmsg;
    char rndStr[30];
    DataMap DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect energyVec; energyVec.reserve(cellContainer->size());

//    m_sub; m_sub.reserve(cellContainer->size());
    m_sub.clear();
//     msg(MSG::INFO)  << "Size of CellC =  " << cellContainer->size() << endmsg;
	  
//Loop Over CaloCellContainer to retrieve TileCell information

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::TILE);

    if (m_doBadTile==true) {

      double energyGeV;
      //int cellInd;

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Start iterator loop over cells" << endmsg;

      for(;it1!=it2;it1++){
      
	if( !(*it1)->badcell() ) continue;
	//if( (*it1)->energy() < m_cellThreshold ) continue;   
	Identifier cellid = (*it1)->ID();   
	//IdentifierHash cell_hash = m_calocell_id->calo_cell_hash( cellid );
	//cellInd = cellContainer->findIndex(cell_hash);
	calcTILELayerSub(cellid);

        energyGeV = (*it1)->energy()*(1./GeV);
	energyVec.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
	
	idVec.push_back(DataType( (Identifier::value_type)(*it1)->ID().get_compact() ));
	phi.push_back(DataType((*it1)->phi()));
	eta.push_back(DataType((*it1)->eta()));

      } // end cell iterator

    } // doBadTile
    
    // write values into DataMap
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["sub"] = m_sub;
    DataMap["id"] = idVec;
    DataMap["energy"] = energyVec;
    
    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // getTileData

  //-----------------------------------------------------------------------------------------------------
    
  void BadTileRetriever::calcTILELayerSub(Identifier& cellid)
  {
    if(m_calocell_id->is_tile_barrel(cellid))
      {
	if(m_calocell_id->is_tile_negative(cellid))
	  m_sub.push_back(DataType(2));
	else
	  m_sub.push_back(DataType(3));
      }
    else if(m_calocell_id->is_tile_extbarrel(cellid))
      {
	if(m_calocell_id->is_tile_negative(cellid))
	  m_sub.push_back(DataType(0));
	else
	  m_sub.push_back(DataType(5));
      }
    //else in ITC or scint
    else
      {
	if(m_calocell_id->is_tile_negative(cellid))
	  m_sub.push_back(DataType(1));
	else
	  m_sub.push_back(DataType(4));
      }
  }

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
