/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArRampBuilder.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArCalibTriggerAccumulator.h"
#include "LArRawConditions/LArRampComplete.h"
#include "LArRecConditions/ILArBadChannelMasker.h"

#include <Eigen/Dense>

#include <fstream>

//template <> const LArRampBuilder::ACCRAMP LArConditionsContainerDB<LArRampBuilder::ACCRAMP>::m_empty;

#include "LArRawConditions/LArConditionsContainerDB.h"

//#define LARRAMPBUILDER_DEBUGOUTPUT

LArRampBuilder::LArRampBuilder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_peakParabolaTool("LArParabolaPeakRecoTool"),
    m_peakShapeTool("LArShapePeakRecoTool"),
    m_peakOFTool("LArOFPeakRecoTool"),
    m_event_counter(0),
    m_recoType(OF),
    m_larCablingSvc("LArCablingService"),
    //m_badChanneMasker("LArBadChanTool"),
    m_onlineHelper(),
    m_emId(0),
    m_groupingType("ExtendedFeedThrough"),
    m_dd_rinj(0),m_iterate(false)
{
  declareProperty("KeyList",         m_keylist);
  declareProperty("KeyOutput",       m_keyoutput="LArRamp");
  declareProperty("SubtractDac0",    m_dac0sub=true);
  declareProperty("StoreRawRamp",    m_saveRawRamp=false);
  declareProperty("StoreRecRamp",    m_saveRecRamp=true);
  // declareProperty("PreviousRunToDB", m_previousrunDB=0);
  declareProperty("FolderName",      m_folderName="LArElecCalibTB04");
  declareProperty("Polynom",         m_degree=1);
  declareProperty("RampRange",       m_maxADC=0);
  declareProperty("doSatSlope",      m_satSlope=true);
  declareProperty("ConsecutiveADCs", m_consADC=50);
  declareProperty("RecoType",        m_recoTypeProp=std::string("Parabola")) ;
  declareProperty("correctBias",     m_correctBias=false);
  declareProperty("ShapeMethodDAC",  m_shapeMethodDAC=400);
  declareProperty("DAC0",            m_DAC0=0);
  declareProperty("LongNtuple",      m_longNtuple=false);
  declareProperty("WithIntercept",   m_withIntercept=true);
  declareProperty("minDAC",          m_minDAC=0);
  declareProperty("GroupingType",    m_groupingType);
  declareProperty("DelayShift",      m_delayShift=23); //Only for OF peak reco
  declareProperty("PeakOFTool",      m_peakOFTool); 
  declareProperty("isHEC",           m_ishec=false);
  declareProperty("HECKey",          m_hec_key="");
  declareProperty("BadChannelMask",  m_badChannelMask);
  declareProperty("CorrectBadChannels",  m_doBadChannelMask = false);
  declareProperty("Iterate",         m_iterate = false);

  int defaultDeadChannelCut;
  if (m_dac0sub)
    defaultDeadChannelCut=300;
  else
    defaultDeadChannelCut=1300;
  declareProperty("DeadChannelCut",m_DeadChannelCut=defaultDeadChannelCut);
  m_delay=-1;
  m_ipassShape = 0; // temporary
  m_ipassPedestal = 0; // temporary
  m_ramps=NULL; 
  m_fatalFebErrorPattern=0xffff;
}

LArRampBuilder::~LArRampBuilder() 
{}

StatusCode LArRampBuilder::initialize()
{
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = m_larCablingSvc.retrieve(); 
  if(sc.isFailure()){
    msg(MSG::FATAL) << "Could not retrieve LArCablingService Tool" << endmsg;
    return sc;
  }
  if(m_doBadChannelMask) { 
    sc=m_badChannelMask.retrieve(); 
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve BadChannelMask "
		    << m_badChannelMask << endmsg;
      return StatusCode::FAILURE;
    }
  }

  m_ramps=new LArConditionsContainer<ACCRAMP>();
  //FIXME: Thats probably nonsenes, these raw ramps aren't written to COOL
  sc=m_ramps->setGroupingType(m_groupingType,msg()); 
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to set groupingType for intermediate LArRamps object" << endmsg;
    return sc;
  }
  sc=m_ramps->initialize(); 
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed initialize intermediate LArRamps object" << endmsg;
    return sc;
  }
  
  chooseRecoMode() ;
  m_event_counter=0;
  
  unsigned int online_id_max = m_onlineHelper->channelHashMax() ; 
  m_thePedestal.resize(online_id_max,-1); 
  
  if(m_ishec) {
     sc = detStore()->regHandle(m_dd_rinj,m_hec_key);
     if (sc!=StatusCode::SUCCESS) {
       msg(MSG::ERROR) << "Cannot get register callback for HEC map" << endmsg; 
       msg(MSG::ERROR) << "Will use default "<< endmsg;
     } else {
       msg(MSG::INFO) << " register callback for HEC map " << endmsg;
     }
  }

  return StatusCode::SUCCESS;
}


