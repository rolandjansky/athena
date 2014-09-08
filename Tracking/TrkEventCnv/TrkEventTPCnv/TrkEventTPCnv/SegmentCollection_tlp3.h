/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENT_COLLECTION_TLP3_TRK_H
#define SEGMENT_COLLECTION_TLP3_TRK_H


//-----------------------------------------------------------------------------
// TrkSegment
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSegment/SegmentCollection_p1.h"
#include "TrkEventTPCnv/TrkSegment/Segment_p1.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParameters_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
// #include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p2.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p2.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkSurfaces
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/Surface_p2.h"   
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurface_p1.h"

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace Trk
{
   class SegmentCollection_tlp3 
   {
  public:
     SegmentCollection_tlp3() {}
     
     TPCnvTokenList_p1                                  m_tokenList;

     std::vector< Trk::SegmentCollection_p1 >           m_segmentCollections;
     std::vector< Trk::Segment_p1 >                     m_segments;

     std::vector< Trk::Surface_p2 >                     m_surfaces;

     std::vector< Trk::FitQuality_p1 >                  m_fitQualities;
     std::vector< Trk::LocalParameters_p1 >             m_localParameters;
     std::vector< Trk::HepSymMatrix_p1 >                m_hepSymMatrices;

     std::vector< Trk::PseudoMeasurementOnTrack_p2 >    m_pseudoMeasurementOnTrack;
     std::vector< Trk::CompetingRIOsOnTrack_p1 >        m_competingRotsOnTrack; 
   };
}

#endif 

