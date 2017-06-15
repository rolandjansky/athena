/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawUtils/TileRegionSelectorTable.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include <fstream>

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
    std::cout << "Unable to retrieve TileDetDescrManager from DetectorStore" << std::endl;
    return;
  }

  TileCablingService* cabling = TileCablingService::getInstance();

  ToolHandle<TileROD_Decoder> dec("TileROD_Decoder/TileROD_Decoder");
  if((dec.retrieve()).isFailure()){
       ATH_MSG_FATAL("Could not find TileRodDecoder");
  }

  const TileHid2RESrcID* src = dec->getHid2reHLT();

  RegionSelectorLUT* ttLut= new RegionSelectorLUT (256);

  std::ofstream* ttmap=0;

  if(m_printTable) {
    ttmap = new std::ofstream(m_roiFileNameTile.c_str()) ;
    *ttmap <<
      "# ROD_ID coll_id  hash_id   etamin       etamax       phimin       phimax" << std::endl;
  }

  //double dphi=0.09817477;

  float etamin[4];
  etamin[1]=-1.0; etamin[0]=-0.1; etamin[3]=-1.60943;  etamin[2]=+0.708779;
  float etamax[4];
  etamax[1]=+0.1; etamax[0]=+1.0; etamax[3]=-0.708779; etamax[2]=+1.60943;


  int sam = 0; // ?
  int layer = 0; //?
  int firstone = tileHWID->drawerIdx( tileHWID->drawer_id(1,0) );
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      int coll = tileHWID->frag(ros, drawer);
      int rod = src->getRodID(coll);

      // magic number 20 coming from Sacha
      int hash = tileHWID->drawerIdx( tileHWID->drawer_id(ros,drawer) ) - firstone;

      double etami = etamin[ros-1];
      double etama = etamax[ros-1];

      Identifier cell_id;
      int index, pmt;
      for (int ch=0; ch<10; ch++){
      HWIdentifier channelID = tileHWID->channel_id(ros,drawer,ch);
      cell_id = cabling->h2s_cell_id_index(channelID,index,pmt);
      if ( index >= 0 ) break; // found a normal cell
      }
      int cell_hash = tileID->cell_hash(cell_id);
      CaloDetDescrElement* DDE = tileMgr->get_cell_element((IdentifierHash) cell_hash);
      float phimin = DDE->phi();
      if ( phimin < 0 ) phimin+=2*M_PI;
      float phimax = phimin;
      phimin -= DDE->dphi()/2.0;
      phimax += DDE->dphi()/2.0;

      if (m_printTable)
        *ttmap << std::setw(7)  << std::hex << rod  << " "
               << std::setw(6)  << std::hex << coll << " "
               << std::setw(7)  << std::hex << hash << " "
               << std::ios::fixed << std::setw(12) << etami  << " "
               << std::ios::fixed << std::setw(12) << etama  << " "
               << std::ios::fixed << std::setw(12) << phimin << " "
               << std::ios::fixed << std::setw(12) << phimax << std::endl;

      ttLut->additem(etami,etama,phimin,phimax,sam,layer,hash,rod);

    }
  }

  m_tileLUT = ttLut;

  recordMap(ttLut,tileKey).ignore();

  if (m_printTable)
    delete ttmap;


}


#ifdef DONTDO
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

  RegionSelectorLUT* ttLut= new RegionSelectorLUT (256);

  std::ofstream* ttmap=0;

  if(m_printTable) {
    ttmap = new std::ofstream(m_roiFileNameTile.c_str()) ;
    *ttmap << 
      "# ROD_ID coll_id  hash_id   etamin       etamax       phimin       phimax" << std::endl;
  }


  double dphi=0.09817477;

  float etamin[4];
  etamin[1]=-1.0; etamin[0]=-0.1; etamin[3]=-1.60943;  etamin[2]=+0.708779;
  float etamax[4];
  etamax[1]=+0.1; etamax[0]=+1.0; etamax[3]=-0.708779; etamax[2]=+1.60943;

  int ROD[4];
  ROD[0]=0x510000; ROD[1]=0x520000; ROD[2]=0x530000; ROD[3]=0x540000;

  int sam = 0; // ?
  int layer = 0; //?

  for (int i =0; i < 4; ++i ) {
    for (int j =0; j < 64; ++j ) {
      //unsigned int rod=ROD[i]+j/4;
      //unsigned int coll=(i+1)*256+j;
      //unsigned int hash=i*64+j;
      int rod;
      if ( m_2017RODs ) {
      int new_j=0;
      if ( (j%2)==1 ) new_j=1;
      rod=ROD[i]+2*(j/4)+new_j;
      } else {
      rod=ROD[i]+j/4;
      }

      int coll=(i+1)*256+j;
      int hash=i*64+j;

      double phimin=j*dphi;
      double phimax=(j+1)*dphi;
      double etami = etamin[i];
      double etama = etamax[i];

      if (m_printTable) 
	*ttmap << std::setw(7)  << std::hex << rod  << " "
	       << std::setw(6)  << std::hex << coll << " "
	       << std::setw(7)  << std::hex << hash << " "
	       << std::ios::fixed << std::setw(12) << etami  << " "
	       << std::ios::fixed << std::setw(12) << etama  << " "
	       << std::ios::fixed << std::setw(12) << phimin << " "
	       << std::ios::fixed << std::setw(12) << phimax << std::endl;
      
      ttLut->additem(etami,etama,phimin,phimax,sam,layer,hash,rod);

    }
  }

  m_tileLUT = ttLut;

  recordMap(ttLut,tileKey).ignore();

  if (m_printTable) 
    delete ttmap;
}

#endif

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

