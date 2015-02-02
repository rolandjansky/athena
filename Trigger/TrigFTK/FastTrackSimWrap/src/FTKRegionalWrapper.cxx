/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastTrackSimWrap/FTKRegionalWrapper.h"
#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/atlClustering.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_Cabling/SCT_OnlineId.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "SCT_Cabling/SCT_SerialNumber.h"

#include <algorithm>

FTKRegionalWrapper::FTKRegionalWrapper (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_hitInputTool("FTK_SGHitInput/FTK_SGHitInput"),
  m_pix_cabling_svc("PixelCablingSvc", name),
  m_sct_cabling_svc("SCT_CablingSvc", name),
  m_storeGate(0),
  m_detStore( 0 ),
  m_evtStore(0 ),
  m_pixelId(0),
  m_sctId(0),
  m_IBLMode(false),
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
  m_Clustering(false),
  m_SaveClusterContent(false),
  m_DiagClustering(true),
  m_SctClustering(false),
  m_PixelClusteringMode(1),
  m_DuplicateGanged(true),
  m_GangedPatternRecognition(false),
  m_outpath("ftksim_smartwrapper.root"),
  m_outfile(0x0),
  m_hittree(0x0),
  m_identifierHashList(0x0),
  m_pix_rodIdlist({0x130011, 0x111510, 0x111508, 0x112414, 0x130015, 0x111716, 0x112416}),
  m_sct_rodIdlist({0x220005, 0x210005, 0x220007})
{
  declareProperty("RMapPath",m_rmap_path);
  declareProperty("PMapPath",m_pmap_path);
  declareProperty("OutFileName",m_outpath);
  declareProperty("HitInputTool",m_hitInputTool);
  declareProperty("IBLMode",m_IBLMode);
  declareProperty("PixelCablingSvc", m_pix_cabling_svc);
  declareProperty("ISCT_CablingSvc",m_sct_cabling_svc);

  // hit type options
  declareProperty("SaveRawHits",m_SaveRawHits);
  declareProperty("SaveHits",m_SaveHits);
  declareProperty("SavePerPlane",m_SavePerPlane);
  declareProperty("DumpTestVectors",m_DumpTestVectors);
  
  // clustering options
  declareProperty("Clustering",m_Clustering);
  declareProperty("SaveClusterContent",m_SaveClusterContent);
  declareProperty("DiagClustering",m_DiagClustering);
  declareProperty("SctClustering",m_SctClustering);
  declareProperty("PixelClusteringMode",m_PixelClusteringMode);
  declareProperty("DuplicateGanged",m_DuplicateGanged);
  declareProperty("GangedPatternRecognition",m_GangedPatternRecognition);
}

FTKRegionalWrapper::~FTKRegionalWrapper ()
{
  if (m_rmap) delete m_rmap;
}

StatusCode FTKRegionalWrapper::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKRegionalWrapper::initialize()" << endreq;

  // FTK library global setup variables
  FTKSetup::getFTKSetup().setIBLMode(m_IBLMode);

  log << MSG::INFO << "Read the logical layer definitions" << endreq;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "Main plane map definition missing" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }
  }

  // initialize the tower/region map
  log << MSG::INFO << "Creating region map" << endreq;
  m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
  if (!(*m_rmap)) {
    log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode schit = m_hitInputTool.retrieve();
  if (schit.isFailure()) {
    log << MSG::FATAL << "Could not retrieve FTK_SGHitInput tool" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO << "Setting FTK_SGHitInput tool" << endreq;
    // set the pmap address to FTKDataInput to use in processEvent
    m_hitInputTool->reference()->setPlaneMaps(m_pmap,0x0);
  }

// Retrieve pixel cabling service
  if (m_pix_cabling_svc.retrieve().isFailure()) {
    log << MSG::FATAL << "Failed to retrieve tool " << m_pix_cabling_svc << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_pix_cabling_svc << endreq;
  }
  
  // Retrieve sct cabling service
  if (m_sct_cabling_svc.retrieve().isFailure()) {
    log << MSG::FATAL << "Failed to retrieve tool " << m_sct_cabling_svc << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Retrieved tool " << m_sct_cabling_svc << endreq;
  }

  if (!m_SaveRawHits && !m_SaveHits) {
      log << MSG::FATAL << "At least one hit format has to be saved: FTKRawHit or FTKHit" << endl;
      return StatusCode::FAILURE;
  }

  /*
   * prepare the output structure to store the hits and the other information
   */

  // create the output files
  log << MSG::INFO << "Creating output file: "  << m_outpath << endreq;
  m_outfile = TFile::Open(m_outpath.c_str(),"recreate");

  // create a TTree to store event information
  m_evtinfo = new TTree("evtinfo","Events info");
  m_evtinfo->Branch("RunNumber",&m_run_number,"RunNumber/I");
  m_evtinfo->Branch("EventNumber",&m_event_number,"EventNumber/I");
  
  // create and populate the TTree
  m_hittree = new TTree("ftkhits","Raw hits for the FTK simulation");
  m_hittree_perplane = new TTree("ftkhits_perplane","Raw hits for the FTK simulation");
  
  // prepare a branch for each tower
  m_ntowers = m_rmap->getNumRegions();

  if (m_SaveRawHits) { // Save FTKRawHit data
    m_original_hits = new vector<FTKRawHit>[m_ntowers];
    if (m_SavePerPlane) { m_original_hits_per_plane = new vector<FTKRawHit>*[m_ntowers]; }
    
    for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
      m_hittree->Branch(Form("RawHits%d.",ireg),&m_original_hits[ireg]);
      
      if (m_SavePerPlane) {
        m_original_hits_per_plane[ireg] = new vector<FTKRawHit>[m_nplanes];
        for (int iplane=0;iplane!=m_nplanes;++iplane) { // planes loop
          m_hittree_perplane->Branch(Form("RawHits_t%d_p%d.",ireg,iplane),
              &m_original_hits_per_plane[ireg][iplane]);
        }
      }
    } // end towers loop
  }

  if (m_SaveHits) {
    m_logical_hits = new vector<FTKHit>[m_ntowers];
    if (m_SavePerPlane) { m_logical_hits_per_plane = new vector<FTKHit>*[m_ntowers]; }
    
    for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
      m_hittree->Branch(Form("Hits%d.",ireg),&m_logical_hits[ireg]);

      if (m_SavePerPlane) {
        m_logical_hits_per_plane[ireg] = new vector<FTKHit>[m_ntowers];
        for (int iplane=0;iplane!=m_nplanes;++iplane) { // planes loop
          m_hittree_perplane->Branch(Form("Hits_t%d_p%d.",ireg,iplane),
              &m_logical_hits_per_plane[ireg][iplane]);
        }
      }
    } // end towers loop
  }

  // This part retrieves the neccessary pixel/SCT Id helpers. They are intialized by the StoreGateSvc
  if( service("StoreGateSvc", m_storeGate).isFailure() ) {
     log << MSG::FATAL << "StoreGate service not found" << endreq;
     return StatusCode::FAILURE;
   }
  if( service("DetectorStore",m_detStore).isFailure() ) {
     log << MSG::FATAL <<"DetectorStore service not found" << endreq;
     return StatusCode::FAILURE;
   }
  if( m_detStore->retrieve(m_pixelId, "PixelID").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_detStore->retrieve(m_sctId, "SCT_ID").isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve Pixel helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  // create a TTree to store the truth tracks
  m_trackstree = new TTree("truthtracks","Truth tracks");
  // add the branch related to the truth tracks
  m_trackstree->Branch("TruthTracks",&m_truth_tracks);

  /* initialize the clustering global variables, decalred in TrigFTKSim/atlClusteringLNF.h */
  SAVE_CLUSTER_CONTENT = m_SaveClusterContent;
  DIAG_CLUSTERING = m_DiagClustering;
  SCT_CLUSTERING = m_SctClustering;
  PIXEL_CLUSTERING_MODE = m_PixelClusteringMode;
  DUPLICATE_GANGED = m_DuplicateGanged;
  GANGED_PATTERN_RECOGNITION = m_GangedPatternRecognition;

  return StatusCode::SUCCESS;
}

