/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelBuilder.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBPhase.h"

#include "LArElecCalib/ILArPedestal.h"
//#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArGlobalTimeOffset.h"
#include "LArElecCalib/ILArFEBTimeOffset.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include <math.h>

using CLHEP::MeV;
using CLHEP::megahertz;
using CLHEP::nanosecond;
using CLHEP::picosecond;

LArRawChannelBuilder::LArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_OFCTool("LArOFCTool"),
  m_adc2mevTool("LArADC2MeVTool/LArADC2MeVToolDefault"),
  m_hvCorrTool("LArHVCorrTool"),
  m_onlineHelper(NULL),
  //m_roiMap("LArRoI_Map"),
  m_DataLocation("FREE"),
  m_ChannelContainerName("LArRawChannels"),
  m_useTDC(false),
  m_useOFCTool(false),
  m_Ecut(256*MeV),
  m_initialTimeSampleShift(0),
  m_ramp_max(),
  m_noEnergy(0),
  m_noTime(0),
  m_noShape(0),
  m_noShapeDer(0),
  m_saturation(0),
  m_lastNoEnergy(0),
  m_lastNoTime(0),
  m_lastNoShape(0),
  m_lastNoShapeDer(0),
  m_aveNoEnergy(0),
  m_aveNoTime(0),
  m_aveNoShape(0),
  m_aveNoShapeDer(0),
  m_aveSaturCells(0),
  m_nEvents(0),
  m_aveChannels(0),
  m_SamplingPeriodeUpperLimit(0),
  m_SamplingPeriodeLowerLimit(0),
  m_emId(NULL),
  m_firstSample(0)
  , m_pedestalKey("LArPedestal")
  , m_shapesKey("LArShape")
 {
   //m_useIntercept={false,false,false,false};
 declareProperty("LArRawChannelContainerName",m_ChannelContainerName);
 declareProperty("DataLocation",              m_DataLocation );
 declareProperty("UseTDC",                    m_useTDC);
 declareProperty("UseOFCTool",                m_useOFCTool);
 declareProperty("Ecut",                      m_Ecut);
 declareProperty("UseHighGainRampIntercept",  m_useIntercept[CaloGain::LARHIGHGAIN]=false);
 declareProperty("UseMedGainRampIntercept",   m_useIntercept[CaloGain::LARMEDIUMGAIN]=false);
 declareProperty("UseLowGainRampIntercept",   m_useIntercept[CaloGain::LARLOWGAIN]=false);
 declareProperty("InitialTimeSampleShift",    m_initialTimeSampleShift);
 declareProperty("NOFCTimeBins",              m_NOFCTimeBins=25); //Number of OFC time bins in a sampling periode
 m_NOFCPhases = m_NOFCTimeBins; 
 declareProperty("NOFCPhases",                m_NOFCPhases);      //Total number of available OFC sets
 declareProperty("UseOFCPhase",               m_useOFCPhase=false);
 declareProperty("PhaseInversion",            m_phaseInv=false);
 declareProperty("SamplingPeriod",            m_SamplingPeriode=1/(40.08*megahertz));
 declareProperty("OFCTimeBin",                m_OFCTimeBin=m_SamplingPeriode/m_NOFCTimeBins);
 declareProperty("BinHalfOffset",             m_binHalfOffset=false);
 declareProperty("AllowTimeSampleJump",       m_allowTimeJump=true);
 declareProperty("PedestalFallbackMode",      m_pedestalFallbackMode=0); // 0=only DB, 1=Only if missing,
 declareProperty("PedestalSample",            m_iPedestal=0);            // 2=Low, 3=Low+Me dium, 4=All LAr
 declareProperty("ShapeMode",                 m_shapeMode=0); 
 declareProperty("SkipSaturCellsMode",        m_skipSaturCells=0);
 declareProperty("ADCMax",                    m_AdcMax=4095);
 declareProperty("HVcorr",                    m_hvcorr=false);
 declareProperty("ADC2MeVTool", 	      m_adc2mevTool);
 declareProperty("OFCTool",                   m_OFCTool);
 declareProperty("firstSample",               m_firstSample,"  first sample used in shape");
 declareProperty("PedestalKey",		      m_pedestalKey);
 declareProperty("ShapesKey",		      m_shapesKey);
}


