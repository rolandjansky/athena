/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 global functions and stuff ...
 -----------------------------------------
 ***************************************************************************/

#ifndef MUONREADOUTGEOMETRY_GLOBALUTILITIES_H
#define MUONREADOUTGEOMETRY_GLOBALUTILITIES_H

#include <string>

namespace MuonGM 
{
    std::string buildString(int i, int ncha);
    int strtoint(const std::string& str, unsigned int istart, unsigned int length);
    int stationPhiTGC(const std::string& stName, int fi, int zi_input, const std::string& geometry_version);
}

#endif // MUONREADOUTGEOMETRY_GLOBALUTILITIES_H