void LArRampBuilder::chooseRecoMode()  {
  
  // choose reconstructiom mode
  if ( m_recoTypeProp == std::string("Parabola") ) {
    m_recoType=PARABOLA;
    StatusCode sc=m_peakParabolaTool.retrieve();
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "Can't get LArParabolaPeakRecoTool" << endmsg;
	return;
      }
    ATH_MSG_DEBUG("LArParabolaPeakRecoTool retrieved with success!");
    
    if(m_correctBias){
      // if using parabola, get the cabling service and offlineID helper to obtain the layer (needed for correction) 
      const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
      m_emId=caloIdMgr->getEM_ID();
      if (!m_emId) {
	msg(MSG::ERROR) << "Could not access lar EM ID helper" << endmsg;
	return ;
      }
      
    }
    
    // Shape reconstruction
  } else if (m_recoTypeProp == std::string("Shape") ) {
    m_recoType=SHAPE;
    ATH_MSG_INFO( "ShapePeakReco mode is ON ! ");
    if (m_peakShapeTool.retrieve().isFailure()) {
      ATH_MSG_ERROR( "Can't get LArShapePeakRecoTool");
      return;
    }
    ATH_MSG_DEBUG("LArShapePeakRecoTool retrieved with success!");
    
    // OFC recontruction 
  } else if ( m_recoTypeProp == std::string("OF") ) {
    m_recoType=OF;
    if (m_peakOFTool.retrieve().isFailure()) {
      ATH_MSG_ERROR( "Can't get LArOFPeakRecoTool");
      return;
    }
    ATH_MSG_DEBUG("LArOFPeakRecoTool retrieved with success!");
  }
}

