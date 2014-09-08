/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENT_COLLECTION_TLP1_TRK_H
#define SEGMENT_COLLECTION_TLP1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SegmentCollection_tlp1.h
// @brief  Top Level persistent object for SegmentCollection
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TrkSegment
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSegment/SegmentCollection_p1.h"
#include "TrkEventTPCnv/TrkSegment/Segment_p1.h"
#include "TrkEventTPCnv/TrkSegment/TrackSegment_p1.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParameters_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p1.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"

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
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBounds_p1.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurface_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/EllipseBounds_p1.h"     

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace Trk
{
   class SegmentCollection_tlp1 
   {
  public:
     SegmentCollection_tlp1() {}
     
     TPCnvTokenList_p1                                  m_tokenList;

     std::vector< Trk::SegmentCollection_p1 >           m_segmentCollections;
     std::vector< Trk::Segment_p1 >                     m_segments;
     std::vector< Trk::TrackSegment_p1 >                m_tracksegments;

     std::vector< Trk::BoundSurface_p1 >                m_boundSurfaces;
     std::vector< Trk::Surface_p1 >                     m_surfaces;
     std::vector< Trk::CylinderBounds_p1 >              m_cylinderBounds;
     std::vector< Trk::DiamondBounds_p1 >               m_diamondBounds;
     std::vector< Trk::DiscBounds_p1 >                  m_discBounds;
     std::vector< Trk::RectangleBounds_p1 >             m_rectangleBounds;
     std::vector< Trk::TrapezoidBounds_p1 >             m_trapesoidBounds;
     std::vector< Trk::RotatedTrapezoidBounds_p1 >      m_rotatedTrapesoidBounds;
     std::vector< Trk::EllipseBounds_p1 >               m_ellipseBounds;

     std::vector< Trk::FitQuality_p1 >                  m_fitQualities;
     std::vector< Trk::LocalParameters_p1 >             m_localParameters;
     std::vector< Trk::HepSymMatrix_p1 >                m_hepSymMatrices;

     std::vector< Trk::RIO_OnTrack_p1 >                 m_RIO_OnTrack;
     std::vector< Trk::PseudoMeasurementOnTrack_p1 >    m_pseudoMeasurementOnTrack;
     std::vector< Trk::CompetingRIOsOnTrack_p1 >        m_competingRotsOnTrack; 
     
     std::vector< Trk::DetElementSurface_p1 >           m_detElementSurfaces; 

   };
}

#endif // SEGMENT_COLLECTION_TLP1_TRK_H

