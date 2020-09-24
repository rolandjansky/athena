/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigInDetTrackTruthMaker.cxx
 **
 **   Description:  - Finds GenParticles which match a TrigInDetTrack,
 **                   creates a TrigInDetTrackTruth for each track and 
 **                   fills a TrigInDetTrackTruthMap with them.
 **
 **   Notes: InDetSimData is just a vector of Deposits 
 **          Deposit is a typedef of pair<HepMcParticleLink,float> 
 **          InDetSimDataCollection is a map<Identifier,InDetSimData>
 **
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan  24 19:55:56 GMT 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/

#include "TrigInDetTrackTruthMaker.h"

// tracking stuff 
#include "TrigInDetTruthEvent/TrigIDHitStats.h"

// offline inner detector truth EDM
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TruthTrajectory.h"

// detector geometry and helper classes
#include "InDetIdentifier/SCT_ID.h" 
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/TRT_ID.h"

// monte carlo edm
#include "GenzModuleEvent/KineHepMcmap.h"
#include "AtlasHepMC/GenParticle.h"

TrigInDetTrackTruthMaker::TrigInDetTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator), m_trackTruthMapKey("TrigInDetTrackTruthMap"), m_minNrMatchHits(2)
{
  
  /*
  std::string TrigInDetTrackTags[]={"HLT_TrigIDSCAN_Jet",
                                    "HLT_TrigIDSCAN_Tau", 
                                    "HLT_TrigIDSCAN_eGamma",
                                    "HLT_TrigIDSCAN_Muon", 
                                    "HLT_TrigIDSCAN_muonIso",
                                    "HLT_TrigIDSCAN_Bphysics", 
                                    "HLT_TrigIDSCAN_FullScan",
				    "HLT_TrigIDSCAN_BeamSpot",
                                    "HLT_TrigIDSCAN_Cosmics", 
                                    "HLT_TrigIDSCAN_eGamma_Brem",
                                    "HLT_TrigIDSCAN_Tile", 
                                    "HLT_TrigSiTrack_eGamma",
                                    "HLT_TrigSiTrack_Muon", 
                                    "HLT_TrigSiTrack_muonIso",
                                    "HLT_TrigSiTrack_Tau", 
                                    "HLT_TrigSiTrack_Jet",
                                    "HLT_TrigSiTrack_Bphysics", 
                                    "HLT_TrigSiTrack_FullScan",
				    // "HLT_TrigSiTrack_BeamSpot",
                                    "HLT_TrigSiTrack_Tile",
                                    "HLT_TrigSiTrack_Cosmics", 
                                    "HLT_TrigSiTrack_eGamma_robust",
                                    "HLT_TrigSiTrack_Muon_robust", 
                                    "HLT_TrigSiTrack_muonIso_robust",
                                    "HLT_TrigSiTrack_Tau_robust", 
                                    "HLT_TrigSiTrack_Jet_robust",
                                    "HLT_TrigSiTrack_Bphysics_robust", 
                                    "HLT_TrigSiTrack_FullScan_robust",
                                    "HLT_TrigSiTrack_Tile_robust", 
                                    "HLT_TRTSegmentFinder", 
                                    "HLT_TRTxK"};
  int ntag=sizeof(TrigInDetTrackTags)/sizeof(std::string);
  for (int ind=0; ind<ntag; ++ind){
    m_TrigInDetTrackCollectionKeys.push_back( TrigInDetTrackTags[ind]);
  }
  */


  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder");
  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_Muon");
  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_eGamma");
  //  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_muonIso");
  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_Tau");
  //  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_TauCore");
  //  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_TauIso");
  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_Jet");
  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_Bphysics");
  //  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_FullScan");
  //  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_BeamSpot");
  //  m_TrigInDetTrackCollectionKeys.push_back("HLT_TrigL2SiTrackFinder_Tile");

  declareProperty("TrigIDTrackTruthMapName",m_trackTruthMapKey);
  declareProperty("MinNrMatchingHits",m_minNrMatchHits);
  declareProperty("TrigInDetTrackCollectionKeys", m_TrigInDetTrackCollectionKeys);

  declareProperty("PixelSDO_MapName", m_pixel_SDO_Map = "PixelSDO_Map");
  declareProperty("SCT_SDO_MapName", m_SCT_SDO_Map = "SCT_SDO_Map");
  declareProperty("TRT_SDO_MapName", m_TRT_SDO_Map = "TRT_SDO_Map");
}