// ********************** EXECUTE ****************************
StatusCode LArRampBuilder::execute()
{ 

  StatusCode sc;
  sc.setChecked();
  if ( m_event_counter < 100 || m_event_counter%100==0 )
    ATH_MSG_INFO( "Processing event " << m_event_counter);
  ++m_event_counter;
  
  if (m_keylist.size()==0) {
    ATH_MSG_ERROR( "Key list is empty! No containers to process!");
    return StatusCode::FAILURE;
  }
  
  const LArFebErrorSummary* febErrSum=NULL;
  if (evtStore()->contains<LArFebErrorSummary>("LArFebErrorSummary")) {
    sc=evtStore()->retrieve(febErrSum);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve FebErrorSummary object!");
      return sc;
    }
  }
  else
    if (m_event_counter==1)
      ATH_MSG_WARNING("No FebErrorSummaryObject found! Feb errors not checked!");
 

  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  
  const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer = nullptr;
  
  
  // if using Shape Reco method, retrieve caliWaveContainer (only once !)
  // FT remove the bitwise &&->&
  if ( m_recoType == SHAPE && m_ipassShape==0) {
    
    m_ipassShape = 1;
    
    // keep only 8 samples of wave - dont need all samples for pseudo-OF reco
    std::vector<double> tempWave;
    int NSamplesKeep = 8;
    tempWave.resize(24*NSamplesKeep);
    
    m_CaliWaves.resize(3);
    m_CaliDACs.resize(3);
    m_IndexDAC0.resize(3);
    m_IndexHighestDAC.resize(3);
    
    // retrieve cali wave container
    
    const LArCaliWaveContainer* caliWaveContainer = NULL;
    ATH_MSG_WARNING("Will retrieve LArCaliWaveContainer ");
    sc= detStore()->retrieve(caliWaveContainer,"CaliWave");
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Cannot read LArCaliWaveContainer from StoreGate for key 'CaliWave' ! ");
      return StatusCode::FAILURE;
    }
	ATH_MSG_DEBUG("Succefully retrieved LArCaliWaveContainer from StoreGate!");
    
    for (;key_it!=key_it_e;key_it++) { //Loop over all containers that are to be processed (e.g. different gains)
      
      // first, set reference DAC (dirty hardcoding for now...)
      CaloGain::CaloGain gainref = CaloGain::LARLOWGAIN;
      if(*key_it == "HIGH") {
	gainref=CaloGain::LARHIGHGAIN;
      }else if(*key_it == "MEDIUM") {
	gainref=CaloGain::LARMEDIUMGAIN;
      }else if(*key_it == "LOW") {
	gainref=CaloGain::LARLOWGAIN;
      }
      
      m_CaliWaves[gainref].resize(m_onlineHelper->channelHashMax());
      m_CaliDACs[gainref].resize(m_onlineHelper->channelHashMax());
      m_IndexDAC0[gainref].resize(m_onlineHelper->channelHashMax());
      m_IndexHighestDAC[gainref].resize(m_onlineHelper->channelHashMax());
      
      // Set gain from key value
      int gain = CaloGain::LARHIGHGAIN;
      if      ((*key_it) == "MEDIUM") gain = CaloGain::LARMEDIUMGAIN;
      else if ((*key_it) == "LOW")    gain = CaloGain::LARLOWGAIN;
      
      // extract from all the waves the ones we are interested in (a given DAC value)
      // and order them by hash ID in a vector
      typedef LArCaliWaveContainer::ConstConditionsMapIterator const_iterator;
      const_iterator itVec   = caliWaveContainer->begin(gain);
      const_iterator itVec_e = caliWaveContainer->end(gain);
      
      for (; itVec != itVec_e; ++itVec) {
	
	LArCaliWaveContainer::LArCaliWaves::const_iterator itwave     = (*itVec).begin();
	LArCaliWaveContainer::LArCaliWaves::const_iterator itwave_end = (*itVec).end();
	
	for (;itwave!=itwave_end;itwave++) {  //Loop over all cells
	  const LArCaliWave& larCaliWave=(*itwave);
	  int DAC = larCaliWave.getDAC();
	  IdentifierHash chidwave_hash = m_onlineHelper->channel_Hash(itVec.channelId());
	  
	  bool IsBad = false;
	  for(int i=0;i<24*NSamplesKeep;i++){
	    tempWave[i] = larCaliWave.getSample(i);
	    if(tempWave[i]<-500) { // check that this wave is not corrupted
	      IsBad = true; 
	      break;
	    }
	  }
	  
	  if(IsBad) continue; // if corrupted wave, skip it;
	  
	  m_CaliWaves[gainref][chidwave_hash].push_back(tempWave);
	  m_CaliDACs[gainref][chidwave_hash].push_back(DAC);
	  
	  // remember index of highest DAC value for this cell (i.e. non-saturating)
	  if(DAC > m_CaliDACs[gainref][chidwave_hash][m_IndexHighestDAC[gainref][chidwave_hash]]) m_IndexHighestDAC[gainref][chidwave_hash]=m_CaliDACs[gainref][chidwave_hash].size()-1;
	  
	  // remember which index corresponds to DAC0
	  if(m_dac0sub && DAC == m_DAC0) {
	    m_IndexDAC0[gainref][chidwave_hash] = m_CaliDACs[gainref][chidwave_hash].size()-1;
	    ATH_MSG_DEBUG("Cell " << chidwave_hash << ": DAC0 is at index = " << m_IndexDAC0[gainref][chidwave_hash]);
	  }
	} // loop over dac values
      } // loop over cells
    } // Loop over gains

    sc= detStore()->remove(caliWaveContainer);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Cannot remove LArCaliWaveContainer from StoreGate ! ");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Succefully removed LArCaliWaveContainer from StoreGate ");
    
  } // m_ipassShape
  
  
  // now start to deal with digits   
  for (;key_it!=key_it_e;key_it++) { //Loop over all containers that are to be processed (e.g. different gains)
    
    sc= evtStore()->retrieve(larAccumulatedCalibDigitContainer,*key_it);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it);
      continue; //Try next container
    }
    HWIdentifier  lastFailedFEB(0);
    LArAccumulatedCalibDigitContainer::const_iterator it=larAccumulatedCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator it_end=larAccumulatedCalibDigitContainer->end();
    
    if(it == it_end) ATH_MSG_DEBUG("LArAccumulatedCalibDigitContainer with key=" << *key_it << " is empty ");
    
    for (;it!=it_end;it++) {  //Loop over all cells
    
      if (!(*it)->isPulsed()){  //Check if cell is pulsed
	continue; //Cell not pulsed -> ignore
      }

      HWIdentifier chid=(*it)->hardwareID();
      HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      if (febErrSum) {
	const uint16_t febErrs=febErrSum->feb_error(febid);
	if (febErrs & m_fatalFebErrorPattern) {
	  if (febid!=lastFailedFEB) {
	    lastFailedFEB=febid;
	    ATH_MSG_ERROR( "Event " << m_event_counter << " Feb " <<  m_onlineHelper->channel_name(febid) 
		<< " reports error(s):" << febErrSum->error_to_string(febErrs) << ". Data ignored.");
	  }
	  continue;
	}
      }

      if (m_delay==-1) { //First (pulsed) cell to be processed:
	m_delay=(*it)->delay();
      }
      else
	if (m_delay!=(*it)->delay()) {
	  ATH_MSG_ERROR( "Delay does not match! Found " << (*it)->delay() << " expected: " << m_delay);
	  continue; //Ignore this cell
	}
      
      CaloGain::CaloGain gain=(*it)->gain();
      if (gain<0 || gain>CaloGain::LARNGAIN)
	{ATH_MSG_ERROR( "Found not-matching gain number ("<< (int)gain <<")");
	  return StatusCode::FAILURE;
	}
      
      // if using bias-corrected Parabola tool or OFC Tool, get the pedestal
      if ( (m_recoType == PARABOLA && m_correctBias )  || m_recoType == OF) {

	//GU, try to get only once the pedestal per channel...
        IdentifierHash chid_hash = m_onlineHelper->channel_Hash(chid);
        if (m_thePedestal[chid_hash] < 0) {
	  
	  //Pointer to conditions data objects 
	  const ILArPedestal* larPedestal=NULL;
	  sc=detStore()->retrieve(larPedestal);
	  if (sc.isFailure()) {
	    //larPedestal=NULL;
	    //ATH_MSG_WARNING("No pedestals found in database. Use default value for all channels.");
	    //m_thePedestal[chid_hash] = 1000;
	    msg(MSG::FATAL) << "No pedestals found in database. Aborting executiong." << endmsg;
	    return sc;
	  }
	  
	  if (larPedestal) {
	    float DBpedestal = larPedestal->pedestal(chid,gain);
	    if (DBpedestal >= (1.0+LArElecCalib::ERRORCODE) ) {
	      m_thePedestal[chid_hash]=DBpedestal;
	    } else {
	      ATH_MSG_WARNING("No pedestal value found for cell hash ID = " 
		  << chid_hash << " " << m_onlineHelper->channel_name(chid) 
		  << ".  Skipping channel." 
		 );
	      continue;
	    }
	  } else {
	    ATH_MSG_WARNING("No pedestal value found for cell hash ID = " 
		<< chid_hash << " " << m_onlineHelper->channel_name(chid) 
		<< ".  Skipping channel." 
		);
	    continue;
	  }

	  ATH_MSG_DEBUG(" channel,pedestal " << chid_hash << " " 
	      << m_thePedestal[chid_hash]);

	} // m_ipassPedestal 	 
      }
      
      // if ((*it)->DAC()==1110 || (*it)->DAC()==410 ) {
      //     int pos_neg = m_onlineHelper->pos_neg(chid);
      //    int ft=m_onlineHelper->feedthrough(chid);
      //    int slot=m_onlineHelper->slot(chid);
      //    int channel=m_onlineHelper->channel(chid);
      //    if (slot>10 && slot<15) {
      //      std::cout << "cell = " << chid << " " << pos_neg << " " << ft << " " << slot << " " << channel 
      //            << ", gain = " << gain << ", DAC = " << (*it)->DAC() 
      //            << " mean " << (*it)->mean()[0] << " " << (*it)->mean()[1] << " " << (*it)->mean()[2] << " " << (*it)->mean()[3]
      //            << " " << (*it)->mean()[4] << " " << (*it)->mean()[5] << " " << (*it)->mean()[6] << std::endl;
      //     }
      //  }
      
      LArCalibTriggerAccumulator& accpoints=(m_ramps->get(chid,gain))[(*it)->DAC()];
      //rawramp.addAccumulatedEvent( (*it)->mean(),(*it)->RMS(), (*it)->nTriggers() );
      LArCalibTriggerAccumulator::ERRTYPE ec=accpoints.add((*it)->sampleSum(),(*it)->sample2Sum(),(*it)->nTriggers());
      if (ec==LArCalibTriggerAccumulator::WrongNSamples) {
	ATH_MSG_ERROR( "Failed to accumulate sub-steps: Inconsistent number of ADC samples");
      }
      if (ec==LArCalibTriggerAccumulator::NumericOverflow) {
	ATH_MSG_ERROR( "Failed to accumulate sub-steps: Numeric Overflow");
      }
    }//End loop over all cells
  } //End loop over all containers
  
  return StatusCode::SUCCESS;
} 

