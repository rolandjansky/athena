/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/TrigCountTrtHits.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/TRT_RDO_Container.h" 

#include "xAODEventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

//---------------------------------------------------------------------------------

TrigCountTrtHits::TrigCountTrtHits(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_hltExecuteInitialisationRun(false),
    m_detStore("DetectorStore", name),
    m_storeGate("StoreGateSvc", name),
    m_trtHelper(0),
    m_rawDataTool("TrigTRT_DriftCircleProviderTool"),
    m_endcapC(0),
    m_barrel(0),
    m_endcapA(0),
    m_trt_barrel_a_side(false),
    m_trt_barrel_c_side(false),
    m_trt_endcap_a_side(false),
    m_trt_endcap_c_side(false),
    m_dataLoopTimer(nullptr),
    m_retrieveDataTimer(nullptr),
    m_attachFTimer(nullptr),
    m_useCachedResult(false),
    m_trtHitCounts(0),
    m_cachedTE(nullptr) {

  declareProperty( "TRT_RDO_ContainerName",             m_trtRdoContainerName = "TRT_RDO_Cache" );
  declareProperty( "TotBins",                           m_hTotBins = 30 );
  declareProperty( "TotMin",                            m_hTotMin = 0.);
  declareProperty( "TotMax",                            m_hTotMax = 30. );
  declareProperty( "TrtDataProviderTool", m_rawDataTool, "TrigTRT_DriftCircleProviderTool");

  // Monitoring of the data stored in TrigSpacePointCounts
  declareMonitoredStdContainer("TrtRdoEndcapC_Tot",     m_endcapC_Tot); 
  declareMonitoredStdContainer("TrtRdoBarrel_Tot",      m_barrel_Tot); 
  declareMonitoredStdContainer("TrtRdoEndcapA_Tot",     m_endcapA_Tot); 

  // Other monitoring
  declareMonitoredVariable("NTrtRdo",                   m_nTrtHits); 

  // Data error monitoring
  declareMonitoredStdContainer("TrtDataErrors",         m_trtDataErrors);

}

//---------------------------------------------------------------------------------

TrigCountTrtHits::~TrigCountTrtHits() {
  if(m_endcapC) delete m_endcapC;
  if(m_barrel)  delete m_barrel;
  if(m_endcapA) delete m_endcapA;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigCountTrtHits::hltInitialize() {
  ATH_MSG_DEBUG("Initialising this TrigCountTrtHits: " << name());
  StatusCode sc = StatusCode::FAILURE;

  // Get storegate svc
  if(m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to connect to " << m_detStore.typeAndName());
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_detStore.typeAndName());

  if(m_storeGate.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to connect to " << m_storeGate.typeAndName());
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_storeGate.typeAndName());

  // Get a TRT identifier helper
  sc = m_detStore->retrieve(m_trtHelper, "TRT_ID");
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_trtHelper); // fatal?
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_trtHelper);
  
  // Get TrigTRT_DriftCircleProviderTool
  if( m_rawDataTool.retrieve().isFailure() ){
    ATH_MSG_FATAL("Failed to retrieve " << m_rawDataTool);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_rawDataTool);
  
  // Create timer
  if ( timerSvc() ) {
    m_retrieveDataTimer   = addTimer("retrieveTrtData");
    m_dataLoopTimer       = addTimer("dataLoop");
    m_attachFTimer        = addTimer("attachingFeatures");
  }

  ATH_MSG_INFO(" TrigCountTrtHits initialized successfully"); 

  // Create empty histograms.
  m_endcapC = new TrigHisto1D(m_hTotBins, m_hTotMin, m_hTotMax);  
  m_barrel  = new TrigHisto1D(m_hTotBins, m_hTotMin, m_hTotMax);
  m_endcapA = new TrigHisto1D(m_hTotBins, m_hTotMin, m_hTotMax);

  // Set the size of the vectors to pass the monitoring's 
  // kVecUO size check.
  m_endcapC_Tot.resize((m_hTotBins+2),0);
  m_barrel_Tot.resize( (m_hTotBins+2),0);
  m_endcapA_Tot.resize((m_hTotBins+2),0);

  return HLT::OK;  
}

