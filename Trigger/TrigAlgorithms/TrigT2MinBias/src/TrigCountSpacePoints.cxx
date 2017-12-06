/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/TrigCountSpacePoints.h"

#include "xAODEventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "IRegionSelector/IRegSelSvc.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

// Space point Classes,
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

//---------------------------------------------------------------------------------

TrigCountSpacePoints::TrigCountSpacePoints(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator), 
    m_hltExecuteInitialisationRun(kFALSE),
    m_detStore("DetectorStore", name),
    m_regionSelector("RegSelSvc", name),
    m_doPixelSp(true), 
    m_doSctSp(true),
    m_doOnlyBLayer(false),
    m_pixHelper(0),
    m_sctHelper(0),
    m_pixelClusEndcapC(0),
    m_pixelClusBarrel(0),
    m_pixelClusEndcapA(0),
    m_useCachedResult(false), 
    m_spacePointCounts(0),
    m_cachedTE(0),
    m_sct_barrel_a_side(true),
    m_sct_barrel_c_side(true),
    m_sct_endcap_a_side(true),
    m_sct_endcap_c_side(true),
    m_pixel_barrel(true),
    m_pixel_b_layer(true),    
    m_pixel_disk(true)  {

  declareProperty( "PixelSP_ContainerName",                   m_pixelSpName = "PixelTrigSpacePoints"  );
  declareProperty( "SCT_SP_ContainerName",                    m_sctSpName = "SCT_TrigSpacePoints"     );
  declareProperty("RegionSelectorTool",                       m_regionSelector);
  declareProperty( "ReadPixelSp",                             m_doPixelSp = true ); 
  declareProperty( "ReadSctSp",                               m_doSctSp = true );
  declareProperty( "OnlyCountBLayer",                         m_doOnlyBLayer = false );

  // note, these are dummy values, they are set in TriggerMenuPython/python/MinBiasDef.py
  declareProperty( "MaxNModIdentifier",                       m_maxnid = 100 );
  declareProperty( "PixelModuleThreshold",                    m_pixModuleThreshold = 50 );
  declareProperty( "SCTModuleThreshold",                      m_sctModuleThreshold = 100 );
  declareProperty( "PixelClusToTCut",                         m_pixelClusToTCut = 20. );
  declareProperty( "PixelClusTotBins",                        m_hPixelClusTotBins = 20 );
  declareProperty( "PixelClusTotMin",                         m_hPixelClusTotMin = 0.5 );
  declareProperty( "PixelClusTotMax",                         m_hPixelClusTotMax = 20.5 );
  declareProperty( "PixelClusSizeBins",                       m_hPixelClusSizeBins = 1 ); // Use underflow for 1 and overflow for 3+
  declareProperty( "PixelClusSizeMin",                        m_hPixelClusSizeMin = 1. );
  declareProperty( "PixelClusSizeMax",                        m_hPixelClusSizeMax = 2. );

  // Monitoring of the data stored in TrigSpacePointCounts
  declareMonitoredStdContainer("PixelClusEndcapC_Tot",        m_pixelClusEndcapC_Tot); 
  declareMonitoredStdContainer("PixelClusEndcapC_Size",       m_pixelClusEndcapC_Size); 
  declareMonitoredStdContainer("PixelClusBarrel_Tot",         m_pixelClusBarrel_Tot); 
  declareMonitoredStdContainer("PixelClusBarrel_Size",        m_pixelClusBarrel_Size); 
  declareMonitoredStdContainer("PixelClusEndcapA_Tot",        m_pixelClusEndcapA_Tot); 
  declareMonitoredStdContainer("PixelClusEndcapA_Size",       m_pixelClusEndcapA_Size); 
  declareMonitoredVariable("sctSpEndcapC",                    m_sctSpEndcapC);
  declareMonitoredVariable("sctSpBarrel",                     m_sctSpBarrel);
  declareMonitoredVariable("sctSpEndcapA",                    m_sctSpEndcapA);

  // Data error monitoring
  declareMonitoredStdContainer("PixelDataErrors",             m_pixelDataErrors);
  declareMonitoredStdContainer("SctDataErrors",               m_sctDataErrors);

  declareMonitoredVariable("PIX_SPCount",                     m_totNumPixSP );
  declareMonitoredVariable("SCT_SPCount",                     m_totNumSctSP );
  declareMonitoredVariable("RatioA",                          m_ratioA );
  declareMonitoredVariable("RatioB",                          m_ratioB );

  // spacepoints per module
  declareMonitoredStdContainer("PixSpPerModule",              m_pixSpPerModule );
  declareMonitoredStdContainer("SctSpPerModule",              m_sctSpPerModule);

  // EC/barrel monitoring
  declareMonitoredVariable("PixClBarrel",                      m_pixClBarrel );
  declareMonitoredVariable("PixClEndcapA",                     m_pixClEndcapA );
  declareMonitoredVariable("PixClEndcapC",                     m_pixClEndcapC );

  // pixel module monitoring
  declareMonitoredStdContainer("pixECA_clust_occ_disk",       m_pixECA_clust_occ_disk );
  declareMonitoredStdContainer("pixECC_clust_occ_disk",       m_pixECC_clust_occ_disk );
  declareMonitoredStdContainer("pixECA_clust_occ_phi",        m_pixECA_clust_occ_phi );
  declareMonitoredStdContainer("pixECC_clust_occ_phi",        m_pixECC_clust_occ_phi );
  declareMonitoredStdContainer("pixBarrL0_clust_occ_eta",     m_pixBarrL0_clust_occ_eta );
  declareMonitoredStdContainer("pixBarrL1_clust_occ_eta",     m_pixBarrL1_clust_occ_eta );
  declareMonitoredStdContainer("pixBarrL2_clust_occ_eta",     m_pixBarrL2_clust_occ_eta );
  declareMonitoredStdContainer("pixBarrL0_clust_occ_phi",     m_pixBarrL0_clust_occ_phi );
  declareMonitoredStdContainer("pixBarrL1_clust_occ_phi",     m_pixBarrL1_clust_occ_phi );
  declareMonitoredStdContainer("pixBarrL2_clust_occ_phi",     m_pixBarrL2_clust_occ_phi );

  // sct module monitoring
  declareMonitoredStdContainer("sctBarrL1_sp_occ_eta",        m_sctBarrL1_sp_occ_eta );
  declareMonitoredStdContainer("sctBarrL2_sp_occ_eta",        m_sctBarrL2_sp_occ_eta );
  declareMonitoredStdContainer("sctBarrL3_sp_occ_eta",        m_sctBarrL3_sp_occ_eta );
  declareMonitoredStdContainer("sctBarrL4_sp_occ_eta",        m_sctBarrL4_sp_occ_eta );  
  declareMonitoredStdContainer("sctBarrL1_sp_occ_phi",        m_sctBarrL1_sp_occ_phi );
  declareMonitoredStdContainer("sctBarrL2_sp_occ_phi",        m_sctBarrL2_sp_occ_phi );
  declareMonitoredStdContainer("sctBarrL3_sp_occ_phi",        m_sctBarrL3_sp_occ_phi );
  declareMonitoredStdContainer("sctBarrL4_sp_occ_phi",        m_sctBarrL4_sp_occ_phi );
  declareMonitoredStdContainer("sctECA_sp_occ_disk",          m_sctECA_sp_occ_disk );
  declareMonitoredStdContainer("sctECA_sp_occ_phi",           m_sctECA_sp_occ_phi );
  declareMonitoredStdContainer("sctECC_sp_occ_disk",          m_sctECC_sp_occ_disk );
  declareMonitoredStdContainer("sctECC_sp_occ_phi",           m_sctECC_sp_occ_phi );


  // initialization of non-static class members to clean up Coverity
  declareProperty( "nPixSP",         m_nPixSP = 0. );
  declareProperty( "pixClSize",      m_pixClSize = 0. );
  declareProperty( "nPixCL_1",       m_nPixCL_1 = 0. );
  declareProperty( "nPixCL_2",       m_nPixCL_2 = 0. );
  declareProperty( "nPixCLmin3",     m_nPixCLmin3 = 0. );
  declareProperty( "pixclToT",       m_pixclToT = 0. );
  declareProperty( "totNumPixCL_1",  m_totNumPixCL_1 = 0. );
  declareProperty( "totNumPixCL_2",  m_totNumPixCL_2 = 0. );
  declareProperty( "totNumPixCLmin3",m_totNumPixCLmin3 = 0. );
  declareProperty( "pixListSize",    m_pixListSize = 0 );
  declareProperty( "sctListSize",    m_sctListSize = 0 );
  declareProperty( "SPpixBarr",      m_SPpixBarr = 0 );
  declareProperty( "SPpixECA",       m_SPpixECA = 0 );
  declareProperty( "SPpixECC",       m_SPpixECC = 0 );
  declareProperty( "nSctSP",         m_nSctSP = 0 );
  declareProperty( "SPsctBarr",      m_SPsctBarr = 0 );
  declareProperty( "SPsctECA",       m_SPsctECA = 0 );
  declareProperty( "SPsctECC",       m_SPsctECC = 0 );

  m_pixSPCTimer = 0;
  m_sctSPCTimer = 0;
  m_pixGetCollTimer = 0;
  m_sctGetCollTimer = 0;
  m_attachFTimer = 0;

}

