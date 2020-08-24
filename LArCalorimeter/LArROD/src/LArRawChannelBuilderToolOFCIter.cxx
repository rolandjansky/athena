/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/* if pool is wanted uncomment this
#include "DataModel/DataPool.h"
#define private public
#define protected public
#include "LArRawEvent/LArRawChannel.h"
#undef private
#undef protected
*/

#include "LArROD/LArRawChannelBuilderToolOFCIter.h"
#include "LArROD/LArRawChannelBuilderDriver.h"
#include "GaudiKernel/MsgStream.h"

#include "LArRawEvent/LArDigit.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <math.h>

using CLHEP::ns;
using CLHEP::nanosecond;
using CLHEP::picosecond;

#define MAXINT 2147483000
#define MAXINT2 -2147483000

LArRawChannelBuilderToolOFCIter::LArRawChannelBuilderToolOFCIter(const std::string& type,
							     const std::string& name,
							     const IInterface* parent):
  LArRawChannelBuilderToolBase(type,name,parent),
  m_larOFIterCont(NULL),
  m_storeGate("StoreGateSvc", name),
  m_peakReco("LArOFPeakRecoTool") {
  m_helper = new LArRawChannelBuilderStatistics( 3,      // number of possible errors
					       0x05);  // bit pattern special for this tool,
                                                       // to be stored in "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "channel saturated");
  m_helper->setErrorString(2, "OFC not valid");
  
  declareProperty("ADCMax",                    m_AdcMax=4095);
  declareProperty("Skip",                      m_skipSaturatedCells=false);
  declareProperty("ECut",                      m_Ecut=50);
  declareProperty("doQual",                    m_doQual=true);
  declareProperty("doIter",                    m_doIter=true);
  declareProperty("defaultPhase",              m_defaultPhase=0);
  declareProperty("minADCforIter",             m_minADCforIter=30);
  declareProperty("minADCforIterInSigma",      m_minADCforIterInSigma=-1);
  declareProperty("minSample",                 m_minSample=0);
  declareProperty("maxSample",                 m_maxSample=31);
  declareProperty("nIterations",               m_nIterProp=10);
  declareProperty("StoreTiming",               m_storeTiming=false,
		  "Create container of LArOFIterResults in StoreGate");
  declareProperty("TimingContainerKey",        m_timingContKey="LArOFIterResult",
		  "Key of the LArOFIterResultsContainer in StoreGate");
  declareProperty("PedestalRMSKey",            m_pedestalKey,
		  "SG key for pedestal object. Needed only if 'minADCforIterInSigma' is set (for the RMS)");
  declareProperty("doMC",                      m_doMC=false,
                  " take noise from LArNoise instead of LArPedestal");

}

StatusCode LArRawChannelBuilderToolOFCIter::initTool()
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc=m_peakReco.retrieve();
  
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve LArOFPeakRecoTool" <<endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = m_storeGate.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "StoreGate service not found" << endmsg;
    return sc;
  }

  if (m_minADCforIterInSigma>0) {//threshold given in terms of pedestal-rms, get pedestal
    if (m_doMC) {
      if (detStore()->regHandle(m_larNoise,"LArNoise").isFailure()) {
         log << MSG::ERROR << " no LArNoise found " << endmsg;
         m_minADCforIterInSigma=-1;
      }
      else
        log << MSG::INFO << " Min ADC for iteration " << m_minADCforIterInSigma << "* LArNoise " <<endmsg;
    }
    else {
      if (detStore()->regHandle(m_larPedestal,m_pedestalKey).isFailure()) {
        log << MSG::ERROR << "No pedestals with key <" << m_pedestalKey << "> found in DetectorStore." 
  	    << "Will only use property minADCforIter ("<<m_minADCforIter <<") as iteration threshold."
  	    << endmsg;
        m_minADCforIterInSigma=-1;
      }
      else
        log << MSG::INFO << " Min ADC for iteration " << m_minADCforIterInSigma << "* pedestalRMS " <<endmsg;
    }
  }
  else
    log << MSG::INFO << " Min ADC for iteration "<<m_minADCforIter <<endmsg;

  log << MSG::INFO << " DefaultPhase  "<<m_defaultPhase <<endmsg;
  log << MSG::INFO << " Min and Max Sample "<<m_minSample<< " "<<m_maxSample<<endmsg;


  return StatusCode::SUCCESS;
}

