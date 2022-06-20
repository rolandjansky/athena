// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

#include "TrigHTTObjects/HTTMultiTruth.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"


#include "TrackFitter.h"


std::vector<HTTTrack>::const_iterator getBestChi2(std::vector<HTTTrack> const & tracks);
bool hasGoodFit(std::vector<HTTTrack> const & track_cands, float minchi2);


///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

TrackFitter::TrackFitter(const HTTFitConstantBank *nominalbank,
			 const std::vector<const HTTFitConstantBank*>& droppedLayerbanks, bool guessingHits) :
  AthMessaging (Athena::getMessageSvc(), "HTTTrackFitter") 
{
  m_guessinghits = guessingHits;
  m_nominalBank = nominalbank;

  if (!m_nominalBank) ATH_MSG_FATAL("Constant bank not set");
  if (!m_guessinghits)
    for (auto bank : droppedLayerbanks) 
      m_droppedLayerBanks.push_back(bank);
}


void TrackFitter::resetCounters()
{
    m_idbase = 0;
    m_nfits = 0;
    m_nfits_maj = 0;
    m_nfits_maj_pix = 0;
    m_nfits_maj_SCT = 0;
    m_nfits_rec = 0;
    m_nfits_addrec = 0;
}


///////////////////////////////////////////////////////////////////////////////
// fitTracks
///////////////////////////////////////////////////////////////////////////////


int TrackFitter::fitTracks(const std::vector<HTTRoad*>& roads, std::vector<HTTTrack>& tracks) {
    resetCounters();
    for (auto cur_road : roads) {
      std::vector<HTTTrack> t;
      int isOK = fitTracks(cur_road, t);
      if (isOK != FITTRACKS_OK) return isOK;
      else tracks.insert(tracks.end(), t.begin(), t.end());
    }
    return FITTRACKS_OK;
}


