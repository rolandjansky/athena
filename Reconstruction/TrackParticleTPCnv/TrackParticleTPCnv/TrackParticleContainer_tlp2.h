/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_TLP2_REC_H
#define TRACKPARTICLECONTAINER_TLP2_REC_H

//-----------------------------------------
// TrackParticle
//-----------------------------------------
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p2.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p2.h"

#include "TrkEventTPCnv/TrkParticleBase/TrackParticleBase_p1.h"

//-----------------------------------------------------------------------------
// TrkSurfaces
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurface_p1.h"

#include "TrkEventTPCnv/TrkSurfaces/CylinderBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiamondBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedDiamondBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiscBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RectangleBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBounds_p1.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurface_p1.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p2.h"
//-----------------------------------------------------------------------------
// TrkTrack (for EL?)
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/Track_p1.h"

//-----------------------------------------------------------------------------
// VxCandidate (for EL?)
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/VxVertex/VxCandidate_p1.h"

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

//------------------------------------------------------------------------------
//  TrackInfo
//------------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackInfo_p1.h"

#include <ios>

// next line can be removed as soon as TPCnvTokenList_p1.h is fixed (i.e. includes string)
#include <string>

#include <ios>

namespace Rec
{
    class TrackParticleContainer_tlp2
    {

    public:

        TrackParticleContainer_tlp2(){}  

        TPCnvTokenList_p1		             m_tokenList;

//TrackParticle
        std::vector<Rec::TrackParticleContainer_p2 > m_trackParticleContainer_p2;
        std::vector<Rec::TrackParticle_p2 >          m_trackParticle_p2;

//TrkParameters   
        std::vector<Trk::TrackParameters_p1>      m_trackParameters;
        std::vector<Trk::AtaSurface_p1 >          m_ataSurfaces;
        std::vector<Trk::MeasuredAtaSurface_p1 >  m_measuredAtaSurfaces;
        std::vector<Trk::Perigee_p1>              m_perigees;
        std::vector<Trk::MeasuredPerigee_p1>      m_measPerigees;

//TrkTrackSummary
        std::vector<Trk::TrackSummary_p2>         m_trackSummaries;

//TrkSurface
        std::vector< Trk::BoundSurface_p1 >                m_boundSurfaces;
        std::vector< Trk::Surface_p1 >                     m_surfaces;
        std::vector< Trk::CylinderBounds_p1 >              m_cylinderBounds;
        std::vector< Trk::DiamondBounds_p1 >               m_diamondBounds;
        std::vector< Trk::RotatedDiamondBounds_p1 >        m_rotatedDiamondBounds;
        std::vector< Trk::DiscBounds_p1 >                  m_discBounds;
        std::vector< Trk::RectangleBounds_p1 >             m_rectangleBounds;
        std::vector< Trk::TrapezoidBounds_p1 >             m_trapesoidBounds;
        std::vector< Trk::RotatedTrapezoidBounds_p1 >      m_rotatedTrapesoidBounds;
        std::vector< Trk::DetElementSurface_p1 >           m_detElementSurfaces; 

//TrkEventPrimitives
        std::vector<Trk::HepSymMatrix_p1>         m_hepSymMatrices;
    };
} 



#endif
