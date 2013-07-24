/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTimeTuningNtuple.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

LArTimeTuningNtuple::LArTimeTuningNtuple (const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator),
  m_ntuplePtr(0),
  m_storeGateSvc(0),
  m_larOnlineHelper(0)
{
  declareProperty("TBPhaseKey",m_TBPhaseKey="");
  declareProperty("GlobalTimeOffsetKey",m_GlobalTimeKey="");
  declareProperty("FebTimeOffsetKey",m_FebTimeKey="");
  declareProperty("CellTimeOffsetKey",m_CellTimeOffsetKey);
}

LArTimeTuningNtuple::~LArTimeTuningNtuple() 
{}
                       
StatusCode LArTimeTuningNtuple::initialize(){
  MsgStream logstr(msgSvc(), name());
  StoreGateSvc* detStore;
  StatusCode sc;

  sc= service("StoreGateSvc",m_storeGateSvc);
  if(sc.isFailure()) {
    logstr<< MSG::ERROR << "StoreGate service not found" << endreq;
    return StatusCode::FAILURE;
  }

  sc=service("DetectorStore",detStore);
  if(sc.isFailure()) {
    logstr<< MSG::ERROR << "Detector service not found" << endreq;
    return StatusCode::FAILURE;
  }

  sc = detStore->retrieve(m_larOnlineHelper,"LArOnlineID");
  if (sc.isFailure()) {
    logstr  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }


  const std::string ntuplePath="/NTUPLES/FILE1/TIMETUNING";
  const std::string ntupleTitle="LAr Time Tuning";
  NTuplePtr nt(ntupleSvc(),ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }

  if (!nt) {
    logstr<< MSG::ERROR << "Booking of NTuple failed" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_GlobalTimeKey.length()>0) {
    sc=nt->addItem("GlobalTimeOffset",m_globalTimeNt,-100.0,100.0);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'GlobalTimeOffset' failed" << endreq;
      return StatusCode::FAILURE;
    }
  }
  if (m_TBPhaseKey.length()) {
    sc=nt->addItem("PhaseTimeOffset",m_phaseNt,-100.0,100.0);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'PhaseTimeOffset' failed" << endreq;
      return StatusCode::FAILURE;
    }
  }
  if (m_FebTimeKey.length()) {
    sc=nt->addItem("FebIndex",m_nFebNt,0,1500);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'FebIndex' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("FebTimeOffset",m_nFebNt,m_febTimeNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'FebTimeOffset' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("FebSlot",m_nFebNt,m_febSlotNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'FebSlot' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("FebFT",m_nFebNt,m_febFTNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'FebFT' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("FebID",m_nFebNt,m_febIDNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'FebID' failed" << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  if (m_CellTimeOffsetKey.length()) {
    sc=nt->addItem("CellIndex",m_nCellNt,0,1500);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellIndex' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellTimeOffset",m_nCellNt,m_cellTimeNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellTimeOffset' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellSlot",m_nCellNt,m_cellSlotNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellSlot' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellFT",m_nCellNt,m_cellFTNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellFT' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellID",m_nCellNt,m_cellIDNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellID' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellRegion",m_nCellNt,m_cellRegionNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellRegion' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellLayer",m_nCellNt,m_cellLayerNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellLayer' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellEta",m_nCellNt,m_cellEtaNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellEta' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellPhi",m_nCellNt,m_cellPhiNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellPhi' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellGain",m_nCellNt,m_cellGainNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellGain' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellChannel",m_nCellNt,m_cellChannelNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellChannel' failed" << endreq;
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("CellCalibLine",m_nCellNt,m_cellCalibLineNt);
    if (sc!=StatusCode::SUCCESS) {
      logstr<< MSG::ERROR << "addItem 'CellCalibLine' failed" << endreq;
      return StatusCode::FAILURE;
    }


  }
  
  m_ntuplePtr=nt;
  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuningNtuple::execute()
{  
  MsgStream logstr(msgSvc(), name());
  StatusCode sc;
  
  if (m_GlobalTimeKey.length()>0) {
    const LArGlobalTimeOffset* larGlobalTimeOffset;
    sc=m_storeGateSvc->retrieve(larGlobalTimeOffset,m_GlobalTimeKey);
    if (sc.isSuccess()) {
      m_globalTimeNt=larGlobalTimeOffset->TimeOffset();
      //std::cout << "Global Time Offset= " << m_globalTimeNt << std::endl;
    }
    else 
      {logstr << MSG::WARNING << "Could not retrieve LArGlobalTimeOffset with key '" << m_GlobalTimeKey << "'" << endreq;
       m_globalTimeNt=-999;
      }
  }
  
  if (m_TBPhaseKey.length()>0) {
    const TBPhase* tbPhase;
    sc=m_storeGateSvc->retrieve(tbPhase,m_TBPhaseKey);
    if (sc.isSuccess()) 
      m_phaseNt=tbPhase->getPhase();
    else {
      logstr << MSG::WARNING << "Could not retrieve TBPhase with key '" << m_TBPhaseKey << "'" << endreq;
      m_phaseNt=-999;
    }  
  }
    
  if (m_FebTimeKey.length()>0) {
    LArFEBTimeOffset* larFebTimeOffset;
    sc=m_storeGateSvc->retrieve(larFebTimeOffset,m_FebTimeKey);
    if (sc.isSuccess() && larFebTimeOffset->size()>0) {
      larFebTimeOffset->setDefaultReturnValue(-999);
      std::vector<HWIdentifier>::const_iterator it=m_larOnlineHelper->feb_begin();
      std::vector<HWIdentifier>::const_iterator it_e=m_larOnlineHelper->feb_end();
      m_nFebNt=0;
      for (;it!=it_e;it++) {
	m_febTimeNt[m_nFebNt] = larFebTimeOffset->TimeOffset(*it);
	m_febSlotNt[m_nFebNt] = m_larOnlineHelper->slot(*it);
	m_febFTNt[m_nFebNt]   = m_larOnlineHelper->feedthrough(*it);
	m_febIDNt[m_nFebNt]   = (*it).get_identifier32().get_compact();
	++m_nFebNt;
      }
    }
    else 
      logstr << MSG::WARNING << "Could not retrieve LArFebTimeOffset with key '" << m_FebTimeKey << "'" << endreq;
  }

  sc=ntupleSvc()->writeRecord(m_ntuplePtr);
  if (sc!=StatusCode::SUCCESS) {
    logstr << MSG::ERROR << "writeRecord failed" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuningNtuple::stop(){

  MsgStream logstr(msgSvc(), name());
  StatusCode sc;
  
  const LArEM_ID* emId;
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  emId=caloIdMgr->getEM_ID();
  if (!emId) {
    logstr << MSG::ERROR << "Could not get lar EM ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  sc = larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    logstr << MSG::ERROR << " Can't get LArCablingSvc " << endreq;
    return sc;
  }
  
  if (m_CellTimeOffsetKey.length()>0) {
    
    LArCellTimeOffset *cellTimeOffset=new LArCellTimeOffset();
    sc=m_storeGateSvc->retrieve(cellTimeOffset,m_CellTimeOffsetKey);
    if (sc.isSuccess() && cellTimeOffset->size()>0) {
    
      cellTimeOffset->setDefaultReturnValue(-999);
      
      std::map <HWIdentifier,float>::const_iterator it=cellTimeOffset->begin();
      std::map <HWIdentifier,float>::const_iterator it_e=cellTimeOffset->end();
      m_nCellNt=0;
      for (;it!=it_e;it++) {
        
	Identifier id = larCablingSvc->cnvToIdentifier(it->first);     

	m_cellTimeNt[m_nCellNt]      = cellTimeOffset->TimeOffset(it->first);
	m_cellSlotNt[m_nCellNt]      = m_larOnlineHelper->slot(it->first);
	m_cellFTNt[m_nCellNt]        = m_larOnlineHelper->feedthrough(it->first);
	m_cellIDNt[m_nCellNt]        = (it->first).get_identifier32().get_compact();
	m_cellRegionNt[m_nCellNt]    = emId->region(id) ;
	m_cellLayerNt[m_nCellNt]     = emId->sampling(id) ;
	m_cellEtaNt[m_nCellNt]       = emId->eta(id) ;
	m_cellPhiNt[m_nCellNt]       = emId->phi(id) ;
	m_cellGainNt[m_nCellNt]      = -999 ; /*** FIX ME ***/
	m_cellChannelNt[m_nCellNt]   = m_larOnlineHelper->channel(it->first);
	const std::vector<HWIdentifier>& calibLineV = larCablingSvc->calibSlotLine(it->first);
        std::vector<HWIdentifier>::const_iterator calibLineIt = calibLineV.begin();   
	m_cellCalibLineNt[m_nCellNt] = m_larOnlineHelper->channel(*calibLineIt) ;
	
	std::cout << " *** Ch = " << std::hex << it->first.get_compact() << std::dec
                  << " Time = " << cellTimeOffset->TimeOffset(it->first) << std::endl ;
	
	++m_nCellNt;
      }
    }
    else
      logstr << MSG::WARNING << "Could not retrieve LArCellTimeOffset with key '" << m_CellTimeOffsetKey << "'" << endreq;
  
    sc=ntupleSvc()->writeRecord(m_ntuplePtr);
    if (sc!=StatusCode::SUCCESS) {
      logstr << MSG::ERROR << "writeRecord failed" << endreq;
      return StatusCode::FAILURE;
    }
  
  }

  return StatusCode::SUCCESS;

}
