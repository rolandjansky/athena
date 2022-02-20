/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArRTMParamExtractor.h"

#include "LArCalibUtils/LArWFParamTool.h"

#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArCaliWave.h"

#include "LArElecCalib/ILArCaliPulseParams.h"
#include "LArElecCalib/ILArDetCellParams.h"

#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "tbb/parallel_for.h"

LArRTMParamExtractor::LArRTMParamExtractor (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_groupingType("FeedThrough") // SubDetector, Single, FeedThrough
{
  declareProperty("KeyList"   ,m_keylist);
  declareProperty("TestMode"  ,m_testmode = false);
  declareProperty("IgnoreDACSelection", m_ignoreDACselection = true);
  declareProperty("isSC", m_isSC = false);

  m_DAC.clear();
  int default_DAC[3] = { -1, -1, -1} ;
  for ( unsigned i=0;i<3;i++) 
     m_DAC.push_back(default_DAC[i]);
  declareProperty("DAC"           ,m_DAC);
  
  declareProperty("ExtractTcal"   ,m_extractTcal   = true) ;
  declareProperty("ExtractFstep"  ,m_extractFstep  = true) ;
  declareProperty("ExtractOmega0" ,m_extractOmega0 = true) ;
  declareProperty("ExtractTaur"   ,m_extractTaur   = true) ;
  declareProperty("DefaultTcal"   ,m_defaultTcal   = LArWFParamTool::DefaultNotSet ) ;
  declareProperty("DefaultFstep"  ,m_defaultFstep  = LArWFParamTool::DefaultNotSet ) ;
  declareProperty("DefaultOmega0" ,m_defaultOmega0 = LArWFParamTool::DefaultNotSet ) ;
  declareProperty("DefaultTaur"   ,m_defaultTaur   = LArWFParamTool::DefaultNotSet ) ;
  declareProperty("RecoverEmptyDB",m_recoverEmptyDB = false);
  declareProperty("SuffixRetrievedCaliPulse" , m_suffixRetrievedCaliPulse = ""     ) ;
  declareProperty("SuffixRetrievedDetCell"   , m_suffixRetrievedDetCell   = ""     ) ;
  declareProperty("SuffixExtractedCaliPulse" , m_suffixExtractedCaliPulse = "_RTM" ) ;
  declareProperty("SuffixExtractedDetCell"   , m_suffixExtractedDetCell   = "_RTM" ) ;
  
  declareProperty("DumpOmegaScan", m_dumpOmegaScan = false ) ;
  declareProperty("OmegaScanKey",  m_omegaScanKey = "OmegaScan" ) ;
  
  declareProperty("DumpResOscill", m_dumpResOscill = false ) ;
  declareProperty("ResOscillKeyBefore",  m_resOscillKeyBefore = "ResOscillBefore" ) ;
  declareProperty("ResOscillKeyAfter",   m_resOscillKeyAfter  = "ResOscillAfter" ) ;  
  declareProperty("GroupingType",  m_groupingType);
  

  declareProperty("FT",            m_FT);
  declareProperty("PosNeg",        m_PosNeg=0);
  declareProperty("Slot",          m_Slot);

  declareProperty("calibLineSelection", m_Calibselection = false);
  declareProperty("cLineGroup",         m_Cline=0);
  
  declareProperty("useTBB",            m_useTBB = false);
}

LArRTMParamExtractor::~LArRTMParamExtractor() {}

StatusCode LArRTMParamExtractor::initialize() {
  ATH_MSG_INFO( "in initialize()");

  m_keyRetrievedCaliPulse = "LArCaliPulseParams" + m_suffixRetrievedCaliPulse ;
  m_keyRetrievedDetCell   = "LArDetCellParams"   + m_suffixRetrievedDetCell ;

  m_keyExtractedCaliPulse = "LArCaliPulseParams" + m_suffixExtractedCaliPulse ;
  m_keyExtractedDetCell   = "LArDetCellParams"   + m_suffixExtractedDetCell ;

  // output new keywords:
  // -------------------
  ATH_MSG_INFO( "new LArCaliPulseParams will be recorded with key=" << m_keyExtractedCaliPulse ) ;
  ATH_MSG_INFO( "  new LArDetCellParams will be recorded with key=" << m_keyExtractedDetCell ) ;

  // output selected action for Tcal   :
  // ---------------------------------
  if ( m_defaultTcal   != LArWFParamTool::DefaultNotSet ) {
    ATH_MSG_INFO( "Tcal   defaulted to " << m_defaultTcal   << " ; use with CARE!" ) ;
  } else if ( m_extractTcal ) {
    ATH_MSG_INFO( "Tcal   will be extracted" ) ;
  } else {
    //ATH_MSG_INFO( "Tcal   will be read from DB/DetStore with key=" << m_keyRetrievedCaliPulse ) ;
    ATH_MSG_INFO( "Tcal   will be read from DB/DetStore" ) ;
  }

  // output selected action for Fstep  :
  // ---------------------------------
  if ( m_defaultFstep  != LArWFParamTool::DefaultNotSet ) {
    ATH_MSG_INFO( "Fstep  defaulted to " << m_defaultFstep  << " ; use with CARE!" ) ;
  } else if ( m_extractFstep  ) {
    ATH_MSG_INFO( "Fstep  will be extracted" ) ;
  } else {
    //ATH_MSG_INFO( "Fstep  will be read from DB/DetStore with key=" << m_keyRetrievedCaliPulse ) ;
    ATH_MSG_INFO( "Fstep  will be read from DB/DetStore" ) ;
  }

  // output selected action for Omega0 :
  // ---------------------------------
  if ( m_defaultOmega0 != LArWFParamTool::DefaultNotSet ) {
    ATH_MSG_INFO( "Omega0 defaulted to " << m_defaultOmega0 << " ; use with CARE!" ) ;
  } else if ( m_extractOmega0 ) {
    ATH_MSG_INFO( "Omega0 will be extracted" ) ;
  } else {
    //ATH_MSG_INFO( "Omega0 will be read from DB/DetStore with key=" << m_keyRetrievedDetCell ) ;
    ATH_MSG_INFO( "Omega0 will be read from DB/DetStore ") ;
  }

  // output selected action for Taur   :
  // ---------------------------------
  if ( m_defaultTaur   != LArWFParamTool::DefaultNotSet ) {
    ATH_MSG_INFO( "Taur   defaulted to " << m_defaultTaur   << " ; use with CARE!" ) ;
  } else if ( m_extractTaur   ) {
    ATH_MSG_INFO( "Taur   will be extracted" ) ;
  } else {
    //ATH_MSG_INFO( "Taur   will be read from DB/DetStore with key=" << m_keyRetrievedDetCell ) ;
    ATH_MSG_INFO( "Taur   will be read from DB/DetStore" ) ;
  }

  // if no Omega0 extraction, dumping Omega scan and Residual oscillation doesn't make sense!
  if ( ! m_extractOmega0 ) {
    m_dumpOmegaScan = false ;
    m_dumpResOscill = false ;
  }

  if ( !m_FT.empty() ) {
    msg(MSG::INFO) << "FT selection enabled, will only process data from FT = [ ";
    for(unsigned i=0; i<m_FT.size()-1; ++i)
      msg() <<  m_FT[i] << ", ";
    ATH_MSG_INFO( m_FT[m_FT.size()-1] << " ] at PosNeg = " << m_PosNeg );
  }

  if ( !m_Slot.empty() ) {
    msg(MSG::INFO) << "Slot selection enabled, will only process data from Slot = [ ";
    for(unsigned i=0; i<m_Slot.size()-1; ++i)
      msg() << m_Slot[i] << ", ";
    msg()<<  m_Slot[m_Slot.size()-1] <<"]" << endmsg;
  }

  if (m_ignoreDACselection) 
    ATH_MSG_INFO( "Will ignore DAC selection and use first value found per channel per gain" );

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_cablingKeySC.initialize(m_isSC) );

  // Retrieve LArWFParamTool
  ATH_CHECK(m_larWFParamTool.retrieve());

  return StatusCode::SUCCESS ;
}

