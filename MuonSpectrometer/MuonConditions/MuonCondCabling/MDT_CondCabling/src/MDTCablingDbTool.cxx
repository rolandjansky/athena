/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  : AthAlgTool(type, name, parent), 
    m_log( msgSvc(), name ),
    m_debug(false),
    m_verbose(false)
{

  declareInterface<IMDTCablingDbTool>(this);


  m_DataLocation="keyMDT";
  


  declareProperty("MezzanineFolders",  m_mezzanineFolder="/MDT/CABLING/MEZZANINE_SCHEMA");
  declareProperty("MapFolders",  m_mapFolder="/MDT/CABLING/MAP_SCHEMA");
}

//StatusCode MDTCablingDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode MDTCablingDbTool::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad)
{
  m_log.setLevel(outputLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
 
  CLID clid        = tad->clID();
  std::string key  = tad->name();
  if ( 51038731== clid && m_DataLocation == key)
    {
      if( m_verbose ) m_log << MSG::VERBOSE << "OK " << endmsg;
      return StatusCode::SUCCESS;
    }
  return StatusCode::FAILURE;
}




StatusCode MDTCablingDbTool::initialize()
{ 
  m_log.setLevel(outputLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  if( m_verbose ) m_log << MSG::VERBOSE << "Initializing " << endmsg;
 
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    if( m_verbose ) m_log << MSG::VERBOSE << "Retrieved DetectorStore" << endmsg;
  }else{
    m_log << MSG::ERROR << "Failed to retrieve DetectorStore" << endmsg;
    return sc;
  }

  // retrieve the mdt id helper
  sc = m_detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (!sc.isSuccess()) {
    m_log << MSG::ERROR << "Can't retrieve MdtIdHelper" << endmsg;
    return sc;
  }

  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       m_log << MSG::ERROR << "Unable to get the IOVSvc" << endmsg;
       return StatusCode::FAILURE;
  }

  // Get interface to IOVDbSvc
  m_IOVDbSvc = 0;
  //  bool CREATEIF(true);
  sc = service( "IOVDbSvc", m_IOVDbSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       m_log << MSG::ERROR << "Unable to get the IOVDbSvc" << endmsg;
       return StatusCode::FAILURE;
  }


  if(sc.isFailure()) return StatusCode::FAILURE;



  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    m_log << MSG::ERROR << "Could not find the ChronoSvc" << endmsg;
    return sc;
  }
      
  // commented for now
    m_cablingData = new MuonMDT_CablingMap();
 
    if( m_verbose ) m_log << MSG::VERBOSE<<" pointer to Map container =<"<<m_cablingData<<endmsg;
   
    sc = m_detStore->record(m_cablingData,m_DataLocation);
    if (sc == StatusCode::FAILURE) {
      m_log << MSG::ERROR << "Cannot record cabling container in the detector store"
	  << endmsg;
      return sc;
    }
    else m_log << MSG::INFO << "Map container recorded in the detector store"<<endmsg;

 

   // Get the TransientAddress from DetectorStore and set "this" as the
   // AddressProvider

 
   SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MuonMDT_CablingMap>::ID(), m_DataLocation);
   if (!proxy) {
     m_log << MSG::ERROR << "Unable to get the proxy for class Cabling Container" << endmsg;
     return StatusCode::FAILURE;
   }else m_log << MSG::INFO << "proxy for class Cabling Container found" << endmsg;



   SG::TransientAddress* tad =  proxy->transientAddress();
   if (!tad) {
     m_log << MSG::ERROR << "Unable to get the tad" << endmsg;
      return StatusCode::FAILURE;
   }else m_log << MSG::INFO << "proxy transient Address found" << endmsg;

   IAddressProvider* addp = this;
   //   tad->setProvider(addp);
   tad->setProvider(addp, StoreID::DETECTOR_STORE);
   if( m_verbose ) m_log << MSG::VERBOSE << "set address provider for CABLING Container" << endmsg;
    
   return StatusCode::SUCCESS;

}


