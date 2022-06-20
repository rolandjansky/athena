// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "HTTOverlapRemovalTool.h"
#include "TrigHTTMaps/HTTPlaneMap.h"
#include "TrigHTTObjects/HTTVectors.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <iostream>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////
HTTOverlapRemovalTool::HTTOverlapRemovalTool(const std::string& algname, const std::string& name, const IInterface *ifc) :
    AthAlgTool(algname, name, ifc)
{
}

StatusCode HTTOverlapRemovalTool::initialize()
{
  ATH_MSG_INFO( "HTTOverlapRemovalTool::initialize()" );
  // Check if this is 2nd stage
  if(m_do2ndStage)
  {
    m_totLayers = m_HTTMapping->PlaneMap_2nd()->getNLogiLayers();;
  }
  else
  {
    m_totLayers = m_HTTMapping->PlaneMap_1st()->getNLogiLayers();;
  }
  ATH_MSG_DEBUG("Total number of layer: " << m_totLayers);

  // Check road OR
  if (m_localMaxWindowSize && !m_roadSliceOR)
      ATH_MSG_WARNING("LocalMaxOR only being run per hough slice (i.e. this tool does nothing) since roadSliceOR is turned off");


  //  Setup OR algorithm
  if(m_algorithm == "Normal") m_algo=ORAlgo::Normal;
  else if(m_algorithm == "Invert") m_algo=ORAlgo::InvertGrouping;
  else
  {
    ATH_MSG_ERROR("initialize(): OR algorithm doesn't exist. ");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Overlap removal algorithm is "<<m_algorithm.value());

  return StatusCode::SUCCESS;
}

bool isLocalMax(vector2D<HTTRoad*> const & acc, unsigned x, unsigned y, int localMaxWindowSize)
{
    if (!acc(y, x)) return false;

    for (int j = -localMaxWindowSize; j <= localMaxWindowSize; j++)
        for (int i = -localMaxWindowSize; i <= localMaxWindowSize; i++)
        {
            if (i == 0 && j == 0) continue;
            if (y + j < acc.size(0) && x + i < acc.size(1))
            {
                if (!acc(y+j, x+i)) continue;
                if (acc(y+j, x+i)->getNHitLayers() > acc(y, x)->getNHitLayers()) return false;
                if (acc(y+j, x+i)->getNHitLayers() == acc(y, x)->getNHitLayers())
                {
                    if (acc(y+j, x+i)->getNHits() > acc(y, x)->getNHits()) return false;
                    if (acc(y+j, x+i)->getNHits() == acc(y, x)->getNHits() && j <= 0 && i <= 0) return false;
                }
            }
        }

    return true;
}

StatusCode HTTOverlapRemovalTool::runOverlapRemoval(std::vector<HTTRoad*>& roads)
{
    if (roads.empty()) return StatusCode::SUCCESS;
    if (!dynamic_cast<HTTRoad_Hough*>(roads.front())) return StatusCode::SUCCESS;
    if (!m_roadSliceOR) return StatusCode::SUCCESS;
    size_t in = roads.size();

    // Hough image
    vector2D<HTTRoad*> acc(m_imageSize_y, m_imageSize_x);

    // Slice-wise duplicate removal: accept only one road (with most hits) per bin
    for (HTTRoad* r: roads)
    {
        HTTRoad_Hough* hough = dynamic_cast<HTTRoad_Hough*>(r);
        if (!hough) return StatusCode::FAILURE; // mixed hough non-hough roads not allowed!

        HTTRoad* & old = acc(hough->getYBin(), hough->getXBin());
        if (!old) old = r;
        else if (r->getNHitLayers() > old->getNHitLayers()) old = r;
        else if (r->getNHitLayers() == old->getNHitLayers() && r->getNHits() > old->getNHits()) old = r;
    }

    // All-slices local max
    if (m_localMaxWindowSize)
        for (unsigned y = 0; y < m_imageSize_y; y++)
            for (unsigned x = 0; x < m_imageSize_x; x++)
                if (!isLocalMax(acc, x, y, m_localMaxWindowSize))
                    acc(y, x) = nullptr;

    // Reformat to vector
    roads.clear();
    for (unsigned y = 0; y < m_imageSize_y; y++)
        for (unsigned x = 0; x < m_imageSize_x; x++)
            if (acc(y, x))
                roads.push_back(acc(y, x));

    ATH_MSG_DEBUG("Input: " << in << " Output: " << roads.size());
    return StatusCode::SUCCESS;
}

StatusCode HTTOverlapRemovalTool::runOverlapRemoval(std::vector<HTTTrack>& tracks)
{

  // Do fast OR instead of requested
  if (m_doFastOR) return runOverlapRemoval_fast(tracks);

  // Otherwise, proceed
  ATH_MSG_DEBUG("Beginning runOverlapRemoval()");

  // Create tracks to hold and compare
  HTTTrack fit1, fit2;
  for(unsigned int i=0; i<tracks.size();i++)
  {
    fit1=tracks.at(i);
    // Apply Chi2 cut
    if(fit1.getChi2ndof()>m_minChi2)
    {
      // Only consider track with chi2 smaller than m_minChi2
      tracks.at(i).setPassedOR(0);
      continue;
    }

    // Create vector for holding duplicate track list
    std::vector<int> duplicates(1,i);

    // Loop through the remaning tracks
    for(unsigned int j=i+1; j<tracks.size(); j++)
    {
      if(i!=j)
      {
        fit2=tracks.at(j);
        // Apply Chi2 cut
        if(fit2.getChi2ndof()>m_minChi2)
        {
          // Only consider track with chi2 smaller than m_minChi2
          tracks.at(j).setPassedOR(0);
          continue;
        }
        //  Based on the algorithm choose common hit of non-common hit
        if(getAlgorithm() == ORAlgo::Normal)
        {
          // Find the number of common hits between two tracks
          int nOverlappingHits = 0;
          nOverlappingHits=findNCommonHits(fit1,fit2);

          // Group overlapping tracks into a vector for removal if at least [m_NumOfHitPerGrouping] hits are the same
          if(nOverlappingHits >= m_NumOfHitPerGrouping)
          {
            duplicates.push_back(j);
          }
        }
        else if(getAlgorithm() == ORAlgo::InvertGrouping)
        {
          //  Find the number of non-common hits between two tracks
          int nNotOverlappingHits=0;
          nNotOverlappingHits=findNonOverlapHits(fit1, fit2);

          // If the number of non-overlapping hit is [m_NumOfHitPerGrouping] or less
          if(nNotOverlappingHits <= m_NumOfHitPerGrouping)
          {
            duplicates.push_back(j);
          }
        }
      }
    }

    findMinChi2MaxHit(duplicates, tracks);
  }

  return StatusCode::SUCCESS;
}

void HTTOverlapRemovalTool::findMinChi2MaxHit(const std::vector<int>& duplicates, std::vector<HTTTrack>& RMtracks)
{
  //  Hardcode the CHI2MAX for now, should change it later
  float minChi2=100000.;
  int   prevID =-1;
  int   maxHitLayers=0;
  for(auto dup: duplicates)
  {
    float t_chi2 = RMtracks.at(dup).getChi2ndof();
    int t_nhitlayers = RMtracks.at(dup).getHTTHits().size();
    for(auto& hit : RMtracks.at(dup).getHTTHits())
    {
      if(!hit.isReal())
      {
        t_nhitlayers--;
      }
    }

    if(t_nhitlayers>maxHitLayers)
    {
      if(prevID!=-1)
      {
        RMtracks.at(prevID).setPassedOR(0);
      }
      prevID=dup;
      maxHitLayers=t_nhitlayers;
      minChi2=t_chi2;
    }
    else if(t_nhitlayers==maxHitLayers)
    {
      if(t_chi2<minChi2)
      {
        if(prevID!=-1)
        {
          RMtracks.at(prevID).setPassedOR(0);
        }
        prevID=dup;
        minChi2=t_chi2;
      }
      else
      {
        RMtracks.at(dup).setPassedOR(0);
      }
    }
    else
    {
      RMtracks.at(dup).setPassedOR(0);
    }
  }
}

int HTTOverlapRemovalTool::findNCommonHits(const HTTTrack& Track1, const HTTTrack& Track2)
{
  int nCommHits=0;
  const std::vector <HTTHit> hits1 = Track1.getHTTHits();
  const std::vector <HTTHit> hits2 = Track2.getHTTHits();

  // Loop through all layers
  for(unsigned int i = 0; i < hits1.size(); ++i)
  {
    // Check if hit is missing
    if(!hits1.at(i).isReal() || !hits2.at(i).isReal())
    {
      continue;
    }
    // Check if hit on the same plane
    else if(hits1.at(i).getLayer() != hits2.at(i).getLayer())
    {
      continue;
    }
    // Check if two hits have the same hashID
    else if(hits1.at(i).getHTTIdentifierHash() != hits2.at(i).getHTTIdentifierHash())
    {
      continue;
    }
    // Check if two hits have same coordinate
    else if(hits1.at(i).getPhiCoord() == hits2.at(i).getPhiCoord()
            && hits1.at(i).getEtaCoord() == hits2.at(i).getEtaCoord())
    {
      nCommHits++;
    }
    else
    {
      continue;
    }
  }
  return nCommHits;
}

int HTTOverlapRemovalTool::findNonOverlapHits(const HTTTrack& Track1, const HTTTrack& Track2)
{
  int nonOverlapHits=0;
  const std::vector <HTTHit> hits1 = Track1.getHTTHits();
  const std::vector <HTTHit> hits2 = Track2.getHTTHits();

  //  Loop through all layers
  for(unsigned int i=0; i<hits1.size(); i++)
  {
    //  First make sure we are looking at real hits
    if(!hits1.at(i).isReal() || !hits2.at(i).isReal())
    {
      continue;
    }
    //  Check if two hits are on the same plane
    else if(hits1.at(i).getLayer() != hits2.at(i).getLayer())
    {
      nonOverlapHits++;
    }
    // Check if two hits have the same hashID
    else if(hits1.at(i).getHTTIdentifierHash() != hits2.at(i).getHTTIdentifierHash())
    {
      nonOverlapHits++;
    }
    else if(hits1.at(i).getPhiCoord() != hits2.at(i).getPhiCoord()
            || hits1.at(i).getEtaCoord() != hits2.at(i).getEtaCoord())
    {
      nonOverlapHits++;
    }
    else
    {
      continue;
    }
  }
  return nonOverlapHits;
}


StatusCode HTTOverlapRemovalTool::removeOverlapping(HTTTrack & track1, HTTTrack & track2) {

    // Hit comparison
    struct HitCompare {
        bool operator()(const HTTHit* a, const HTTHit* b) const { 
            auto hash_a = a->getIdentifierHash();
            auto hash_b = b->getIdentifierHash();
            if ( hash_a == hash_b ) {
                auto phi_a = a->getPhiCoord();
                auto phi_b = b->getPhiCoord();
                if ( phi_a == phi_b ) {
                    auto eta_a = a->getEtaCoord();
                    auto eta_b = b->getEtaCoord();
                    if ( eta_a == eta_b) {
                        auto layer_a = a->getLayer();
                        auto layer_b = b->getLayer();
                        return layer_a < layer_b;
                    }
                    return eta_a < eta_b;
                }
                return phi_a < phi_b;
            }
            return hash_a <  hash_b; 
        }
    };

    std::set<const HTTHit*, HitCompare > hitsInTrack1;
    for ( auto& hit : track1.getHTTHits()) {
        if (hit.isReal()) hitsInTrack1.insert(&hit);
    }

    std::set<const HTTHit*, HitCompare> hitsInTrack2;
    for ( auto& hit: track2.getHTTHits()){
        if (hit.isReal()) hitsInTrack2.insert(&hit);
    }

    std::vector<const HTTHit*> sharedHits;    
    std::set_intersection( hitsInTrack1.begin(), hitsInTrack1.end(), 
                         hitsInTrack2.begin(), hitsInTrack2.end(), 
                         std::back_inserter(sharedHits), 
                         HitCompare() );

    // Number of real hits in track 1, number of real hits in track 2, number of shared hits, number of non-shared hits (?)
    int nHitsInTrack1 = hitsInTrack1.size();
    int nHitsInTrack2 = hitsInTrack2.size();
    int nSharedHits = sharedHits.size();
    // Original version seems to be only track 1; I want to compare each pair only once so
    // let's make this the most conservative option (i.e. smallest number) ?
    int nonOverlappingHits = std::min(nHitsInTrack1 - nSharedHits, nHitsInTrack2 - nSharedHits);

    // Now check if these pass our criteria for overlapping. If not, just return.
    if(getAlgorithm() == ORAlgo::Normal) {
      // Here decision is based on number of overlapping hits.
      // Consider these overlapping if they share >= m_NumOfHitPerGrouping
      if(nSharedHits < m_NumOfHitPerGrouping) return StatusCode::SUCCESS;

    } else if(getAlgorithm() == ORAlgo::InvertGrouping) {
      // This is the opposite: duplicates of number of unique hits is <= m_NumOfHitPerGrouping.
      if(nonOverlappingHits > m_NumOfHitPerGrouping) return StatusCode::SUCCESS;

    } else {
      // Unknown.
      return StatusCode::FAILURE;
    }

    // Made it here: these tracks are overlapping. 
    // But we already sorted them such that track 2 is the one
    // we want to keep, so we can just set track 1 to be removed.
    track1.setPassedOR(0);

    return StatusCode::SUCCESS;
}

bool HTTOverlapRemovalTool::compareTrackQuality(const HTTTrack & track1, const HTTTrack & track2)
{
    std::vector<const HTTHit*> hitsInTrack1;
    for ( auto& hit : track1.getHTTHits()) {
        if (hit.isReal()) hitsInTrack1.push_back(&hit);
    }

    std::vector<const HTTHit*> hitsInTrack2;
    for ( auto& hit: track2.getHTTHits()){
        if (hit.isReal()) hitsInTrack2.push_back(&hit);
    }

    // If one track has more hits than the other, it's better.
    // Return true if track 2 is better than track 1.
    // Otherwise, decide based on chi2.
    bool goodOrder = true;
    if (hitsInTrack1.size() == hitsInTrack2.size()) {
      // Surprising number of cases where the chi2 is actually identical.
      // In these cases, let's default to the track ID number as the next most important property.
      // So put it higher since we are considering later tracks to be better.
      if (track1.getChi2ndof() == track2.getChi2ndof() && track1.getTrackID() < track2.getTrackID()) goodOrder = false; 
      // Now assuming they're different, we want them in decreasing chi2 order
      else if (track1.getChi2ndof() < track2.getChi2ndof()) goodOrder = false;
    } else if (hitsInTrack1.size() > hitsInTrack2.size()) {
      goodOrder = false;
    } 

    return goodOrder;

}

StatusCode HTTOverlapRemovalTool::runOverlapRemoval_fast(std::vector<HTTTrack>& tracks)
{
  ATH_MSG_DEBUG("Beginning fast overlap removal");

  // Sort tracks in order of increasing quality. 
  // This way, once a track has been eliminated in a comparison, we don't
  // need to check it against any other tracks - they will always be
  // better than it.
  std::sort(std::begin(tracks), 
            std::end(tracks), 
            compareTrackQuality); 

  // Now compare every pair of tracks.
  // Set passedOR to 0 for the worst one (first one)
  // if they are found to overlap.
  for (unsigned int i=0; i < tracks.size(); i++) {

    // Skip track i if bad chi2.
    if (tracks.at(i).getChi2ndof() > m_minChi2) {
      tracks.at(i).setPassedOR(0);
      continue;
    }

    // Now check against all other tracks.
    for (unsigned int j=i+1; j< tracks.size(); j++) {

      // Uniquely comparing tracks i and j here.

      // If have set track i to 0 in a previous comparison, 
      // no need to look at it any more - we're done with it.
      if (!tracks.at(i).passedOR()) break;      

      // Ignore j if its chi2 is bad.
      if (tracks.at(j).getChi2ndof() > m_minChi2) tracks.at(j).setPassedOR(0);

      // If we just set track j to zero for bad chi2,
      // no need to do the comparison.
      if (!tracks.at(j).passedOR()) continue;      

      // If we're still here, two at least semi-decent tracks.
      // Compare them and remove one if necessary.
      ATH_CHECK(removeOverlapping(tracks.at(i),tracks.at(j))); 

    }
  }

  return StatusCode::SUCCESS;
}


