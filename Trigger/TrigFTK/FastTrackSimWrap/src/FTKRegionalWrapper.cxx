/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTracking/TrackParticleContainer.h"
#include "FastTrackSimWrap/FTKRegionalWrapper.h"
#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/atlClustering.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "TrigFTKTrackConverter/TrigFTKClusterConverterTool.h"
#include "TrigFTKToolInterfaces/ITrigFTKClusterConverterTool.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_Cabling/SCT_OnlineId.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "SCT_Cabling/SCT_SerialNumber.h"

#include <algorithm>
#include <sstream> 

FTKRegionalWrapper::FTKRegionalWrapper (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_hitInputTool("FTK_SGHitInput/FTK_SGHitInput"),
  m_clusterConverterTool("TrigFTKClusterConverterTool"),
  m_pix_cabling_svc("PixelCablingSvc", name),
  m_sct_cabling_svc("SCT_CablingSvc", name),
  m_storeGate(0),
  m_detStore( 0 ),
  m_evtStore(0 ),
  m_pixelId(0),
  m_sctId(0),
  m_idHelper(0),
  m_PIX_mgr(0),
  m_SCT_mgr(0),
  m_IBLMode(0),
  m_fixEndcapL0(false),
  m_ITkMode(false),
  m_pmap_path(""),
  m_pmap(0x0),
  m_rmap_path(""),
  m_rmap(0x0),
  m_ntowers(0),
  m_nplanes(12),
  m_SaveRawHits(true),
  m_SaveHits(false),
  m_SavePerPlane(false),
  m_DumpTestVectors(false),
  m_EmulateDF(false),
  m_Clustering(false),
  m_SaveClusterContent(false),
  m_DiagClustering(true),
  m_SctClustering(false),
  m_PixelClusteringMode(1),
  m_Ibl3DRealistic(false),
  m_DuplicateGanged(true),
  m_GangedPatternRecognition(false),
  m_WriteClustersToESD(false),
  m_getOffline(false),
  m_offlineName("InDetTrackParticles"),
  m_outpath("ftksim_smartwrapper.root"),
  m_outfile(0x0),
  m_hittree(0x0),
  m_hittree_perplane(0),
  m_original_hits(0),
  m_logical_hits(0),
  m_original_hits_per_plane(0),
  m_logical_hits_per_plane(0),
  m_evtinfo(0),
  m_run_number(0),
  m_event_number(0),
  m_trackstree(0),
  m_identifierHashList(0x0),
//  m_pix_rodIdlist({0x130011, 0x111510, 0x111508, 0x112414, 0x130015, 0x111716, 0x112416}),  //old ROD list for consistency.  to be removed soon.
//  m_sct_rodIdlist({0x220005, 0x210005, 0x220007}),

  //run II configuration for towers 44 & 45 (DF Crate 3 slot 9)
  //m_pix_rodIdlist({0x140160, 0x130007, 0x112508, 0x111816, 0x140170, 0x130015,0x112414}),
  //m_sct_rodIdlist({0x21010d, 0x21010c, 0x21010e,0x21010f}),

  //run 1 configuration for towers 44 & 45 (DF Crate 3 slot 9)
  //m_pix_rodIdlist({0x130007, 0x111510, 0x111816, 0x111508,0x112414}),
  //m_sct_rodIdlist({0x21010a, 0x210000}),

  //AUX test vector configuration for tower 44 & 45 (DF Crate 3 slot 9)
  //m_pix_rodIdlist({0x130007, 0x112510, 0x111816, 0x112508,0x112414}),
  //m_sct_rodIdlist({0x21010a, 0x210000}),

  //  m_pix_rodIdlist({0x130007, 0x111510, 0x111816, 0x111508,0x112414,0x1400a3,0x130010,0x130011,0x112508,0x112510}),
  // m_sct_rodIdlist({0x21010a, 0x210000, 0x210109}),

  m_pix_rodIdlist({0x112414, 0x140170, 0x111816, 0x112411, 0x112416, 0x140140, 0x140180, 0x130011}),
  m_sct_rodIdlist({0x210000, 0x210109, 0x210108, 0x230100, 0x220000, 0x220109, 0x22010a, 0x21010a}),

  m_spix_rodIdlist({"0x112414", "0x140170", "0x111816", "0x112411", "0x112416", "0x140140", "0x140180", "0x130011"}),
  m_ssct_rodIdlist({"0x210000", "0x210109", "0x210108", "0x230100", "0x220000", "0x220109", "0x22010a", "0x21010a"}),

  m_FTKPxlClu_CollName("FTK_Pixel_Clusters"), 
  m_FTKPxlCluContainer(0x0),
  m_FTKSCTClu_CollName("FTK_SCT_Cluster"),
  m_FTKSCTCluContainer(0x0),
  m_ftkPixelTruthName("PRD_MultiTruthPixel_FTK"),
  m_ftkSctTruthName("PRD_MultiTruthSCT_FTK"),
  m_mcTruthName("TruthEvent"),
  m_L1ID_to_save(std::vector<int>()),
  offline_locX(nullptr),
  offline_locY(nullptr),
  offline_isPixel(nullptr),
  offline_isBarrel(nullptr),
  offline_layer(nullptr),
  offline_clustID(nullptr),
  offline_trackNumber(nullptr),
  offline_pt(nullptr),
  offline_eta(nullptr),
  offline_phi(nullptr),
  m_offline_cluster_tree(nullptr)
{
  
  declareProperty("TrigFTKClusterConverterTool", m_clusterConverterTool);
  declareProperty("RMapPath",m_rmap_path);
  declareProperty("PMapPath",m_pmap_path);
  declareProperty("OutFileName",m_outpath);
  declareProperty("HitInputTool",m_hitInputTool);
  declareProperty("IBLMode",m_IBLMode);
  declareProperty("FixEndcapL0", m_fixEndcapL0);
  declareProperty("ITkMode",m_ITkMode);
  declareProperty("PixelCablingSvc", m_pix_cabling_svc);
  declareProperty("ISCT_CablingSvc",m_sct_cabling_svc);

  // hit type options
  declareProperty("SaveRawHits",m_SaveRawHits);
  declareProperty("SaveHits",m_SaveHits);

  // special options for test vector production
  declareProperty("SavePerPlane",m_SavePerPlane);
  declareProperty("DumpTestVectors",m_DumpTestVectors);
  
  declareProperty("GetOffline",m_getOffline);
  declareProperty("OfflineName",m_offlineName);

  // clustering options
  declareProperty("Clustering",m_Clustering);
  declareProperty("SaveClusterContent",m_SaveClusterContent);
  declareProperty("DiagClustering",m_DiagClustering);
  declareProperty("SctClustering",m_SctClustering);
  declareProperty("PixelClusteringMode",m_PixelClusteringMode);
  declareProperty("Ibl3DRealistic",m_Ibl3DRealistic);
  declareProperty("DuplicateGanged",m_DuplicateGanged);
  declareProperty("GangedPatternRecognition",m_GangedPatternRecognition);


  //output for PseduoTracking
  declareProperty("WriteClustersToESD",m_WriteClustersToESD);
  declareProperty("FTKPixelClustersCollName",m_FTKPxlClu_CollName,"FTK pixel clusters collection");
  declareProperty("FTKSCTClusterCollName",m_FTKSCTClu_CollName,"FTK SCT clusters collection");

  //for DF board emulation 
  declareProperty("EmulateDF",m_EmulateDF);
  declareProperty("pixRodIds", m_spix_rodIdlist);
  declareProperty("sctRodIds", m_ssct_rodIdlist);
  declareProperty("L1IDToSave", m_L1ID_to_save);
}

FTKRegionalWrapper::~FTKRegionalWrapper ()
{
  if (m_rmap) delete m_rmap;
}

StatusCode FTKRegionalWrapper::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKRegionalWrapper::initialize()" << endmsg;

  // FTK library global setup variables
  FTKSetup::getFTKSetup().setIBLMode(m_IBLMode);
  FTKSetup::getFTKSetup().setfixEndcapL0(m_fixEndcapL0);
  FTKSetup::getFTKSetup().setITkMode(m_ITkMode);

  log << MSG::INFO << "Read the logical layer definitions" << endmsg;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "Main plane map definition missing" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // initialize the tower/region map
  log << MSG::INFO << "Creating region map" << endmsg;
  m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
  if (!(*m_rmap)) {
    log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode schit = m_hitInputTool.retrieve();
  if (schit.isFailure()) {
    log << MSG::FATAL << "Could not retrieve FTK_SGHitInput tool" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO << "Setting FTK_SGHitInput tool" << endmsg;
    // set the pmap address to FTKDataInput to use in processEvent
    m_hitInputTool->reference()->setPlaneMaps(m_pmap,0x0);
  }

  // Get the cluster converter tool
  if (m_clusterConverterTool.retrieve().isFailure() ) {
    log << MSG::ERROR << "Failed to retrieve tool " << m_clusterConverterTool << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve pixel cabling service
  if( m_ITkMode ) {
    // Pixel Cabling database not ready
    // Ignore this error but make sure everything that uses the cabling database is off
    log << MSG::WARNING << "ITkMode is set to True --> not loading Pixel Cabling Svc" << endmsg;
    if( m_DumpTestVectors || m_EmulateDF ) {
      log << MSG::FATAL << "PixelCabling not initialized so m_DumpTestVectors and m_EmulateDF must both be set to false!" << endmsg;
      return StatusCode::FAILURE;
    }
  } else if (m_pix_cabling_svc.retrieve().isFailure()) {
    log << MSG::FATAL << "Failed to retrieve tool " << m_pix_cabling_svc << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_pix_cabling_svc << endmsg;
  }
  
  // Retrieve sct cabling service
  if( m_ITkMode ) {
    // SCT Cabling database not ready
    // Ignore this error but make sure everything that uses the cabling database is off
    log << MSG::WARNING << "ITkMode is set to True --> not loading SCT Cabling Svc" << endmsg;
    if( m_DumpTestVectors || m_EmulateDF ) {
      log << MSG::FATAL << "SCT_Cabling not initialized so m_DumpTestVectors and m_EmulateDF must both be set to false!" << endmsg;
      return StatusCode::FAILURE;
    }
  } else if (m_sct_cabling_svc.retrieve().isFailure()) {
    log << MSG::FATAL << "Failed to retrieve tool " << m_sct_cabling_svc << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_sct_cabling_svc << endmsg;
  }

  if (!m_SaveRawHits && !m_SaveHits) {
      log << MSG::FATAL << "At least one hit format has to be saved: FTKRawHit or FTKHit" << endl;
      return StatusCode::FAILURE;
  }

  // This part retrieves the neccessary pixel/SCT Id helpers. They are intialized by the StoreGateSvc
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
     log << MSG::FATAL << "StoreGate service not found" << endmsg;
     return StatusCode::FAILURE;
   }
  if( service("DetectorStore",m_detStore).isFailure() ) {
     log << MSG::FATAL <<"DetectorStore service not found" << endmsg;
     return StatusCode::FAILURE;
   }
  // if( service("EventStore",m_evtStore).isFailure() ) {
  //    log << MSG::FATAL <<"EventStore service not found" << endmsg;
  //    return StatusCode::FAILURE;
  //  }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_idHelper, "AtlasID").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve AtlasDetector helper from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }

  if( m_detStore->retrieve(m_SCT_mgr, "SCT").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endmsg;
    return StatusCode::FAILURE;
  }

  
  // Write clusters in InDetCluster format to ESD for use in Pseudotracking
  if (m_WriteClustersToESD){
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    // Creating collection for pixel clusters
    m_FTKPxlCluContainer = new InDet::PixelClusterContainer(m_pixelId->wafer_hash_max());
    m_FTKPxlCluContainer->addRef();
    sc = m_storeGate->record(m_FTKPxlCluContainer,m_FTKPxlClu_CollName);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error registering the FTK pixel container in the SG" << endmsg;
      return StatusCode::FAILURE;
    }
    
    // Generic format link for the pixel clusters
    const InDet::SiClusterContainer *symSiContainerPxl(0x0);
    sc = m_storeGate->symLink(m_FTKPxlCluContainer,symSiContainerPxl);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error creating the sym-link to the Pixel clusters" << endmsg;
      return StatusCode::FAILURE;
    }
    
    // Creating collection for the SCT clusters
    m_FTKSCTCluContainer = new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max());
    m_FTKSCTCluContainer->addRef();
    sc = m_storeGate->record(m_FTKSCTCluContainer,m_FTKSCTClu_CollName);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error registering the FTK SCT container in the SG" << endmsg;
      return StatusCode::FAILURE;
    }
    // Generic format link for the pixel clusters
    const InDet::SiClusterContainer *symSiContainerSCT(0x0);
    sc = m_storeGate->symLink(m_FTKSCTCluContainer,symSiContainerSCT);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error creating the sym-link to the SCT clusters" << endmsg;
      return StatusCode::FAILURE;
    }
    
    // getting sct truth
    if(!m_storeGate->contains<PRD_MultiTruthCollection>(m_ftkSctTruthName)) { 
      m_ftkSctTruth = new PRD_MultiTruthCollection;
      StatusCode sc=m_storeGate->record(m_ftkSctTruth,m_ftkSctTruthName); 
      if(sc.isFailure()) { 
	ATH_MSG_WARNING("SCT FTK Truth Container " << m_ftkSctTruthName  
			<<" cannot be recorded in StoreGate !"); 
      } 
      else { 
	ATH_MSG_DEBUG("SCT FTK Truth Container " << m_ftkSctTruthName   
		      << " is recorded in StoreGate"); 
      } 
    } 
    //getting pixel truth 
    if(!m_storeGate->contains<PRD_MultiTruthCollection>(m_ftkPixelTruthName)) { 
      m_ftkPixelTruth = new PRD_MultiTruthCollection;
      StatusCode sc=m_storeGate->record(m_ftkPixelTruth,m_ftkPixelTruthName); 
      if(sc.isFailure()) { 
	ATH_MSG_WARNING("Pixel FTK Truth Container " << m_ftkPixelTruthName  
			<<" cannot be recorded in StoreGate !"); 
      } 
      else { 
	ATH_MSG_DEBUG("Pixel FTK Truth Container " << m_ftkPixelTruthName   
		      << " is recorded in StoreGate"); 
      } 
    }  
  }


  /* initialize the clustering global variables, decalred in TrigFTKSim/atlClusteringLNF.h */
  SAVE_CLUSTER_CONTENT = m_SaveClusterContent;
  DIAG_CLUSTERING = m_DiagClustering;
  SCT_CLUSTERING = m_SctClustering;
  PIXEL_CLUSTERING_MODE = m_PixelClusteringMode;
  IBL3D_REALISTIC = m_Ibl3DRealistic;
  DUPLICATE_GANGED = m_DuplicateGanged;
  GANGED_PATTERN_RECOGNITION = m_GangedPatternRecognition;


  //Dump to the log output the RODs used in the emulation
  if(m_EmulateDF){

    ATH_MSG_DEBUG("Printing full map via  m_pix_cabling_svc->get_idMap_offrob(); ");
    std::map< Identifier, uint32_t> offmap = m_pix_cabling_svc->get_idMap_offrob();
    for (auto mit = offmap.begin(); mit != offmap.end(); mit++){
      //uint id = mit->first;
      ATH_MSG_DEBUG("Pixel offline map hashID to RobId "<<MSG::dec<<mit->first<<" "<<MSG::hex<<mit->second<<MSG::dec);
    }
    ATH_MSG_DEBUG("Printing full SCT map  via m_sct_cabling_svc->getAllRods()");
    std::vector<uint32_t>  sctVector;
    m_sct_cabling_svc->getAllRods(sctVector);
    ATH_MSG_DEBUG("Printing full SCT map  via m_sct_cabling_svc->getAllRods() "<<sctVector.size()<<" rods ");
    
    for(auto mit = sctVector.begin(); mit != sctVector.end(); mit++){
	// Retrive hashlist
	m_sct_cabling_svc->getHashesForRod(m_identifierHashList,*mit );	
	ATH_MSG_DEBUG("Retrieved  "<<m_identifierHashList.size()<<" hashes ");

	for (auto mhit = m_identifierHashList.begin(); mhit != m_identifierHashList.end(); mhit++)
	  ATH_MSG_DEBUG("SCT  offline map hashID to RobId "<<MSG::dec<<*mhit<<" "<<MSG::hex<<(*mit)<<MSG::dec);
      }

    m_pix_rodIdlist.clear();

    for (auto it = m_spix_rodIdlist.begin(); it < m_spix_rodIdlist.end(); it++){
      std::stringstream str;
      str<<(*it);
      int val;
      str>>std::hex>>val;
      
      m_pix_rodIdlist.push_back(val);

      ATH_MSG_DEBUG("Going to test against the following Pix RODIDs "<< MSG::hex
		    << val <<MSG::dec);
    
	std::vector<IdentifierHash> offlineIdHashList;
	m_pix_cabling_svc->getOfflineList(offlineIdHashList, m_pix_cabling_svc->getRobId(val));
	ATH_MSG_DEBUG("Trying m_pix_cabling_svc->getOfflineList(offlineIdHashList, m_pix_cabling_svc->getRobId("<<MSG::hex<<val<<MSG::dec<<"));");
	for (auto oit = offlineIdHashList.begin(); oit != offlineIdHashList.end(); oit++){

	    Identifier id = m_pixelId->wafer_id( *oit );
	    int barrel_ec      = m_pixelId->barrel_ec(id);
	    int layer_disk     = m_pixelId->layer_disk(id);
	    int phi_module     = m_pixelId->phi_module(id);
	    int eta_module     = m_pixelId->eta_module(id);

	    ATH_MSG_DEBUG("hashId "<<*oit<<"for rodID "<<MSG::hex<<val<<MSG::dec
			  << "corresponds to b/ec lay_disk phi eta "<<barrel_ec
			  << " "<<layer_disk<<" "<<phi_module<<" "<<eta_module);
	}
    }

    m_sct_rodIdlist.clear();
    for (auto it = m_ssct_rodIdlist.begin(); it < m_ssct_rodIdlist.end(); it++){
      std::stringstream str;
      str<<(*it);
      int val;
      str>>std::hex>>val;
      m_sct_rodIdlist.push_back(val);
      
      ATH_MSG_DEBUG("Going to test against the following SCT RODIDs "<< MSG::hex
		    << val <<MSG::dec);
    }
    
  }


  return StatusCode::SUCCESS;
}