TrigInDetTrackTruthMaker::~TrigInDetTrackTruthMaker() {}


StatusCode TrigInDetTrackTruthMaker::initialize() 
{
  // print out properties
  if (msgLvl(MSG::INFO)) {
    msg() << MSG::INFO << "Initializing TrigInDetTrackTruthMaker..." << endmsg;
    msg() << MSG::INFO << "Storing truth map as "<<m_trackTruthMapKey<< endmsg;
    msg() << MSG::INFO << "PixelSDO_MapName is "<<m_pixel_SDO_Map<< endmsg;
    msg() << MSG::INFO << "SCT_SDO_MapName is "<<m_SCT_SDO_Map<< endmsg;
    msg() << MSG::INFO << "TRT_SDO_MapName is "<<m_TRT_SDO_Map<< endmsg;
    msg() << MSG::INFO << "Minimum nr.common hits = "<<m_minNrMatchHits<< endmsg;
  }

  // get pointer to Pixel ID helper
  if (detStore()->contains<PixelID>( "PixelID" )) {
    if (detStore()->retrieve( m_PIXid, "PixelID").isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve Pixel ID helper" << endmsg;
      return StatusCode::FAILURE; 
    }
  } else {
    msg() << MSG::ERROR << "Could not retrieve Pixel ID helper" << endmsg;
    return StatusCode::FAILURE; 
  }
  
  // get pointer to SCT ID helper
  if (detStore()->contains<SCT_ID>( "SCT_ID" )) {
    if (detStore()->retrieve( m_SCTid, "SCT_ID").isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve SCT ID helper" << endmsg;
      return StatusCode::FAILURE; 
    }
  } else {
    msg() << MSG::ERROR << "Could not retrieve SCT ID helper" << endmsg;
    return StatusCode::FAILURE; 
  }
  
  // get pointer to TRT ID helper
  if (detStore()->contains<TRT_ID>( "TRT_ID" )) {
    if (detStore()->retrieve( m_TRTid, "TRT_ID").isFailure()) {
      msg() << MSG::ERROR << "Could not retrieve TRT ID helper" << endmsg;
      return StatusCode::FAILURE; 
    }
  } else {
    msg() << MSG::ERROR << "Could not retrieve TRT ID helper" << endmsg;
    return StatusCode::FAILURE; 
  }
  
  if (msgLvl(MSG::INFO)) msg() << MSG::INFO << "TrigInDetTrackTruthMaker initialization done" << endmsg;
  return StatusCode::SUCCESS; 
}


