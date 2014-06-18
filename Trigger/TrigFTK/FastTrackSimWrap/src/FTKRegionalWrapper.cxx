/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastTrackSimWrap/FTKRegionalWrapper.h"
#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/atlClustering.h"

FTKRegionalWrapper::FTKRegionalWrapper (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_hitInputTool("FTK_SGHitInput/FTK_SGHitInput"),
  m_IBLMode(false),
  m_pmap_path(""),
  m_pmap(0x0),
  m_rmap_path(""),
  m_rmap(0x0),
  m_ntowers(0),
  m_SaveRawHits(true),
  m_SaveHits(false),
  m_Clustering(false),
  m_SaveClusterContent(false),
  m_DiagClustering(true),
  m_SctClustering(false),
  m_PixelClusteringMode(1),
  m_DuplicateGanged(true),
  m_GangedPatternRecognition(false),
  m_outpath("ftksim_smartwrapper.root"),
  m_outfile(0x0),
  m_hittree(0x0)
{
  declareProperty("RMapPath",m_rmap_path);
  declareProperty("PMapPath",m_pmap_path);
  declareProperty("OutFileName",m_outpath);
  declareProperty("HitInputTool",m_hitInputTool);
  declareProperty("IBLMode",m_IBLMode);

  // hit type options
  declareProperty("SaveRawHits",m_SaveRawHits);
  declareProperty("SaveHits",m_SaveHits);

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
  
  // prepare a branch for each tower
  m_ntowers = m_rmap->getNumRegions();

  if (m_SaveRawHits) { // Save FTKRawHit data
    m_original_hits = new vector<FTKRawHit>[m_ntowers];
    for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
      m_hittree->Branch(Form("RawHits%d.",ireg),&m_original_hits[ireg]);
    } // end towers loop
  }

  if (m_SaveHits) {
    m_logical_hits = new vector<FTKHit>[m_ntowers];
    for (int ireg=0;ireg!=m_ntowers;++ireg) { // towers loop
      m_hittree->Branch(Form("Hits%d.",ireg),&m_logical_hits[ireg]);
    } // end towers loop
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
  }

  // TODO: test
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

    // check the region
    for (int ireg=0;ireg!=m_ntowers;++ireg) {
      if (m_rmap->isHitInRegion(hitref,ireg)) {
        // if the equivalent hit is compatible with this tower the hit is saved
        if (m_SaveRawHits) m_original_hits[ireg].push_back(currawhit);
        if (m_SaveHits) m_logical_hits[ireg].push_back(hitref);
      }
    }
  } // end hit loop

  // fill the branches
  m_hittree->Fill();

  // get the list of the truth tracks
  m_truth_tracks.clear();
  const vector<FTKTruthTrack> &truthtracks = datainput->getTruthTrack();
  m_truth_tracks.insert(m_truth_tracks.end(),truthtracks.begin(),truthtracks.end());
  // Write the tracks
  m_trackstree->Fill();

  return StatusCode::SUCCESS;
}

StatusCode FTKRegionalWrapper::finalize()
{
  // close the output files
  m_outfile->Write();
  m_outfile->Close();
  return StatusCode::SUCCESS;
}