// ********************** FINALIZE ****************************
StatusCode LArRampBuilder::stop()
{ 
  ATH_MSG_INFO( "in stop."); 

  StatusCode sc;
  sc.setChecked();
  //Create transient ramp object (to be filled later) (one object for all gains)
  LArRampComplete* larRampComplete;
  if (m_saveRecRamp){
    larRampComplete=new LArRampComplete();
    sc=larRampComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArRampComplete object");
      return sc;
    }
    sc=larRampComplete->initialize(); 
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed initialize LArRampComplete object");
      return sc;
    }
    
  }
  else
    larRampComplete=NULL;
  
  int containerCounter=0;
  //Outermost loop goes over all gains (different containers).
  //for (CaloGain::CaloGain gain=CaloGain::LARHIGHGAIN;gain<CaloGain::LARNGAIN;gain++) {

  int NRamp=0;

  for (unsigned k=0;k<(int)CaloGain::LARNGAIN;k++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)k;
    LArConditionsContainer<ACCRAMP>::ConditionsMapIterator cell_it=m_ramps->begin(gain);
    LArConditionsContainer<ACCRAMP>::ConditionsMapIterator cell_it_e=m_ramps->end(gain);
    if (cell_it==cell_it_e) {
      ATH_MSG_INFO( "No ramp points found for gain " << gain);
      continue; //No data for this gain
    }
    //Create transient object for raw ramp (one container per gain)
    LArRawRampContainer* larRawRampContainer;
    if (m_saveRawRamp)
      larRawRampContainer=new LArRawRampContainer();
    else
      larRawRampContainer=NULL;

    //Inner loop goes over the cells.
    for (;cell_it!=cell_it_e;cell_it++){
      
      const HWIdentifier chid = cell_it.channelId();

      ACCRAMP::const_iterator dac_it=cell_it->begin();
      ACCRAMP::const_iterator dac_it_e=cell_it->end();
      LArRawRamp* rawramp=new LArRawRamp(chid,gain);
      
      std::vector<float> peak;
      float adcpeak, timepeak;
      std::vector<float> adc0v;
      bool isADCsat = false;

      for (;dac_it!=dac_it_e;dac_it++) {

        LArRawRamp::RAMPPOINT_t ramppoint;

	adcpeak  = -999.;
	timepeak = -999.;
	
	// prepare samples
	float MaxADC = 0;
	int iMaxADC = 0;
	// if DAC0, fill adc0v vector
	if(m_dac0sub && dac_it->first== m_DAC0){
	  // check that DAC0 is the first DAC of list
	  if(dac_it!=cell_it->begin()) 
	    msg(MSG::ERROR) << "DAC0 is not the first DAC ? This might be a problem... " << endmsg;
	  adc0v = dac_it->second.mean();
	  ramppoint.Samples   = adc0v;
	  ramppoint.RMS       = dac_it->second.RMS();
	  ramppoint.NTriggers = dac_it->second.nTriggers();
	}else{// if not DAC0, substract DAC0 to current DAC
	  const size_t nS=dac_it->second.nsamples();
	  adc0v.resize(nS,0.0);
	  ramppoint.Samples.resize(nS);
	  ramppoint.RMS.resize(nS);
	  ramppoint.NTriggers = dac_it->second.nTriggers();
	  for(size_t k=0;k<nS;++k){
	    ramppoint.Samples[k]=dac_it->second.mean(k) - adc0v[k];
	    ramppoint.RMS[k]=dac_it->second.RMS(k);
	    // find sample max and its index
	    if(ramppoint.Samples[k]>MaxADC){
	      MaxADC = ramppoint.Samples[k];
	      iMaxADC = k;
	    }
	  }
	}
      
  	// reconstruct
	if ( m_recoType == OF) {
	  // subtract pedestal (if no DAC0 subtraction)
          if (!m_dac0sub) {
            IdentifierHash chid_hash = m_onlineHelper->channel_Hash(chid);
            for (size_t k=0;k<ramppoint.Samples.size();++k) {
              ramppoint.Samples[k] = ramppoint.Samples[k] - m_thePedestal[chid_hash];
            }
          }

	  //The following lines have been moved here from LArOFPeakReco tool:
	  float delay=m_delay;
	  unsigned kMax = max_element(ramppoint.Samples.begin(),ramppoint.Samples.end()) - ramppoint.Samples.begin() ;
          unsigned kLow, kUp;
          //unsigned nIter=0;
          if(!m_iterate) { // No iteration, original code
	   if ( kMax < 2 || kMax+2 >= ramppoint.Samples.size() ) {
	    // probably wrong max for small signal = noise artifact 
	    kMax=2;
            bool isgood=true;
            if(m_doBadChannelMask && m_badChannelMask->cellShouldBeMasked(chid)) isgood=false;
	    if (m_larCablingSvc->isOnlineConnected(chid) && isgood) {
	      msg(MSG::WARNING) << "Not enough samples around the maximum! Use kMax=2 ("
				<< m_onlineHelper->channel_name(chid) <<", DAC=" << dac_it->first 
				<< ", Amp[2]=" << ramppoint.Samples[2] <<   " )" << endmsg;
	      if (msgLvl(MSG::VERBOSE)) {
		msg(MSG::VERBOSE) <<  " Samples: ";
		for (unsigned k=0;k<ramppoint.Samples.size();k++) 
		  msg() << ramppoint.Samples[k] << " "; 
		msg() << endmsg;
	      }//end if verbose message
	    }//end if bad or disconnected channel
	   }//end if kmax out-of-range
	  
	   // convention delay=0 OFC use samp 0-1-2-3-4
	   //            delay=24 OFC use samp 1-2-3-4-5
	   // => if kmax=2 : choose OFC with delay + delayShift
	   //    if kmax=3 : choose OFC with delay+ delayShift+24
	   //    if kmax=4 : stick to kmax=3 
	   //GU temporary hardcoded number. To move to jobOptions

	   if (kMax==4) kMax=3;
	   if (kMax==3) delay += (m_delayShift+24);
	   if (kMax==2) delay += m_delayShift;
	   ATH_MSG_VERBOSE("kMax " << kMax << " delay " << delay);
	   //std::cout << " kMax " << kMax << " delay " << delay << std::endl;
	   delay=(delay-0.5)*(25./24.);
	   //Call OF peak reco tool with no iteration and peak-sample forced to kMax
           kLow = kUp = kMax;
          } else { // code with iteration
           kLow = kMax - 2;
           kUp = kMax + 2;
           delay = 0.;
           //nIter = 10;
          }
	  //const LArOFPeakRecoTool::Result &results=m_peakOFTool->peak(ramppoint.Samples,chid,gain,delay,0,kMax,kMax,kMax);
	  const LArOFPeakRecoTool::Result results=m_peakOFTool->peak(ramppoint.Samples,chid,gain,delay,0,kMax,kLow,kUp);
	  if (results.getValid()) {
	    adcpeak  = results.getAmplitude();
	    timepeak = results.getTau();

	    //  std::cout << "RampReco: A=" << adcpeak << " t=" << timepeak 
	    // 		 << " peakIndex=" << results.getPeakSample() << "(" << kMax<<")"
	    // 		 << " delay=" << results.getDelay() << "(" << m_delay<<")"
	    // 		 << " #samples: "<< ramppoint.Samples.size()  << std::endl;

	  }
	  else 
	    ATH_MSG_ERROR( "LArOFPeak reco tool returns invalid result.");


// 	  peak=m_peakOFTool->peak1(ramppoint.Samples,chid,gain,m_delay);
//           if (peak.size()>1) {
//             adcpeak = peak[0];
//             timepeak = peak[1];
// 	    std::cout << "RampReco: A=" << adcpeak << " t=" << timepeak << std::endl;
//           }
	} else if ( m_recoType == SHAPE) {
	  
	  IdentifierHash chid_hash = m_onlineHelper->channel_Hash(chid);
	  
	  // reconstruct for non-DAC0 values and non-saturating waves
	  if(dac_it->first!= m_DAC0 && dac_it->first <= m_CaliDACs[gain][chid_hash][m_IndexHighestDAC[gain][chid_hash]]){
	    
	    //	    std::cout <<" Highest DAC for cell " << chid_hash << " is " << m_CaliDACs[chid_hash][m_IndexHighestDAC[chid_hash]] << ", current DAC is " << dac_it->first << std::endl;

	    // find appropriate wave
	    unsigned int GoodIndex = 9999;
	    for(unsigned int i=0;i<m_CaliDACs[gain][chid_hash].size();i++){
	      if(dac_it->first == m_CaliDACs[gain][chid_hash][i]) GoodIndex=i;
	    }
	    if(GoodIndex == 9999) {
	      ATH_MSG_WARNING("No wave found for cell = " << chid_hash << ", DAC = " << dac_it->first);
	      float min = 9999999;
	      for(unsigned int i=0;i<m_CaliDACs[gain][chid_hash].size();i++){
		if(abs(dac_it->first - m_CaliDACs[gain][chid_hash][i])<min) 
		  {
		    min=abs(dac_it->first - m_CaliDACs[gain][chid_hash][i]);
		    GoodIndex=i;
		  }
	      } 
	      ATH_MSG_WARNING("Replace with DAC = " << m_CaliDACs[gain][chid_hash][GoodIndex]);
	    }
	    // substract DAC0 wave
	    for(unsigned int k=0;k<m_CaliWaves[gain][chid_hash][GoodIndex].size();k++){
	      m_CaliWaves[gain][chid_hash][GoodIndex][k] -= m_CaliWaves[gain][chid_hash][m_IndexDAC0[gain][chid_hash]][k];
	    }
	    
	    // apply reconstruction 
	    if(m_CaliWaves[gain][chid_hash][GoodIndex].size() > 0){
	      peak=m_peakShapeTool->peak(ramppoint.Samples,m_CaliWaves[gain][chid_hash][GoodIndex]);  
	      ATH_MSG_DEBUG("cell chid=" << chid.get_compact() << ",peak= " << peak[0]);
	    }else{
	      ATH_MSG_ERROR( "No wave for this cell chid=" << chid.get_compact() << ",hash= " << chid_hash);
	      peak.push_back(-999);
	      peak.push_back(-999);
	    }
	    
	    if(peak.size()>0){
	      adcpeak = peak[0];
	      timepeak = peak[1];
	    }
	  }
        } else if ( m_recoType == PARABOLA ) {
	  
	  if(m_correctBias){
	    
	    IdentifierHash chid_hash = m_onlineHelper->channel_Hash(chid);
	    
	    // get layer for correction
	    Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
	    int layer=m_emId->sampling(id);
	    //	    std::cout << "samples = " << ramppoint.Samples.size() << ", 0 = " << ramppoint.Samples[0] << ", 1 = " << ramppoint.Samples[1] << std::endl;
	    peak=m_peakParabolaTool->peak(ramppoint.Samples,layer,m_thePedestal[chid_hash]);
	    
	  }else{
	    // call peak reco without layer --> no bias correction 
	    peak=m_peakParabolaTool->peak(ramppoint.Samples);
	  }
	  adcpeak = peak[0];
	  timepeak = peak[1];
	  
	  
	} else {
	  ATH_MSG_ERROR( "Both OF and Parabola reconstruction modes not available!" ) ;
          delete larRawRampContainer;
	  return StatusCode::FAILURE ;
	} 
	
	ramppoint.ADC        = adcpeak;
	ramppoint.DAC        = dac_it->first;
        if(m_ishec && m_onlineHelper->isHECchannel(chid)) {
           if(m_dd_rinj) {
              const float rinj = m_dd_rinj->Rinj(chid);
              if(rinj < 4) ramppoint.DAC /= 2;
           }
        }

	ramppoint.iMaxSample = iMaxADC;
	ramppoint.TimeMax    = timepeak;
	
	// resize samples to 0 if dont want to make large ntuple
	if(!m_longNtuple){
	  ramppoint.Samples.resize(0);
	  ramppoint.RMS.resize(0);
	}
	// only add to rawramp non saturing points (using rawdata information)
	//	if( (dac_it->first>= m_minDAC) &&  ((m_maxADC <= 0) || (MaxADC < m_maxADC)) && ((dac_it->first!=m_DAC0&adcpeak>-999) || (dac_it->first==m_DAC0)) ) { 
	if( (dac_it->first>= m_minDAC) &&  ramppoint.ADC > -998 
	    && ((m_maxADC <= 0) || (MaxADC < m_maxADC)) ) {
	  //      std::cout << "Adding Point for Ramp: Gain=" << gain << " DAC="<< ramppoint.DAC << " ADC=" << ramppoint.ADC << std::endl;
	  rawramp->add(ramppoint);
	}
	else if ((m_maxADC > 0)&&(MaxADC >= m_maxADC)) { isADCsat = true; } // if ADC saturated at least once, it should be notified
      }
      
      //Build ramp object..........
      if (larRampComplete) {
	std::vector<LArRawRamp::RAMPPOINT_t>& data=rawramp->theRamp();
	sort(data.begin(),data.end()); //Sort vector of raw data (necessary to cut off nonlinar high ADC-values)
	std::vector<float> rampCoeffs;
	std::vector<int> vSat;
	//bool isConnected = m_larCablingSvc->isOnlineConnected(chid);
	StatusCode sc=rampfit(m_degree+1,data,rampCoeffs,vSat,chid);
	if (sc!=StatusCode::SUCCESS){
	  if (!m_larCablingSvc->isOnlineConnected(chid))
	      ATH_MSG_DEBUG("Failed to produce ramp for disconnected channel " << m_onlineHelper->channel_name(chid));
	  else if (m_doBadChannelMask && m_badChannelMask->cellShouldBeMasked(chid,gain))
	    ATH_MSG_INFO( "Failed to produce ramp for known bad channel " << m_onlineHelper->channel_name(chid));
	  else
	    ATH_MSG_ERROR( "Failed to produce ramp for channel " << m_onlineHelper->channel_name(chid));
	}
	else{
	  if(rampCoeffs[1]<0) 
	    msg (MSG::ERROR) <<  "Negative 1rst order coef for ramp = " << rampCoeffs[1] << " for channel " 
			     << m_onlineHelper->channel_name(chid) << endmsg;

	  if (vSat[0] != -1) { rawramp->setsat(vSat[0]); } 	// if a saturation point was found in rampfit, record it 
	  else {
	    if (isADCsat) { rawramp->setsat(data.size()-1); }	// if no saturation point was found, and ADC saturation happened, record the last ramp point 	
	    if (!isADCsat) { rawramp->setsat(data.size()); }	// if no saturation point was found, and ADC saturation did not happen, record the ramp size
	  }
	   
	  if (larRampComplete){ //Produce transient object
	    larRampComplete->set(chid,(int)gain,rampCoeffs);
	    NRamp++;
	  }
	}// end else (rampfitting suceeded)
      }// end if (build ramp object)
      //Save raw ramp for this cell, if requested by jobOpts
      if (larRawRampContainer){
	larRawRampContainer->push_back(rawramp);
      }
      else{
	delete rawramp;
      }
    }//end loop cells

    if (larRawRampContainer) {
      std::string key;
      switch (gain) {
      case CaloGain::LARHIGHGAIN:
	key="HIGH";
	break;
      case  CaloGain::LARMEDIUMGAIN:
	key="MEDIUM";
	break;
      case  CaloGain::LARLOWGAIN:
	key="LOW";
	break;
      default:
	key="UNKNOWN";
	break;
      }
      key = m_keyoutput + key;
      ATH_MSG_INFO( "Recording LArRawRampContainer for gain " << (int)gain << " key=" << key);
      sc=detStore()->record(larRawRampContainer,key);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to record LArRawRamp object");
      }
    }// end if larRawRampContainer
    ++containerCounter;
  }//end loop over containers

  if (containerCounter==0) {
    ATH_MSG_WARNING("No Ramps have been produced. No data found.");
    return StatusCode::FAILURE;
  }

  if (larRampComplete){  //Save the transient  Ramp object. 

    //ATH_MSG_INFO( " Summary : Number of cells with a ramp value computed : " << larRampComplete->totalNumberOfConditions() );
    ATH_MSG_INFO( " Summary : Number of cells with a ramp value computed : " << NRamp );
    ATH_MSG_INFO( " Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 ");
    ATH_MSG_INFO( " Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 ");
    ATH_MSG_INFO( " Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 ");
    ATH_MSG_INFO( " Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 ");
    ATH_MSG_INFO( " Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 ");

    sc=detStore()->record(larRampComplete,m_keyoutput);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to record LArRampComplete object");
    }
    sc=detStore()->symLink(larRampComplete, (ILArRamp*)larRampComplete);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to symlink LArRawRamp object");
    }
  }
  delete m_ramps;//Not needed any more. Free memory.
  ATH_MSG_INFO( "LArRampBuilder has finished.");
  return StatusCode::SUCCESS;
}// end finalize-method.

 
StatusCode LArRampBuilder::rampfit(unsigned deg, const std::vector<LArRawRamp::RAMPPOINT_t>& data, 
				   std::vector<float>& rampCoeffs, std::vector<int>& vSat, 
                                   const HWIdentifier chid) {
  unsigned linRange=data.size();
  if (linRange<2) {
    bool isgood=true;
    if(m_doBadChannelMask && m_badChannelMask->cellShouldBeMasked(chid)) isgood=false; 
    if (m_larCablingSvc->isOnlineConnected(chid) && isgood ) {
      msg(MSG::ERROR) << "Not enough datapoints (" << linRange << ") to fit a polynom!" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Not enough datapoints (" << linRange << ") to fit a polynom for a disconnected or known bad channel!" );
      return StatusCode::FAILURE;
    }
  }
  //std::cout << "linRange = " << linRange << std::endl;
  //std::cout << "highest DAC = " << data[linRange-1].DAC << std::endl;  

  int satpoint = -1;
  if (m_satSlope) {
  
    float thisslope = 0., meanslope = 0.;
//    float accslope[20];
    std::vector<float> accslope;
    accslope.push_back(0);
    for (unsigned int DACIndex=1;DACIndex<linRange;DACIndex++){
    
      thisslope = (data[DACIndex].ADC - data[DACIndex-1].ADC)/(data[DACIndex].DAC - data[DACIndex-1].DAC);

//      if ( (satpoint >= 1) && (thisslope > meanslope) ) { satpoint = -1; } // the slope rises again ! it was a fake saturation
      if ( (satpoint == -1) && ((meanslope-thisslope) > meanslope/10.) ) { satpoint = DACIndex; } // saturation was reached

      meanslope = ( thisslope + (DACIndex-1)*(accslope[DACIndex-1]) )/DACIndex;
//      accslope[DACIndex] = meanslope;
      accslope.push_back(meanslope);

    }
  
    if (satpoint != -1) { linRange = satpoint; } // if a saturation was found, linRange becomes the saturation index
  
  }
  vSat.push_back(satpoint);
  
//if (m_maxADC>0) //Fit only until a adc value smaller or equal than m_maxADC (linRange is the index)
//   for (linRange=data.size();(data[linRange-1].ADC>m_maxADC || fabs(data[linRange-1].ADC-data[linRange-2].ADC)<(double)m_consADC) && linRange>0 ;--linRange);

 //  if (m_maxADC>0) //Fit only until a adc value smaller or equal than m_maxADC (linRange is the index)
 //    for(linRange=1;linRange<=data.size();linRange++){
 //      if(data[linRange-1].ADC>m_maxADC) {
 //	std::cout << "skipping points above " << linRange-1 << std::endl;
 //	break;
 //      } 
 //    }
  
  if (!m_withIntercept) 
    deg--;
  bool isgood=true;
  if(m_doBadChannelMask && m_badChannelMask->cellShouldBeMasked(chid)) isgood=false;
  if (deg>linRange) {
    if (m_larCablingSvc->isOnlineConnected(chid) && isgood ) 
      msg(MSG::ERROR) << "Not enough datapoints before saturation (" << linRange << ") to fit a polynom of degree " << deg << endmsg;
    else
      ATH_MSG_DEBUG("Not enough datapoints before saturation (" << linRange << ") to fit a polynom of degree " << deg 
		    << " (channel disconnected or known to be bad)");
    
    return StatusCode::FAILURE;
  }
  
  if (data[linRange-1].DAC>0 && data[linRange-1].ADC<m_DeadChannelCut && data[linRange-1].ADC!=-999.) {
    msg(MSG::ERROR) << "DAC= " << data[linRange-1].DAC << " yields ADC= " << data[linRange-1].ADC 
	   << ". Dead channel?" << endmsg;
    return StatusCode::FAILURE;
  }

  int begin = 0;
  if(data[0].DAC == m_DAC0) begin = 1; // starts at 1 to skip DAC=0

  Eigen::MatrixXd alpha(deg,deg);
  Eigen::VectorXd beta(deg);
  float sigma2 = 1.;
  for (unsigned k=0;k<deg;k++)
    for (unsigned j=0;j<=k;j++)
      {
	alpha(k,j)=0;
	for (unsigned i=begin;i<linRange;i++) 
	  {
	    // we are not storing any error on the reconstructed
	    // peaks, but we can simply use the error on the sample
	    // means (RMS/sqrt(NTriggers)) to account for any
	    // potential variation on the number of accumulated
	    // triggers. BTW, this would be proportional to the ADC
	    // uncertainly but used as *DAC* uncertainty: in the limit
	    // in which the ramp is linear this is still correct, and
	    // anyway better than nothing. -- M.D. 13/7/2009	    
	    sigma2 = 1.;
	    if ( data[i].NTriggers ) {
	      //float sigma2 = (data[i].RMS[0]*data[i].RMS[0])/data[i].NTriggers;
	      sigma2 = 100./data[i].NTriggers;
	    }
	      // just use trigger number, assume RMS is constant for
	      // all DAC points (same noise). The 100. scale factor is
	      // there to guarantee the same results with respect to
	      // previous fits withour errors (having usually 100
	      // triggers), because of poternaitl numerical
	      // differences when inverting the fit matrix even if
	      // errors are all the same.
	    if (m_withIntercept) {    
	      //alpha[k][j]+=(pow(data[i].ADC,(int)k)*pow(data[i].ADC,(int)j)); // no error
	      alpha(k,j)+=(std::pow(data[i].ADC,(int)k)*std::pow(data[i].ADC,(int)j))/sigma2;
	    } else {
	      //alpha[k][j]+=(pow(data[i].ADC,(int)k+1)*pow(data[i].ADC,(int)j+1)); // no error
	      alpha(k,j)+=(std::pow(data[i].ADC,(int)k+1)*std::pow(data[i].ADC,(int)j+1))/sigma2;
	    }
	    alpha(j,k)=alpha(k,j); //Use symmetry
	  }
      }
  
  for (unsigned k=0;k<deg;k++)
    {
      beta[k]=0;
      for (unsigned i=begin;i<linRange;i++) {
	sigma2 = 1.;
	if ( data[i].NTriggers ) {
	  //float sigma2 = (data[i].RMS[0]*data[i].RMS[0])/data[i].NTriggers;
	  sigma2 = 100./data[i].NTriggers;
	}
	if (m_withIntercept) {
	  // beta[k]+=data[i].DAC*pow(data[i].ADC,(int)k); // no error
	  beta[k]+=(data[i].DAC*pow(data[i].ADC,(int)k))/sigma2; 
	} else {
	  //beta[k]+=data[i].DAC*pow(data[i].ADC,(int)k+1); // no error
	  beta[k]+=(data[i].DAC*pow(data[i].ADC,(int)k+1))/sigma2;
	}
      }
    }
  
  //HepVector comp=solve(alpha,beta);
  //const Eigen::VectorXd comp=alpha.fullPivLu().solve(beta);
  const Eigen::VectorXd comp=alpha.colPivHouseholderQr().solve(beta);

  //Fill RampDB object
  if (!m_withIntercept)
    rampCoeffs.push_back(0);
    
//  for (int l=0;l<comp.num_row() && l<3;l++)
  for (int l=0;l<comp.size() ;l++)
    rampCoeffs.push_back(comp[l]);
  
#ifdef LARRAMPBUILDER_DEBUGOUTPUT
  // ****************************************
  // Output for Dugging:
  for (unsigned i=1;i<data.size();i++)
    std::cout << data[i].DAC << " " << data[i].ADC << " " << std::endl;
  std::cout << "LinRange=" << linRange << std::endl;
  
  //Calculate error:
  double sigma=0;
  for (unsigned k=0;k<linRange;k++) //Run over all data points
    {double DACcalc=comp[0];
      for (int i=1;i<comp.size();i++) //Apply polynom
	DACcalc+=comp[i]*pow(data[k].ADC,i);
      sigma+=(DACcalc-data[k].DAC)*(DACcalc-data[k].DAC);
    }
  sigma=sqrt(sigma);
  if (linRange>1)
    sigma=sigma/(linRange-1);
  
  std::cout << "Components: ";
  for (int i=0;i<comp.size();i++)
    std::cout << comp[i] << " ";
  std::cout << "sigma=" << sigma << std::endl;
#undef LARRAMPBUILDER_DEBUGOUTPUT
#endif 
  
  return StatusCode::SUCCESS;
}
