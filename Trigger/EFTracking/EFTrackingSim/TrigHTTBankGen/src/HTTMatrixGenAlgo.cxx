// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTMatrixGenAlgo.cxx
 * @author Rewrite by Riley Xu - riley.xu@cern.ch after FTK code
 * @date May 8th, 2020
 * @brief See HTTMatrixGenAlgo.h
 */


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "HTTMatrixGenAlgo.h"
#include "HTTMatrixAccumulator.h"
#include "TrigHTTConfTools/HTTRegionSlices.h"
#include "TrigHTTObjects/HTTConstants.h"

#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"

#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <utility>


///////////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////////

HTTMatrixGenAlgo::HTTMatrixGenAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}



///////////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTMatrixGenAlgo::initialize()
{
  ATH_MSG_DEBUG("initialize()");

  // set the slicing variables from inputs
  m_sliceMax.qOverPt = m_temp_c_max;
  m_sliceMax.phi = m_temp_phi_max;
  m_sliceMax.eta = m_temp_eta_max;
  m_sliceMax.d0 = m_temp_d0_max;
  m_sliceMax.z0 = m_temp_z0_max;

  m_sliceMin.qOverPt = m_temp_c_min;
  m_sliceMin.phi = m_temp_phi_min;
  m_sliceMin.eta = m_temp_eta_min;
  m_sliceMin.d0 = m_temp_d0_min;
  m_sliceMin.z0 = m_temp_z0_min;

  m_nBins.qOverPt = m_temp_c_slices;
  m_nBins.phi = m_temp_phi_slices;
  m_nBins.eta = m_temp_eta_slices;
  m_nBins.d0 = m_temp_d0_slices;
  m_nBins.z0 = m_temp_z0_slices;

  // Retrieve handles
  ATH_CHECK(m_tHistSvc.retrieve());
  ATH_CHECK(m_HTTMapping.retrieve());
  ATH_CHECK(m_hitInputTool.retrieve());
  ATH_CHECK(m_hitMapTool.retrieve());
  ATH_CHECK(m_EvtSel.retrieve());
  ATH_CHECK(m_roadFinderTool.retrieve());
  if (m_doClustering) ATH_CHECK(m_clusteringTool.retrieve());

  if (m_doHoughConstants) {
    if (m_ideal_geom == 0) {
      ATH_MSG_INFO("Hough constants method needs idealized geometry > 0, aborting.");
      return StatusCode::FAILURE;
    }
    m_pmap = m_HTTMapping->PlaneMap_1st();
    // Get detector configurations
    m_nLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();
    m_nRegions = m_HTTMapping->RegionMap_1st()->getNRegions();
    m_nDim = m_HTTMapping->PlaneMap_1st()->getNCoords();

  }
  else {
    m_pmap = m_HTTMapping->PlaneMap_2nd();
    // Get detector configurations
    m_nLayers = m_HTTMapping->PlaneMap_2nd()->getNLogiLayers();
    m_nRegions = m_HTTMapping->RegionMap_2nd()->getNRegions();
    m_nDim = m_HTTMapping->PlaneMap_2nd()->getNCoords();
  }

  m_nDim2 = m_nDim * m_nDim;
  m_sector_cum.resize(m_nRegions);

  // Retrieve slice information
  m_sliceMin = m_EvtSel->getMin();
  m_sliceMax = m_EvtSel->getMax();

  // Histograms
  ATH_CHECK(bookHistograms());

  m_eventHeader            = new HTTEventInputHeader();

  return StatusCode::SUCCESS;
}


