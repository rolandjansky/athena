/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTimeTuningNtuple.h"
#include "LArCabling/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

LArTimeTuningNtuple::LArTimeTuningNtuple (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_ntuplePtr(0),
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
  ATH_CHECK( detStore()->retrieve(m_larOnlineHelper,"LArOnlineID") );

  const std::string ntuplePath="/NTUPLES/FILE1/TIMETUNING";
  const std::string ntupleTitle="LAr Time Tuning";
  NTuplePtr nt(ntupleSvc(),ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }

  if (!nt) {
    ATH_MSG_ERROR ( "Booking of NTuple failed" );
    return StatusCode::FAILURE;
  }
  if (m_GlobalTimeKey.length()>0) {
    ATH_CHECK( nt->addItem("GlobalTimeOffset",m_globalTimeNt,-100.0,100.0) );
  }
  if (m_TBPhaseKey.length()) {
    ATH_CHECK( nt->addItem("PhaseTimeOffset",m_phaseNt,-100.0,100.0) );
  }
  if (m_FebTimeKey.length()) {
    ATH_CHECK( nt->addItem("FebIndex",m_nFebNt,0,1500) );
    ATH_CHECK( nt->addItem("FebTimeOffset",m_nFebNt,m_febTimeNt) );
    ATH_CHECK( nt->addItem("FebSlot",m_nFebNt,m_febSlotNt) );
    ATH_CHECK( nt->addItem("FebFT",m_nFebNt,m_febFTNt) );
    ATH_CHECK( nt->addItem("FebID",m_nFebNt,m_febIDNt) );
  }
  
  if (m_CellTimeOffsetKey.length()) {
    ATH_CHECK( nt->addItem("CellIndex",m_nCellNt,0,1500) );
    ATH_CHECK( nt->addItem("CellTimeOffset",m_nCellNt,m_cellTimeNt) );
    ATH_CHECK( nt->addItem("CellSlot",m_nCellNt,m_cellSlotNt) );
    ATH_CHECK( nt->addItem("CellFT",m_nCellNt,m_cellFTNt) );
    ATH_CHECK( nt->addItem("CellID",m_nCellNt,m_cellIDNt) );
    ATH_CHECK( nt->addItem("CellRegion",m_nCellNt,m_cellRegionNt) );
    ATH_CHECK( nt->addItem("CellLayer",m_nCellNt,m_cellLayerNt) );
    ATH_CHECK( nt->addItem("CellEta",m_nCellNt,m_cellEtaNt) );
    ATH_CHECK( nt->addItem("CellPhi",m_nCellNt,m_cellPhiNt) );
    ATH_CHECK( nt->addItem("CellGain",m_nCellNt,m_cellGainNt) );
    ATH_CHECK( nt->addItem("CellChannel",m_nCellNt,m_cellChannelNt) );
    ATH_CHECK( nt->addItem("CellCalibLine",m_nCellNt,m_cellCalibLineNt) );
  }
  
  m_ntuplePtr=nt;
  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuningNtuple::execute()
{  
  if (m_GlobalTimeKey.length()>0) {
    const LArGlobalTimeOffset* larGlobalTimeOffset;
    StatusCode sc=evtStore()->retrieve(larGlobalTimeOffset,m_GlobalTimeKey);
    if (sc.isSuccess()) {
      m_globalTimeNt=larGlobalTimeOffset->TimeOffset();
      //std::cout << "Global Time Offset= " << m_globalTimeNt << std::endl;
    }
    else {
      ATH_MSG_WARNING ( "Could not retrieve LArGlobalTimeOffset with key '" << m_GlobalTimeKey << "'" );
      m_globalTimeNt=-999;
    }
  }
  
  if (m_TBPhaseKey.length()>0) {
    const TBPhase* tbPhase;
    StatusCode sc=evtStore()->retrieve(tbPhase,m_TBPhaseKey);
    if (sc.isSuccess()) 
      m_phaseNt=tbPhase->getPhase();
    else {
      ATH_MSG_WARNING ( "Could not retrieve TBPhase with key '" << m_TBPhaseKey << "'" );
      m_phaseNt=-999;
    }  
  }
    
  if (m_FebTimeKey.length()>0) {
    LArFEBTimeOffset* larFebTimeOffset;
    StatusCode sc=evtStore()->retrieve(larFebTimeOffset,m_FebTimeKey);
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
      ATH_MSG_WARNING ( "Could not retrieve LArFebTimeOffset with key '" << m_FebTimeKey << "'" );
  }

  ATH_CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuningNtuple::stop(){

  const LArEM_ID* emId;
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  emId=caloIdMgr->getEM_ID();
  if (!emId) {
    ATH_MSG_ERROR ( "Could not get lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  ATH_CHECK(  larCablingSvc.retrieve() );
  
  if (m_CellTimeOffsetKey.length()>0) {
    
    LArCellTimeOffset *cellTimeOffset=new LArCellTimeOffset();
    StatusCode sc=evtStore()->retrieve(cellTimeOffset,m_CellTimeOffsetKey);
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
      ATH_MSG_WARNING ( "Could not retrieve LArCellTimeOffset with key '" << m_CellTimeOffsetKey << "'" );
  
    ATH_CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  }

  return StatusCode::SUCCESS;
}