StatusCode TrigInDetTrackTruthMaker::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode TrigInDetTrackTruthMaker::execute() {
  // declare local StatusCode flag
  // StatusCode sc;  // Causes a FATAL unchecked StatusCode if execute() returns early.

  // safeguard to avoid crashes when running from POOL
//   if ( !( m_havePIXmgr || m_haveSCTmgr || m_haveTRTmgr ) ) {
//     if (msgLvl(MSG::ERROR)) msg() << MSG::ERROR << "Detector managers not available!" << endmsg;
//     return StatusCode::SUCCESS; 
//   }

  // create truth-association maps for TrigInDetTracks and record to StoreGate
  if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "Recording truth map to StoreGate with key " 
             << m_trackTruthMapKey << endmsg;

  if (evtStore()->contains<TrigInDetTrackTruthMap>( m_trackTruthMapKey )) {
    if (msgLvl(MSG::WARNING)) msg() << MSG::WARNING << m_trackTruthMapKey << " already exists: stop!" 
           << endmsg;
    return StatusCode::SUCCESS;
  }
  
  TrigInDetTrackTruthMap* trk_truth_map = new TrigInDetTrackTruthMap();
  // declare StatusCode here instead so it is always checked
  StatusCode sc = evtStore()->record(trk_truth_map, m_trackTruthMapKey);
  if (sc.isFailure()) {
    if (msgLvl(MSG::ERROR)) msg() << MSG::ERROR << "Failed recording truth map to StoreGate!" << endmsg;
    delete trk_truth_map;
    return StatusCode::SUCCESS;
  }
      
  // get truth association maps and check some were found
  bool haveTruthMaps = this->GetTruthMaps();
  if ( !haveTruthMaps ) {
    if (msgLvl(MSG::WARNING)) msg() << MSG::WARNING << "No truth maps found in StoreGate!" << endmsg;
    return StatusCode::SUCCESS;
  } else {
    std::vector<std::string>::iterator firstKey = m_TrigInDetTrackCollectionKeys.begin();
    std::vector<std::string>::iterator lastKey = m_TrigInDetTrackCollectionKeys.end();

    for ( ; firstKey != lastKey; ++firstKey) {
      const TrigInDetTrackCollection* trkColl;
      
      if (evtStore()->contains<TrigInDetTrackCollection>(*firstKey)) {
        StatusCode sc = evtStore()->retrieve(trkColl, *firstKey);
        if (sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "No TrigInDetTrackCollection found with key "
                    << *firstKey << endmsg;
          continue;
        }
      } else {
        if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "No TrigInDetTrackCollection found with key "
                    << *firstKey << endmsg;
        continue;
      }
      if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "TrigInDetTrackCollection retrieved with key "
                    << *firstKey << endmsg;

      // tracks      
      TrigInDetTrackCollection::const_iterator trkIter = trkColl->begin();
      TrigInDetTrackCollection::const_iterator lastTrk = trkColl->end();

      for (unsigned int iTrk=0; trkIter !=lastTrk; ++trkIter, ++iTrk) {

        if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "Doing TrigInDetTrack " << iTrk << endmsg;

        // check if track has hit information
        if ((*trkIter)->siSpacePoints() || (*trkIter)->trtDriftCircles()) {
          
          // find the truth information using class private method and fill map
          TrigInDetTrackTruth trk_truth;
          if ( this->TrackTruth(*trkIter, &trk_truth) ) {
            
            // check against null pointers and put in map
            if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "Filling truth match family relations" << endmsg;
            int nr_relations = trk_truth.updateFamilyTree();
            if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << nr_relations << " family relations found" << endmsg;
            
            if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "Adding truth match to map" << endmsg;
            
            // add truth-association object to map
            trk_truth_map->addMatch(trkColl,iTrk,trk_truth);
            
          } else {
            if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "No truth match found" << endmsg;
          }
        } else {
          if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "TrigInDetTrack " << iTrk 
               << " in collection with key " << *firstKey << " has no hit information" << endmsg;
        } // any hits in track?
      }   // loop over tracks
    }     // loop over track collections
  }       // retrieved tracks from storegate

  // debug  
//   if(m_outputLevel <= MSG::DEBUG) {
//     trk_truth_map->print();
//   }

  // lock truth map
  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "Locking TrigInDetTrackTruthMap with key " 
               << m_trackTruthMapKey << endmsg;
  
  sc = evtStore()->setConst(trk_truth_map);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg() << MSG::WARNING << "Could not lock TrigInDetTrackTruthMap!" << endmsg;
  } else if (evtStore()->contains<TrigInDetTrackTruthMap>( m_trackTruthMapKey )) {
    if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "TrigInDetTrackTruthMap \"" 
               << m_trackTruthMapKey << "\" in SG; done!" << endmsg;
    return StatusCode::SUCCESS;
  }
  return  StatusCode::SUCCESS;
}