StatusCode HTTMatrixGenAlgo::bookHistograms()
{
  // Training tracks
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++) {
    std::string name = "h_trainingTrack_" + HTTTrackPars::parName(i);
    std::string title = HTTTrackPars::parName(i) + " of tracks passing pt/barcode check";
    
    m_h_trainingTrack[i] = new TH1I(name.c_str(), title.c_str(), 100, m_sliceMin[i], m_sliceMax[i]);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/" + name, m_h_trainingTrack[i]));
  }

  // Sector pars
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++) {
    
    std::string name = "h_sectorPars_" + HTTTrackPars::parName(i);
    std::string title = "Average " + HTTTrackPars::parName(i) + " in sector";
    
    m_h_sectorPars[i] = new TH1I(name.c_str(), title.c_str(), 100, m_sliceMin[i], m_sliceMax[i]);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/" + name, m_h_sectorPars[i]));
  }

  // Select hit failure
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++) {
    
    std::string name = "h_SHfailure_" + HTTTrackPars::parName(i);
    std::string title = HTTTrackPars::parName(i) + " of tracks failing in selectHit()";
    
    m_h_SHfailure[i] = new TH1I(name.c_str(), title.c_str(), 100, m_sliceMin[i], m_sliceMax[i]);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/" + name, m_h_SHfailure[i]));
  }

  // 3 hits in layer
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++) {

    std::string name = "h_3hitsInLayer_" + HTTTrackPars::parName(i);
    std::string title = HTTTrackPars::parName(i) + " of tracks containing 3+ hits in a single layer";
    
    m_h_3hitsInLayer[i] = new TH1I(name.c_str(), title.c_str(), 100, m_sliceMin[i], m_sliceMax[i]);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/" + name, m_h_3hitsInLayer[i]));
  }

  // Not enough hits
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++) {
    std::string name = "h_notEnoughHits_" + HTTTrackPars::parName(i);
    std::string title = HTTTrackPars::parName(i) + " of tracks failing because it didn't have enough hits";
    
    m_h_notEnoughHits[i] = new TH1I(name.c_str(), title.c_str(), 100, m_sliceMin[i], m_sliceMax[i]);
    ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/" + name, m_h_notEnoughHits[i]));
  }
  
  m_h_trackQoP_okHits = new TH1I("h_trackQoP_okHits", "half inverse pt of tracks passing hit check",
			       m_nBins.qOverPt, m_sliceMin.qOverPt, m_sliceMax.qOverPt);
  m_h_trackQoP_okRegion = new TH1I("h_trackQoP_okRegion", "half inverse pt of tracks passing region check",
				 m_nBins.qOverPt, m_sliceMin.qOverPt, m_sliceMax.qOverPt);
  m_h_nHit = new TH1I("h_nHit", "number of hits in sector", 100, 0, 100);

  ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_trackQoP_okHits", m_h_trackQoP_okHits));
  ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_trackQoP_okRegion", m_h_trackQoP_okRegion));
  ATH_CHECK(m_tHistSvc->regHist("/TRIGHTTMATRIXOUT/h_nHit",m_h_nHit));

  return StatusCode::SUCCESS;
}

void fillTrackPars(TH1I* const hists[HTTTrackPars::NPARS], HTTTruthTrack const & track)
{
  HTTTrackPars pars = track.getPars();
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
    hists[i]->Fill(pars[i]);
}

