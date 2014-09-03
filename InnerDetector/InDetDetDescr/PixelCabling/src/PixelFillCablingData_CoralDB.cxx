/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelFillCablingData_CoralDB.cxx
//   Implementation file for class PixelFillCablingData_CoralDB
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// reading the final mapping file and depending on the selected bandwidth filling the maps
// - FillMaps() - call intialiseFromFile() with the correct file name
// - initialiseFromFile() - reading the cosmic mapping file and depending on the selected bandwidth filling the maps
// - commentOrBlankLine() - for finding whether the line is valid data or just a comment or blank line
///////////////////////////////////////////////////////////////////
// 14/05/2008 Florian Hirsch
///////////////////////////////////////////////////////////////////
#include "PixelFillCablingData_CoralDB.h"

#include <iostream>
#include <vector>
#include "CoralBase/MessageStream.h"
#include "eformat/SourceIdentifier.h"  // change to new eformat v3
#include "InDetIdentifier/PixelID.h"

// Classes
#include "Identifier/Identifier.h"
#include "PixelCablingData.h"
#include "CoralDB/CoralDB.h"
#include "CoralDB/Connection.h"
#include "CoralDB/Alias.h"

using eformat::helper::SourceIdentifier; 

using namespace std;

static const InterfaceID IID_IPixelFillCablingData_CoralDB("PixelFillCablingData_CoralDB", 1, 0);

////////////////////////
// constructor
////////////////////////
PixelFillCablingData_CoralDB::PixelFillCablingData_CoralDB( const std::string& type, const std::string& name,const IInterface* parent): AthAlgTool(type,name,parent), m_idHelper(0), m_cabling(0), m_coraldb(0), m_optCompoundTag(0)
{
   declareInterface< PixelFillCablingData_CoralDB >( this );   
   //   declareProperty("MappingFile", m_final_mapping_file = "Pixels_Atlas_IdMapping.dat");
   declareProperty("Bandwidth", m_bandwidth = 0);
}
	 
////////////////////////
// destructor
////////////////////////
PixelFillCablingData_CoralDB::~PixelFillCablingData_CoralDB()
{
  if (m_cabling != NULL){
    delete m_cabling;
    m_cabling=NULL;
  }
  if (m_coraldb != NULL){
    delete m_coraldb;
    m_coraldb = NULL;
  }
  if (m_optCompoundTag != NULL){
    delete m_optCompoundTag;
    m_optCompoundTag = NULL;
  }

}

////////////////////////
// interfaceID
////////////////////////
const InterfaceID& PixelFillCablingData_CoralDB::interfaceID()
{
   return IID_IPixelFillCablingData_CoralDB;
}

////////////////////////
// initialize
////////////////////////
StatusCode PixelFillCablingData_CoralDB::initialize( )
{
   StatusCode sc;
   msg(MSG::DEBUG) << "PixelFillCablingData_CoralDB::initialize" <<endreq;
  
   // Get the PixelID Helper
   if (detStore()->retrieve(m_idHelper, "PixelID").isFailure()) {
     msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
   }
   m_cntxpixel = m_idHelper->wafer_context();

   return sc;
}

