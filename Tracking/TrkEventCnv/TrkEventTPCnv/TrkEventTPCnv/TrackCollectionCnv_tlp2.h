/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_CNV_TLP2_TRK_H
#define TRACK_COLLECTION_CNV_TLP2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv_tlp2.h
// @brief  Top Level converter for persistent Track Collection
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrackCollection_tlp2.h"

#include "TrkEventTPCnv/TrkTrack/TrackCollectionCnv_p2.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p2.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p2.h"     

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
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBoundsCnv_p1.h"     
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

class TrackCollectionCnv_tlp2
 : public AthenaPoolTopLevelTPConverter< TrackCollectionCnv_p2, Trk::TrackCollection_tlp2 >
{
public:

  TrackCollectionCnv_tlp2();
  virtual ~TrackCollectionCnv_tlp2() {}

  virtual void	setPStorage( Trk::TrackCollection_tlp2 *storage );

  // return list of tokens to extending persistent objects
  // it is stored in m_tokenList member of the Track persistent rep
  virtual TPCnvTokenList_p1* 	getTokenListVar() { return &(getTLPersObject()->m_tokenList); }


  // all TP converters for types used in the Track package
protected:
  TrackCnv_p2                       m_tracksCnv;
  TrackStateOnSurfaceCnv_p2         m_trackStatesCnv;	

  RIO_OnTrackCnv_p1                 m_RIOsCnv;
  PseudoMeasurementOnTrackCnv_p1    m_pseudoMeasurementOnTrackCnv;
  CompetingRIOsOnTrackCnv_p1        m_crotCnv;

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
  DetElementSurfaceCnv_p1           m_detElSurfCnv;
  
  SurfaceCnv_p1                     m_surfacesCnv;
  CylinderBoundsCnv_p1              m_cylinderBoundsCnv;
  DiamondBoundsCnv_p1               m_diamondBoundsCnv;
  DiscBoundsCnv_p1                  m_discBoundsCnv;
  RectangleBoundsCnv_p1             m_rectangleBoundsCnv;
  TrapezoidBoundsCnv_p1             m_trapesoidBoundsCnv;
  RotatedTrapezoidBoundsCnv_p1      m_rotatedTrapesoidBoundsCnv;

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
};

#endif 

