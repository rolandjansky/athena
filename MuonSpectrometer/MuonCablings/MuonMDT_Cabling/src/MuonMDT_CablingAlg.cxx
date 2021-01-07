/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMDT_Cabling/MuonMDT_CablingAlg.h"

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include "MuonCondSvc/MdtStringUtils.h"
#include <map>

MuonMDT_CablingAlg::MuonMDT_CablingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readKeyMez("/MDT/CABLING/MEZZANINE_SCHEMA"),
  m_readKeyMap("/MDT/CABLING/MAP_SCHEMA"),
  m_condSvc{"CondSvc", name}
{
  declareProperty("MezzanineFolders", m_readKeyMez);
  declareProperty("MapFolders", m_readKeyMap);
}

StatusCode MuonMDT_CablingAlg::initialize(){

  ATH_MSG_DEBUG( "initialize " << name() );  
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKeyMez.initialize());
  ATH_CHECK(m_readKeyMap.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MuonMDT_CablingAlg::execute(){
  
  ATH_MSG_DEBUG( "execute " << name() );  
  
  // Write Cond Handle

  SG::WriteCondHandle<MuonMDT_CablingMap> writeHandle{m_writeKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }
  std::unique_ptr<MuonMDT_CablingMap> writeCdo{std::make_unique<MuonMDT_CablingMap>()};

  // Read Cond Handle
  
  SG::ReadCondHandle<CondAttrListCollection> readHandleMez{ m_readKeyMez };
  const CondAttrListCollection* readCdoMez{*readHandleMez}; 
  if(readCdoMez==nullptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  }   
  EventIDRange rangeMez;
  if ( !readHandleMez.range(rangeMez) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleMez.key());
    return StatusCode::FAILURE;
  }  
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleMez.fullKey() << " readCdoMez->size()= " << readCdoMez->size());
  ATH_MSG_INFO("Range of input is " << rangeMez);

  SG::ReadCondHandle<CondAttrListCollection> readHandleMap{ m_readKeyMap };
  const CondAttrListCollection* readCdoMap{*readHandleMap}; 
  if(readCdoMap==nullptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  }   
  EventIDRange rangeMap;
  if ( !readHandleMap.range(rangeMap) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleMap.key());
    return StatusCode::FAILURE;
  }  
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleMap.fullKey() << " readCdoMap->size()= " << readCdoMap->size());
  ATH_MSG_INFO("Range of input is " << rangeMap);

  // Create an intersection of input IOVs
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeMez,rangeMap);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }
  
  // begin like MDTCablingDbTool::loadMDTMap() ----------

  // access to Mezzanine Schema Table to obtained the mezzanine type   
  
  CondAttrListCollection::const_iterator itrMez;
  for (itrMez = readCdoMez->begin(); itrMez != readCdoMez->end(); ++itrMez) {
    const coral::AttributeList& atr=itrMez->second;
    int sequence; 
    int layer;
    int mezzanine_type;    
    mezzanine_type=*(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));
    layer=*(static_cast<const int*>((atr["Layer"]).addressOfData()));
    sequence=*(static_cast<const int*>((atr["Sequence"]).addressOfData()));    
    ATH_MSG_VERBOSE( "Sequence load is " << sequence << " for the mezzanine type =  "<< mezzanine_type<< " for the layer  number  = " <<layer  );        
    // here add the mezzanine type to the cabling class
    bool addLine = writeCdo->addMezzanineLine(mezzanine_type, layer, sequence);
    if (!addLine) {
      ATH_MSG_ERROR( "Could not add the mezzanine sequence to the map " );
    }
    else {
      ATH_MSG_VERBOSE( "Sequence added successfully to the map"  );
    }    
  }
    
  // access to Map Schema Table to obtained the Map
  
  bool BMGchamberadded = false;
  
  CondAttrListCollection::const_iterator itrMap;
  for (itrMap = readCdoMap->begin(); itrMap != readCdoMap->end(); ++itrMap) {
    const coral::AttributeList& atr=itrMap->second;
    
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
                     "Chamber_Name = " << chamber_name << " eta= " << eta << "   Phi= " << phi 
		     << " sub_id = " <<subdetector_id << "  mrod = " << mrod << " csm = " << csm 
		     << "  chan= " << chan << " mezzanine_type= " << mezzanine_type << "  map = " <<map 
		     << " FINISHED HERE " );

    // convert the string name to index
    std::string stationNameString = chamber_name.substr(0,3);
    // fix for the BOE chambers, which in the offline are treated as BOL                                                                            
    if (stationNameString == "BOE") {
      stationNameString = "BOL";
    }
    if (stationNameString == "BMG") BMGchamberadded = true;    
    int stationIndex = m_idHelperSvc->mdtIdHelper().stationNameIndex(stationNameString);
    ATH_MSG_VERBOSE( "station name: " << stationNameString << " index: " << stationIndex  );
    // convert the subdetector id to integer
    int subdetectorId = atoi(subdetector_id.c_str());

    std::string delimiter = ",";
    std::vector<std::string> info_map;
    MuonCalib::MdtStringUtils::tokenize(map,info_map,delimiter);   
    ATH_MSG_VERBOSE( " parsing of the map"  );

    int index=0;
    int tdcId = -99;
    int channelId = -99;
    int multilayer = -99;
    int layer = -99;
    int tube = -99;

    // this is a loop on the mezzanines, add each mezzanine to the map

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
	writeCdo->addMezzanine(mezzanine_type, stationIndex, eta, phi, multilayer,
				    layer, tube, subdetectorId, mrod, csm, tdcId, channelId);				    
      }	
    }

  }

  if(m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1 && !BMGchamberadded) {
    ATH_MSG_WARNING( "Running a layout including BMG chambers, but missing them in cabling from conditions --> hard-coding BMG cabling."  );
    int stationIndex = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");

    // BMG1A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,   1,   6,          1,     4,(i*6)+1,            97,   50,   0, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,   1,   6,          2,     4,(i*6)+1,            97,   50,   0,   16-2*i,         0);
    // BMG2A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,   2,   6,          1,     4,(i*6)+1,            97,   50,   1, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,   2,   6,          2,     4,(i*6)+1,            97,   50,   1,   16-2*i,         0);
    // BMG3A12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,   3,   6,          1,     4,(i*6)+1,            97,   50,   2, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,   3,   6,          2,     4,(i*6)+1,            97,   50,   2,   16-2*i,         0);

    // BMG1C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,  -1,   6,          1,     4,(i*6)+6,            98,   50,   0,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,  -1,   6,          2,     4,(i*6)+6,            98,   50,   0,      2*i,         0);
    // BMG2C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,  -2,   6,          1,     4,(i*6)+6,            98,   50,   1,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,  -2,   6,          2,     4,(i*6)+6,            98,   50,   1,      2*i,         0);
    // BMG3C12 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,  -3,   6,          1,     4,(i*6)+6,            98,   50,   2,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,  -3,   6,          2,     4,(i*6)+6,            98,   50,   2,      2*i,         0);

    // BMG1A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,   1,   7,          1,     4,(i*6)+6,            97,   51,   0,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,   1,   7,          2,     4,(i*6)+6,            97,   51,   0,      2*i,         0);
    // BMG2A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,   2,   7,          1,     4,(i*6)+6,            97,   51,   1,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,   2,   7,          2,     4,(i*6)+6,            97,   51,   1,      2*i,         0);
    // BMG3A14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,   3,   7,          1,     4,(i*6)+6,            97,   51,   2,    2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,   3,   7,          2,     4,(i*6)+6,            97,   51,   2,      2*i,         0);

    // BMG1C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,  -1,   7,          1,     4,(i*6)+1,            98,   51,   0, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,  -1,   7,          2,     4,(i*6)+1,            98,   51,   0,   16-2*i,         0);
    // BMG2C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,  -2,   7,          1,     4,(i*6)+1,            98,   51,   1, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,  -2,   7,          2,     4,(i*6)+1,            98,   51,   1,   16-2*i,         0);
    // BMG3C14 ---------------- mezzanine_type, stationIndex, eta, phi, multilayer, layer,   tube, subdetectorId, mrod, csm, tdcId, channelId
    for(int i=0; i<9; i++) // ML1
       writeCdo->addMezzanine(         60, stationIndex,  -3,   7,          1,     4,(i*6)+1,            98,   51,   2, 16-2*i+1,         0);
    for(int i=0; i<9; i++) // ML2
       writeCdo->addMezzanine(         60, stationIndex,  -3,   7,          2,     4,(i*6)+1,            98,   51,   2,   16-2*i,         0);
  }
    
  ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID "
		   << readCdoMap->clID()  );

  // end like MDTCablingDbTool::loadMDTMap() ----------

  // maybe better empty check here

  if ( writeHandle.record(rangeIntersection, std::move(writeCdo)).isFailure() ) {
    ATH_MSG_FATAL("Could not record MuonMDT_CablingMap " << writeHandle.key() 
		  << " with EventRange " << rangeIntersection << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");
  
  return StatusCode::SUCCESS;
}
