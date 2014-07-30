/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GenericTGCCache_H
#define GenericTGCCache_H 

namespace MuonGM {

struct GenericTGCCache {
   double frame_h;
   double frame_ab;
   int nlayers;
   std::vector<std::string> materials;
   std::vector<double> positions;
   std::vector<double> tck;
};

}
#endif