////////////////////////
// finalize
////////////////////////
StatusCode PixelFillCablingData_CoralDB::finalize()
{
  /*
  if (m_coraldb != NULL){
    delete m_coraldb;
    m_coraldb = NULL;
  }
  */
  if (m_optCompoundTag != NULL){
  delete m_optCompoundTag;
  m_optCompoundTag = NULL;
  }
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

////////////////////////
// FillMaps - call intialiseFromFile() with the correct file name
////////////////////////
PixelCablingData* PixelFillCablingData_CoralDB::FillMaps()
{
  m_coraldb = new CoralDB::CoralDB(m_coraldbconnstring, coral::Update,coral::Error, CoralDB::CoralDB::AUTOCOMMIT);
  m_coraldb->setObjectDictionaryTag(m_dictTag);
  m_coraldb->setConnectivityTag(m_connTag);
  m_coraldb->setDataTag(m_connTag);   
  m_coraldb->setAliasTag(m_connTag);
  m_optCompoundTag = new CoralDB::CompoundTag(m_dictTag,m_connTag,m_connTag,m_connTag);
  
  if (m_coraldb == NULL){
    msg(MSG::FATAL) << "Cannot create CoralDB object" << endreq;
    return NULL;
  }

  m_cabling = new PixelCablingData;

  //  CoralDB::ObjectDictionaryMap odmap;  // map containing the ids of all CoralDB objects
  //  m_coraldb->getObjectDictionary(odmap,*m_optCompoundTag);
  vector<string> modules; // used to store module ids
  //  vector<string> rods; // used to store ROD ids

  uint32_t Layer_Disk,PhiModule,RODID;
  int Barrel_EC,EtaModule;
  string offlineid;

  // read the linkmaps from the connectivity
  CoralDB::ObjectDictionaryMap obMap;
  m_coraldb->getIdClobbedForType("OBLINKMAP",obMap);
  for(CoralDB::ObjectDictionaryMap::const_iterator i = obMap.begin(); i != obMap.end(); i++) {
    //    if (i->second == "OBLINKMAP"){
      int number;
      string tmp;
      map<int,int> dto2map;
      map<int,int> dtomap;

      CoralDB::ClobDataContainer OBLinkMapContainer;
      m_coraldb->findCLOBs(i->first,OBLinkMapContainer);
      int OBLinkMapContainer_size = OBLinkMapContainer.size();
      for (int j=0; j<OBLinkMapContainer_size; j++){
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_0"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(0,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_1"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(1,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_2"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(2,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_3"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(3,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_4"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(4,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_5"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(5,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dto2Fiber_6"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dto2map.insert(make_pair(6,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_0"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(0,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_1"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(1,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_2"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(2,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_3"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(3,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_4"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(4,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_5"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(5,number));
	}
	if (OBLinkMapContainer[j].first == "Maps_dtoFiber_6"){
	  tmp = OBLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  dtomap.insert(make_pair(6,number));
	}
      }
      m_oblinkmaps_dto2.insert(make_pair(i->first,dto2map));
      m_oblinkmaps_dto.insert(make_pair(i->first,dtomap));
  }

  CoralDB::ObjectDictionaryMap rodMap;
  m_coraldb->getIdClobbedForType("RODBPCLINKMAP",rodMap);
  for(CoralDB::ObjectDictionaryMap::const_iterator i = rodMap.begin(); i != rodMap.end(); i++) {
    //    if(i->second == "RODBOCLINKMAP"){
      int number;
      string tmp;
      map<int,int> rod_40;
      map<int,int> rod_80a;
      map<int,int> rod_80b;

      CoralDB::ClobDataContainer RODBOCLinkMapContainer;
      m_coraldb->findCLOBs(i->first,RODBOCLinkMapContainer);
      int RODBOCLinkMapContainer_size = RODBOCLinkMapContainer.size();
      for (int j=0; j<RODBOCLinkMapContainer_size; j++){
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(0,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(1,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(2,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(3,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(4,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(5,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(6,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_0_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(7,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(10,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(11,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(12,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(13,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(14,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(15,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(16,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_1_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(17,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(20,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(21,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(22,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(23,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(24,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(25,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(26,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_2_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(27,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(30,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(31,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(32,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(33,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(34,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(35,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(36,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx40_3_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_40.insert(make_pair(37,number));
	}
	
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(0,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(1,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(2,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(3,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(4,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(5,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(6,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_0_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(7,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(10,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(11,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(12,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(13,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(14,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(15,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(16,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_1_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(17,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(20,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(21,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(22,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(23,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(24,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(25,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(26,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_2_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(27,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(30,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(31,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(32,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(33,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(34,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(35,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(36,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80a_3_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80a.insert(make_pair(37,number));
	}
	
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(0,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(1,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(2,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(3,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(4,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(5,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(6,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_0_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(7,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(10,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(11,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(12,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(13,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(14,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(15,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(16,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_1_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(17,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(20,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(21,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(22,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(23,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(24,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(25,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(26,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_2_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(27,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_0"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(30,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_1"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(31,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_2"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(32,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_3"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(33,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_4"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(34,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_5"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(35,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_6"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(36,number));
	}
	if (RODBOCLinkMapContainer[j].first == "Maps_RodRx80b_3_7"){
	  tmp = RODBOCLinkMapContainer[j].second;
	  tmp= tmp.substr(tmp.find(" ")+1);
	  tmp.erase(tmp.size()-1, tmp.size());
	  number = atoi(tmp.c_str());
	  rod_80b.insert(make_pair(37,number));
	}
      }
      m_rodbocmaps_40.insert(make_pair(i->first,rod_40));
      m_rodbocmaps_80a.insert(make_pair(i->first,rod_80a));
      m_rodbocmaps_80b.insert(make_pair(i->first,rod_80b));	
  }

  // Retrieve the Connection data as a bulk to decrease access time, filter out all unneeded connections
  CoralDB::ConnectionTableMap connTable1, connTable2;
  map<string,string> connTableMod;
  map<string,pair<string,string> > connTableROD;
  
  m_coraldb->getConnectionTableForType("MODULE",true,connTable1);
  m_coraldb->getConnectionTableForType("OPTOBOARD",true,connTable2);
  int connTableSize1 = connTable1.size();
  int connTableSize2 = connTable2.size();
  for (int i=0; i<connTableSize1; i++){
    if (connTable1[i].toSlot().find("UP") != string::npos) // pp0->module connection 
      connTableMod.insert(make_pair(connTable1[i].toId(),connTable1[i].fromSlot()));
  }
  for (int i=0; i<connTableSize2; i++){
    if (connTable2[i].toSlot().find("UP") != string::npos)  // ROD->Opto connection
      connTableROD.insert(make_pair(connTable2[i].toId(),make_pair(connTable2[i].fromId(),connTable2[i].fromSlot())));
  }

  // Retrieve the Alias data as a bulk to decrease access time, filter out all unneeded connections
  CoralDB::AliasesTable aliasTable;
  map<string,string> aliasTableMod;
  map<string,string> aliasTableROD;
  m_coraldb->getAliasesTable(aliasTable);
  int aliasTableSize = aliasTable.size();
  for (int i=0; i<aliasTableSize; i++){
    if ((aliasTable[i].id().find("_M") != string::npos) && (aliasTable[i].convention() == "OFFLINEID")) // module offline ID
      aliasTableMod.insert(make_pair(aliasTable[i].id(),aliasTable[i].alias()));
    if ((aliasTable[i].id().find("ROD") != string::npos) && (aliasTable[i].convention() == "OFFLINEID"))  // ROD offline ID
      aliasTableROD.insert(make_pair(aliasTable[i].id(),aliasTable[i].alias()));
  }
  // Retrieve the CLOB data as a bulk to decrease access time
  CoralDB::ClobDataContainer readoutSpeedCLOBs;
  CoralDB::ClobDataContainer OBLinkMapCLOBs;
  CoralDB::ClobDataContainer RODLinkMapCLOBs;
  m_coraldb->getNamedClobsForType("MODULE","Config_readoutSpeed",readoutSpeedCLOBs);
  m_coraldb->getNamedClobsForType("OPTOBOARD","Link_mapSName",OBLinkMapCLOBs);
  m_coraldb->getNamedClobsForType("RODBOC","Link_MapSName",RODLinkMapCLOBs);
  int readoutSpeedCLOBsSize = readoutSpeedCLOBs.size();
  int OBLinkMapCLOBsSize = OBLinkMapCLOBs.size();
  int RODLinkMapCLOBsSize =  RODLinkMapCLOBs.size();

  // retrieve module CLOBs and loop
  CoralDB::ObjectDictionaryMap moduleMap;
  m_coraldb->getIdClobbedForType("MODULE",moduleMap);
  for(CoralDB::ObjectDictionaryMap::const_iterator i = moduleMap.begin(); i != moduleMap.end(); i++) {
    //    if (i->second == "MODULE"){
      modules.push_back(i->first);

      map<string,string>::iterator aliasTableIterator = aliasTableMod.find(i->first);
      if (aliasTableIterator != aliasTableMod.end())
	offlineid = aliasTableIterator->second;
      else{
	msg(MSG::WARNING) << "OfflineID alias for module " << i->first << " not found" << endreq;
	continue;
      }
      // Offline ID format: [2.1.EC.Disk.phi.eta.0]  
      int points[6];
      points[0] = offlineid.find(".");
      for (int p = 1; p<6; p++)
	points[p] = offlineid.find(".",points[p-1]+1);

      Barrel_EC = atoi(offlineid.substr(points[1]+1,points[2]-points[1]-1).c_str());
      Layer_Disk = atoi(offlineid.substr(points[2]+1,points[3]-points[2]-1).c_str());
      PhiModule = atoi(offlineid.substr(points[3]+1,points[4]-points[3]-1).c_str());
      EtaModule = atoi(offlineid.substr(points[4]+1,points[5]-points[4]-1).c_str());
      //find the optofiber
      string pp0fiber;
      map<string,string>::iterator moduleConnectionTableIterator = connTableMod.find(i->first);
      if (moduleConnectionTableIterator != connTableMod.end())
	pp0fiber = moduleConnectionTableIterator->second;
      else{
	msg(MSG::WARNING) << "Pp0 fiber for module " << i->first << " not found" << endreq;
	continue;
      }
      //vector<CoralDB::Connection> moduleconn = m_coraldb->findConnectionsByType(i->first,"PP0",false);
      
      string readout_speed;
      bool readout_speed_40 = false;

      for (int j=0; j<readoutSpeedCLOBsSize; j++){
	if (readoutSpeedCLOBs[j].first == i->first){
	  readout_speed = readoutSpeedCLOBs[j].second;
	  break;
	}
      }

      if (readout_speed.find("40") != string::npos)
	readout_speed_40 = true;
      
      int pp0fibernumber=-1;

      //to create the pp0name (and find the corresponding ROD) alter modulestring, e.g.:  D3A_B02_S1_M2 -> D3A_B02_S1_OB
      string optoname = i->first;
      optoname.erase(optoname.find_last_of("_")+1);
      optoname.append("OB");

      // find the linkmap for this optoboard
      string optotype;
      // optotype = m_coraldb->findCLOB(optoname,"Link_mapSName");
      for (int j=0; j<OBLinkMapCLOBsSize; j++){
	  if (OBLinkMapCLOBs[j].first == optoname){
	    optotype = OBLinkMapCLOBs[j].second;
	    break;
	  }
	}

      // those strings have the form "string 6 D-TYPE", cut of the unnecessary stuff
      optotype= optotype.substr(optotype.find(" ")+1);
      optotype= optotype.substr(optotype.find(" ")+1);
      optotype.erase(optotype.size()-1, optotype.size());
      if (pp0fiber == "M1")
	pp0fibernumber = 0;
      else if (pp0fiber == "M2")
	pp0fibernumber = 1;
      else if (pp0fiber == "M3")
	pp0fibernumber = 2;
      else if (pp0fiber == "M4")
	pp0fibernumber = 3;
      else if (pp0fiber == "M5")
	pp0fibernumber = 4;
      else if (pp0fiber == "M6")
	pp0fibernumber = 5;
      else if (pp0fiber == "M7")
	pp0fibernumber = 6;
      int dto2number = m_oblinkmaps_dto2.find(optotype)->second.find(pp0fibernumber)->second;
      int dtonumber = m_oblinkmaps_dto.find(optotype)->second.find(pp0fibernumber)->second;

      //find the corresponding ROD for the module (pp0)
      string rodname, rodslot;
      map<string,pair<string,string> >::iterator RODConnectionTableIterator = connTableROD.find(optoname);
      if (RODConnectionTableIterator != connTableROD.end()){
	rodname = RODConnectionTableIterator->second.first;
	rodslot = RODConnectionTableIterator->second.second;
      }
      else{
	msg(MSG::WARNING) << "ROD information for optoboard " << optoname << " not found" << endreq;
	continue;
      }
      //vector<CoralDB::Connection> obconn = m_coraldb->findConnectionsByType(optoname,"RODBOC",false);

      int rodslotnumber;
      string rodtype;
      // rodtype = m_coraldb->findCLOB(rodname,"Link_MapSName");
      for (int j=0; j<RODLinkMapCLOBsSize; j++){
	if (RODLinkMapCLOBs[j].first == rodname){
	    rodtype = RODLinkMapCLOBs[j].second;
	    break;
	  }
	}

      // those strings have the form "string 5 BOC_1 ", cut of the unnecessary stuff
      rodtype = rodtype.substr(rodtype.find(" ")+1);
      rodtype = rodtype.substr(rodtype.find(" ")+1);
      rodtype.erase(rodtype.size()-1,rodtype.size());
      if (rodslot == "AA"){
	rodslotnumber = 0;
      }
      else if (rodslot == "BB"){
	rodslotnumber = 10;
      }
      else if (rodslot == "CC"){
	rodslotnumber = 20;
      }
      else if (rodslot == "DD"){
	rodslotnumber = 30;
      }
      else if (rodslot == "ADA"){
	if (dto2number < 10)
	  rodslotnumber = 30;
	else
	  rodslotnumber = 0;
      }
      else if ( rodslot == "AAD"){ // AAD for compatibility with M5
	if (dto2number < 10)
	  rodslotnumber = 0;
	else
	  rodslotnumber = 30;
      }
      else{
	msg(MSG::FATAL) << "invalid Rodslot found in cabling, aborting" << endreq;
	return NULL;
      }

      aliasTableIterator = aliasTableROD.find(rodname);
      if (aliasTableIterator != aliasTableROD.end())
	sscanf(aliasTableIterator->second.c_str(),"%x",&RODID);
      else{
	msg(MSG::WARNING) << "OfflineID alias for ROD " << rodname << " not found" << endreq;
	continue;
      }

      //get the linknumbers from the readout speed and the connectivity information        
      Identifier offlineId = m_idHelper->wafer_id(Barrel_EC,Layer_Disk,PhiModule,EtaModule);
      int linknumber[4];
      if (readout_speed_40){
	linknumber[0] = m_rodbocmaps_40.find(rodtype)->second.find(rodslotnumber+(dto2number%10))->second;
	if (dtonumber != -1)
	  linknumber[1] = m_rodbocmaps_40.find(rodtype)->second.find(rodslotnumber+(dtonumber%10))->second;
	else
	  linknumber[1] = -1;
	linknumber[2]=-1;
	linknumber[3]=-1;
      }
      else{
	linknumber[0] = m_rodbocmaps_80a.find(rodtype)->second.find(rodslotnumber+(dto2number%10))->second;
	if (dtonumber != -1)
	  linknumber[1] = m_rodbocmaps_40.find(rodtype)->second.find(rodslotnumber+(dtonumber%10))->second;
	else
	  linknumber[1] = -1;
	linknumber[2] = m_rodbocmaps_80b.find(rodtype)->second.find(rodslotnumber+(dto2number%10))->second;
	if (dtonumber != -1)
	  linknumber[3] = m_rodbocmaps_40.find(rodtype)->second.find(rodslotnumber+(dtonumber%10))->second;
	else
	  linknumber[3] = -1;
      }
      // some stuff from PixLib
      for (int k=0; k<=3; k++){
	int fmt;
	if ((linknumber[k] / 16 == 2) || (linknumber[k] / 16 == 6))
	  fmt = 0xba980000;
	else
	  fmt = 0x32100000;
	int pos =-1;
	for (int ip=0; ip<4; ip++) {
	  int ch = (fmt>>(16+4*ip))&0xf;
	  if (linknumber[k]%16 == ch) pos = ip;
	}
	if (pos >= 0) {
	  linknumber[k] = (linknumber[k]/16)*16 + pos; 
	} else {
	  linknumber[k] = -1;
	}
      }

      // take the smallest link as outlink
      int link = linknumber[0];
      if (link == -1)
	link = linknumber[1];
      if (link == -1)
	link = linknumber[2];
      if (link == -1)
	link = linknumber[3];
      if ((link > linknumber[1]) && linknumber[1] != -1)
	link = linknumber[1];
      if ((link > linknumber[2]) && linknumber[2] != -1)
	link = linknumber[2];
      if ((link > linknumber[3]) && linknumber[3] != -1)
	link = linknumber[3];

      uint32_t onlineId = (RODID & 0xFFFFFF) | ((link & 0xFF) << 24);

      IdentifierHash hashId;
      m_idHelper->get_hash(offlineId, hashId, &m_cntxpixel);
      if (hashId > m_idHelper->wafer_hash_max()) {
	msg(MSG::ERROR) << "IdHash overflow! HashId is 0x" << std::hex << hashId << std::dec << endreq;
	msg(MSG::ERROR) << "not mapped OfflineID: 0x" << std::hex << offlineId << std::dec << " Barrel_EC: " << Barrel_EC << " Layer_Disk: " << Layer_Disk << " PhiModule: " << PhiModule << " EtaModule: " << EtaModule << endreq;
	msg(MSG::ERROR) << "to OnlineID: 0x" << std::hex << onlineId << " RODID: 0x" << RODID << std::dec << " Link: " << link << " HashId: 0x" << std::hex << hashId << std::dec << endreq;
    return NULL;
      }
      //Fill the maps 
      m_cabling->add_entry_onoff(onlineId, offlineId);
      m_cabling->add_entry_offon(offlineId, onlineId);
      // hack for some wrong connectivity tags
      if ( RODID<= 0x111510 && RODID >= 0x111505 ) RODID += 0x001000; 
      m_cabling->add_entry_offlineList(RODID,offlineId);
      m_cabling->add_entry_offrob(offlineId, RODID);
      m_cabling->add_entry_rodrob(RODID, RODID); // obsolete, take out soon FIXME
      m_cabling->add_entry_DCSoffline(i->first,offlineId); 
      //Debug messages
      msg(MSG::DEBUG) << "mapped OfflineID: 0x" << std::hex << offlineId << std::dec << " Barrel_EC: " << Barrel_EC << " Layer_Disk: " << Layer_Disk << " PhiModule: " << PhiModule << " EtaModule: " << EtaModule << endreq;
      msg(MSG::DEBUG) << "to OnlineID: 0x" << std::hex << onlineId  << " RODID: 0x" << RODID << std::dec << " Link: " << link << " HashId: 0x" << std::hex << hashId << std::dec << endreq;
  }

  if (m_coraldb != NULL){
    delete m_coraldb;
    m_coraldb = NULL;
  }
  return m_cabling;
}