StatusCode LArRawChannelBuilder::initialize(){

  MsgStream log(msgSvc(), name());

  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_useOFCTool) {
    sc = m_OFCTool.retrieve();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to find tool for LArOFCTool" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  sc = m_adc2mevTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to find tool for LArADC2MeV" << endmsg;
    return StatusCode::FAILURE;
  }
  
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId) {
    log << MSG::ERROR << "Could not get lar EM ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_cablingKey.initialize() );

  if (m_hvcorr) {
    sc = m_hvCorrTool.retrieve();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to find tool for LArHVCorrTool" << endmsg;
      return StatusCode::FAILURE;
    }
  }


  //Set counters for errors and warnings to zero
  m_noEnergy   = 0; // Number of events with at least completly failed channel
  m_noTime     = 0; // Number of events with at least one channel without time info
  m_noShape    = 0; // Number of events with at least one channel without Shape (=with not quality factor);
  m_noShapeDer = 0; // Number of events with at least one channel without ShapeDerivative (=not taken into accout for quality factor);
  m_saturation = 0; // Number of events with at least one saturating channel
  
  m_lastNoEnergy   = -1; // Number of completly failed channels in previous event
  m_lastNoTime     = -1; // Number of channels without time info in previous event
  m_lastNoShape    = -1; // Number of channels without Shape (=with not quality factor) in previous event
  m_lastNoShapeDer = -1; // Number of channels without ShapeDerivative in previous event
  
  //m_lastSaturCells = -1; // Number of saturating channels without in previous event (not used)
  
  m_aveNoEnergy    = 0.; // Average number of completly failed channels per event
  m_aveNoTime      = 0.; // Average number of channels without time info per event
  m_aveNoShape     = 0.; // Average number of channels without Shape (=with not quality factor) per event
  m_aveNoShapeDer  = 0.; // Average number of channels without ShapeDerivative per event
  m_aveSaturCells  = 0.; // Average number of saturating channels without per event
  
  m_nEvents        = 0 ; // Total number of processed events ;
  m_aveChannels    = 0 ; // Average number of readout channels per event

  if ( m_skipSaturCells > 2 ) m_skipSaturCells = 0 ;

  m_ramp_max[CaloGain::LARHIGHGAIN]   = 500.;
  m_ramp_max[CaloGain::LARMEDIUMGAIN] = 5000.;
  m_ramp_max[CaloGain::LARLOWGAIN]    = 50000.;

  // Validity range for a set of OFC's. If the time shift is larger than this number,
  // we make a ADC sample jump (e.g. from [0,5] to [1,6]. The second half of the uppermost 
  // bin should already be rounded to the 0th bin of the following ADC sample.  
  if ( m_binHalfOffset ) {
    m_SamplingPeriodeUpperLimit = m_SamplingPeriode-m_SamplingPeriode/(2*m_NOFCTimeBins);
    m_SamplingPeriodeLowerLimit = -m_SamplingPeriode/(2*m_NOFCTimeBins);
  } else {
    m_SamplingPeriodeUpperLimit = m_SamplingPeriode;
    m_SamplingPeriodeLowerLimit = 0;
  }

  log << MSG::DEBUG << "Number of OFC time bins per sampling periode=" << m_NOFCTimeBins << endmsg;
  log << MSG::DEBUG << "Sampling Periode=" << m_SamplingPeriode << "ns" << endmsg;
  log << MSG::DEBUG << "Sampling Periode Limits: (" << m_SamplingPeriodeLowerLimit
      << "," << m_SamplingPeriodeUpperLimit << ") ns" << endmsg;

  return StatusCode::SUCCESS;
}



