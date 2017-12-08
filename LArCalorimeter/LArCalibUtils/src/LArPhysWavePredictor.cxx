/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArPhysWavePredictor.h"

#include "GaudiKernel/ToolHandle.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArPhysWaveContainer.h"

#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArWFParams.h"

#include "LArElecCalib/ILArCaliPulseParams.h"
#include "LArElecCalib/ILArDetCellParams.h"
#include "LArElecCalib/ILArPhysCaliTdiff.h"
#include "LArElecCalib/ILArTdrift.h"
#include "LArElecCalib/ILArMphysOverMcal.h"

#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
#include "LArRawConditions/LArTdriftComplete.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"


#include "LArCalibUtils/LArPhysWaveTool.h"     // added by FT
#include "LArCalibUtils/LArPhysWaveHECTool.h"  // added by FT

#include "LArCalibUtils/LArPhysWaveTool.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


typedef LArPhysWaveContainer::ConstConditionsMapIterator PhysWaveIt;

LArPhysWavePredictor::LArPhysWavePredictor (const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator),
   m_badChanTool("LArBadChanTool"),
   m_onlineHelper(0),
   m_groupingType("FeedThrough") // SubDetector, Single, FeedThrough
{
  declareProperty("MaskingTool",      m_maskingTool,"Only for messaging");
  declareProperty("TestMode",         m_testmode   = false);
  declareProperty("StoreEmpty",       m_storeEmpty = false);

  m_keyCali.clear() ;
  declareProperty("KeyCaliList",      m_keyCali);                  // Keys of LArCaliWaveContainers
  declareProperty("KeyPhys",          m_keyPhys = "LArPhysWave") ; // Key of LArPhysWaveContainer
  declareProperty("KeyIdealPhys",     m_keyIdealPhys   = "LArPhysWaveHECIdeal") ; // added by FT    
  declareProperty("KeyMphysMcali",    m_keyMphysMcali = "LArMphysOverMcal") ; // Key of LArMphysOverMcalComplete
  declareProperty("DumpMphysMcali",   m_dumpMphysMcali = false ) ; // for debugging
  declareProperty("NormalizeCali",    m_normalizeCali = false ) ; // for debugging
  declareProperty("isHEC",            m_isHEC = false ) ; 

  //
  // jobOptions for calibration pulse paramers (Tcali, Fstep)
  //
  declareProperty("UseCaliPulseParamsFromJO", m_useJOCaliPulseParams = false );
  declareProperty("Tcali", m_Tcali = 450. ); 
  declareProperty("Fstep", m_Fstep = 0.07 ); 
  
  declareProperty("UseDetCellParamsFromJO"  , m_useJODetCellParams   = false );
  declareProperty("Omega0", m_Omega0 = 0. ); // Omega0 == 0 will skip injection point correction 
  declareProperty("Taur",   m_Taur   = 0. ); 
  
  //
  // jobOptions for drift time of predicted pulses (Tdrift)
  //
  declareProperty("UseTdriftFromJO", m_useJOTdrift = false );
  m_Tdrift.clear();
  double default_Tdrift[4] = { 420 , 475 , 475 , 475 } ;
  for ( unsigned i=0;i<4;i++) m_Tdrift.push_back(default_Tdrift[i]);
  declareProperty("Tdrift", m_Tdrift) ;
  m_Tdrift2.clear();
  double default_Tdrift2[4] = { 1200. , 1200. , 1200. , 1200. } ;
  for ( unsigned i=0;i<4;i++) m_Tdrift2.push_back(default_Tdrift2[i]);
  declareProperty("Tdrift2", m_Tdrift2) ;
  m_wTriangle2.clear();
  double default_wTriangle2[4] = { 0.01 , 0.01 , 0.01 , 0.01 } ;
  for ( unsigned i=0;i<4;i++) m_wTriangle2.push_back(default_wTriangle2[i]);
  declareProperty("WeightTriangle2", m_wTriangle2) ;
  declareProperty("UseDoubleTriangle", m_doubleTriangle = false );
  
  //
  // jobOptions for time shift of predicted pulses
  // 
  declareProperty("UseTimeShiftFromJO",   m_useJOPhysCaliTdiff   = false );
  declareProperty("TimeShiftByIndex" ,    m_timeShiftByIndex     = 0);
  declareProperty("TimeShiftByHelper",    m_timeShiftByHelper    = false);
  declareProperty("TimeShiftByLayer",     m_timeShiftByLayer     = false) ;
  declareProperty("TimeShiftByFEB",       m_timeShiftByFEB       = false) ;
  declareProperty("TimeShiftGuardRegion", m_timeShiftGuardRegion = 0 ) ;  
  m_TshiftLayer.clear();
  unsigned int default_TshiftLayer[4] = { 0 , 0 , 0 , 0 } ;
  for ( unsigned i=0;i<4;i++)  m_TshiftLayer.push_back(default_TshiftLayer[i]);
  declareProperty("Tshift",m_TshiftLayer) ;
  
  // Grouping type
  declareProperty("GroupingType",      m_groupingType);  
}


