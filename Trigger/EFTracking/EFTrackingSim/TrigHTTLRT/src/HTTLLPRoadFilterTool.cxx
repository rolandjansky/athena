// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "TrigHTTLRT/HTTLLPRoadFilterTool.h"

HTTLLPRoadFilterTool::HTTLLPRoadFilterTool (const std::string& algname, const std::string& name, const IInterface* ifc) 
  : AthAlgTool(algname, name, ifc) {}


StatusCode HTTLLPRoadFilterTool::filterUsedHits( std::vector<HTTTrack> &tracks, 
                                              const std::vector<const HTTHit*>& allHits, 
                                              std::vector<const HTTHit*>& unusedHits ) {
    std::vector<HTTHit> hitsInTracksVec;
    for (const auto& track : tracks) {
        for (const HTTHit& hit : track.getHTTHits()) {
            if (hit.isReal()) {
                hitsInTracksVec.push_back(hit);
            }
        }
    }
    std::set<const HTTHit*, HitCompare > hitsInTracks;
    for ( auto& hit : hitsInTracksVec) {
        hitsInTracks.insert(&hit);
    }

    ATH_MSG_DEBUG("Number of hits from tracks " <<  hitsInTracks.size());

    std::set<const HTTHit*, HitCompare> orderedHits;
    for ( auto hit: allHits){
        if ( hit->isReal()) {
            orderedHits.insert(hit);
        }
    }
    ATH_MSG_DEBUG("Number of all hits " << orderedHits.size() << " " << allHits.size() );


    std::set_difference( orderedHits.begin(), orderedHits.end(), 
                         hitsInTracks.begin(), hitsInTracks.end(), 
                         std::back_inserter(unusedHits), 
                         HitCompare() );

   return StatusCode::SUCCESS;
}