///////////////////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTMatrixGenAlgo::execute()
{
  ATH_MSG_DEBUG("execute()");
  m_eventHeader->clearHits();
  m_eventHeader->reset();

  // Get hits and training tracks from this event
  bool last=false;
  ATH_CHECK(m_hitInputTool->readData(m_eventHeader, last));
  if (last) return StatusCode::SUCCESS;
    
  std::vector<HTTHit> hits = getLogicalHits();

  std::vector<HTTTruthTrack> truth_tracks = m_eventHeader->optional().getTruthTracks();
  std::vector<HTTTruthTrack> tracks = filterTrainingTracks(truth_tracks);

  m_nTracks += truth_tracks.size();
  if (tracks.empty()) {  
    ATH_MSG_DEBUG("Empty training tracks");
    return StatusCode::SUCCESS;
  }

  // Prepare a map of the hits according the barcode
  std::map<int, std::vector<HTTHit>> barcode_hits = makeBarcodeMap(hits, tracks);

  // For each training track, find the sector it belongs to and accumulate the
  // hit coordinates and track parameters.
  for (HTTTruthTrack const & track : tracks) {
    // Get list of hits associated to the current truth track
    std::vector<HTTHit> & track_hits = barcode_hits[track.getBarcode()];
    
    // Get the hits that will form the actual sector
    std::vector<HTTHit> sector_hits;
    bool success = filterSectorHits(track_hits, sector_hits, track);
    if (!success) continue; // Skip this track if it has bad hits (not complete, etc.)
    m_h_trackQoP_okHits->Fill(track.getQOverPt());
    
    // Get the region of this sector
    int region = getRegion(sector_hits);
    if (region < 0 || region >= m_nRegions) continue;
    m_h_trackQoP_okRegion->Fill(track.getQOverPt());
    
    //For the Hough constants, find the Hough roads
    std::vector<HTTRoad_Hough*> houghRoads;
    if (m_doHoughConstants){
      
      std::vector<const HTTHit*> phits;
      std::vector<HTTRoad*> roads;
      
      for (const HTTHit& hit : sector_hits) phits.push_back(&hit);
      
      StatusCode sc = m_roadFinderTool->getRoads(phits, roads);
      if (sc.isFailure()) ATH_MSG_WARNING("Hough Transform -> getRoads() failed");
      
      if (!roads.empty()){
	for (HTTRoad* r : roads){
	  HTTRoad_Hough* hr = dynamic_cast<HTTRoad_Hough*>(r);
	  houghRoads.push_back(hr);
	}
      }
      
      if (!houghRoads.empty()){
	double y = 0.0;
	double x = 0.0;
	
	//For each Hough road, make the accumulator
	for (HTTRoad_Hough* hr : houghRoads){
	  y = hr->getY();
	  x = hr->getX();
	  
	  // Prepare the accumulator struct
	  std::vector<module_t> modules(m_nLayers);
	  HTTMatrixAccumulator acc(m_nLayers, m_nDim);
	  acc.pars.qOverPt = y;
	  acc.pars.phi = x;
	  std::pair<std::vector<module_t>, HTTMatrixAccumulator> modules_acc = {modules, acc};
	  ATH_CHECK(makeAccumulator(sector_hits, track, modules_acc));
	  
	  // Add the track to the accumulate map
	  accumulate(m_sector_cum[region], modules_acc.first, modules_acc.second);
	  m_nTracksUsed++;
	}
      }
      else{
	ATH_MSG_DEBUG("execute(): no hough roads?");	
	return StatusCode::SUCCESS;
      }
    }
    else{
      // Prepare the accumulator struct
      std::vector<module_t> modules(m_nLayers);
      HTTMatrixAccumulator acc(m_nLayers, m_nDim);
      std::pair<std::vector<module_t>, HTTMatrixAccumulator> modules_acc = {modules, acc};
      ATH_CHECK(makeAccumulator(sector_hits, track, modules_acc));
      
      // Add the track to the accumulate map
      accumulate(m_sector_cum[region], modules_acc.first, modules_acc.second);
      m_nTracksUsed++;
    }
  }
  
  return StatusCode::SUCCESS;
}


// Converts raw hits from header into logical hits, and filters those in HTT layers
// Could replace this with the RawToLogical tool
std::vector<HTTHit> HTTMatrixGenAlgo::getLogicalHits() 
{
  std::vector<HTTHit> hits;
  //Setup the logical header...
  HTTLogicalEventInputHeader logicalHeader;
  //Map the hits to the logical header...
  unsigned stage = 0;
  if (m_doHoughConstants) stage = 1; // For now Hough constants only works on 1st stage
  else stage = 2;
  StatusCode sc = m_hitMapTool->convert(stage, *m_eventHeader, logicalHeader);
  if (sc.isFailure()) ATH_MSG_ERROR("Hit mapping failed");

  if (m_doClustering) {
    std::vector<HTTCluster> clustered_hits;
    sc = m_clusteringTool->DoClustering(logicalHeader, clustered_hits);
    if (sc.isFailure()) ATH_MSG_ERROR("Clustering failed");
    for (HTTCluster const & cluster : clustered_hits) {
      HTTHit clusterEquiv = cluster.getClusterEquiv();
      hits.push_back(clusterEquiv);
    }
  }
  else {
    std::vector<HTTTowerInputHeader> towers = logicalHeader.towers();
    for(auto &tower:towers){
      std::vector<HTTHit> const & towerHits = tower.hits();
      for (HTTHit const & hit : towerHits)
	hits.push_back(hit);
    }
  }
  return hits;
}


