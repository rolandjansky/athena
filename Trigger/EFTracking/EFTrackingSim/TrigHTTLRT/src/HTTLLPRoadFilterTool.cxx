// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "HTTLLPRoadFilterTool.h"

HTTLLPRoadFilterTool::HTTLLPRoadFilterTool (const std::string& algname, const std::string& name, const IInterface* ifc) 
    : AthAlgTool(algname, name, ifc) {}


StatusCode HTTLLPRoadFilterTool::filterUsedHits( std::vector<HTTTrack> &tracks, 
                                              const std::vector<const HTTHit*>& allHits, 
                                              std::vector<const HTTHit*>& unusedHits ) {
    // Make a list of all hits used in any track
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

    std::vector<HTTHit> hitsInTracksVec;
    for (auto track : tracks) {
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