StatusCode MDTCablingDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  StatusCode sc = StatusCode::SUCCESS;
  m_log.setLevel(outputLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    m_log << MSG::INFO << *itr << " I="<<I<<" ";
    if(*itr==m_mapFolder) {
      sc=loadMDTMap(I,keys);
    }
  }
  return sc;
}




StatusCode MDTCablingDbTool::loadMezzanine(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{
  m_log.setLevel(outputLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;

  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load Mezzanine Type parameters  from DB" << endmsg;
  return  sc;
 
  // maybe not used....
}



StatusCode MDTCablingDbTool::loadMDTMap(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/)) 
{

  m_log.setLevel(outputLevel());
  m_debug = m_log.level() <= MSG::DEBUG;
  m_verbose = m_log.level() <= MSG::VERBOSE;
  StatusCode sc=StatusCode::SUCCESS;
  m_log << MSG::INFO << "Load Mezzanine Type parameters  from DB" << endmsg;

  m_chrono1 = "loadMDTMap method";
  m_chrono2 = "access only DB and parsing";
  m_chrono3 = "access only DB map table";
  m_chronoSvc->chronoStart(m_chrono1);



  // retreive the and remove the old collection 
  
  sc = m_detStore->retrieve( m_cablingData, m_DataLocation );
  if(sc.isSuccess())  {
    if( m_verbose ) m_log << MSG::VERBOSE << "Cabling Container found " << m_cablingData << endmsg;
    sc = m_detStore->remove( m_cablingData );
    if (sc.isSuccess()) {
     if( m_verbose ) m_log << MSG::VERBOSE << "Cabling Container at " << m_cablingData << " removed "<<endmsg;
    } else{
      m_log << MSG::WARNING <<"Remove failed for: "<<m_cablingData<<endmsg;
    }
  }

  else {
    if (m_cablingData) {
        if( m_verbose ) m_log << MSG::VERBOSE << "Previous Map Container not in the DetStore but pointer not NULL <" << m_cablingData <<">"<< endmsg;
        delete m_cablingData;
    }
  }
  
  // reinitialize 
 

  m_cablingData=0;
 
  
  m_cablingData = new  MuonMDT_CablingMap() ;
  
  if( m_debug ) m_log << MSG::DEBUG<<"New Map container pointer "<<m_cablingData<<endmsg;

  
  //******************************************************************
  // access to Mezzanine Schema Table to obtained the mezzanine type 
  //*****************************************************************
  m_chronoSvc->chronoStart(m_chrono2);
  m_chronoSvc->chronoStart(m_chrono3); 
  const CondAttrListCollection * atrc;
  
  sc=m_detStore->retrieve(atrc,m_mezzanineFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR 
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_mezzanineFolder << endmsg;
    return sc;
  }
  
  else
       m_log<<MSG::INFO<<" CondAttrListCollection from DB folder have been obtained with size "<< atrc->size() <<endmsg;
  
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
    
    if( m_verbose ) m_log << MSG::VERBOSE << "Sequence load is " << sequence << " for the mezzanine type =  "<< mezzanine_type<< " for the layer  number  = " <<layer <<endmsg;
    
    
    // here add the mezzanine type to the cabling class
    bool addLine = m_cablingData->addMezzanineLine(mezzanine_type, layer, sequence);
    if (!addLine) {
      m_log << MSG::ERROR << "Could not add the mezzanine sequence to the map "
	  << endmsg;
    }
    else {
      if( m_verbose ) m_log << MSG::VERBOSE << "Sequence added successfully to the map" << endmsg;
    }
    
  }
  
  //******************************************************************
  // access to Map Schema Table to obtained the Map  
  //*****************************************************************
  
  const CondAttrListCollection * atrc_map;
  sc=m_detStore->retrieve(atrc_map,m_mapFolder);
  if(sc.isFailure())  {
    m_log << MSG::ERROR 
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_mapFolder << endmsg;
    return sc;
  }
  else
    m_log<<MSG::INFO<<" CondAttrListCollection from DB Map folder have been obtained with size "<< atrc->size() <<endmsg;
  
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
    
    if( m_verbose ) m_log << MSG::VERBOSE << "Data load is: /n" <<
      "Chamber_Name = " << chamber_name << " eta= " << eta << "   Phi= " << phi << " sub_id = " <<subdetector_id << "  mrod = " << mrod << " csm = " << csm << "  chan= " << chan << " mezzanine_type= " << mezzanine_type << "  map = " <<map << " FINISHED HERE "<<endmsg;

    // convert the string name to index
    std::string stationNameString = chamber_name.substr(0,3);
    // fix for the BOE chambers, which in the offline are treated as BOL                                                                            
    if (stationNameString == "BOE") {
      stationNameString = "BOL";
    }
    if (stationNameString == "BMG") BMGchamberadded = true;    
    int stationIndex = m_mdtIdHelper->stationNameIndex(stationNameString);
    if( m_verbose ) m_log << MSG::VERBOSE << "station name: " << stationNameString << " index: " << stationIndex << endmsg;
    
    // convert the subdetector id to integer
    int subdetectorId = atoi(subdetector_id.c_str());

    std::string delimiter = ",";
    std::vector<std::string> info_map;

    MuonCalib::MdtStringUtils::tokenize(map,info_map,delimiter);
    
    if( m_verbose ) m_log << MSG::VERBOSE << " parsing of the map" << endmsg;

    // this is a loop on the mezzanines, add each mezzanine to the map
    int index=0;
    int tdcId = -99;
    int channelId = -99;
    int multilayer = -99;
    int layer = -99;
    int tube = -99;

    for(unsigned int i=0; i<info_map.size();i++){
      if( m_verbose ) m_log<< MSG::VERBOSE << i << "..."<< info_map[i]<< endmsg;
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
  // this will for sure not work for BIS78, multilayer2, since there, we have 108 tubes per tubeLayer (cf. ATLASRECTS-5961)
	tube = info%100;
	layer = ((info-tube)/100)%10;
	multilayer = (((info-tube)/100)-layer)/10 ;
	index = 0;
  // the stationIndex is later on passed to the MdtIdHelper, thus, it must be a reasonable station name, i.e. not < 0
  if (stationIndex<0) {
    static bool stWarningPrinted = false;
    if (!stWarningPrinted) {
      ATH_MSG_WARNING("Found stationIndex="<<stationIndex<<" which is not reasonable, maybe related to ATLASRECTS-5961, continuing...");
      stWarningPrinted=true;
    }
    continue;
  }
	ATH_MSG_VERBOSE( "Adding new mezzanine: tdcId " << tdcId << " channel " << channelId
	    << " station " << stationIndex << " multilayer " << multilayer << " layer " << layer << " tube " << tube);

	// now this mezzanine can be added to the map:
	/*bool addMezzanine = */m_cablingData->addMezzanine(mezzanine_type, stationIndex, eta, phi, multilayer,
							layer, tube, subdetectorId, mrod, csm, tdcId,
							channelId);
      }
	
    }

    // now add this mezzanine to the map

    
  }

  if(m_mdtIdHelper->stationNameIndex("BMG") != -1 && !BMGchamberadded) {
    m_log << MSG::WARNING << "Running a layout including BMG chambers, but missing them in cabling from conditions --> hard-coding BMG cabling." << endmsg;
    int stationIndex = m_mdtIdHelper->stationNameIndex("BMG");

    // BMG1A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,   1,   6,          1,     4,(i*6)+1,            97,   50,   0, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,   1,   6,          2,     4,(i*6)+1,            97,   50,   0,   16-2*i,         0);
    // BMG2A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,   2,   6,          1,     4,(i*6)+1,            97,   50,   1, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,   2,   6,          2,     4,(i*6)+1,            97,   50,   1,   16-2*i,         0);
    // BMG3A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,   3,   6,          1,     4,(i*6)+1,            97,   50,   2, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,   3,   6,          2,     4,(i*6)+1,            97,   50,   2,   16-2*i,         0);

    // BMG1C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,  -1,   6,          1,     4,(i*6)+6,            98,   50,   0,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,  -1,   6,          2,     4,(i*6)+6,            98,   50,   0,      2*i,         0);
    // BMG2C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,  -2,   6,          1,     4,(i*6)+6,            98,   50,   1,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,  -2,   6,          2,     4,(i*6)+6,            98,   50,   1,      2*i,         0);
    // BMG3C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,  -3,   6,          1,     4,(i*6)+6,            98,   50,   2,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,  -3,   6,          2,     4,(i*6)+6,            98,   50,   2,      2*i,         0);

    // BMG1A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,   1,   7,          1,     4,(i*6)+6,            97,   51,   0,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,   1,   7,          2,     4,(i*6)+6,            97,   51,   0,      2*i,         0);
    // BMG2A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,   2,   7,          1,     4,(i*6)+6,            97,   51,   1,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,   2,   7,          2,     4,(i*6)+6,            97,   51,   1,      2*i,         0);
    // BMG3A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,   3,   7,          1,     4,(i*6)+6,            97,   51,   2,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,   3,   7,          2,     4,(i*6)+6,            97,   51,   2,      2*i,         0);

    // BMG1C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,  -1,   7,          1,     4,(i*6)+1,            98,   51,   0, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,  -1,   7,          2,     4,(i*6)+1,            98,   51,   0,   16-2*i,         0);
    // BMG2C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,  -2,   7,          1,     4,(i*6)+1,            98,   51,   1, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,  -2,   7,          2,     4,(i*6)+1,            98,   51,   1,   16-2*i,         0);
    // BMG3C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       m_cablingData->addMezzanine(         60, stationIndex,  -3,   7,          1,     4,(i*6)+1,            98,   51,   2, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       m_cablingData->addMezzanine(         60, stationIndex,  -3,   7,          2,     4,(i*6)+1,            98,   51,   2,   16-2*i,         0);
  }
  
  // return  sc; 
   m_chronoSvc->chronoStop(m_chrono2);
  
  
  if( m_verbose ) m_log << MSG::VERBOSE << "Collection CondAttrListCollection CLID "
      << atrc_map->clID() << endmsg;

 

    
  sc=m_detStore->record( m_cablingData, m_DataLocation );
  if (sc==StatusCode::SUCCESS) {
    m_log << MSG::INFO<<"New Mapcontainer recoded in the DetStore with key "<<m_DataLocation<<endmsg;
  }

  SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MuonMDT_CablingMap>::ID(), m_DataLocation);
  if (!proxy) {
    m_log << MSG::ERROR << "Unable to get the proxy for class Cabling Container" << endmsg;
    return StatusCode::FAILURE;
  }

  SG::TransientAddress* tad =  proxy->transientAddress();
  if (!tad) {
    m_log << MSG::ERROR << "Unable to get the tad" << endmsg;
    return StatusCode::FAILURE;
  }
  

  IAddressProvider* addp = this;
  //  tad->setProvider(addp);
  tad->setProvider(addp, StoreID::DETECTOR_STORE);
  if( m_verbose ) m_log << MSG::VERBOSE<< "set address provider for Cabling Container" << endmsg;
 
  //IOVRange range;
  //sc=m_IOVSvc->getRange(1238547719, m_mapFolder, range);
  
  //m_log << MSG::VERBOSE <<"CondAttrListCollection IOVRange "<<range<<endmsg;
  
  // IOVRange range2;
  //sc=m_IOVSvc->setRange(51038731, m_DataLocation, range, "StoreGateSvc");

  
  //sc=m_IOVSvc->setRange(51038731, m_DataLocation, range);
  //sc=m_IOVSvc->getRange(51038731, m_DataLocation, range2);
  //m_log << MSG::VERBOSE <<"Container new IOVRange "<<range2<<endmsg;
  
  m_IOVDbSvc->dropObject(m_mapFolder, true);
  //m_detStore->releaseObject(51038731, "keyMDT"); 
  m_chronoSvc->chronoStop(m_chrono1);
  

  return StatusCode::SUCCESS;

}

