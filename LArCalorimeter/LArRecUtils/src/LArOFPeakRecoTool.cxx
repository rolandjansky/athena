/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecUtils/LArOFPeakRecoTool.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "LArIdentifier/LArOnlineID.h"
#include <algorithm>
#include <cmath>


//Changes that affect the result: 
//set the nphases, stepwith, etc..
//set fabs(tau) <= timeBinWidth

/////////////////////////////////////////////////////////////////////////////////////////////

LArOFPeakRecoTool::LArOFPeakRecoTool(const std::string& type, const std::string& name, 
				     const IInterface* parent) : 
  AthAlgTool(type, name, parent)
  //m_nSamples(0xFFFFFFFF) //shall compare false to any possible number of samples
{
  declareProperty("Iterate",m_iterate=false) ;
  declareProperty("KeyOFC",m_keyOFC="LArOFC") ;
  declareProperty("KeyShape",m_keyShape="LArShape") ;
  declareProperty("UseShape",m_useShape=false,"Use the LArShape to compute quality factor");
  declareProperty("UseShapeDer",m_useShapeDer=true,"Use the shape derivative in the quality factor computation");
// DelayShift = different with delay run in 25/24 ns units
//   23 is the ad-hoc value for run 5640-5641
  declareProperty("DelayShift",m_delayShift=23);
  declareProperty("SamplingPeriod",m_samplingPeriod=1./(40.08*Gaudi::Units::megahertz));
  declareProperty("forceHighGain",m_forceHighGain=false,"Force use of high gain for all shapes and OFC (default=false)");
  declareInterface<LArOFPeakRecoTool>(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////

LArOFPeakRecoTool::~LArOFPeakRecoTool() {
}

StatusCode LArOFPeakRecoTool::initialize() {
  ATH_MSG_DEBUG("initializing LArOFPeakRecoTool...");
  
// call back for OFC 
  
//  sc = m_detStore->regFcn(&LArOFPeakRecoTool::LoadCalibration,this,m_dd_ofc,m_keyOFC);
  ATH_CHECK(  detStore()->regHandle(m_dd_ofc,m_keyOFC) );
  ATH_MSG_INFO( " register callback for OFC "  );

  if (m_useShape) {
    ATH_CHECK(  detStore()->regHandle(m_dd_shape,m_keyShape) );
  }
  else {
    ATH_MSG_WARNING( "jobOption 'UseShape' set to false. Will work without shape"  );
  }

  ATH_CHECK( detStore()->retrieve(m_lar_on_id,"LArOnlineID") );

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

const LArOFPeakRecoTool::Result& LArOFPeakRecoTool::peak(const std::vector<short>& samples, HWIdentifier chID,
							 CaloGain::CaloGain gain, int delay) {

  std::vector<float> newsamples;
  for(unsigned int i=0;i<samples.size();i++)
    {
      newsamples.push_back((float)samples[i]);
    }
  return peak(newsamples,chID,gain,delay);

}


/////////////////////////////////////////////////////////////////////////////////////////////////
const LArOFPeakRecoTool::Result& LArOFPeakRecoTool::peak(
			   const std::vector<float>& samples, // raw data after pedestal subtraction
			   const HWIdentifier chID,           // online channel id
			   const CaloGain::CaloGain gain,     // gain 
			   const float delayIn,               // initial delay for Shape and OFC  
			   const unsigned nIter,		  // number of iteration 
			   const unsigned npeak,            // initial peak position.                  
			   unsigned peak_low,         // lower limit for peak position
			   unsigned peak_high        // upper limit for peak position
			   ){

  float epsilon=0.001;

  //Fill m_result with default/input values, 
  //calculation will be done with this object
  m_result.m_valid=false;
  m_result.m_converged=false;
  m_result.m_amplitude= 0;
  m_result.m_tau     =  0;
  m_result.m_quality =  0;
  m_result.m_delay_final      = delayIn;
  m_result.m_peakSample_init  =  npeak; 
  m_result.m_peakSample_final =  npeak; //Assumed index of highest sample (may change in the process)
  m_result.m_chid = chID;
  //Set some reference to improve readablity of the code:
  unsigned& kMax =  m_result.m_peakSample_final; //Make reference just to have code more readable 
  float& delay = m_result.m_delay_final;
  float& q=m_result.m_quality;
  unsigned& delayIdx=m_result.m_ofcIndex;
  //Quantities used during iteration
  unsigned kIter=0;
  //Computation is done as double
  double At=0;
  double A=0;
  //std::cout << "***** ChannelID=" << (int)chID << " ; Gain=" << (int)gain << " ; Delay=" << delay << " nIter=" << nIter  << std::endl ;

  //Tying to avoid doing all checks for every event/channel/iteation step by assuming that
  //the number of OFC samples is the same for all delays of a certain cell/gain. 
  //Code will segfault if not the case. 

  const unsigned nSamples=samples.size();
  //if (samples.size()<5){ 
  //  msg() << MSG::WARNING << "Not enough ADC samples (" << nSamples << ") found for channel 0x"  
 //	     << std::hex << chID.get_compact() << std::dec << endmsg;
 //   return m_result;
 // }

  // force uses of high gain if required for OFC and shape
  CaloGain::CaloGain usedGain = gain;
  if (m_forceHighGain) {
      if (m_lar_on_id->isHECchannel(chID)) usedGain = CaloGain::LARMEDIUMGAIN;
      else                                 usedGain = CaloGain::LARHIGHGAIN;
  }

  // Quantities depending on this cell
  const unsigned nOFCPhase=m_dd_ofc->nTimeBins(chID,usedGain);
  float timeOffset = m_dd_ofc->timeOffset(chID,usedGain);

  // convert delay to internal OFC delay (from 0 to Nphases*timeBinWidth)
  delay = delay-timeOffset;
 
  float timeBinWidth;
  float timeMax;
  if (nOFCPhase<2) { //Only one time bin
    delayIdx=0;
    timeBinWidth=25.; //ns
    timeMax=(nOFCPhase-1)*timeBinWidth;
  } else { //Have more than one OFC bin
    timeBinWidth=m_dd_ofc->timeBinWidth(chID,usedGain);
    timeMax =  (nOFCPhase-1)*timeBinWidth;
    if (timeBinWidth==0.) {
      ATH_MSG_ERROR( "timeBinWidth is zero for channel " << m_lar_on_id->channel_name(chID)  );
      return m_result;
    }
    //Check if initial delay isn't too big
    if (delay>timeMax) delay=timeMax-epsilon;
    if (delay<0.) delay=0.;
    //Index of the in in the vector according to the delay
    delayIdx=(unsigned)floor(0.5+delay/timeBinWidth);
  }

  //std::cout << "Timebinwidth=" << timeBinWidth << " nOFCPhase=" << nOFCPhase << std::endl; 
  //std::cout << "Delay= " << delay << " Index=" << delayIdx << std::endl;
  //Get first set of OFC's
  ILArOFC::OFCRef_t this_OFC_a = m_dd_ofc->OFC_a(chID,(int)usedGain,delayIdx);
  ILArOFC::OFCRef_t this_OFC_b = m_dd_ofc->OFC_b(chID,(int)usedGain,delayIdx);
  const unsigned ofcSize=this_OFC_a.size(); //Assumed to be the same of all delay-indices
  //std::cout << " got OFC " << this_OFC_a.size() << " " << this_OFC_b.size() << std::endl;

  //some sanity check on the OFCs
  if ( ofcSize == 0 || this_OFC_b.size() == 0 ) {
    ATH_MSG_DEBUG("OFC not found for channel " << m_lar_on_id->channel_name(chID));
    return m_result;
  }

  if ( this_OFC_a.size() != this_OFC_b.size() ) {
    ATH_MSG_ERROR( "OFC a (" << this_OFC_a.size() << 
                   ")and b (" << this_OFC_b.size() << ") are not the same size for channel 0x" 
                   << std::hex << chID.get_compact() << std::dec  );
    return m_result;
  }

  //Coerce kmax, peak_high and peak_low to someting that can work
  if (peak_low<2) peak_low=2; //By convention we expect at least 2 samples before the peak
  if (peak_high>(nSamples+2-ofcSize)) peak_high=(nSamples+2-ofcSize);
  if (peak_high<peak_low) {
    ATH_MSG_WARNING( "Channel 0x"  << std::hex << chID.get_compact() << std::dec 
                     << "Not enough ADC samples (" << nSamples << ") to apply " << ofcSize << " OFCs."  );
    return m_result;
  }
  if(kMax<peak_low)  kMax=peak_low; 
  if(kMax>peak_high) kMax=peak_high; 

  /*
  std::cout << "Samples: ";
  for ( unsigned k=0;k<samples.size();k++) 
    std::cout << " " << samples[k];
  std::cout << std::endl;
  std::cout << "OFC_a: ";
  for ( unsigned k=0;k<this_OFC_a.size();k++) 
    std::cout << " " << this_OFC_a.at(k);
  std::cout << std::endl;
  */

  float amplitude_save=0.;
  float tau_save= 99999.;
  unsigned int kMax_save=0;
  float delay_save=0.;
  unsigned int delayIdx_save=0;

  unsigned int mynIter = nIter;

  do {
    //std::cout << "*** Start iteration step: "<< kIter << " kmax=" <<kMax << " delay=" << delay << std::endl;
    
    // Uncomment the following if you suspect that the ofc are corrupt for some phases:
    /*  
    if ( this_OFC_a.size() == 0 || this_OFC_b.size() == 0 ) {
      ATH_MSG_DEBUG( "OFC not found for channel 0x"  << std::hex << chID.get_compact() << std::dec  );
      std::cout << "OFC not found for channel 0x"  << std::hex << chID.get_compact() << std::dec << std::endl;
      return m_result;
    }

    if ( this_OFC_a.size() != this_OFC_b.size() ) {
      ATH_MSG_ERROR( "OFC a (" << this_OFC_a.size() << 
	")and b (" << this_OFC_b.size() << ") are not the same size for channel 0x" 
	  << std::hex << chID.get_compact() << std::dec  );
      return m_result;
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
    m_result.m_valid = true; //Doesn't mean that the result is really good, but we have something
    if ( A == 0 ) {
      ATH_MSG_DEBUG("Null amplitude: " << A << "  for channel" << m_lar_on_id->channel_name(chID));
      m_result.m_amplitude=0;
      m_result.m_tau=0;
      return m_result;
    }
    m_result.m_amplitude=A; 
    m_result.m_tau = At / A ;
    //std::cout << "Iteration step: " <<  kIter << " Delay=" << delay << " A = " << A << " tau=" << m_result.m_tau << std::endl;

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
    if (fabs(m_result.m_tau) <= (0.5*timeBinWidth)) { 
      m_result.m_converged=true;
     // std::cout << "Converged." << std::endl;
      delay = delayIdx*timeBinWidth;
      break;
    }

    if (kIter>=mynIter) { //Max. number of iterations reached
      //std::cout << "Did not converge after " << nIter << " iterations." << std::endl;
      delay = delayIdx*timeBinWidth;
      if (m_result.m_converged) {
        if (fabs(tau_save) < fabs(m_result.m_tau)) {
            m_result.m_amplitude = amplitude_save;
            m_result.m_tau       = tau_save;
            kMax                 = kMax_save;
            delay                = delay_save;
            delayIdx             = delayIdx_save;
        }
      }
      if (fabs(m_result.m_tau) <= timeBinWidth) m_result.m_converged=true;
      break;
    }

    // if we are within +-Dt of time bin, we consider that we have converged but we allow for one more
    // iteration to see if we can find a smaller tau, if not we keep the previous one
    if (fabs(m_result.m_tau) <= timeBinWidth) {
       m_result.m_converged = true;
       mynIter = kIter+1;    // allow only for more iteration
       amplitude_save = m_result.m_amplitude;
       tau_save       = m_result.m_tau;
       kMax_save      = kMax;
       delay_save     =  delayIdx*timeBinWidth;
       delayIdx_save  = delayIdx;
    }

    //std::cout << "not yet converged" << std::endl ;
    delay = delay - m_result.m_tau;  // moved this line up so first iteration delay results treated like subsequent

    if(delay<(-0.5*timeBinWidth)) { 
      if(kMax<peak_high){ 
	kMax = kMax+1 ; 
	delay=delay+m_samplingPeriod;
	if( delay < 0 ) delay = 0;
        if (delay > timeMax ) delay = timeMax-epsilon;
	//std::cout  << " delay too small, shift sample, set delay to " << delay << std::endl ;
      } else { // don't shift sample
	//std::cout << " delay too small, set delay to 0 "<< std::endl ;
	delay = 0 ; 
      }	
    }//else if delay<0
    else
      if( delay>(timeMax+0.5*timeBinWidth) ) {
	if(kMax>peak_low){
	  kMax = kMax-1 ; 
	  delay=delay-m_samplingPeriod;
          if (delay < 0 ) delay=0.;
	  if( delay > timeMax ) delay = timeMax-epsilon;
	  //std::cout << " delay too large , shift sample "<< std::endl ;
	} else { 
	  // don't shift sample
	  //std::cout << " delay too large set delay to max "<< std::endl ;
	  delay = timeMax-epsilon;
	}	
      }//end if delay>nOFCPhase
    //Prepare next iteration step:
    kIter++;
    delayIdx=(unsigned)floor(0.5+delay/timeBinWidth);
    if (delayIdx>=nOFCPhase) delayIdx = nOFCPhase-1;
    //std::cout << " new kMax, delay,delayIdx " << kMax << " " << delay << " " << delayIdx << std::endl;
    //Get next set of OFC's
    this_OFC_a = m_dd_ofc->OFC_a(chID,(int)usedGain,delayIdx);
    this_OFC_b = m_dd_ofc->OFC_b(chID,(int)usedGain,delayIdx);
  }
  while(1);  // end for iteration loop 

  // go back to overal time
  delay = delay + timeOffset;  // sign to check

  //std::cout << "Done! A= " << m_result.m_amplitude << " ; tau= " << m_result.m_tau << " kIter=" << kIter << " nIter=" << nIter << std::endl;
  // get shape
  if (m_useShape){
    q = 0.; 
    ILArShape::ShapeRef_t thisShape    = m_dd_shape->Shape(chID,(int)usedGain,delayIdx) ;
    ILArShape::ShapeRef_t thisShapeDer;
    if (m_useShapeDer) thisShapeDer = m_dd_shape->ShapeDer(chID,(int)usedGain,delayIdx) ;
    if( thisShape.size() >= ofcSize ) {
      for ( unsigned k=0 ; k<ofcSize ; k++ ) {
	const float& this_sample = samples[kMax-2+k];
         if (m_useShapeDer && thisShapeDer.size() >= ofcSize) 
	    q += std::pow((m_result.m_amplitude*(thisShape[k]-m_result.m_tau*thisShapeDer[k]) - this_sample),2); 
        else
            q += std::pow((m_result.m_amplitude*thisShape[k] - this_sample),2);
      }
    }
    else {
      ATH_MSG_DEBUG("No shape for this channel");
    } 
  }

  m_result.m_nIterPerf = kIter;
  m_result.m_valid    =    true;
  return m_result;
}