StatusCode FTKRegionalWrapper::execute()
{
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

  // ask to read the data in the current event
  datainput->readData();

  // get the event information
  m_run_number = datainput->runNumber(); // event's run number
  m_event_number = datainput->eventNumber(); // event number
  m_evtinfo->Fill();

  // retrieve the original list of hits, the list is copied because the clustering will change it
  vector<FTKRawHit> fulllist = datainput->getOriginalHits();
  
  // if the clustering is requested it has to be done before the hits are distributed
  if (m_Clustering ) {
    atlClusteringLNF(fulllist);
  }

  // prepare to iterate on the input files
  vector<FTKRawHit>::const_iterator ihit = fulllist.begin();
  vector<FTKRawHit>::const_iterator ihitE = fulllist.end();

  for (;ihit!=ihitE;++ihit) { // hit loop
    const FTKRawHit &currawhit = *ihit;
   
    //cout << "Hit " << currawhit.getHitType() << ": " << currawhit.getEventIndex() << " " << currawhit.getBarcode() << endl;
    // calculate the equivalent hit
    FTKHit hitref = currawhit.getFTKHit(m_pmap);
    if (m_Clustering ) {		
      assert(currawhit.getTruth());
      hitref.setTruth(*(currawhit.getTruth()));
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
          if (m_SaveRawHits) m_original_hits_per_plane[ireg][plane].push_back(currawhit);
          if (m_SaveHits) m_logical_hits_per_plane[ireg][plane].push_back(hitref);
        }
      }
    }
  } // end hit loop

  // fill the branches
  m_hittree->Fill();
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
	 log << MSG::ERROR << "Couldn't open file to store online-/offlinehashid" << endreq;
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
	log << MSG::ERROR << "Couldn't open file to store online-/offlinehashid" << endreq;
	 return false;
	}
      // Clear the stringstream and close file
      ss.str( string() ); 
      myfile.close();
    }

    log << MSG::INFO << "Dumped FTKTestvectors" << endreq;
    return true;
    
}
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------


StatusCode FTKRegionalWrapper::finalize()
{
  // close the output files
  m_outfile->Write();
  m_outfile->Close();
  return StatusCode::SUCCESS;
}