bool TrigInDetTrackTruthMaker::TrackTruth(const TrigInDetTrack* p_trk, TrigInDetTrackTruth* p_truth) const
{
  bool success = false;

  // make a local map of GenParticles and number of common hits, to fill 
  // the TrigInDetTrackTruth object after going trough all detectors in ID
  std::map<HepMcParticleLink, unsigned int> true_hits_map_all;
  std::map<HepMcParticleLink, unsigned int> true_hits_map_pix;
  std::map<HepMcParticleLink, unsigned int> true_hits_map_sct;
  std::map<HepMcParticleLink, unsigned int> true_hits_map_trt;
  
  // get TrigSiSpacePoints
  const std::vector <const TrigSiSpacePoint*>* p_sp_vec = p_trk->siSpacePoints();
  
  // check TrigSiSpacePoints are there and if so carry on
  if (p_sp_vec) {
    
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "TrackTruth() : TrigInDetTrack has " 
           << p_sp_vec->size() << " SiSpacePoints" << endmsg;

    std::vector<const TrigSiSpacePoint*>::const_iterator spIter = p_sp_vec->begin();
    std::vector<const TrigSiSpacePoint*>::const_iterator lastSP = p_sp_vec->end();
    
    // make local vector to contain GenParticles corresponding to space point
    std::vector<HepMcParticleLink>* p_gp_vec = new std::vector<HepMcParticleLink>;
    
    // loop over SPs and collect all the kine that contributed to each
    for (int iSP=0; spIter != lastSP; ++iSP, ++spIter) {

      // check TrigSiSpacePoint is in PIXEL detector
      if ( m_PIXid->is_pixel((*spIter)->identify()) ) {
        if (msgLvl(MSG::VERBOSE)) {
          msg() << MSG::VERBOSE << "TrackTruth() : Pixel Space Point " << iSP 
              << " R: " << (*spIter)->r() << " phi: " << (*spIter)->phi() 
              << " z: " << (*spIter)->z() << endmsg;
          msg() << MSG::VERBOSE << "TrackTruth() : SP-id: "
              << m_PIXid->print_to_string((*spIter)->identify()) << endmsg;
        }
        
        // get all the GenParticles that contributed to SP
        this->PIXspTruth(*spIter, p_gp_vec);  
        
        // update maps of <GenParticle*,number of hits> 
        this->updatePLmap(p_gp_vec, true_hits_map_all);
        this->updatePLmap(p_gp_vec, true_hits_map_pix);
      } else if ( m_SCTid->is_sct((*spIter)->identify()) ) {
        
        // do the same thing for the SCT
        if (msgLvl(MSG::VERBOSE)) {
          msg() << MSG::VERBOSE << "TrackTruth() : SCT Space Point " << iSP 
              << " R: " << (*spIter)->r() << " phi: " << (*spIter)->phi() 
              << " z: " << (*spIter)->z() << endmsg;
          msg() << MSG::VERBOSE 
              << "TrackTruth() : SP-id: "
              << m_SCTid->print_to_string((*spIter)->identify())
              << endmsg;
        }
        // get all the GenParticles that contributed to SP
        this->SCTspTruth(*spIter, p_gp_vec);
        
        // update maps of <GenParticle*,number of hits> 
        this->updatePLmap(p_gp_vec, true_hits_map_all);
        this->updatePLmap(p_gp_vec, true_hits_map_sct);
      }
      
      // clear the vector ready for the next space point
      p_gp_vec->clear();
    }
    
    // don't need the vector anymore, better delete it
    delete p_gp_vec;
  } // ! if (siSpacePoints exist)

  // get TRT_DriftCircles
  const std::vector<const InDet::TRT_DriftCircle*>* p_dc_vec = p_trk->trtDriftCircles();
  
  // check TRT_DriftCircles are there and if so carry on
  if (p_dc_vec) {
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "TrackTruth() : TrigInDetTrack has " 
           << p_dc_vec->size() << " TRT_DriftCircles" << endmsg;
    
    std::vector<const InDet::TRT_DriftCircle*>::const_iterator dcIter = p_dc_vec->begin();
    std::vector<const InDet::TRT_DriftCircle*>::const_iterator lastSP = p_dc_vec->end();
    
    // make vector to contain GenParticles corresponding to drift circle
    std::vector<HepMcParticleLink>* p_gp_vec = new std::vector<HepMcParticleLink>;
    
    // loop over DCs and collect all the kine that contributed to each
    for (int iDC=0; dcIter != lastSP; ++iDC, ++dcIter) {
      
      // check InDet::TRT_DriftCircle 
      if ( m_TRTid->is_trt((*dcIter)->identify()) ) {
      
      if (msgLvl(MSG::VERBOSE)) {
        msg() << MSG::VERBOSE << "TrackTruth() : TRT Drift Circle " << iDC << endmsg;
        msg() << MSG::VERBOSE << "TrackTruth() : DC-id: "
            << m_TRTid->print_to_string((*dcIter)->identify()) << endmsg;
      }
        // get all the GenParticles that contributed to SP
      this->TRTdcTruth(*dcIter, p_gp_vec);    
      
      // update maps of <GenParticle*,number of hits> 
      this->updatePLmap(p_gp_vec, true_hits_map_all);
      this->updatePLmap(p_gp_vec, true_hits_map_trt);
      }
      
      // clear the vector ready for the next drift circle
      p_gp_vec->clear();
    }
    
    // don't need the vector anymore, better delete it
    delete p_gp_vec;
  } // if (siSpacePoints exist)
  
  // finally, fill the TrigInDetTrackTruth object using the local map
  std::map<HepMcParticleLink, unsigned int>::iterator mapIt  = true_hits_map_all.begin();    
  std::map<HepMcParticleLink, unsigned int>::iterator mapEnd = true_hits_map_all.end(); 
  
  for (int itrk=0; mapIt != mapEnd; ++mapIt, ++itrk) {
    
    // look for this GenParticle in each map and if found, count hits
    std::map<HepMcParticleLink, unsigned int>::iterator pixIt(true_hits_map_pix.find((*mapIt).first));
    unsigned int pix_hits = 0;
    if (pixIt != true_hits_map_pix.end()) {
          pix_hits = (*pixIt).second;
    }
    std::map<HepMcParticleLink, unsigned int>::iterator sctIt(true_hits_map_sct.find((*mapIt).first));
    unsigned int sct_hits = 0;
    if (sctIt != true_hits_map_sct.end()) {
          sct_hits = (*sctIt).second;
    }
    std::map<HepMcParticleLink, unsigned int>::iterator trtIt(true_hits_map_trt.find((*mapIt).first));
    unsigned int trt_hits = 0;
    if (trtIt != true_hits_map_trt.end()) {
          trt_hits = (*trtIt).second;
    }
    
    // add matching GenParticle (and hits) to truth association object 
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "TrackTruth(): add match to track " << itrk
           << ": hits=" << pix_hits << "(pix), " << sct_hits << "(sct), "
           << trt_hits << "(trt)"<<endmsg;
    
    // sanity check
    if ( (*mapIt).second != (pix_hits+sct_hits+trt_hits) ) {
      msg() << MSG::WARNING << "TrackTruth(): ERROR counting hits" << endmsg;
    }
    
    // if at least one match was found: update success flag
    if ( (*mapIt).second > 0 ) success = true;
    
    TrigIDHitStats t_hits;
    t_hits[TrigIDHitStats::PIX] = pix_hits;
    t_hits[TrigIDHitStats::SCT] = sct_hits;
    t_hits[TrigIDHitStats::TRT] = trt_hits;
    
    // only add match if at least 2 hits? just leave it like this for now...
    if (t_hits.total() > 2) {   
      unsigned int indx = p_truth->addMatch( (*mapIt).first, t_hits);
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE <<
        "TrackTruth(): matching particle has match index " << indx << endmsg;
    }
  }
  return success;
}