StatusCode LArRawChannelBuilder::execute() 
{
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  
  //Counters for errors & warnings per event
  int noEnergy   = 0; // Number of completly failed channels in a given event
  int BadTiming  = 0; // Number of channels with bad timing in a given event
  int noTime     = 0; // Number of channels without time info in a given event
  int noShape    = 0; // Number of channels without Shape (= with no quality factor) in a given event 
  int noShapeDer = 0; // Number of channels without ShapeDerivative in a given event 
  int highE      = 0; // Number of channels with 'high' (above threshold) energy in a given event 
  int saturation = 0; // Number of saturating channels in a given event   
  
  //Pointer to input data container
  const LArDigitContainer* digitContainer=NULL;//Pointer to LArDigitContainer
  //const TBPhase* theTBPhase; //Pointer to Testbeam TDC-Phase object (if needed)
  float PhaseTime=0;                //Testbeam TDC phase (if needed)
  float globalTimeOffset=0;
  //Pointer to conditions data objects 
  const ILArFEBTimeOffset* larFebTimeOffset=NULL;
  const ILArPedestal* larPedestal=NULL;
  const ILArOFC* larOFC=NULL;
  const ILArShape* larShape=NULL;
  //Retrieve Digit Container

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
    return StatusCode::FAILURE;
  }

  sc=evtStore()->retrieve(digitContainer,m_DataLocation);
  if(sc.isFailure()) {
    log << MSG::ERROR << "Can't retrieve LArDigitContainer with key " <<m_DataLocation << "from StoreGate." << endmsg;
    return StatusCode::FAILURE;
  }
  
  //Retrieve calibration data
  sc=detStore()->retrieve(larPedestal,m_pedestalKey);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Can't retrieve LArPedestal from Conditions Store " << m_pedestalKey << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc=detStore()->retrieve(larShape,m_shapesKey);
  if (sc.isFailure()) {
    log << MSG::WARNING << "Can't retrieve LArShape from Conditions Store" << std::endl
	<< "Quality factor will not be caluclated." << endmsg;
    larShape=NULL;
  }
  log << MSG::DEBUG << "Retrieving LArOFC object" << endmsg;
  if (!m_useOFCTool) {  //OFC-Conditons object only needed if OFC's not computed on-the-fly
    sc=detStore()->retrieve(larOFC);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Can't retrieve LArOFC from Conditions Store" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  //retrieve TDC
  if (m_useTDC) { //All this timing business is only necessary if the readout and the beam are not in phase (Testbeam)
    const TBPhase* theTBPhase;
    const ILArGlobalTimeOffset* larGlobalTimeOffset;
    sc = evtStore()->retrieve(theTBPhase,"TBPhase");
    if (sc.isFailure()) {
      log << MSG::ERROR << "cannot allocate TBPhase with key <TBPhase>"<< endmsg;
      //return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
    }
    //Get Phase in nanoseconds
    PhaseTime = theTBPhase->getPhase();
    // ###
    if (m_phaseInv) PhaseTime = m_SamplingPeriode - PhaseTime ;
    log << MSG::DEBUG << " *** Phase = " << PhaseTime << endmsg;
    // ###
    
    //Get Global Time Offset
    sc=detStore()->retrieve(larGlobalTimeOffset);
    if (sc.isSuccess()) globalTimeOffset = larGlobalTimeOffset->TimeOffset();

    //Get FEB time offset
    sc=detStore()->retrieve(larFebTimeOffset);
    if (sc.isFailure()) larFebTimeOffset=NULL;
  }


  LArRawChannelContainer* larRawChannelContainer=new LArRawChannelContainer();
  larRawChannelContainer->reserve(digitContainer->size());
  sc = evtStore()->record(larRawChannelContainer,m_ChannelContainerName);
  if(sc.isFailure()) {
    log << MSG::ERROR << "Can't record LArRawChannelContainer in StoreGate" << endmsg;
  }

  // Average number of LArDigits per event
  m_nEvents++;
  m_aveChannels += digitContainer->size();

  bool verbosePrint=false;
  if (log.level() <= MSG::VERBOSE) verbosePrint=true;
  bool debugPrint=false;
  if (log.level() <= MSG::DEBUG) debugPrint=true;

  // Now all data is available, start loop over Digit Container
  LArDigitContainer::const_iterator cont_it=digitContainer->begin();
  LArDigitContainer::const_iterator cont_it_e=digitContainer->end();
  int ntot_raw=0;

  for (;cont_it!=cont_it_e;cont_it++) {

    //Data that goes into RawChannel:
    float energy=0;
    float time=0;
    float quality=0;

    int OFCTimeBin=0;
    int timeSampleShift=m_initialTimeSampleShift;

    //Get data from LArDigit
    const std::vector<short>& samples=(*cont_it)->samples();
    const unsigned nSamples=samples.size(); 
    const HWIdentifier chid=(*cont_it)->channelID();
    const CaloGain::CaloGain gain=(*cont_it)->gain();
    
    // to be used in case of DEBUG output
    int layer  = -99999 ;
    int eta    = -99999 ; 
    int phi    = -99999 ; 
    int region = -99999 ;    
    if (debugPrint) {
      Identifier id ;
      try {
        id = cabling->cnvToIdentifier(chid);
      } catch ( LArID_Exception & except ) {
        log << MSG::DEBUG << "A Cabling exception was caught for channel 0x!" 
  	    << MSG::hex << chid.get_compact() << MSG::dec << endmsg;
        continue ;
      }
      layer  = m_emId->sampling(id);
      eta    = m_emId->eta(id); 
      phi    = m_emId->phi(id);
      region = m_emId->region(id);    
      //log << MSG::VERBOSE << "Channex 0x" << MSG::hex << chid.get_compact() << MSG::dec 
      //                    << " [ Layer = " << layer << " - Eta = " << eta 
      //			  << " - Phi = " << phi << " - Region = " << region << " ] " << endmsg ;
    }
        
    // check for saturation, in case skip channel
    int nSatur=-1 ;
    for (unsigned iSample=0;iSample<samples.size();iSample++) {
      if (samples[iSample]>=m_AdcMax) {
        nSatur++;
        break ;
      }
    }
    if ( nSatur>-1 ) {
      if (debugPrint)log << MSG::DEBUG << "Saturation on channel 0x" << MSG::hex << chid.get_compact() << MSG::dec ;         
      saturation++;
    }
    if ( m_skipSaturCells && nSatur>-1 ) {
      log << ". Skipping channel." << endmsg; 
      continue; // Ignore this cell, saturation on at least one sample
    } else if ( nSatur>-1 ) {
      log << "." << endmsg;
    }   
    
    //Get conditions data for this channel:

    // Pedestal
    float pedestal=larPedestal->pedestal(chid,gain);
    float pedestalAverage;
    if (pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
      if( m_pedestalFallbackMode >= 1 ) {
        if (debugPrint) {
         log << MSG::DEBUG << "No pedestal found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
              << " Gain " << gain <<".  Using time sample " << m_iPedestal << endmsg;
        }
        pedestalAverage=samples[m_iPedestal];
      } else {              
        if (debugPrint) {
	  log << MSG::DEBUG << noEnergy << ". No pedestal found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
              //<< " Gain " << gain << ". Skipping channel." << endmsg;
              << " [ Layer = " << layer << " - Eta = " << eta << " - Phi = " << phi << " - Region = " << region << " ]"
	      << " Gain = " << gain << ". Skipping channel." << endmsg;
        }
	noEnergy++;
        continue;
      }
    } else {
      if( ( m_pedestalFallbackMode>=2 && gain==CaloGain::LARLOWGAIN )   ||
          ( m_pedestalFallbackMode>=3 && gain==CaloGain::LARMEDIUMGAIN ) ||
          ( m_pedestalFallbackMode>=4 && gain==CaloGain::LARHIGHGAIN )       ) {
        if (debugPrint) {
          log << MSG::DEBUG << "Forcing pedestal fallback for  channel 0x" << MSG::hex << chid.get_compact()
              << MSG::dec   << " Gain=" << gain << ". Using time sample " << m_iPedestal << endmsg;
        }
        pedestalAverage=samples[m_iPedestal];
      } else {
        //Assume there is only one pedestal value, even the ILArPedestal object returns a vector<float>
        pedestalAverage=pedestal;
      }      
    }

    // Optimal Filtering Coefficients
    ILArOFC::OFCRef_t ofc_a;
    ILArOFC::OFCRef_t ofc_b;
    if (m_useOFCTool) { //Use OFC-Tool to compute OFC on-the-fly
      ofc_a=m_OFCTool->OFC_a(chid,gain);
      ofc_b=m_OFCTool->OFC_b(chid,gain);//retrieve only when needed
    }
    else {// get OFC from Conditions Store
      float febTimeOffset=0;
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      if (larFebTimeOffset)
	febTimeOffset=larFebTimeOffset->TimeOffset(febid);
      double timeShift=PhaseTime+globalTimeOffset+febTimeOffset;
      if (verbosePrint)
       log << MSG::VERBOSE << "Channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
 	   << " phase=" << PhaseTime  << " Feb=" << febTimeOffset 
	   << " Global=" << globalTimeOffset;

      if (m_useOFCPhase) {
	const double ofcTimeOffset=larOFC->timeOffset(chid,gain);
	timeShift+=ofcTimeOffset;
	if (verbosePrint) log << MSG::VERBOSE << " OFC=" << ofcTimeOffset;
      }

      if (verbosePrint) log << MSG::VERBOSE << " Total=" << timeShift << endmsg;
      
      if (m_allowTimeJump && timeShift >= m_NOFCPhases*m_OFCTimeBin ) {
	if (verbosePrint) log << MSG::VERBOSE << "Time Sample jump: +1" << endmsg;
	timeSampleShift += 1;
	//timeShift       -= m_NOFCTimeBins*m_OFCTimeBin ;
	timeShift       -= m_SamplingPeriode ;
      }
      else if (m_allowTimeJump && timeShift < 0 ) {
	if (verbosePrint) log << MSG::VERBOSE << "Time Sample jump: -1" << endmsg;
	timeSampleShift -= 1;
	//timeShift       += m_NOFCTimeBins*m_OFCTimeBin ;
        timeShift       += m_SamplingPeriode ;
      }

      if (m_allowTimeJump && ( timeShift > m_NOFCPhases*m_OFCTimeBin || timeShift < 0 ) ) {
	BadTiming++;
	noEnergy++;
	log << MSG::ERROR << noEnergy << ". Time offset out of range for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	    << " Found " << timeShift << ",  expected ( 0 - " << m_NOFCPhases*m_OFCTimeBin << ") ns. Skipping channel." << endmsg;
	continue;
      }
      
      if (m_allowTimeJump && timeSampleShift < 0) {
	BadTiming++;
	noEnergy++;
	log << MSG::ERROR << noEnergy << ". Negative time sample (" << timeSampleShift << ") shift for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	    << " Found. Skipping channel." << endmsg;
	continue;
      }

      OFCTimeBin = (int) ( timeShift / m_OFCTimeBin );
      
      if ( !m_phaseInv ) // if not done with PhaseTime at the beginning, invert time bin for OFC bin selection
         OFCTimeBin = ( m_NOFCTimeBins - 1 ) - OFCTimeBin;
      // do not use the following: 24<PhaseTime<25 you always get OFCTimeBin = -1!
      //else 
      //   OFCTimeBin -= 1 ; 
      
      if (verbosePrint) log << MSG::VERBOSE << "OFC bin width = " << m_OFCTimeBin << " - OFCBin = " << OFCTimeBin << " - timeShift = " << timeShift << endmsg;
      
      if ( OFCTimeBin < 0 ) {
        log << MSG::ERROR << "Channel " << MSG::hex << chid.get_compact() << MSG::dec << " asks for OFC bin = " << OFCTimeBin << ". Set to 0." << endmsg;
        OFCTimeBin=0;
      } else if ( OFCTimeBin >= m_NOFCPhases ) {
        log << MSG::ERROR << "Channel " << MSG::hex << chid.get_compact() << MSG::dec << " asks for OFC bin = " << OFCTimeBin << ". Set to (NOFCPhases-1) =" << m_NOFCTimeBins-1 << endmsg;
        OFCTimeBin = m_NOFCPhases-1;
      }

      ofc_a=larOFC->OFC_a(chid,gain,OFCTimeBin);
      //ofc_b=&(larOFC->OFC_b(chid,gain,OFCTimeBin)); retrieve only when needed
    }
    
    //Check if we have OFC for this channel and time bin
    if (ofc_a.size()==0) {
      noEnergy++;
      if (debugPrint) log << MSG::DEBUG << noEnergy << ". No OFC's found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
          << " [ Layer = " << layer << " - Eta = " << eta << " - Phi = " << phi << " - Region = " << region << " ]"
	  << " Time bin = " << OFCTimeBin << ", Gain = " << gain << ". Skipping channel." << endmsg;
      continue;
    } 
    if (ofc_a.size()+timeSampleShift>nSamples) {
      BadTiming++;
      noEnergy++;
      if (debugPrint) {
        if (timeSampleShift==0)
	  log << MSG::DEBUG << "Found LArDigit with " << nSamples << " samples, but OFCs for " 
	      << ofc_a.size() << " samples. Skipping Channel "<< endmsg;
        else //have time sample shift
	  log << MSG::DEBUG << "After time sample shift of " << timeSampleShift << ", " << nSamples-timeSampleShift
	      << " samples left, but have OFCs for " << ofc_a.size() << " samples. Skipping Channel "<< endmsg;
      }
      continue;
    } 

    //Now apply Optimal Filtering to get ADC peak
    float ADCPeak=0;
    for (unsigned i=0;i<(ofc_a.size());i++) 
      ADCPeak+=(samples[i+timeSampleShift]-pedestalAverage)*ofc_a.at(i);
    
    if (verbosePrint) log << MSG::VERBOSE << "ADC Height calculated " << ADCPeak << " TimeBin=" << OFCTimeBin  << endmsg;
     
    //ADC2MeV (a.k.a. Ramp)   
    const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(chid,gain);
    //Check ramp coefficents
    if (ramp.size()==0) {
      noEnergy++;
      if (debugPrint) log << MSG::DEBUG << noEnergy << ". No ADC2MeV data found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec
	  //<< " Gain "<< gain << " Skipping channel." << endmsg;
          << " [ Layer = " << layer << " - Eta = " << eta << " - Phi = " << phi << " - Region = " << region << " ]"
	  << " Gain = " << gain << ". Skipping channel." << endmsg;
      continue;
    } 

    // temporalery fix for bad ramps... should be done in the DB
    // if(ramp[1]>500 || ramp[1]<0) {

    if(ramp[1]>m_ramp_max[gain] || ramp[1]<0) {
      noEnergy++;
      if (debugPrint) log << MSG::DEBUG << "Bad ramp for channel " << chid << " (ramp[1] = " << ramp[1] << "): skip this channel" << endmsg;
      continue;
    } 

    float ADCPeakPower=ADCPeak;

    if (m_useIntercept[gain])
      energy=ramp[0];
    //otherwise ignore intercept, E=0;
    for (unsigned i=1;i<ramp.size();i++)
      {energy+=ramp[i]*ADCPeakPower; //pow(ADCPeak,i);
       //std::cout << "Step "<< i <<":" << ramp[i] << " * " << pow(ADCPeak,i) << "Sum=" << energy << std::endl;
       ADCPeakPower*=ADCPeak;
      }

// HV correction

    if (m_hvcorr) {
// HV tool
       float hvCorr = m_hvCorrTool->Scale(chid);
       energy = energy*hvCorr;
    }
  
    //Check if energy is above threshold for time & quality calculation
    if (energy>m_Ecut) {
      highE++;
      if (m_useOFCTool)  //Use OFC-Tool to compute OFC on-the-fly
	ofc_b=m_OFCTool->OFC_b(chid,gain);
      else 
	ofc_b=larOFC->OFC_b(chid,gain,OFCTimeBin);
      if (ofc_b.size() != ofc_a.size()) {//don't have proper number of coefficients
        if (debugPrint) {
	  if (ofc_b.size()==0)
	    log << MSG::DEBUG << "No time-OFC's found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	        << " Gain "<< gain << " found. Time not calculated." << endmsg;
	  else
	    log << MSG::DEBUG << "OFC for time size " << ofc_b.size() 
                << " not equal to OFC for energy size " << ofc_a.size() 
                << "   Time not calculated " << endmsg;
        }
	noTime++;
      }else{
	for (unsigned i=0;i<(ofc_b.size());i++) 
	  time+=(samples[i+timeSampleShift]-pedestalAverage)*ofc_b.at(i);
	time/=ADCPeak;
	// !! Time is now in ns with respect to calibration pulse shape
	// Used to calculate quality factor
      }
      if (verbosePrint) log << MSG::VERBOSE << "Time calculated " << time << " TimeBin=" << OFCTimeBin  << endmsg;

      //Calculate Quality factor
      if (larShape) { //Have shape object
	//Get Shape & Shape Derivative for this channel

	//const std::vector<float>& shape=larShape->Shape(chid,gain,OFCTimeBin);
	//const std::vector<float>& shapeDer=larShape->ShapeDer(chid,gain,OFCTimeBin);
        // ###
        ILArShape::ShapeRef_t shape=larShape->Shape(chid,gain,OFCTimeBin,m_shapeMode);
        ILArShape::ShapeRef_t shapeDer=larShape->ShapeDer(chid,gain,OFCTimeBin,m_shapeMode);
        // ###
        // fixing HEC to move +1 in case of 4 samples and firstSample 0
        int ihecshift=0;
        if(nSamples == 4 && m_firstSample == 0 ){
          Identifier id ;
          try {
               id = cabling->cnvToIdentifier(chid); 
               if(m_emId->is_lar_hec(id)) {
                  ihecshift=1;
                  log << MSG::DEBUG << "Setting firstSample to 1 for HEC channel "<< chid.get_compact() << endmsg;
               }
          } catch ( LArID_Exception & except ) {
               log << MSG::DEBUG << "A Cabling exception was caught for channel 0x!"
                   << MSG::hex << chid.get_compact() << MSG::dec << endmsg;
          }
        }
	
	//Check Shape
        if ((shape.size()+m_firstSample+ihecshift) < ofc_a.size()) {
          if (debugPrint) {
	    if (shape.size()==0) 
	       log << MSG::DEBUG << "No Shape found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	   	   << " Gain "<< gain << ". Quality factor not calculated." << endmsg;
            else
               log << MSG::DEBUG << "Shape size " << shape.size() 
                   << "smaller than OFC size " << ofc_a.size() 
                   << "for channel 0x" << MSG::hex << chid.get_compact() 
                   << MSG::dec << ". Quality factor not calculated." << endmsg;
          }
	  quality=0;  //Can't calculate chi^2, assume good hit.
	  noShape++;
	}
	else {//Shape ok
	  if (time!=0 && shapeDer.size()!=shape.size()) { 
	    //Send warning
	    if (debugPrint) log << MSG::DEBUG << "Shape-Derivative has different size than Shape for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	        << ". Derivative not taken into accout for quality factor." << endmsg;
	    noShapeDer++;
	  }//end-if 
	  if (time==0 || shapeDer.size()!=shape.size() ) { //Calculate Q without time info
	    for (unsigned i=0;i<(ofc_a.size());i++)
	      quality+=((samples[i+timeSampleShift]-pedestalAverage)-shape[i+m_firstSample+ihecshift]*ADCPeak)*
		       ((samples[i+timeSampleShift]-pedestalAverage)-shape[i+m_firstSample+ihecshift]*ADCPeak);
	  }
	  else { //All input data ok, calculate Q with time info
	    for (unsigned i=0;i<(ofc_a.size());i++) 
	      quality+=((samples[i+timeSampleShift]-pedestalAverage)-((shape[i+m_firstSample+ihecshift]-shapeDer[i+m_firstSample+ihecshift]*time)*ADCPeak))*
		       ((samples[i+timeSampleShift]-pedestalAverage)-((shape[i+m_firstSample+ihecshift]-shapeDer[i+m_firstSample+ihecshift]*time)*ADCPeak));
	  }
	} // end else (Shape ok)

      } //end if larShape
      else { //No Shape found at all 
	quality=0; //Can't calculate chi^2, assume good hit.
	noShape++;
      }
    //   if (m_useTDC) //Correct time according to EMTB definition (do we really want this?)
    // 	time= -time+24.5-tbin;
    }// end-if energy>Ecut
    else 
      quality=-1; //in case E<Ecut
    //time*=1000.0; 
    time=time*(nanosecond/picosecond); //Convert time to ps
    //Make LArRawChannel Object with new data

    uint16_t iqual=0;
    uint16_t iprov=0xA5;
    if (quality>=0) {
       iqual = ( (int)(quality) ) & 0xFFFF;
       iprov = iprov | 0x2000;
    }

    LArRawChannel larRawChannel(chid,(int)energy,(int)time,iqual,iprov, gain);   
    larRawChannelContainer->push_back(larRawChannel); //Add to container 
    ntot_raw++;
    if (verbosePrint)
      log << MSG::VERBOSE << "Got LArRawChannel #" << ntot_raw << ", chid=0x" << MSG::hex << chid.get_compact() << MSG::dec  
  	  << " e=" << energy << " t=" << time << " Q=" << quality << endmsg;
  } // End loop over LArDigits

  if (debugPrint) log << MSG::DEBUG <<  ntot_raw << " channels successfully processed, (" << highE << " with high energy)" << endmsg;

  // deal with bad timing
  if(BadTiming>=128){
    log<< MSG::ERROR << "Too many channels (" <<BadTiming<<  " !) have a bad timing !!" << endmsg;
    log<< MSG::ERROR << "OFC time constants should be revisited !!!" << endmsg;
    log<< MSG::ERROR << "Event is skipped" << endmsg;
    larRawChannelContainer->clear();
    //return StatusCode::SUCCESS;
  }
  
  // in case of at least one saturating cell, skip all event (if selected) 
  if ( saturation && m_skipSaturCells == 2 ) {
    log<< MSG::ERROR << saturation << " saturating channels were found. Event is skipped." << endmsg;
    larRawChannelContainer->clear();
  }
  
  //Put this LArRawChannel container in the transient store
  //sc = evtStore()->record(m_larRawChannelContainer, m_ChannelContainerName);
  //if(sc.isFailure()) {
  // log << MSG::ERROR << "Can't record LArRawChannelContainer in StoreGate" << endmsg;
  //}
  //else
  //  std::cout << "Successfully recorded LArRawChannelContainer to StoreGate" << std::endl;
  
  /*
   
   Error & Warning summary *per event*
   
   Strategy: 'No Energy' is an ERROR, no time or no quality is a WARNING
   
   Missing calibration constants are most likly missing for an entire run, threfore:
   In DEBUG: Print summary for each event if something is missing
   otherwise: Print summary only for new problems (different number of missing channels) 
  
   Saturatin cells summary is shown in any case, WARNING if not skipped, ERROR if skipped
   
  */
  
  if (noEnergy)   m_noEnergy++;
  if (noTime)     m_noTime++;
  if (noShape)    m_noShape++;
  if (noShapeDer) m_noShapeDer++;
  if (saturation) m_saturation++;
  
  m_aveNoEnergy   += noEnergy;  
  m_aveNoTime     += noTime;
  m_aveNoShape    += noShape;
  m_aveNoShapeDer += noShapeDer;
  m_aveSaturCells += saturation;
  
  if ( (   noEnergy!=m_lastNoEnergy 
        || noTime!=m_lastNoTime 
        || noShape>m_lastNoShape 
        || noShapeDer>m_lastNoShapeDer 
        || saturation>0 ) 
	   || ( msgSvc()->outputLevel(name()) <= MSG::DEBUG && ( noEnergy || noTime || noShape || noShapeDer || saturation ) )
      ) {
    
    m_lastNoEnergy = noEnergy;
    m_lastNoTime   = noTime;
    if (noShape>m_lastNoShape) m_lastNoShape=noShape;
    if (noShapeDer>m_lastNoShapeDer) m_lastNoShapeDer=noShapeDer;
    //m_lastSaturCells = saturation ;

    MSG::Level msglvl;
    if (noEnergy) 
      msglvl=MSG::ERROR;
    else if (noTime || noShape || noShapeDer || saturation)
      msglvl=MSG::WARNING;
    else
      msglvl=MSG::INFO;

    log << msglvl << " *** Error & Warning summary for this event *** " << std::endl;
    
    if ( noEnergy ) {
      log << msglvl << "   " << noEnergy << " out of " 
	  << digitContainer->size() << " channel(s) skipped due to a lack of basic calibration constants." 
	  << std::endl;  	  
    }
    if ( noTime ) {
      log << msglvl << "   " << noTime << " out of " 
	  << highE << " high-enegy channel(s) have no time-info due to a lack of Optimal Filtering Coefficients." 
	  << std::endl;
    }
    if ( noShape ) {
      log << msglvl << "   " << noShape << " out of " 
	  << highE << " high-enegy channel(s) have no quality factor due to a lack of shape." 
	  << std::endl;
    }
    if ( noShapeDer ) { 
      log << msglvl << "   " << noShapeDer << " out of " 
	  << highE << " high-enegy channel(s) lack the derivative of the shape. Not taken into accout for Quality factor." 
	  << std::endl;
    }
    if ( saturation ) {
      if ( m_skipSaturCells == 2 )
          log << MSG::ERROR << "   " << saturation << " out of " 
	      << digitContainer->size() << " channel(s) showed saturations. The complete event was skipped." << std::endl;
      else if ( m_skipSaturCells == 1 )
          log << MSG::ERROR << "   " << saturation << " out of " 
	      << digitContainer->size() << " channel(s) showed saturations and were skipped." << std::endl;
      else
          log << MSG::WARNING << "   " << saturation << " out of " 
	      << digitContainer->size() << " channel(s) showed saturations." << std::endl;
    }
    log << endmsg;
  }
    
  // lock raw channel container
  sc = evtStore()->setConst(larRawChannelContainer);
  if (sc.isFailure()) {
    log << MSG::ERROR << " Cannot lock RawChannel Container " << endmsg;
    return(StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

StatusCode LArRawChannelBuilder::finalize()
{ 

  if (m_noEnergy>0) m_aveNoEnergy   /= m_noEnergy;
  else m_aveNoEnergy=0;
  if (m_noTime>0) m_aveNoTime     /= m_noTime;
  else m_aveNoTime=0;
  if (m_noShape>0) m_aveNoShape    /= m_noShape;
  else m_aveNoShape=0;
  if (m_noShapeDer>0) m_aveNoShapeDer /= m_noShapeDer;
  else m_aveNoShapeDer=0;
  if (m_saturation>0) m_aveSaturCells /= m_saturation;
  else m_aveSaturCells=0;

  if (m_nEvents>0) m_aveChannels   /= m_nEvents;
  else m_aveChannels =0;

  // Error and Warning Summary for this job:
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "  LArRawChannelBuilder::finalize " 
      << m_noEnergy << " " << m_noTime << " " << m_noShape << " " << m_noShapeDer << " " << m_saturation << endmsg;
  
  if ( m_noEnergy || m_noTime || m_noShape || m_noShapeDer || m_saturation ) {
    MSG::Level msglvl;
    if ( m_noEnergy || m_skipSaturCells ) 
      msglvl=MSG::ERROR;
    else
      msglvl=MSG::WARNING;
    log << msglvl << " *** Error & Warning Summary for all events *** " << std::endl ;
    
    if (m_noEnergy)
      log << msglvl << "   " << m_noEnergy << " events had on average " << (int)round(m_aveNoEnergy) 
          << " channels out of " << (int)round(m_aveChannels) << " without basic calibration constants." 
	  << std::endl;
    
    if (m_noTime) 
      log << msglvl << "   " << m_noTime  << " events had on average " << (int)round(m_aveNoTime) 
          << " channels out of " << (int)round(m_aveChannels) << " without OFCs for timing." 
	  << std::endl ;
   
    if (m_noShape)
      log << msglvl << "   " << m_noShape << " events had on average " << (int)round(m_aveNoShape) 
          << " channels out of " << (int)round(m_aveChannels) << " without shape information." 
	  << std::endl;
    
    if (m_noShapeDer)
      log << msglvl << "   " << m_noShapeDer << " events had on average " << (int)round(m_aveNoShapeDer) 
          << " channels out of " << (int)round(m_aveChannels) << " without shape derivative." 
	  << std::endl;
	
    if ( m_saturation )
      log << msglvl << "   " << m_saturation << " events had on average " << (int)round(m_aveSaturCells) 
          << " out of " << (int)round(m_aveChannels) << " saturating channels."  
	  << std::endl ;
    
    log << endmsg;
  } 
  else
    log << MSG::INFO << "LArRawChannelBuilder finished without errors or warnings." << endmsg;

  //if (m_larRawChannelContainer) {
    //m_larRawChannelContainer->release();
    //m_larRawChannelContainer = 0;
  //}

  return StatusCode::SUCCESS;
}
