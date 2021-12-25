/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "MuonCondInterface/ITriggerCoolSvc.h"
#include "MuonCondCool/TriggerCool.h"

#include "string.h"

namespace MuonCalib {
TriggerCool::TriggerCool(const std::string& name, 
  ISvcLocator* pSvcLocator) :AthAlgorithm(name,pSvcLocator),
   m_log(msgSvc(),name),
   p_detstore(nullptr),
   p_coolsvc(nullptr),
   m_readDone(false),
   m_writeDoneEta(false),
   m_writeDonePhi(false),
   m_read(false),
   m_writeEta(false),
   m_writePhi(false),
   m_fileName_CM_Phi(""),
   m_fileName_Th0_Phi(""),
   m_fileName_CM(""),
   m_fileName_Th0(""),
//   m_fileName_Th1(""),
//   m_fileName_Th2(""),
   m_sequence(""),
   m_infoEta(""),
   m_infoPhi(""),
   m_chan_Eta(),
   m_chan_Phi(),
   m_phiFolder("/RPC/TRIGGER/CM_THR_PHI"),
   m_etaFolder("/RPC/TRIGGER/CM_THR_ETA")

{
  // declare properties

  declareProperty("Read",m_read);
  declareProperty("WritePhi",m_writePhi);
  declareProperty("WriteEta",m_writeEta);
  declareProperty("FileName_CM_Phi",m_fileName_CM_Phi);
  declareProperty("FileName_Th0_Phi",m_fileName_Th0_Phi);
  declareProperty("FileName_CM",m_fileName_CM);
  declareProperty("FileName_Th0",m_fileName_Th0);
  //  declareProperty("FileName_Th1",m_fileName_Th1);
  //  declareProperty("FileName_Th2",m_fileName_Th2);
  declareProperty("Sequence",m_sequence);
  declareProperty("InfoEta",m_infoEta);
  declareProperty("InfoPhi",m_infoPhi);
  declareProperty("Chan_Phi",m_chan_Phi);
  declareProperty("Chan_Eta",m_chan_Eta);
  declareProperty("PhiFolder",m_phiFolder);
  declareProperty("EtaFolder",m_etaFolder);


}

TriggerCool::~TriggerCool() {}

StatusCode TriggerCool::initialize()
{
  m_log << MSG::INFO << "TriggerCool::initialize() called" << endmsg;

  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endmsg;
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=service("MuonCalib::TriggerCoolSvc",p_coolsvc)) {
    m_log << MSG::ERROR << "Cannot get TriggerCoolSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  

}

StatusCode TriggerCool::execute() {

  if (m_writePhi) {

    if(!m_writeDonePhi){

      m_writeDonePhi=true;
      m_log << MSG::DEBUG << "writing to DB Phi"<<endmsg;

      if(p_coolsvc->writeToDBPhi(m_phiFolder,m_fileName_CM_Phi, m_fileName_Th0_Phi,m_chan_Phi,m_infoPhi)!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in writing to DB Phi"<<endmsg;
      m_log << MSG::DEBUG << "***********************************************************"<<endmsg;
    }
  }


  if (m_writeEta) {

    if(!m_writeDoneEta){

      m_writeDoneEta=true;

      if(p_coolsvc->writeToDBEta(m_etaFolder,m_fileName_CM, m_fileName_Th0, m_chan_Eta, m_sequence, m_infoEta )!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in writing to DB Eta"<<endmsg;

    }
  }

 

  if (m_read) {
   
    m_readDone=true;
    if(p_coolsvc->getData(m_phiFolder, m_chan_Phi)!=StatusCode::SUCCESS) m_log << MSG::ERROR << "Problems in reading to DB PHI +++++++++++++++"<<endmsg;
 
    m_log << MSG::DEBUG << "reading to DB PHI +++++++++++++++"<<endmsg;
    // readData();
    return StatusCode::SUCCESS;
  }


  return StatusCode::SUCCESS;

}

StatusCode TriggerCool::finalize() {
  return StatusCode::SUCCESS;
}


}
