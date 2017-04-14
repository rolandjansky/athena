/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <map>
#include <utility>


#ifdef __CINT__
#ifndef FTK_STANDALONE
#define FTK_STANDALONE
#endif
#pragma link off globals;
#pragma link off classes;
#pragma link off functions;

#pragma link C++ class FTKPattern+;
#pragma link C++ class std::vector<FTKPattern>+; 
#pragma link C++ class FTKCoord+;
#pragma link C++ class FTKHit+;
#pragma link C++ class std::vector<FTKHit>+;
#pragma link C++ class std::vector<FTKHit*>+;
#pragma link C++ class FTKRawHit+;
#pragma link C++ class std::vector<FTKRawHit>+;
#pragma link C++ class FTKRoad+;
#pragma link C++ class MultiTruth+;
#pragma link C++ class FTKSS+;
#pragma link C++ class std::vector< std::map<int,FTKSS> >+;
#if ROOT_VERSION_CODE<334848
#pragma link C++ class std::pair<long int,long int>+;
#endif
#pragma link C++ class std::vector<TBits>+;
#pragma link C++ class std::vector<FTKHit>+; 
#pragma link C++ class std::vector<FTKSS>+; 
#pragma link C++ class FTKRoadStream+;
#pragma link C++ class FTKTrack+;
#pragma link C++ class std::vector<FTKTrack>+; 
#pragma link C++ class FTKTrackStream+;
#pragma link C++ class FTKTruthTrack+;
#pragma link C++ class std::vector<FTKTruthTrack>+;
#endif
