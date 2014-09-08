/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArSuperCellBuilderDriver.h"
#include "LArROD/LArRawChannelBuilderToolBase.h"
#include "LArROD/LArRawChannelBuilderADC2EToolBase.h"
#include "LArROD/LArRawChannelBuilderPedestalToolBase.h"

#include "LArRecUtils/LArParabolaPeakRecoTool.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "Identifier/Identifier.h"
#include "LArTools/LArSuperCellCablingTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArCOOLConditions/LArDAC2uASC.h"
#include "LArCOOLConditions/LAruA2MeVSC.h"
#include "LArCOOLConditions/LArRampSC.h"

LArSuperCellBuilderDriver::LArSuperCellBuilderDriver (const std::string& name,
                                          ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_params (0),
  m_storeGateSvc(NULL),
  m_detStore(NULL),
  m_toolSvc(NULL),
  m_onlineHelper(0),
  m_DataLocation("FREE"),
  m_ChannelContainerName("LArRawChannels"),
  m_buildTools(),
  m_adc2eTools(),
  m_pedestalTools(),
  m_oldPedestal(0.),
  m_larCablingSvc("LArCablingService")
{
  declareProperty("LArRawChannelContainerName",   m_ChannelContainerName);
  declareProperty("DataLocation",                 m_DataLocation );
  declareProperty("BuilderTools",                 m_buildTools);
  declareProperty("ADCtoEnergyTools",             m_adc2eTools);
  declareProperty("PedestalTools",                m_pedestalTools);
  declareProperty("checkSamples",                 m_checkSamples=32);
  declareProperty("BuildDiscChannel",             m_buildDiscChannel=false);
  declareProperty("DefaultShiftTimeSample",       m_defaultShiftTimeSamples=0);
  declareProperty("BCID_Iter",                    m_bcs=2);
}

LArSuperCellBuilderDriver::~LArSuperCellBuilderDriver()
{ }

StatusCode LArSuperCellBuilderDriver::initialize()
{
  MsgStream m_log(msgSvc(), name());
  
  // initialize parameters
  m_params = new LArRawChannelBuilderParams;
  m_params->m_larRawChannelContainer = NULL;
  
  //StatusCode sc;
  
  if (this->retrieveDetectorStore(m_onlineHelper, "LArOnlineID").isFailure())
    {
      m_log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
      return StatusCode::FAILURE;
    }
  // if (m_roiMap.retrieve().isFailure())
//     {
//       m_log << MSG::ERROR << "Unable to find tool LArRoI_Map" << endreq;
//       return StatusCode::FAILURE; 
//     }
  
  if(m_larCablingSvc.retrieve().isFailure())
    {
      m_log << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
      return StatusCode::FAILURE;
    }
  
  // ***
  //m_larRawOrdering.setMap(&(*m_roiMap)); 
  
  if ( m_buildTools.retrieve().isFailure() )
    {
      m_log << MSG::ERROR << "Unable to find Builder Tools " << m_buildTools << endreq;
      return StatusCode::FAILURE; 
    }else{
      m_log << MSG::INFO << "Successfully retrieved Builder Tools " << m_buildTools << endreq;
    }
  for( builderToolVector::iterator it = m_buildTools.begin(); it != m_buildTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      m_log << MSG::ERROR << "Unable to initialize Builder Tool " << (*it)->name() << endreq;
  
  if ( m_adc2eTools.retrieve().isFailure() )
    {
      m_log << MSG::ERROR << "Unable to find ADC2E Tools " << m_buildTools << endreq;
      return StatusCode::FAILURE; 
    }else{
      m_log << MSG::INFO << "Successfully retrieved ADC2E Tools " << m_buildTools << endreq;
    }
  for( adc2eToolVector::iterator it = m_adc2eTools.begin(); it != m_adc2eTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      m_log << MSG::ERROR << "Unable to initialize ADC2E Tool " << (*it)->name() << endreq;
  
  if ( m_pedestalTools.retrieve().isFailure() )
    {
      m_log << MSG::ERROR << "Unable to find Pedestal Tools " << m_buildTools << endreq;
      return StatusCode::FAILURE; 
    }else{
      m_log << MSG::INFO << "Successfully retrieved Pedestal Tools " << m_buildTools << endreq;
    }
  for( pedestalToolVector::iterator it = m_pedestalTools.begin(); it != m_pedestalTools.end(); it++ )
    if( ( (*it)->initToolHidden(m_params) ).isFailure() )
      m_log << MSG::ERROR << "Unable to initialize Pedestal Tool " << (*it)->name() << endreq;
  
  // check that we have tools to run the reconstruction !
  if( m_buildTools.size() == 0 )
    {
      m_log << MSG::ERROR << "Didn't find any BuilderTools to do reconstruction !" << endreq;
      return(StatusCode::FAILURE);  
    }
  if( m_adc2eTools.size() == 0 )
    {
      m_log << MSG::ERROR << "Didn't find and ADC2ETools to do reconstruction !" << endreq;
      return(StatusCode::FAILURE);  
    }
  if( m_pedestalTools.size() == 0 )
    {
      m_log << MSG::ERROR << "Didn't find and PedestalTools to do reconstruction !" << endreq;
      return(StatusCode::FAILURE);  
    }
  return StatusCode::SUCCESS;
}

void LArSuperCellBuilderDriver::initEventTools(){
  // declare all variables first ...
  builderToolVector::iterator itStart = m_buildTools.begin();
  builderToolVector::iterator itEnd   = m_buildTools.end();
  
  for( builderToolVector::iterator it = itStart;
       it != itEnd; it++ ) (*it)->initEventHidden();
  
  // reset to zero
  m_oldIdentifier=HWIdentifier(0);
}

StatusCode LArSuperCellBuilderDriver::execute() {

  MsgStream msg(msgSvc(), name());

  msg << MSG::DEBUG << "In execute" << endreq;

  //Pointer to input data container
  const LArDigitContainer* digitContainer=0;
  
  //Retrieve Digit Container
  msg << MSG::DEBUG << "About to retrieve LArDigitContainer with key " << m_DataLocation << endreq;
  
  StatusCode sc=evtStore()->retrieve(digitContainer,m_DataLocation);
  if(sc.isFailure()) {
    msg<< (MSG::WARNING) << "Can't retrieve LArDigitContainer with key " << m_DataLocation << "from StoreGate." << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Put this LArRawChannel container in the transient store
  CaloCellContainer* caloCell= new CaloCellContainer();
  sc = evtStore()->record(caloCell, "SCell");
  if(sc.isFailure()) {
      msg<< (MSG::WARNING) << "Can't record LArRawChannelContainer in StoreGate" << endreq;
      return StatusCode::FAILURE;
  }

  msg<<(MSG::VERBOSE) <<  "LArDigitContainer container size = " <<  digitContainer->size() << endreq;
  //
  if( digitContainer->size() < 1 ) {
        msg<<(MSG::INFO) << "Empty LArDigitContainer container." << endreq;
        return StatusCode::SUCCESS;
  }

  ToolHandle<LArSuperCellCablingTool> cabling = 0;
  const CaloSuperCellDetDescrManager* m_sem_mgr;
  CHECK( cabling.retrieve() );
  CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );

  initEventTools();

  for (LArDigitContainer::const_iterator cont_it=digitContainer->begin();
       cont_it!=digitContainer->end(); cont_it++){
       int energy=0;
       int time=0;
       int prov=0;
       CaloGain::CaloGain gain;
       if ( buildLArCell( (*cont_it), energy, time, gain, prov, &msg ) ){
	   Identifier id = cabling->cnvToIdentifier((*cont_it)->channelID());
	   IdentifierHash idhash = m_sem_mgr->getCaloCell_ID()->calo_cell_hash(id);
	   const CaloDetDescrElement* dde = m_sem_mgr->get_element (idhash);
           CaloCell* cell = new CaloCell( dde, (float)energy, (float)time, (uint16_t)0, (uint16_t)prov, gain );
	   caloCell->push_back( cell );
       }

  } // end of digitContainer loop


  return StatusCode::SUCCESS;
}

bool LArSuperCellBuilderDriver::buildLArCell(const LArDigit* digit,
					int&  energy ,
					int&  time ,
					CaloGain::CaloGain&  gain ,
                                        int& prov,
					MsgStream* pLog)
{
  /* #ifndef NDEBUG
     if(bool(pLog))
     (*pLog) << MSG::DEBUG << "In buildLArCell" << endreq;
     if(bool(pLog))
     (*pLog) << MSG::DEBUG << "chid : " << digit->channelID() << endreq;
     if(bool(pLog))
     (*pLog) << MSG::DEBUG << "gain : " << digit->gain() << endreq;
     #endif */
  
  //StatusCode sc;
  
  // declare all variables first ...
  builderToolVector::iterator itStart = m_buildTools.begin();
  builderToolVector::iterator itEnd   = m_buildTools.end();
  
      m_params->curr_chid=digit->channelID();
      m_params->curr_gain=digit->gain();
      
      if(!m_buildDiscChannel && !m_larCablingSvc->isOnlineConnected(m_params->curr_chid))
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
      
      // retrieve pedestal and get the ADC2E factors.
      //  The ramps are stored in the data member m_ramps
      
      float pedestal = this->pedestal(pLog);
      this->ADC2energy(pLog);

      const LArDigit* digit_from_cont = digit;
      HWIdentifier hwid = digit_from_cont->hardwareID();
      CaloGain::CaloGain g = digit_from_cont->gain();

      std::vector<short> samples;
      std::vector<int> energies(m_bcs+1,0);
      std::vector<int> times(m_bcs+1,0);
      energy=0; time=0; gain=(CaloGain::CaloGain)0;
      for(int bcid = 0 ; bcid <= m_bcs  ; ++ bcid ) {
        samples.clear();
        samples.insert(samples.end(), digit_from_cont->samples().begin()+bcid, digit_from_cont->samples().end()-m_bcs+bcid );
        LArDigit* digit_go = new LArDigit(hwid, g, samples );
      
        builderToolVector::iterator it = itStart;
        bool ok=true;
        while( !(ok=(*it)->buildRawChannel(digit_go, pedestal, m_ramps, pLog)) && ++it != itEnd )
	   if(bool(pLog))
	     (*pLog) << MSG::DEBUG << "One LArRawChannelBuilderTool failed" << endreq;
        // Keep the last calculated values
        if ( ok ) {
           energies[bcid] = (*it)->energy();
	   times[bcid]   = (*it)->time();
	   gain   = (*it)->gain(); 
         }
         delete digit_go;
      } // bcid loop
      // BCID max
      int middle_point=m_bcs/2;
      bool passBCIDmax=true;
      bool passBCIDTime=true;
      for(int bcid = 0 ; bcid <= m_bcs  ; ++ bcid ) {
	if ( (bcid!=middle_point) && (energies[bcid]>energies[middle_point]) )
		passBCIDmax=false;
        if ( (bcid!=middle_point) && (times[bcid]!=0) && (fabsf(times[bcid])<13e3) ) // If within bad BC, bad
		passBCIDTime=false;
      }
      energy=energies[middle_point];
      time  =times[middle_point]/1e3;
      if ( passBCIDTime ) {
	if ( fabsf(time)<25 ) passBCIDTime=true;
	else passBCIDTime=false;
      }
	
      if ( passBCIDmax  ) prov |= 0x40 ;
      if ( passBCIDTime ) prov |= 0x200 ;
      prov |= 0x2000;
	 
      // (*pLog) << MSG::VERBOSE << "done building LArRawChannel" << endreq;
      m_params->curr_id=0;

      /*
	#ifndef NDEBUG
	if(bool(pLog))
	(*pLog) << MSG::DEBUG << "Finished computing energy : " 
	<< energy << " time : " << time << endreq;
	#endif
      */
      return true;
      
}

void LArSuperCellBuilderDriver::finishEventTools(){
  
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

float LArSuperCellBuilderDriver::pedestal(MsgStream* pLog)
{
  // did we retrieved pedestals for this channel before ?
  if(m_oldIdentifier==m_params->curr_chid)
    return m_oldPedestal;
  
  pedestalToolVector::iterator it    = m_pedestalTools.begin();
  pedestalToolVector::iterator itEnd = m_pedestalTools.end();
  
  while( !(*it)->pedestal(m_oldPedestal, pLog) && ++it != itEnd )
    if(bool(pLog))
      (*pLog) << MSG::DEBUG << "One PedestalTool failed" << endreq;
  
  // remember for which channel we retrieved the pedestal
  m_oldIdentifier=m_params->curr_chid;
  
  return m_oldPedestal;
}

void LArSuperCellBuilderDriver::ADC2energy(MsgStream* pLog)
{

  m_ramps.clear();
  m_ramps.resize(2,0.0);

  adc2eToolVector::iterator it    = m_adc2eTools.begin();
  adc2eToolVector::iterator itEnd = m_adc2eTools.end();
  
  while( !(*it)->ADC2E(m_ramps, pLog) && ++it != itEnd )
    if(bool(pLog))
      (*pLog) << MSG::DEBUG << "One ADC2Energy Tool failed" << endreq;
  
  return;
}

StatusCode LArSuperCellBuilderDriver::finalize()
{
  MsgStream m_log(msgSvc(), name());
  
  m_log << MSG::INFO << "LArSuperCellBuilderdriver finalize." << endreq;
  m_log << MSG::INFO << "  Build Tools:" << endreq;
  for( builderToolVector::iterator it = m_buildTools.begin();
       it != m_buildTools.end(); it++ ) (*it)->printSummary();
  
  m_log << MSG::INFO << "  ADC2Energy Tools:" << endreq;
  for( adc2eToolVector::iterator it = m_adc2eTools.begin();
       it != m_adc2eTools.end(); it++ ) (*it)->printSummary();
  
  m_log << MSG::INFO << "  Pedestal Tools:" << endreq;
  for( pedestalToolVector::iterator it = m_pedestalTools.begin();
       it != m_pedestalTools.end(); it++ ) (*it)->printSummary();

  delete m_params;
  
  return StatusCode::SUCCESS;
}
