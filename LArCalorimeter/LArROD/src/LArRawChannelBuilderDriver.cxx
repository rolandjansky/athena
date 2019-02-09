/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderDriver.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "Identifier/Identifier.h"

#include "CLHEP/Units/SystemOfUnits.h"

LArRawChannelBuilderDriver::LArRawChannelBuilderDriver(const std::string& name,
						       ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_params(0),
  m_onlineHelper(0),
  m_DataLocation("FREE"),
  m_ChannelContainerName("LArRawChannels"),
  m_buildTools(this),
  m_adc2eTools(this),
  m_pedestalTools(this),
  m_oldPedestal(0)
{
  declareProperty("LArRawChannelContainerName",   m_ChannelContainerName);
  declareProperty("DataLocation",                 m_DataLocation );
  declareProperty("BuilderTools",                 m_buildTools);
  declareProperty("ADCtoEnergyTools",             m_adc2eTools);
  declareProperty("PedestalTools",                m_pedestalTools);
  declareProperty("checkSamples",                 m_checkSamples=32);
  declareProperty("BuildDiscChannel",             m_buildDiscChannel=false);
  declareProperty("DefaultShiftTimeSample",       m_defaultShiftTimeSamples=0);
}

StatusCode LArRawChannelBuilderDriver::initialize()
{
  // initialize parameters
  m_params = new LArRawChannelBuilderParams;
  m_params->m_larRawChannelContainer = 0;
  
  if (detStore()->retrieve(m_onlineHelper, "LArOnlineID").isFailure()){
    msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  
  ATH_CHECK( m_cablingKey.initialize() );

  if ( m_buildTools.retrieve().isFailure() ){
    msg(MSG::ERROR) << "Unable to find Builder Tools " << m_buildTools << endmsg;
    return StatusCode::FAILURE; 
  }else{
    ATH_MSG_INFO("Successfully retrieved Builder Tools " << m_buildTools);
  }
  for( builderToolVector::iterator it = m_buildTools.begin(); it != m_buildTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      msg(MSG::ERROR) << "Unable to initialize Builder Tool " << (*it)->name() << endmsg;
  
  if ( m_adc2eTools.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Unable to find ADC2E Tools " << m_buildTools << endmsg;
      return StatusCode::FAILURE; 
  }else{
    ATH_MSG_INFO("Successfully retrieved ADC2E Tools " << m_buildTools);
  }

  for( adc2eToolVector::iterator it = m_adc2eTools.begin(); it != m_adc2eTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      msg(MSG::ERROR) << "Unable to initialize ADC2E Tool " << (*it)->name() << endmsg;
  
  if ( m_pedestalTools.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Unable to find Pedestal Tools " << m_buildTools << endmsg;
      return StatusCode::FAILURE; 
    }else{
    ATH_MSG_INFO("Successfully retrieved Pedestal Tools " << m_buildTools);
  }
  for( pedestalToolVector::iterator it = m_pedestalTools.begin(); it != m_pedestalTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      msg(MSG::ERROR) << "Unable to initialize Pedestal Tool " << (*it)->name() << endmsg;
  
  // check that we have tools to run the reconstruction !
  if( m_buildTools.size() == 0 ){
      msg(MSG::ERROR) << "Didn't find any BuilderTools to do reconstruction !" << endmsg;
      return(StatusCode::FAILURE);  
    }
  if( m_adc2eTools.size() == 0 ){
      msg(MSG::ERROR) << "Didn't find and ADC2ETools to do reconstruction !" << endmsg;
      return(StatusCode::FAILURE);  
    }
  if( m_pedestalTools.size() == 0 ){
    msg(MSG::ERROR) << "Didn't find and PedestalTools to do reconstruction !" << endmsg;
    return(StatusCode::FAILURE);  
  }
  return StatusCode::SUCCESS;
}

StatusCode LArRawChannelBuilderDriver::execute()
{
  ATH_MSG_DEBUG("In execute");

  //Pointer to input data container
  const LArDigitContainer* digitContainer=0;
  
  //Retrieve Digit Container
  ATH_MSG_DEBUG("About to retrieve LArDigitContainer with key " << m_DataLocation);
  
  StatusCode sc=evtStore()->retrieve(digitContainer,m_DataLocation);
  if(sc.isFailure()) {
    ATH_MSG_WARNING("Can't retrieve LArDigitContainer with key " << m_DataLocation << "from StoreGate.");
    return StatusCode::SUCCESS;
  }

  m_params->m_larRawChannelContainer = new LArRawChannelContainer();
  m_params->m_larRawChannelContainer->reserve(digitContainer->size());
  // Put this LArRawChannel container in the transient store
  sc = evtStore()->record(m_params->m_larRawChannelContainer, m_ChannelContainerName);
  if(sc.isFailure()) {
    ATH_MSG_WARNING("Can't record LArRawChannelContainer in StoreGate");
    return StatusCode::FAILURE;
  }
  

  ATH_MSG_VERBOSE("1) LArDigitContainer container size = " <<  digitContainer->size());
  
  if( digitContainer->size() < 1 ) {
    msg(MSG::INFO) << "Empty LArDigitContainer container." << endmsg;
    return StatusCode::SUCCESS;
  }
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  // declare all variables first ...
  builderToolVector::iterator itStart = m_buildTools.begin();
  builderToolVector::iterator itEnd   = m_buildTools.end();
  
  for( builderToolVector::iterator it = itStart; it != itEnd; it++ )
    (*it)->initEventHidden();
  
  ATH_MSG_DEBUG("Start to build LArRawChannels");
  
  // reset to zero
  m_oldIdentifier=HWIdentifier(0);
  
  for (LArDigitContainer::const_iterator cont_it=digitContainer->begin();
       cont_it!=digitContainer->end(); cont_it++)
    {
      ATH_MSG_VERBOSE("building LArRawChannel");
      m_params->curr_chid=(*cont_it)->channelID();
      m_params->curr_gain=(*cont_it)->gain();

      if (m_params->curr_gain >= CaloGain::LARNGAIN || m_params->curr_gain<0) {
	// write ERROR message, could change to warning if needed
	msg(MSG::WARNING) << " Corrupted data found : gain = " << m_params->curr_gain << endmsg;
	m_params->curr_gain = CaloGain::UNKNOWNGAIN;
      }
      
      if(!m_buildDiscChannel && !cabling->isOnlineConnected(m_params->curr_chid))
	continue;
      
      m_params->curr_sample0   = (*cont_it)->samples()[0];
      m_params->curr_maximum   = m_params->curr_sample0;
      m_params->curr_maxsample = 0;
      m_params->curr_nsamples  = m_checkSamples < (*cont_it)->nsamples() ?
	m_checkSamples : (*cont_it)->nsamples();
      m_params->curr_shiftTimeSamples = m_defaultShiftTimeSamples;
      
      m_params->qualityBitPattern = 0;
      
      if(m_params->curr_nsamples>0)
	{
	  for( unsigned int i=0; i<m_params->curr_nsamples; i++ )
	    {
	      if( m_params->curr_maximum<(*cont_it)->samples()[i] )
		{
		  m_params->curr_maxsample=i;
		  m_params->curr_maximum= (*cont_it)->samples()[i];
		}
	    }
	}
      //ATH_MSG_VERBOSE("now building LArRawChannel");
      
      //Invoke all LArRawChannelBuilderTools until one returns success
      // indicated by a int=zero return code. The return code contains
      // the error if non-zero
      builderToolVector::iterator it = itStart;
      
      // retrieve pedestal and get the ADC2E factors.
      //  The ramps are stored in the data member m_ramps
      const float pedestal = this->pedestal();
      this->ADC2energy();
      
      while( !((*it)->buildRawChannel(*cont_it, pedestal, m_ramps, &msg())) && ++it != itEnd )
	ATH_MSG_DEBUG("One LArRawChannelBuilderTool failed");
      
      //ATH_MSG_VERBOSE("done building LArRawChannel")
      m_params->curr_id=0;
    }
  
  ATH_MSG_DEBUG("Finished building LArRawChannels");
  
  //Organize Collections  
  //  sortChannels(m_larRawChannelContainer);
  //m_log << MSG::DEBUG << "sorted RawChannelContainer, now lock it " << endmsg;
  // lock raw channel container
  sc = evtStore()->setConst(m_params->m_larRawChannelContainer);
  if (sc.isFailure()) {
    msg(MSG::WARNING) << " Cannot lock RawChannel Container " << endmsg;
    // return(StatusCode::FAILURE);
  }
  

  // reset error counter per event
  for( builderToolVector::iterator it = itStart; it != itEnd; it++ )
    (*it)->finalEventHidden();
  
  pedestalToolVector::iterator pitStart = m_pedestalTools.begin();
  pedestalToolVector::iterator pitEnd   = m_pedestalTools.end();
  for( pedestalToolVector::iterator it = pitStart; it != pitEnd; it++ )
    (*it)->finalEventHidden();
  
  adc2eToolVector::iterator citStart = m_adc2eTools.begin();
  adc2eToolVector::iterator citEnd   = m_adc2eTools.end();
  for( adc2eToolVector::iterator it = citStart; it != citEnd; it++ )
    (*it)->finalEventHidden();
  
  return StatusCode::SUCCESS;
}

float LArRawChannelBuilderDriver::pedestal()
{
  // did we retrieved pedestals for this channel before ?
  if(m_oldIdentifier==m_params->curr_chid)
    return m_oldPedestal;
  
  pedestalToolVector::iterator it    = m_pedestalTools.begin();
  pedestalToolVector::iterator itEnd = m_pedestalTools.end();
  
  while( !(*it)->pedestal(m_oldPedestal, &msg()) && ++it != itEnd )
    ATH_MSG_DEBUG("One PedestalTool failed");
  
  // remember for which channel we retrieved the pedestal
  m_oldIdentifier=m_params->curr_chid;
  
  return m_oldPedestal;
}

void LArRawChannelBuilderDriver::ADC2energy()
{
  adc2eToolVector::iterator it    = m_adc2eTools.begin();
  adc2eToolVector::iterator itEnd = m_adc2eTools.end();
  
  while( !(*it)->ADC2E(m_ramps, &msg()) && ++it != itEnd )
    ATH_MSG_DEBUG("One ADC2Energy Tool failed");
  
  return;
}

StatusCode LArRawChannelBuilderDriver::finalize()
{
  msg(MSG::INFO) << "LArRawChannelBuilderDriver finalize." << endmsg;
  msg( MSG::INFO) << "  Build Tools:" << endmsg;
  for( builderToolVector::iterator it = m_buildTools.begin();
       it != m_buildTools.end(); it++ ) (*it)->printSummary();
  
  msg(MSG::INFO) << "  ADC2Energy Tools:" << endmsg;
  for( adc2eToolVector::iterator it = m_adc2eTools.begin();
       it != m_adc2eTools.end(); it++ ) (*it)->printSummary();
  
  msg(MSG::INFO) << "  Pedestal Tools:" << endmsg;
  for( pedestalToolVector::iterator it = m_pedestalTools.begin();
       it != m_pedestalTools.end(); it++ ) (*it)->printSummary();

  delete m_params;
  
  return StatusCode::SUCCESS;
}
