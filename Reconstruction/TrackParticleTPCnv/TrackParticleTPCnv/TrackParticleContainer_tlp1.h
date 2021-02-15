/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_TLP1_REC_H
#define TRACKPARTICLECONTAINER_TLP1_REC_H

//-----------------------------------------
// TrackParticle
//-----------------------------------------
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p1.h"
#include "TrackParticleTPCnv/Particle/TrackParticle_p1.h"

// REMOVE LATER?
//-----------------------------------------
// TrkParticleBase
//-----------------------------------------
// #include "TrkEventTPCnv/TrkParticleBase/TrackParticleBaseCollection_p1.h"
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
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/InDetTrackSummary_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummary_p1.h"

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
// REMOVE LATER END

// next line can be removed as soon as TPCnvTokenList_p1.h is fixed (i.e. includes string)
#include <string>

#include <ios>

namespace Rec
{
 class TrackParticleContainer_tlp1 
 {
 
  public:
  
   TrackParticleContainer_tlp1(){}  
   
   TPCnvTokenList_p1		             m_tokenList;

//TrackParticle
   std::vector<Rec::TrackParticleContainer_p1 > m_trackParticleContainer;
   std::vector<Rec::TrackParticle_p1 >          m_trackParticle;
 
// REMOVE LATER?
//TrkParticleBase
//    std::vector<Trk::TrackParticleBaseCollection_p1 >    m_trackParticleBaseCollection;
   std::vector<Trk::TrackParticleBase_p1 >   m_trackParticleBase;

// TrkTrack and Vx 
   std::vector<Trk::Track_p1>                m_tracks;
   std::vector<Trk::VxCandidate_p1>          m_vxCandidates;
   
//TrkParameters   
   std::vector<Trk::TrackParameters_p1>      m_trackParameters;
   std::vector<Trk::AtaSurface_p1 >          m_ataSurfaces;
   std::vector<Trk::MeasuredAtaSurface_p1 >  m_measuredAtaSurfaces;
   std::vector<Trk::Perigee_p1>              m_perigees;
   std::vector<Trk::MeasuredPerigee_p1>      m_measPerigees;

//TrkTrackSummary
   std::vector<Trk::TrackSummary_p1>         m_trackSummaries;
   std::vector<Trk::InDetTrackSummary_p1>    m_indetTrackSummaries;
   std::vector<Trk::MuonTrackSummary_p1>     m_muonTrackSummaries;

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
   std::vector<Trk::FitQuality_p1>           m_fitQualities;
   std::vector<Trk::HepSymMatrix_p1>         m_hepSymMatrices;
// REMOVE LATER END 

   std::vector< Trk::TrackInfo_p1 >                   m_trackInfos;
 
 };//end of class definitions
} //end of namespace definitions



#endif //TRACKPARTICLECONTAINER_TLP1_REC_H
