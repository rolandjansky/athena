/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <map>
#include <utility>


#ifdef __CINT__
#ifndef HTT_STANDALONE
#define HTT_STANDALONE
#endif
// unimplemented in rel 22
// #pragma link off globals;
// #pragma link off classes;
// #pragma link off functions;


#pragma link C++ class HTTEventInfo+;
#pragma link C++ class HTTOptionalEventInfo+;
#pragma link C++ class HTTEventInputHeader+;
#pragma link C++ class HTTLogicalEventInputHeader+;
#pragma link C++ class HTTLogicalEventOutputHeader+;
#pragma link C++ class HTTTowerInputHeader+;
#pragma link C++ class std::vector<HTTTowerInputHeader>+;
#pragma link C++ class HTTDataFlowInfo+;

#pragma link C++ class HTTOfflineTrack+;
#pragma link C++ class std::vector<HTTOfflineTrack>+;
#pragma link C++ class HTTOfflineHit+;
#pragma link C++ class std::vector<HTTOfflineHit>+;

#pragma link C++ class HTTCluster+;
#pragma link C++ class std::vector<HTTCluster*>+;
#pragma link C++ class std::vector<HTTCluster>+;

#pragma link C++ class HTTHit+;
#pragma link C++ class std::vector<HTTHit>+;
#pragma link C++ class std::vector<HTTHit*>+;
#pragma link C++ class std::vector<std::vector<const HTTHit*>>+;

#pragma link C++ class HTTRoad+;
#pragma link C++ class HTTMultiTruth+;
#pragma link C++ class HTTMatchInfo+;

#pragma link C++ class HTTTrack+;
#pragma link C++ class std::vector<HTTTrack>+;

#pragma link C++ class HTTTruthTrack+;
#pragma link C++ class std::vector<HTTTruthTrack>+;

#pragma link C++ class HTTTrackStream+;
#endif
