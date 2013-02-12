/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETCONDCFLOAT_P1_H
#define DETDESCRCOND_DETCONDCFLOAT_P1_H
//
// DetCondCFloat_p1.h - persistent class for DetCondCFloat
//
// RD Schaffer, started 23/02/12
//

#include <string>
#include <vector>

class DetCondCFloat_p1 {
 public:
    DetCondCFloat_p1() : m_size(0) {}

    int                       m_size;
    std::string               m_tag;
    std::vector<unsigned int> m_bufKeyValue;
    std::vector<float>        m_buf;
};

#endif // DETDESCRCOND_DETCONDCFLOAT_P1_H