/**
 * Creates tracks from the given road.
 *
 * Takes all combinations of hits in the road to create track candidates,
 * fits them using the constant bank, and filters them based on the chi2 of the fit.
 */
 int TrackFitter::fitTracks(HTTRoad *road, std::vector<HTTTrack>& tracks)
{
    m_tracks_missinghits_track.clear();

    HTTRoad_Hough *hough_road = dynamic_cast<HTTRoad_Hough*>(road);
    double y = 0.0;
    double x = 0.0;
    if (hough_road != nullptr && m_IdealCoordFitType != TrackCorrType::None ) {
      y = hough_road->getY();
      x = hough_road->getX();
    }

    // Error checking
    int sector = road->getSector();
    if (sector < 0) {
        ATH_MSG_DEBUG("Bad sector " << sector);
        return FITTRACKS_OK;
    }
    else if (sector >= m_nominalBank->getNSectors()) {
        ATH_MSG_WARNING("Constants for sector " << sector << " don't exist");
        return FITTRACKS_BAD;
    }
    else if (!m_nominalBank->getIsGood(sector)) {
      ATH_MSG_WARNING("Constants for sector " << sector << " are not valid");
      return FITTRACKS_BAD;
    }

    // Get info on layers with missing hits
    int nMissing;
    bool missPixel;
    bool missStrip;
    layer_bitmask_t missing_mask;
    layer_bitmask_t norecovery_mask; // mask to prevent majority in planes with multiple hits
    getMissingInfo(*road, nMissing, missPixel, missStrip, missing_mask, norecovery_mask);
    // Create a template track with common parameters filled already for initializing below
    HTTTrack temp;
    if(!m_do2ndStage){
      temp.setTrackStage(TrackStage::FIRST);
      temp.setFirstSectorID(road->getSector());
    }
    else{
      temp.setTrackStage(TrackStage::SECOND);
      temp.setSecondSectorID(road->getSector());
    }
    temp.setNLayers(m_pmap->getNLogiLayers());
    temp.setBankID(-1); // TODO
    temp.setPatternID(road->getPID());
    temp.setHitMap(missing_mask);
    temp.setNMissing(nMissing);
    temp.setHoughX(x);
    temp.setHoughY(y);
    temp.setQOverPt(y);
    temp.setTrackCorrType(m_IdealCoordFitType);
    temp.setDoDeltaGPhis(m_doDeltaGPhis);

    // Create a list of track candidates by taking all possible combinations of hits in road.
    std::vector<HTTTrack> track_cands;
    makeTrackCandidates(*road, temp, track_cands);

    // Fit the track candidates
    size_t nFits = std::min((size_t)m_max_ncomb, track_cands.size());
    for (size_t icomb = 0; icomb < nFits; icomb++)
    {
        bool ok;
        if (nMissing == 0 || m_guessinghits)
        {
	        ok = m_nominalBank->linfit(sector, track_cands[icomb], m_do2ndStage);
        }
        else
        {
            unsigned int ic = 0; // this will be the leading coordinate with a missing hit
            for (unsigned int ic = 0; ic < m_pmap->getNCoords(); ic++) {
                if (!( (missing_mask >> ic) & 0x1)) break; // if we found one, stop
            }
            // banks are indexed by plane, not by coordinate
            ok = m_droppedLayerBanks[m_pmap->getCoordLayer(ic)]->linfit(sector, track_cands[icomb], m_do2ndStage);
        }

        if (!ok) {
	  continue;
    	}
        track_cands[icomb].setOrigChi2(track_cands[icomb].getChi2());
        if (getDoMissingHitsCheck()) {
            if (track_cands[icomb].getNMissing() == 0) { // missing 0 hits, drop hits one at a time and refit!
                for (unsigned icoord = 0; icoord < m_pmap->getNCoords(); icoord++) { // 9 coordinates to refit, nominally

                    HTTTrack newtrack = track_cands[icomb];
                    newtrack.setNMissing(1);
                    unsigned int bitmask = ( 1 << m_pmap->getNCoords() ) - 1; // all bits set to 1
                    bitmask &= ~(1 << icoord); // remove this coordinate
                    if (m_pmap->getDim(icoord) == 2) { // if we need to do two coordinates at once
                        bitmask &= ~(1 << (icoord+1)); // remove the next coordinate, too
                        newtrack.setNMissing(2);
                    }
                    newtrack.setHitMap(bitmask); // update bitmask
                    m_nominalBank->linfit(sector, newtrack, m_do2ndStage);
                    m_tracks_missinghits_track.push_back(newtrack);
                    if (m_pmap->getDim(icoord) == 2) {
                        icoord++; // skip 2nd of pixel coordinates so we don't do them twice
                    }
                }
            }
        }
        tracks.push_back(track_cands[icomb]);
    }

    // Increment counters
    m_nfits += nFits;
    if (nMissing > 0)
    {
        m_nfits_maj += nFits;
        if (missPixel) m_nfits_maj_pix += nFits;
        if (missStrip) m_nfits_maj_SCT += nFits;
    }

    // Do recovery fits
    if (nMissing == 0) {
        // In the case of m_do_majority > 1, we only do majority fits if ALL full fits fail the chi2 cut
        if (m_do_majority == 1 || (m_do_majority > 1 && !hasGoodFit(tracks, m_Chi2Dof_recovery_min)))
        {
            for (HTTTrack & t : tracks)
	      if (t.getChi2ndof() > m_Chi2Dof_recovery_min && t.getChi2ndof() < m_Chi2Dof_recovery_max){
		double y(0);
		HTTRoad_Hough *hough_road = dynamic_cast<HTTRoad_Hough*>(road);
		if (hough_road != nullptr && m_IdealCoordFitType != TrackCorrType::None)
		  y = hough_road->getY();

		t = recoverTrack(t, norecovery_mask, sector, y);

	      }
        }
    }

    // Add truth info
    for (HTTTrack & t : tracks) {
        compute_truth(t); // match the track to a geant particle using the channel-level geant info in the hit data.
    }

    return FITTRACKS_OK;
}