//---------------------------------------------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigCountTrtHits::hltBeginRun() {
  // This initialisation has been moved into the event loop.
  // @see TrigCountTrtHitsHypo::checkDetectorMask
  ATH_MSG_DEBUG(" TrigCountTrtHits will be initialized in hltExecute"); 
  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigCountTrtHits::checkDetectorMask() {
  m_hltExecuteInitialisationRun = true;
  ATH_MSG_DEBUG("[TrigCountTrtHits::checkDetectorMask]  beginning run with this " << name());

  const xAOD::EventInfo* evinfo = 0;
  if (store()->retrieve(evinfo).isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve xAOD::EventInfo from SG for detmasks");
    return HLT::SG_ERROR;
  }
  else {
    ATH_MSG_INFO("xAOD::EventInfo Run Information [Run,Evt,Lumi,Time,BunchCross,DetMask] = [" 
      << evinfo->runNumber()
      << "," << evinfo->eventNumber()
      << "," << evinfo->lumiBlock()
      << "," << evinfo->timeStamp()
      << ":" << evinfo->timeStampNSOffset()
      << "," << evinfo->bcid()
      << ",0x" << std::hex << evinfo->detectorMask() << std::dec
      << "]");

    uint64_t mask = evinfo->detectorMask();
    eformat::helper::DetectorMask decoder(mask);

    if (mask == 0) {
      ATH_MSG_INFO("Detector Mask == 0. Assuming MC file and setting all of ID to ON."); 
      m_trt_barrel_a_side = true;
      m_trt_barrel_c_side = true;
      m_trt_endcap_a_side = true;
      m_trt_endcap_c_side = true;
    } else {
      m_trt_barrel_a_side = decoder.is_set(eformat::TRT_BARREL_A_SIDE);
      m_trt_barrel_c_side = decoder.is_set(eformat::TRT_BARREL_C_SIDE);
      m_trt_endcap_a_side = decoder.is_set(eformat::TRT_ENDCAP_A_SIDE);
      m_trt_endcap_c_side = decoder.is_set(eformat::TRT_ENDCAP_C_SIDE);
    }

    ATH_MSG_INFO("trt_barrel_a_side is " << (m_trt_barrel_a_side==true? "present" : "OFF! "));
    ATH_MSG_INFO("trt_barrel_c_side is " << (m_trt_barrel_c_side==true? "present" : "OFF! "));
    ATH_MSG_INFO("trt_endcap_a_side is " << (m_trt_endcap_a_side==true? "present" : "OFF! "));
    ATH_MSG_INFO("trt_endcap_c_side is " << (m_trt_endcap_c_side==true? "present" : "OFF! ")); 
  }
  return HLT::OK;
}