void LArRawChannelBuilderToolOFCIter::initEvent() {
  //std::cout << "in  LArRawChannelBuilderToolOFCIter::initEvent()" << std::endl;
  if (m_storeTiming) {
    m_larOFIterCont= new LArOFIterResultsContainer;
    StatusCode sc=m_storeGate->record(m_larOFIterCont,m_timingContKey);
    if (sc.isFailure()) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Failed to record a LArOFIterResultsContainer with key " << m_timingContKey << " to StoreGate." << endmsg;
      delete m_larOFIterCont;
      m_larOFIterCont=NULL;
    }
  }
}


bool LArRawChannelBuilderToolOFCIter::buildRawChannel(const LArDigit* digit,
						      float pedestal,
						      const std::vector<float>& ramps,
						      MsgStream* pLog )
{
  const HWIdentifier chid=m_parent->curr_chid;
  const CaloGain::CaloGain gain=m_parent->curr_gain;

  uint16_t iprovenance=0;

  bool debugPrint=false;
  if (bool(pLog)) {
    if (pLog->level() <= MSG::DEBUG) debugPrint=true;
  }

  if(debugPrint)
    (*pLog) << MSG::DEBUG << "Start " <<MSG::hex<< chid.get_compact() <<MSG::dec<< endmsg;


  // Loop over samples, to find maximum, check for saturation and subtract pedestal
  const std::vector<short>& samples = digit->samples();
  unsigned int sampsize = (unsigned int) samples.size();
  float peakval = -999.;
  unsigned int ipeak = 0;  
  //static std::vector<float> signal(sampsize); //Pedestal-subtracted
  m_signal.resize(sampsize);
  float currval = 0.;
  for (unsigned int ii = 0; ii < sampsize; ii++) {
    if (samples[ii]==0 || samples[ii]>=m_AdcMax) { //Check for saturation
      if(debugPrint) (*pLog) << MSG::DEBUG 
			     << "Saturation on channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
			     << " ADC=" << samples[ii];
      if ( m_skipSaturatedCells ) {
	  if(debugPrint) (*pLog) << " Skipping channel." << endmsg; 
	  m_helper->incrementErrorCount(1);
	  return false;
      }
      else
	if(debugPrint) (*pLog) << endmsg;

      iprovenance = iprovenance | 0x0400;
    } //end if saturated
    currval = (float)(samples[ii] - pedestal);
    m_signal[ii]=currval;
    if ((ii >= m_minSample)&&(ii <= m_maxSample)&&(currval > peakval)) { ipeak = ii; peakval = currval; }
  }

  if(debugPrint) (*pLog) << MSG::DEBUG << "Peak value: " << peakval << ", peak sample:" << ipeak << endmsg;

  int nIteration = m_nIterProp;
  bool doIter=false;
  if (m_doIter) {
    if (m_minADCforIterInSigma>0) {//threshold given in terms of pedestal-rms, get pedestal
       if (m_doMC) {
         float sigma = m_larNoise->noise(chid,gain);
         if (peakval > (sigma*m_minADCforIterInSigma)) doIter=true;
       }
       else {
         float vRMS=m_larPedestal->pedestalRMS(chid,gain);
         if (vRMS >= (1.0+LArElecCalib::ERRORCODE)) { 
	   if (peakval > (vRMS*m_minADCforIterInSigma)) doIter=true;//enough signal...
         }
         else { //no pedestal found, use adc threshold
	   if (peakval > m_minADCforIter) doIter=true;//enough signal...
         }
       }
    }//end if threshold given in terms of pedestal rms
    else
      if (peakval >= m_minADCforIter) doIter=true;//enough signal...
  }//end if doIter


  if (!doIter) {//No iteration, insufficient signal
    nIteration=1;
    ipeak = m_parent->curr_shiftTimeSamples + 2 ; 
  }


  if (ipeak > sampsize - 3) ipeak = sampsize - 3 ; 
  if (ipeak < 2) ipeak = 2; 


  unsigned int peak_min = ipeak - 1 ; 
  unsigned int peak_max = ipeak + 1 ; 

  float ADCPeak=0;
  float time=0.;
  const LArOFPeakRecoTool::Result results = m_peakReco->peak(m_signal, chid, gain, 
							     m_defaultPhase, 
							     nIteration,
							     ipeak,peak_min, peak_max 
							     );

  if (m_larOFIterCont) {
    //the iteration works always on teh same copy of the results object. Need to do a deep-copy in order to publish...
    //LArOFIterResults* res=new LArOFIterResults(results);
    m_larOFIterCont->push_back(results);
  }
  if (results.getValid()) {
    ADCPeak = results.getAmplitude();
    // this should be ~0 if the peak is at curr_shiftTimeSamples

    // FIXME: this time definition still misses the tstart from the OFC to be absolutely computed
    time = (25.*((int)(results.getPeakSample_final())-2-m_parent->curr_shiftTimeSamples)-(results.getDelay_final()-results.getTau()));
    //log << MSG::DEBUG << "Peak and time properly retrieved with OFPeakRecoTool:";
    //log << MSG::DEBUG << "ADCPeak = " << ADCPeak <<", time = "<< time << endmsg;
  }
  else {
  //  log << MSG::DEBUG << ". OFC iteration not valid for channel 0x"
  // 	<< MSG::hex << chid.get_compact() << MSG::dec 
  // 	<< " Gain = " << gain << ". Skipping channel." << endmsg;
    m_helper->incrementErrorCount(2);
    return false;
  }

  float power=1;
  float energy=0;
  for( unsigned int i=0; i<ramps.size(); i++)
    {
      energy += ramps[i] * power;
      power  *= ADCPeak;
    }
  


  //log << MSG::DEBUG << "ADCPeak = " << ADCPeak <<", time = "<< time<<
  //	" energy "<<energy <<endmsg;

  uint16_t iquality=0;

  if ((m_doQual) && (std::fabs(energy)>m_Ecut) && (results.getValid()))  {
    int iqua = (int)(results.getQuality());
    if (iqua > 0xFFFF) iqua=0xFFFF;
    iquality = (uint16_t)(iqua & 0xFFFF);
    iprovenance = iprovenance | 0x2000;
  }
  

  //Reminder: Bit-pattern 
  //ppcc bbbb sqqq qqqq qqqq

  iprovenance |= (m_parent->qualityBitPattern & 0x00FF);
  iprovenance |= (m_helper->returnBitPattern() & 0x00FF);
  if (results.getConverged())
    iprovenance |= 0x0100;
  iprovenance = iprovenance & 0x3FFF;

  time=time*(nanosecond/picosecond); //Convert time to ps

  const float fMAXINT = static_cast<float>(MAXINT);
  const float fMAXINT2 = static_cast<float>(MAXINT2);

  if (time>fMAXINT) time=fMAXINT;
  if (time<fMAXINT2) time=fMAXINT2;

  if (energy>fMAXINT) energy=fMAXINT;
  if (energy<fMAXINT2) energy=fMAXINT2;


  (this->*m_buildIt)((int)(floor(energy+0.5)),(int)floor(time+0.5),iquality,iprovenance,digit);
   
  
  m_helper->incrementErrorCount(0);

  
  return true;
}
