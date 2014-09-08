/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_CNV_TLP3_TRK_H
#define TRACK_COLLECTION_CNV_TLP3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv_tlp3.h
// @brief  Top Level converter for persistent Track Collection
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackCollection_tlp3.h"

#include "TrkEventTPCnv/TrkTrack/TrackCollectionCnv_p3.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p3.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p2.h"  
#include "TrkEventTPCnv/TrkTrack/TrackInfoCnv_p1.h"    

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"             
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p1.h"             
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrackCnv_p1.h"             

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
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBoundsCnv_p1.h"     
#include "TrkEventTPCnv/TrkSurfaces/EllipseBoundsCnv_p1.h"     
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"              
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkEventPrimitives/CovarianceMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"          
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"           
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"         

#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3DCnv_p1.h"      

#include <iostream>


class TrackCollectionCnv_tlp3
 : public AthenaPoolTopLevelTPConverter< TrackCollectionCnv_p3, Trk::TrackCollection_tlp3 >
{
 public:
 
 TrackCollectionCnv_tlp3();
  virtual ~TrackCollectionCnv_tlp3() {}

  virtual void	setPStorage( Trk::TrackCollection_tlp3 *storage );

// return list of tokens to extending persistent objects
// it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }

// all TP converters for types used in the Track package

protected:
  TrackCnv_p3                       m_tracksCnv;
  TrackStateOnSurfaceCnv_p2         m_trackStatesCnv;	

  RIO_OnTrackCnv_p1                 m_RIOsCnv;
  PseudoMeasurementOnTrackCnv_p1    m_pseudoMeasurementOnTrackCnv; 
  VertexOnTrackCnv_p1               m_vertexOnTrackCnv;
  CompetingRIOsOnTrackCnv_p1        m_crotCnv;

  MeasuredAtaDiscCnv_p1             m_measuredAtaDiscsCnv;
  MeasuredAtaCylinderCnv_p1         m_measuredAtaCylindersCnv;
  MeasuredAtaPlaneCnv_p1            m_measuredAtaPlanesCnv;
  MeasuredAtaStraightLineCnv_p1     m_measuredAtaStraightLinesCnv;
  MeasuredCurvilinearParametersCnv_p1 m_measuredCurvilinearParametersCnv;
  AtaDiscCnv_p1                     m_ataDiscsCnv;
  AtaCylinderCnv_p1                 m_ataCylindersCnv;
  AtaPlaneCnv_p1                    m_ataPlanesCnv;
  AtaStraightLineCnv_p1             m_ataStraightLinesCnv;
  CurvilinearParametersCnv_p1       m_curvilinearParametersCnv;
  PerigeeSurfaceCnv_p1              m_perigeeSurfacesCnv;
  PerigeeCnv_p1                     m_perigeesCnv;
  MeasuredPerigeeCnv_p1             m_measuredPerigeesCnv;
  TrackParametersCnv_p1             m_parametersCnv;
  
  DiscSurfaceCnv_p1                 m_discSurfacesCnv;
  PlaneSurfaceCnv_p1                m_planeSurfacesCnv;
  CylinderSurfaceCnv_p1             m_cylinderSurfacesCnv;
  StraightLineSurfaceCnv_p1         m_straightLineSurfacesCnv;
  DetElementSurfaceCnv_p1           m_detElSurfCnv;
  
  SurfaceCnv_p1                     m_surfacesCnv;
  CylinderBoundsCnv_p1              m_cylinderBoundsCnv;
  DiamondBoundsCnv_p1               m_diamondBoundsCnv;
  DiscBoundsCnv_p1                  m_discBoundsCnv;
  RectangleBoundsCnv_p1             m_rectangleBoundsCnv;
  TrapezoidBoundsCnv_p1             m_trapesoidBoundsCnv;
  RotatedTrapezoidBoundsCnv_p1      m_rotatedTrapesoidBoundsCnv;
  EllipseBoundsCnv_p1               m_ellipseBoundsCnv;

  FitQualityCnv_p1                  m_fitQualitiesCnv;
  CovarianceMatrixCnv_p1            m_covarianceMatricesCnv;
  ErrorMatrixCnv_p1                 m_errorMatricesCnv;
  MaterialEffectsBaseCnv_p1         m_matEffectsBaseCnv;
  EnergyLossCnv_p1                  m_energyLossCnv;
  MaterialEffectsOnTrackCnv_p2      m_materialEffectsCnv;
  EstimatedBremOnTrackCnv_p1        m_estimatedBremCnv;
  LocalDirectionCnv_p1              m_localDirectionsCnv;
  LocalPositionCnv_p1               m_localPositionsCnv;
  LocalParametersCnv_p1             m_localParametersCnv;            

  HepPoint3DCnv_p1                  m_pointsCnv;            	
  TrackInfoCnv_p1                   m_trackInfosCnv;
};
#endif
