/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCabling/LArCablingService.h"
#include <set>
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"

//-------------------------------------------------------------
LArCablingService::LArCablingService( const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent ) 
  : LArCablingBase(type,name,parent),
    m_calibValid(false),
    m_febRodValid(false) {

  declareInterface<LArCablingService>(this);
  declareProperty("LArFebRodMapKey",m_febRodMapKey="/LAR/Identifier/FebRodMap");
  declareProperty("LArCalibIDFolder",m_calibIdKey="/LAR/Identifier/CalibIdMap");
}


LArCablingService::~LArCablingService() { } 

//---------------------------------------------------------
StatusCode LArCablingService::initialize ()
{
  ATH_MSG_DEBUG("Initializing LArCablingService");

  const LArOnlineID* onlineId;
  StatusCode sc=detStore()->retrieve(onlineId, "LArOnlineID");
  if (sc.isFailure()) {
    msg() <<  MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 

  m_onlineId=onlineId; //Cast to base class

  const CaloCell_ID* caloCell_ID;
  sc=detStore()->retrieve(caloCell_ID,"CaloCell_ID");
  if (sc.isFailure()) {
    msg() <<  MSG::FATAL << "Could not get CaloCell_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_caloId=caloCell_ID; //Cast to base-class

  unsigned nCallbacks=0;

  if (m_onOffIdKey.size()) {
    sc=detStore()->regFcn(&LArCablingService::iovCallBack,this,m_attrOnOff,m_onOffIdKey,true);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to register callback on SG key" << m_onOffIdKey << endmsg;
    }
    else {
      msg(MSG::INFO) << "Successfully installed callback on folder" << m_onOffIdKey << endmsg;
      nCallbacks++;
    }
  }
 
  if (m_calibIdKey.size()) {
    sc=detStore()->regFcn(&LArCablingService::iovCallBack,this,m_attrCalib,m_calibIdKey);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to register callback on SG key" << m_calibIdKey << endmsg;
    }
    else {
      msg(MSG::INFO) << "Successfully installed callback on folder" << m_calibIdKey << endmsg;
      nCallbacks++;
    }
  }
  if (m_febRodMapKey.size()) {
    sc=detStore()->regFcn(&LArCablingService::iovCallBack,this, m_attrFebRod,m_febRodMapKey); 
    if (sc.isFailure()) {
      msg() <<MSG::ERROR<<"Failed to register callback on SG key" << m_febRodMapKey << endmsg;
    }
    else {
      msg(MSG::INFO) << "Successfully installed callback on folder" << m_febRodMapKey << endmsg;
      nCallbacks++;
    }
  }

  if (nCallbacks==0) {
    msg(MSG::ERROR) << "No callback was sucessfully installed! Configuration problem?" << endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO)<< "Sucessfully initialized LArCablingService with " << nCallbacks << " callbacks." << endmsg; 
  return StatusCode::SUCCESS;
}



StatusCode LArCablingService::iovCallBack(IOVSVC_CALLBACK_ARGS_K(keys)) {
  msg() << MSG::INFO<<" ====> iovCallBack " << endmsg;
  
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_DEBUG("IOV callback for key " << *itr);
    if (*itr==m_onOffIdKey) {
      m_onOffValid=false;
      readOnlOffMap();
      continue;
    }
    if (*itr==m_calibIdKey) {
      m_calibValid=false;
      continue;
    }
    if (*itr== m_febRodMapKey) {
      m_febRodValid=false;
      continue;
    }

    msg(MSG::WARNING) << "Callback fired for unknown key " << *itr << endmsg;
  } //end loop over keys
  return StatusCode::SUCCESS;
}



