/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelBuilderIterAlg.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <limits.h>

#define MAXINT INT_MAX
#define MAXINT2 -INT_MAX

LArRawChannelBuilderIterAlg::LArRawChannelBuilderIterAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator) {}
  
StatusCode LArRawChannelBuilderIterAlg::initialize() {
  ATH_CHECK(m_digitKey.initialize());	 
  ATH_CHECK(m_rawChannelKey.initialize());
  ATH_CHECK(m_pedestalKey.initialize());	 
  ATH_CHECK(m_adc2MeVKey.initialize());	 
  ATH_CHECK(m_ofcKey.initialize());	 
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_cablingKey.initialize() );
  ATH_CHECK(m_run1DSPThresholdsKey.initialize(SG::AllowEmpty) );
  ATH_CHECK(m_run2DSPThresholdsKey.initialize(SG::AllowEmpty) );
  if (m_useDBFortQ) {
    if (m_run1DSPThresholdsKey.empty() && m_run2DSPThresholdsKey.empty()) {
      ATH_MSG_ERROR ("useDB requested but neither Run1DSPThresholdsKey nor Run2DSPThresholdsKey initialized.");
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK(detStore()->retrieve(m_onlineId,"LArOnlineID"));

  const std::string cutmsg = m_absECutFortQ.value() ? " fabs(E) < " : " E < "; 
  ATH_MSG_INFO("Energy cut for time and quality computation: " << cutmsg << 
               " taken from COOL folder "<<
               m_run1DSPThresholdsKey.key() << " (run1) " <<
               m_run2DSPThresholdsKey.key() << " (run2) ");

  ATH_CHECK(m_timingContKey.initialize(SG::AllowEmpty));

  return StatusCode::SUCCESS;
}     

StatusCode LArRawChannelBuilderIterAlg::finalize() {
  return StatusCode::SUCCESS;
} 

StatusCode LArRawChannelBuilderIterAlg::execute(const EventContext& ctx) const {

  //Get event inputs from read handles:
  SG::ReadHandle<LArDigitContainer> inputContainer(m_digitKey,ctx);

  //Write output via write handle
  SG::WriteHandle<LArRawChannelContainer>outputContainer(m_rawChannelKey,ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<LArRawChannelContainer>()));
	    
  //Should we store iter results ?
  LArOFIterResultsContainer* outputTimingContainer{nullptr};
  if(!m_timingContKey.empty()) {
    SG::WriteHandle<LArOFIterResultsContainer> timingContHandle(m_timingContKey,ctx);
    ATH_CHECK(timingContHandle.record(std::make_unique<LArOFIterResultsContainer>()));
    outputTimingContainer = timingContHandle.ptr();
  }
  //Get Conditions input
  SG::ReadCondHandle<ILArPedestal> pedHdl(m_pedestalKey,ctx);
  const ILArPedestal* peds=*pedHdl;

  SG::ReadCondHandle<LArADC2MeV> adc2mevHdl(m_adc2MeVKey,ctx);
  const LArADC2MeV* adc2MeVs=*adc2mevHdl;

  SG::ReadCondHandle<ILArOFC> ofcHdl(m_ofcKey,ctx);
  const ILArOFC* ofcs=*ofcHdl;

  SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey,ctx);
  const ILArShape* shapes=*shapeHdl;

  SG::ReadCondHandle<LArOnOffIdMapping> cabling(m_cablingKey,ctx);
  
  std::unique_ptr<LArDSPThresholdsFlat> run2DSPThresh;
  const LArDSPThresholdsComplete* run1DSPThresh = nullptr;
  if (m_useDBFortQ) {
    if (!m_run2DSPThresholdsKey.empty()) {
      SG::ReadCondHandle<AthenaAttributeList> dspThrshAttr (m_run2DSPThresholdsKey, ctx);
      run2DSPThresh = std::unique_ptr<LArDSPThresholdsFlat>(new LArDSPThresholdsFlat(*dspThrshAttr));
      if (ATH_UNLIKELY(!run2DSPThresh->good())) {
        ATH_MSG_ERROR( "Failed to initialize LArDSPThresholdFlat from attribute list loaded from " << m_run2DSPThresholdsKey.key()
                       << ". Aborting." ); 
        return StatusCode::FAILURE;
      }
    }
    else if (!m_run1DSPThresholdsKey.empty()) {
      SG::ReadCondHandle<LArDSPThresholdsComplete> dspThresh (m_run1DSPThresholdsKey, ctx);
      run1DSPThresh = dspThresh.cptr();
    }
    else {
      ATH_MSG_ERROR( "No DSP threshold configured.");
      return StatusCode::FAILURE;
    }
  }

  std::vector<float> signal; //Pedestal-subtracted

  const float fMAXINT = static_cast<float>(MAXINT);
  const float fMAXINT2 = static_cast<float>(MAXINT2);

  //Loop over digits:
  for (const LArDigit* digit : *inputContainer) {

    const HWIdentifier id=digit->hardwareID();
    const bool connected=(*cabling)->isOnlineConnected(id);

    ATH_MSG_VERBOSE("Working on channel " << m_onlineId->channel_name(id));

    const std::vector<short>& samples=digit->samples();
    auto gain=digit->gain();
    const float p=peds->pedestal(id,gain);


    //The following autos will resolve either into vectors or vector-proxies
    const auto& adc2mev=adc2MeVs->ADC2MEV(id,gain);
    
    if (ATH_UNLIKELY(p==ILArPedestal::ERRORCODE)) {
      if (!connected) continue; //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("No valid pedestal for connected channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain);
      return StatusCode::FAILURE;
    }

    if(ATH_UNLIKELY(adc2mev.size()<2)) {
      if (!connected) continue; //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("No valid ADC2MeV for connected channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain);
      return StatusCode::FAILURE;
    }

    uint16_t prov=0; 

    float peakval = -999.;
    unsigned short ipeak = 0;
    float currval = 0.;
    const unsigned int sampsize = (unsigned int) samples.size();

    signal.resize(sampsize);
    for (unsigned int ii = 0; ii < sampsize; ++ii) {
       if (samples[ii]==0 || samples[ii]>=m_AdcMax) { //Check for saturation
          ATH_MSG_DEBUG("Saturation on channel 0x" << 
                        MSG::hex << id.get_compact() << MSG::dec << " ADC=" << samples[ii]);
          if ( m_skipSaturatedCells ) {
             ATH_MSG_DEBUG(" Skipping channel...");
             continue;
          }
          prov|=0x0400;
       }
       currval = (float)(samples[ii] - p);
       signal[ii]=currval;      
       if ((ii >= m_minSample)&&(ii <= m_maxSample)&&(currval > peakval)) { 
          ipeak = ii; peakval = currval; 
       }
    }
    ATH_MSG_DEBUG("Peak value: " << peakval << ", peak sample:" << ipeak);

    int nIteration = m_nIterProp;
    bool doIter=false;
    if (m_minADCforIterInSigma>0) {//threshold given in terms of pedestal-rms, get pedestal
       float vRMS=peds->pedestalRMS(id,gain);
       if (vRMS >= (1.0+LArElecCalib::ERRORCODE)) { 
	   if (peakval > (vRMS*m_minADCforIterInSigma)) doIter=true;//enough signal...
         }
         else { //no pedestal found, use adc threshold
	   if (peakval > m_minADCforIter) doIter=true;//enough signal...
         }
    } else {
      if (peakval >= m_minADCforIter) doIter=true;//enough signal...
    }

    if (!doIter) {//No iteration, insufficient signal
       nIteration=1;
       ipeak = m_defaultShiftTimeSamples + 2 ; 
    }

    if (ipeak > sampsize - 3) ipeak = sampsize - 3 ; 
    if (ipeak < 2) ipeak = 2;

    unsigned int peak_min = ipeak - 1 ; 
    unsigned int peak_max = ipeak + 1 ;

    float ADCPeak=0;
    float time=0.;

    const LArOFIterResults results = peak(signal, id, gain, m_defaultPhase, ofcs, shapes, 
					  nIteration, ipeak,peak_min, peak_max );
    if(outputTimingContainer) {
       outputTimingContainer->push_back(results);
    }
    if (results.getValid()) {
       ADCPeak = results.getAmplitude();
       // this should be ~0 if the peak is at curr_shiftTimeSamples
       // FIXME: this time definition still misses the tstart from the OFC to be absolutely computed
       time = (25.*((int)(results.getPeakSample_final())
              -2-m_defaultShiftTimeSamples)
              -(results.getDelay_final()-results.getTau()));

       ATH_MSG_DEBUG("Peak and time properly retrieved with OFPeakRecoTool: ADCPeak = " 
                     << ADCPeak <<", time = "<< time);
    } else {
       ATH_MSG_DEBUG(". OFC iteration not valid for channel 0x"<< MSG::hex << 
                     id.get_compact() << MSG::dec << " Gain = " << gain << 
                     ". Skipping channel.");
       continue;
    }

    //Apply Ramp
    float E=adc2mev[0]+ADCPeak*adc2mev[1];

    if (E>fMAXINT) E=fMAXINT;
    if (E<fMAXINT2) E=fMAXINT2;
    
    if (results.getConverged()) prov |= 0x0100;
    prov = prov & 0x3FFF;

    uint16_t iquaShort=0;
    float tau=0;


    //uint16_t prov=0xa5; //Means all constants from DB

    const float E1=m_absECutFortQ.value() ? std::fabs(E) : E;
    float ecut(0.);
    if (m_useDBFortQ) {
      if (run2DSPThresh) {
        ecut = run2DSPThresh->tQThr(id);
      }
      else if (run1DSPThresh) {
        ecut = run1DSPThresh->tQThr(id);
      }
      else {
        ATH_MSG_ERROR ("DSP threshold problem");
        return StatusCode::FAILURE;
      }
    }
    else {
      ecut = m_eCutFortQ;
    }

    if (E1 > ecut) { // fill also time and quality
      ATH_MSG_VERBOSE("Channel " << m_onlineId->channel_name(id) << " gain " << 
                      gain << " above threshold for tQ computation");
      prov|=0x2000; //  fill bit in provenance that time+quality information are available

      tau=time*(Gaudi::Units::nanosecond/Gaudi::Units::picosecond); //Convert time to ps
      if (tau>fMAXINT) tau=fMAXINT;
      if (tau<fMAXINT2) tau=fMAXINT2;

      //Get Q-factor

      int iqua = (int)(results.getQuality());
      if (iqua > 0xFFFF) iqua=0xFFFF;
      iquaShort = static_cast<uint16_t>(iqua & 0xFFFF);

    }//end if above cut

   
    outputContainer->emplace_back(id,static_cast<int>(std::floor(E+0.5)),
				  static_cast<int>(std::floor(tau+0.5)),
				  iquaShort,prov,(CaloGain::CaloGain)gain);
  }

  return StatusCode::SUCCESS;
}

