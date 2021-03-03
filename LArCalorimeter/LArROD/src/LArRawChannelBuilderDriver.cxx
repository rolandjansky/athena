/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderDriver.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "Identifier/Identifier.h"

#include "CLHEP/Units/SystemOfUnits.h"

LArRawChannelBuilderDriver::LArRawChannelBuilderDriver(const std::string& name,
						       ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
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
  m_params.m_larRawChannelContainer = 0;
  
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_CHECK( m_cablingKey.initialize() );

  ATH_CHECK( m_buildTools.retrieve() );
  ATH_MSG_INFO("Successfully retrieved Builder Tools " << m_buildTools);
  for (ToolHandle<ILArRawChannelBuilderToolBase>& tool : m_buildTools)
    if( ( tool->initToolHidden(&m_params) ).isFailure() )
      ATH_MSG_ERROR( "Unable to initialize Builder Tool " << tool->name()  );
  
  ATH_CHECK( m_adc2eTools.retrieve() );
  ATH_MSG_INFO("Successfully retrieved ADC2E Tools " << m_adc2eTools);
  for (ToolHandle<ILArRawChannelBuilderADC2EToolBase>& tool : m_adc2eTools)
    if( ( tool->initToolHidden(&m_params) ).isFailure() )
      ATH_MSG_ERROR( "Unable to initialize ADC2E Tool " << tool->name()  );
  
  ATH_CHECK( m_pedestalTools.retrieve() );
  ATH_MSG_INFO("Successfully retrieved Pedestal Tools " << m_pedestalTools);
  for (ToolHandle<ILArRawChannelBuilderPedestalToolBase>& tool : m_pedestalTools)
    if( ( tool->initToolHidden(&m_params) ).isFailure() )
      ATH_MSG_ERROR( "Unable to initialize Pedestal Tool " << tool->name()  );
  
  // check that we have tools to run the reconstruction !
  if( m_buildTools.size() == 0 ){
      ATH_MSG_ERROR( "Didn't find any BuilderTools to do reconstruction !"  );
      return(StatusCode::FAILURE);  
    }
  if( m_adc2eTools.size() == 0 ){
      ATH_MSG_ERROR( "Didn't find and ADC2ETools to do reconstruction !"  );
      return(StatusCode::FAILURE);  
    }
  if( m_pedestalTools.size() == 0 ){
      ATH_MSG_ERROR( "Didn't find and PedestalTools to do reconstruction !"  );
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
  
  if (evtStore()->retrieve(digitContainer,m_DataLocation).isFailure()) {
    ATH_MSG_WARNING("Can't retrieve LArDigitContainer with key " << m_DataLocation << "from StoreGate.");
    return StatusCode::SUCCESS;
  }

  m_params.m_larRawChannelContainer = new LArRawChannelContainer();
  m_params.m_larRawChannelContainer->reserve(digitContainer->size());
  // Put this LArRawChannel container in the transient store
  ATH_CHECK( evtStore()->record(m_params.m_larRawChannelContainer, m_ChannelContainerName) );
  

  ATH_MSG_VERBOSE("1) LArDigitContainer container size = " <<  digitContainer->size());
  
  if( digitContainer->size() < 1 ) {
    ATH_MSG_INFO( "Empty LArDigitContainer container."  );
    return StatusCode::SUCCESS;
  }
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  // declare all variables first ...
  for (ToolHandle<ILArRawChannelBuilderToolBase>& tool : m_buildTools)
    tool->initEventHidden();
  
  ATH_MSG_DEBUG("Start to build LArRawChannels");
  
  // reset to zero
  m_oldIdentifier=HWIdentifier(0);

  for (const LArDigit* digit : *digitContainer)
    {
      ATH_MSG_VERBOSE("building LArRawChannel");
      m_params.curr_chid=digit->channelID();
      m_params.curr_gain=digit->gain();

      if (m_params.curr_gain >= CaloGain::LARNGAIN || m_params.curr_gain<0) {
	// write ERROR message, could change to warning if needed
	ATH_MSG_WARNING( " Corrupted data found : gain = " << m_params.curr_gain  );
	m_params.curr_gain = CaloGain::UNKNOWNGAIN;
      }
      
      if(!m_buildDiscChannel && !cabling->isOnlineConnected(m_params.curr_chid))
	continue;
      
      m_params.curr_sample0   = digit->samples()[0];
      m_params.curr_maximum   = m_params.curr_sample0;
      m_params.curr_maxsample = 0;
      m_params.curr_nsamples  = m_checkSamples < digit->nsamples() ?
	m_checkSamples : digit->nsamples();
      m_params.curr_shiftTimeSamples = m_defaultShiftTimeSamples;
      
      m_params.qualityBitPattern = 0;
      
      if(m_params.curr_nsamples>0)
	{
	  for( unsigned int i=0; i<m_params.curr_nsamples; i++ )
	    {
	      if( m_params.curr_maximum<digit->samples()[i] )
		{
		  m_params.curr_maxsample=i;
		  m_params.curr_maximum= digit->samples()[i];
		}
	    }
	}
      //ATH_MSG_VERBOSE("now building LArRawChannel");
      
      //Invoke all LArRawChannelBuilderTools until one returns success
      // indicated by a int=zero return code. The return code contains
      // the error if non-zero
      
      // retrieve pedestal and get the ADC2E factors.
      //  The ramps are stored in the data member m_ramps
      const float pedestal = this->pedestal();
      this->ADC2energy();
      
      for (ToolHandle<ILArRawChannelBuilderToolBase>& tool : m_buildTools) {
        if (tool->buildRawChannel(digit, pedestal, m_ramps, &msg())) break;
	ATH_MSG_DEBUG("One LArRawChannelBuilderTool failed");
      }
      
      //ATH_MSG_VERBOSE("done building LArRawChannel")
      m_params.curr_id=0;
    }
  
  ATH_MSG_DEBUG("Finished building LArRawChannels");
  
  //Organize Collections  
  //  sortChannels(m_larRawChannelContainer);
  //ATH_MSG_DEBUG( "sorted RawChannelContainer, now lock it "  );
  // lock raw channel container
  if ( evtStore()->setConst(m_params.m_larRawChannelContainer).isFailure() ) {
    ATH_MSG_WARNING( " Cannot lock RawChannel Container "  );
    // return(StatusCode::FAILURE);
  }
  

  // reset error counter per event
  for (ToolHandle<ILArRawChannelBuilderToolBase>& tool : m_buildTools)
    tool->finalEventHidden();
  
  for (ToolHandle<ILArRawChannelBuilderPedestalToolBase>& tool : m_pedestalTools)
    tool->finalEventHidden();
  
  for (ToolHandle<ILArRawChannelBuilderADC2EToolBase>& tool : m_adc2eTools)
    tool->finalEventHidden();
  
  return StatusCode::SUCCESS;
}

float LArRawChannelBuilderDriver::pedestal()
{
  // did we retrieved pedestals for this channel before ?
  if(m_oldIdentifier==m_params.curr_chid)
    return m_oldPedestal;
  
  pedestalToolVector::iterator it    = m_pedestalTools.begin();
  pedestalToolVector::iterator itEnd = m_pedestalTools.end();
  
  while( !(*it)->pedestal(m_oldPedestal, &msg()) && ++it != itEnd )
    ATH_MSG_DEBUG("One PedestalTool failed");
  
  // remember for which channel we retrieved the pedestal
  m_oldIdentifier=m_params.curr_chid;
  
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
  ATH_MSG_INFO( "LArRawChannelBuilderDriver finalize."  );
  ATH_MSG_INFO( "  Build Tools:"  );
  for (ToolHandle<ILArRawChannelBuilderToolBase>& tool : m_buildTools)
    tool->printSummary();
  
  ATH_MSG_INFO( "  ADC2Energy Tools:"  );
  for (ToolHandle<ILArRawChannelBuilderADC2EToolBase>& tool : m_adc2eTools)
    tool->printSummary();
  
  ATH_MSG_INFO( "  Pedestal Tools:"  );
  for (ToolHandle<ILArRawChannelBuilderPedestalToolBase>& tool : m_pedestalTools)
    tool->printSummary();

  return StatusCode::SUCCESS;
}