bool LArCablingService::readCalibMap() {
  msg(MSG::DEBUG) << "Start reading calibration line mapping" << endmsg;
  m_calibValid=false;
  m_onlHashToCalibLines.clear();
  StatusCode sc=detStore()->retrieve(m_attrCalib,m_calibIdKey);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to read AthenaAttributeList with key " << m_calibIdKey << endmsg;
    return false;
  }
  const coral::Blob& blobCalib=(*m_attrCalib)["OnlineHashToCalibIds"].data<coral::Blob>();
  const unsigned nEntries=blobCalib.size()/sizeof(uint32_t);
  const uint32_t* pBlobCalib=static_cast<const uint32_t*>(blobCalib.startingAddress());

  const unsigned chanMax=m_onlineId->channelHashMax();

  m_onlHashToCalibLines.resize(chanMax);
  

  unsigned blobIdx=0;
  unsigned chanIdx=0;
  unsigned totCalibLines=0;
  for (;blobIdx<nEntries && chanIdx<chanMax;++chanIdx) {
    const unsigned nCalibLines=pBlobCalib[blobIdx++];
    totCalibLines+=nCalibLines;
    if (nCalibLines>4) {
      msg(MSG::ERROR) << "Found unresonable large number of calib lines (" << nCalibLines << ") for channel hash " << chanIdx << endmsg;
      return false;
    }
    m_onlHashToCalibLines[chanIdx].clear();
    for (unsigned i=0;i<nCalibLines && blobIdx<nEntries;++i) {
      m_onlHashToCalibLines[chanIdx].push_back(HWIdentifier(pBlobCalib[blobIdx++])); //FIXME C++11 emplace_back
    }//End loop over calib-lines
  }//end loop over channels
  msg() << MSG::INFO << "Done reading readout/calibration line mapping." << endmsg;
  ATH_MSG_DEBUG("BlobIdx=" << blobIdx<<", chanIdx=" << chanIdx << ", totCalibLines=" << totCalibLines);
  m_calibValid=true;
  return m_calibValid;
}


bool LArCablingService::readFebRodMap() {
  ATH_MSG_DEBUG("Start reading Feb/Rod mapping");
  m_febRodValid=false;
  m_pFebHashtoROD=NULL;
  m_readoutModuleIDVec.clear();
  StatusCode sc=detStore()->retrieve(m_attrFebRod,m_febRodMapKey);
  if (sc.isFailure()) {
    msg() << MSG::ERROR << "Failed to read AthenaAttributeList with key " << m_febRodMapKey << endmsg;
    return false;
  }
  const coral::Blob& blobFebRod=(*m_attrFebRod)["FebHashToRODs"].data<coral::Blob>();
  unsigned nFebRod=blobFebRod.size()/sizeof(uint32_t);
  m_pFebHashtoROD=static_cast<const uint32_t*>(blobFebRod.startingAddress());

  std::set<uint32_t> allReadoutModuleIds;
  for (unsigned i=0;i<nFebRod;++i) 
    allReadoutModuleIds.insert(m_pFebHashtoROD[i]);

  //Fixme: this may require some special ordering! 
  std::set<uint32_t>::const_iterator it=allReadoutModuleIds.begin();
  std::set<uint32_t>::const_iterator it_e=allReadoutModuleIds.end();
  for(;it!=it_e;++it) {
    m_readoutModuleIDVec.push_back(HWIdentifier(*it));
  }

  m_febRodValid=true;
  msg(MSG::INFO) << "Done reading Feb/Rod mapping. Found " << nFebRod << " Febs and " <<  m_readoutModuleIDVec.size() << " Rods" << endmsg;
  return  m_febRodValid;
}



const std::vector<HWIdentifier>& LArCablingService::calibSlotLine(const HWIdentifier & sid) { 
  if (m_calibValid || readCalibMap()) {
    const IdentifierHash sid_hash=m_onlineId->channel_Hash(sid); 
    return m_onlHashToCalibLines[sid_hash];
  }
  else {
    // return empty vector if there is no mapping. 
    return m_vecEmpty;
  }
} 



HWIdentifier LArCablingService::getReadoutModuleID(const HWIdentifier& febId) {
  const IdentifierHash fHash=m_onlineId->feb_Hash(febId);
  return getReadoutModuleIDByHash(fHash);
}


HWIdentifier LArCablingService::getReadoutModuleIDByHash(const IdentifierHash& febIdHash) {
  if (m_febRodValid || readFebRodMap()) {
    //Check range?
    //std::cout<< "getReadoutModuleIDByHash " << std::hex << m_pFebHashtoROD[febIdHash] << std::dec << std::endl;
    return HWIdentifier(m_pFebHashtoROD[febIdHash]);
  }
  else
    return m_hwidEmpty;
}
