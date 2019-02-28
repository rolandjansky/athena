/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FindDuplicatedLArDigits.h"
#include "CaloIdentifier/CaloGain.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBScintillatorCont.h"

#include "LArElecCalib/ILArPedestal.h"
#include <fstream>

 

FindDuplicatedLArDigits::FindDuplicatedLArDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_nProblemEvent(0),
    m_onlineHelper(0),
    m_emId(0),
    m_hecId(0),
    m_fcalId(0)
{ 
  declareProperty("ContainerKey",m_contKey);
  m_nDigits=-1;
}

FindDuplicatedLArDigits::~FindDuplicatedLArDigits() 
{}

StatusCode FindDuplicatedLArDigits::initialize()
{
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();
  m_fcalId=idHelper->fcal_idHelper();
  m_hecId=idHelper->hec_idHelper();

  if (!m_emId) {
    ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  if (!m_fcalId) {
    ATH_MSG_ERROR ( "Could not access lar FCAL ID helper" );
    return StatusCode::FAILURE;
  }
  if (!m_hecId) {
    ATH_MSG_ERROR ( "Could not access lar HEC ID helper" );
    return StatusCode::FAILURE;
  }


  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_MSG_DEBUG ( " Found the LArOnlineID helper. " );

  m_nProblemEvent=0;
  return StatusCode::SUCCESS;
}  

StatusCode FindDuplicatedLArDigits::execute()
{
  // Retrieve EventInfo
  const DataHandle<xAOD::EventInfo> thisEventInfo;
  StatusCode sc=evtStore()->retrieve(thisEventInfo);
  unsigned eventnumber=0;
  if (sc!=StatusCode::SUCCESS)
    ATH_MSG_WARNING ( "No EventInfo object found!" );
  else {
    eventnumber=thisEventInfo->eventNumber();
  }

  // Retrieve LArDigitContainer
  const DataHandle < LArDigitContainer > digit_cont;
  if (m_contKey.size())
    ATH_CHECK( evtStore()->retrieve(digit_cont,m_contKey) );
  else
    ATH_CHECK( evtStore()->retrieve(digit_cont) );
  ATH_MSG_DEBUG ( "Retrieved LArDigitContainer from StoreGate! key=" << m_contKey );

  int nDigits=digit_cont->size();
  if (m_nDigits==-1 || m_nDigits==nDigits) { //first event or no change 
    ATH_MSG_DEBUG ( "Event " << eventnumber << ": Found " << nDigits << " Digits in container" );
  }
  else {
    ATH_MSG_ERROR ( "Event " << eventnumber << ": Size of digit container changed! Now: " << nDigits << " Previous Event: " << m_nDigits );
  }
  m_nDigits=nDigits;
  m_bitpattern.reset();
  LArDigitContainer::const_iterator it = digit_cont->begin();
  LArDigitContainer::const_iterator it_e = digit_cont->end();
  unsigned nDoubleDigits=0;
  for(; it!=it_e; ++it){
    const HWIdentifier hwid=(*it)->channelID();//hardwareID();
    const IdentifierHash h=m_onlineHelper->channel_Hash(hwid);
    if (m_bitpattern.test(h)) {
      int barrel_ec = m_onlineHelper->barrel_ec(hwid);
      std::string bc;
      if (barrel_ec==0)
	bc="Barrel";
      else if (barrel_ec==1)
	bc="Endcap";
      else
	bc="unknown";

      int pos_neg   = m_onlineHelper->pos_neg(hwid);
      std::string pn;
      if (pos_neg==0) 
	pn="C-Side";
      else if (pos_neg==1) 
	pn="A-Side";
      else
	pn="unknown";

      int FT        = m_onlineHelper->feedthrough(hwid);
      int slot      = m_onlineHelper->slot(hwid);
      int channel   = m_onlineHelper->channel(hwid);
      ATH_MSG_ERROR ( "Event " << eventnumber << " Found duplicated cell! Location: (" << bc << "/" << pn << "/FT=" 
                      << FT << "/Slot=" << slot << "/Channel=" << channel << ")" );
      nDoubleDigits++;
    }
    m_bitpattern.set(h);
  }
  if (nDoubleDigits) {
    m_nProblemEvent++;
    ATH_MSG_ERROR ( "Found " << nDoubleDigits << " duplicated digits in event " << eventnumber );
    //return StatusCode::RECOVERABLE;
  }
  return StatusCode::SUCCESS;
}// end finalize-method.


StatusCode FindDuplicatedLArDigits::finalize() {
  if (m_nProblemEvent)
    ATH_MSG_ERROR ( "Found  " << m_nProblemEvent << " Events with duplicated LArDigits" );
  else
    ATH_MSG_INFO ( "No duplicated LArDigits found." );
  return StatusCode::SUCCESS;
}