// Filters tracks based on m_PT_THRESHOLD and m_TRAING_PDG and D0_THRESHOLD
std::vector<HTTTruthTrack> HTTMatrixGenAlgo::filterTrainingTracks(std::vector<HTTTruthTrack> const & truth_tracks) const
{
  std::vector<HTTTruthTrack> training_tracks;

  for (HTTTruthTrack const & track : truth_tracks) {
    if (track.getBarcode() >= 1000000 || abs(track.getPDGCode()) != m_TRAIN_PDG) continue;
    if (abs(track.getD0()) > m_D0_THRESHOLD) continue;
    
    double pt = TMath::Sqrt(track.getPX()*track.getPX() + track.getPY()*track.getPY());
    double pt_GeV = pt / 1000;
    
    // Add the track to the list of good tracks
    if (pt_GeV > m_PT_THRESHOLD) training_tracks.push_back(track);
    
    // Debug
    if (msgLvl(MSG::DEBUG)) {
      double c = track.getQ() /(2 * pt);
      double eta = TMath::ASinH(track.getPZ() / pt);
      double phi = TMath::ATan2(track.getPY(), track.getPX());
      ATH_MSG_DEBUG("pt_GeV = "<< pt_GeV
		    << " c = " << c
		    << " eta = " << eta
		    << " phi = " << phi
		    << " pdgcode = " << track.getPDGCode());
    }
    fillTrackPars(m_h_trainingTrack, track);
  }
  
  return training_tracks;
}


// Sorts 'hits' by barcodes appearing in 'tracks', drops the rest.
std::map<int, std::vector<HTTHit>> HTTMatrixGenAlgo::makeBarcodeMap(std::vector<HTTHit> const & hits, std::vector<HTTTruthTrack> const & tracks) const
{
  std::map<int, std::vector<HTTHit>> map;

  // Ready the barcodes
  for (const HTTTruthTrack & track : tracks)
    map[track.getBarcode()] = std::vector<HTTHit>();

  // Add the hits
  for (const HTTHit & hit : hits) {
    // Get the predominant barcode for the current hit
    int barcode = hit.getTruth().best_barcode();
    
    // Add hit to the list; skip the hits if not associated to a good truth tracks
    auto it = map.find(barcode);
    if (it != map.end()) (*it).second.push_back(hit);
  }
  
  return map;
}