StatusCode FTKRegionalWrapper::initOutputFile() {
  /*
   * prepare the output structure to store the hits and the other information
   */

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKRegionalWrapper::initOutputFile()" << endmsg;


  // create the output files
  log << MSG::INFO << "Creating output file: "  << m_outpath << endmsg;
  m_outfile = TFile::Open(m_outpath.c_str(),"recreate");


  // create a TTree to store the truth tracks
  m_trackstree = new TTree("truthtracks","Truth tracks");
  // add the branch related to the truth tracks
  m_trackstree->Branch("TruthTracks",&m_truth_tracks);


  // create a TTree to store event information
  m_evtinfo = new TTree("evtinfo","Events info");
  m_evtinfo->Branch("RunNumber",&m_run_number,"RunNumber/I");
  m_evtinfo->Branch("EventNumber",&m_event_number,"EventNumber/I");

  m_evtinfo->Branch("LB",&m_LB,"LB/I");
  m_evtinfo->Branch("BCID",&m_BCID,"BCID/I");
  m_evtinfo->Branch("ExtendedLevel1ID",&m_extendedLevel1ID,"ExtendedLevel1ID/I");
  m_evtinfo->Branch("Level1TriggerType",&m_level1TriggerType,"Level1TriggerType/I");
  m_evtinfo->Branch("Level1TriggerInfo",&m_level1TriggerInfo);
  m_evtinfo->Branch("AverageInteractionsPerCrossing",&m_averageInteractionsPerCrossing,"AverageInteractionsPerCrossing/F");
  m_evtinfo->Branch("ActualInteractionsPerCrossing",&m_actualInteractionsPerCrossing,"ActualInteractionsPerCrossing/F");  

  // create and populate the TTree
  m_hittree = new TTree("ftkhits","Raw hits for the FTK simulation");
  m_hittree_perplane = new TTree("ftkhits_perplane","Raw hits for the FTK simulation");


  if (m_getOffline) {
    offline_locX = new std::vector<float>;
    offline_locY = new std::vector<float>;
    offline_isPixel = new std::vector<bool>;
    offline_isBarrel = new std::vector<bool>;
    offline_layer = new std::vector<int>;
    offline_clustID = new std::vector<int>;
    offline_trackNumber = new std::vector<int>;
    offline_pt = new std::vector<float>;
    offline_eta = new std::vector<float>;
    offline_phi = new std::vector<float>;

    m_offline_cluster_tree = new TTree("offline_cluster_tree","offline_cluster_tree");
    m_offline_cluster_tree->Branch("offline_locX",&offline_locX);
    m_offline_cluster_tree->Branch("offline_locY",&offline_locY);
    m_offline_cluster_tree->Branch("offline_is_Pixel",&offline_isPixel);
    m_offline_cluster_tree->Branch("offline_is_Barrel",&offline_isBarrel);
    m_offline_cluster_tree->Branch("offline_layer",&offline_layer);
    m_offline_cluster_tree->Branch("offline_clustID",&offline_clustID);
    
    m_offline_cluster_tree->Branch("offline_trackNumber",&offline_trackNumber);
    m_offline_cluster_tree->Branch("offline_pt",&offline_pt);
    m_offline_cluster_tree->Branch("offline_eta",&offline_eta);
    m_offline_cluster_tree->Branch("offline_phi",&offline_phi);
  }


  // prepare a branch for each tower
  m_ntowers = m_rmap->getNumRegions();

  if (m_SaveRawHits) { // Save FTKRawHit data
    m_original_hits = new vector<FTKRawHit>[m_ntowers];
    if (m_SavePerPlane) { m_original_hits_per_plane = new vector<FTKRawHit>*[m_ntowers]; }
    
    for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
      m_hittree->Branch(Form("RawHits%d.",ireg),&m_original_hits[ireg], 32000, 1);
      
      if (m_SavePerPlane) {
        m_original_hits_per_plane[ireg] = new vector<FTKRawHit>[m_nplanes];
        for (int iplane=0;iplane!=m_nplanes;++iplane) { // planes loop
          m_hittree_perplane->Branch(Form("RawHits_t%d_p%d.",ireg,iplane),&m_original_hits_per_plane[ireg][iplane],32000, 1);
        }
      }
    } // end towers loop
  }

  if (m_SaveHits) {
    m_logical_hits = new vector<FTKHit>[m_ntowers];
    if (m_SavePerPlane) { m_logical_hits_per_plane = new vector<FTKHit>*[m_ntowers]; }
    
    for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
      m_hittree->Branch(Form("Hits%d.",ireg),&m_logical_hits[ireg], 32000, 1);

      if (m_SavePerPlane) {
        m_logical_hits_per_plane[ireg] = new vector<FTKHit>[m_nplanes];
        for (int iplane=0;iplane!=m_nplanes;++iplane) { // planes loop
          m_hittree_perplane->Branch(Form("Hits_t%d_p%d.",ireg,iplane), &m_logical_hits_per_plane[ireg][iplane],32000, 1);
        }
      }
    } // end towers loop
  }
  return StatusCode::SUCCESS;
}

