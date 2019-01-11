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
#include "LArCabling/LArSuperCellCablingTool.h"
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
  m_larCablingSvc("LArSuperCellCablingTool"),
  m_counter(0)
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
  declareProperty("bCIDLowLim",                   m_bcidLowLim={8,-2,-8});
  declareProperty("bCIDUpLim",                    m_bcidUpLim={-8,2,8});
  declareProperty("bCIDbands",                    m_bcidBands={-1e4,0,10,1e4});
}

LArSuperCellBuilderDriver::~LArSuperCellBuilderDriver()
{ }

StatusCode LArSuperCellBuilderDriver::initialize()
{
  // initialize parameters
  m_params = new LArRawChannelBuilderParams;
  m_params->m_larRawChannelContainer = NULL;
  
  //StatusCode sc;
  
  if (this->retrieveDetectorStore(m_onlineHelper, "LArOnline_SuperCellID").isFailure())
    {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !"  );
      return StatusCode::FAILURE;
    }
  // if (m_roiMap.retrieve().isFailure())
//     {
//       ATH_MSG_ERROR( "Unable to find tool LArRoI_Map"  );
//       return StatusCode::FAILURE; 
//     }
  
  if(m_larCablingSvc.retrieve().isFailure())
    {
      ATH_MSG_ERROR( "Could not retrieve LArCablingService Tool"  );
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

  //Pointer to input data container
  const LArDigitContainer* digitContainer=0;
  
  //Retrieve Digit Container
  ATH_MSG_DEBUG( "About to retrieve LArDigitContainer with key " << m_DataLocation  );
  
  ATH_CHECK( evtStore()->retrieve(digitContainer,m_DataLocation) );
  
  // Put this LArRawChannel container in the transient store
  CaloCellContainer* caloCell= new CaloCellContainer();
  ATH_CHECK( evtStore()->record(caloCell, "SCell") );

  ATH_MSG_VERBOSE(  "LArDigitContainer container size = " <<  digitContainer->size()  );
  //
  if( digitContainer->size() < 1 ) {
        ATH_MSG_INFO( "Empty LArDigitContainer container."  );
        return StatusCode::SUCCESS;
  }

  ToolHandle<LArSuperCellCablingTool> cabling;
  const CaloSuperCellDetDescrManager* sem_mgr;
  CHECK( cabling.retrieve() );
  CHECK( detStore()->retrieve (sem_mgr, "CaloSuperCellMgr") );

  initEventTools();

  int ii=0;
  m_counter = 0;
  for (LArDigitContainer::const_iterator cont_it=digitContainer->begin();
       cont_it!=digitContainer->end(); cont_it++){
       int energy=0;
       int time=0;
       int prov=0;
       CaloGain::CaloGain gain;
       if ( buildLArCell( (*cont_it), energy, time, gain, prov, &msg() ) ){
	   ii++;
	   Identifier id = cabling->cnvToIdentifier((*cont_it)->channelID());
	   IdentifierHash idhash = sem_mgr->getCaloCell_ID()->calo_cell_hash(id);
	   const CaloDetDescrElement* dde = sem_mgr->get_element (idhash);
           float et_calc = energy*dde->sinTh()*1e-3; // in GeV
           float et_calc_t = et_calc * time;
           bool passBCID=false;
           for(unsigned int i=0;i<m_bcidBands.size()-1;i++){
                if ( (et_calc > m_bcidBands[i]) && (et_calc< m_bcidBands[i+1]) ){
                if ( (et_calc_t > m_bcidLowLim[i]*et_calc) && (et_calc_t < m_bcidUpLim[i]*et_calc) ) passBCID=true;
                  std::cout << "check : " << et_calc << " " << energy << " " << time << " " << i << " " << m_bcidBands[i] << " " << m_bcidBands[i+1] << " " << et_calc_t << " " << m_bcidLowLim[i] << " " << m_bcidUpLim[i] << " " << m_bcidLowLim[i]*et_calc << " " << m_bcidUpLim[i]*et_calc << std::endl;
           	  std::cout << prov << " " << (!0x200) << " " << (prov & (!0x200)) << std::endl;
                  std::cout << "final result : " << passBCID << std::endl;
		  break;
                }
           }

           prov = prov & (!0x200);
           if ( passBCID ) prov |= 0x200;
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
      
      if(!m_buildDiscChannel && !m_larCablingSvc->isOnlineConnected(m_params->curr_chid)){
	  m_counter++;
  	  return false;
      }

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
	     (*pLog) << MSG::DEBUG << "One LArRawChannelBuilderTool failed" << endmsg;
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
        if ( (bcid!=middle_point) && (times[bcid]!=0) && (std::abs(times[bcid])<13e3) ) // If within bad BC, bad
		passBCIDTime=false;
      }
      energy=energies[middle_point];
      time  =times[middle_point]*1e-3;
      if ( passBCIDTime ) {
	if ( std::abs(time)<25 ) passBCIDTime=true;
	else passBCIDTime=false;
      }
	
      if ( passBCIDmax  ) prov |= 0x40 ;
      if ( passBCIDTime ) prov |= 0x200 ;
      prov |= 0x2000;
	 
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
      (*pLog) << MSG::DEBUG << "One PedestalTool failed" << endmsg;
  
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
      (*pLog) << MSG::DEBUG << "One ADC2Energy Tool failed" << endmsg;
  
  return;
}

StatusCode LArSuperCellBuilderDriver::finalize()
{
  ATH_MSG_INFO( "LArSuperCellBuilderdriver finalize."  );
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
