// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTRoadFilterTool.h
 * @author Tomasz Bold - toold to filter rods & hits for LLP
 * @date 2021
 * @brief Declares a concrete class that is directly used the HTT main algorithm to filter hits belonging to already found roads
 */
#ifndef HTT_LLPROADFILTERTOOL_H
#define HTT_LLPROADFILTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTHit.h"

#include <string>
#include <vector>

class HTTLLPRoadFilterTool : public AthAlgTool {
public:
  HTTLLPRoadFilterTool (const std::string& algname, const std::string& name, const IInterface* ifc);

  StatusCode filterUsedHits(std::vector<HTTTrack> &tracks, 
			    const std::vector<const HTTHit*>& allHits, 
			    std::vector<const HTTHit*>& unusedHits );

private:
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

};

#endif