// Given two hits in the same layer, selects the better hit to use for sector
// generation based on multiple criteria.
//
// Returns:
//      0 - Failure, this track should be discarded
//      1 - Keep old hit
//      2 - Use new hit
//
// NB: sector overlap is a perfectly reasonable situation with forward disks
// eta and phi will differ in general in this case
// Take the lower-z hit preferentially (right thing to do? d0/pT tradeoff)
// But something fishy is going on if we've got two hits on the same disk.
HTTMatrixGenAlgo::selectHit_returnCode HTTMatrixGenAlgo::selectHit(HTTHit const & old_hit, HTTHit const & new_hit) const
{
  if ((new_hit.getSection() == old_hit.getSection()) && (new_hit.getLayer() == old_hit.getLayer())
      && (new_hit.getHTTEtaModule() == old_hit.getHTTEtaModule()) && (new_hit.getPhiModule() == old_hit.getPhiModule())) {
    ATH_MSG_DEBUG("Two hits on same module");
    return selectHit_returnCode::SH_FAILURE;
  }
  

  int new_section = new_hit.getSection();
  int old_section = old_hit.getSection();

  if (old_section == new_section) {

    if (old_hit.getHTTEtaModule() == new_hit.getHTTEtaModule()) {
      int rmax = 0;
      if (m_doHoughConstants) {
	int reg = m_HTTMapping->RegionMap_1st()->getRegions(new_hit)[0]; // just take region with lowest index
	
	int phi_max = m_HTTMapping->RegionMap_1st()->getRegionBoundaries(reg, new_hit.getLayer(), new_section).phi_max;
	int phi_min = m_HTTMapping->RegionMap_1st()->getRegionBoundaries(reg, new_hit.getLayer(), new_section).phi_min;
	
	rmax = phi_max - phi_min;
      }
      else {
	int reg = m_HTTMapping->RegionMap_2nd()->getRegions(new_hit)[0]; // just take region with lowest index
	
	int phi_max = m_HTTMapping->RegionMap_2nd()->getRegionBoundaries(reg, new_hit.getLayer(), new_section).phi_max;
	int phi_min = m_HTTMapping->RegionMap_2nd()->getRegionBoundaries(reg, new_hit.getLayer(), new_section).phi_min;
	
	rmax = phi_max - phi_min;
      }
      
      int phi_diff = old_hit.getPhiModule() - new_hit.getPhiModule();
      
      if (phi_diff == 1 || phi_diff == -rmax) return selectHit_returnCode::SH_KEEP_OLD;
      else if (phi_diff == -1 || phi_diff == rmax) return selectHit_returnCode::SH_KEEP_NEW;
      else {
	ATH_MSG_DEBUG("Hits are too far away in phi");
	return selectHit_returnCode::SH_FAILURE;
      }
    }
    else { // Different eta is no good
      
      ATH_MSG_DEBUG("Hits are in different eta");
      return selectHit_returnCode::SH_FAILURE;
    }
  }
  else { // sections are different
    
    int  layer = old_hit.getLayer();
    bool old_isEC = 0;
    bool new_isEC = 0;
    int  old_disk = 0;
    int  new_disk = 0;
    if (m_doHoughConstants) {
      old_isEC = m_HTTMapping->PlaneMap_1st()->isEC(layer, old_section);
      new_isEC = m_HTTMapping->PlaneMap_1st()->isEC(layer, new_section);
      old_disk = m_HTTMapping->PlaneMap_1st()->getLayerInfo(layer, old_section).physDisk;
      new_disk = m_HTTMapping->PlaneMap_1st()->getLayerInfo(layer, new_section).physDisk;
    }
    else {
      old_isEC = m_HTTMapping->PlaneMap_2nd()->isEC(layer, old_section);
      new_isEC = m_HTTMapping->PlaneMap_2nd()->isEC(layer, new_section);
      old_disk = m_HTTMapping->PlaneMap_2nd()->getLayerInfo(layer, old_section).physDisk;
      new_disk = m_HTTMapping->PlaneMap_2nd()->getLayerInfo(layer, new_section).physDisk;
    }
    // If one is barrel and one endcap, it's definitely OK, take the barrel hit
    if (old_isEC != new_isEC) {
      
      if (old_isEC) return selectHit_returnCode::SH_KEEP_NEW;
      else return selectHit_returnCode::SH_KEEP_OLD;
    }
    // Two endcap hits : same disk: discard
    else if (old_disk == new_disk) {
      
      ATH_MSG_DEBUG("Two modules hit in same physical disk " << old_disk);
      return selectHit_returnCode::SH_FAILURE;
    }
    // Two endcap hits on same side: different disks: take the lower-z
    else {
      ATH_MSG_DEBUG("Keeping the lower-z of the two disks hit");
      if (old_disk > new_disk) return selectHit_returnCode::SH_KEEP_NEW;
      else return selectHit_returnCode::SH_KEEP_OLD;
    }
  }
}


