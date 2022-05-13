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
  HTTLLPRoadFilterTool() = default;

  static const InterfaceID& interfaceID();

  StatusCode filterUsedHits(std::vector<HTTTrack> &tracks, 
                          const std::vector<const HTTHit*>& allHits, 
                          std::vector<const HTTHit*>& unusedHits );

private:

};

#endif