HLT::ErrorCode TrigCountTrtHits::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out) {

  // Do initialisation at start of first event
  if (m_hltExecuteInitialisationRun == false) {
    HLT::ErrorCode ec = checkDetectorMask();
    if (ec != HLT::OK) return ec;
  }

  ATH_MSG_DEBUG("Executing this TrigCountTrtHits " << name());

  // Caching.
  // First check whether we executed this instance before:
  if( m_useCachedResult ){
    ATH_MSG_DEBUG("Executing " << name() << " in cached mode");
      
    // Get all input TEs (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    std::vector<HLT::TEVec>::const_iterator itEnd = tes_in.end();
    for( std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != itEnd; ++it ){
      
      HLT::TEVec::const_iterator inner_it = (*it).begin();
      HLT::TEVec::const_iterator inner_itEnd = (*it).end();
      for( ; inner_it != inner_itEnd; ++inner_it)
        allTEs.push_back(*inner_it);
    }
    
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);
    
    // Save (cached) feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );
    
    return HLT::OK;
  }

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();

  if( !(m_trt_barrel_a_side && m_trt_barrel_c_side && m_trt_endcap_a_side && m_trt_endcap_c_side) ){
    ATH_MSG_DEBUG("Trt detector is not present. ");   
    // currently dont return since only old data without detector flags exsist
  }
  // Clear TrigHisto1d histograms 
  m_endcapC->clear();
  m_barrel->clear();
  m_endcapA->clear();

  // Clear the monitoring variables.
  m_endcapC_Tot.clear();
  m_barrel_Tot.clear();
  m_endcapA_Tot.clear();
  m_nTrtHits = 0;
  m_trtDataErrors.clear();

  // Only if part of detector present
  if (m_trt_barrel_a_side ||  m_trt_barrel_c_side ||  m_trt_endcap_a_side || m_trt_endcap_c_side) {

    //1. Get the data
    if( timerSvc() ) 
      m_retrieveDataTimer->start();

    TrigRoiDescriptor fsroi(true);
    StatusCode sc_fc = m_rawDataTool->fillCollections(fsroi);
    if( sc_fc.isRecoverable() ){
        ATH_MSG_DEBUG("Recoverable error(s) during TRT data preparation");

        //  error monitoring 
        const std::vector<int>* errVect = m_rawDataTool->fillTRT_DataErrors();
        std::copy(errVect->begin(),errVect->end(),std::back_inserter(m_trtDataErrors));
    } 
    else if(sc_fc.isFailure()){
      ATH_MSG_ERROR("BS conversion into RDOs failed");
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::CORRUPTED_ROD);
    }
    
    if( timerSvc() ){
      m_retrieveDataTimer->stop();
      m_dataLoopTimer->start();
    }

    const TRT_RDO_Container *trtContainer;
    StatusCode sc_sg = m_storeGate->retrieve( trtContainer, m_trtRdoContainerName );

    if( sc_sg.isFailure() ){
      ATH_MSG_ERROR(" Failed to retrieve trt data from SG. "); 
      return HLT::TOOL_FAILURE;
    }
    else
      ATH_MSG_DEBUG(" Successfully retrieved trt data from SG. "); 
    
    // loop over collections of container
    TRT_RDO_Container::const_iterator trthitsCollIt = trtContainer->begin();
    TRT_RDO_Container::const_iterator trthitsCollItEnd = trtContainer->end();
    
    for( ; trthitsCollIt!=trthitsCollItEnd; ++trthitsCollIt ){  
      if( trthitsCollIt == trthitsCollItEnd ) continue;
      if(!(*trthitsCollIt)) continue;
      
      m_nTrtHits += (*trthitsCollIt)->size();
      
      // loop over each collection to get the time over threshold information per hit   
      TRT_RDO_Collection::const_iterator rdoItEnd = (*trthitsCollIt)->end();    
      TRT_RDO_Collection::const_iterator rdoIt = (*trthitsCollIt)->begin();
      if( rdoIt == rdoItEnd ) continue;

      for( ; rdoIt != rdoItEnd; ++rdoIt ){
        
        Identifier trt_id = (*rdoIt)->identify();
        int bec = m_trtHelper->barrel_ec(trt_id);

        if(bec == -2) { 
          m_endcapC->fill((*rdoIt)->timeOverThreshold(),1);
        }
        else if(bec == -1 || bec == 1) {
          m_barrel->fill((*rdoIt)->timeOverThreshold(),1);
        }
        else if(bec == 2) {
          m_endcapA->fill((*rdoIt)->timeOverThreshold(),1);
        }
      }
    }
    
    // Dump the contents of the TrigHisto1D such that it can be
    // integrated by the online monitoring.
    m_endcapC_Tot = m_endcapC->contents();
    m_barrel_Tot  = m_barrel->contents();
    m_endcapA_Tot = m_endcapA->contents();

  } // End of part-of-detector-present if statement 
  
  if( timerSvc() ){
    m_dataLoopTimer->stop();
    m_attachFTimer->start();
  }
  
  ATH_MSG_DEBUG("REGTEST : number of TRT hits is  " << m_nTrtHits);
  
  m_trtHitCounts = new TrigTrtHitCounts(*m_endcapC, *m_barrel, *m_endcapA);
  
  // A vector of trigger elements is passed into this function, where
  // for each trigger element type provided there is a vector of
  // trigger elements.  For example, L1_RD0, L1_BCM_2, L1_MBTS_2 are
  // each a vector of one element.  Therefore start by concatenating
  // the trigger elements.
  
  HLT::TEVec allTEs;
  std::vector<HLT::TEVec>::const_iterator it = tes_in.begin();
  std::vector<HLT::TEVec>::const_iterator itEnd = tes_in.end();
  
  for( ; it != itEnd ; ++it) {
    HLT::TEVec::const_iterator inner_it = (*it).begin();
    HLT::TEVec::const_iterator inner_itEnd = (*it).end();

    for( ; inner_it != inner_itEnd; ++inner_it ){
      ATH_MSG_DEBUG("Creating TE seeded from input TE " << (*inner_it)->getId());
      allTEs.push_back(*inner_it);
    }
  }

  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);  
  
  HLT::ErrorCode hltStatus = attachFeature( outputTE, m_trtHitCounts, "TrtHitCount");
  if(hltStatus != HLT::OK) {
    ATH_MSG_ERROR("Unable to attach HLT feature TrtHitCount to output TE.");
    m_attachFTimer->stop();
    return hltStatus;
  }
  
  if( timerSvc() ) m_attachFTimer->stop();

  // Cache the TE in case this Fex is called again in this event.
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();

  return HLT::OK;

}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigCountTrtHits::hltFinalize() {
  ATH_MSG_DEBUG(" finalizing TrigCountTrtHits : "<< name()); 
  return HLT::OK;  
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigCountTrtHits::hltEndEvent() {
    m_useCachedResult = false;
    m_trtHitCounts = 0;
    m_cachedTE=0;
    return HLT::OK;
}
