/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderADC2EDataBase.h"
#include "LArROD/LArRawChannelBuilderStatistics.h"

#include "Identifier/Identifier.h"
#include "StoreGate/StoreGateSvc.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "LArIdentifier/LArOnline_SuperCellID.h" 


LArRawChannelBuilderADC2EDataBase::LArRawChannelBuilderADC2EDataBase(const std::string& type,
								     const std::string& name,
								     const IInterface* parent):
  LArRawChannelBuilderADC2EToolBase(type,name,parent),
  m_adc2mevTool("LArADC2MeVTool"),
  m_onlineHelper(0)
{
  m_helper = new LArRawChannelBuilderStatistics( 3,      // number of possible errors
					       0x20);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "no DataBase");
  m_helper->setErrorString(2, "bad Ramps");
  
  declareProperty("TestBadRamps",              m_testRamps  = true);
  
  declareProperty("RampMaxHighGain",           m_ramp_max_high   = 500.);
  declareProperty("RampMaxMediumGain",         m_ramp_max_medium = 5000.);
  declareProperty("RampMaxLowGain",            m_ramp_max_low    = 50000.);
  
  declareProperty("UseHighGainRampIntercept",  m_useIntercept_high   = false);
  declareProperty("UseMedGainRampIntercept",   m_useIntercept_medium = true);
  declareProperty("UseLowGainRampIntercept",   m_useIntercept_low    = true);
  declareProperty("ADC2MeVTool",               m_adc2mevTool);
}

StatusCode LArRawChannelBuilderADC2EDataBase::initialize() {

   if( m_adc2mevTool.retrieve().isFailure() ){
    msg(MSG::ERROR) << "Could not retrieve LArADC2MeVTool" << endmsg;
    return StatusCode::FAILURE;
  }
   else 
     msg(MSG::DEBUG) << "Retrieved LArADC2MeVTool" << endmsg;
  return StatusCode::SUCCESS;
}
  
StatusCode LArRawChannelBuilderADC2EDataBase::initTool()
{
 
  if ( m_isSC ) {
     const LArOnline_SuperCellID* lonlID;
     CHECK( detStore()->retrieve(lonlID, "LArOnline_SuperCellID") );
     m_onlineHelper = lonlID;

  }  else { // end of if m_isSC

    const LArOnlineID* lonlID;
    CHECK( detStore()->retrieve(lonlID, "LArOnlineID") );
    m_onlineHelper = lonlID;

  } // end of if m_isSC

  return StatusCode::SUCCESS;
}

bool
LArRawChannelBuilderADC2EDataBase::ADC2E(std::vector<float>& Ramps, MsgStream* /*pLog*/)
{
  //ADC2MeV (a.k.a. Ramp)   
  Ramps=m_adc2mevTool->ADC2MEV(m_parent->curr_chid,m_parent->curr_gain);
  
  //Check ramp coefficents
  if (Ramps.size()==0) {
    ATH_MSG_DEBUG("No ADC2MeV data found for channel 0x" << MSG::hex
		  << m_parent->curr_chid.get_compact() << MSG::dec
		  << " Gain "<< m_parent->curr_gain << " Skipping channel.");
    m_helper->incrementErrorCount(1);
    return false;
  }
  
  // temporary fix for bad ramps... should be done in the DB
  if( m_testRamps &&
      ((( m_parent->curr_gain == CaloGain::LARHIGHGAIN )   && Ramps[1]>m_ramp_max_high) ||
       (( m_parent->curr_gain == CaloGain::LARMEDIUMGAIN ) && Ramps[1]>m_ramp_max_medium) ||
       (( m_parent->curr_gain == CaloGain::LARLOWGAIN )    && Ramps[1]>m_ramp_max_low) ||
       Ramps[1]<0 ) )
    {
      ATH_MSG_DEBUG("Bad ramp for channel " << m_parent->curr_chid
		    << " (Ramps[1] = " << Ramps[1] << "): skip this channel");
      m_helper->incrementErrorCount(2);
      return false;
    }
  
  //use intercept ?
  // for HEC treat medium gain as high gains in the others subsystems
  bool useIntercept_medium = m_useIntercept_medium;
  if (m_onlineHelper->isHECchannel(m_parent->curr_chid)) useIntercept_medium = m_useIntercept_high;

  if( !((( m_parent->curr_gain == CaloGain::LARHIGHGAIN )   && m_useIntercept_high ) ||
	(( m_parent->curr_gain == CaloGain::LARMEDIUMGAIN ) && useIntercept_medium ) ||
	(( m_parent->curr_gain == CaloGain::LARLOWGAIN )    && m_useIntercept_low )) )
    Ramps[0]=0;
  
  /*  //otherwise ignore intercept, E=0;
      for (unsigned i=1;i<ramp.size();i++)
      {
      energy+=ramp[i]*ADCPeakPower; //pow(ADCPeak,i);
      //std::cout << "Step "<< i <<":" << ramp[i] << " * " << pow(ADCPeak,i) << "Sum=" << energy << std::endl;
      ADCPeakPower*=Peak;
      }
  */
  /*
    energy=Peak;
    m_helper->incrementErrorCount(1);
    
    return false;
  */
  //  (*pLog) << MSG::VERBOSE << "ADC2EDataBase tool - energy : " << energy << endmsg;
  m_helper->incrementErrorCount(0);
  m_parent->qualityBitPattern |= m_helper->returnBitPattern();
  
  return true;
}

