/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_INDETTRACKSUMMARY_P1_H
#define TRKEVENTTPCNV_INDETTRACKSUMMARY_P1_H
#include <vector>

namespace Trk
{
    class InDetTrackSummary_p1
    {
    public:
        InDetTrackSummary_p1() : m_likelihoodspixeldedx(std::vector<float>()),m_massdedx(-1){}
       
        std::vector<float> m_likelihoodspixeldedx;
        float m_massdedx;
    };
}

#endif // TRACK_SUMMARY_P1_TRK_H
