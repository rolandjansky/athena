/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_SEGMENTCOLLECTIONCNVTLP1_H
#define TRK_SEGMENTCOLLECTIONCNVTLP1_H

//-----------------------------------------------------------------------------
//
// file:   SegmentCollectionCnv_tlp1.h
// @brief  Top Level converter for persistent Track Collection
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "SegmentCollection_tlp1.h"

#include "TrkEventTPCnv/TrkSegment/SegmentCollectionCnv_p1.h"         
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"                   
#include "TrkEventTPCnv/TrkSegment/TrackSegmentCnv_p1.h"                   

#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"            
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"                 
#include "TrkEventTPCnv/TrkSurfaces/CylinderBoundsCnv_p1.h"          
#include "TrkEventTPCnv/TrkSurfaces/DiamondBoundsCnv_p1.h"           
#include "TrkEventTPCnv/TrkSurfaces/DiscBoundsCnv_p1.h"              
#include "TrkEventTPCnv/TrkSurfaces/RectangleBoundsCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBoundsCnv_p1.h"     
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBoundsCnv_p1.h"             
#include "TrkEventTPCnv/TrkSurfaces/EllipseBoundsCnv_p1.h"     

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"              
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"         
#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3DCnv_p1.h"         

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"             
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p1.h"       
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

#include <iostream>

class SegmentCollectionCnv_tlp1
 : public AthenaPoolTopLevelTPConverter< SegmentCollectionCnv_p1, Trk::SegmentCollection_tlp1 >
{
public:

  SegmentCollectionCnv_tlp1();
  virtual ~SegmentCollectionCnv_tlp1();

  virtual void                  setPStorage( Trk::SegmentCollection_tlp1 *storage );

  // return list of tokens to extending persistent objects
  // it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1*    getTokenListVar() { return &(getTLPersObject()->m_tokenList); }


  // all TP converters for types used in the Track package
protected:
  SegmentCnv_p1                 m_segmentCnv;
  TrackSegmentCnv_p1            m_trackSegmentCnv;
  
  DiscSurfaceCnv_p1             m_discSurfacesCnv;
  PlaneSurfaceCnv_p1            m_planeSurfacesCnv;
  CylinderSurfaceCnv_p1         m_cylinderSurfacesCnv;
  StraightLineSurfaceCnv_p1     m_straightLineSurfacesCnv;
  SurfaceCnv_p1                 m_surfacesCnv;
  DetElementSurfaceCnv_p1       m_detElSurfCnv;
  
  CylinderBoundsCnv_p1          m_cylinderBoundsCnv;
  DiamondBoundsCnv_p1           m_diamondBoundsCnv;
  DiscBoundsCnv_p1              m_discBoundsCnv;
  RectangleBoundsCnv_p1         m_rectangleBoundsCnv;
  TrapezoidBoundsCnv_p1         m_trapesoidBoundsCnv;
  RotatedTrapezoidBoundsCnv_p1      m_rotatedTrapesoidBoundsCnv;
  EllipseBoundsCnv_p1               m_ellipseBoundsCnv;
  
  FitQualityCnv_p1              m_fitQualitiesCnv;
  CovarianceMatrixCnv_p1        m_covarianceMatricesCnv;
  ErrorMatrixCnv_p1             m_errorMatricesCnv;
  LocalParametersCnv_p1         m_localParametersCnv;            

  //trial and error for the datavector ...
  RIO_OnTrackCnv_p1             m_RIOsCnv;
  PseudoMeasurementOnTrackCnv_p1 m_pseudoMeasurementOnTrackCnv;
  CompetingRIOsOnTrackCnv_p1    m_crotCnv;
  
};

#endif // SEGMENT_COLLECTION_CNV_TLP1_TRK_H

