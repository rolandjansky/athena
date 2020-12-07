/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_TLP3_REC_H
#define TRACKPARTICLECONTAINER_TLP3_REC_H

//-----------------------------------------
// TrackParticle
//-----------------------------------------
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p2.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p3.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p2.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p2.h"

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

#include <ios>

namespace Rec
{
    class TrackParticleContainer_tlp3
    {

    public:

        TrackParticleContainer_tlp3(){}  

        TPCnvTokenList_p1		             m_tokenList;

//TrackParticle
        std::vector<Rec::TrackParticleContainer_p2 > m_trackParticleContainer;
        std::vector<Rec::TrackParticle_p3 >          m_trackParticle;

//TrkParameters   
        std::vector<Trk::TrackParameters_p2>      m_trackParameters;

//TrkTrackSummary
        std::vector<Trk::TrackSummary_p2>         m_trackSummaries;

//TrkSurface
        // std::vector< Trk::BoundSurface_p2 >                m_boundSurfaces;
        // std::vector< Trk::DetElementSurface_p1 >           m_detElementSurfaces; 

//TrkEventPrimitives
        std::vector<Trk::HepSymMatrix_p1>         m_hepSymMatrices;
    };
} 



#endif
