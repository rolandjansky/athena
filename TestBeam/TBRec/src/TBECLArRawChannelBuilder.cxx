/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBECLArRawChannelBuilder.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

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
#include "AthenaKernel/Units.h"

#include <math.h>

using CLHEP::MeV;
using CLHEP::megahertz;
using Athena::Units::picosecond;

TBECLArRawChannelBuilder::TBECLArRawChannelBuilder (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_OFCTool("LArOFCTool"),
  m_adc2mevTool("LArADC2MeVTool"),
  m_onlineHelper(0),
  m_calo_id(0),
  m_calo_dd_man(0),
  //m_roiMap("LArRoI_Map"),
  m_DataLocation("FREE"),
  m_ChannelContainerName("LArRawChannels"),
  m_useTDC(false),
  m_useRamp(true),
  m_useShape(true),
  m_useOFCTool(false),
  m_ConvertADCToHighGain(false),
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
  m_emId(0),
  m_adc2mev()
  //m_NOFCPhases(24),
  //m_NOFCTimeBins(24)
 {
   //m_useIntercept={false,false,false,false};
 declareProperty("LArRawChannelContainerName",m_ChannelContainerName);
 declareProperty("DataLocation",              m_DataLocation );
 declareProperty("UseTDC",                    m_useTDC);
 declareProperty("UseRamp",m_useRamp);
 declareProperty("UseShape",m_useShape);
 declareProperty("UseOFCTool",                m_useOFCTool);
 declareProperty("ConvertADCToHighGain",m_ConvertADCToHighGain);
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
}


StatusCode TBECLArRawChannelBuilder::initialize(){

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  if (m_useOFCTool)
    ATH_CHECK( m_OFCTool.retrieve() );

  if (m_useRamp) {
    ATH_CHECK( m_adc2mevTool.retrieve() );
  } else {
    // pointer to detector manager:
    m_calo_dd_man = CaloDetDescrManager::instance();
    m_calo_id   = m_calo_dd_man->getCaloCell_ID();
    for (int i=0; i<30; i++) {
      m_adc2mev[i] = 0;
      if (i == 6)  m_adc2mev[i] = 0.041*637;    // EMEC2
      if (i == 7)  m_adc2mev[i] = 0.030*637;    // EMEC3
      if (i == 8)  m_adc2mev[i] = 0.00360*3270; // HEC0
      if (i == 9)  m_adc2mev[i] = 0.00380*3270; // HEC1
      if (i == 10) m_adc2mev[i] = 0.00186*6540; // HEC2
      if (i == 21 || i == 22) m_adc2mev[i] = 0.1087*767;  // FCal1,2
      if (i == 23) m_adc2mev[i] = 0.1087*1508;  // FCal3
    }
  }
  
  // ***
  
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  if (!m_emId) {
    ATH_MSG_ERROR ( "Could not get lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  
  // translate offline ID into online ID
  ATH_CHECK( m_cablingKey.initialize() );
  
  // ***

  if (m_hvcorr) {
    ATH_CHECK( m_offlineHVScaleCorrKey.initialize() );
  }


  //m_larRawOrdering.setMap(&(*m_roiMap)); 

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

  ATH_MSG_DEBUG ( "Number of OFC time bins per sampling periode=" << m_NOFCTimeBins );
  ATH_MSG_DEBUG ( "Sampling Periode=" << m_SamplingPeriode << "ns" );
  ATH_MSG_DEBUG ( "Sampling Periode Limits: (" << m_SamplingPeriodeLowerLimit
                  << "," << m_SamplingPeriodeUpperLimit << ") ns" );

  return StatusCode::SUCCESS;
}



StatusCode TBECLArRawChannelBuilder::execute() 
{
  //Counters for errors & warnings per event
  int noEnergy   = 0; // Number of completly failed channels in a given event
  int BadTiming  = 0; // Number of channels with bad timing in a given event
  int noTime     = 0; // Number of channels without time info in a given event
  int noShape    = 0; // Number of channels without Shape (= with no quality factor) in a given event 
  int noShapeDer = 0; // Number of channels without ShapeDerivative in a given event 
  int highE      = 0; // Number of channels with 'high' (above threshold) energy in a given event 
  int saturation = 0; // Number of saturating channels in a given event   
  
  const ILArHVScaleCorr *oflHVCorr=nullptr;
  if(m_hvcorr) {
     SG::ReadCondHandle<ILArHVScaleCorr> oflHVCorrHdl(m_offlineHVScaleCorrKey);
     oflHVCorr = *oflHVCorrHdl;
     if(!oflHVCorr) {
        ATH_MSG_ERROR( "Could not get the HVScaleCorr from key " << m_offlineHVScaleCorrKey.key() );
        return StatusCode::FAILURE;
     }
  }
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
        ATH_MSG_ERROR( "Could not get the cabling mapping from key " << m_cablingKey.key() );
        return StatusCode::FAILURE;
     }

  //Pointer to input data container
  const LArDigitContainer* digitContainer=NULL;//Pointer to LArDigitContainer
  //const TBPhase* theTBPhase; //Pointer to Testbeam TDC-Phase object (if needed)
  float PhaseTime=0;                //Testbeam TDC phase (if needed)
  float globalTimeOffset=0;
  //Pointer to conditions data objects 
  const ILArFEBTimeOffset* larFebTimeOffset=NULL;
  const ILArOFC* larOFC=NULL;
  const ILArShape* larShape=NULL;
  //Retrieve Digit Container

  ATH_CHECK( evtStore()->retrieve(digitContainer,m_DataLocation) );
  
  //Retrieve calibration data
  const ILArPedestal* larPedestal=nullptr;
  ATH_CHECK( detStore()->retrieve(larPedestal) );
  
  if (m_useShape) {
    ATH_MSG_DEBUG ( "Retrieving LArShape object" );
    StatusCode sc=detStore()->retrieve(larShape);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "Can't retrieve LArShape from Conditions Store" << std::endl
                        << "Quality factor will not be caluclated." );
      larShape=NULL;
    }
  }
  ATH_MSG_DEBUG ( "Retrieving LArOFC object" );
  if (!m_useOFCTool) {  //OFC-Conditons object only needed if OFC's not computed on-the-fly
    ATH_CHECK( detStore()->retrieve(larOFC) );
  }

  //retrieve TDC
  if (m_useTDC) { //All this timing business is only necessary if the readout and the beam are not in phase (Testbeam)
    const TBPhase* theTBPhase;
    const ILArGlobalTimeOffset* larGlobalTimeOffset;
    ATH_CHECK( evtStore()->retrieve(theTBPhase,"TBPhase") );
    //Get Phase in nanoseconds
    PhaseTime = theTBPhase->getPhase();
    // ###
    if (m_phaseInv) PhaseTime = m_SamplingPeriode - PhaseTime ;
    ATH_MSG_DEBUG ( " *** Phase = " << PhaseTime );
    // ###
    
    //Get Global Time Offset
    StatusCode sc=detStore()->retrieve(larGlobalTimeOffset);
    if (sc.isSuccess()) globalTimeOffset = larGlobalTimeOffset->TimeOffset();

    //Get FEB time offset
    sc=detStore()->retrieve(larFebTimeOffset);
    if (sc.isFailure()) larFebTimeOffset=NULL;
  }


  LArRawChannelContainer* larRawChannelContainer=new LArRawChannelContainer();
  larRawChannelContainer->reserve(digitContainer->size());
  StatusCode sc = evtStore()->record(larRawChannelContainer,m_ChannelContainerName);
  if(sc.isFailure()) {
    ATH_MSG_ERROR ( "Can't record LArRawChannelContainer in StoreGate" );
  }

  // Average number of LArDigits per event
  m_nEvents++;
  m_aveChannels += digitContainer->size();

  bool debugPrint=false;
  if (msgLvl(MSG::DEBUG) ) debugPrint=true;

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
    if (msgLvl(MSG::DEBUG) ) {
      Identifier id ;
      try {
        id = cabling->cnvToIdentifier(chid);
      } catch ( LArID_Exception & except ) {
        ATH_MSG_DEBUG ( "A Cabling exception was caught for channel 0x!" 
                        << MSG::hex << chid.get_compact() << MSG::dec );
        continue ;
      }
      layer  = m_emId->sampling(id);
      eta    = m_emId->eta(id); 
      phi    = m_emId->phi(id);
      region = m_emId->region(id);    
      ATH_MSG_VERBOSE ( "Channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                        << " [ Layer = " << layer << " - Eta = " << eta 
                        << " - Phi = " << phi << " - Region = " << region << " ] " );
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
      msg() << MSG::DEBUG << "Saturation on channel 0x" << MSG::hex << chid.get_compact() << MSG::dec ;         
      saturation++;
    }
    if ( m_skipSaturCells && nSatur>-1 ) {
      msg() << ". Skipping channel." << endmsg; 
      continue; // Ignore this cell, saturation on at least one sample
    } else if ( nSatur>-1 ) {
      msg() << "." << endmsg;
    }   
    
    //Get conditions data for this channel:

    // Pedestal
    float pedestal=larPedestal->pedestal(chid,gain);

    float pedestalAverage;
    if (pedestal < (1.0+LArElecCalib::ERRORCODE)) {
      if( m_pedestalFallbackMode >= 1 ) {
        ATH_MSG_DEBUG ( "No pedestal found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                        << " Gain " << gain <<".  Using time sample " << m_iPedestal );
        pedestalAverage=samples[m_iPedestal];
      } else {              
	ATH_MSG_DEBUG ( noEnergy << ". No pedestal found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
            << " [ Layer = " << layer << " - Eta = " << eta << " - Phi = " << phi << " - Region = " << region << " ]"
                        << " Gain = " << gain << ". Skipping channel." );
	noEnergy++;
        continue;
      }
    } else {
      if( ( m_pedestalFallbackMode>=2 && gain==CaloGain::LARLOWGAIN )   ||
          ( m_pedestalFallbackMode>=3 && gain==CaloGain::LARMEDIUMGAIN ) ||
          ( m_pedestalFallbackMode>=4 && gain==CaloGain::LARHIGHGAIN )       ) {
        ATH_MSG_DEBUG ( "Forcing pedestal fallback for  channel 0x" << MSG::hex << chid.get_compact()
                        << MSG::dec   << " Gain=" << gain << ". Using time sample " << m_iPedestal );
        pedestalAverage=samples[m_iPedestal];
      } else {
        pedestalAverage=pedestal;
      }      
    }

    // Optimal Filtering Coefficients
    ILArOFC::OFCRef_t ofc_a;
    ILArOFC::OFCRef_t ofc_b;
    if (m_useOFCTool) { //Use OFC-Tool to compute OFC on-the-fly
      ofc_a=m_OFCTool->OFC_a(chid,gain);
      //ofc_b=&(m_OFCTool->OFC_b(chid,gain));//retrieve only when needed
    }
    else {// get OFC from Conditions Store
      float febTimeOffset=0;
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      if (larFebTimeOffset)
	febTimeOffset=larFebTimeOffset->TimeOffset(febid);
      double timeShift=PhaseTime+globalTimeOffset+febTimeOffset;
      if (debugPrint)
        msg() << MSG::VERBOSE << "Channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
 	   << " phase=" << PhaseTime  << " Feb=" << febTimeOffset 
	   << " Global=" << globalTimeOffset;

      if (m_useOFCPhase) {
	const double ofcTimeOffset=larOFC->timeOffset(chid,gain);
	timeShift+=ofcTimeOffset;
	if (debugPrint) msg() << MSG::VERBOSE << " OFC=" << ofcTimeOffset;
      }

      if (debugPrint) msg() << MSG::VERBOSE << " Total=" << timeShift << endmsg;
      
      if (m_allowTimeJump && timeShift >= m_NOFCPhases*m_OFCTimeBin ) {
	if (debugPrint) ATH_MSG_VERBOSE ( "Time Sample jump: -1" );
	timeSampleShift -= 1;
	//timeShift       -= m_NOFCTimeBins*m_OFCTimeBin ;
	timeShift       -= m_SamplingPeriode ;
      }
      else if (m_allowTimeJump && timeShift < 0 ) {
	if (debugPrint) ATH_MSG_VERBOSE ( "Time Sample jump: +1" );
	timeSampleShift += 1;
	//timeShift       += m_NOFCTimeBins*m_OFCTimeBin ;
        timeShift       += m_SamplingPeriode ;
      }

      if (m_allowTimeJump && ( timeShift > m_NOFCPhases*m_OFCTimeBin || timeShift < 0 ) ) {
	BadTiming++;
	noEnergy++;
	ATH_MSG_ERROR ( noEnergy << ". Time offset out of range for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                        << " Found " << timeShift << ",  expected ( 0 - " << m_NOFCPhases*m_OFCTimeBin << ") ns. Skipping channel." );
	continue;
      }
      
      if (m_allowTimeJump && timeSampleShift < 0) {
	BadTiming++;
	noEnergy++;
	ATH_MSG_ERROR ( noEnergy << ". Negative time sample (" << timeSampleShift << ") shift for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                        << " Found. Skipping channel." );
	continue;
      }

      OFCTimeBin = (int) ( timeShift / m_OFCTimeBin );
      
      if ( !m_phaseInv ) // if not done with PhaseTime at the beginning, invert time bin for OFC bin selection
         OFCTimeBin = ( m_NOFCTimeBins - 1 ) - OFCTimeBin;
      // do not use the following: 24<PhaseTime<25 you always get OFCTimeBin = -1!
      //else 
      //   OFCTimeBin -= 1 ; 
      
      if (debugPrint) ATH_MSG_VERBOSE ( "OFC bin width = " << m_OFCTimeBin << " - OFCBin = " << OFCTimeBin << " - timeShift = " << timeShift );
      
      if ( OFCTimeBin < 0 ) {
        ATH_MSG_ERROR ( "Channel " << MSG::hex << chid.get_compact() << MSG::dec << " asks for OFC bin = " << OFCTimeBin << ". Set to 0." );
        OFCTimeBin=0;
      } else if ( OFCTimeBin >= m_NOFCPhases ) {
        ATH_MSG_ERROR ( "Channel " << MSG::hex << chid.get_compact() << MSG::dec << " asks for OFC bin = " << OFCTimeBin << ". Set to (NOFCPhases-1) =" << m_NOFCTimeBins-1 );
        OFCTimeBin = m_NOFCPhases-1;
      }

      ofc_a=larOFC->OFC_a(chid,gain,OFCTimeBin);
      //ofc_b=&(larOFC->OFC_b(chid,gain,OFCTimeBin)); retrieve only when needed
    }
    
    //Check if we have OFC for this channel and time bin
    if (ofc_a.size()==0) {
      noEnergy++;
      ATH_MSG_DEBUG ( noEnergy << ". No OFC's found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
          << " [ Layer = " << layer << " - Eta = " << eta << " - Phi = " << phi << " - Region = " << region << " ]"
                      << " Time bin = " << OFCTimeBin << ", Gain = " << gain << ". Skipping channel." );
      continue;
    } 
    if (ofc_a.size()+timeSampleShift>nSamples) {
      BadTiming++;
      noEnergy++;
      if (timeSampleShift==0)
	ATH_MSG_DEBUG ( "Found LArDigit with " << nSamples << " samples, but OFCs for " 
                        << ofc_a.size() << " samples. Skipping Channel ");
      else //have time sample shift
	ATH_MSG_DEBUG ( "After time sample shift of " << timeSampleShift << ", " << nSamples-timeSampleShift
                        << " samples left, but have OFCs for " << ofc_a.size() << " samples. Skipping Channel ");
      continue;
    } 

    //Now apply Optimal Filtering to get ADC peak
    float ADCPeak=0;
    for (unsigned i=0;i<(ofc_a.size());i++) 
      ADCPeak+=(samples[i+timeSampleShift]-pedestalAverage)*ofc_a.at(i);
    
    if (debugPrint) ATH_MSG_VERBOSE ( "ADC Height calculated " << ADCPeak << " TimeBin=" << OFCTimeBin  );
     
    if (m_useRamp) {
      //ADC2MeV (a.k.a. Ramp)   
      const std::vector<float>& ramp=m_adc2mevTool->ADC2MEV(chid,gain);
      //Check ramp coefficents
      if (ramp.size()==0) {
	noEnergy++;
	ATH_MSG_DEBUG ( noEnergy << ". No ADC2MeV data found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec
	    << " [ Layer = " << layer << " - Eta = " << eta << " - Phi = " << phi << " - Region = " << region << " ]"
                        << " Gain = " << gain << ". Skipping channel." );
	continue;
      } 
      
      // temporalery fix for bad ramps... should be done in the DB
      if(ramp[1]>m_ramp_max[gain] || ramp[1]<0) {
	noEnergy++;
	ATH_MSG_DEBUG ( "Bad ramp for channel " << chid << " (ramp[1] = " << ramp[1] << "): skip this channel" );
	continue;
      } 
      
      float ADCPeakPower=ADCPeak;
      
      if (m_useIntercept[gain])
	energy=ramp[0];
      //otherwise ignore intercept, E=0;
      for (unsigned i=1;i<ramp.size();i++)
	{energy+=ramp[i]*ADCPeakPower; //pow(ADCPeak,i);
	ADCPeakPower*=ADCPeak;
	}
    } else {
      energy = ADCPeak;
      if (m_ConvertADCToHighGain && gain == CaloGain::LARMEDIUMGAIN) 
	energy *= 9.5;
      Identifier id = cabling->cnvToIdentifier(chid);
      int is = m_calo_id->calo_sample(id);
      energy *= m_adc2mev[is];                // Ramp for h.g. scale
    }
    
// HV correction

    if (m_hvcorr) {
// HV tool
       float hvCorr = oflHVCorr-> HVScaleCorr(chid);
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
	if (ofc_b.size()==0)
	  ATH_MSG_DEBUG ( "No time-OFC's found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                          << " Gain "<< gain << " found. Time not calculated." );
	else
	  ATH_MSG_DEBUG ( "OFC for time size " << ofc_b.size() 
              << " not equal to OFC for energy size " << ofc_a.size() 
                          << "   Time not calculated " );
	noTime++;
      }else{
	for (unsigned i=0;i<(ofc_b.size());i++) 
	  time+=(samples[i+timeSampleShift]-pedestalAverage)*ofc_b.at(i);
	time/=ADCPeak;
	// !! Time is now in ns with respect to calibration pulse shape
	// Used to calculate quality factor
      }
      if (debugPrint) ATH_MSG_VERBOSE ( "Time calculated " << time << " TimeBin=" << OFCTimeBin  );

      //Calculate Quality factor
      if (larShape) { //Have shape object
	//Get Shape & Shape Derivative for this channel

	//const std::vector<float>& shape=larShape->Shape(chid,gain,OFCTimeBin);
	//const std::vector<float>& shapeDer=larShape->ShapeDer(chid,gain,OFCTimeBin);
        // ###
        ILArShape::ShapeRef_t shape=larShape->Shape(chid,gain,OFCTimeBin,m_shapeMode);
        ILArShape::ShapeRef_t shapeDer=larShape->ShapeDer(chid,gain,OFCTimeBin,m_shapeMode);
        // ###
	
	//Check Shape
        if (shape.size() < ofc_a.size()) {
	  if (shape.size()==0) 
	    ATH_MSG_DEBUG ( "No Shape found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " Gain "<< gain << ". Quality factor not calculated." );
          else
            ATH_MSG_DEBUG ( "Shape size " << shape.size() 
                << "smaller than OFC size " << ofc_a.size() 
                << "for channel 0x" << MSG::hex << chid.get_compact() 
                            << MSG::dec << ". Quality factor not calculated." );
	  quality=0;  //Can't calculate chi^2, assume good hit.
	  noShape++;
	}
	else {//Shape ok
	  if (time!=0 && shapeDer.size()!=shape.size()) { 
	    //Send warning
	    ATH_MSG_DEBUG ( "Shape-Derivative has different size than Shape for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << ". Derivative not taken into accout for quality factor." );
	    noShapeDer++;
	  }//end-if 
	  if (time==0 || shapeDer.size()!=shape.size() ) { //Calculate Q without time info
	    for (unsigned i=0;i<(ofc_a.size());i++)
	      quality+=((samples[i+timeSampleShift]-pedestalAverage)-shape[i]*ADCPeak)*
		       ((samples[i+timeSampleShift]-pedestalAverage)-shape[i]*ADCPeak);
	  }
	  else { //All input data ok, calculate Q with time info
	    for (unsigned i=0;i<(ofc_a.size());i++) 
	      quality+=((samples[i+timeSampleShift]-pedestalAverage)-((shape[i]-shapeDer[i]*time)*ADCPeak))*
		       ((samples[i+timeSampleShift]-pedestalAverage)-((shape[i]-shapeDer[i]*time)*ADCPeak));
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
    time=time/picosecond; //Convert time to ps
    //Make LArRawChannel Object with new data

    uint16_t iquality=0; 
    uint16_t iprovenance=0xA5; 
    if (quality>=0) {  
       iquality = ((int)(quality) ) & 0xFFFF; 
       iprovenance=iprovenance | 0x2000; 
    }


    LArRawChannel larRawChannel(chid,(int)energy,(int)time,iquality,iprovenance, gain);
    larRawChannelContainer->push_back(larRawChannel); //Add to container 
    ntot_raw++;
    if (debugPrint)
      ATH_MSG_VERBOSE ( "Got LArRawChannel #" << ntot_raw << ", chid=0x" << MSG::hex << chid.get_compact() << MSG::dec  
                        << " e=" << energy << " t=" << time << " Q=" << quality );
  } // End loop over LArDigits

  ATH_MSG_DEBUG (  ntot_raw << " channels successfully processed, (" << highE << " with high energy)" );

  // deal with bad timing
  if(BadTiming>=128){
    ATH_MSG_ERROR ( "Too many channels (" <<BadTiming<<  " !) have a bad timing !!" );
    ATH_MSG_ERROR ( "OFC time constants should be revisited !!!" );
    ATH_MSG_ERROR ( "Event is skipped" );
    larRawChannelContainer->clear();
    //return StatusCode::SUCCESS;
  }
  
  // in case of at least one saturating cell, skip all event (if selected) 
  if ( saturation && m_skipSaturCells == 2 ) {
    ATH_MSG_ERROR ( saturation << " saturating channels were found. Event is skipped." );
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
    else
      msglvl=MSG::WARNING;
    msg() << msglvl << " *** Error & Warning summary for this event *** " << std::endl;
    
    if ( noEnergy ) {
      msg() << msglvl << "   " << noEnergy << " out of " 
	  << digitContainer->size() << " channel(s) skipped due to a lack of basic calibration constants." 
	  << std::endl;  	  
    }
    if ( noTime ) {
      msg() << msglvl << "   " << noTime << " out of " 
	  << highE << " high-enegy channel(s) have no time-info due to a lack of Optimal Filtering Coefficients." 
	  << std::endl;
    }
    if ( noShape ) {
      msg() << msglvl << "   " << noShape << " out of " 
	  << highE << " high-enegy channel(s) have no quality factor due to a lack of shape." 
	  << std::endl;
    }
    if ( noShapeDer ) { 
      msg() << msglvl << "   " << noShapeDer << " out of " 
	  << highE << " high-enegy channel(s) lack the derivative of the shape. Not taken into accout for Quality factor." 
	  << std::endl;
    }
    if ( saturation ) {
      if ( m_skipSaturCells == 2 )
        msg() << MSG::ERROR << "   " << saturation << " out of " 
	      << digitContainer->size() << " channel(s) showed saturations. The complete event was skipped." << std::endl;
      else if ( m_skipSaturCells == 1 )
        msg() << MSG::ERROR << "   " << saturation << " out of " 
	      << digitContainer->size() << " channel(s) showed saturations and were skipped." << std::endl;
      else
        msg() << MSG::WARNING << "   " << saturation << " out of " 
	      << digitContainer->size() << " channel(s) showed saturations." << std::endl;
    }
    msg() << endmsg;
  }
    
  // lock raw channel container
  ATH_CHECK( evtStore()->setConst(larRawChannelContainer) );

  return StatusCode::SUCCESS;
}

StatusCode TBECLArRawChannelBuilder::finalize()
{ 

  m_aveNoEnergy   /= m_noEnergy;
  m_aveNoTime     /= m_noTime;
  m_aveNoShape    /= m_noShape;
  m_aveNoShapeDer /= m_noShapeDer;
  m_aveSaturCells /= m_saturation;

  m_aveChannels   /= m_nEvents;

  // Error and Warning Summary for this job:
  
  ATH_MSG_DEBUG ( "  TBECLArRawChannelBuilder::finalize " 
                  << m_noEnergy << " " << m_noTime << " " << m_noShape << " " << m_noShapeDer << " " << m_saturation );
  
  if ( m_noEnergy || m_noTime || m_noShape || m_noShapeDer || m_saturation ) {
    MSG::Level msglvl;
    if ( m_noEnergy || m_skipSaturCells ) 
      msglvl=MSG::ERROR;
    else
      msglvl=MSG::WARNING;
    msg() << msglvl << " *** Error & Warning Summary for all events *** " << std::endl ;
    
    if (m_noEnergy)
      msg() << msglvl << "   " << m_noEnergy << " events had on average " << (int)round(m_aveNoEnergy) 
          << " channels out of " << (int)round(m_aveChannels) << " without basic calibration constants." 
	  << std::endl;
    
    if (m_noTime) 
      msg() << msglvl << "   " << m_noTime  << " events had on average " << (int)round(m_aveNoTime) 
          << " channels out of " << (int)round(m_aveChannels) << " without OFCs for timing." 
	  << std::endl ;
   
    if (m_noShape)
      msg() << msglvl << "   " << m_noShape << " events had on average " << (int)round(m_aveNoShape) 
          << " channels out of " << (int)round(m_aveChannels) << " without shape information." 
	  << std::endl;
    
    if (m_noShapeDer)
      msg() << msglvl << "   " << m_noShapeDer << " events had on average " << (int)round(m_aveNoShapeDer) 
          << " channels out of " << (int)round(m_aveChannels) << " without shape derivative." 
	  << std::endl;
	
    if ( m_saturation )
      msg() << msglvl << "   " << m_saturation << " events had on average " << (int)round(m_aveSaturCells) 
          << " out of " << (int)round(m_aveChannels) << " saturating channels."  
	  << std::endl ;
    
    msg() << endmsg;
  } 
  else
    ATH_MSG_INFO ( "TBECLArRawChannelBuilder finished without errors or warnings." );

  //if (m_larRawChannelContainer) {
    //m_larRawChannelContainer->release();
    //m_larRawChannelContainer = 0;
  //}

  return StatusCode::SUCCESS;
}
