/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericTGCCache_H
#define GenericTGCCache_H
#include <string>
#include <vector>
namespace MuonGM {

    struct GenericTGCCache {
        double frame_h{0.};
        double frame_ab{0.};
        int nlayers{0};
        std::vector<std::string> materials;
        std::vector<double> positions;
        std::vector<double> tck;
    };

}  // namespace MuonGM
#endif