// A sector is created from 8 hits in 8 layers. Sometimes there will be extraneous hits
// that need to be filtered. This functions returns true on success, and by reference
// the filtered hit list with size m_nLayers.
//
// See selectHit() for details on which hit is chosen when there's more than 1 per layer.
bool HTTMatrixGenAlgo::filterSectorHits(std::vector<HTTHit> const & all_hits, std::vector<HTTHit> & sector_hits,
					/* TEMP */ HTTTruthTrack const & t) const
{
  HTTHit nohit;
  nohit.setHitType(HitType::wildcard);
  sector_hits.resize(m_nLayers, nohit);
  std::vector<int> layer_count(m_nLayers); // count number of hits seen in each layer

  for (HTTHit const & hit : all_hits) {
    int layer = hit.getLayer();
    
    if (layer_count[layer] == 0){
      layer_count[layer]++;
      sector_hits[layer] = hit;
    }
    else if (layer_count[layer] == 1) {
      layer_count[layer]++;
      
      // Already found a hit in this layer, so pick which hit to use
      selectHit_returnCode selected_hit = selectHit(sector_hits[layer], hit);
      
      if (selected_hit == selectHit_returnCode::SH_FAILURE) {
	fillTrackPars(m_h_SHfailure, t);
	return false;
      }
      else if (selected_hit == selectHit_returnCode::SH_KEEP_NEW) sector_hits[layer] = hit;
    }
    else {
      ATH_MSG_DEBUG("Too many hits on a plane, exiting filterHitsSec");
      fillTrackPars(m_h_3hitsInLayer, t);
      return false;
    }
  }
  
  int nwc(0);
  // Check we have 8 hits
  for (int i = 0; i < m_nLayers; ++i) {
    if (layer_count[i] == 0) {
      ATH_MSG_DEBUG("Layer " << i << " has no hits");
      nwc++;
    }
  }
  
  if (nwc > m_MaxWC) {
    fillTrackPars(m_h_notEnoughHits, t);
    return false;
  }
  return true;
}


// Returns the lowest index region that contains all hits in 'hits'
int HTTMatrixGenAlgo::getRegion(std::vector<HTTHit> const & hits) const
{
  // Start with a bitmask, all true, and set a region to false if any mismatch is found
  std::vector<bool> region_mask(m_nRegions, true);

  for (HTTHit const & hit : hits) {
    if (hit.getHitType() !=  HitType::wildcard){ // don't worry about hits that are WCs
      for (int region = 0; region < m_nRegions; region++) {
	if (m_doHoughConstants) {
	  if (!m_HTTMapping->RegionMap_1st()->isInRegion(region, hit))
	    region_mask[region] = false;
	}
	else {
	  if (!m_HTTMapping->RegionMap_2nd()->isInRegion(region, hit))
	    region_mask[region] = false;
	}
      }
    }
  }

  // For now just give preference to lowest region index for simplicity
  for (int region = 0; region < m_nRegions; region++)
    if (region_mask[region])
      return region;

  return -1;
}