StatusCode FTKRegionalWrapper::execute()
{

  // moved here for compatibilty with multithreaded athena
  if (m_outfile == 0) {
    StatusCode sc = initOutputFile();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Did not initialize output file and trees correctly!");
    }
  }

  // retrieve the pointer to the datainput object
  FTKDataInput *datainput = m_hitInputTool->reference();

  // reset the branches
  for (int ireg=0;ireg!=m_ntowers;++ireg) {
    if (m_SaveRawHits) m_original_hits[ireg].clear();
    if (m_SaveHits) m_logical_hits[ireg].clear();
    
    if (m_SavePerPlane) {
      for (int iplane=0;iplane!=m_nplanes;++iplane) { // planes loop
	if (m_SaveRawHits) m_original_hits_per_plane[ireg][iplane].clear();
	if (m_SaveHits) m_logical_hits_per_plane[ireg][iplane].clear();
      }
    }
  }

  if (m_getOffline) {
    offline_locX->clear();
    offline_locY->clear();
    offline_isPixel->clear();
    offline_isBarrel->clear();
    offline_layer->clear(); 
    offline_clustID->clear();
    offline_trackNumber->clear(); 
    offline_pt->clear(); 
    offline_eta->clear();
    offline_phi->clear();
  }

  // ask to read the data in the current event
  datainput->readData();

  // get the event information
  m_run_number = datainput->runNumber(); // event's run number
  m_event_number = datainput->eventNumber(); // event number
  m_LB = datainput->LB();
  m_BCID = datainput->BCID();
  m_averageInteractionsPerCrossing = datainput->averageInteractionsPerCrossing();
  m_actualInteractionsPerCrossing = datainput->actualInteractionsPerCrossing();
  m_level1TriggerType = datainput->level1TriggerType();
  m_level1TriggerInfo = datainput->level1TriggerInfo();
  m_extendedLevel1ID = datainput->extendedLevel1ID();

  if (std::find(m_L1ID_to_save.begin(), m_L1ID_to_save.end(), m_level1TriggerType) == m_L1ID_to_save.end() 
      && m_L1ID_to_save.size() != 0)
    return StatusCode::SUCCESS;

  m_evtinfo->Fill();
  // retrieve the original list of hits, the list is copied because the clustering will change it
  vector<FTKRawHit> fulllist;

  bool dumpedSCT = false;

  //if DF emulation is requested then first check if hits are in DF Rods before doing clustering or writing to file
  if(m_EmulateDF){

    //get list of original hits
    vector<FTKRawHit> templist = datainput->getOriginalHits();

    vector<FTKRawHit>::const_iterator ihit = templist.begin();
    vector<FTKRawHit>::const_iterator ihitE = templist.end();

    for (;ihit!=ihitE;++ihit) { // hit loop
      const FTKRawHit &currawhit = *ihit;
    
      //first get the hit's Module identifier hash
      uint32_t modHash = currawhit.getIdentifierHash();
      

      if (currawhit.getIsSCT()){

	ATH_MSG_VERBOSE("Processing SCT hit");
     	//SCT
	
	//then get the corresponding RobId
	uint32_t robid = m_sct_cabling_svc->getRobIdFromHash(modHash);
	
	if (dumpedSCT == false){
	  ATH_MSG_VERBOSE("Dumping SCT Rod List ");
	for (auto its = m_sct_rodIdlist.begin(); its <  m_sct_rodIdlist.end(); ++its)
	  ATH_MSG_VERBOSE(MSG::hex <<*its<<MSG::dec<< " is in the SCT Rod list");
	dumpedSCT = true;
	}
	//then try to find in rob list
	auto it = find(m_sct_rodIdlist.begin(), m_sct_rodIdlist.end(), robid);
	
	ATH_MSG_VERBOSE("Trying to find "<< MSG::hex <<robid<<MSG::dec
		      << "in the DF SCT Rod list");

	//only keep hit if found
	if (it != m_sct_rodIdlist.end()){
	  ATH_MSG_VERBOSE("Found the SCT module in the DF Rod list!");
	}else{
	  ATH_MSG_VERBOSE("SCT Module is not in the DF Rod list!");
	  continue;
	}	
      }else if (currawhit.getIsPixel()) {
	  ATH_MSG_VERBOSE("Processing Pixel hit");

	  //pixel
	  //need to get the identifier from the hash
	  Identifier dehashedId = m_pixelId->wafer_id(modHash);

	  //then get the corresponding RobId
	  uint32_t robid = m_pix_cabling_svc->getRobId(dehashedId);
	  
	  //then try to find in rob list
	  auto it = find(m_pix_rodIdlist.begin(), m_pix_rodIdlist.end(), robid);
	  
	  ATH_MSG_VERBOSE("Trying to find pixel "<< MSG::hex <<robid<<MSG::dec
			<< "in the DF Rod list from "<<dehashedId);
	  //only keep hit if found
	  if (it != m_pix_rodIdlist.end()){
	    ATH_MSG_VERBOSE("Found the Pixel module in the DF Rod list!");
	  }else{
	    ATH_MSG_VERBOSE("Pixel Module is not in the DF Rod list!");
	    continue;
	  }
	  
      }else{
	//this shouldn't happen, so throw error
	ATH_MSG_ERROR("Hit is neither Pixel or SCT!!");
	return StatusCode::FAILURE;
      }
      //save the hit if it has the correct RodID
      ATH_MSG_DEBUG("Found hit to keep");
      fulllist.push_back(currawhit);
    }
  }else{
    //if no DF emulation, just copy the hits as originally intended
    fulllist = datainput->getOriginalHits();
  }
  
  ATH_MSG_VERBOSE("Going to run  on "<< fulllist.size()<<" hits");

  // if the clustering is requested it has to be done before the hits are distributed
  if (m_Clustering ) {
    atlClusteringLNF(fulllist);
  }

  if (m_getOffline) {
    const xAOD::TrackParticleContainer *offlineTracks = nullptr;
    if(m_storeGate->retrieve(offlineTracks,m_offlineName).isFailure()) {
      ATH_MSG_ERROR("Failed to retrieve Offline Tracks " << m_offlineName);
      return StatusCode::FAILURE;
    }
    if(offlineTracks->size()!=0){
      auto track_it   = offlineTracks->begin();
      auto last_track = offlineTracks->end();
      for (int iTrk=0 ; track_it!= last_track; track_it++, iTrk++){
	auto track = (*track_it)->track();
	offline_pt->push_back((*track_it)->pt()*(*track_it)->charge());
	offline_eta->push_back((*track_it)->eta());
	offline_phi->push_back((*track_it)->phi());
	const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();   
	if(!trackStates)     ATH_MSG_ERROR("trackStatesOnSurface troubles");
	DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
	DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end=trackStates->end();
	if (!(*it)) {
	  ATH_MSG_WARNING("TrackStateOnSurface == Null" << endl);
	  continue;
	}
	for (; it!=it_end; it++) {
	  const Trk::TrackStateOnSurface* tsos=(*it);       
	  if (tsos == 0) continue;
	  if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){
	    const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
	    if(  (*it)->trackParameters() !=0 &&
		 (*it)->trackParameters()->associatedSurface().associatedDetectorElement() != nullptr &&
		 (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->identify() !=0
		 ){
	      const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
	      const Identifier & hitId = hit->identify();

	      if (m_idHelper->is_pixel(hitId)) {
		offline_isPixel->push_back(true);
		offline_isBarrel->push_back(m_pixelId->is_barrel(hitId));
		const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(hitId);
		offline_clustID->push_back(sielement->identifyHash());
		offline_trackNumber->push_back(iTrk);
		offline_layer->push_back(m_pixelId->layer_disk(hitId));
		offline_locX->push_back((float)measurement->localParameters()[Trk::locX]);
		offline_locY->push_back((float)measurement->localParameters()[Trk::locY]);
	      }
	      else if (m_idHelper->is_sct(hitId)) {
		offline_isPixel->push_back(false);
		offline_isBarrel->push_back(m_sctId->is_barrel(hitId));
		const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(hitId);
		offline_clustID->push_back(sielement->identifyHash());
		offline_trackNumber->push_back(iTrk);
		offline_layer->push_back(m_sctId->layer_disk(hitId));
		offline_locX->push_back((float)measurement->localParameters()[Trk::locX]);
		offline_locY->push_back(-99999.9);
	      }
	    }
	  }
	}
      }
    }
  }

  //get all the containers to write clusters
  if(m_WriteClustersToESD){
    if(!m_storeGate->contains<PRD_MultiTruthCollection>(m_ftkSctTruthName)) { 
      m_ftkSctTruth = new PRD_MultiTruthCollection;
      
      StatusCode sc=m_storeGate->record(m_ftkSctTruth,m_ftkSctTruthName,false); 
      if(sc.isFailure()) { 
	ATH_MSG_WARNING("SCT FTK Truth Container " << m_ftkSctTruthName  
			<<" cannot be re-recorded in StoreGate !"); 
      }
    } 
    
    if(!m_storeGate->contains<PRD_MultiTruthCollection>(m_ftkPixelTruthName)) { 
      m_ftkPixelTruth = new PRD_MultiTruthCollection;
      
      StatusCode sc=m_storeGate->record(m_ftkPixelTruth,m_ftkPixelTruthName,false); 
      if(sc.isFailure()) { 
	ATH_MSG_WARNING("SCT FTK Truth Container " << m_ftkPixelTruthName  
			<<" cannot be re-recorded in StoreGate !"); 
      } 
    }   
    
    
    // Check the FTK pixel container
    if (!m_storeGate->contains<InDet::PixelClusterContainer>(m_FTKPxlClu_CollName)) {
      m_FTKPxlCluContainer->cleanup();
      if (m_storeGate->record(m_FTKPxlCluContainer,m_FTKPxlClu_CollName,false).isFailure()) {
	return StatusCode::FAILURE;
      }
    }
    
    // check the FTK SCT container
    if (!m_storeGate->contains<InDet::SCT_ClusterContainer>(m_FTKSCTClu_CollName)) {
      m_FTKSCTCluContainer->cleanup();
      if (m_storeGate->record(m_FTKSCTCluContainer,m_FTKSCTClu_CollName,false).isFailure()) {
	return StatusCode::FAILURE;
      }
    }
  }




  // prepare to iterate on the input files
  vector<FTKRawHit>::const_iterator ihit = fulllist.begin();
  vector<FTKRawHit>::const_iterator ihitE = fulllist.end();

  for (;ihit!=ihitE;++ihit) { // hit loop
    const FTKRawHit &currawhit = *ihit;

    if( m_ITkMode ) {
      // In the ITk geometry, some of the plane IDs are -1 if the layers are not yet being used.
      // This causes the code in this hit loop to crash. As a workaround for the moment, we currently
      // skip over hits in layers that are not included in the FTK geometry, with plane = -1
      if( m_pmap->getMap( currawhit.getHitType() , !(currawhit.getBarrelEC()==0) , currawhit.getLayer() ).getPlane() == -1 )
	continue;
    }
    
    //cout << "Hit " << currawhit.getHitType() << ": " << currawhit.getEventIndex() << " " << currawhit.getBarcode() << endl;
    // calculate the equivalent hit
    FTKHit hitref = currawhit.getFTKHit(m_pmap);
    if (m_Clustering ) {		
      assert(currawhit.getTruth());
      hitref.setTruth(*(currawhit.getTruth()));
    }
    
    if(m_WriteClustersToESD){
      int dim = hitref.getDim();
      switch (dim) {
      case 0: {
	//missing hit - just ignore this for the time being
      }
	break;
      case 1: {
	InDet::SCT_Cluster* pSctCL = m_clusterConverterTool->createSCT_Cluster(hitref.getIdentifierHash(), hitref.getCoord(0), hitref.getNStrips() ); //createSCT_Cluster(h);

	if(pSctCL!=NULL) {
	  InDet::SCT_ClusterCollection* pColl = m_clusterConverterTool->getCollection(m_FTKSCTCluContainer, hitref.getIdentifierHash());
	  if(pColl!=NULL) {
	    pSctCL->setHashAndIndex(pColl->identifyHash(), pColl->size()); 
	    pColl->push_back(pSctCL); 
	    const MultiTruth& t = hitref.getTruth();
	    m_clusterConverterTool->createSCT_Truth(pSctCL->identify(), t, m_ftkSctTruth, m_mcEventCollection, m_storeGate, m_mcTruthName);
	  }
	}
      }
	break; 
      case 2: {
	InDet::PixelCluster* pPixCL = m_clusterConverterTool->createPixelCluster(hitref.getIdentifierHash(), hitref.getCoord(0), hitref.getCoord(1), hitref.getEtaWidth(), hitref.getPhiWidth(), hitref.getCoord(1)); //need to fix trkPerigee->eta());
	
	if(pPixCL!=NULL) {
	  InDet::PixelClusterCollection* pColl = m_clusterConverterTool->getCollection(m_FTKPxlCluContainer, hitref.getIdentifierHash());
	  if(pColl!=NULL) {
	    pPixCL->setHashAndIndex(pColl->identifyHash(), pColl->size()); 
	    pColl->push_back(pPixCL); 
	    const MultiTruth& t = hitref.getTruth();
	    m_clusterConverterTool->createPixelTruth(pPixCL->identify(), t, m_ftkPixelTruth, m_mcEventCollection, m_storeGate, m_mcTruthName);
	  }
	}
      }
	break;
      }
    }


    // get plane id
    const int plane = hitref.getPlane();
    
    // check the region
    for (int ireg=0;ireg!=m_ntowers;++ireg) {
      
      if (m_rmap->isHitInRegion(hitref,ireg)) {
        // if the equivalent hit is compatible with this tower the hit is saved
        if (m_SaveRawHits) m_original_hits[ireg].push_back(currawhit);
        if (m_SaveHits) m_logical_hits[ireg].push_back(hitref);

        if (m_SavePerPlane) {
	  if (m_SaveRawHits)
	    m_original_hits_per_plane[ireg][plane].push_back(currawhit);
	  if (m_SaveHits)
	    m_logical_hits_per_plane[ireg][plane].push_back(hitref);
	}
      }
    }
  } // end hit loop
  

  // fill the branches
  m_hittree->Fill();

  if (m_getOffline) m_offline_cluster_tree->Fill();

  if (m_SavePerPlane) { m_hittree_perplane->Fill(); }
  
  // get the list of the truth tracks
  m_truth_tracks.clear();
  const vector<FTKTruthTrack> &truthtracks = datainput->getTruthTrack();
  m_truth_tracks.insert(m_truth_tracks.end(),truthtracks.begin(),truthtracks.end());
  // Write the tracks
  m_trackstree->Fill();
  if (m_DumpTestVectors) {
      // Dumps the data, needed to be placed here in order to make sure that StoreGateSvc has loaded
      dumpFTKTestVectors(m_pmap,m_rmap);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------------
// ------- Added by Jean----------------------------------------------------------------------------------------------------

bool FTKRegionalWrapper::dumpFTKTestVectors(FTKPlaneMap *pmap, FTKRegionMap *rmap)
{
    MsgStream log(msgSvc(), name());   

   // Some dummy loop variables
    uint64_t onlineId ;
    IdentifierHash hashId;
    Identifier id;

    stringstream ss ; 
    int m_hitTyp;
    //------------------------ Do PIXEL RODIDs first ------------------------------
    // Note PIXEL RODs are input
    vector<uint32_t>::iterator m_rodit = m_pix_rodIdlist.begin();
    vector<uint32_t>::iterator m_rodit_e = m_pix_rodIdlist.end();
    m_hitTyp = 1; // pixel
        
    for (; m_rodit!=m_rodit_e; m_rodit++){

      // Create file for output, format: LUT_0xABCDFGH.txt
      ss << "LUT_";
      ss.setf(ios::showbase); 
      ss << hex << *m_rodit;
      ss.unsetf(ios::showbase);
      ss << ".txt";
      ofstream myfile(ss.str() );
      myfile.setf(ios::right | ios::showbase);

      if ( myfile.is_open() ) {
	for (int m_link = 0; m_link < 128;m_link++){   // Loop over all modules
	    // Retrieve onlineId = m_link+ROD
	    onlineId  = m_pix_cabling_svc->getOnlineIdFromRobId((*m_rodit),m_link) ;
	    hashId   = m_pix_cabling_svc->getOfflineIdHash(onlineId);

	    if (hashId <=999999){ // Adjust for correct output format incase of invalid hashId // TODO: add a proper cutoff!

	        id = m_pixelId->wafer_id( hashId );
		int barrel_ec      = m_pixelId->barrel_ec(id);
		int layer_disk     = m_pixelId->layer_disk(id);
		int phi_module     = m_pixelId->phi_module(id);
		int eta_module     = m_pixelId->eta_module(id);
		int eta_module_min = m_pixelId->eta_module_min(id);
		int eta_module_max = m_pixelId->eta_module_max(id);
		int eta_index      = m_pixelId->eta_index(id);
		int eta_index_max  = m_pixelId->eta_index_max(id);

		// Get Plane information
                FTKPlaneSection &pinfo =  pmap->getMap(m_hitTyp,!(barrel_ec==0),layer_disk);

		// Get tower information
	        FTKRawHit dummy;

	        dummy.setBarrelEC(barrel_ec);
	        dummy.setLayer(layer_disk);
	        dummy.setPhiModule(phi_module);
	        dummy.setEtaModule(eta_module);

		stringstream towerList;
	        FTKHit hitref = dummy.getFTKHit(pmap);
		int nTowers = 0;
		int towerId;
	        for (towerId = 0; towerId<64;towerId++){ // Loop over all 64 eta-phi towers
	             if (rmap->isHitInRegion(hitref,towerId)){
		       towerList << towerId << ", ";
		       nTowers++;
		     }
		}
		

		// Dump data to file:  
	        // Comment out lines below to adjust output (
		// Linknumber | OnlineID | HashID | Plane | #Towers| TowerList|
		 myfile << dec   << setprecision(4) << m_link  << '\t'
	                << hex   << onlineId                   << '\t'
			<< dec   << hashId                     << '\t'
	                << dec   << pinfo.getPlane()           << '\t'
                        << dec   << nTowers                    << '\t'; 
		      

		 // Dump TowerList
                myfile.unsetf(ios::right | ios::showbase);
		myfile << setw(20)  << towerList.str() << '\t';
		towerList.str( string() ) ;

	        myfile.setf(ios::right | ios::showbase);
	          
	        // Dump the extend table
	        myfile << "#"                                 << '\t' 
		       << m_hitTyp                            << '\t'
		       << barrel_ec                           << '\t'
                       << layer_disk                          << '\t'
                       << phi_module                          << '\t'
                       << eta_module                          << '\t'
                       << eta_module_min                      << '\t'
                       << eta_module_max                      << '\t'
                       << eta_index                           << '\t'
                       << eta_index_max                       << '\t'
		       << endl;      

	    }
            else{
	      myfile  << '\t' << '\t' << '\t' <<hex << hashId << endl;    // Dump invalid hashId
	        }
	  }
      }
      else{
	 log << MSG::ERROR << "Couldn't open file to store online-/offlinehashid" << endmsg;
	 return false;
      }
      // Clear the stringstream and close file
      ss.str( string() ); 
      myfile.close();
    }	  
    
// ----------------------------Do SCT rodIds-------------------------------------   
// Note no input for specific SCT RODs are being used here. 
// The getAllRods returns all of the rods in the StoreGateSvc
    m_hitTyp = 0;
    vector<uint32_t> sctrods;
    m_sct_cabling_svc->getAllRods(sctrods);
    vector<uint32_t>::iterator rodit = sctrods.begin();
    vector<uint32_t>::iterator rodit_e = sctrods.end();
    id = 0;

    for (; rodit != rodit_e ; rodit++){
      // Create file for output, format: LUT_0xABCDFGH.txt
      ss << "LUT_";
      ss.setf(ios::showbase); 
      ss << hex << *rodit;
      ss.unsetf(ios::showbase);
      ss << ".txt";
      ofstream myfile(ss.str() );
      myfile.setf( ios::right | ios::showbase);
      
      if (myfile.is_open() ) {

	// Retrive hashlist
	 m_sct_cabling_svc->getHashesForRod(m_identifierHashList,*rodit );

	 // Some dumping variables
	 vector<IdentifierHash>::const_iterator hashit = m_identifierHashList.begin();
	 vector<IdentifierHash>::const_iterator hashit_e = m_identifierHashList.end();
	 SCT_OnlineId  m_sct_onlineId;

	 for (; hashit != hashit_e; ++hashit){  // TODO: Check for invalid onlineId && hashId numbers (?)

	    // Retrieve OnlineId
	      m_sct_onlineId = m_sct_cabling_svc->getOnlineIdFromHash( *hashit );
	      if (m_sct_onlineId.rod()  == (*rodit)){ // Check for correct rodId

		myfile.setf(ios::right | ios::showbase);
		id  = m_sctId->wafer_id( *hashit);
		  
	        int barrel_ec      = m_sctId->barrel_ec(id);
	        int layer_disk     = m_sctId->layer_disk(id);
	        int phi_module     = m_sctId->phi_module(id);
	        int phi_module_max = m_sctId->phi_module_max(id);
	        int eta_module     = m_sctId->eta_module(id)  ;
	        int eta_module_min = m_sctId->eta_module_min(id)  ;
	        int eta_module_max = m_sctId->eta_module_max(id)  ;
		int side           = m_sctId->side(id);
		int strip          = m_sctId->strip(id);

               // Get  plane information
		FTKPlaneSection &pinfo =  pmap->getMap(m_hitTyp,!(barrel_ec==0),layer_disk);

	        // Get tower information
	        FTKRawHit dummy;
	        dummy.setBarrelEC(barrel_ec);
	        dummy.setLayer(layer_disk);
	        dummy.setPhiModule(phi_module);
	        dummy.setEtaModule(eta_module);

		stringstream towerList;
		int towerId;
		int nTowers = 0;
	        FTKHit hitref = dummy.getFTKHit(pmap);
	        for (towerId = 0; towerId<64;towerId++){ // Looping over all 64 eta-phi towers
	             if (rmap->isHitInRegion(hitref,towerId)){
		       towerList << towerId << ", ";
		       nTowers++;
		     }
		}

       

	        // Dump data to file:  
	        // Comment out lines below to adjust output
		// Linknumber | OnlineID | HashID | Plane | #Towers | TowerList
		 myfile << dec  <<  m_sct_onlineId.fibre() << '\t'
			<< hex  <<  m_sct_onlineId         << '\t'
		        << dec  << *hashit                 << '\t'
	                << dec  << pinfo.getPlane()         << '\t'
                        << dec  << nTowers                  << '\t';

		 // Dump TowerList

		myfile.unsetf(ios::right | ios::showbase);
		myfile << setw(20) << towerList.str() << '\t';
		towerList.str( string() );
		myfile.setf(ios::right | ios::showbase);

		// Dump Extended table
		myfile  <<  "#"                            << '\t'
                        << m_hitTyp                        << '\t'
		        << barrel_ec                       << '\t'
                        << layer_disk                      << '\t'
                        << phi_module                      << '\t'
                        << phi_module_max                  << '\t'
                        << eta_module                      << '\t'
                        << eta_module_min                  << '\t'
                        << eta_module_max                  << '\t'
                        << side                            << '\t'
                        << strip                           << '\t'
	                << endl;  
	      }
	 }
      
      }
      else {
	log << MSG::ERROR << "Couldn't open file to store online-/offlinehashid" << endmsg;
	 return false;
	}
      // Clear the stringstream and close file
      ss.str( string() ); 
      myfile.close();
    }

    log << MSG::INFO << "Dumped FTKTestvectors" << endmsg;
    return true;
    
}
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------


StatusCode FTKRegionalWrapper::finalize()
{

  // the cluster container has to be explictly released
  if(m_WriteClustersToESD){
    m_FTKPxlCluContainer->cleanup();
    m_FTKPxlCluContainer->release();
    m_FTKSCTCluContainer->cleanup();
    m_FTKSCTCluContainer->release();
  }

  // close the output files, but check that it exists (for athenaMP)
  if (m_outfile) {
    m_outfile->Write();
    m_outfile->Close();
  }

  return StatusCode::SUCCESS;
}
