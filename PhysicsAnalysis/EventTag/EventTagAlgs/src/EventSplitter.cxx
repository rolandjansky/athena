/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventTagAlgs/EventSplitter.h"

#include "AnalysisTriggerEvent/CTP_Decision.h"
#include "AnalysisTriggerEvent/FakeHLTWord.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

#include "StoreGate/StoreGateSvc.h"

EventSplitter::EventSplitter(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator), 
    TagBuilderBase(),
    m_attribList(0),
    m_attrName ("NONE"),
    m_KeyCTP_Decision("CTP_Decision"),
    m_KeyHLTWord     ("Fake_HLTWord"),    
    m_PassAll (false)
{
  // init mask patterns
  for (int i=0; i<MaxN; ++i)
    {
      m_MaskCTP.push_back(std::vector<uint32_t>(8, 0));

      m_MaskTTW [i]=0;
      m_MaskHLT [i]=0;
    }

  // Declare the properties
  declareProperty("TagListName",         m_attrName);
  declareProperty("KeyCTP_Decision", m_KeyCTP_Decision);
  declareProperty("KeyHLTWord",      m_KeyHLTWord);

  declareProperty("CTPMask_1",       m_MaskCTP[0]);
  declareProperty("CTPMask_2",       m_MaskCTP[1]);
  declareProperty("CTPMask_3",       m_MaskCTP[2]);
  declareProperty("CTPMask_4",       m_MaskCTP[3]);

  declareProperty("TTWMask_1",       m_MaskTTW[0]);
  declareProperty("TTWMask_2",       m_MaskTTW[1]);
  declareProperty("TTWMask_3",       m_MaskTTW[2]);
  declareProperty("TTWMask_4",       m_MaskTTW[3]);
  declareProperty("HLTMask_1",       m_MaskHLT[0]);
  declareProperty("HLTMask_2",       m_MaskHLT[1]);
  declareProperty("HLTMask_3",       m_MaskHLT[2]);
  declareProperty("HLTMask_4",       m_MaskHLT[3]);
  declareProperty("PassAll",         m_PassAll);
}


StatusCode EventSplitter::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  // Check to see if it should be added to the tags
  if (m_attrName != "NONE") {
    // define attribute 
    //std::string name("Filter"+name());
    log << MSG::DEBUG << "Defining the attribute list specification." << endreq;
    log << MSG::DEBUG << "Adding attribute " << name() << " to tags" << endreq;
    if (m_attribListSpec!=0)
      addAttribute( name(), "bool" );
    else {
      log << MSG::DEBUG << "AttribListSpec not initialized" << endreq;
      m_attribListSpec  = new AthenaAttributeListSpecification;
    }                                                                               
  }
  
  // Locate the StoreGateSvc
  StatusCode sc = service("StoreGateSvc", m_StoreGate);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
      return sc;
    }
  return StatusCode::SUCCESS;
}


