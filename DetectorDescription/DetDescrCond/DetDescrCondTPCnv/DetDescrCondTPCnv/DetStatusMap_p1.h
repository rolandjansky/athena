/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETSTATUSMAP_P1_H
#define DETDESCRCOND_DETSTATUSMAP_P1_H
//
// DetStatusMap_p1.h - persistent class for DetStatusMap
//
// RD Schaffer, started 23/02/12
//
#include <string>
#include <vector>

class DetStatusMap_p1 {
public:
    DetStatusMap_p1() {}

    std::vector<std::string>  m_keys;
    std::vector<int>          m_codes;
    std::vector<float>        m_deadfrac_thrust;
};

#endif // DETDESCRCOND_DETSTATUSMAP_P1_H