void TrigInDetTrackTruthMaker::updatePLmap(std::vector<HepMcParticleLink>* p_vec, std::map<HepMcParticleLink, unsigned int>& hits_map) const
{
  // find if each GenParticle is already in map and in that 
  // case increment nr of common hits; if not then put in map
  std::vector<HepMcParticleLink>::iterator gpIter = p_vec->begin();
  std::vector<HepMcParticleLink>::iterator lastGP = p_vec->end();
  
  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "updatePLmap() : looping over GenParticles" << endmsg;
  
  for (int igp=0; gpIter != lastGP; ++gpIter, ++igp) {
    if(hits_map.count(*gpIter) == 0) {
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "updatePLmap() : adding GenParticle " << igp 
             << " to local map for insertion in track truth" <<endmsg;
      hits_map[(*gpIter)] = 1;
    } else {
      hits_map[(*gpIter)] = hits_map[(*gpIter)]+1;
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "updatePLmap() : GenParticle " << igp 
             << " already in local map : " << hits_map[(*gpIter)]  << " common hits so far" << endmsg;
    }
  }
  return;
}



void TrigInDetTrackTruthMaker::SCTspTruth(const TrigSiSpacePoint* p_SP, std::vector<HepMcParticleLink>* p_GP) const
{
  // check if the SCT_SDO_Map has been found
  if (!m_haveSCTmap) return;
  
  // check against NULL
  if ( !(p_SP->clusters().first) || !(p_SP->clusters().second) ) return;
  
  // check both clusters in TrigSiSpacePoint 
  for (int iClusInSP=0; iClusInSP<2; iClusInSP++){
    
    const InDet::SiCluster* pSCTclus;
    if (iClusInSP == 1) {
      pSCTclus = p_SP->clusters().first;
    } else {
      pSCTclus = p_SP->clusters().second;
    }
    
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "SCTspTruth() : cluster-id: " 
           << m_SCTid->print_to_string((*pSCTclus).identify()) << endmsg;
    
    // get list of Raw Data Objects identifiers in cluster
    const std::vector<Identifier> clusRDOids = pSCTclus->rdoList();
    
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "SCTspTruth() : cluster " << iClusInSP << " has " 
           << clusRDOids.size() << " RDO identifiers" << endmsg; 
    
    std::vector<Identifier>::const_iterator rdoIter=clusRDOids.begin();
    std::vector<Identifier>::const_iterator lastRDO=clusRDOids.end();
    
    // loop over RDO identifiers and get collection of InDetSimData
    for (int iRDO=0; rdoIter != lastRDO; ++rdoIter, ++iRDO) {
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "SCTspTruth() : Doing RDO nr " << iRDO << endmsg; 
      
      // find InDetSimData object corresponding to this RDO from the
      // SCT InDetSimDataCollection map; a InDetSimData is basically a 
      // vector of deposits (typedef of pair<HepMcParticleLink,float>) 
      // a InDetSimDataCollection is a map<Identifier,InDetSimData>
      InDetSimDataCollection::const_iterator clusSimData = m_id2SimDataMapSCT->find(*rdoIter);
      
      // check if the InDetSimData corresponding to the RDO was found
      if (clusSimData == m_id2SimDataMapSCT->end()) {
        if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "SCTspTruth() : Cannot find simulation info for "
            <<  m_SCTid->print_to_string(*rdoIter) << endmsg;
        continue;
      } else {
        // add all GenParticles which contributed to this cluster into vector
        this->updatePLvector(p_GP,clusSimData);
      }
    }
  }
  return;
}


