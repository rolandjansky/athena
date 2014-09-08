/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_CNV_TLP5_TRK_H
#define TRACK_COLLECTION_CNV_TLP5_TRK_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackCollection_tlp5.h"

#include "TrkEventTPCnv/TrkTrack/TrackCollectionCnv_p4.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p4.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"  

#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"  
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrackCnv_p1.h"             

#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"      
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p2.h" // Also takes care of surfaces now, for these objects

#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"                 
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"          
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"           
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"         

#include <iostream>


class TrackCollectionCnv_tlp5
 : public AthenaPoolTopLevelTPConverter< TrackCollectionCnv_p4, Trk::TrackCollection_tlp5 >
{
 public:
 
 TrackCollectionCnv_tlp5();
  virtual ~TrackCollectionCnv_tlp5() {}

  virtual void	setPStorage( Trk::TrackCollection_tlp5 *storage );

// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

// all TP converters for types used in the Track package

protected:
  TrackCnv_p4                       m_tracksCnv;
  TrackStateOnSurfaceCnv_p3         m_trackStatesCnv;	

  ErrorMatrixCnv_p1                 m_errorMatricesCnv;

  PseudoMeasurementOnTrackCnv_p2    m_pseudoMeasurementOnTrackCnv; 
  VertexOnTrackCnv_p1               m_vertexOnTrackCnv;
  CompetingRIOsOnTrackCnv_p1        m_crotCnv;

  TrackParametersCnv_p2             m_parametersCnv;
  
  ConeSurfaceCnv_p2                 m_coneSurfacesCnv       ;
  CylinderSurfaceCnv_p2             m_cylSurfacesCnv        ;
  DiscSurfaceCnv_p2                 m_discSurfacesCnv       ;
  PerigeeSurfaceCnv_p2              m_perigeeSurfacesCnv    ;
  PlaneSurfaceCnv_p2                m_planeSurfacesCnv      ;
  StraightLineSurfaceCnv_p2         m_lineSurfacesCnv       ;
  SaggedLineSurfaceCnv_p2           m_saggedLineSurfacesCnv ;

  FitQualityCnv_p1                  m_fitQualitiesCnv;
  MaterialEffectsBaseCnv_p2         m_matEffectsBaseCnv;
  EnergyLossCnv_p1                  m_energyLossCnv;
  MaterialEffectsOnTrackCnv_p2      m_materialEffectsCnv;
  EstimatedBremOnTrackCnv_p1        m_estimatedBremCnv;

  LocalDirectionCnv_p1              m_localDirectionsCnv;
  LocalPositionCnv_p1               m_localPositionsCnv;
  LocalParametersCnv_p1             m_localParametersCnv;            
};
#endif
