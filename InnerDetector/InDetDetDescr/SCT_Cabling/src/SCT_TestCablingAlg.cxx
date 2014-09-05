/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TestCablingAlg.cxx
 * Implementation file for  SCT cabling service
 * @author Shaun Roe
 * @date 20 October, 2008
 **/
 
//STL, boost
#include <set>
#include "boost/cstdint.hpp"
//for o/p to file
#include <iostream>
#include <fstream>
//to access environment
#include <cstdlib>


//Gaudi
#include "GaudiKernel/StatusCode.h"

//Athena
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
//Package
#include "SCT_TestCablingAlg.h"
#include "SCT_CablingUtilities.h"



using namespace std;
using namespace SCT_Cabling;
 
SCT_TestCablingAlg::SCT_TestCablingAlg (const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator),
  m_cablingSvc("SCT_CablingSvc",name),
  m_idHelper(0){
  //nop
}

SCT_TestCablingAlg::~SCT_TestCablingAlg(){
  //nop
}

StatusCode
SCT_TestCablingAlg::initialize(){
  ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
  if (detStore.retrieve().isFailure()) return msg(MSG::FATAL)  << "Detector service  not found !" << endreq, StatusCode::FAILURE;
  if (detStore->retrieve(m_idHelper,"SCT_ID").isFailure()) return msg(MSG::ERROR)  << "SCT mgr failed to retrieve" << endreq, StatusCode::FAILURE;
  if(m_cablingSvc.retrieve().isFailure() ) return msg(MSG::FATAL)<<"Unable to get the cabling svc"<<endreq, StatusCode::FAILURE;
  msg(MSG::INFO)<<"Test algorithm for SCT_Cabling"<<endreq;
  return StatusCode::SUCCESS;
}

std::string
SCT_TestCablingAlg::coordString(const Identifier & offlineId){
  std::ostringstream os("");
  os<<"["<<m_idHelper->barrel_ec(offlineId)<<", ";
  os<<m_idHelper->layer_disk(offlineId)<<", ";
  os<<m_idHelper->phi_module(offlineId)<<", ";
  os<<m_idHelper->eta_module(offlineId)<<", ";
  os<<m_idHelper->side(offlineId)<<"]";
  return os.str();
}

StatusCode
SCT_TestCablingAlg::execute(){
  string testAreaPath=CoveritySafe::getenv("TestArea");
  string filename=testAreaPath+"/cabling.txt";
  msg(MSG::INFO)<<"Filename: "<<filename<<endreq;
  ofstream opFile(filename.c_str(),ios::out);
  msg(MSG::INFO)<<"Executing..."<<endreq;
  msg(MSG::INFO)<<"hash, offline Id, online Id(hex), serial number"<<endreq;
  for (int i(0);i!=8176;++i){
    IdentifierHash hash(i);
    Identifier offlineId(m_idHelper->wafer_id(hash));
    SCT_OnlineId onlineId(m_cablingSvc->getOnlineIdFromHash(hash));
    SCT_SerialNumber sn(m_cablingSvc->getSerialNumberFromHash(hash));
    msg(MSG::INFO)<<i<<" "<<dec<<offlineId<<" "<<hex<<onlineId<<dec<<" "<<sn<<" "<<coordString(offlineId)<<endreq;
    opFile<<i<<" "<<dec<<offlineId<<" "<<hex<<onlineId<<dec<<" "<<sn<<" "<<coordString(offlineId)<<std::endl;
    if (m_cablingSvc->getHashFromOnlineId(onlineId) != hash){
      msg(MSG::INFO)<<"?? "<<m_cablingSvc->getHashFromOnlineId(onlineId)<<endreq;
    }
  }
  
  msg(MSG::INFO)<<"Size: "<<m_cablingSvc->size()<<endreq;
  std::vector<unsigned int> rods;
  m_cablingSvc->getAllRods(rods);
  msg(MSG::INFO)<<"Num. of rods= "<<rods.size()<<endreq;
  msg(MSG::INFO)<<"First rod id "<<std::hex<<rods[0]<<std::dec<<endreq;
  string sn("20220130000299");
  msg(MSG::INFO)<<"Hash from serial number "<<m_cablingSvc->getHashFromSerialNumber(sn)<<endreq;
  int tsn(130000299);
  msg(MSG::INFO)<<"Hash from truncated serial number "<<m_cablingSvc->getHashFromSerialNumber(tsn)<<endreq;
  unsigned long long snll(20220130000299LL);
  msg(MSG::INFO)<<"Hash from truncated serial number "<<m_cablingSvc->getHashFromSerialNumber(snll)<<endreq;
  msg(MSG::INFO)<<"Hash from onlineid "<<m_cablingSvc->getHashFromOnlineId(0x3d230006)<<endreq;
  msg(MSG::INFO)<<"Hash from invalid onlineid "<<m_cablingSvc->getHashFromOnlineId(0x3d250006)<<endreq;
  msg(MSG::INFO)<<"Hash from textfile onlineid "<<m_cablingSvc->getHashFromOnlineId(0x3d220010)<<endreq;
  msg(MSG::INFO)<<"Hash from db onlineid "<<m_cablingSvc->getHashFromOnlineId(0x3d220105)<<endreq;
  std::vector<IdentifierHash> hashVec;
  m_cablingSvc->getHashesForRod(hashVec, 0x220005);
  msg(MSG::INFO)<<"number of hashes for rod 0x220005: "<<hashVec.size()<<endreq;
  return StatusCode::SUCCESS;
}


StatusCode
SCT_TestCablingAlg::finalize(){
  msg(MSG::INFO)<<"finalize()"<<endreq;
  return StatusCode::SUCCESS;
}