void TrigInDetTrackTruthMaker::PIXspTruth(const TrigSiSpacePoint* p_SP, std::vector<HepMcParticleLink>* p_GP) const
{
  // check if the PixelSDO_Map has been found
  if (!m_havePIXmap) return;
  
  const InDet::SiCluster* pPIXclus = p_SP->clusters().first;

  // check against NULL  
  if (!pPIXclus) return;
    
  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "PIXspTruth() : cluster-id: " 
               << m_PIXid->print_to_string((*pPIXclus).identify()) << endmsg;
    
  // get list of Raw Data Objects identifiers in cluster
  const std::vector<Identifier> clusRDOids = pPIXclus->rdoList(); 
    
  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "PIXspTruth() : cluster has " 
               << clusRDOids.size() << " RDO identifiers" << endmsg; 
    
  std::vector<Identifier>::const_iterator rdoIter = clusRDOids.begin();
  std::vector<Identifier>::const_iterator lastRDO = clusRDOids.end();
    
  // loop over RDO identifiers and get collection of InDetSimData
  for (int iRDO=0; rdoIter != lastRDO; ++rdoIter, ++iRDO) {
      
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "PIXspTruth() : Doing RDO nr " << iRDO << endmsg; 
    
    if ( ! (*rdoIter).is_valid() ) {
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::WARNING <<
        "Invalid identifier from pixel SP->rdoList() !!" << endmsg;
      continue;
    } else {
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "Pixel identifier: "
             << m_PIXid->print_to_string(*rdoIter) << endmsg;
    }

    // find InDetSimData object corresponding to this RDO from the
    // PIXel InDetSimDataCollection map; a InDetSimData is basically a 
    // vector of deposits (typedef of pair<HepMcParticleLink,float>) 
    // a InDetSimDataCollection is a map<Identifier,InDetSimData>
    InDetSimDataCollection::const_iterator clusSimData = m_id2SimDataMapPIX->find(*rdoIter);
      
    if (clusSimData == m_id2SimDataMapPIX->end()) {
      // we didn't find the ID in the map - check if it is a ganged pixel
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::DEBUG << "PIXspTruth() : Cannot find simulation info for " 
             << m_PIXid->print_to_string(*rdoIter) <<" : looking for ganged pixel" << endmsg;
      
      const unsigned int phi_index = m_PIXid->phi_index(*rdoIter);
      unsigned int ganged_phi_index=0;
      
      switch (phi_index) {
        case 153: ganged_phi_index = 160; break;
        case 155: ganged_phi_index = 161; break;
        case 157: ganged_phi_index = 162; break;
        case 159: ganged_phi_index = 163; break;
        case 168: ganged_phi_index = 164; break;
        case 170: ganged_phi_index = 165; break;
        case 172: ganged_phi_index = 166; break;
        case 174: ganged_phi_index = 167; break;
        default: ganged_phi_index =0;
      }

      if ( ganged_phi_index != 0) {
        // it is a ganged pixel : get the ID of the partner.
        const Identifier new_rdoID = m_PIXid->pixel_id(m_PIXid->barrel_ec(*rdoIter),
                   m_PIXid->layer_disk(*rdoIter),
                   m_PIXid->phi_module(*rdoIter),
                   m_PIXid->eta_module(*rdoIter),
                   ganged_phi_index, 
                   m_PIXid->eta_index(*rdoIter));
    
        if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "PIXspTruth() : ganged to pixel " 
            <<  m_PIXid->print_to_string(new_rdoID) << endmsg;

        clusSimData = m_id2SimDataMapPIX->find(new_rdoID);
      }
      if(clusSimData == m_id2SimDataMapPIX->end() )  {
        // check again, in case ganged pixel is also not in map
        if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG << "SCTspTruth() : Cannot find simulation info for " 
            <<  m_PIXid->print_to_string(*rdoIter) << endmsg;
        continue;
      }
    }
    
    // add all GenParticles which contributed to this cluster into vector
    this->updatePLvector(p_GP,clusSimData);
  }
  return;
}


