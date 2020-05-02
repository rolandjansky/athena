/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <map>

#include "MuonCondTool/MDT_MapConversion.h"


#include "MuonCondSvc/MdtStringUtils.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the MDT Map Info from ORACLE DB
//* retrieving mapping conversion tables from DB
//* convert method online name into offline ID 
//*********************************************************


MDT_MapConversion::MDT_MapConversion (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_mdtIdHelper(0),
    m_chronoSvc(0)
{
  
  declareInterface< IMDT_MapConversion >(this);
  
}


StatusCode MDT_MapConversion::initialize()
{
  MsgStream log(msgSvc(), name());

  StatusCode sc = detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (sc.isFailure())
    {
      log << MSG::FATAL << " Cannot retrieve MdtIdHelper " << endmsg;
      return sc;
    }
  
    
  if(sc.isFailure()) return StatusCode::FAILURE;
  
  
  
  // initialize the chrono service
  sc = service("ChronoStatSvc",m_chronoSvc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not find the ChronoSvc" << endmsg;
    return sc;
  }
	
 
  
  if(sc.isFailure()) return StatusCode::FAILURE;

  
  IGeoModelSvc *geoModel;
  ATH_CHECK(service ("GeoModelSvc",geoModel));
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string MuonVersion = geoModel->muonVersionOverride();
  std::string detectorKey = MuonVersion.empty() ? AtlasVersion : MuonVersion;
  std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";

 
  IRDBAccessSvc *accessSvc;
  ATH_CHECK(service("RDBAccessSvc",accessSvc));
  
  

//   const IRDBRecordset *switchSet = accessSvc->getRecordset("HwSwIdMapping", detectorKey, detectorNode);
 
//   if ((*switchSet).size()==0) {
//     log<< MSG::WARNING <<"Old Atlas Version :  "<< AtlasVersion << " Only Online Identifier"<<endmsg;
//     return StatusCode::SUCCESS;
    
//   }
 

  IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("HwSwIdMapping", detectorKey, detectorNode);

  if ((*switchSet).size()==0) {
    log<< MSG::WARNING <<"Old Atlas Version : "<< AtlasVersion << " Only Online Identifier. Falling back to HwSwIdMapping-00 tag"<<endmsg;
    switchSet = accessSvc->getRecordsetPtr("HwSwIdMapping","HwSwIdMapping-00");
  } 
  


  for (unsigned int irow=0; irow<(*switchSet).size(); ++irow){
    
    const IRDBRecord *switches = (*switchSet)[irow];
    std::string hardwareName = switches->getString("HARDNAME");
    std::string stName = switches->getString("SOFTNAME");
    int stPhi = switches->getInt("SOFTOCTANT");
    int stEta = switches->getInt("SOFTIZ");
    // log << MSG::INFO << "*********************" <<hardwareName<< endmsg; 
    //log << MSG::INFO << "*********************" <<stName<< endmsg;
    //log << MSG::INFO << "*********************" <<stPhi<< endmsg;
    // log << MSG::INFO << "*********************" <<stEta<< endmsg;
    Identifier ChamberId = m_mdtIdHelper->elementID(stName,stEta,stPhi);
    //log << MSG::INFO << "#### Chamber Name Offline" << ChamberId<< endmsg;
    
    //m_Chamber_Map.insert(std::make_pair(hardwareName,ChamberId));
    m_Chamber_Map[hardwareName]=ChamberId;
    
  }
	
    log << MSG::INFO << "#### Chamber Map SIZE" << m_Chamber_Map.size()<< endmsg;
  return StatusCode::SUCCESS;
 
  
}

const Identifier& MDT_MapConversion::ConvertToOffline(const std::string &OnlineId) const
{
  MsgStream log(msgSvc(), name());
//  int size = m_Chamber_Map.size();
  //log << MSG::VERBOSE << "*************** size is\n" << size<< endmsg;
  std::map<std::string, Identifier>::iterator iter;
  
  //const Identifier m_Online_empty;
  
  if (m_Chamber_Map.size()!=0){
    const auto& mapit = m_Chamber_Map.find(OnlineId);
    if (ATH_UNLIKELY(mapit == m_Chamber_Map.end())) { 
      log << MSG::ERROR << "Lookup of ID " << OnlineId << " in MDT_MapConversion::ConvertToOffline failed" << endmsg; 
      return m_Online_empty; // not quite right but should never get here
    }
    const Identifier & OfflineName = (mapit->second);
    return OfflineName;
  } else { 
    return m_Online_empty; 
  }
  
  
}





