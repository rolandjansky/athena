/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArRampCorr.h"

#include <fstream>

#include "LArRawConditions/LArConditionsContainerDB.h"

//#define LARRAMPBUILDER_DEBUGOUTPUT

LArRampCorr::LArRampCorr(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_detStore(0),
    m_onlineHelper(0)
{
 m_inputStringIDs.resize(0);
 declareProperty("KeyOutput",       m_keyoutput="LArRampCorr");
 declareProperty("GroupingType",    m_groupingType="SubDetector");
 declareProperty("Corrections",     m_inputStringIDs);
}

LArRampCorr::~LArRampCorr() 
{}

StatusCode LArRampCorr::initialize()
{

  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
    return StatusCode::FAILURE;
  } 
  
  sc = m_detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


// ********************** EXECUTE ****************************
StatusCode LArRampCorr::execute()
{ 
  return StatusCode::SUCCESS;
} 

// ********************** FINALIZE ****************************
StatusCode LArRampCorr::stop()
{ 
 MsgStream log(msgSvc(), name());
 log << MSG::INFO << "in stop." << endreq; 
  
 LArRampComplete* larRampCorr =new LArRampComplete();
 StatusCode sc=larRampCorr->setGroupingType(m_groupingType,log);
 if (sc.isFailure()) {
   log << MSG::ERROR << "Failed to set groupingType for LArRampComplete object" << endreq;
   return sc;
 }

 sc=larRampCorr->initialize(); 
 if (sc.isFailure()) {
   log << MSG::ERROR << "Failed initialize LArRampComplete object" << endreq;
   return sc;
 }


 std::vector<float> coeffs;
  
 std::vector<std::string>::const_iterator itrStringID=m_inputStringIDs.begin();
 for (;itrStringID!=m_inputStringIDs.end();++itrStringID) {

   std::string theString=*itrStringID;
   std::stringstream is;
   is << theString << std::endl;
   int iBarrel,iSide,iFT,iSlot,iChannel,iGain;
   float slope;

   is >>iBarrel >> iSide >> iFT >> iSlot >> iChannel >> iGain >> slope;
   
   HWIdentifier chid;
   try {
    chid  =    m_onlineHelper->channel_Id (iBarrel,
	   				   iSide,
					   iFT,
					   iSlot,
					   iChannel );   
   }
   catch(LArOnlID_Exception & except){
    
    
    log << MSG::ERROR 
	<<  " LArOnlId exception creating chid " 
	<< (std::string)except
	<< " barrel_ec, side, feedthrough, slot, channel= " << iBarrel << " " 
	<< iSide << " " 
	<< iFT << " " 
	<< iSlot << " "
	<< iChannel 
	<< endreq;
   }

   coeffs.resize(2,0);
   coeffs[0]=0.;
   coeffs[1]=slope;

   std::cout << " Fix channel " << iBarrel << " " << iSide << " " << iFT << " " << iSlot << " "
             << " " << iChannel << " Gain = " << iGain << " New Slope " << slope << std::endl;
   
   //LArRampP ramp; // avoid direct use of payload object!
   LArRampComplete::LArCondObj ramp;
   
   ramp.m_vRamp = coeffs;
   larRampCorr->insertCorrection(chid,ramp,iGain);
 }

 m_detStore->record(larRampCorr,m_keyoutput);
 m_detStore->symLink(larRampCorr, (ILArRamp*)larRampCorr);
 log << MSG::INFO << "LArRampCorr has finished." << endreq;
 
 return StatusCode::SUCCESS;
}// end finalize-method.