LArOFIterResults LArRawChannelBuilderIterAlg::peak(const std::vector<float>& samples, 
                        const HWIdentifier chID, const CaloGain::CaloGain gain, const float delayIn,   
                        const ILArOFC* ofcs, const ILArShape* shapes, const unsigned  nIter,  
                        const unsigned npeak, unsigned peak_low, unsigned peak_high) const
{
  const float epsilon=0.001;
  const double samplingPeriod=1./(40.08*Gaudi::Units::megahertz);
  LArOFIterResults result;

  //Fill m_result with default/input values, 
  //calculation will be done with this object
  result.m_valid=false;
  result.m_converged=false;
  result.m_amplitude= 0;
  result.m_tau     =  0;
  result.m_quality =  0;
  result.m_delay_final      = delayIn;
  result.m_peakSample_init  =  npeak; 
  result.m_peakSample_final =  npeak; //Assumed index of highest sample (may change in the process)
  result.m_chid = chID;

  //Set some reference to improve readablity of the code:
  unsigned& kMax =  result.m_peakSample_final; //Make reference just to have code more readable 
  float& delay = result.m_delay_final;
  float& q=result.m_quality;
  unsigned& delayIdx=result.m_ofcIndex;
  //Quantities used during iteration
  unsigned kIter=0;
  //Computation is done as double
  double At=0;
  double A=0;

  //Tying to avoid doing all checks for every event/channel/iteation step by assuming that
  //the number of OFC samples is the same for all delays of a certain cell/gain. 
  //Code will segfault if not the case. 

  const unsigned nSamples=samples.size();
  // force uses of high gain if required for OFC and shape
  CaloGain::CaloGain usedGain = gain;
  if (m_forceHighGain) {
      if (m_onlineId->isHECchannel(chID)) usedGain = CaloGain::LARMEDIUMGAIN;
      else                                 usedGain = CaloGain::LARHIGHGAIN;
  }

  // Quantities depending on this cell
  const unsigned nOFCPhase=ofcs->nTimeBins(chID,usedGain);
  float timeOffset = ofcs->timeOffset(chID,usedGain);

  // convert delay to internal OFC delay (from 0 to Nphases*timeBinWidth)
  delay = delay-timeOffset;
 
  float timeBinWidth;
  float timeMax;
  if (nOFCPhase<2) { //Only one time bin
    delayIdx=0;
    timeBinWidth=25.; //ns
    timeMax=(nOFCPhase-1)*timeBinWidth;
  } else { //Have more than one OFC bin
    timeBinWidth=ofcs->timeBinWidth(chID,usedGain);
    timeMax =  (nOFCPhase-1)*timeBinWidth;
    if (timeBinWidth==0.) {
      ATH_MSG_ERROR( "timeBinWidth is zero for channel " << m_onlineId->channel_name(chID)  );
      return result;
    }
    //Check if initial delay isn't too big
    if (delay>timeMax) delay=timeMax-epsilon;
    if (delay<0.) delay=0.;
    //Index of the in in the vector according to the delay
    delayIdx=(unsigned)floor(0.5+delay/timeBinWidth);
  }

  //Get first set of OFC's
  ILArOFC::OFCRef_t this_OFC_a = ofcs->OFC_a(chID,(int)usedGain,delayIdx);
  ILArOFC::OFCRef_t this_OFC_b = ofcs->OFC_b(chID,(int)usedGain,delayIdx);
  const unsigned ofcSize=this_OFC_a.size(); //Assumed to be the same of all delay-indices

  //some sanity check on the OFCs
  if ( ofcSize == 0 || this_OFC_b.size() == 0 ) {
    ATH_MSG_DEBUG("OFC not found for channel " << m_onlineId->channel_name(chID));
    return result;
  }

  if ( this_OFC_a.size() != this_OFC_b.size() ) {
    ATH_MSG_ERROR( "OFC a (" << this_OFC_a.size() << 
                   ")and b (" << this_OFC_b.size() << ") are not the same size for channel 0x" 
                   << std::hex << chID.get_compact() << std::dec  );
    return result;
  }

  //Coerce kmax, peak_high and peak_low to someting that can work
  if (peak_low<2) peak_low=2; //By convention we expect at least 2 samples before the peak
  if (peak_high>(nSamples+2-ofcSize)) peak_high=(nSamples+2-ofcSize);
  if (peak_high<peak_low) {
    ATH_MSG_WARNING( "Channel 0x"  << std::hex << chID.get_compact() << std::dec 
                     << "Not enough ADC samples (" << nSamples << ") to apply " << ofcSize << " OFCs."  );
    return result;
  }
  if(kMax<peak_low)  kMax=peak_low; 
  if(kMax>peak_high) kMax=peak_high; 

  float amplitude_save=0.;
  float tau_save= 99999.;
  unsigned int kMax_save=0;
  float delay_save=0.;
  unsigned int delayIdx_save=0;

  unsigned int mynIter = nIter;

  do {
    
    // Uncomment the following if you suspect that the ofc are corrupt for some phases:
    /*  
    if ( this_OFC_a.size() == 0 || this_OFC_b.size() == 0 ) {
      ATH_MSG_DEBUG( "OFC not found for channel 0x"  << std::hex << chID.get_compact() << std::dec  );
      std::cout << "OFC not found for channel 0x"  << std::hex << chID.get_compact() << std::dec << std::endl;
      return result;
    }

    if ( this_OFC_a.size() != this_OFC_b.size() ) {
      ATH_MSG_ERROR( "OFC a (" << this_OFC_a.size() << 
	")and b (" << this_OFC_b.size() << ") are not the same size for channel 0x" 
	  << std::hex << chID.get_compact() << std::dec  );
      return result;
    }
    */
    
        
    //Apply Optimal Filtering coefficients
    A = At = 0 ;
    for ( unsigned k=0 ; (k<ofcSize); k++ ) {
      //for ( unsigned k=0 ; (k<ofcSize) && (kMax-2+k<nSamples); k++ ) {
      const float& this_sample = samples[kMax-2+k];
      A  += this_OFC_a.at(k) * this_sample ;
      At += this_OFC_b.at(k) * this_sample ;
    }
    //Validate the result
    result.m_valid = true; //Doesn't mean that the result is really good, but we have something
    if ( A == 0 ) {
      ATH_MSG_DEBUG("Null amplitude: " << A << "  for channel" << m_onlineId->channel_name(chID));
      result.m_amplitude=0;
      result.m_tau=0;
      return result;
    }
    result.m_amplitude=A; 
    result.m_tau = At / A ;

    //First iteration done, break loop if possible.... 
    if (mynIter<=1) {
        delay = delayIdx*timeBinWidth;
        break; //No iteration requested
    }

    // Nsamples=OFCsize and only one phase available, no point to iterate
    if (samples.size() == ofcSize && nOFCPhase<2) {
        delay = delayIdx*timeBinWidth;
        break;
    }

    // if we are within +-0.5*Dt of time bin, we have converged for sure
    if (fabs(result.m_tau) <= (0.5*timeBinWidth)) { 
      result.m_converged=true;
      delay = delayIdx*timeBinWidth;
      break;
    }

    if (kIter>=mynIter) { //Max. number of iterations reached
      delay = delayIdx*timeBinWidth;
      if (result.m_converged) {
        if (fabs(tau_save) < fabs(result.m_tau)) {
            result.m_amplitude = amplitude_save;
            result.m_tau       = tau_save;
            kMax                 = kMax_save;
            delay                = delay_save;
            delayIdx             = delayIdx_save;
        }
      }
      if (fabs(result.m_tau) <= timeBinWidth) result.m_converged=true;
      break;
    }

    // if we are within +-Dt of time bin, we consider that we have converged but we allow for one more
    // iteration to see if we can find a smaller tau, if not we keep the previous one
    if (fabs(result.m_tau) <= timeBinWidth) {
       result.m_converged = true;
       mynIter = kIter+1;    // allow only for more iteration
       amplitude_save = result.m_amplitude;
       tau_save       = result.m_tau;
       kMax_save      = kMax;
       delay_save     =  delayIdx*timeBinWidth;
       delayIdx_save  = delayIdx;
    }

    delay = delay - result.m_tau;  // moved this line up so first iteration delay results treated like subsequent

    if(delay<(-0.5*timeBinWidth)) { 
      if(kMax<peak_high){ 
	kMax = kMax+1 ; 
	delay=delay+samplingPeriod;
	if( delay < 0 ) delay = 0;
        if (delay > timeMax ) delay = timeMax-epsilon;
      } else { // don't shift sample
	delay = 0 ; 
      }	
    }//else if delay<0
    else
      if( delay>(timeMax+0.5*timeBinWidth) ) {
	if(kMax>peak_low){
	  kMax = kMax-1 ; 
	  delay=delay-samplingPeriod;
          if (delay < 0 ) delay=0.;
	  if( delay > timeMax ) delay = timeMax-epsilon;
	} else { 
	  // don't shift sample
	  delay = timeMax-epsilon;
	}	
      }//end if delay>nOFCPhase
    //Prepare next iteration step:
    kIter++;
    delayIdx=(unsigned)floor(0.5+delay/timeBinWidth);
    if (delayIdx>=nOFCPhase) delayIdx = nOFCPhase-1;
    //Get next set of OFC's
    this_OFC_a = ofcs->OFC_a(chID,(int)usedGain,delayIdx);
    this_OFC_b = ofcs->OFC_b(chID,(int)usedGain,delayIdx);
  }
  while(1);  // end for iteration loop 

  // go back to overal time
  delay = delay + timeOffset;  // sign to check

  q = 0.; 
  ILArShape::ShapeRef_t thisShape    = shapes->Shape(chID,(int)usedGain,delayIdx) ;
  ILArShape::ShapeRef_t thisShapeDer;
  if (m_useShapeDer) thisShapeDer = shapes->ShapeDer(chID,(int)usedGain,delayIdx) ;
  if( thisShape.size() >= ofcSize ) {
    for ( unsigned k=0 ; k<ofcSize ; k++ ) {
      const float& this_sample = samples[kMax-2+k];
       if (m_useShapeDer && thisShapeDer.size() >= ofcSize) 
          q += std::pow((result.m_amplitude*(thisShape[k]-result.m_tau*thisShapeDer[k]) - this_sample),2); 
      else
          q += std::pow((result.m_amplitude*thisShape[k] - this_sample),2);
    }
  }
  else {
    ATH_MSG_DEBUG("No shape for this channel");
  } 

  result.m_nIterPerf = kIter;
  result.m_valid    =    true;
   return result;

}