// Given a track and corresponding hits, returns the sector (list of modules) and the accumulation
// struct.
StatusCode HTTMatrixGenAlgo::makeAccumulator(std::vector<HTTHit> const & sector_hits, HTTTruthTrack const & track, std::pair<std::vector<module_t>, HTTMatrixAccumulator> & accumulator) const
{
  std::vector<module_t> modules(m_nLayers);
  HTTMatrixAccumulator acc(m_nLayers, m_nDim);

  double qoverpt = track.getQ() / track.getPt();
  int sectorbin = 0;
  for (unsigned bin = 0; bin < htt::QOVERPT_BINS.size()-1; bin++) {
    sectorbin = bin;
    if (qoverpt < htt::QOVERPT_BINS[bin+1]) break;
  }


  // Create sector definitions (list of modules)
  for (int i = 0; i < m_nLayers; i++)
    {
      if (sector_hits[i].getHitType() != HitType::wildcard) {
	if (m_single) modules[i] = sector_hits[i].getHTTIdentifierHash();
	else modules[i] = sectorbin; // Here we used to set the identifier, now just global zero! we can change this by large region if needed
      }
      else {
        modules[i] = -1; // WC
      }
    }

  if (m_single) {
    const int ToKeep[13] = {2200,2564,2861,3831,5368,14169,14173,20442,20446,29625,29629,42176,42180};
    bool keepThis = true;
    for (int i = 0; i < 13; i++) {
      if (modules[i] != ToKeep[i] && modules[i] != -1) {
	keepThis = false;
      }
    }

    if (!keepThis) {
      for (int i = 0; i < 13; i++) modules[i] = -1;
    }
    else {
      for (int i = 0; i < 13; i++) {
      }
    }
  }


  // rho = (0.33 m) (pT/GeV) / (B/T)
  // 0.33 in m -> 330.0 mm. Track pt -> GeV. 2.0 Tesla
  // inverse, x2 for convenience

  // rho =    330 mm * track.getPT() / 2 T
  // 2 rho =  330 mm * track.getPT()
  //    1/ (2rho) = 1./(330 * pT) for pT in GeV
  // or 1./(0.33 * pt) for pT in MeV

  double const trackTwoRhoInv = track.getQ() * 1.0 / ( 0.33 * track.getPt() );

  // Hough Constants parameters
  double y = accumulator.second.pars.qOverPt;
  double x = accumulator.second.pars.phi;
  double const houghRho = htt::A * y; // Aq/pT

  // Vectorize (flatten) coordinates
  std::vector<double> coords;
  std::vector<double> coordsG;
  for (int i = 0; i < m_nLayers; ++i) {
    if (sector_hits[i].getHitType() != HitType::wildcard) {
      double hitGPhi = sector_hits[i].getGPhi(); // need to be careful about 2 pi boundary in the future!
      
      if (m_doHoughConstants){
	double expectedGPhi = x; // to get the intersection of the hough road with detector layer
	
	hitGPhi += ( sector_hits[i].getR() - htt::TARGET_R_1STAGE[i] ) * houghRho; //first order
	expectedGPhi -= htt::TARGET_R_1STAGE[i] * houghRho; //first order
	
	if ( m_ideal_geom > 1 ) {
	  hitGPhi += ( pow( sector_hits[i].getR() * houghRho, 3.0 ) / 6.0 ); //higher order
	  expectedGPhi -= ( pow( htt::TARGET_R_1STAGE[i] * houghRho, 3.0 ) / 6.0 ); //higher order
	}
	
	coords.push_back(hitGPhi - expectedGPhi);
	coordsG.push_back(hitGPhi - expectedGPhi);	  
      }
      else {
	// Idealise phi coordinate if requested
	if ( m_ideal_geom > 0 ) {
	  hitGPhi += ( sector_hits[i].getR() - htt::TARGET_R_2STAGE[i] ) * trackTwoRhoInv; //first order
	}
	if ( m_ideal_geom > 1 ) {
	  hitGPhi += ( pow( sector_hits[i].getR() * trackTwoRhoInv, 3.0 ) / 6.0 ); //higher order
	}
	coords.push_back(hitGPhi);
	coordsG.push_back(hitGPhi);
      }
      
      if (sector_hits[i].getDim() == 2){
	double hitZ = sector_hits[i].getZ();
	
	if(m_doHoughConstants){
	  hitZ -= sector_hits[i].getGCotTheta() * (sector_hits[i].getR() - htt::TARGET_R_1STAGE[i]);
	  if ( m_ideal_geom > 1 )
	    hitZ -= (sector_hits[i].getGCotTheta() * pow (sector_hits[i].getR(), 3.0) * houghRho * houghRho) / 6.0;
	}
	else{
	  if ( m_ideal_geom > 0 ) {
	    hitZ -= sector_hits[i].getGCotTheta() * (sector_hits[i].getR() - htt::TARGET_R_2STAGE[i]);
	  }
	  if ( m_ideal_geom > 1 ) {
	    hitZ -= sector_hits[i].getGCotTheta() * ( pow (sector_hits[i].getR(), 3.0) * trackTwoRhoInv * trackTwoRhoInv) / 6.0;
	  }
	}
	
	coords.push_back(hitZ);
	coordsG.push_back(hitZ);
      }
    }
    else {
      coords.push_back(0);
      coordsG.push_back(0);
      if (m_pmap->getDim(i) == 2) coords.push_back(0);
      if (m_pmap->getDim(i) == 2) coordsG.push_back(0);
    }
  }
  assert(coords.size() == (size_t)m_nDim);
  acc.hit_coords = coords;
  acc.hit_coordsG = coordsG;
  
  // Get the track parameters
  acc.pars = track.getPars();
  if (m_doHoughConstants) {
    acc.pars.qOverPt = (y / 1000.0) - track.getQOverPt(); // fit for delta q/pT
    acc.pars.phi = x - track.getPhi(); // fit for delta phi_0
  }
  
  HTTTrackParsI bins;
  for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
    bins[i] = (acc.pars[i] - m_sliceMin[i]) * m_nBins[i] / (m_sliceMax[i] - m_sliceMin[i]);
  acc.track_bins.push_back(bins);

  // Force phi to be in [0, 2pi] (post binning)
  if (!m_doHoughConstants) {
    while (acc.pars.phi < 0) acc.pars.phi += 2*M_PI;
    while (acc.pars.phi > 2*M_PI) acc.pars.phi -= 2*M_PI;
  }

  // Calculate the pre-multiplied elements
  for (int i = 0; i < m_nDim; i++)
    {
      acc.hit_x_QoP[i] = coords[i] * acc.pars.qOverPt;
      acc.hit_xG_HIP[i] = coordsG[i] * acc.pars.qOverPt;
      acc.hit_x_d0[i]  = coords[i] * acc.pars.d0;
      acc.hit_x_z0[i]  = coords[i] * acc.pars.z0;
      acc.hit_x_eta[i] = coords[i] * acc.pars.eta;
      acc.hit_xG_eta[i] = coordsG[i] * acc.pars.eta;
      acc.hit_x_phi[i] = coords[i] * acc.pars.phi;

      for (int j = i; j < m_nDim; j++)
	acc.covariance[i * m_nDim + j] = coords[i] * coords[j];

      for (int j = i; j < m_nDim; j++)
	acc.covarianceG[i * m_nDim + j] = coordsG[i] * coordsG[j];
    }

  accumulator = {modules, acc};
  return StatusCode::SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTMatrixGenAlgo::finalize()
{
  ATH_MSG_DEBUG("finalize()");
  ATH_MSG_INFO("Tracks used: " << m_nTracksUsed << "/" << m_nTracks);

  for (int region = 0; region < m_nRegions; region++) {
      // Create the tree
      std::stringstream name;
      std::stringstream title;
      name << "am" << region;
      title << "Ambank " << region << " parameters";
      TTree* tree = new TTree(name.str().c_str(), title.str().c_str());
      ATH_CHECK(m_tHistSvc->regTree(Form("/TRIGHTTMATRIXOUT/%s",tree->GetName()), tree));

      // Fill the tree
      ::fillTree(m_sector_cum[region], tree, m_nLayers, m_nDim);
      // Monitoring
      ATH_MSG_INFO("Sectors found in region " << region << ": " << m_sector_cum[region].size());
      for (auto & sector_info : m_sector_cum[region]) {
	double coverage = sector_info.second.track_bins.size();
	m_h_nHit->Fill(coverage);
	for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
	  m_h_sectorPars[i]->Fill(sector_info.second.pars[i] / coverage);
      }
  }
  
  writeSliceTree();
  ATH_CHECK(m_tHistSvc->finalize());
  return StatusCode::SUCCESS;
}


void HTTMatrixGenAlgo::writeSliceTree()
{
  TTree* sliceTree = new TTree("slice", "Region slice boundaries"); // slice

  sliceTree->Branch("c_max",  &m_sliceMax.qOverPt);
  sliceTree->Branch("d0_max",         &m_sliceMax.d0);
  sliceTree->Branch("phi_max",        &m_sliceMax.phi);
  sliceTree->Branch("z0_max",         &m_sliceMax.z0);
  sliceTree->Branch("eta_max",        &m_sliceMax.eta);

  sliceTree->Branch("c_min",  &m_sliceMin.qOverPt);
  sliceTree->Branch("d0_min",         &m_sliceMin.d0);
  sliceTree->Branch("phi_min",        &m_sliceMin.phi);
  sliceTree->Branch("z0_min",         &m_sliceMin.z0);
  sliceTree->Branch("eta_min",        &m_sliceMin.eta);

  sliceTree->Branch("c_slices", &m_nBins.qOverPt);
  sliceTree->Branch("d0_slices",        &m_nBins.d0);
  sliceTree->Branch("phi_slices",       &m_nBins.phi);
  sliceTree->Branch("z0_slices",        &m_nBins.z0);
  sliceTree->Branch("eta_slices",       &m_nBins.eta);

  StatusCode sc = m_tHistSvc->regTree("/TRIGHTTMATRIXOUT/slice",sliceTree);
  if (sc.isFailure()) ATH_MSG_ERROR("tHist failed");

  sliceTree->Fill();
}
