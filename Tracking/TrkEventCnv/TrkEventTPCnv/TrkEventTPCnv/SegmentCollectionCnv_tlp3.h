/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_SEGMENTCOLLECTIONCNV_TLP3_H
#define TRK_SEGMENTCOLLECTIONCNV_TLP3_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "SegmentCollection_tlp3.h"

#include "TrkEventTPCnv/TrkSegment/SegmentCollectionCnv_p1.h"         
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"                   
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"                 
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"              
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"         

// #include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p2.h"             
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"       
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

#include <iostream>

class SegmentCollectionCnv_tlp3
 : public AthenaPoolTopLevelTPConverter< SegmentCollectionCnv_p1, Trk::SegmentCollection_tlp3 >
{
public:

  SegmentCollectionCnv_tlp3();
  virtual ~SegmentCollectionCnv_tlp3();

  virtual void                  setPStorage( Trk::SegmentCollection_tlp3 *storage );

  // return list of tokens to extending persistent objects
  // it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1*    getTokenListVar() { return &(getTLPersObject()->m_tokenList); }


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
};

#endif 

