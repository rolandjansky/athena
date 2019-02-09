/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibDataQuality/LArBadEventCatcher.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h" 
#include "xAODEventInfo/EventInfo.h"
#include "LArIdentifier/LArOnlineID.h"

LArBadEventCatcher::LArBadEventCatcher(const std::string & name, ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator), 
  m_onlineID(0),
  m_thisSize(0)
{
  m_keyList.push_back("HIGH");
  m_keyList.push_back("MEDIUM");
  m_keyList.push_back("LOW");
  
  declareProperty("CheckDigitCont",m_checkDigits=false);
  declareProperty("CheckAccCalibDigitCont",m_checkAccCalibDigits=false);
  declareProperty("CheckFebHeader",m_checkFebHeaders=false);
  declareProperty("CheckBSErrors",m_checkBSErrors=false);
  declareProperty("KeyList",m_keyList);
  declareProperty("StopOnError",m_stopOnError=true);


  m_nDigits=0; 
  m_nAccCalibDigits=0; 
  m_nFebheaders=0;
  m_nEvent=0;
  m_BSErrFailMask=0;
  m_stopJob=false;
  m_thisSize=0;
}


LArBadEventCatcher::~LArBadEventCatcher() {
}
				     

StatusCode LArBadEventCatcher::initialize() {

  if (!m_checkDigits && !m_checkAccCalibDigits && !m_checkFebHeaders && !m_checkBSErrors) 
    ATH_MSG_WARNING ("All checks switched off ?!");

  ATH_MSG_INFO ("LArBadEventChacher initialized");


  m_BSErrFailMask=(1<< LArFebErrorSummary::CheckSum);

  /// Get LAr Online ID helper class
  StatusCode sc = detStore()->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not get LArOnlineID helper !");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode LArBadEventCatcher::execute() {

  if (m_checkDigits || m_checkAccCalibDigits || m_checkFebHeaders)
    m_thisSize=0;
  else
    m_thisSize=1; //Assume non-empty event if no event-container is checked

  if (m_checkDigits) {
    for (size_t i=0;i<m_keyList.size();++i) {	   
      if (evtStore()->contains<LArDigitContainer>(m_keyList[i])) {
	const LArDigitContainer* cont;
	StatusCode sc=evtStore()->retrieve(cont,m_keyList[i]);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("Can't retrieve LArDigitContainer with key " << m_keyList[i]);
	  return StatusCode::FAILURE;
	}
	const size_t currSize=cont->size();
	m_thisSize=currSize;
	if (currSize>0) { //We ignore emtpy containers
	  if (m_nDigits==0)  m_nDigits=currSize; //Apparently the first event with an non-empty container
	  if (m_nDigits!=currSize) {
	    ATH_MSG_FATAL (eventDetails() <<"Mismatch in size of LArDigitContainer: Have " << currSize << " elements, expected " << m_nDigits);
	    m_stopJob=true;
	    if (m_stopOnError)
	      return StatusCode::FAILURE;
	  } //end if have mismatch
	}//end if currSize>0
      }//end if storeGate->contains
    }//end loop over keys
  }//end if m_checkDigits


   if (m_checkFebHeaders) {
    for (size_t i=0;i<m_keyList.size();++i) {	   
      if (evtStore()->contains<LArFebHeaderContainer>(m_keyList[i])) {
	const LArFebHeaderContainer* cont;
	StatusCode sc=evtStore()->retrieve(cont,m_keyList[i]);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("Can't retrieve LArFebHeaderContainer with key " << m_keyList[i]);
	  return StatusCode::FAILURE;
	}
	const size_t currSize=cont->size();
	if (currSize>0) { //We ignore emtpy containers
	  if (m_nFebheaders==0)  m_nFebheaders=currSize; //Apparently the first event with an non-empty container
	  if (m_nFebheaders!=currSize) {
	    ATH_MSG_FATAL (eventDetails() << "Mismatch in size of LArFebHeaderContainer: Have " << currSize << " elements, expected " << m_nFebheaders);
	    m_stopJob=true;
	    if (m_stopOnError)
	      return StatusCode::FAILURE;
	  } //end if have mismatch
	}//end if currSize>0
      }//end if storeGate->contains
    }//end loop over keys
  }//end if m_checkFebHeaders



  if (m_checkAccCalibDigits) {
    for (size_t i=0;i<m_keyList.size();++i) {
      if (evtStore()->contains<LArAccumulatedCalibDigitContainer>(m_keyList[i])) {
	const LArAccumulatedCalibDigitContainer* cont;
	StatusCode sc=evtStore()->retrieve(cont,m_keyList[i]);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR ("Can't retrieve LArAccumulatedCalibDigitContainer with key " << m_keyList[i]);
	  return StatusCode::FAILURE;
	}
	const size_t currSize=cont->size();
	m_thisSize=currSize;
	if (currSize>0) { //We ignore emtpy containers
	  if (m_nAccCalibDigits==0)  m_nAccCalibDigits=currSize; //Apparently the first event with an non-empty container
	  if (m_nAccCalibDigits!=currSize) {
	    ATH_MSG_FATAL (eventDetails() << "Mismatch in size of LArAccCalibDigitContainer: Have " << currSize << " elements, expected " << m_nAccCalibDigits);
	    m_stopJob=true;
	    if (m_stopOnError)
	      return StatusCode::FAILURE;
	  } //end if have mismatch
	}//end if currSize>0
      }//end if storeGate->contains
    }//end loop over keys
  }//end if m_checkAccCalibDigits


 if (m_checkBSErrors) {
    const LArFebErrorSummary* febErrSum;
    StatusCode sc=evtStore()->retrieve(febErrSum);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ("Can't retrieve LArFEBErrorSummary");
      return sc;
    }
    
    const std::map<unsigned int,uint16_t>& allFebErrs=febErrSum->get_all_febs();
    std::map<unsigned int,uint16_t>::const_iterator it=allFebErrs.begin();
    std::map<unsigned int,uint16_t>::const_iterator it_e=allFebErrs.end();
    for(;it!=it_e;++it) {
      if (it->second & 0xEFFF) { //FIXME all events have missing header ???
	//std::cout << std::hex << "Feb: 0x" << it->first << " Err 0x" 
	//	  << it->second << std::endl;
	MSG::Level msglvl=MSG::WARNING;
	if ((it->second & m_BSErrFailMask) && (m_thisSize>0)) {
	  m_stopJob=true;  
	  if (m_stopOnError) msglvl=MSG::FATAL; else msglvl=MSG::ERROR;
	}
	const HWIdentifier fId(it->first);
	msg() << msglvl << eventDetails() << "FEB 0x" << std::hex << it->first << " reports the following error(s): " 
		 << decipherFebError(it->second) << endmsg; 
	msg() << msglvl << "Feb location: " << m_onlineID->channel_name(fId) << endmsg;

	if (m_thisSize>0) msg() << msglvl << "This event carries data" << endmsg;
	
	if (m_stopJob && m_stopOnError)
	  return StatusCode::FAILURE;
      }//end if error
    }//end loop over FEBs
  }//end if check BS Errs  


  m_nEvent++;
  return StatusCode::SUCCESS;
}


