/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcReadWriteCoolStr.cxx - simple example of algorithm demonstrating 
// read/write of CondStrFile
// 18 may 2006 monica.verducci@cern.ch

#include "GaudiKernel/ISvcLocator.h"
#include "MuonCondInterface/RpcICoolStrSvc.h"
#include "MuonCondCool/RpcReadWriteCoolStr.h"

#include "string.h"

namespace MuonCalib {

RpcReadWriteCoolStr::RpcReadWriteCoolStr(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(nullptr),
   p_coolsvc(nullptr),
   m_readDone(false),
   m_writeDone(false),
   m_read(false),
   m_write(false),
   m_onlineWrite(false),
   m_onlineRead(false),
   m_fileName("")

{
  // declare properties

  declareProperty("Read",m_read);
  declareProperty("Write",m_write);
  declareProperty("WriteToOnlineDb",m_onlineWrite);
  declareProperty("ReadFromOnlineDb",m_onlineRead);
  declareProperty("FileName",m_fileName);
}

RpcReadWriteCoolStr::~RpcReadWriteCoolStr() {}

StatusCode RpcReadWriteCoolStr::initialize()
{
  m_log << MSG::INFO << "RpcReadWriteCoolStr::initialize() called" << endmsg;

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endmsg;
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=service("RpcCoolStrSvc",p_coolsvc)) {
    m_log << MSG::ERROR << "Cannot get RpcCoolStrSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  

}

StatusCode RpcReadWriteCoolStr::execute() {

  if(m_fileName==""){

    m_log << MSG::ERROR << "You must specify a file name!"<<endmsg;
    return StatusCode::FAILURE;

  }


  if (m_write) {

    if(!m_writeDone){

      m_writeDone=true;

      if(p_coolsvc->putFile(m_fileName)!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in writing to DB"<<endmsg;

    }
  }

  if (m_read) {

    if(!m_readDone){

      m_readDone=true;

      if(p_coolsvc->makeFile(m_fileName)!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in reading from DB"<<endmsg;

    }
  }

  if (m_onlineWrite) {

    if(!m_writeDone){

      m_writeDone=true;
      if(p_coolsvc->putOnlineFile(m_fileName)!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in writing to DB"<<endmsg;

    }
  }

  if (m_onlineRead) {

    if(!m_readDone){

      m_readDone=true;

      if(p_coolsvc->makeOnlineFile(m_fileName)!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in reading from DB"<<endmsg;

    }
  }


  return StatusCode::SUCCESS;

}

StatusCode RpcReadWriteCoolStr::finalize() {
  return StatusCode::SUCCESS;
}

    
} //namespace

