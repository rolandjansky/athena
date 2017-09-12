/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <string>
#include <stdlib.h>

#include "MuonCablingData/MuonMDT_CablingMap.h"

#include "MDT_CondCabling/MDTCablingDbTool.h" 
#include "MuonCondInterface/IMDTCablingDbTool.h"
#include "MuonCondSvc/MdtStringUtils.h"
//#include "AthenaKernel/IIOVDbSvc.h"


#include <map>
#include "Identifier/Identifier.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT Cabling Map from COOL DB
//* one callback aganist the MuonMDT_CablingMAP class
//* retrieving of two tables from DB
//*********************************************************


MDTCablingDbTool::MDTCablingDbTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  declareInterface<IMDTCablingDbTool>(this);


  m_DataLocation="keyMDT";
  


  declareProperty("MezzanineFolders",  m_mezzanineFolder="/MDT/CABLING/MEZZANINE_SCHEMA");
  declareProperty("MapFolders",  m_mapFolder="/MDT/CABLING/MAP_SCHEMA");
}

//StatusCode MDTCablingDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode MDTCablingDbTool::updateAddress(StoreID::type /*storeID*/,
                                           SG::TransientAddress* tad,
                                           const EventContext& /*ctx*/)
{
  CLID clid        = tad->clID();
  std::string key  = tad->name();
  if ( 51038731== clid && m_DataLocation == key)
    {
      ATH_MSG_VERBOSE( "OK "  );
      return StatusCode::SUCCESS;
    }
  return StatusCode::FAILURE;
}




StatusCode MDTCablingDbTool::initialize()
{ 
  ATH_MSG_VERBOSE( "Initializing "  );
 
  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );

  m_IOVSvc = 0;
  bool CREATEIF(true);
  ATH_CHECK( service( "IOVSvc", m_IOVSvc, CREATEIF ) );

  m_IOVDbSvc = 0;
  ATH_CHECK( service( "IOVDbSvc", m_IOVDbSvc, CREATEIF ) );

  ATH_CHECK( service("ChronoStatSvc",m_chronoSvc) );
      
  // commented for now
  m_cablingData = new MuonMDT_CablingMap();
 
  ATH_MSG_VERBOSE(" pointer to Map container =<"<<m_cablingData );
   
  ATH_CHECK( detStore()->record(m_cablingData,m_DataLocation) );
  ATH_MSG_INFO( "Map container recorded in the detector store" );

   // Get the TransientAddress from DetectorStore and set "this" as the
   // AddressProvider

 
   SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<MuonMDT_CablingMap>::ID(), m_DataLocation);
   if (!proxy) {
     ATH_MSG_ERROR( "Unable to get the proxy for class Cabling Container"  );
     return StatusCode::FAILURE;
   }
   else ATH_MSG_INFO( "proxy for class Cabling Container found"  );



   IAddressProvider* addp = this;
   //   tad->setProvider(addp);
   proxy->setProvider(addp, StoreID::DETECTOR_STORE);
   ATH_MSG_VERBOSE( "set address provider for CABLING Container"  );
    
   return StatusCode::SUCCESS;

}


StatusCode MDTCablingDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  StatusCode sc;
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_INFO( *itr << " I="<<I<<" " );
    if(*itr==m_mapFolder) {
      sc=loadMDTMap(I,keys);
    }
  }
  return sc;
}




StatusCode MDTCablingDbTool::loadMezzanine(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  ATH_MSG_INFO( "Load Mezzanine Type parameters  from DB"  );
  return  StatusCode::SUCCESS;
}



