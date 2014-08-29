/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_CHAMBERT0S_P1_H
#define MUONEVENTTPCNV_CHAMBERT0S_P1_H
#include <map>
#include <vector>

namespace Muon
{
    class ChamberT0s_p1
    {
    public:
        ChamberT0s_p1() {}
        std::vector< std::pair < unsigned int, float > > m_t0s;
    };
}

#endif