void TrigInDetTrackTruthMaker::TRTdcTruth(const InDet::TRT_DriftCircle* p_DC, std::vector<HepMcParticleLink>* p_GP) const
{
  // check if the TRT_SDO_Map has been found
  if (!m_haveTRTmap) return;
  
  // check against NULL
  if (!p_DC) return;
  
  // get list of Raw Data Objects identifiers in Drift Circle
  const std::vector<Identifier>& dcircRDOids = p_DC->rdoList();
  
  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "TRTdcTruth() : drift circle has " 
               << dcircRDOids.size() << " RDO identifiers" << endmsg; 
    
  std::vector<Identifier>::const_iterator rdoIter = dcircRDOids.begin();
  std::vector<Identifier>::const_iterator lastRDO = dcircRDOids.end();

  // loop over RDO identifiers and get collection of InDetSimData
  for (int iRDO=0; rdoIter != lastRDO; ++rdoIter, ++iRDO) {
      
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "TRTdcTruth() : Doing RDO nr " << iRDO << endmsg; 
    
    if ( !(*rdoIter).is_valid() ) {
      if (msgLvl(MSG::WARNING)) msg() << MSG::WARNING << "Invalid identifier from TRT DC->rdoList() !!" 
             << endmsg;
      continue;
    } else {
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "TRT identifier: "
             << m_TRTid->print_to_string(*rdoIter) << endmsg;
    }
    
    // find InDetSimData object corresponding to this RDO from the
    // TRT InDetSimDataCollection map; a InDetSimData is basically a 
    // vector of deposits (typedef of pair<HepMcParticleLink,float>) 
    // a InDetSimDataCollection is a map<Identifier,InDetSimData>
    InDetSimDataCollection::const_iterator dcircSimData = m_id2SimDataMapTRT->find(*rdoIter); 
    
    if (dcircSimData == m_id2SimDataMapTRT->end())  {
      
      // we didn't find the ID in the map
      if (msgLvl(MSG::VERBOSE)) msg() << MSG::DEBUG << "TRTdcTruth() : Cannot find simulation info for " 
             << m_TRTid->print_to_string(*rdoIter) << endmsg;
      continue; 
    }
    
    // add all GenParticles which contributed to this cluster into vector
    this->updatePLvector(p_GP,dcircSimData);
  }
  return;
}