///////////////////////////////////////////////////////////////////////////////
// fitTracks core helper functions
///////////////////////////////////////////////////////////////////////////////


// Given road, populates the supplied variables with info on which layers missed hits
void TrackFitter::getMissingInfo(const HTTRoad & road, int & nMissing, bool & missPixel, bool & missStrip,
        layer_bitmask_t & missing_mask, layer_bitmask_t & norecovery_mask)
{
    nMissing = m_pmap->getNCoords(); // init with nCoords and decrement as we find misses
    missPixel = false;
    missStrip = false;
    missing_mask = 0;
    norecovery_mask = 0;
    unsigned int wclayers = road.getWCLayers();
    for (unsigned layer = 0; layer < m_pmap->getNLogiLayers(); layer++)
    {
        int nHits = road.getHits(layer).size();
        if (nHits==0)
        {
            if (m_IdealCoordFitType == TrackCorrType::None && ((wclayers >> layer) & 1))
            {
                int ix = m_pmap->getCoordOffset(layer);
                int iy = ix + 1;
                if (m_pmap->isSCT(layer))
                {
                    missing_mask |= 1 << ix;
                    nMissing -= 1;
                }
                else
                {
                    missing_mask |= (1<<ix) | (1<<iy);
                    nMissing -= 2;
                }
            }
            else
            {
                if (m_pmap->isSCT(layer)) missStrip = true;
                else missPixel = true;
            }
        }
        else if (!((wclayers >> layer) & 1)) { // we have a hit
            int ix = m_pmap->getCoordOffset(layer);
            int iy = ix + 1;
            if (m_pmap->isSCT(layer))
	      {
                missing_mask |= 1 << ix;
                nMissing -= 1;
	      }
            else
	      {
                missing_mask |= (1<<ix) | (1<<iy);
                nMissing -= 2;
	      }

            // set the mask to avoid recovery in crowded planes
            if (m_norecovery_nhits != -1 && nHits > m_norecovery_nhits)
                norecovery_mask |= (1<<layer);
        }
    }
}



/**
 * Creates a list of track candidates by taking all possible combination of hits in road.
 * Sets basic ID info and hits.
 *
 * NB: If the number of combinations becomes large and memory is a concern,
 * it may be worth turning this function into a sort of iterator
 * over `combs`, return a single track each call. 
 */
void TrackFitter::makeTrackCandidates(const HTTRoad & road, const HTTTrack & temp, std::vector<HTTTrack>& track_cands)
{
    std::vector<std::vector<int>> combs = getComboIndices(road.getNHits_layer());
    track_cands.resize(combs.size(), temp);
    //
    //get the WC hits:
    layer_bitmask_t wcbits= road.getWCLayers();
    // Add the hits from each combination to the track, and set ID
    for (size_t icomb = 0; icomb < combs.size(); icomb++)
    {
      //Need to set the ID and the hits size of this track
        track_cands[icomb].setTrackID(m_idbase + icomb);
	    track_cands[icomb].setNLayers(m_pmap->getNLogiLayers());

        std::vector<int> const & hit_indices = combs[icomb]; // size nLayers
        for (unsigned layer = 0; layer < m_pmap->getNLogiLayers(); layer++)
        {
            if (hit_indices[layer] < 0) // Set a dummy hit if road has no hits in this layer
            {
                HTTHit newhit=HTTHit();
                newhit.setLayer(layer);
                newhit.setSection(0);
                if (m_pmap->getDim(layer) == 2) newhit.setDetType(SiliconTech::pixel);
	            else newhit.setDetType(SiliconTech::strip);

                if (wcbits & (1 << layer ) ) {
                    newhit.setHitType(HitType::wildcard);
		    newhit.setLayer(layer);
		}
                
                track_cands[icomb].setHTTHit(layer, newhit);
            }
            else
            {
                const HTTHit* hit = road.getHits(layer)[hit_indices[layer]];
                track_cands[icomb].setHTTHit(layer, *hit);               
            }
        }
    }

    m_idbase += combs.size();
}



