/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file HTTNNTrackTool.cxx
 * @author Elliott Cheu
 * @date April 28, 2021
 * @brief Does NN tracking
 *
 * Uses lwtnn to calculate an NN score for a set of hits from a track. This is then stored in an HTTTrack object
 */


#include "HTTNNTrackTool.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTMaps/HTTNNMap.h"
#include "TrigHTTObjects/HTTMultiTruth.h"
#include "TrigHTTObjects/HTTFunctions.h"


/////////////////////////////////////////////////////////////////////////////
HTTNNTrackTool::HTTNNTrackTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
    AthAlgTool(algname, name, ifc)
{
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTNNTrackTool::initialize()
{
    ATH_CHECK(m_HTTMapping.retrieve());
    ATH_CHECK(m_tHistSvc.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode HTTNNTrackTool::getTracks(std::vector<HTTRoad*> &roads, std::vector<HTTTrack> &tracks, 
                                     const HTTNNMap *nnMap)
{

  int n_track = 0;

  // Loop over roads 
  for (auto iroad : roads){


    HTTRoad_Hough *hough_road = dynamic_cast<HTTRoad_Hough*>(iroad);
    double y = 0;
    if (hough_road != nullptr) { 
      y = hough_road->getY();
    }

    // Error checking
    int sector = iroad->getSector();
    if (sector < 0) {
      ATH_MSG_DEBUG("Bad sector " << sector);
      return StatusCode::SUCCESS;
    }

    // Get info on layers with missing hits
    int nMissing = 0;
    layer_bitmask_t missing_mask = 0;
    
    // Just used to get number of layers considered
    const HTTPlaneMap *planeMap = m_HTTMapping->PlaneMap_1st();

    // Create a template track with common parameters filled already for initializing below
    HTTTrack temp;
    temp.setTrackStage(TrackStage::FIRST);
    temp.setNLayers(planeMap->getNLogiLayers());
    temp.setBankID(-1); 
    temp.setPatternID(iroad->getPID());
    temp.setFirstSectorID(iroad->getSector());
    temp.setHitMap(missing_mask);
    temp.setNMissing(nMissing);
    temp.setQOverPt(y);
  
    ////////////////////////////////////////////////////////////////////////
    // Get a list of indices for all possible combinations given a certain number of layers
    std::vector<std::vector<int>> combs = getComboIndices(iroad->getNHits_layer());

    // Loop over possible combinations for this road
    for (size_t icomb = 0; icomb < combs.size(); icomb++)  {

      // list of indices for this particular combination
      std::vector<int> const & hit_indices = combs[icomb]; 
      std::vector<const HTTHit*> hit_list;

      // Loop over all layers
      for (unsigned layer = 0; layer < planeMap->getNLogiLayers(); layer++) {

        // Check to see if this is a valid hit
        if (hit_indices[layer] >= 0) { 

          const HTTHit* hit = iroad->getHits(layer)[hit_indices[layer]];
          // Add this hit to the road
          hit_list.push_back(hit);
        }
      }

      // Sort the list by radial distance
      std::sort(hit_list.begin(), hit_list.end(), [](const HTTHit* hit1, const HTTHit* hit2) {
	  double rho1 = std::hypot(hit1->getX(), hit1->getY());
	  double rho2 = std::hypot(hit2->getX(), hit2->getY());
	  return rho1 < rho2;
      });

      // ECC - Input maps for lwtnn
      std::map<std::string, std::map<std::string, double>> valMap;
      std::map<std::string, std::map<std::string, std::vector<double>>> vectorMap;

      // Initalize to default
      for(int i = 1; i <= 8; i++)
      {
        TString indexStr = Form("%d", i);
        valMap["dNN"][("hitX" + indexStr).Data()] = 0;
        valMap["dNN"][("hitY" + indexStr).Data()] = 0;
        valMap["dNN"][("hitZ" + indexStr).Data()] = 0;
      }

      int index = 1;
      bool flipZ = false;
      double rotateAngle = 0;

      // Loop over all hits
      for(const auto& hit: hit_list) {

        // Need to rotate hits
        double x0 = hit->getX();
        double y0 = hit->getY();
        double z0 = hit->getZ();
        if (index == 1) {
          if (z0 < 0) flipZ = true;
          rotateAngle = std::atan(x0/y0);
          if(y0 < 0) rotateAngle += M_PI;
        }	  

        double xf = x0 * std::cos(rotateAngle) - y0 * std::sin(rotateAngle);
        double yf = x0 * std::sin(rotateAngle) + y0 * std::cos(rotateAngle);
        double zf = z0; 
	if (flipZ) zf = z0*-1;

        // Fill map for lwtnn
        TString indexStr = Form("%d", index);
        valMap["dNN"][("hitX" + indexStr).Data()] = xf/getXScale();
        valMap["dNN"][("hitY" + indexStr).Data()] = yf/getYScale();
        valMap["dNN"][("hitZ" + indexStr).Data()] = zf/getZScale();
        index++;
      }

      // Use NN filter to see if these set of hits comprise a good track
      std::shared_ptr<lwt::LightweightGraph> lwnn_map = nnMap->getNNMap();
      auto scoreMap = lwnn_map->compute(valMap, vectorMap);
      float nn_val = scoreMap.at("NNScore");

      // Save all hits on this road if it passes our "good" criteria
      if (nn_val > m_NNCut) {

        // This is a track. Fill class and add to track list
        n_track++;
        HTTTrack track_cand;
        track_cand.setTrackID(n_track);
        track_cand.setNLayers(planeMap->getNLogiLayers());
        for (const auto &ihit : hit_list) {
          unsigned int layer = ihit->getLayer();
          track_cand.setHTTHit(layer, *ihit);
        }
        // Nominal chi2ndof cut is 40 and we want to use NN>0.0075 (or NN<(1-0.0075)
        // Nominal chi2ndof cut is 40 and we want to use NN>0.001 (or NN<(1-0.1)
        double scale = m_chi2_scalefactor*(track_cand.getNCoords()-track_cand.getNMissing()-5);
        double chi2 = scale*(1-nn_val);
        track_cand.setOrigChi2(chi2);
        track_cand.setChi2(chi2);
        tracks.push_back(track_cand);
      }
    }  // loop over combinations

  } // loop over roads

  // Add truth info
  for (HTTTrack & t : tracks)
      compute_truth(t); // match the track to a geant particle using the channel-level geant info in the hit data.

  return StatusCode::SUCCESS;
}

// Borrowed same code from TrackFitter - probably a nicer way to inherit instead
void HTTNNTrackTool::compute_truth(HTTTrack & t) const
{
    std::vector<HTTMultiTruth> mtv;
    const HTTPlaneMap *planeMap = m_HTTMapping->PlaneMap_1st();

    for (unsigned layer = 0; layer < planeMap->getNLogiLayers(); layer++)
    {
      if (t.getHitMap() & (1 << planeMap->getCoordOffset(layer))) continue; // no hit in this plane
	    //Sanity check that we have enough hits.
	    if (layer < t.getHTTHits().size()) mtv.push_back(t.getHTTHits().at(layer).getTruth());

        // adjust weight for hits without (and also with) a truth match, so that each is counted with the same weight.
        mtv.back().assign_equal_normalization();
    }

    HTTMultiTruth mt( std::accumulate(mtv.begin(), mtv.end(), HTTMultiTruth(), HTTMultiTruth::AddAccumulator()) );
    // frac is then the fraction of the total number of hits on the track attributed to the barcode.

    HTTMultiTruth::Barcode tbarcode;
    HTTMultiTruth::Weight tfrac;
    const bool ok = mt.best(tbarcode, tfrac);
    if (ok)
    {
        t.setEventIndex(tbarcode.first);
        t.setBarcode(tbarcode.second);
        t.setBarcodeFrac(tfrac);
    }
}  

