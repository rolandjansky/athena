/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_CNV_TLP1_TRK_H
#define TRACK_COLLECTION_CNV_TLP1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv_tlp1.h
// @brief  Top Level converter for persistent Track Collection
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackCollection_tlp1.h"

#include "TrkEventTPCnv/TrkTrack/TrackCollectionCnv_p1.h"         
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p1.h"                   
#include "TrkEventTPCnv/TrkMaterialOnTrack/ScatteringAngleOnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p1.h"     

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"             
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p1.h"             
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurfaceCnv_p1.h"      
#include "TrkEventTPCnv/TrkParameters/AtaSurfaceCnv_p1.h"              
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigeeCnv_p1.h"         
#include "TrkEventTPCnv/TrkParameters/PerigeeCnv_p1.h"         
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"         

#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"            
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"                 
#include "TrkEventTPCnv/TrkSurfaces/CylinderBoundsCnv_p1.h"          
#include "TrkEventTPCnv/TrkSurfaces/DiamondBoundsCnv_p1.h"           
#include "TrkEventTPCnv/TrkSurfaces/DiscBoundsCnv_p1.h"              
#include "TrkEventTPCnv/TrkSurfaces/RectangleBoundsCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBoundsCnv_p1.h"         

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"              
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p1.h"  
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"          
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"           
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"         

#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3DCnv_p1.h"         

#include <iostream>

class TrackCollectionCnv_tlp1
 : public AthenaPoolTopLevelTPConverter< TrackCollectionCnv_p1, Trk::TrackCollection_tlp1 >
{
public:

  TrackCollectionCnv_tlp1();
  virtual ~TrackCollectionCnv_tlp1() {}

  virtual void	setPStorage( Trk::TrackCollection_tlp1 *storage );

  // return list of tokens to extending persistent objects
  // it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }


  // all TP converters for types used in the Track package
protected:
  TrackCnv_p1                       m_tracksCnv;
  ScatteringAngleOnTrackCnv_p1      m_scatterAnglesCnv;		
  TrackStateOnSurfaceCnv_p1         m_trackStatesCnv;	

  RIO_OnTrackCnv_p1                 m_RIOsCnv;
  PseudoMeasurementOnTrackCnv_p1    m_pseudoMeasurementOnTrackCnv;

  MeasuredAtaDiscCnv_p1             m_measuredAtaDiscsCnv;
  MeasuredAtaCylinderCnv_p1         m_measuredAtaCylindersCnv;
  MeasuredAtaPlaneCnv_p1            m_measuredAtaPlanesCnv;
  MeasuredAtaStraightLineCnv_p1     m_measuredAtaStraightLinesCnv;
  AtaDiscCnv_p1                     m_ataDiscsCnv;
  AtaCylinderCnv_p1                 m_ataCylindersCnv;
  AtaPlaneCnv_p1                    m_ataPlanesCnv;
  AtaStraightLineCnv_p1             m_ataStraightLinesCnv;
  PerigeeSurfaceCnv_p1              m_perigeeSurfacesCnv;
  PerigeeCnv_p1                     m_perigeesCnv;
  MeasuredPerigeeCnv_p1             m_measuredPerigeesCnv;
  TrackParametersCnv_p1             m_parametersCnv;
  
  DiscSurfaceCnv_p1                 m_discSurfacesCnv;
  PlaneSurfaceCnv_p1                m_planeSurfacesCnv;
  CylinderSurfaceCnv_p1             m_cylinderSurfacesCnv;
  StraightLineSurfaceCnv_p1         m_straightLineSurfacesCnv;
  
  SurfaceCnv_p1                     m_surfacesCnv;
  CylinderBoundsCnv_p1              m_cylinderBoundsCnv;
  DiamondBoundsCnv_p1               m_diamondBoundsCnv;
  DiscBoundsCnv_p1                  m_discBoundsCnv;
  RectangleBoundsCnv_p1             m_rectangleBoundsCnv;
  TrapezoidBoundsCnv_p1             m_trapesoidBoundsCnv;

  FitQualityCnv_p1                  m_fitQualitiesCnv;
  CovarianceMatrixCnv_p1            m_covarianceMatricesCnv;
  ErrorMatrixCnv_p1                 m_errorMatricesCnv;
  MaterialEffectsOnTrackCnv_p1      m_materialEffectsCnv;
  LocalDirectionCnv_p1              m_localDirectionsCnv;
  LocalPositionCnv_p1               m_localPositionsCnv;
  LocalParametersCnv_p1             m_localParametersCnv;            

  HepPoint3DCnv_p1                  m_pointsCnv;      
  
  CompetingRIOsOnTrackCnv_p1        m_crotCnv;
};

#endif 