void TrigInDetTrackTruthMaker::updatePLvector(std::vector<HepMcParticleLink>* p_GP, InDetSimDataCollection::const_iterator& simData) const
{
  // get the vector of deposits for each GenParticle from the InDetSimData 
  const std::vector< std::pair<HepMcParticleLink,float> >& simDeposits = ((*simData).second).getdeposits();
  
  if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "updatePLvector() : RDO has " 
               << simDeposits.size() << " deposits" << endmsg; 
  
  // loop over the deposits and get info from HepMcParticleLinks
  std::vector< std::pair<HepMcParticleLink,float> >::const_iterator depIter = simDeposits.begin();
  std::vector< std::pair<HepMcParticleLink,float> >::const_iterator lastDep = simDeposits.end();
  
  for (int iDep=0 ; depIter != lastDep; ++depIter, ++iDep) {
      
    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "updatePLvector() : Doing deposit " 
           << iDep << endmsg; 
      
    // get the HepMcParticleLink from the Deposit particle link and check it's ok
    HepMcParticleLink partLink =  (*depIter).first;

    if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "updatePLvector() : Deposit " << iDep 
           << ": kine " << partLink.barcode() 
           << ", event index " << partLink.eventIndex() 
           << ", energy deposit " << (*depIter).second << endmsg; 
    
    if (partLink.isValid()) {
        
      // add the GenParticle to vector if it's not there yet
      std::vector<HepMcParticleLink>::iterator gpIter = p_GP->begin();
      std::vector<HepMcParticleLink>::iterator lastGP = p_GP->end();
      bool putGPinVec = true;
      for (; gpIter != lastGP; ++gpIter) {
        if (partLink == (*gpIter)) putGPinVec = false;
      }
      if (putGPinVec) {
        p_GP->push_back(partLink);
        if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE 
               << "updatePLvector() : Added particle to vector: " 
               << p_GP->size() << " matches so far" << endmsg;
      }
    }
  }
  return;
}



bool TrigInDetTrackTruthMaker::GetTruthMaps() 
 {
   // StatusCode sc;

   if (evtStore()->contains<InDetSimDataCollection>(m_pixel_SDO_Map)) {
     // retrieve Pixel SDO map for this event
     StatusCode sc = evtStore()->retrieve(m_id2SimDataMapPIX, m_pixel_SDO_Map);
     if (sc.isFailure()) {
        if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG
                 << "GetTruthMaps() : Could not retrieve " << m_pixel_SDO_Map << "!" << endmsg;
        m_havePIXmap=false;
     } else {
       if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "GetTruthMaps() : Retrieved "
                 << m_pixel_SDO_Map << endmsg;
       m_havePIXmap=true;
     }
   } else {
     if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG
                 << "GetTruthMaps() : Could not retrieve " << m_pixel_SDO_Map << "!" << endmsg;
     m_havePIXmap=false;
   }
  
   if (evtStore()->contains<InDetSimDataCollection>(m_SCT_SDO_Map)) {
     // retrieve SCT SDO map for this event
     StatusCode sc = evtStore()->retrieve(m_id2SimDataMapSCT, m_SCT_SDO_Map);
     if (sc.isFailure()) {
       if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG
                 << "GetTruthMaps() : Could not retrieve " << m_SCT_SDO_Map << "!" << endmsg;
       m_haveSCTmap=false;
     } else {
       if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE
                 << "GetTruthMaps() : Retrieved " << m_SCT_SDO_Map << endmsg;
       m_haveSCTmap=true;
     }
   } else {
     if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG
                 << "GetTruthMaps() : Could not retrieve " << m_SCT_SDO_Map << "!" << endmsg;
     m_haveSCTmap=false;
   }
   
   if (evtStore()->contains<InDetSimDataCollection>(m_TRT_SDO_Map)) {
     // retrieve TRT SDO map for this event
     StatusCode sc = evtStore()->retrieve(m_id2SimDataMapTRT, m_TRT_SDO_Map);
     if (sc.isFailure()) {
       if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG
                 << "GetTruthMaps() : Could not retrieve " << m_TRT_SDO_Map << "!" << endmsg;
       m_haveTRTmap=false;
     } else {
       if (msgLvl(MSG::VERBOSE)) msg() << MSG::VERBOSE << "GetTruthMaps() : Retrieved "
                 << m_TRT_SDO_Map << endmsg;
       m_haveTRTmap=true;
     }
   } else {
     if (msgLvl(MSG::DEBUG)) msg() << MSG::DEBUG
                 << "GetTruthMaps() : Could not retrieve " << m_TRT_SDO_Map << "!" << endmsg;
     m_haveTRTmap=false;
   }
   
   return (m_havePIXmap || m_haveSCTmap || m_haveTRTmap);
   //return true;
 }