std::string LArBadEventCatcher::eventDetails() const {
  std::stringstream result;

  const xAOD::EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) 
    result << "[No EventInfo]";
  else {
    unsigned int evtNum = eventInfo->eventNumber();
    unsigned int runNum = eventInfo->runNumber();
    result << "Run " << runNum << ", Evt " << evtNum;
  }
  result << ", Idx " << m_nEvent << ": ";
  return result.str();
}


StatusCode LArBadEventCatcher::stop() {
  if (m_stopJob && m_stopOnError) {
    ATH_MSG_FATAL ("Fatal error found during execute");
    return StatusCode::FAILURE;
  }
  else
    return StatusCode::SUCCESS;
}


std::string LArBadEventCatcher::decipherFebError(const uint32_t errword) const {

  std::string result;

  if (errword & (0x1U << LArFebErrorSummary::Parity))
    result+="Parity Error, ";
  if (errword & (0x1U << LArFebErrorSummary::BCID))
    result+="BCID Mismatch, ";
  if (errword & (0x1U << LArFebErrorSummary::SampleHeader))
    result+="Sample Header Error, ";
  if (errword & (0x1U << LArFebErrorSummary::EVTID))
    result+="EVTID Error, ";
  if (errword & (0x1U << LArFebErrorSummary::ScacStatus))
    result+="SCAC Status, ";
  if (errword & (0x1U << LArFebErrorSummary::ScaOutOfRange))
    result+="SCA out of range, ";
  if (errword & (0x1U << LArFebErrorSummary::GainMismatch))
    result+="Gain Mismatch, ";
  if (errword & (0x1U << LArFebErrorSummary::TypeMismatch))
    result+="Type Mismatch ";
  if (errword & (0x1U << LArFebErrorSummary::NumOfSamples))
    result+="Number of Samples Mismatch, ";
  if (errword & (0x1U << LArFebErrorSummary::EmptyDataBlock))
    result+="Empty Data Block, ";
  if (errword & (0x1U << LArFebErrorSummary::DspBlockSize))
    result+="DSP Block Size, ";
  if (errword & (0x1U << LArFebErrorSummary::CheckSum))
    result+="Checksum error, ";
  if (errword & (0x1U << LArFebErrorSummary::MissingHeader))
    result+="Missing Header, ";

  if (result.size()<2)
    result="Unknown error";
  else 
    result.erase(result.end()-2,result.end());

  return result;
}

      