//---------------------------------------------------------------------------------

TrigCountSpacePoints::~TrigCountSpacePoints() {
  if(m_pixelClusEndcapC) delete m_pixelClusEndcapC;
  if(m_pixelClusBarrel) delete m_pixelClusBarrel;
  if(m_pixelClusEndcapA) delete m_pixelClusEndcapA;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigCountSpacePoints::hltInitialize() {

  ATH_MSG_DEBUG("Initialize this TrigCountSpacePoints: " << name());

  // Retrieving Region Selector Tool
  if ( m_regionSelector.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Unable to retrieve RegionSelector tool " << m_regionSelector.type());
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  // get detector store
  if(m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to connect to " << m_detStore.typeAndName());
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_INFO("Successfully initialised DetectorStore !");
  }
 
  // Only get the Pixel helper if Pixel spacepoints are requested
  if(m_doPixelSp) { 
    StatusCode sc_pixH = m_detStore->retrieve(m_pixHelper, "PixelID");
    if( sc_pixH.isFailure() ){
      ATH_MSG_WARNING("Could not obtain pix helper!");
      return StatusCode::FAILURE;
    }
  }

  // Only get the SCT helper if SCT spacepoints are requested
  if(m_doSctSp) {
    StatusCode sc_sctH = m_detStore->retrieve(m_sctHelper, "SCT_ID");
    if( sc_sctH.isFailure() ){
      ATH_MSG_WARNING("Could not obtain sct helper!");
      return StatusCode::FAILURE;
    }
  }
  
  // Create timers
  if ( timerSvc() ) {
    m_pixSPCTimer      = addTimer("PixTotalPartSPC");
    m_pixGetCollTimer  = addTimer("PixGetColl");
    m_sctSPCTimer      = addTimer("SctTotalPartSPC");
    m_sctGetCollTimer  = addTimer("SctGetColl");
    m_attachFTimer     = addTimer("AttachingFeatures");
  }

  // Create empty histograms.
  m_pixelClusEndcapC = new xAOD::TrigHisto2D();
  m_pixelClusEndcapC->makePrivateStore();
  m_pixelClusEndcapC->initialize(m_hPixelClusTotBins, m_hPixelClusTotMin, m_hPixelClusTotMax,
         m_hPixelClusSizeBins, m_hPixelClusSizeMin, m_hPixelClusSizeMax);

  m_pixelClusBarrel = new xAOD::TrigHisto2D();
  m_pixelClusBarrel->makePrivateStore();
  m_pixelClusBarrel->initialize(m_hPixelClusTotBins, m_hPixelClusTotMin, m_hPixelClusTotMax,
              m_hPixelClusSizeBins, m_hPixelClusSizeMin, m_hPixelClusSizeMax);
  
  m_pixelClusEndcapA = new xAOD::TrigHisto2D();
  m_pixelClusEndcapA->makePrivateStore();
  m_pixelClusEndcapA->initialize(m_hPixelClusTotBins, m_hPixelClusTotMin, m_hPixelClusTotMax,
               m_hPixelClusSizeBins, m_hPixelClusSizeMin, m_hPixelClusSizeMax);

  // Set the size of the vectors to pass the monitoring's 
  // kVecUO size check.
  m_pixelClusEndcapC_Tot.resize((m_hPixelClusTotBins+2),0);
  m_pixelClusEndcapC_Size.resize((m_hPixelClusSizeBins+2),0);
  m_pixelClusBarrel_Tot.resize((m_hPixelClusTotBins+2),0);
  m_pixelClusBarrel_Size.resize((m_hPixelClusSizeBins+2),0);
  m_pixelClusEndcapA_Tot.resize((m_hPixelClusTotBins+2),0);
  m_pixelClusEndcapA_Size.resize((m_hPixelClusSizeBins+2),0);

  ATH_MSG_INFO(" TrigCountSpacePoints initialized successfully"); 
  return HLT::OK;  
}


//---------------------------------------------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigCountSpacePoints::hltBeginRun() {
  // This initialisation has been moved into the event loop.
  // @see TrigCountSpacePoints::checkDetectorMask
  ATH_MSG_DEBUG(" TrigCountSpacePoints will be initialized in hltExecute"); 
  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigCountSpacePoints::checkDetectorMask() {
  m_hltExecuteInitialisationRun = true;
  ATH_MSG_DEBUG("[TrigCountSpacePoints::checkDetectorMask]  beginning run with this " << name());

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
      m_sct_barrel_a_side = true;
      m_sct_barrel_c_side = true;
      m_sct_endcap_a_side = true;
      m_sct_endcap_c_side = true;
      m_pixel_barrel  = true;
      m_pixel_b_layer = true;
      m_pixel_disk    = true;
    } else {
      m_sct_barrel_a_side = decoder.is_set(eformat::SCT_BARREL_A_SIDE);
      m_sct_barrel_c_side = decoder.is_set(eformat::SCT_BARREL_C_SIDE);
      m_sct_endcap_a_side = decoder.is_set(eformat::SCT_ENDCAP_A_SIDE);
      m_sct_endcap_c_side = decoder.is_set(eformat::SCT_ENDCAP_C_SIDE);
      m_pixel_barrel  = decoder.is_set(eformat::PIXEL_BARREL);
      m_pixel_b_layer = decoder.is_set(eformat::PIXEL_B_LAYER);
      m_pixel_disk    = decoder.is_set(eformat::PIXEL_DISK);
    }

    ATH_MSG_INFO("sct_barrel_a_side is " << (m_sct_barrel_a_side==true? "present" : "OFF! "));
    ATH_MSG_INFO("sct_barrel_c_side is " << (m_sct_barrel_c_side==true? "present" : "OFF! "));
    ATH_MSG_INFO("sct_endcap_a_side is " << (m_sct_endcap_a_side==true? "present" : "OFF! "));
    ATH_MSG_INFO("sct_endcap_c_side is " << (m_sct_endcap_c_side==true? "present" : "OFF! "));
      
    ATH_MSG_INFO("pixel_barrel is      " << (m_pixel_barrel==true? "present" : "OFF! "));
    ATH_MSG_INFO("pixel_b_layer is     " << (m_pixel_b_layer==true? "present" : "OFF! "));
    ATH_MSG_INFO("pixel_disk is        " << (m_pixel_disk==true? "present" : "OFF! "));
  }
  return HLT::OK;
}

