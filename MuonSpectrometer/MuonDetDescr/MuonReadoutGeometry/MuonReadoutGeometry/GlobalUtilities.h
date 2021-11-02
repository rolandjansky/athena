/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 global functions and stuff ...
 -----------------------------------------
 ***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_GLOBALUTILITIES_H
#define MUONREADOUTGEOMETRY_GLOBALUTILITIES_H

#include <string>

namespace MuonGM {
    std::string buildString(int i, int ncha);
    int strtoint(std::string_view str, unsigned int istart, unsigned int length);
    int stationPhiTGC(std::string_view stName, int fi, int zi_input, std::string_view geometry_version);
}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_GLOBALUTILITIES_H
