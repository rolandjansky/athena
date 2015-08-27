/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_TRKEVENTTPCNVDICT_H
#define TRKEVENTTPCNV_TRKEVENTTPCNVDICT_H


#include "TrkEventTPCnv/TrackCollection_tlp1.h"
#include "TrkEventTPCnv/TrackCollection_tlp2.h"
#include "TrkEventTPCnv/TrackCollection_tlp3.h"
#include "TrkEventTPCnv/TrackCollection_tlp4.h"
#include "TrkEventTPCnv/TrackCollection_tlp5.h"
#include "TrkEventTPCnv/VxContainer_tlp1.h"
#include "TrkEventTPCnv/VxContainer_tlp2.h"
#include "TrkEventTPCnv/MVFVxContainer_tlp1.h"
#include "TrkEventTPCnv/V0Container_tlp1.h"
#include "TrkEventTPCnv/V0Container_tlp2.h"
#include "TrkEventTPCnv/SegmentCollection_tlp3.h"
#include "TrkEventTPCnv/SegmentCollection_tlp2.h"
#include "TrkEventTPCnv/SegmentCollection_tlp1.h"

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCollection_p1.h"
#include "TrkEventTPCnv/TrkTrack/Track_p1.h"
#include "TrkEventTPCnv/TrkTrack/Track_p12.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p1.h"
#include "TrkEventTPCnv/TrkTrack/Track_p2.h"
#include "TrkEventTPCnv/TrkTrack/Track_p4.h"
#include "TrkEventTPCnv/TrkTrack/TrackInfo_p1.h"

//-----------------------------------------------------------------------------
// TrkPseudoMeasurementOnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p1.h"
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/InDetTrackSummary_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummary_p1.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummary_p2.h"

#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p2.h"

//-----------------------------------------------------------------------------
// TrkSegment
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSegment/SegmentCollection_p1.h"
#include "TrkEventTPCnv/TrkSegment/Segment_p1.h"
#include "TrkEventTPCnv/TrkSegment/TrackSegment_p1.h"

//-----------------------------------------------------------------------------
// TrkMaterialOnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkMaterialOnTrack/ScatteringAngleOnTrack_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrack_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBase_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrack_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLoss_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirection_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParameters_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkSurfaces
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/Surface_p2.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurface_p1.h"

#include "TrkEventTPCnv/TrkSurfaces/CylinderBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiamondBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiscBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RectangleBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/TriangleBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/EllipseBounds_p1.h"

//-----------------------------------------------------------------------------
// TrkParameters (also used by TrackNeutralParameters)
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p2.h"
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"
// #include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p2.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p1.h"
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p2.h"

//-----------------------------------------------------------------------------
// TrkCompetingRioOnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"


//-----------------------------------------------------------------------------
// TrkPrepRawData
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"


#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3D_p1.h"

//-----------------------------------------------------------------------------
// VxVertex
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p2.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p2.h"
#include "TrkEventTPCnv/VxVertex/VxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/ExtendedVxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/VxContainer_p1.h"

//-----------------------------------------------------------------------------
// VxMultiVertex
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/VxMultiVertex/MVFVxTrackAtVertex_p1.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxCandidate_p1.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxContainer_p1.h"

//-----------------------------------------------------------------------------
// TrkV0Vertex
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkV0Vertex/V0Hypothesis_p1.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Candidate_p1.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Container_p1.h"

//-----------------------------------------------------------------------------
// TrkParticleBase
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParticleBase/TrackParticleBase_p1.h"

//namespace TrackEventTPCnvDict 

struct TrackEventTPCnvDict 
{
    std::vector< Trk::TrackCollection_p1 >              m_v1;
    std::vector< Trk::Track_p1 >                        m_v2;
    std::vector< Trk::Track_p12 >                       m_v2AB;
    std::vector< Trk::Track_p2 >                        m_v2b;
    std::vector< Trk::TrackInfo_p1 >                    m_v2c;
    std::vector< Trk::ScatteringAngleOnTrack_p1 >       m_v3;
    std::vector< Trk::TrackStateOnSurface_p1 >          m_v4;

    std::vector< Trk::RIO_OnTrack_p1 >                  m_v5;
    std::vector< Trk::RIO_OnTrack_p2 >                  m_v51;
    std::vector< Trk::PrepRawData_p1 >                  m_v6;

