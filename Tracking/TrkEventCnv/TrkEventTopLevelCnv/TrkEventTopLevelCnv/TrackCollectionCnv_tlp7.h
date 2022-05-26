/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_CNV_TLP7_TRK_H
#define TRACK_COLLECTION_CNV_TLP7_TRK_H


#include "TPTools/TopLevelTPConverter.h"

#include "TrackCollection_tlp7.h"

#include "TrkEventTPCnv/TrkTrack/TrackCollectionCnv_p4.h"
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p4.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"  

#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"  
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrackCnv_p1.h"             

#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"      
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p2.h" // Also takes care of surfaces now, for these objects

#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p2.h"            
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"            
#include "TrkEventTPCnv/TrkSurfaces/CylinderBoundsCnv_p1.h"          
#include "TrkEventTPCnv/TrkSurfaces/DiamondBoundsCnv_p1.h"           
#include "TrkEventTPCnv/TrkSurfaces/RotatedDiamondBoundsCnv_p1.h"           
#include "TrkEventTPCnv/TrkSurfaces/DiscBoundsCnv_p1.h"              
#include "TrkEventTPCnv/TrkSurfaces/RectangleBoundsCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBoundsCnv_p1.h"         
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBoundsCnv_p1.h"     
#include "TrkEventTPCnv/TrkSurfaces/EllipseBoundsCnv_p1.h"     
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"            
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"          
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"           
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"         

#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp2.h"
#include "InDetEventTPCnv/InDetTrackCnv_tlp2.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"


typedef TopLevelTPConverter< TrackCollectionCnv_p4, Trk::TrackCollection_tlp7 > BaseTrackCollectionCnv_tlp7;

class TrackCollectionCnv_tlp7 : public BaseTrackCollectionCnv_tlp7
{
 public:
 
  TrackCollectionCnv_tlp7();

  virtual void	setPStorage( PERS *storage ) override;
  virtual PERS* createPersistentWithKey(const TRANS* transObj,
                                        const std::string& key,
                                        MsgStream &log) override;

// all TP converters for types used in the Track package
protected:
  TrackCnv_p4                       m_tracksCnv;
  TrackStateOnSurfaceCnv_p3         m_trackStatesCnv;	
  // Converter for Multicomponent TSOS
  MultiComponentStateOnSurfaceCnv_p1 m_multiTrackStatesCnv;

  ErrorMatrixCnv_p1                 m_errorMatricesCnv;

  PseudoMeasurementOnTrackCnv_p2    m_pseudoMeasurementOnTrackCnv; 
  VertexOnTrackCnv_p1               m_vertexOnTrackCnv;
  CompetingRIOsOnTrackCnv_p1        m_crotCnv;

  TrackParametersCnv_p2             m_parametersCnv;
  
  PerigeeSurfaceCnv_p2              m_perigeeSurfacesCnv    ;
  BoundConeSurfaceCnv_p2            m_coneSurfacesCnv       ;
  BoundCylinderSurfaceCnv_p2        m_cylSurfacesCnv        ;
  BoundDiscSurfaceCnv_p2            m_discSurfacesCnv       ;
  BoundPlaneSurfaceCnv_p2           m_planeSurfacesCnv      ;
  BoundStraightLineSurfaceCnv_p2    m_lineSurfacesCnv       ;
  BoundSaggedLineSurfaceCnv_p2      m_saggedLineSurfacesCnv       ;

  ConeBoundsCnv_p1                  m_coneBoundsCnv;
  CylinderBoundsCnv_p1              m_cylinderBoundsCnv;
  DiamondBoundsCnv_p1               m_diamondBoundsCnv;
  RotatedDiamondBoundsCnv_p1        m_rotatedDiamondBoundsCnv;
  DiscBoundsCnv_p1                  m_discBoundsCnv;
  RectangleBoundsCnv_p1             m_rectangleBoundsCnv;
  TrapezoidBoundsCnv_p1             m_trapesoidBoundsCnv;
  RotatedTrapezoidBoundsCnv_p1      m_rotatedTrapesoidBoundsCnv;
  EllipseBoundsCnv_p1               m_ellipseBoundsCnv;
  
  FitQualityCnv_p1                  m_fitQualitiesCnv;
  MaterialEffectsBaseCnv_p2         m_matEffectsBaseCnv;
  EnergyLossCnv_p1                  m_energyLossCnv;
  MaterialEffectsOnTrackCnv_p2      m_materialEffectsCnv;
  EstimatedBremOnTrackCnv_p1        m_estimatedBremCnv;

  LocalDirectionCnv_p1              m_localDirectionsCnv;
  LocalPositionCnv_p1               m_localPositionsCnv;
  LocalParametersCnv_p1             m_localParametersCnv;

  // TLP converters for extending TLP objects
  MuonMeasurementsCnv_tlp2          m_muonMeasurementsCnv;
  InDetTrackCnv_tlp2                m_inDetTrackCnv;
  MuonCaloEnergyContainerCnv_tlp1   m_muonCaloEnergiesCnv;

};
#endif
