/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArCellBuilderDriver.h"
#include "LArROD/LArRawChannelBuilderToolBase.h"
#include "LArROD/LArRawChannelBuilderADC2EToolBase.h"
#include "LArROD/LArRawChannelBuilderPedestalToolBase.h"

#include "LArRecUtils/LArParabolaPeakRecoTool.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "Identifier/Identifier.h"

#include "CLHEP/Units/SystemOfUnits.h"

LArCellBuilderDriver::LArCellBuilderDriver(
					const std::string& type,
					const std::string& name,
					const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_params (0),
  m_toolSvc(NULL),
  m_onlineHelper(0),
  m_DataLocation("FREE"),
  m_ChannelContainerName("LArRawChannels"),
  m_buildTools(),
  m_adc2eTools(),
  m_pedestalTools(),
  m_oldPedestal(0.)
{
  //m_useIntercept={false,false,false,false};
  declareProperty("LArRawChannelContainerName",   m_ChannelContainerName);
  declareProperty("DataLocation",                 m_DataLocation );
  declareProperty("BuilderTools",                 m_buildTools);
  declareProperty("ADCtoEnergyTools",             m_adc2eTools);
  declareProperty("PedestalTools",                m_pedestalTools);
  declareProperty("checkSamples",                 m_checkSamples=32);
  declareProperty("BuildDiscChannel",             m_buildDiscChannel=false);
  declareProperty("DefaultShiftTimeSample",       m_defaultShiftTimeSamples=0);
}

LArCellBuilderDriver::~LArCellBuilderDriver()
{ }

StatusCode LArCellBuilderDriver::initialize()
{
  // initialize parameters
  m_params = new LArRawChannelBuilderParams;
  m_params->m_larRawChannelContainer = NULL;
  
  //StatusCode sc;
  
  if (this->retrieveDetectorStore(m_onlineHelper, "LArOnlineID").isFailure())
    {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !"  );
      return StatusCode::FAILURE;
    }
  
  // ***
  //m_larRawOrdering.setMap(&(*m_roiMap)); 
  
  if ( m_buildTools.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "Unable to find Builder Tools " << m_buildTools  );
      return StatusCode::FAILURE; 
    }else{
    ATH_MSG_INFO( "Successfully retrieved Builder Tools " << m_buildTools  );
    }
  for( builderToolVector::iterator it = m_buildTools.begin(); it != m_buildTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      ATH_MSG_ERROR( "Unable to initialize Builder Tool " << (*it)->name()  );
  
  if ( m_adc2eTools.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "Unable to find ADC2E Tools " << m_buildTools  );
      return StatusCode::FAILURE; 
    }else{
    ATH_MSG_INFO( "Successfully retrieved ADC2E Tools " << m_buildTools  );
    }
  for( adc2eToolVector::iterator it = m_adc2eTools.begin(); it != m_adc2eTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      ATH_MSG_ERROR( "Unable to initialize ADC2E Tool " << (*it)->name()  );
  
  if ( m_pedestalTools.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "Unable to find Pedestal Tools " << m_buildTools  );
      return StatusCode::FAILURE; 
    }else{
    ATH_MSG_INFO( "Successfully retrieved Pedestal Tools " << m_buildTools  );
  }
  for( pedestalToolVector::iterator it = m_pedestalTools.begin(); it != m_pedestalTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      ATH_MSG_ERROR( "Unable to initialize Pedestal Tool " << (*it)->name()  );
  
  // check that we have tools to run the reconstruction !
  if( m_buildTools.size() == 0 )
    {
      ATH_MSG_ERROR( "Didn't find any BuilderTools to do reconstruction !"  );
      return(StatusCode::FAILURE);  
    }
  if( m_adc2eTools.size() == 0 )
    {
      ATH_MSG_ERROR( "Didn't find and ADC2ETools to do reconstruction !"  );
      return(StatusCode::FAILURE);  
    }
  if( m_pedestalTools.size() == 0 )
    {
      ATH_MSG_ERROR( "Didn't find and PedestalTools to do reconstruction !"  );
      return(StatusCode::FAILURE);  
    }
  return StatusCode::SUCCESS;
}

void LArCellBuilderDriver::initEventTools(){
  // declare all variables first ...
  builderToolVector::iterator itStart = m_buildTools.begin();
  builderToolVector::iterator itEnd   = m_buildTools.end();
  
  for( builderToolVector::iterator it = itStart;
       it != itEnd; it++ ) (*it)->initEventHidden();
  
  // reset to zero
  m_oldIdentifier=HWIdentifier(0);
}

bool LArCellBuilderDriver::buildLArCell(const LArDigit* digit,
					int&  energy ,
					int&  time ,
					CaloGain::CaloGain&  gain ,
					MsgStream* pLog)
{
  /* #ifndef NDEBUG
     if(bool(pLog))
     (*pLog) << MSG::DEBUG << "In buildLArCell" << endmsg;
     if(bool(pLog))
     (*pLog) << MSG::DEBUG << "chid : " << digit->channelID() << endmsg;
     if(bool(pLog))
     (*pLog) << MSG::DEBUG << "gain : " << digit->gain() << endmsg;
     #endif */
  
  //StatusCode sc;
  
  // declare all variables first ...
  builderToolVector::iterator itStart = m_buildTools.begin();
  builderToolVector::iterator itEnd   = m_buildTools.end();
  
      m_params->curr_chid=digit->channelID();
      m_params->curr_gain=digit->gain();
      
      if(!m_buildDiscChannel)
  	  return false;

      m_params->curr_sample0   = digit->samples()[0];
      m_params->curr_maximum   = m_params->curr_sample0;
      m_params->curr_maxsample = 0;
      m_params->curr_nsamples  = m_checkSamples < digit->nsamples() ?
	m_checkSamples : digit->nsamples();
      m_params->curr_shiftTimeSamples = m_defaultShiftTimeSamples;
      
      m_params->qualityBitPattern = 0;
      
      if(m_params->curr_nsamples>0)
	{
	  for( unsigned int i=0; i<m_params->curr_nsamples; i++ )
	    {
	      if( m_params->curr_maximum<digit->samples()[i] )
		{
		  m_params->curr_maxsample=i;
		  m_params->curr_maximum= digit->samples()[i];
		}
	    }
	}
      
      //Invoke all LArRawChannelBuilderTools until one returns success
      // indicated by a int=zero return code. The return code contains
      // the error if non-zero
      builderToolVector::iterator it = itStart;
      
      // retrieve pedestal and get the ADC2E factors.
      //  The ramps are stored in the data member m_ramps
      float pedestal = this->pedestal(pLog);
      this->ADC2energy(pLog);
      
      while( !((*it)->buildRawChannel(digit, pedestal, m_ramps, pLog)) && ++it != itEnd )
	if(bool(pLog))
	  (*pLog) << MSG::DEBUG << "One LArRawChannelBuilderTool failed" << endmsg;
      // Keep the last calculated values
       energy = (*it)->energy();
	 time   = (*it)->time();
	 gain   = (*it)->gain(); 
	 
      // (*pLog) << MSG::VERBOSE << "done building LArRawChannel" << endmsg;
      m_params->curr_id=0;

      /*
	#ifndef NDEBUG
	if(bool(pLog))
	(*pLog) << MSG::DEBUG << "Finished computing energy : " 
	<< energy << " time : " << time << endmsg;
	#endif
      */
      return true;
      
}

void LArCellBuilderDriver::finishEventTools(){
  
  builderToolVector::iterator itStart = m_buildTools.begin();
  builderToolVector::iterator itEnd   = m_buildTools.end();
  // reset error counter per event
  for( builderToolVector::iterator it = itStart;
       it != itEnd; it++ ) (*it)->finalEventHidden();
  
  pedestalToolVector::iterator pitStart = m_pedestalTools.begin();
  pedestalToolVector::iterator pitEnd   = m_pedestalTools.end();
  
  for( pedestalToolVector::iterator it = pitStart;
       it != pitEnd; it++ ) (*it)->finalEventHidden();
  
  adc2eToolVector::iterator citStart = m_adc2eTools.begin();
  adc2eToolVector::iterator citEnd   = m_adc2eTools.end();
  
  for( adc2eToolVector::iterator it = citStart;
       it != citEnd; it++ ) (*it)->finalEventHidden();
  
}

float LArCellBuilderDriver::pedestal(MsgStream* pLog)
{
  // did we retrieved pedestals for this channel before ?
  if(m_oldIdentifier==m_params->curr_chid)
    return m_oldPedestal;
  
  pedestalToolVector::iterator it    = m_pedestalTools.begin();
  pedestalToolVector::iterator itEnd = m_pedestalTools.end();
  
  while( !(*it)->pedestal(m_oldPedestal, pLog) && ++it != itEnd )
    if(bool(pLog))
      (*pLog) << MSG::DEBUG << "One PedestalTool failed" << endmsg;
  
  // remember for which channel we retrieved the pedestal
  m_oldIdentifier=m_params->curr_chid;
  
  return m_oldPedestal;
}

void LArCellBuilderDriver::ADC2energy(MsgStream* pLog)
{
  adc2eToolVector::iterator it    = m_adc2eTools.begin();
  adc2eToolVector::iterator itEnd = m_adc2eTools.end();
  
  while( !(*it)->ADC2E(m_ramps, pLog) && ++it != itEnd )
    if(bool(pLog))
      (*pLog) << MSG::DEBUG << "One ADC2Energy Tool failed" << endmsg;
  
  return;
}

StatusCode LArCellBuilderDriver::finalize()
{
  ATH_MSG_INFO( "LArCellBuilderdriver finalize."  );
  ATH_MSG_INFO( "  Build Tools:"  );
  for( builderToolVector::iterator it = m_buildTools.begin();
       it != m_buildTools.end(); it++ ) (*it)->printSummary();
  
  ATH_MSG_INFO( "  ADC2Energy Tools:"  );
  for( adc2eToolVector::iterator it = m_adc2eTools.begin();
       it != m_adc2eTools.end(); it++ ) (*it)->printSummary();
  
  ATH_MSG_INFO( "  Pedestal Tools:"  );
  for( pedestalToolVector::iterator it = m_pedestalTools.begin();
       it != m_pedestalTools.end(); it++ ) (*it)->printSummary();

  delete m_params;
  
  return StatusCode::SUCCESS;
}