    std::vector< Trk::TrackParameters_p1 >              m_v7;
    std::vector< Trk::TrackParameters_p2 >              m_v7a;
    std::vector< Trk::AtaSurface_p1 >                   m_v8;
    std::vector< Trk::MeasuredAtaSurface_p1 >           m_v9;
    std::vector< Trk::Perigee_p1 >                      m_v10;
    std::vector< Trk::Perigee_p2 >                      m_v10A;
    std::vector< Trk::MeasuredPerigee_p1 >              m_v11;
     
    std::vector< Trk::BoundSurface_p1 >                 m_v12;
    std::vector< Trk::Surface_p1 >                      m_v13;
    std::vector< Trk::Surface_p1 >                      m_v13a;
    std::vector< Trk::CylinderBounds_p1 >               m_v14;
    std::vector< Trk::DiamondBounds_p1 >                m_v15;
    std::vector< Trk::DiscBounds_p1 >                   m_v16;
    std::vector< Trk::RectangleBounds_p1 >              m_v17;
    std::vector< Trk::TrapezoidBounds_p1 >              m_v18;
    std::vector< Trk::RotatedTrapezoidBounds_p1 >       m_v18b;
    std::vector< Trk::TriangleBounds_p1 >               m_v18c;
    std::vector< Trk::EllipseBounds_p1 >                m_v18d;

    std::vector< Trk::FitQuality_p1 >                   m_v19;
    std::vector< Trk::HepSymMatrix_p1 >                 m_v20;
    std::vector< Trk::MaterialEffectsOnTrack_p1 >       m_v21;
    std::vector< Trk::LocalDirection_p1 >               m_v22;
    std::vector< Trk::LocalPosition_p1 >                m_v23;
    std::vector< Trk::LocalParameters_p1 >              m_v24;
    
    std::vector< Trk::Vertex_p1 >                       m_v25;
    std::vector< Trk::RecVertex_p1 >                    m_v26;
    std::vector< Trk::RecVertex_p2 >                    m_v26a;
    std::vector< Trk::VxTrackAtVertex_p1 >              m_v27;
    std::vector< Trk::VxTrackAtVertex_p2 >              m_v27a;
    std::vector< Trk::VxCandidate_p1 >                  m_v28;
    std::vector< Trk::ExtendedVxCandidate_p1 >          m_v29;
    std::vector< Trk::VxContainer_p1 >                  m_v30;

    std::vector< Trk::MVFVxTrackAtVertex_p1 >           m_v31;
    std::vector< Trk::MVFVxCandidate_p1 >               m_v32;
    std::vector< Trk::MVFVxContainer_p1 >               m_v33;
   
    std::vector< Trk::Segment_p1>                       m_v34;
    std::vector< Trk::TrackSegment_p1>                  m_v35;
    std::vector< Trk::SegmentCollection_p1>             m_v36;

    std::vector< Trk::V0Hypothesis_p1 >                 m_v37;
    std::vector< Trk::V0Candidate_p1 >                  m_v38; 
    std::vector< Trk::V0Container_p1 >                  m_v39; 

    std::vector< Trk::PseudoMeasurementOnTrack_p1 >     m_v40;
    std::vector< Trk::PseudoMeasurementOnTrack_p2 >     m_v40a;
    
    std::vector< Trk::TrackSummary_p1 >                 m_v41;
    std::vector< Trk::TrackSummary_p2 >                 m_v41ilija;
    std::vector< Trk::InDetTrackSummary_p1 >            m_v41b;
    std::vector< Trk::MuonTrackSummary_p1 >             m_v41a;
    std::vector< Trk::MuonTrackSummary_p2 >             m_v41c;
    
    std::vector< Trk::TrackParticleBase_p1 >            m_v42;
    
    std::vector< Trk::CompetingRIOsOnTrack_p1 >         m_v44;
    std::vector< Trk::TrackStateOnSurface_p2 >          m_v45;
    std::vector< Trk::MaterialEffectsOnTrack_p2 >       m_v46;
    std::vector< Trk::MaterialEffectsBase_p1 >          m_v47;
    std::vector< Trk::EnergyLoss_p1 >                   m_v48;
    std::vector< Trk::EstimatedBremOnTrack_p1 >         m_v49;
    std::vector< Trk::VertexOnTrack_p1 >                m_v50;
    std::vector< Trk::Track_p4 >                        m_v52;
};

#endif // TRKEVENTTPCNV_H
