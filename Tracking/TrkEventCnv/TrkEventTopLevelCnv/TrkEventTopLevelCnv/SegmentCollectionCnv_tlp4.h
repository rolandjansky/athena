/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_SEGMENTCOLLECTIONCNV_TLP4_H
#define TRK_SEGMENTCOLLECTIONCNV_TLP4_H

#include "TPTools/TopLevelTPConverter.h"

#include "SegmentCollection_tlp4.h"

#include "TrkEventTPCnv/TrkSegment/SegmentCollectionCnv_p1.h"
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"       
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp2.h"
#include "InDetEventTPCnv/InDetTrackCnv_tlp2.h"

typedef TopLevelTPConverter< SegmentCollectionCnv_p1, Trk::SegmentCollection_tlp4 > BaseSegmentCollectionCnv;

class SegmentCollectionCnv_tlp4 : public BaseSegmentCollectionCnv
{
 public:
  SegmentCollectionCnv_tlp4();

  virtual void  setPStorage( PERS *storage );
  virtual PERS* createPersistent(const TRANS* transObj, MsgStream &log);

  // all TP converters for types used in the Track package
protected:
  SegmentCnv_p1                 m_segmentCnv;
  
  ConeSurfaceCnv_p2             m_coneSurfacesCnv       ;
  CylinderSurfaceCnv_p2         m_cylSurfacesCnv        ;
  DiscSurfaceCnv_p2             m_discSurfacesCnv       ;
  PerigeeSurfaceCnv_p2          m_perigeeSurfacesCnv    ;
  PlaneSurfaceCnv_p2            m_planeSurfacesCnv      ;
  StraightLineSurfaceCnv_p2     m_lineSurfacesCnv       ;
  SaggedLineSurfaceCnv_p2       m_saggedLineSurfacesCnv ;
  
  FitQualityCnv_p1              m_fitQualitiesCnv;
  CovarianceMatrixCnv_p1        m_covarianceMatricesCnv;
  ErrorMatrixCnv_p1             m_errorMatricesCnv;
  LocalParametersCnv_p1         m_localParametersCnv;            

  PseudoMeasurementOnTrackCnv_p2 m_pseudoMeasurementOnTrackCnv;
  CompetingRIOsOnTrackCnv_p1    m_crotCnv;

  MuonMeasurementsCnv_tlp2      m_muonMeasurementsCnv;
  InDetTrackCnv_tlp2            m_inDetTrackCnv;
};

#endif 

