/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_TLP1_TRK_H
#define TRACK_COLLECTION_TLP1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollection_tlp1.h
// @brief  Top Level persistent object for Track Collection
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCollection_p1.h"
#include "TrkEventTPCnv/TrkTrack/Track_p1.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p1.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirection_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParameters_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkMaterialOnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrack_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/ScatteringAngleOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkSurfaces
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurface_p1.h"

#include "TrkEventTPCnv/TrkSurfaces/CylinderBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiamondBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiscBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RectangleBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBounds_p1.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p1.h"
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p1.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkPrepRawData
//-----------------------------------------------------------------------------
// #include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"


#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"
//#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3D_p1.h"

namespace Trk
{
   class TrackCollection_tlp1 
   {
  public:
     TrackCollection_tlp1() {}
     
     TPCnvTokenList_p1                                  m_tokenList;

     std::vector< Trk::TrackCollection_p1 >             m_trackCollections;
     std::vector< Trk::Track_p1 >                       m_tracks;
     std::vector< Trk::ScatteringAngleOnTrack_p1 >      m_scatterAngles;		
     std::vector< Trk::TrackStateOnSurface_p1 >         m_trackStates;	

     std::vector< Trk::RIO_OnTrack_p1 >                 m_RIOs;
     std::vector< Trk::PseudoMeasurementOnTrack_p1 >    m_pseudoMeasurementOnTrack;	
     std::vector< Trk::CompetingRIOsOnTrack_p1 >        m_competingRotsOnTrack;	

     std::vector< Trk::TrackParameters_p1 >             m_parameters;
     std::vector< Trk::AtaSurface_p1 >                  m_ataSurfaces;
     std::vector< Trk::MeasuredAtaSurface_p1 >          m_measuredAtaSurfaces;
     std::vector< Trk::Perigee_p1 >                     m_perigees;
     std::vector< Trk::MeasuredPerigee_p1 >             m_measuredPerigees;
     
     std::vector< Trk::BoundSurface_p1 >                m_boundSurfaces;
     std::vector< Trk::Surface_p1 >                     m_surfaces;
     std::vector< Trk::CylinderBounds_p1 >              m_cylinderBounds;
     std::vector< Trk::DiamondBounds_p1 >               m_diamondBounds;
     std::vector< Trk::DiscBounds_p1 >                  m_discBounds;
     std::vector< Trk::RectangleBounds_p1 >             m_rectangleBounds;
     std::vector< Trk::TrapezoidBounds_p1 >             m_trapesoidBounds;

     std::vector< Trk::FitQuality_p1 >                  m_fitQualities;
     std::vector< Trk::HepSymMatrix_p1 >                m_hepSymMatrices;
     std::vector< Trk::MaterialEffectsOnTrack_p1 >      m_materialEffects;
     std::vector< Trk::LocalDirection_p1 >              m_localDirections;
     std::vector< Trk::LocalPosition_p1 >               m_localPositions;
     std::vector< Trk::LocalParameters_p1 >             m_localParameters;

     //std::vector< HepPoint3D_p1 >			m_points;     

   };
}

#endif // TRACK_COLLECTION_P1_TRK_H

