/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawUtils/TileRegionSelectorTable.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include <fstream>
#include <iomanip>
#include <cmath>

#include "RegionSelector/RegionSelectorLUT.h" 
#include "RegionSelector/StoreGateRS_ClassDEF.h" 

//abc static const InterfaceID 
//abc IID_ILArRegionSelectorTable("LArRegionSelectorTable", 1, 0);
//abc const InterfaceID& LArRegionSelectorTable::interfaceID( ) 
//abc { return IID_ILArRegionSelectorTable; }

//TileRegionSelectorTable::TileRegionSelectorTable
//(const std::string& name, ISvcLocator* pSvcLocator): Algorithm(name, pSvcLocator), 

//abc (const std::string& type,const std::string& name,const IInterface* parent)
//abc : AlgTool(type,name,parent),

//(const std::string& name, ISvcLocator* pSvcLocator): 
//Service(name, pSvcLocator), 

//#define USE_CELL_PHI 1
//#define USE_MODULE_PHI 1

TileRegionSelectorTable::TileRegionSelectorTable (const std::string& type,
						  const std::string& name,
						  const IInterface* parent)
  :  AthAlgTool(type,name,parent),  
     m_tileLUT(nullptr),
     m_printTable(false),
     m_testTable(false),
     m_roiFileNameTile("TileMapIdAndHash.txt")
{
  declareInterface<IRegionLUT_Creator>(this);
  declareProperty("OutputFileTile", m_roiFileNameTile);
  declareProperty("PrintTable", m_printTable);
  declareProperty("TestTable", m_testTable);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TileRegionSelectorTable::initialize(){

  ATH_MSG_INFO ( "initialize()" );

  ATH_MSG_INFO ( "Algorithm Properties" );
  ATH_MSG_INFO ( " Print Table:      " 
                 << ((m_printTable) ? "true" : "false") );
  ATH_MSG_INFO ( " Test Table:      " 
                 << ((m_testTable) ? "true" : "false") );

  if (m_printTable) 
    ATH_MSG_INFO ( " Output File:  " << m_roiFileNameTile );
  
  fillMaps();

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TileRegionSelectorTable::execute() {

  if (m_testTable) ATH_CHECK( testMaps() );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TileRegionSelectorTable::finalize() {

  ATH_MSG_INFO ( "finalize()" );
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

const RegionSelectorLUT *
TileRegionSelectorTable::getLUT(std::string subSyst) const
{
  if(subSyst=="TILE") {
      return m_tileLUT;
  }
  else {
    ATH_MSG_WARNING ( "getLUT called with wrong argument, returning Tile by default" );
    ATH_MSG_WARNING ( "valid list of arguments is TILE" );
    return m_tileLUT;
  }

}

void
TileRegionSelectorTable::fillMaps()
{
  // check on the map if job not already done by s.o. else
  std::string tileKey = "TileRegionSelectorLUT";
  StatusCode sc = detStore()->contains< RegionSelectorLUT >(tileKey);
  if (sc == StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " RegionSelectorLUT " << tileKey
                    << " already exists => do nothing " );
    return;
  }

  // Get the TileID helper from the detector store
  const TileID* tileID;
  if (detStore()->retrieve(tileID, "TileID").isFailure()) {
    ATH_MSG_ERROR( "Could not get TileID helper !");
    return;
  }

  // Get the TileHWID helper from the detector store
  const TileHWID* tileHWID;
  if (detStore()->retrieve(tileHWID, "TileHWID").isFailure()) {
    ATH_MSG_ERROR( "Could not get TileHWID helper !");
    return;
  }

  // Get pointer to TileDetDescrManager
  const TileDetDescrManager* tileMgr;
  if (detStore()->retrieve(tileMgr).isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve TileDetDescrManager from DetectorStore" );
    return;
  }

  ToolHandle<TileROD_Decoder> dec("TileROD_Decoder/TileROD_Decoder");
  if((dec.retrieve()).isFailure()){
    ATH_MSG_FATAL("Could not find TileRodDecoder");
    return;
  }

  const TileHid2RESrcID* src = dec->getHid2reHLT();

  RegionSelectorLUT* ttLut= new RegionSelectorLUT (256);

  std::ofstream* ttmap=0;

  if(m_printTable) {
    ttmap = new std::ofstream(m_roiFileNameTile.c_str()) ;
    *ttmap <<
      "# ROD_ID coll_id  hash_id   etamin       etamax       phimin       phimax" << std::endl;
  }

  enum Partition { Ancillary = 0, LBA = 1, LBC = 2, EBA = 3, EBC = 4 };
  // hard-coded etamin/etamax - ignoring slightly different positions of EBA end ENC partitions
  std::vector<double> etamin(TileCalibUtils::MAX_ROS);
  etamin[LBA]=-0.1;
  etamin[LBC]=-1.0;
  etamin[EBA]=+0.708779;
  etamin[EBC]=-1.60943;
  std::vector<double> etamax(TileCalibUtils::MAX_ROS);
  etamax[LBA]=+1.0;
  etamax[LBC]=+0.1;
  etamax[EBA]=+1.60943;
  etamax[EBC]=-0.708779;

#if (defined USE_CELL_PHI)
  TileCablingService* cabling = TileCablingService::getInstance();
#elif (defined USE_MODULE_PHI)
#else
  double dphi = 2 * M_PI / TileCalibUtils::MAX_DRAWER; // 0.09817477;
#endif

  int sam = 0; // ?
  int layer = 0; //?
  int firstone = tileHWID->drawerIdx( tileHWID->drawer_id(LBA,0) ); // hash index of first real drawer (LBA01)
  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      int hash = tileHWID->drawerIdx( tileHWID->drawer_id(ros,drawer) ) - firstone;
      if (hash<0) continue;
      
      int coll = tileHWID->frag(ros, drawer);
      int rod = src->getRodID(coll);

      double etami = etamin[ros];
      double etama = etamax[ros];

#if (defined USE_CELL_PHI)
      Identifier cell_id;
      int index, pmt;
      for (int ch=0; ch<cabling->getMaxChannels(); ch++){
         HWIdentifier channelID = tileHWID->channel_id(ros,drawer,ch);
         cell_id = cabling->h2s_cell_id_index(channelID,index,pmt);
         if ( index >= 0 ) break; // found a normal cell
      }

      int cell_hash = tileID->cell_hash(cell_id);
      CaloDetDescrElement* DDE = tileMgr->get_cell_element((IdentifierHash) cell_hash);
      double phimin = DDE->phi() - DDE->dphi()/2.0;
      if ( phimin < 0.0 ) phimin+=2*M_PI;
      double phimax = phimin + DDE->dphi();
#elif (defined USE_MODULE_PHI)
      // alternative approach
      int section = (ros==LBA || ros==LBC) ? TileID::BARREL : TileID::EXTBAR;
      int side = (ros==LBA || ros==EBA) ? TileID::POSITIVE : TileID::NEGATIVE;
      Identifier moduleID = tileID->module_id(section,side,drawer);
      CaloDetDescriptor *moduleDDE = tileMgr->get_module_element(moduleID);
      double phimin = moduleDDE->calo_phi_min();
      double phimax = moduleDDE->calo_phi_max();
      if ( phimin < 0.0 ) {
        phimin+=2*M_PI;
        if ( phimax < phimin ) phimax+=2*M_PI;
      }
#else
      // OLD method - we can predict phi boundaries from module number
      double phimin = drawer * dphi;
      double phimax = (drawer+1) * dphi;
#endif

      if (m_printTable){
        ttmap->precision(7);
        *ttmap << std::hex
               << std::setw(7) << rod  << " "
               << std::setw(6) << coll << " "
               << std::setw(7) << hash << " "
               << std::dec
               << std::setw(12) << etami  << " "
               << std::setw(12) << etama  << " ";
        ttmap->precision(17);
        *ttmap << std::setw(22) << phimin << " "
               << std::setw(22) << phimax
               << std::endl;
      }

      ttLut->additem(etami,etama,phimin,phimax,sam,layer,hash,rod);

    }
  }

  m_tileLUT = ttLut;

  recordMap(ttLut,tileKey).ignore();

  if (m_printTable)
    delete ttmap;


}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
TileRegionSelectorTable::recordMap(RegionSelectorLUT* RSlut, std::string key) 
{
  static const bool SETCONST(false);
  StatusCode sc = detStore()->contains< RegionSelectorLUT >(key);
  if (sc == StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " RegionSelectorLUT " << key 
                    << " already exists " );
  } else {
    ATH_CHECK( detStore()->record(RSlut, key,SETCONST) );
    ATH_MSG_INFO( "TileRegionSelectorLUT successfully saved in detector Store" 
                  << key );
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode 
TileRegionSelectorTable::testMaps() {

  std::string tileKey = "TileRegionSelectorLUT";
  const RegionSelectorLUT* ttLutTile;
  ATH_CHECK( detStore()->retrieve(ttLutTile, tileKey) );
  ATH_MSG_DEBUG ( " Found RegionSelectorLUT for Tile" << tileKey );
  return StatusCode::SUCCESS;
}

