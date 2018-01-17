/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArRampCorr.h"

#include <fstream>

#include "LArRawConditions/LArConditionsContainerDB.h"

//#define LARRAMPBUILDER_DEBUGOUTPUT

LArRampCorr::LArRampCorr(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
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
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
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
  ATH_MSG_INFO ( "in stop." );
  
  auto larRampCorr = std::make_unique<LArRampComplete>();
 ATH_CHECK( larRampCorr->setGroupingType(m_groupingType,msg()) );
 ATH_CHECK( larRampCorr->initialize() );

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
    
    
     ATH_MSG_ERROR(  " LArOnlId exception creating chid " 
                     << (std::string)except
                     << " barrel_ec, side, feedthrough, slot, channel= " << iBarrel << " " 
                     << iSide << " " 
                     << iFT << " " 
                     << iSlot << " "
                     << iChannel );
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

 ATH_CHECK( detStore()->record(std::move(larRampCorr),m_keyoutput) );
 ATH_MSG_INFO ( "LArRampCorr has finished." );
 
 return StatusCode::SUCCESS;
}// end finalize-method.
