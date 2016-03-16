/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_MUONTRACKSUMMARY_P1_H
#define TRKEVENTTPCNV_MUONTRACKSUMMARY_P1_H
#include <vector>

namespace Trk
{
    class MuonTrackSummary_p1
    {
    public:
        MuonTrackSummary_p1() :  m_nscatterers(0), m_npseudoMeasurements(0) {}

        unsigned int m_nscatterers; //<! number of scattering centres on the track
        unsigned int m_npseudoMeasurements; //<! number of pseudomeasurements on the track

       // for these, we have one entry per ChamberHitSummary (transient EDM)
        std::vector<unsigned int> m_chId; //<! chamber identifier
        std::vector<bool> m_isMdt;      //<! is this a MDT chamber
        
        // These represent the (decomposed) first and second Projections
        
        //eta projection
        std::vector<int> nEtahits;      //<! number of hits on track in the eta projection
        std::vector<int> nEtaholes;     //<! number of holes in the eta projection
        std::vector<int> nEtaoutliers;  //<! number of outliers in the eta projection
        std::vector<int> nEtadeltas;    //<! number of delta electrons in the eta projection (only filled for MDT chambers)
        std::vector<int> nEtacloseHits; //<! number of hits within a road around the track in the eta projection

       //phi projections for cluster chambers, first multi layer for mdt chambers
        std::vector<int> nPhihits;      //<! number of hits on track in the eta projection
        std::vector<int> nPhiholes;     //<! number of holes in the eta projection
        std::vector<int> nPhioutliers;  //<! number of outliers in the eta projection
        std::vector<int> nPhideltas;    //<! number of delta electrons in the eta projection (only filled for MDT chambers)
        std::vector<int> nPhicloseHits; //<! number of hits within a road around the track in the eta projection
    };
}

#endif 