StatusCode LArRTMParamExtractor::stop()
{
  ATH_MSG_INFO( "in stop()" ) ;
  
  // get LArOnlineID helper
  const LArOnlineID_Base* onlineHelper;
  /*StatusCode sc = detStore()->retrieve(onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID" );
    return sc;
  }*/
  StatusCode sc;
  if ( m_isSC ) {
    const LArOnline_SuperCellID* ll;
    sc = detStore()->retrieve(ll, "LArOnline_SuperCellID");
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      onlineHelper = (const LArOnlineID_Base*)ll;
      ATH_MSG_DEBUG("Found the LArOnlineID helper");
    }
  } else { // m_isSC
    const LArOnlineID* ll;
    sc = detStore()->retrieve(ll, "LArOnlineID");
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      onlineHelper = (const LArOnlineID_Base*)ll;
      ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
    }
  }

  

  const LArOnOffIdMapping* cabling(0);
  if( m_isSC ){
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKeySC};
    cabling = {*cablingHdl};
    if(!cabling) {
	ATH_MSG_ERROR("Do not have mapping object " << m_cablingKeySC.key());
        return StatusCode::FAILURE;
    }
  }else{
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    cabling = {*cablingHdl};
    if(!cabling) {
       ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
       return StatusCode::FAILURE;
    }
  }


      
  // retrieve previous complete objects from DetStore, if needed
  // -----------------------------------------------------------

  // use complete objects
  //const LArCaliPulseParamsComplete* prevCaliPulseParams ;
  //const LArDetCellParamsComplete*   prevDetCellParams ;
  
  // use abstract interfaces
  const ILArCaliPulseParams* prevCaliPulseParams = nullptr;
  const ILArDetCellParams*   prevDetCellParams = nullptr;

  if ( ! ( m_extractTcal && m_extractFstep ) ) {
    //sc = detStore()->retrieve(prevCaliPulseParams,m_keyRetrievedCaliPulse);
    sc = detStore()->retrieve(prevCaliPulseParams);
    if ( sc.isFailure() ) {
      //ATH_MSG_ERROR( "Can't get CaliPulseParams from DetStore, key=" << m_keyRetrievedCaliPulse );
      ATH_MSG_ERROR( "Can't get CaliPulseParams from DetStore" ) ;
      ATH_MSG_ERROR( "(was requested for" ;
      if ( ! m_extractTcal )   msg() << " Tcal" ;
      if ( ! m_extractFstep )  msg() << " Fstep" ;
      msg() << ")" ) ;
      return sc ;
    }
    //ATH_MSG_INFO( "CaliPulseParams retrieved from DetStore, key=" << m_keyRetrievedCaliPulse ) ;
    ATH_MSG_INFO( "CaliPulseParams retrieved from DetStore");
  }
  
  if ( ! ( m_extractOmega0 && m_extractTaur ) ) {
    //sc = detStore()->retrieve(prevDetCellParams,m_keyRetrievedDetCell);
    sc = detStore()->retrieve(prevDetCellParams);
    if ( sc.isFailure() ) {
      //ATH_MSG_ERROR( "Can't get DetCellParams from DetStore, key=" << m_keyRetrievedDetCell ) ;
      ATH_MSG_ERROR( "Can't get DetCellParams from DetStore" ) ;
      ATH_MSG_ERROR( "(was requested for" ;
      if ( ! m_extractOmega0 ) msg() << " Omega0" ;
      if ( ! m_extractTaur )   msg() << " Taur" ;
      msg() << ")" ) ;
      return sc ;
    }
    //ATH_MSG_INFO( "DetCellParams retrieved from DetStore, key=" << m_keyRetrievedDetCell ) ;
    ATH_MSG_INFO( "DetCellParams retrieved from DetStore");
  }


  // create and initialize new complete objects
  // ------------------------------------------
  std::unique_ptr<LArCaliPulseParamsComplete> newCaliPulseParams=std::make_unique<LArCaliPulseParamsComplete>() ;

  sc=newCaliPulseParams->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to set groupingType for LArCaliPulseParams object" );
    return sc;
  }
  sc=newCaliPulseParams->initialize();
   if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to initialize for LArCaliPulseParams object" );
    return sc;
  }

  std::unique_ptr<LArDetCellParamsComplete> newDetCellParams   = std::make_unique<LArDetCellParamsComplete>();
  sc=newDetCellParams->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to set groupingType for LArDetCellParamsComplete object" );
    return sc;
  }
  sc=newDetCellParams->initialize();
   if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to initialize LArDetCellParamsComplete object" );
    return sc;
  }

  // needed for omega scan dump:
  // use LArCaliWave container to store frequency spectra
  std::unique_ptr<LArCaliWaveContainer> omegaScanContainer;
  if ( m_dumpOmegaScan ) {
    omegaScanContainer = std::make_unique<LArCaliWaveContainer>();
    sc=omegaScanContainer->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArCaliWaveContainer object" );
      return sc;
    }
   sc=omegaScanContainer->initialize();
   if (sc.isFailure()) {
     ATH_MSG_ERROR( "Failed to initialize LArCaliWaveContainer object" );
     return sc;
   }
  }// end if m_dumpOmegaScan

  // needed for residual oscillation dump:
  std::unique_ptr<LArCaliWaveContainer> resOscillContainerBefore;
  std::unique_ptr<LArCaliWaveContainer> resOscillContainerAfter;
  if ( m_dumpResOscill ) {
    resOscillContainerBefore = std::make_unique<LArCaliWaveContainer>();
    resOscillContainerAfter  = std::make_unique<LArCaliWaveContainer>();
    if(resOscillContainerBefore->setGroupingType(m_groupingType,msg()).isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArCaliWaveContainer object" );
      return StatusCode::FAILURE;
    }
    if(resOscillContainerBefore->initialize().isFailure()) {
      ATH_MSG_ERROR( "Failed to initialize LArCaliWaveContainer object" );
      return StatusCode::FAILURE;
    }

    if(resOscillContainerAfter->setGroupingType(m_groupingType,msg()).isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArCaliWaveContainer object" );
      return StatusCode::FAILURE;
    }
    if(resOscillContainerAfter->initialize().isFailure()) {
      ATH_MSG_ERROR( "Failed to initialize LArCaliWaveContainer object" );
      return StatusCode::FAILURE;
    }
  }

  // needed for existence checks:
  static const LArCaliPulseParamsComplete::LArCondObj emptyCaliPulse ;
  static const LArDetCellParamsComplete::LArCondObj   emptyDetCell ;

  // prepare for nested loops: { containers { gains { channels { DACs } } } }
  // ------------------------
  
  int nchannel = 0 ;

  // get the waveforms from the detector store, according to key list
  
  int NCalibParams=0;
  int NDetParams=0;

  unsigned nWaveConts=0;

  //Collect all input params into this flat vector:
  std::vector<helperParams> inputParams;

  for (const std::string& key : m_keylist) { //Loop over all containers that are to be processed 
    
    // Get current LArCaliWaveContainer
    const LArCaliWaveContainer* caliWaveContainer;
    sc = detStore()->retrieve(caliWaveContainer,key);
    if (sc.isFailure()) {
      ATH_MSG_INFO( "LArCaliWaveContainer (key = " << key << ") not found in StoreGate");
      continue;   
    }
    if ( caliWaveContainer == NULL ) {
      ATH_MSG_INFO( "LArCaliWaveContainer (key = " << key << ") is empty");
      continue;
    }
    
    ATH_MSG_INFO( "Processing LArCaliWaveContainer from StoreGate, key = " << key);
    ++nWaveConts;
    
    for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++ gain ) {
      
      typedef LArCaliWaveContainer::ConstConditionsMapIterator const_iterator;
      const_iterator itVec   = caliWaveContainer->begin(gain);
      const_iterator itVec_e = caliWaveContainer->end(gain);
      
      if ( itVec == itVec_e ) {
	ATH_MSG_INFO( "LArCaliWaveContainer (key=" << key << ") has no channels with gain = " << gain);
	continue ;
      }
      ATH_MSG_INFO( "Now processing gain = " << gain << " in LArCaliWaveContainer with key=" << key);

      for (; itVec != itVec_e; ++itVec) { // loop over channels for a given gain
	
        LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it   = (*itVec).begin();
        LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it_e = (*itVec).end();
        
	if ( cont_it == cont_it_e ) { 
	  ATH_MSG_DEBUG(  "No DACs found for channel 0x" << MSG::hex << itVec.channelId() << MSG::dec);
	  continue ; 	  
	} else {
	  ATH_MSG_DEBUG( (*itVec).size() << " DACs found for channel 0x" << MSG::hex << itVec.channelId()<< MSG::dec);
	}
        
	// FT selection
	if ( !m_FT.empty() ) {
          int PosNeg    = onlineHelper->pos_neg(itVec.channelId());
          int FT        = onlineHelper->feedthrough(itVec.channelId());
	  std::vector<int>::const_iterator selectFT = std::find(m_FT.begin(),m_FT.end(),FT);
	  if ( PosNeg!=m_PosNeg || selectFT==m_FT.end() ) {
	    ATH_MSG_DEBUG( "Channel 0x" << MSG::hex << itVec.channelId() << MSG::dec 
			   << " does not match FT selection (FT = " << FT << ", PosNeg = " << PosNeg 
			   << "), skipping...");
	    continue;
	  } else {
	    ATH_MSG_INFO( "Channel 0x" << MSG::hex << itVec.channelId() << MSG::dec 
			  << " matches FT selection (FT = " << FT << ", PosNeg = " << PosNeg << ")");
	  }
	}
	
	// Slot selection
	if ( !m_Slot.empty() ) {
          int Slot = onlineHelper->slot(itVec.channelId());
	  std::vector<int>::const_iterator selectSlot = std::find(m_Slot.begin(),m_Slot.end(),Slot);
	  if ( selectSlot==m_Slot.end() ) { 
	    ATH_MSG_DEBUG( "Channel 0x" << MSG::hex << itVec.channelId() << MSG::dec 
			   << " does not match Slot selection (Slot = " << Slot << "), skipping...");
	    continue; 
	  } else {
	    ATH_MSG_INFO( "Channel 0x" << MSG::hex << itVec.channelId() << MSG::dec 
			  << " matches Slot selection (Slot = " << Slot << ")");
	  }
	}
		
	unsigned nDACproc = 0;

	for (;cont_it!=cont_it_e;++cont_it) { // loop over DAC values for a given channel
	  
	  // get current cali wave:
	  // ---------------------
	  const LArCaliWave& larCaliWave = (*cont_it);
	  const HWIdentifier& chid = itVec.channelId() ;
	
	  //if ( larCaliWave.getFlag() == LArWave::dac0 )  continue ; // skip dac0 waves  
	  
	  if ( !m_ignoreDACselection ) { // use jobOptions DAC selection
	    if ( larCaliWave.getDAC() != m_DAC[gain] ) {
	      ATH_MSG_VERBOSE( "Skipping DAC = " << larCaliWave.getDAC() 
			       << " for channel " << MSG::hex << chid << MSG::dec 
			       << " in Gain = " << gain);
	      continue;
	    } 
	  } else { // DAC selection will be ignored, will be used first value (if found)
	    if ( nDACproc>0 )
	      continue;
	  }

          // calibLine selection
          if(m_Calibselection) {
             if( !( (larCaliWave.getIsPulsedInt()>>m_Cline) & 1) ) {
                    ATH_MSG_DEBUG( "Channel 0x" << MSG::hex << itVec.channelId() << 
				   "isPulsedInt 0x"<<(larCaliWave.getIsPulsedInt()&0xF)<<MSG::dec 
				   << " not match group "<<m_Cline<<" skipping...");
                    continue; 
             }
          }//end if calibLine selection

   	  nDACproc++;
	  
	  nchannel++ ;
	  if ( nchannel < 100 || ( nchannel < 1000 && nchannel%100==0 ) || nchannel%1000==0 ) 
	    ATH_MSG_INFO( "Ingesting calibration waveform number " << nchannel);

	  inputParams.emplace_back(&larCaliWave,chid,gain);
          if ( omegaScanContainer ) inputParams.back().omegaScan.emplace();
          if ( resOscillContainerBefore ) inputParams.back().resOscill0.emplace();
          if ( resOscillContainerAfter ) inputParams.back().resOscill1.emplace();
	  LArWFParams& wfParams=inputParams.back().wfParams ;
	  float retrievedParam ;

	  ATH_MSG_VERBOSE("Extracting parameters for channel " << MSG::hex << chid << MSG::dec 
			  << " in Gain = " << gain << " at DAC = " << larCaliWave.getDAC());

	  if ( m_defaultTcal != LArWFParamTool::DefaultNotSet ) {
	    wfParams.setTcal( m_defaultTcal ) ;
	  } else if ( m_extractTcal ) {
	    wfParams.setTcal( LArWFParamTool::DoExtract ) ;
	  } else {
	    wfParams.setTcal( retrievedParam = prevCaliPulseParams->Tcal(chid,gain) ) ;
	    if ( retrievedParam == emptyCaliPulse.m_Tcal ) {
	      ATH_MSG_WARNING( "Parameters Tcal requested from DB but not found for channel " 
			       << onlineHelper->channel_name(chid) 
				<< " gain=" << gain << " DAC=" << larCaliWave.getDAC());
	      if (m_recoverEmptyDB) {
	        wfParams.setTcal( LArWFParamTool::DoExtract ) ;
	        ATH_MSG_WARNING( " -> Recovering with RTM extraction." );
	      } else {	
                ATH_MSG_WARNING( " -> Skipping cell!" );
	        continue ; 
	      }
	    }
	  }
	  
	  if ( m_defaultFstep != LArWFParamTool::DefaultNotSet ) {
	    wfParams.setFstep( m_defaultFstep ) ;
	  } else if ( m_extractFstep ) {
	    wfParams.setFstep( LArWFParamTool::DoExtract ) ;
	  } else {
	    wfParams.setFstep( retrievedParam = prevCaliPulseParams->Fstep(chid,gain) ) ;
	    if ( retrievedParam == emptyCaliPulse.m_Fstep ) {
	      ATH_MSG_WARNING( "Parameters Fstep requested from DB but not found for channel " 
			       << onlineHelper->channel_name(chid)  
			       << " gain=" << gain << " DAC=" << larCaliWave.getDAC());
	      if (m_recoverEmptyDB) {
	        ATH_MSG_WARNING( " -> Recovering with RTM extraction." );
	        wfParams.setFstep( LArWFParamTool::DoExtract ) ;
	      } else {
      	      	ATH_MSG_WARNING( " -> Skipping cell!" ); 
	        continue ; 
	      } 
	    }
	  }

	  if ( m_defaultOmega0 != LArWFParamTool::DefaultNotSet ) {
	    wfParams.setOmega0( m_defaultOmega0 ) ;
	  } else if ( m_extractOmega0 ) {
	    wfParams.setOmega0( LArWFParamTool::DoExtract ) ;
	  } else {
	    wfParams.setOmega0( retrievedParam = prevDetCellParams->Omega0(chid,gain) ) ;
	    if ( retrievedParam == emptyDetCell.m_Omega0 ) {
	      ATH_MSG_WARNING( "Parameters Omega0 requested from DB but not found for channel " 
				<< onlineHelper->channel_name(chid)
				<< " gain=" << gain << " DAC=" << larCaliWave.getDAC() );
	      if (m_recoverEmptyDB) {
	        ATH_MSG_WARNING( " -> Recovering with RTM extraction." ); 
		wfParams.setOmega0( LArWFParamTool::DoExtract ) ;
	      } else {
	      	ATH_MSG_WARNING( " -> Skipping cell!" ); 
	        continue ; 
	      } 
	    }
	  }

	  if ( m_defaultTaur != LArWFParamTool::DefaultNotSet ) {
	    wfParams.setTaur( m_defaultTaur ) ;
	  } else if ( m_extractTaur ) {
	    wfParams.setTaur( LArWFParamTool::DoExtract ) ;
	  } else {
	    wfParams.setTaur( retrievedParam = prevDetCellParams->Taur(chid,gain) ) ;
	    if ( retrievedParam == emptyDetCell.m_Taur ) {
	      ATH_MSG_WARNING( "Parameters Taur requested from DB but not found for channel " 
			       << onlineHelper->channel_name(chid)  
			       << " gain=" << gain << " DAC=" << larCaliWave.getDAC());
	      if (m_recoverEmptyDB) {
	        ATH_MSG_WARNING( " -> Recovering with RTM extraction." ); 
	        wfParams.setTaur( LArWFParamTool::DoExtract ) ;
	      } else {
	        ATH_MSG_WARNING( " -> Skipping cell!" ); 
	        continue ; 
	      }
	    }
	  }

	  ATH_MSG_VERBOSE( "Pre-setting: Tcal     = " << wfParams.tcal() ) ;
	  ATH_MSG_VERBOSE( "Pre-setting: Fstep    = " << wfParams.fstep() ) ;
	  ATH_MSG_VERBOSE( "Pre-setting: Omega0   = " << wfParams.omega0() ) ;
	  ATH_MSG_VERBOSE( "Pre-setting: Taur     = " << wfParams.taur() ) ;

	  //end collection of input values. All stored in inputParams vector
	  
	}//end loop over DAC values
	if ( m_testmode ) {
	  ATH_MSG_INFO( "Test mode selected, process only one channel per gain per container!" ) ;
	  break ;
	}
      }//end loop over channels
    }//end loop over gains
  }//end loop over input containers (SG keys)


  if (!m_useTBB) { //traditional, serial processing:
    Looper looper(&inputParams,cabling,m_larWFParamTool.operator->(),msg(),m_counter);
    tbb::blocked_range<size_t> r(0,inputParams.size());
    looper(r);
  }
  else {
    ATH_MSG_INFO("Now calling TBB parallel_for");
    // NOW CALL TBB PARALLEL FOR
    tbb::parallel_for(tbb::blocked_range<size_t>(0, inputParams.size()),Looper(&inputParams,cabling,
									       m_larWFParamTool.operator->(),
									       msg(),m_counter));

    ATH_MSG_INFO("Done with parallel_for");
  }

  //Loop over inputParams to collect output:
  for (helperParams& params : inputParams) {
    if (!params.success) { // bad parameters
      ATH_MSG_WARNING( "Bad parameters for channel " << onlineHelper->channel_name(params.chid) << MSG::dec 
		       << " gain=" << params.gain << " DAC=" << params.caliWave->getDAC() ) ;
      continue ; 
    } 
    const LArWFParams& wfParams=params.wfParams;
    const HWIdentifier& chid=params.chid;
    const unsigned& gain=params.gain;
    const LArCaliWave& larCaliWave=*(params.caliWave);

    ATH_MSG_VERBOSE( "parameters extracted for channel " << MSG::hex << chid << MSG::dec 
		     << " gain=" << gain << " DAC=" << larCaliWave.getDAC() ) ;
          
    // fill params structures to be registered in detStore
    if ( newCaliPulseParams->Tcal(chid,gain) != emptyCaliPulse.m_Tcal ) {
      ATH_MSG_WARNING( "Already present in LArCaliPulseParams, don't add: channel " 
		       << MSG::hex << chid << MSG::dec << " gain=" << gain ) ;
    } else {
      ATH_MSG_VERBOSE( "add to LArCaliPulseParams..." ) ;
      NCalibParams++;
      if(m_Calibselection) {
	newCaliPulseParams->set(chid,(int)(gain),wfParams.tcal(),wfParams.fstep(),0.,0.,larCaliWave.getIsPulsedInt() ) ;
      } else {
	newCaliPulseParams->set(chid,(int)(gain),wfParams.tcal(),wfParams.fstep() ) ;
      }
    }
	  
    if ( newDetCellParams->Omega0(chid,gain) != emptyDetCell.m_Omega0 ) {
      ATH_MSG_WARNING( "Already present in LArDetCellParams, don't add: channel " 
		       << MSG::hex << chid << MSG::dec << " gain=" << gain ) ;
    } else {
      ATH_MSG_VERBOSE( "add to LArDetCellParams..." ) ;
      newDetCellParams->set(chid,(int)(gain),wfParams.omega0(),wfParams.taur() ) ;
      NDetParams++;
    }

    // collect this omega scan
    if ( omegaScanContainer ) {
      LArCaliWaveContainer::LArCaliWaves& dacScans = omegaScanContainer->get(chid, gain);
      dacScans.push_back( *params.omegaScan);
      ATH_MSG_VERBOSE( "omega scan added to container, channel=" << MSG::hex << chid << MSG::dec 
		       << " gain=" << gain ) ;
    }
	  
    // collect this residual oscillation before Taur extraction
    if ( resOscillContainerBefore ) {
      LArCaliWaveContainer::LArCaliWaves& dacResOsc0 = resOscillContainerBefore->get(chid, gain);
      dacResOsc0.push_back( *params.resOscill0) ;
      ATH_MSG_VERBOSE( "residual oscillation before Taur extraction added to container, channel=" << MSG::hex << chid << MSG::dec 
		<< " gain=" << gain ) ;
    }
	  
    // collect this residual oscillation after Taur extraction
    if ( resOscillContainerAfter ) {
      LArCaliWaveContainer::LArCaliWaves& dacResOsc1 = resOscillContainerAfter->get(chid, gain);
      dacResOsc1.push_back( *params.resOscill1 ) ;
      ATH_MSG_VERBOSE( "residual oscillation after Taur extraction added to container, channel=" << MSG::hex << chid << MSG::dec 
		       << " gain=" << gain ) ;
    }
	  
  } // end loop over input/output container
          
  if (nWaveConts==0) {
    ATH_MSG_ERROR( "Did not process any caliwave container!" );
    return StatusCode::FAILURE;
  }

  //ATH_MSG_INFO( " Summary : Number of cells with a LArCaliPulseParams values computed : " << newCaliPulseParams->totalNumberOfConditions()  );
  //ATH_MSG_INFO( " Summary : Number of cells with a LArDetCellParams values computed : " << newDetCellParams->totalNumberOfConditions()  );
  ATH_MSG_INFO( " Summary : Number of cells with a LArCaliPulseParams values computed : " << NCalibParams  );
  ATH_MSG_INFO( " Summary : Number of cells with a LArDetCellParams values computed : " << NDetParams  );
  ATH_MSG_INFO( " Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 " );
  ATH_MSG_INFO( " Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 " );
  ATH_MSG_INFO( " Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 " );
  ATH_MSG_INFO( " Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 " );
  ATH_MSG_INFO( " Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 " );


  // record extracted LArCaliPulseParamsComplete to detStore
  ATH_MSG_INFO( "...recording LArCaliPulseParams into det.store, key=" << m_keyExtractedCaliPulse ) ;
  const LArCaliPulseParamsComplete* paramsPtr=newCaliPulseParams.get(); // remember ptr for 
  if ( StatusCode::FAILURE == ( detStore()->record(std::move(newCaliPulseParams), m_keyExtractedCaliPulse ) ) ) {
    ATH_MSG_ERROR( "Could not record LArCaliPulseParams into det.store!" ) ;
    return StatusCode::FAILURE ;
  }
  
  // Symlink LArCaliPulseParamsComplete to ILArCaliPulseParams for further use
  ATH_MSG_DEBUG( "Trying to symlink ILArCaliPulseParams with LArCaliPulseParamsComplete...");
  ILArCaliPulseParams *larCaliPulseParams = NULL;
  sc = detStore()->symLink(paramsPtr,larCaliPulseParams);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not symlink ILArCaliPulseParams with LArCaliPulseParamsComplete." );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "ILArCaliPulseParams symlink with LArCaliPulseParamsComplete successfully");

  // record extracted LArDetCellParamsComplete to detStore
  ATH_MSG_INFO( "...recording LArDetCellParams into det.store, key=" << m_keyExtractedDetCell) ;
  const LArDetCellParamsComplete* detcellPtr=newDetCellParams.get();
  if ( StatusCode::FAILURE == ( detStore()->record(std::move(newDetCellParams), m_keyExtractedDetCell ) ) ) {
    ATH_MSG_ERROR( "Could not record LArDetCellParams into det.store!" ) ;
    return StatusCode::FAILURE ;
  }

  // Symlink LArDetCellParamsComplete to ILArDetCellParams for further use
  ATH_MSG_DEBUG( "Trying to symlink ILArDetCellParams with LArDetCellParamsComplete...");
  ILArDetCellParams *lArDetCellParams = NULL;
  sc = detStore()->symLink(detcellPtr,lArDetCellParams);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Could not symlink ILArDetCellParams with LArDetCellParamsComplete." );
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "ILArDetCellParams symlink with LArDetCellParamsComplete successfully" ) ;

  if ( omegaScanContainer ) {
    ATH_MSG_INFO( "Recording omega scan container into det.store, key=" << m_omegaScanKey ) ;
    if ( StatusCode::FAILURE == ( detStore()->record(std::move(omegaScanContainer), m_omegaScanKey ) ) ) {
      ATH_MSG_WARNING( "Could not record omega scan container into DetStore!" ) ;
      // return StatusCode::FAILURE ;
    }
  }

  if ( resOscillContainerBefore ) {
    ATH_MSG_INFO( "Recording residual oscillation (before Taur extraction) container into DetStore, key = " << m_resOscillKeyBefore ) ;
    if ( StatusCode::FAILURE == ( detStore()->record(std::move(resOscillContainerBefore), m_resOscillKeyBefore ) ) ) {
      ATH_MSG_WARNING( "Could not record residual oscillation (before Taur extraction) container into DetStore!" ) ;
      // return StatusCode::FAILURE ;
    }
  }

  if ( resOscillContainerAfter ) {
    ATH_MSG_INFO( "Recording residual oscillation (after Taur extraction) container into DetStore, key = " << m_resOscillKeyAfter ) ;
    if ( StatusCode::FAILURE == ( detStore()->record(std::move(resOscillContainerAfter), m_resOscillKeyAfter ) ) ) {
      ATH_MSG_WARNING( "Could not record residual oscillation (after Taur extraction) container into DetStore!" ) ;
      // return StatusCode::FAILURE ;
    }
  }

  ATH_MSG_INFO( "LArRTMParamExtractor finalized!" );  
  
return StatusCode::SUCCESS;
}


void LArRTMParamExtractor::Looper::operator() (const tbb::blocked_range<size_t>& r) const {

  for (size_t i=r.begin();i!=r.end();++i) {
    helperParams& p=m_tbbparams->at(i);
    StatusCode sc = m_tool->getLArWaveParams(*(p.caliWave), 
					     p.chid, 
					     (CaloGain::CaloGain)p.gain, 
					     p.wfParams,
					     m_cabling,
					     p.omegaScan,
					     p.resOscill0,
					     p.resOscill1
                                             );	

    p.success=sc.isSuccess() ;

    
    unsigned cnt=(++m_counter);
    if (cnt % 100 == 0) {
      m_msg << MSG::INFO << "Processing wavefrom No " << cnt << endmsg;
    }
    
  }
  return;
}