StatusCode EventSplitter::execute() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;

  // set attribute list to not filled, yet
  m_attribList=0;

  // if pass all
  if (m_PassAll)
    {
      setSuccess(true);
      return StatusCode::SUCCESS;
    }

  // get Trigger Type Word for EventInfo

  const DataHandle<EventInfo> eventInfo;
  const DataHandle<EventInfo> eventInfoEnd;
  StatusCode sc = m_StoreGate->retrieve(eventInfo, eventInfoEnd);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Cannot get event info" << endreq;
      return sc;
    }
  if (eventInfo == eventInfoEnd)
    {
      log << MSG::ERROR << "No event info objects" << endreq;
      return sc;
    }

  uint32_t triggerTypeWord = 0;
  const TriggerInfo *tInfo = eventInfo->trigger_info();
  if (tInfo!=0)
    triggerTypeWord = tInfo->level1TriggerType();

  // get CTP_Decision
  const CTP_Decision * ctpDecision = 0;
  sc =m_StoreGate->retrieve(ctpDecision, m_KeyCTP_Decision);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "could not find CTP_Decision" << endreq;
      //return sc;
    }

  // HLT word
  const FakeHLTWord * hltWord;
  sc =m_StoreGate->retrieve(hltWord, m_KeyHLTWord);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "could not find CTP_Decision" << endreq;
      return sc;
    }
  
  // check Trigger Type Word
  for (int iN=0; iN<MaxN; ++iN)
    {
      log << MSG::DEBUG << "TTW: " << MSG::hex << triggerTypeWord << " Mask: " <<  m_MaskTTW[iN] << " reqN: " << 
iN << endreq;
      uint32_t maskedWord = triggerTypeWord & m_MaskTTW[iN];
      int nItem = 0;
      for (int iBit=0; iBit<32; ++iBit)
	if ((0x1 << iBit) & maskedWord) ++nItem;
	  
      if (nItem > iN)
	{
	  setSuccess(true);
	  return StatusCode::SUCCESS;
	}
    }      

  // check CTP word
  if (ctpDecision != 0)
    {
      for (int iN=0; iN<MaxN; ++iN)
	{
	  uint32_t ctpWord = 0;
	  uint32_t mask = 0;
	  uint32_t maskedWord = 0;
	  int nItem = 0;

	  std::vector<uint32_t> ctpWords = ctpDecision->getWords();

	  for (size_t iCTP(0); iCTP < ctpWords.size(); ++iCTP) 
	    {

	      ctpWord = ctpWords[iCTP];
	      mask = m_MaskCTP[iN][iCTP];
	      maskedWord = ctpWord & mask;
	      log << MSG::DEBUG << "CTP" << iCTP << ": " << MSG::hex << ctpWord << " Mask: " <<  mask << " reqN: " << iN << endreq;

	      for (int iBit=0; iBit<32; ++iBit)
		if ((0x1 << iBit) & maskedWord) ++nItem;

	    } // for (size_t iCTP(0); iCTP < ctpWords.size(); ++iCTP)

	  if (nItem > iN)
	    {
	      setSuccess(true);
	      return StatusCode::SUCCESS;
	    }
	}      
    }
  
  // check HLT word
  for (int iN=0; iN<MaxN; ++iN)
    {
      log << MSG::DEBUG << "HLT: " << MSG::hex << hltWord->word() << " Mask: " <<  m_MaskHLT[iN] << " reqN: " << 
iN << endreq;
      uint32_t maskedWord = (hltWord->word()) & m_MaskHLT[iN];
      int nItem = 0;
      for (int iBit=0; iBit<32; ++iBit)
	if ((0x1 << iBit) & maskedWord) ++nItem;
	  
      if (nItem > iN)
	{
	  setSuccess(true);
	  return StatusCode::SUCCESS;
	}
    }      

  // if no item, set filter flag to false
  setSuccess(false);
  
  return StatusCode::SUCCESS;
}

StatusCode EventSplitter::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

void EventSplitter::setSuccess(bool flag)
{
  MsgStream log(msgSvc(), name());
  // Fetch attrib list if not done already
  log << MSG::DEBUG << "in setSuccess" << endreq;
  if (m_attrName != "NONE" && m_attribList==0) {
    // retrieve TagAthenaAttributeList
    StatusCode sc = m_StoreGate->retrieve( m_attribList, m_attrName);
    if (sc.isFailure())
      {
        log << MSG::ERROR << "No attribute list in SG" << endreq; 
        //return sc;
      }
  }

  if (flag) 
    {
      setFilterPassed(true);
      if (m_attrName != "NONE" && m_attribList!=0) 
	      (*m_attribList)[name()].setValue(true);
    }
  else
    {
      setFilterPassed(false);
      if (m_attrName != "NONE" && m_attribList!=0) 
	      (*m_attribList)[name()].setValue(false);
    }
  
  if (m_attrName != "NONE" && m_attribList!=0)
    {
      // decrease number of builders
      TagBuilderBase::decNumOfBuilder();
      // if this is the last builder, lock the Attribute List
      //if (TagBuilderBase::lastBuilder()) sc = m_StoreGate->setConst(m_attribList);
      if (TagBuilderBase::lastBuilder()) m_StoreGate->setConst(m_attribList);
    }
}