StatusCode MDTCablingDbTool::loadMDTMap(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  ATH_MSG_INFO( "Load Mezzanine Type parameters  from DB"  );

  m_chrono1 = "loadMDTMap method";
  m_chrono2 = "access only DB and parsing";
  m_chrono3 = "access only DB map table";
  m_chronoSvc->chronoStart(m_chrono1);



  // retreive the and remove the old collection 
  
  StatusCode sc = detStore()->retrieve( m_cablingData, m_DataLocation );
  if(sc.isSuccess())  {
    ATH_MSG_VERBOSE( "Cabling Container found " << m_cablingData  );
    sc = detStore()->remove( m_cablingData );
    if (sc.isSuccess()) {
      ATH_MSG_VERBOSE( "Cabling Container at " << m_cablingData << " removed " );
    } else{
      ATH_MSG_WARNING("Remove failed for: "<<m_cablingData );
    }
  }

  else {
    if (m_cablingData) {
        ATH_MSG_VERBOSE( "Previous Map Container not in the DetStore but pointer not NULL <" << m_cablingData <<">" );
        delete m_cablingData;
    }
  }
  
  // reinitialize 
 

  m_cablingData=0;
 
  
  m_cablingData = new  MuonMDT_CablingMap() ;
  
  ATH_MSG_DEBUG("New Map container pointer "<<m_cablingData );

  
  //******************************************************************
  // access to Mezzanine Schema Table to obtained the mezzanine type 
  //*****************************************************************
  m_chronoSvc->chronoStart(m_chrono2);
  m_chronoSvc->chronoStart(m_chrono3); 
  const CondAttrListCollection * atrc;
  
  ATH_CHECK( detStore()->retrieve(atrc,m_mezzanineFolder) );
  ATH_MSG_INFO(" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size()  );
  
  m_chronoSvc->chronoStop(m_chrono3);
  CondAttrListCollection::const_iterator itr;
  for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    
    int sequence; 
    int layer;
    int mezzanine_type;
    
    mezzanine_type=*(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));
    layer=*(static_cast<const int*>((atr["Layer"]).addressOfData()));
    sequence=*(static_cast<const int*>((atr["Sequence"]).addressOfData()));
    
    ATH_MSG_VERBOSE( "Sequence load is " << sequence << " for the mezzanine type =  "<< mezzanine_type<< " for the layer  number  = " <<layer  );
    
    
    // here add the mezzanine type to the cabling class
    bool addLine = m_cablingData->addMezzanineLine(mezzanine_type, layer, sequence);
    if (!addLine) {
      ATH_MSG_ERROR( "Could not add the mezzanine sequence to the map " );
    }
    else {
      ATH_MSG_VERBOSE( "Sequence added successfully to the map"  );
    }
    
  }
  
  //******************************************************************
  // access to Map Schema Table to obtained the Map  
  //*****************************************************************
  
  const CondAttrListCollection * atrc_map;
  ATH_CHECK( detStore()->retrieve(atrc_map,m_mapFolder) );
  ATH_MSG_INFO(" CondAttrListCollection from DB Map folder have been obtained with size "<< atrc->size()  );
  
  bool BMGchamberadded = false;
  
  //CondAttrListCollection::const_iterator itr;
  for (itr = atrc_map->begin(); itr != atrc_map->end(); ++itr) {
    const coral::AttributeList& atr=itr->second;
    
    std::string map;
    std::string chamber_name, subdetector_id;
    int  eta, phi, chan;
    int mrod, csm, mezzanine_type;
   
    chamber_name=*(static_cast<const std::string*>((atr["Chamber_Name"]).addressOfData()));
    eta=*(static_cast<const int*>((atr["Eta"]).addressOfData()));   
    phi=*(static_cast<const int*>((atr["Phi"]).addressOfData()));
    subdetector_id=*(static_cast<const std::string*>((atr["SubDet_Id"]).addressOfData()));
    mrod=*(static_cast<const int*>((atr["MROD"]).addressOfData()));
    csm=*(static_cast<const int*>((atr["CSM"]).addressOfData()));
    chan=*(static_cast<const int*>((atr["Chan"]).addressOfData()));
    mezzanine_type=*(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));
    map=*(static_cast<const std::string*>((atr["Map"]).addressOfData()));
    
    ATH_MSG_VERBOSE( "Data load is: /n" <<
                     "Chamber_Name = " << chamber_name << " eta= " << eta << "   Phi= " << phi << " sub_id = " <<subdetector_id << "  mrod = " << mrod << " csm = " << csm << "  chan= " << chan << " mezzanine_type= " << mezzanine_type << "  map = " <<map << " FINISHED HERE " );

    // convert the string name to index
    std::string stationNameString = chamber_name.substr(0,3);
    // fix for the BOE chambers, which in the offline are treated as BOL                                                                            
    if (stationNameString == "BOE") {
      stationNameString = "BOL";
    }
    if (stationNameString == "BMG") BMGchamberadded = true;    
    int stationIndex = m_mdtIdHelper->stationNameIndex(stationNameString);
    ATH_MSG_VERBOSE( "station name: " << stationNameString << " index: " << stationIndex  );
    
    // convert the subdetector id to integer
    int subdetectorId = atoi(subdetector_id.c_str());

    std::string delimiter = ",";
    std::vector<std::string> info_map;

    MuonCalib::MdtStringUtils::tokenize(map,info_map,delimiter);
    
    ATH_MSG_VERBOSE( " parsing of the map"  );

    // this is a loop on the mezzanines, add each mezzanine to the map
    int index=0;
    int tdcId = -99;
    int channelId = -99;
    int multilayer = -99;
    int layer = -99;
    int tube = -99;

    for(unsigned int i=0; i<info_map.size();i++){
      ATH_MSG_VERBOSE( i << "..."<< info_map[i] );
      int info = atoi(info_map[i].c_str());

      index++;
      // this is a tdcid
      if (index==1) {
	tdcId = info;	
      }
      // this is a channel Id
      else if (index==2) {
	channelId = info;
      }
      // this is a tube id that must be unpacked
      else if (index==3) {
	// unpack the tube Id
	tube = info%100;
	layer = ((info-tube)/100)%10;
	multilayer = (((info-tube)/100)-layer)/10 ;
	index = 0;

	ATH_MSG_VERBOSE( "Adding new mezzanine: tdcId " << tdcId << " channel " << channelId
                         << " station " << stationIndex << " multilayer " << multilayer << " layer " << layer << " tube " << tube  );

	// now this mezzanine can be added to the map:
	/*bool addMezzanine = */m_cablingData->addMezzanine(mezzanine_type, stationIndex, eta, phi, multilayer,
							layer, tube, subdetectorId, mrod, csm, tdcId,
							channelId);
      }
	
    }

    // now add this mezzanine to the map

    
  }

  if(m_mdtIdHelper->stationNameIndex("BMG") != -1 && !BMGchamberadded) {
    ATH_MSG_WARNING( "Running a layout including BMG chambers, but missing them in cabling from conditions --> hard-coding BMG cabling."  );
    int stationIndex = m_mdtIdHelper->stationNameIndex("BMG");

    // BMG1A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,   1,   6,          1,     1,(i*6)+4,            99,   48,   2,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,   1,   6,          2,     1,(i*6)+4,            99,   48,   2,   i+9,         0);
    // BMG2A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,   2,   6,          1,     1,(i*6)+4,            99,   48,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,   2,   6,          2,     1,(i*6)+4,            99,   48,   5,   i+9,         0);
    // BMG3A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,   3,   6,          1,     1,(i*6)+4,            97,   48,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,   3,   6,          2,     1,(i*6)+4,            97,   48,   5,   i+9,         0);

    // BMG1C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,  -1,   6,          1,     1,(i*6)+4,           100,   48,   2,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,  -1,   6,          2,     1,(i*6)+4,           100,   48,   2,   i+9,         0);
    // BMG2C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,  -2,   6,          1,     1,(i*6)+4,           100,   48,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,  -2,   6,          2,     1,(i*6)+4,           100,   48,   5,   i+9,         0);
    // BMG3C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,  -3,   6,          1,     1,(i*6)+4,            98,   48,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,  -3,   6,          2,     1,(i*6)+4,            98,   48,   5,   i+9,         0);

    // BMG1A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,   1,   7,          1,     1,(i*6)+4,           100,   51,   2,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,   1,   7,          2,     1,(i*6)+4,           100,   51,   2,   i+9,         0);
    // BMG2A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,   2,   7,          1,     1,(i*6)+4,           100,   50,   2,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,   2,   7,          2,     1,(i*6)+4,           100,   50,   2,   i+9,         0);
    // BMG3A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,   3,   7,          1,     1,(i*6)+4,           100,   49,   2,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,   3,   7,          2,     1,(i*6)+4,           100,   49,   2,   i+9,         0);

    // BMG1C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,  -1,   7,          1,     1,(i*6)+4,           100,   51,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,  -1,   7,          2,     1,(i*6)+4,           100,   51,   5,   i+9,         0);
    // BMG2C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,  -2,   7,          1,     1,(i*6)+4,           100,   50,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,  -2,   7,          2,     1,(i*6)+4,           100,   50,   5,   i+9,         0);
    // BMG3C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         43, stationIndex,  -3,   7,          1,     1,(i*6)+4,           100,   49,   5,     i,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         43, stationIndex,  -3,   7,          2,     1,(i*6)+4,           100,   49,   5,   i+9,         0);
  }
  
  // return  sc; 
   m_chronoSvc->chronoStop(m_chrono2);
  
  
   ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID "
                    << atrc_map->clID()  );

    
  sc=detStore()->record( m_cablingData, m_DataLocation );
  if (sc==StatusCode::SUCCESS) {
    ATH_MSG_INFO("New Mapcontainer recoded in the DetStore with key "<<m_DataLocation );
  }

  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<MuonMDT_CablingMap>::ID(), m_DataLocation);
  if (!proxy) {
    ATH_MSG_ERROR( "Unable to get the proxy for class Cabling Container"  );
    return StatusCode::FAILURE;
  }

  IAddressProvider* addp = this;
  //  tad->setProvider(addp);
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  ATH_MSG_VERBOSE( "set address provider for Cabling Container"  );
 
  //IOVRange range;
  //sc=m_IOVSvc->getRange(1238547719, m_mapFolder, range);
  
  //m_log << MSG::VERBOSE <<"CondAttrListCollection IOVRange "<<range<<endmsg;
  
  // IOVRange range2;
  //sc=m_IOVSvc->setRange(51038731, m_DataLocation, range, "StoreGateSvc");

  
  //sc=m_IOVSvc->setRange(51038731, m_DataLocation, range);
  //sc=m_IOVSvc->getRange(51038731, m_DataLocation, range2);
  //m_log << MSG::VERBOSE <<"Container new IOVRange "<<range2<<endmsg;
  
  m_IOVDbSvc->dropObject(m_mapFolder, true);
  //detStore()->releaseObject(51038731, "keyMDT"); 
  m_chronoSvc->chronoStop(m_chrono1);
  

  return StatusCode::SUCCESS;

}