/**
 * Given a N/N track that has a bad chi2, try to refit the track by taking away a single hit,
 * trying N combinations of N-1/N tracks. Returns the track with the best chi2 (or the original track
 * if none of the N-1/N tracks are an improvement).
 *
 * @param norecovery_mask - will skip removing hits from layers with bit set in this mask
 */
HTTTrack TrackFitter::recoverTrack(HTTTrack const & t, sector_t sector, layer_bitmask_t norecovery_mask, double qoverpt)
{
    m_nfits_rec += 1;

    std::vector<HTTTrack> recovered_tracks(m_pmap->getNLogiLayers(),t); // start with the complete track in all layers

    float best_chi2ndof = t.getChi2ndof();
    int best_i = -1;

    // Remove a hit from each layer and do a fit
    for (unsigned layer = (m_require_first ? 1 : 0); layer < recovered_tracks.size(); ++layer)
    {
        // Skip planes with multiple hits
        if (norecovery_mask & (1<<layer)) continue;
        m_nfits_addrec += 1;
        // Zero the cluster for the missing layer, make sure to set the number of dimensions for it
        
        HTTHit newhit=HTTHit();
        newhit.setLayer(layer);
        newhit.setSection(0);
        newhit.setHitType(HitType::guessed);
        recovered_tracks[layer].setHTTHit(layer,newhit);
        // Set the number of missing points and the related bitmask
        int ix = m_pmap->getCoordOffset(layer);

        unsigned int missing_mask = t.getHitMap();
        missing_mask &= ~(1 << ix);

        if (m_pmap->isPixel(layer)) missing_mask &= ~(1 << (ix + 1));

        recovered_tracks[layer].setHitMap(missing_mask);
        recovered_tracks[layer].setNMissing(t.getNMissing() + m_pmap->getDim(layer));

	// reset the pt
	recovered_tracks[layer].setQOverPt(qoverpt);
	recovered_tracks[layer].setTrackCorrType(m_IdealCoordFitType);


        // If not guessing do the fit using layer+1 since 1st set of constants are the 8/8
	if (m_guessinghits) m_nominalBank->linfit(sector, recovered_tracks[layer], m_do2ndStage);
	else m_droppedLayerBanks[layer]->linfit(sector, recovered_tracks[layer], m_do2ndStage);


        // Check if the chi2ndof is better
        if (recovered_tracks[layer].getChi2ndof() < best_chi2ndof)
        {
            best_chi2ndof = recovered_tracks[layer].getChi2ndof();
            best_i = layer;
        }
    }

    if (best_i >= 0)  {// a better track was found
        return recovered_tracks[best_i];
    } else
        return t;
}




// compute the best geant match, the barcode with the largest number of hits contributing to the track,
// and store in the track.
void TrackFitter::compute_truth(HTTTrack & t) const
{
    std::vector<HTTMultiTruth> mtv;

    for (unsigned layer = 0; layer < m_pmap->getNLogiLayers(); layer++)
    {
        if (t.getHitMap() & (1 << m_pmap->getCoordOffset(layer))) continue; // no hit in this plane
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


///////////////////////////////////////////////////////////////////////////////
// findTracks utility helper functions
///////////////////////////////////////////////////////////////////////////////

// Returns true if there is a fit in track_cands with chi2ndof < minchi2ndof
bool hasGoodFit(std::vector<HTTTrack> const & track_cands, float minchi2ndof)
{   
    for (HTTTrack const & t: track_cands)
        {
        if (t.getChi2ndof() > 0 && t.getChi2ndof() < minchi2ndof)
            return true;
        }   
    return false;
}