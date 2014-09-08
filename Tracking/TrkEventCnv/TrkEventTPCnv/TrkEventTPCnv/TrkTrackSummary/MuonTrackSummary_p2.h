/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_MUONTRACKSUMMARY_P2_H
#define TRKEVENTTPCNV_MUONTRACKSUMMARY_P2_H
#include <vector>

namespace Trk
{
    class MuonTrackSummary_p2
    {
    public:
        MuonTrackSummary_p2() :  m_nscatterers(0), m_npseudoMeasurements(0) {}

        unsigned int m_nscatterers; //<! number of scattering centres on the track
        unsigned int m_npseudoMeasurements; //<! number of pseudomeasurements on the track

       // for these, we have one entry per ChamberHitSummary (transient EDM)
        std::vector<bool> m_isMdt;      //<! is this a MDT chamber
                
        /** For each chamber we have 11 numbers, comprising of:
        - chamber identifier
        Then the (decomposed) first and second Projections
        - eta projection:
         - number of hits on track in the eta projection
         - number of holes in the eta projection
         - number of outliers in the eta projection
         - number of delta electrons in the eta projection (only filled for MDT chambers)
         - number of hits within a road around the track in the eta projection
        - phi projections for cluster chambers, first multi layer for mdt chambers:
         - number of hits on track in the eta projection
         - number of holes in the eta projection
         - number of outliers in the eta projection
         - number of delta electrons in the eta projection (only filled for MDT chambers)
         - number of hits within a road around the track in the eta projection
        */
        std::vector<int> m_data; 
    };
}

#endif