LArPhysWavePredictor::~LArPhysWavePredictor() 
{}


StatusCode LArPhysWavePredictor::initialize() 
{
    if ( ! m_doubleTriangle ) {
      msg(MSG::INFO) << "Using standard triangular ionization pulse" << endreq ;
  } else {
    msg(MSG::INFO) << "Using refined ionization pulse (double triangle)" << endreq ;
  }

  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get LArOnlineID" << endreq;
    return sc;
  }
  

  sc=m_badChanTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve BadChannelTool " << m_badChanTool << endreq;
    return StatusCode::FAILURE;
  }

  sc=m_maskingTool.retrieve(); 
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve BadChannelMask "
                    << m_maskingTool << endreq;
    return StatusCode::FAILURE;
  }


  

  return StatusCode::SUCCESS ;
}


namespace {
struct FileCloser
{
  FileCloser (FILE* the_f): f (the_f) {}
  ~FileCloser() { if (f) fclose(f); }
  FILE* f;
};
} // anonymous namespace

StatusCode LArPhysWavePredictor::stop()
{
  msg(MSG::INFO) << "... in stop()" << endreq ;
  
  LArWaveHelper larWaveHelper;

  // Get access to the Detector Store
  //StoreGateSvc* detStore; 
  //StatusCode sc = service("DetectorStore",detStore);
  //if (sc!=StatusCode::SUCCESS) {
  //  msg(MSG::ERROR) << "Cannot get DetectorStore!" << endreq;
  //  return sc;
  //}
  
  // Retrieve LArPhysWaveTool
  ToolHandle<LArPhysWaveTool> larPhysWaveTool("LArPhysWaveTool");
  StatusCode sc=larPhysWaveTool.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    msg(MSG::ERROR) << " Can't get LArPhysWaveTool " << endreq;
    return sc;
  }

  // Retrieve LArPhysWaveHECTool   // added by FT
  ToolHandle<LArPhysWaveHECTool> larPhysWaveHECTool("LArPhysWaveHECTool");
  if(m_isHEC){
    sc=larPhysWaveHECTool.retrieve();
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << " Can't get LArPhysWaveHECTool " << endreq;
      return sc;
    }}

  const CaloIdManager *caloIdMgr = CaloIdManager::instance() ;
  const LArEM_ID* emId = caloIdMgr->getEM_ID();
  if (!emId) {
      msg(MSG::ERROR) << "Could not access lar EM ID helper" << endreq;
      return StatusCode::FAILURE;
  }   
  
  // Retrieve LArCablingService
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  sc = larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    msg(MSG::ERROR) << " Can't get LArCablingSvc " << endreq;
    return sc;
  }

  if(sc.isFailure()){
      msg(MSG::ERROR) << "Could not retrieve LArCablingService Tool" << endreq;
      return StatusCode::FAILURE;
  }

  // Get parameters from detStore (access through abtract interfaces)
  const ILArCaliPulseParams* larCaliPulseParams = nullptr;
  const ILArDetCellParams*   larDetCellParams = nullptr;  
  const ILArTdrift*          larTdrift = nullptr;
  const ILArPhysCaliTdiff*   larPhysCaliTdiff = nullptr;

  if ( !m_useJOCaliPulseParams ) {
    sc = detStore()->retrieve(larCaliPulseParams);
    if ( sc == StatusCode::FAILURE ) {
      msg(MSG::WARNING) << "Cannot retrieve LArCaliPulseParams" << endreq ;
      return sc;
    } else {
      msg(MSG::INFO) << "LArCaliPulseParams successfully retrieved" << endreq ;
    }
  }

  if ( !m_useJODetCellParams ) {
    if (!m_isHEC) {
      sc = detStore()->retrieve(larDetCellParams);
      if ( sc == StatusCode::FAILURE ) {
	msg(MSG::WARNING) << "Cannot retrieve LArDetCellParams" << endreq;
	return sc;
      }else {
	msg(MSG::INFO) << "LArDetCellParams successfully retrieved" << endreq;
      }
    }else{
      m_useJODetCellParams = true ;
    }
  }
  
  if ( !m_useJOTdrift ) {
    sc = detStore()->retrieve(larTdrift);
    if ( sc == StatusCode::FAILURE ) {
      msg(MSG::WARNING) << "Cannot retrieve LArTdriftComplete" << endreq;
      return sc;
    }else {
      msg(MSG::INFO) << "LArTdriftComplete successfully retrieved" << endreq;
    }
  }
  
  if ( !m_useJOPhysCaliTdiff ) {
    sc = detStore()->retrieve(larPhysCaliTdiff);
    if ( sc == StatusCode::FAILURE ) {
      msg(MSG::WARNING) << "Cannot retrieve LArPhysCaliTdiff" << endreq; 
      return sc;
    }else {
      msg(MSG::INFO) << "LArPhysCaliTdiff successfully retrieved" << endreq;
    }
  }
  
  const ILArFEBTimeOffset* larFebTshift = NULL;  
  if ( m_useJOPhysCaliTdiff ) { // no LArPhysCaliTdiffComplete found, or manual time shift selected
    if ( m_timeShiftByHelper ) {
      msg(MSG::INFO) << "Will use helper class for start time." << endreq;
      m_timeShiftByIndex = -1 ;
      m_timeShiftByLayer = false ;
      m_timeShiftByFEB   = false ;
    }  
    if ( m_timeShiftByIndex != -1 ) {
      msg(MSG::INFO) << "Manually shifting pulses by time index " << m_timeShiftByIndex << endreq;
      m_timeShiftByLayer = false ;
      m_timeShiftByFEB   = false ;
    }
    if ( m_timeShiftByLayer ) {
      msg(MSG::INFO) << "Manually shifting pulses by *layer-dependent* time indexes." << endreq;
      m_timeShiftByFEB   = false ;
    }
    if ( m_timeShiftByFEB ) {
      msg(MSG::INFO) << "Manually shifting pulses by *FEB* time indexes." << endreq;
      sc = detStore()->retrieve(larFebTshift);
      if (sc.isFailure())
         larFebTshift = NULL;
    }
  }
  
  int nchannel = 0 ;
  
  // Create LArPhysWaveContainer for predicted physics waveforms
  LArPhysWaveContainer* larPhysWaveContainer = new LArPhysWaveContainer();

  sc=larPhysWaveContainer->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to set groupingType for LArPhysWaveContainer object" << endreq;
    return sc;
  }

  sc=larPhysWaveContainer->initialize(); 
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed initialize LArPhysWaveContainer object" << endreq;
    return sc;
  }   

  // Create LArMphysOverMcalComplete for predicted Mphys/Mcali
  LArMphysOverMcalComplete* MphysOverMcalComplete = new LArMphysOverMcalComplete(); 
  sc=MphysOverMcalComplete->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to set groupingType for LArMphysOverMcalComplete object" << endreq;
    return sc;
  }

  sc=MphysOverMcalComplete->initialize(); 
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed initialize LArMphysOverMcalComplete object" << endreq;
    return sc;
  }   
  
  FILE* f = NULL;
  if (m_dumpMphysMcali) {
     f = fopen("MphysOverMcali.dat","w");
     fprintf(f,"# Region Layer Eta Phi Gain  MphysMcali\n");
  }
  FileCloser fcloser (f);
  
  std::vector<int> nTotal;
  std::vector<int> noTcali;
  std::vector<int> noFstep;
  std::vector<int> noOmega0;
  std::vector<int> noTaur;
  std::vector<int> noTdrift;
  std::vector<int> noTdiff;
		  
  for ( unsigned i=0; i<CaloGain::LARNGAIN; ++i ) {
    nTotal.push_back(0);
    noTcali.push_back(0);
    noFstep.push_back(0);
    noOmega0.push_back(0);
    noTaur.push_back(0);
    noTdrift.push_back(0);
    noTdiff.push_back(0);
  }

  /////////////IDEAL PHYSWAVE/////////////////////////////
  // Get current LArPhysWaveContainer
  const LArPhysWaveContainer* larIdealPhysWaveContainer=0;
  if(m_isHEC){
    ATH_CHECK(detStore()->retrieve(larIdealPhysWaveContainer,m_keyIdealPhys));
    msg(MSG::INFO) <<"LArPhysWaveContainer with (key = " << m_keyIdealPhys << ") reading from StoreGate" << endreq;
  }
  /////////////IDEAL PHYSWAVE/////////////////////////////
    
  // get the calibration waveforms from the detector store 
  std::vector<std::string>::const_iterator key_it = m_keyCali.begin();
  std::vector<std::string>::const_iterator key_it_e = m_keyCali.end();

  int NPhysWave=0;
  int NMPMC=0;  

  for (;key_it!=key_it_e;key_it++) { // Loop over all LArCaliWave containers that are to be processed

    // Get current LArCaliWaveContainer
    const LArCaliWaveContainer* caliWaveContainer;
    sc = detStore()->retrieve(caliWaveContainer,*key_it);
    if (sc.isFailure()) {
       //log << MSG::INF0 << "LArCaliWaveContainer (key = " << *key_it << ") not found in StoreGate" << endreq;
       continue;   
    }
    if ( caliWaveContainer == NULL ) {
       msg(MSG::INFO) << "LArCaliWaveContainer (key = " << *key_it << ") is empty" << endreq;
       continue;
    }
    
    msg(MSG::INFO) << "Processing LArCaliWaveContainer from StoreGate, key = " << *key_it << endreq;
    
    for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++ gain ) { // loop over gain in the current   LArCaliWAveContainer
      
      msg(MSG::INFO) << "Now processing gain = " << gain << " in LArCaliWaveContainer with key = " << *key_it << endreq;
    
      // loop over current cali wave container
      typedef LArCaliWaveContainer::ConstConditionsMapIterator const_iterator;
      const_iterator itVec   = caliWaveContainer->begin(gain);
      const_iterator itVec_e = caliWaveContainer->end(gain);

      for (; itVec != itVec_e; ++itVec) { // loop over channels for a given gain
		
        LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it   = (*itVec).begin();
        LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it_e = (*itVec).end();

	if ( itVec == itVec_e ) {
          msg(MSG::INFO) << "LArCaliWaveContainer (key = " << *key_it << ") has no wave with gain = " << gain << endreq;
          continue ;
        }
        
        for (;cont_it!=cont_it_e;cont_it++) { // loop over DAC values for a given channel

          if ( cont_it == cont_it_e ) { 
	    ATH_MSG_DEBUG("Empty channel found in LArCaliWave container: skipping...");
	    continue ; 	  
	  } else {
	    ATH_MSG_DEBUG((*itVec).size() << " LArCaliWaves found for channel 0x" << MSG::hex << itVec.channelId() << MSG::dec);
	  }
	  nchannel++ ;
	  if ( nchannel < 100 || ( nchannel < 1000 && nchannel%100==0 ) || nchannel%1000==0 ) 
	     msg(MSG::INFO) << "Processing calibration waveform number " << nchannel << endreq;

	  const LArCaliWave& larCaliWave = (*cont_it);
	  
	  if ( larCaliWave.getFlag() == LArWave::dac0 )  continue ; // skip dac0 waves          
	  // TODO: here we should add a DAC selection mechanism for TCM method

          nTotal[gain]++; // counter of processed pulse per gain 

	  const HWIdentifier chid = itVec.channelId();
          ATH_MSG_VERBOSE("Predicting physics waveform for channel 0x" << MSG::hex << chid << MSG::dec 
			  << " (gain = " << gain << " - DAC = " << (*cont_it).getDAC() << ")");

	  // calibration pulse copy (working around the const iterator to be able to manipulate it...)
	  LArCaliWave theLArCaliWave = larCaliWave;
	  
	  // region and layer information are needed
	  Identifier id;
	  try {
            id = larCablingSvc->cnvToIdentifier(chid);
          } catch (LArID_Exception & execpt) {
	    msg(MSG::ERROR) << "LArCabling exception caught for channel 0x" << MSG::hex << chid << MSG::dec 
	        << ". Skipping channel." << endreq ;
            continue ;
	  }

	  if ( !larCablingSvc->isOnlineConnected(chid)  ) { // unconnected channel : skipping ...          
	    ATH_MSG_VERBOSE("Unconnected channel 0x" << MSG::hex << chid << MSG::dec 
			    << ". Skipping channel.");
	    continue ; 	  
	  }

	  int region = emId->region(id);
          int layer  = emId->sampling(id);

  	  // Get the parameters corresponding to current LArCaliWave
	  float Tcali;
	  float Fstep;
	  float Omega0;
	  float Taur;
	  float Tdrift;
	  int   Tdiff;
	  
	  // LArCaliPulseParams: if not fould, will use jobOptions values
	  if ( !m_useJOCaliPulseParams ) {
             Tcali = larCaliPulseParams->Tcal(chid,gain) ;

	     if ( Tcali <= 1.0+LArCaliPulseParamsComplete::ERRORCODE ) {
	       notFoundMsg(chid,gain,"Tcali");
	       Tcali = m_Tcali;
	       noTcali[gain]++;
	     }
	     Fstep = larCaliPulseParams->Fstep(chid,gain) ;
	     if ( Fstep <= 1.0+LArCaliPulseParamsComplete::ERRORCODE ) {
	       notFoundMsg(chid,gain,"Fstep");
	       Fstep = m_Fstep;
	       noFstep[gain]++;
	     }
          } else {
	    Tcali = m_Tcali;
	    Fstep = m_Fstep;
	  }
	  
	  // LArDetCellParams: if not found, will not apply injection point correction
          if ( !m_useJODetCellParams ) {
	     Omega0 = larDetCellParams->Omega0(chid,gain) ;
	     if ( Omega0 <= 1.0+LArDetCellParamsComplete::ERRORCODE ) {
	       notFoundMsg(chid,gain,"Omega0");
	       Omega0 = m_Omega0;
	       noOmega0[gain]++;
	     }
	     Taur = larDetCellParams->Taur(chid,gain) ;
	     if ( Taur <= 1.0+LArDetCellParamsComplete::ERRORCODE ) {
	       notFoundMsg(chid,gain,"Taur");
	       Taur = m_Taur;
	       noTaur[gain]++;
	     }
	  } else {
	    Omega0 = m_Omega0;
	    Taur   = m_Taur;
	  }
	  
          // LArTdrift: if not found will be set to jobOptions settings (or defaults if none) according to layer
	  if ( !m_useJOTdrift ) {
	    Tdrift = larTdrift->Tdrift(chid) ;
	     if ( Tdrift <= 1.0+LArTdriftComplete::ERRORCODE ) {
	       notFoundMsg(chid,gain,"Tdrift");
	       //msg(MSG::ERROR) << "Cannot access Tdrift for channel 0x" << MSG::hex << chid << MSG::dec 
	       //   << ". Will use jobOption setting." << endreq ;
		if ( layer>=0 && layer<4 ) 
		  Tdrift = m_Tdrift[layer]; // Set drift time according to layer
		else 
		  Tdrift = 475.;  // very crude! 
		noTdrift[gain]++;
	     }
	  } else { // use jobOptions settings
	    if ( layer>=0 && layer<4 ) 
	      Tdrift = m_Tdrift[layer]; // Set drift time according to layer
	    else 
	      Tdrift = 475.;  // very crude! 
	  }

	  
          // LArPhysCaliTdiff: if not found, will use jobOptions settings
	  Tdiff = 0 ; // default value if everything else fails...
	  if ( !m_useJOPhysCaliTdiff ) {
	     Tdiff = (int)larPhysCaliTdiff->Tdiff(chid,gain) ;
	     if ( Tdiff <= 1.0+LArPhysCaliTdiffComplete::ERRORCODE ) {
	       notFoundMsg(chid,gain,"Tdiff");
	       //msg(MSG::WARNING) << "Cannot access Tdiff for channel 0x" << MSG::hex << chid << MSG::dec 
	       //                    << ", gain = " << gain << ". Will use jobOption setting." << endreq ;
	        Tdiff = 0 ;
		m_useJOPhysCaliTdiff = true ;
		noTdiff[gain]++;
	     }
          }
	  if ( m_useJOPhysCaliTdiff ) {
	    if ( m_timeShiftByHelper ) {
	      Tdiff  = larWaveHelper.getStart(theLArCaliWave) ;
	      Tdiff -= m_timeShiftGuardRegion ;
	    }
	    if ( m_timeShiftByIndex != 0 ) {
	      Tdiff = m_timeShiftByIndex;
	    }
	    if ( m_timeShiftByLayer ) {
	      Tdiff = m_TshiftLayer[layer] ;
	    }
	    if ( m_timeShiftByFEB && larFebTshift ) {
	      const HWIdentifier febid = m_onlineHelper->feb_Id(chid);
	      Tdiff  = (int)larFebTshift->TimeOffset(febid);
	      Tdiff -= m_timeShiftGuardRegion ;
	    }
          }

          // Fill the LArWFParams structure to be used by the LArPhysWaveTool
          float Tshaper  = 15. ;
	  float Amplitude = 1. ;
          LArWFParams wfParams(Tcali,Fstep,Tdrift,Omega0,Taur,Tshaper,Amplitude);
          wfParams.setFlag( 0 ) ;  // this should contain the method used to find parameters and the gain
          wfParams.setTdiff(Tdiff);
	  	  
          // calibration pulse normalization 
	  // (should be done here instead than in LArPhysWaveTool to get 
	  // the correct Mphys/Mcali in case of double triangle prediction)
	  if ( m_normalizeCali ) {
	    double peak = theLArCaliWave.getSample(larWaveHelper.getMax(theLArCaliWave));
            ATH_MSG_VERBOSE("Channel 0x" << MSG::hex << chid << MSG::dec << " -> Applying normalisation (CaliWave peak = " << peak << ")");
            if ( peak <=0 ) {
               msg(MSG::WARNING) << "Peak value <=0 , cannot normalize!" << endreq ;
            } else {
               theLArCaliWave = LArCaliWave( (theLArCaliWave*(1./peak)).getWave(),
					     theLArCaliWave.getErrors(),
					     theLArCaliWave.getTriggers(),
					     theLArCaliWave.getDt(), 
					     theLArCaliWave.getDAC(), 
					     theLArCaliWave.getFlag() );
            }
          } 
	  
	  //
	  // Predict the Physics Waveform
	  //      
	  LArPhysWave larPhysWave;
	  float MphysMcali ;	
	  if(larIdealPhysWaveContainer && emId->is_lar_hec(id)) {
	    const LArPhysWave& laridealPhysWave = larIdealPhysWaveContainer -> get(chid,gain);
	    int LArWaveFlag=LArWave::predCali;    // 111 - for HEC Wave
	    //int LArIdealPhysWaveFlag=LArWave::predCali;    // 111 - for HEC Wave
	    
	    sc = larPhysWaveHECTool->makeLArPhysWaveHEC(wfParams,theLArCaliWave,larPhysWave,laridealPhysWave,MphysMcali,chid,gain,LArWaveFlag);
	    
	    //laridealPhysWave.setFlag( LArIdealPhysWaveFlag ) ;
	  }
	  else 
	    sc = larPhysWaveTool->makeLArPhysWave(wfParams,theLArCaliWave,region,layer,larPhysWave,MphysMcali);
	  if (sc.isFailure()) {
	    msg(MSG::FATAL) << "Cannot predict LArPhysWave for channel 0x" << MSG::hex << chid << MSG::dec << endreq;
	    continue;
	  }
	  larPhysWave.setFlag( LArWave::predCali ) ;

	  if ( m_doubleTriangle ) { // mix of Tdrift ...
	    ATH_MSG_DEBUG("Applying double triangle correction");
	    if ( region==0 && layer>=0 && layer<4 && m_wTriangle2[layer]>0 ) {  // EMB: set drift times and weight according to layer
	      LArWFParams wfParams2 = wfParams ;
	      wfParams2.setTdrift(m_Tdrift2[layer]);
	      LArPhysWave larPhysWave2;
	      float MphysMcali2 ;
	      sc = larPhysWaveTool->makeLArPhysWave(wfParams2,theLArCaliWave,region,layer,larPhysWave2,MphysMcali2);
	      if (sc.isFailure()) {
                msg(MSG::FATAL) << "Cannot predict LArPhysWave for channel 0x" << MSG::hex << chid << MSG::dec << "with double triangle."  << endreq;
                continue;
	      }
	      larPhysWave = LArPhysWave ( ( larPhysWave*(1.-m_wTriangle2[layer])+larPhysWave2*m_wTriangle2[layer] ).getWave() ,
			                    larPhysWave.getDt(), 
			                    larPhysWave.getFlag() ) ;
	      // Double-triagle predicion was used, Mphys/Mcali should be recomputed... 
	      MphysMcali = larPhysWave.getSample(larWaveHelper.getMax(larPhysWave))/theLArCaliWave.getSample(larWaveHelper.getMax(theLArCaliWave));
	    } else {
	      msg(MSG::WARNING) << "Double triangle implemented only for EMB, skip channel!" << endreq ;
	    }
	  } // end if 2nd triangle
          
	  // Apply time shift...
	  if ( Tdiff !=0 ) {
	    ATH_MSG_DEBUG("Time shift for channel " << (itVec.channelId()).get_compact() << " is " 
			  << Tdiff << " samples (" << Tdiff*larPhysWave.getDt() << " ns)");
	    larPhysWave = LArPhysWave( (larWaveHelper.translate(larPhysWave,-Tdiff,0)).getWave() ,
                                       larPhysWave.getDt(), 
				       larPhysWave.getFlag() ) ;
	  }
          
	  // Add predicted physics wave to container
          larPhysWaveContainer->setPdata(chid,larPhysWave,gain);
          NPhysWave++; 

	  // Add Mphys/Mcali to container
	  if (MphysMcali<=0.) {
	     MphysMcali = LArMphysOverMcalComplete::ERRORCODE ;
	  }
	  ATH_MSG_VERBOSE("Channel 0x" << MSG::hex << chid << MSG::dec << " -> Mphys/Mcali = " << MphysMcali);
	  MphysOverMcalComplete->set(chid,gain,MphysMcali);
	  NMPMC++;
	  if ( m_dumpMphysMcali ) { 
	     int eta = emId->eta(id);
	     int phi = emId->phi(id);
	     fprintf( f , "%2d %2d %3d %3d  %2d %8.3f \n", region, layer, eta, phi, gain, MphysMcali ) ;
	  }
	  
	  if ( m_testmode && nchannel>=10 ) {

	    msg(MSG::INFO) << "Test mode selected, process only one channel!" << endreq ;
	    
	    // Record LArPhysWaveContainer to DetectorStore
  	    sc = detStore()->record(larPhysWaveContainer,m_keyPhys);
	    if (sc.isFailure()) {
	      msg(MSG::FATAL) << "Cannot record LArPhysWaveContainer to StoreGate! key=" << m_keyPhys << endreq;
	      return StatusCode::FAILURE;
            }

            // Record LArMphysOverMcalComplete to DetectorStore
            sc = detStore()->record(MphysOverMcalComplete,m_keyMphysMcali); 
            if (sc.isFailure()) {
              msg(MSG::FATAL) << "Cannot record LArMphysOverMcalComplete to StoreGate! key=" << m_keyMphysMcali << endreq;
              return StatusCode::FAILURE;
            }
  
            // Symlink LArMphysOverMcalComplete to ILArMphysOverMcal for further use
            ATH_MSG_DEBUG("Trying to symlink ILArMphysOverMcal with LArMphysOverMcalComplete...");
            ILArMphysOverMcal *larMphysOverMcal = NULL;
            sc = detStore()->symLink(MphysOverMcalComplete,larMphysOverMcal);
            if (sc.isFailure()) {
              msg(MSG::FATAL) << "Could not symlink ILArMphysOverMcal with LArMphysOverMcalComplete." << endreq;
              return StatusCode::FAILURE;
            } 
            msg(MSG::INFO) << "ILArMphysOverMcal symlink with LArMphysOverMcalComplete successfully" << endreq ;

	    return StatusCode::SUCCESS; 

          } // end of if ( m_testmode && nchannel>=1 )

        } // end loop over DAC value for a given cell

	// intermediate map cleanup (save memory)
	const LArCaliWaveVec* cvec = &(*itVec);
        LArCaliWaveVec* vec = const_cast<LArCaliWaveVec*>(cvec);
        vec->clear();

      }  // end loop over cells for a given gain

    } // end loop over gains for a give container
  
  }  // End loop over all CaliWave containers

  //msg(MSG::INFO) << " Summary : Number of cells with a PhysWave values computed : " << larPhysWaveContainer->totalNumberOfConditions()  << endreq;
  //msg(MSG::INFO) << " Summary : Number of cells with a MphysOverMcal values computed : " << MphysOverMcalComplete->totalNumberOfConditions()  << endreq;
  msg(MSG::INFO) << " Summary : Number of cells with a PhysWave values computed : " << NPhysWave  << endreq;
  msg(MSG::INFO) << " Summary : Number of cells with a MphysOverMcal values computed : " << NMPMC  << endreq;
  msg(MSG::INFO) << " Summary : Number of Barrel PS cells side A or C (connected+unconnected):  3904+ 192 " << endreq;
  msg(MSG::INFO) << " Summary : Number of Barrel    cells side A or C (connected+unconnected): 50944+2304 " << endreq;
  msg(MSG::INFO) << " Summary : Number of EMEC      cells side A or C (connected+unconnected): 31872+3456 " << endreq;
  msg(MSG::INFO) << " Summary : Number of HEC       cells side A or C (connected+unconnected):  2816+ 256 " << endreq;
  msg(MSG::INFO) << " Summary : Number of FCAL      cells side A or C (connected+unconnected):  1762+  30 " << endreq;

  ATH_MSG_DEBUG("LArPhysWaveContainer->totalNumberOfConditions()     = " <<  larPhysWaveContainer->totalNumberOfConditions());
  ATH_MSG_DEBUG("LArMphysOverMcalComplete->totalNumberOfConditions() = " <<  MphysOverMcalComplete->totalNumberOfConditions());

  // final report
 msg(MSG::INFO) << "\n Final report \n" << endreq;
  for ( unsigned theGain = CaloGain::LARHIGHGAIN ; theGain < CaloGain::LARNGAIN ; ++theGain ) { 

    msg(MSG::INFO) << " *** Gain = " << theGain << " ***" << endreq;
    if ( !m_useJOCaliPulseParams ) {
       msg(MSG::INFO) << "\t" << noTcali[theGain]  << " / " << nTotal[theGain] << " channel(s) missing Tcali"  << endreq;
       msg(MSG::INFO) << "\t" << noFstep[theGain]  << " / " << nTotal[theGain] << " channel(s) missing Fstep"  << endreq;
    }
    if ( !m_useJODetCellParams ) {  
       msg(MSG::INFO) << "\t" << noOmega0[theGain] << " / " << nTotal[theGain] << " channel(s) missing Omega0"  << endreq;
       msg(MSG::INFO) << "\t" << noTaur[theGain]   << " / " << nTotal[theGain] << " channel(s) missing Taur"    << endreq;
    }
    if ( !m_useJOTdrift )
       msg(MSG::INFO) << "\t" << noTdrift[theGain] << " / " << nTotal[theGain] << " channel(s) missing Tdrift" << endreq;
    if ( !m_useJOPhysCaliTdiff )
       msg(MSG::INFO) << "\t" << noTdiff[theGain]  << " / " << nTotal[theGain] << " channel(s) missing Tdiff"  << endreq;	    
  }
  msg(MSG::INFO) << "\n" << endreq;

  // Record LArPhysWaveContainer to DetectorStore
  sc = detStore()->record(larPhysWaveContainer,m_keyPhys);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot record LArPhysWaveContainer to StoreGate! key=" << m_keyPhys << endreq;
    return StatusCode::FAILURE;
  }

  // Record LArMphysOverMcalComplete to DetectorStore
  sc = detStore()->record(MphysOverMcalComplete,m_keyMphysMcali); 
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot record LArMphysOverMcalComplete to StoreGate! key=" << m_keyMphysMcali << endreq;
    return StatusCode::FAILURE;
  }
  
  // Symlink LArMphysOverMcalComplete to ILArMphysOverMcal for further use
  ATH_MSG_DEBUG("Trying to symlink ILArMphysOverMcal with LArMphysOverMcalComplete...");
  ILArMphysOverMcal *larMphysOverMcal = NULL;
  sc = detStore()->symLink(MphysOverMcalComplete,larMphysOverMcal);
  if (sc.isFailure()) {
      msg(MSG::FATAL) << "Could not symlink ILArMphysOverMcal with LArMphysOverMcalComplete." << endreq;
      return StatusCode::FAILURE;
  } 
  msg(MSG::INFO) << "ILArMphysOverMcal symlink with LArMphysOverMcalComplete successfully" << endreq ;
  
  msg(MSG::INFO) << "LArPhysWavePredictor finalized!" << endreq;  
  
  return StatusCode::SUCCESS;
}

void LArPhysWavePredictor::notFoundMsg(const HWIdentifier chid, const int gain, const char* value) {
  if (m_maskingTool->cellShouldBeMasked(chid,gain))
    msg(MSG::WARNING) << "Cannot access " << value << " for known bad channel channel " << m_onlineHelper->channel_name(chid)
		      << ", gain = " << gain << ". Will use jobO setting." << endreq ;
  else {    
    LArBadChanBitPacking packer;
    const LArBadChannel bc=m_badChanTool->status(chid);
    const std::string badChanStatus=packer.stringStatus(bc);

    msg(MSG::ERROR) << "Cannot access " << value << " for channel " << m_onlineHelper->channel_name(chid) 
		    << ", gain = " << gain << " BC status=[" << badChanStatus << "]. Will use jobO setting." << endreq ;
  }
  return;
}

