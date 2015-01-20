/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventTagAlgs/EventSplitter.h"

#include "AnalysisTriggerEvent/CTP_Decision.h"
#include "AnalysisTriggerEvent/FakeHLTWord.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/TagAthenaAttributeList.h"

#include "xAODEventInfo/EventInfo.h"

#include "StoreGate/StoreGateSvc.h"

EventSplitter::EventSplitter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), 
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
  ATH_MSG_INFO( "in initialize()" );

  // Check to see if it should be added to the tags
  if (m_attrName != "NONE") {
    // define attribute 
    //std::string name("Filter"+name());
    ATH_MSG_DEBUG( "Defining the attribute list specification." );
    ATH_MSG_DEBUG( "Adding attribute " << name() << " to tags" );
    if (m_attribListSpec!=0)
      addAttribute( name(), "bool" );
    else {
      ATH_MSG_DEBUG( "AttribListSpec not initialized" );
      m_attribListSpec  = new AthenaAttributeListSpecification;
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode EventSplitter::execute() 
{
  ATH_MSG_DEBUG( "in execute()" );

  // set attribute list to not filled, yet
  m_attribList=0;

  // if pass all
  if (m_PassAll)
    {
      setSuccess(true);
      return StatusCode::SUCCESS;
    }

  // get Trigger Type Word for EventInfo

  const DataHandle<xAOD::EventInfo> eventInfo;
  const DataHandle<xAOD::EventInfo> eventInfoEnd;
  ATH_CHECK( evtStore()->retrieve(eventInfo, eventInfoEnd) );
  if (eventInfo == eventInfoEnd)
    {
      ATH_MSG_ERROR( "No event info objects" );
      return StatusCode::FAILURE;
    }

  uint32_t triggerTypeWord = eventInfo->level1TriggerType();

  // get CTP_Decision
  const CTP_Decision * ctpDecision = 0;
  StatusCode sc =evtStore()->retrieve(ctpDecision, m_KeyCTP_Decision);
  if (sc.isFailure())
    {
      ATH_MSG_ERROR( "could not find CTP_Decision" );
      //return sc;
    }

  // HLT word
  const FakeHLTWord * hltWord = nullptr;
  ATH_CHECK( evtStore()->retrieve(hltWord, m_KeyHLTWord) );
  
  // check Trigger Type Word
  for (int iN=0; iN<MaxN; ++iN)
    {
      ATH_MSG_DEBUG( "TTW: " << MSG::hex << triggerTypeWord << " Mask: " <<  m_MaskTTW[iN] << " reqN: " << 
                     iN );
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
	      ATH_MSG_DEBUG( "CTP" << iCTP << ": " << MSG::hex << ctpWord << " Mask: " <<  mask << " reqN: " << iN );

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
      ATH_MSG_DEBUG( "HLT: " << MSG::hex << hltWord->word() << " Mask: " <<  m_MaskHLT[iN] << " reqN: " << 
                     iN );
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
  ATH_MSG_INFO( "in finalize()" );
  return StatusCode::SUCCESS;
}

void EventSplitter::setSuccess(bool flag)
{
  // Fetch attrib list if not done already
  ATH_MSG_DEBUG( "in setSuccess" );
  if (m_attrName != "NONE" && m_attribList==0) {
    // retrieve TagAthenaAttributeList
    StatusCode sc = evtStore()->retrieve( m_attribList, m_attrName);
    if (sc.isFailure())
      {
        ATH_MSG_ERROR( "No attribute list in SG" );
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
      //if (TagBuilderBase::lastBuilder()) evtStore()->setConst(m_attribList);
      if (TagBuilderBase::lastBuilder())
        evtStore()->setConst(m_attribList).ignore();
    }
}