HLT::ErrorCode TrigCountSpacePoints::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out) {
  // Do initialisation at start of first event
  if (m_hltExecuteInitialisationRun == false) {
    HLT::ErrorCode ec = checkDetectorMask();
    if (ec != HLT::OK) return ec;
  }

  ATH_MSG_DEBUG("Executing this TrigCountSpacePoints " << name());

  // Caching.
  // First check whether we executed this instance before:
  if (m_useCachedResult) {
    ATH_MSG_DEBUG("Executing " << name() << " in cached mode");
    
    // Get all input TEs (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    std::vector<HLT::TEVec>::const_iterator itEnd = tes_in.end();
    for( std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != itEnd; ++it ){
      
      HLT::TEVec::const_iterator inner_it = (*it).begin();
      HLT::TEVec::const_iterator inner_itEnd = (*it).end();
      for( ; inner_it != inner_itEnd; ++inner_it) {
        allTEs.push_back(*inner_it);
      }
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

  std::vector<IdentifierHash>::iterator hashIt, hashItEnd;
 
  // ------ PIXEL PART ---------------------------------------------
  if( !(m_pixel_barrel &&  m_pixel_b_layer &&  m_pixel_disk) ) {
    ATH_MSG_DEBUG("NO pixel detector present.");
  }

  if( timerSvc() ){
    m_pixSPCTimer->start();
  }

  // Clear the pixel cluster histograms
  m_pixelClusEndcapC->clear();
  m_pixelClusBarrel->clear();
  m_pixelClusEndcapA->clear();
  
  m_nPixSP = 0; 
  m_pixelDataErrors.clear();
  m_pixSpPerModule.clear();

  // for storing noisy modules
  std::vector<Identifier> droppedPixelModules;
  droppedPixelModules.clear();
  
  // pixel cluster part
  m_pixClSize = 0; 
  m_nPixCL_1 = 0; 
  m_nPixCL_2 = 0; 
  m_nPixCLmin3 = 0; // # pixel clusters with size >= 3 
  m_totNumPixSP = 0;
  m_totNumPixCL_1 = 0; 
  m_totNumPixCL_2 = 0; 
  m_totNumPixCLmin3 = 0; 
  m_pixclToT = 0.;
 
  // pixel spacepoints according to position EC/Barrel
  m_pixClBarrel = 0;
  m_pixClEndcapA = 0;
  m_pixClEndcapC = 0;
  m_SPpixBarr = 0;
  m_SPpixECA = 0;
  m_SPpixECC = 0;


  // Only retrieve Pixel space points if requested to do so and at least part included in the det mask.
  if (m_doPixelSp && ( m_pixel_barrel ||  m_pixel_b_layer ||  m_pixel_disk)) {
    StatusCode sc;
    if( timerSvc() ) m_pixGetCollTimer->start();
    
    // this counts timing for pixels only though SCT SP are retrieved at the same time
    m_listOfPixIds.clear();
    m_listOfSctIds.clear();
    const SpacePointContainer* pixCont = 0;
    sc = store()->retrieve( pixCont, m_pixelSpName );
    if(sc.isFailure() || !pixCont){
      ATH_MSG_ERROR("Trig Pixel SP container " << m_pixelSpName <<" not found"); 
      return HLT::TOOL_FAILURE;
    } else {
      ATH_MSG_DEBUG("Successfully retrieved pixel SP container!");
      //sc = StatusCode::FAILURE;
    }
    
    m_regionSelector->DetHashIDList(PIXEL, m_listOfPixIds );
    m_pixListSize = m_listOfPixIds.size();
    
    if( m_pixListSize != 0 ){
      // Initialise monitoring histogram variables
      m_pixECA_clust_occ_disk.clear();
      m_pixECC_clust_occ_disk.clear();
      m_pixECA_clust_occ_phi.clear();
      m_pixECC_clust_occ_phi.clear();
      
      m_pixBarrL0_clust_occ_eta.clear();
      m_pixBarrL1_clust_occ_eta.clear();
      m_pixBarrL2_clust_occ_eta.clear();
      m_pixBarrL0_clust_occ_phi.clear();
      m_pixBarrL1_clust_occ_phi.clear();
      m_pixBarrL2_clust_occ_phi.clear();
      
      // loop over pixel collections
      const InDet::PixelCluster* pixClust;
      const std::vector<Identifier>& rdoList();
      
      hashIt=m_listOfPixIds.begin();
      hashItEnd=m_listOfPixIds.end();
      
      //std::vector<int>::iterator idItEnd = m_listOfPixIds.end();
      //std::vector<int>::iterator idIt = m_listOfPixIds.begin();
      for( ; hashIt != hashItEnd; ++hashIt ){
        // get single pixel collection -> pixSpCollIt
        SpacePointContainer::const_iterator pixSpCollIt = pixCont->indexFind( (*hashIt) );
        if( pixSpCollIt == pixCont->end() ) continue;
        if( (*pixSpCollIt) == NULL ) continue;
    
        // identify a module/wafer
        Identifier pixid = (*pixSpCollIt)->identify(); 

        // If B-layer only mode, then enforce layer 0
        if (m_doOnlyBLayer == true && m_pixHelper->layer_disk(pixid) != 0) continue;
  
        int bec = m_pixHelper->barrel_ec(pixid);
  
        // retrieve number of pixel SP/CL per collection
        SpacePointCollection::const_iterator spItEnd = (*pixSpCollIt)->end();
        SpacePointCollection::const_iterator spIt = (*pixSpCollIt)->begin();
  
        for( ; spIt != spItEnd; ++spIt ){
          const Trk::SpacePoint* pSP = (*spIt);
          //pixClust = dynamic_cast<const InDet::PixelCluster*> ( pSP->clusterList().first );
          pixClust = static_cast<const InDet::PixelCluster*> ( pSP->clusterList().first );
          m_pixClSize = (pixClust->rdoList()).size();
          m_pixclToT = pixClust->totalToT();
    
          if( m_pixclToT > m_pixelClusToTCut ){
            ++m_nPixSP;
            if( m_pixClSize == 1 )++m_nPixCL_1; 
            if( m_pixClSize == 2 )++m_nPixCL_2; 
            if( m_pixClSize >= 3 )++m_nPixCLmin3; 
          }
    
          // Histogram time over threshold against pixel size {1,2,3+}
          if(bec == -2) {
            m_pixelClusEndcapC->fill(m_pixclToT, m_pixClSize,1);
            ATH_MSG_DEBUG(" ECC pixel totalToT is " << m_pixclToT);
            ATH_MSG_DEBUG(" ECC pixel size is " << m_pixclToT);
          } else if(bec == 0) { 
            m_pixelClusBarrel->fill(m_pixclToT, m_pixClSize,1);
            ATH_MSG_DEBUG(" Barr pixel totalToT is " << m_pixclToT);
          } else if(bec == 2) { 
            m_pixelClusEndcapA->fill(m_pixclToT, m_pixClSize,1);
            ATH_MSG_DEBUG(" ECA pixel totalToT is " << m_pixclToT);
          }
        }

        // barrel
        if( bec==0 ){
          m_SPpixBarr = m_nPixSP;
          ATH_MSG_VERBOSE(" Formed  " << m_nPixSP << " PIX spacepoints in PIX Barrel after ToT cut.");
        } else if ( bec==2 ) { // endcap A
          m_SPpixECA = m_nPixSP;
          ATH_MSG_VERBOSE(" Formed  " << m_nPixSP << " PIX spacepoints in PIX ECA after ToT cut.");
        } else if( bec==-2 ) { // endcap C
          m_SPpixECC = m_nPixSP;
          ATH_MSG_VERBOSE(" Formed  " << m_nPixSP << " PIX spacepoints in PIX ECC after ToT cut.");
        }
        // total 
        m_pixSpPerModule.push_back(m_nPixSP);
        
        // check if there is a hot module and monitor it
        if( (m_SPpixBarr > m_pixModuleThreshold) || (m_SPpixECA > m_pixModuleThreshold) || (m_SPpixECC > m_pixModuleThreshold) ){
          ATH_MSG_WARNING(" This pixel module : " << pixid << " produced " << m_nPixSP << " pix spacepoints. ");
    
          unsigned int nPixModId = droppedPixelModules.size();
          if( nPixModId <= m_maxnid ) {
            droppedPixelModules.push_back(pixid);
          } else {
            ATH_MSG_WARNING("More than " << m_maxnid << " pixel modules are noisy, dump their id : " << pixid);
          }

          int layer_disk  = m_pixHelper->layer_disk(pixid);
          int phi_module  = m_pixHelper->phi_module(pixid);
          int eta_module  = m_pixHelper->eta_module(pixid);
    
          // check which module makes noise
          // barrel
          if( (m_SPpixBarr > m_pixModuleThreshold) ) {
            ATH_MSG_DEBUG(" PIX module in the barrel in disk " << layer_disk  << " with eta index " << eta_module << " and phi_module " << phi_module  << " produced " << m_nPixSP << " spacepoints!");
            ATH_MSG_DEBUG(" Will not account them. ");
            if( layer_disk==0 ){
              m_pixBarrL0_clust_occ_eta.push_back( eta_module );
              m_pixBarrL0_clust_occ_phi.push_back( phi_module );
            }
            else if( layer_disk==1 ){
              m_pixBarrL1_clust_occ_eta.push_back( eta_module );
              m_pixBarrL1_clust_occ_phi.push_back( phi_module );
            }
            else if( layer_disk==2 ){
              m_pixBarrL2_clust_occ_eta.push_back( eta_module );
              m_pixBarrL2_clust_occ_phi.push_back( phi_module );
            }
          } else if( m_SPpixECA > m_pixModuleThreshold ){ // endcaps
            ATH_MSG_DEBUG(" PIX module in the ECA in disk " << layer_disk << " with eta index " << eta_module << " and phi_module " << phi_module << " produced " << m_nPixSP << " spacepoints!");
            ATH_MSG_DEBUG(" Will not account them. ");
            m_pixECA_clust_occ_disk.push_back( layer_disk );
            m_pixECA_clust_occ_phi.push_back( phi_module ); 
          } else if( m_SPpixECC > m_pixModuleThreshold ){
            ATH_MSG_DEBUG(" PIX module in the ECC in disk " << layer_disk  << " with eta index " << eta_module << " and phi_module " << phi_module  << " produced " << m_nPixSP << " spacepoints!");
            ATH_MSG_DEBUG(" Will not account them. ");
            m_pixECC_clust_occ_disk.push_back( layer_disk );
            m_pixECC_clust_occ_phi.push_back( phi_module );
          }
        } else {
          m_totNumPixSP += m_nPixSP;  
          m_totNumPixCL_1 += m_nPixCL_1; 
          m_totNumPixCL_2 += m_nPixCL_2; 
          m_totNumPixCLmin3 += m_nPixCLmin3; 
          m_pixClBarrel += m_SPpixBarr;
          m_pixClEndcapA += m_SPpixECA;
          m_pixClEndcapC += m_SPpixECC;
        }
        m_nPixSP = 0;      
        m_nPixSP = 0; 
        m_nPixCL_1 = 0; 
        m_nPixCL_2 = 0; 
        m_nPixCLmin3 = 0; 
        m_SPpixBarr = 0;
        m_SPpixECA = 0;
        m_SPpixECC = 0;
      }
    } else{ //end //if( m_pixListSize != 0 ){
      ATH_MSG_DEBUG("Data is ok, but pixel detector might be off or not a single pixel hit was produced.");    
    }// end of pixel data quality check
    
    ATH_MSG_DEBUG("REGTEST : Formed  " << m_totNumPixSP << " pixel spacepoints in total."); 
    ATH_MSG_DEBUG("REGTEST : " << m_totNumPixCL_1 << " have cl size == 1 in total."); 
    ATH_MSG_DEBUG("REGTEST : " << m_totNumPixCL_2 << " have cl size == 2 in total."); 
    ATH_MSG_DEBUG("REGTEST : " << m_totNumPixCLmin3 << " have cl size >= 3 in total."); 
    ATH_MSG_DEBUG("REGTEST : Formed  " << m_totNumPixSP << " pixel spacepoints after ToT cut in total.");
    ATH_MSG_DEBUG("REGTEST : Formed " << m_pixClBarrel << " SP in pixel barrel in total.");
    ATH_MSG_DEBUG("REGTEST : Formed " << m_pixClEndcapA << " SP in pixel ECA in total.");
    ATH_MSG_DEBUG("REGTEST : Formed " << m_pixClEndcapC << " SP in pixel ECC in total.");
    
    // Monitor the data stored in TrigSpacePointCounts
    
    // Project the TrigHisto2D histograms into 1D histograms and dump
    // their contents such that it can be integrated by the online
    // monitoring.
    
    m_pixelClusEndcapC_Tot  = m_pixelClusEndcapC->profileX();
    m_pixelClusEndcapC_Size = m_pixelClusEndcapC->profileY();
    m_pixelClusBarrel_Tot   = m_pixelClusBarrel->profileX();
    m_pixelClusBarrel_Size  = m_pixelClusBarrel->profileY();
    m_pixelClusEndcapA_Tot  = m_pixelClusEndcapA->profileX();
    m_pixelClusEndcapA_Size = m_pixelClusEndcapA->profileY();
    
    // Calculate the pixel cluser ratio values for this event.  
    m_ratioA = -1.;  m_ratioB = -1.;  
    if( m_totNumPixSP > 0. ) m_ratioA = (m_totNumPixCL_2 + m_totNumPixCLmin3)/m_totNumPixSP;  
    
    if( m_totNumPixCL_1 > 0. ) m_ratioB = m_totNumPixCL_2/m_totNumPixCL_1;
    
  } // end of doPixelSp
  
  if( timerSvc() ) m_pixSPCTimer->stop();
  
  // ------ SCT PART ------------------------------------------------
  
  if( !(m_sct_barrel_a_side && m_sct_barrel_c_side &&  m_sct_endcap_a_side &&  m_sct_endcap_c_side) ) ATH_MSG_DEBUG("NO sct detector present.");
  
  if( timerSvc() ) m_sctSPCTimer->start();
  
  std::vector<Identifier> droppedSctModules;
  droppedSctModules.clear();

  m_nSctSP = 0;
  m_sctDataErrors.clear();
  m_sctSpPerModule.clear();
  m_sctSpBarrel = 0;
  m_sctSpEndcapA = 0;
  m_sctSpEndcapC = 0;
  m_SPsctBarr = 0;
  m_SPsctECA = 0;
  m_SPsctECC = 0;

  // Only get the SCT space points if requested to do so and included
  if(m_doSctSp && (m_sct_barrel_a_side || m_sct_barrel_c_side || m_sct_endcap_a_side || m_sct_endcap_c_side)) {
    
    StatusCode sc;

    if( timerSvc() ) m_sctGetCollTimer->start();
    
    // empty the pixel list here
    m_listOfPixIds.clear();
    m_listOfSctIds.clear();

    const SpacePointContainer* sctCont;
    sc = store()->retrieve( sctCont, m_sctSpName );
    if( sc.isFailure() ){
      ATH_MSG_WARNING("Trig SP SCT container " << m_sctSpName <<" not found"); 
      return HLT::TOOL_FAILURE;
    }
      
    m_regionSelector->DetHashIDList(SCT, m_listOfSctIds );
    m_sctListSize = m_listOfSctIds.size();
    
    if( m_sctListSize !=0 ){
      // initialise histogram values
      m_sctBarrL1_sp_occ_eta.clear();
      m_sctBarrL2_sp_occ_eta.clear();
      m_sctBarrL3_sp_occ_eta.clear();
      m_sctBarrL4_sp_occ_eta.clear();
      
      m_sctBarrL1_sp_occ_phi.clear();
      m_sctBarrL2_sp_occ_phi.clear();
      m_sctBarrL3_sp_occ_phi.clear();
      m_sctBarrL4_sp_occ_phi.clear();
      
      // ec
      m_sctECA_sp_occ_disk.clear();
      m_sctECA_sp_occ_phi.clear();
      m_sctECC_sp_occ_disk.clear();
      m_sctECC_sp_occ_phi.clear();
      
      // loop over SCT collections
      std::vector<IdentifierHash>::iterator sctItEnd = m_listOfSctIds.end();
      std::vector<IdentifierHash>::iterator sctIt = m_listOfSctIds.begin();
      for ( ; sctIt != sctItEnd; ++sctIt ) {
        SpacePointContainer::const_iterator sctSpCollIt = sctCont->indexFind( (*sctIt) );
  
        if( sctSpCollIt == sctCont->end() ) continue;
        if( (*sctSpCollIt) == NULL ) continue;
  
        const std::vector<Identifier>& rdoList();
        m_nSctSP = (*sctSpCollIt)->size();
  
        // returns detector element identifier
        Identifier sctid = (*sctSpCollIt)->identify();
  
        int bec = (int)m_sctHelper->barrel_ec(sctid);
  
        ATH_MSG_VERBOSE(" Formed " << m_nSctSP << " sct spacepoints");
        ATH_MSG_VERBOSE(" with sctid module " << sctid);
          
        // barrel
        if( bec==0 ){
          m_SPsctBarr = m_nSctSP;
          ATH_MSG_VERBOSE(" Formed  " << m_nSctSP << " SCT barrel spacepoints .");
        } else if( bec==2 ) { // endcap, side A
          m_SPsctECA = m_nSctSP;
          ATH_MSG_VERBOSE(" Formed  " << m_nSctSP << " SCT ECA spacepoints.");
        } else if( bec==-2 ){ // endcap, side C
          m_SPsctECC = m_nSctSP;
          ATH_MSG_VERBOSE(" Formed  " << m_nSctSP << " SCT ECC spacepoints.");
        }
  
        // total 
        m_sctSpPerModule.push_back(m_nSctSP);
  
        // check if there is a tripped module
        if( (m_SPsctBarr > m_sctModuleThreshold) || (m_SPsctECA > m_sctModuleThreshold) || (m_SPsctECC > m_sctModuleThreshold) ){
    
          unsigned int nSctModIds = droppedSctModules.size();
          if( nSctModIds <= m_maxnid ) {
            droppedSctModules.push_back(sctid);
          } else {
            ATH_MSG_WARNING("More than " << m_maxnid << " sct modules are noisy, dump their id : " << sctid);
          }
    
          int layer_disk  = m_sctHelper->layer_disk(sctid); // reference table in SCT_ID.h
          int phi_module  = m_sctHelper->phi_module(sctid);
          int eta_module  = m_sctHelper->eta_module(sctid);
    
          ATH_MSG_VERBOSE(" SCT layer_disk = " << layer_disk);
          ATH_MSG_VERBOSE(" SCT phi_module = " << phi_module);
          ATH_MSG_VERBOSE(" SCT eta_module = " << eta_module);
    
          if( m_SPsctBarr > m_sctModuleThreshold ){
            ATH_MSG_DEBUG(" SCT module in the barrel in disk " << layer_disk << " with eta index " << eta_module << " and phi_module " << phi_module << " produced " << m_nSctSP << " spacepoints!");
            ATH_MSG_DEBUG(" Will not account them. ");
            if( layer_disk == 0 ) {
              m_sctBarrL1_sp_occ_eta.push_back( eta_module );
              m_sctBarrL1_sp_occ_phi.push_back( phi_module );
            } else if( layer_disk == 1 ) {
              m_sctBarrL2_sp_occ_eta.push_back( eta_module );
              m_sctBarrL2_sp_occ_phi.push_back( phi_module );
            } else if( layer_disk == 2 ) {
              m_sctBarrL3_sp_occ_eta.push_back( eta_module );
              m_sctBarrL3_sp_occ_phi.push_back( phi_module );
            } else if( layer_disk == 3 ) {
              m_sctBarrL4_sp_occ_eta.push_back( eta_module );
              m_sctBarrL4_sp_occ_phi.push_back( phi_module );
            }
          } else if( m_SPsctECA > m_sctModuleThreshold ) {
            ATH_MSG_DEBUG(" SCT module in the ECA in disk " << layer_disk << " with eta index " << eta_module << " and phi_module " << phi_module << " produced " << m_nSctSP << " spacepoints!");
            ATH_MSG_DEBUG(" Will not account them. ");    
            m_sctECA_sp_occ_disk.push_back( layer_disk );
            m_sctECA_sp_occ_phi.push_back( phi_module );
          } else if( m_SPsctECC > m_sctModuleThreshold ) {
            ATH_MSG_DEBUG(" SCT module in the ECC in disk " << layer_disk << " with eta index " << eta_module << " and phi_module " << phi_module << " produced " << m_nSctSP << " spacepoints!");
            ATH_MSG_DEBUG(" Will not account them. ");
            m_sctECC_sp_occ_disk.push_back( layer_disk );
            m_sctECC_sp_occ_phi.push_back( phi_module );
          } else { // Accept the spacepoints
            m_sctSpBarrel += m_SPsctBarr;
            m_sctSpEndcapA += m_SPsctECA;
            m_sctSpEndcapC += m_SPsctECC;
          } // if m_SPsctBarr / m_SPsctECA / m_SPsctECC
          m_SPsctBarr = 0;
          m_SPsctECA = 0;
          m_SPsctECC = 0;
          m_nSctSP = 0;
        }
      } // end of for ( ; sctIt != sctItEnd; ++sctIt )
    }//end of //if( m_sctListSize !=0 )
    
    m_totNumSctSP = m_sctSpEndcapC + m_sctSpBarrel + m_sctSpEndcapA;
    ATH_MSG_DEBUG("REGTEST : Formed  " << m_totNumSctSP << " sct spacepoints in total.");
    ATH_MSG_DEBUG("REGTEST : Formed  " << m_sctSpEndcapC << " sct ECC spacepoints in total.");
    ATH_MSG_DEBUG("REGTEST : Formed  " << m_sctSpBarrel << " sct Barr spacepoints in total.");
    ATH_MSG_DEBUG("REGTEST : Formed  " << m_sctSpEndcapA << " sct ECA spacepoints in total.");    
  } // end of doSctSp

  if( timerSvc() ) { 
    m_sctSPCTimer->stop();
  }  

  // TODO: use the arrays of identifiers to store modules that have
  // been dropped when dynamic prescaling is enabled.  Only a maximum
  // of 100 should be stored.

  if( timerSvc() ) m_attachFTimer->start();

  m_spacePointCounts = new xAOD::TrigSpacePointCounts();
  
  m_spacePointCounts->makePrivateStore();
  m_spacePointCounts->setContentsPixelClusEndcapC(m_pixelClusEndcapC->contents());
  m_spacePointCounts->setContentsPixelClusBarrel(m_pixelClusBarrel->contents());
  m_spacePointCounts->setContentsPixelClusEndcapA(m_pixelClusEndcapA->contents());
  m_spacePointCounts->setPixelClusTotBins(m_hPixelClusTotBins);
  m_spacePointCounts->setPixelClusTotMin(m_hPixelClusTotMin);
  m_spacePointCounts->setPixelClusTotMax(m_hPixelClusTotMax);
  m_spacePointCounts->setPixelClusSizeBins(m_hPixelClusSizeBins);
  m_spacePointCounts->setPixelClusSizeMin(m_hPixelClusSizeMin);
  m_spacePointCounts->setPixelClusSizeMax(m_hPixelClusSizeMax);
  m_spacePointCounts->setSctSpEndcapC(m_sctSpEndcapC);
  m_spacePointCounts->setSctSpBarrel(m_sctSpBarrel);
  m_spacePointCounts->setSctSpEndcapA(m_sctSpEndcapA);

  // A vector of trigger elements is passed into this function, where
  // for each trigger element type provided there is a vector of
  // trigger elements.  For example, L1_RD0, L1_BCM_2, L1_MBTS_2 are
  // each a vector of one element.  Therefore start by concatenating
  // the trigger elements.
  
  HLT::TEVec allTEs;
  std::vector<HLT::TEVec>::const_iterator itEnd = tes_in.end();
  for( std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != itEnd ; ++it) {
    HLT::TEVec::const_iterator inner_itEnd = (*it).end();
    for( HLT::TEVec::const_iterator inner_it = (*it).begin(); inner_it != inner_itEnd; ++inner_it ){
      ATH_MSG_DEBUG("Creating TE seeded from input TE " << (*inner_it)->getId());
      allTEs.push_back(*inner_it);
    }
  }
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);

  HLT::ErrorCode hltStatus = attachFeature( outputTE, m_spacePointCounts, "spacepoints");

  if( timerSvc() ) m_attachFTimer->stop(); 
  if(hltStatus != HLT::OK) {
    ATH_MSG_ERROR("Unable to attach HLT feature spacepoints to output TE.");
    return hltStatus;
  }

  // Cache the TE in case this Fex is called again in this event.
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();

  return HLT::OK;
}

//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigCountSpacePoints::hltFinalize() {
  ATH_MSG_DEBUG(" finalizing TrigCountSpacePoints : "<< name()); 
  return HLT::OK;  
}

//---------------------------------------------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigCountSpacePoints::hltEndEvent() {
    m_useCachedResult = false;
    m_spacePointCounts = 0;   
    m_cachedTE=0;
    return HLT::OK;
}
