/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTTPCNV_TRKEVENTTPCNVDICT_H
#define TRKEVENTTPCNV_TRKEVENTTPCNVDICT_H

#include "TrkEventTPCnv/VxContainer_tlp2.h"
#include "TrkEventTPCnv/MVFVxContainer_tlp1.h"
#include "TrkEventTPCnv/V0Container_tlp2.h"
#include "TrkEventTPCnv/SegmentCollection_tlp3.h"

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCollection_p1.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p3.h"
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
#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p2.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p2.h"

//-----------------------------------------------------------------------------
// TrkCompetingRioOnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkPrepRawData
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"

//-----------------------------------------------------------------------------
// VxVertex
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p2.h"
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

#include "TrkEventTPCnv/TrkClusterSplitProbabilityContainer/ClusterSplitProbabilityContainer_p1.h"

#endif // TRKEVENTTPCNV_H
