/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_tlp4_TRK_H
#define TRACK_COLLECTION_tlp4_TRK_H


//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCollection_p1.h"
#include "TrkEventTPCnv/TrkTrack/Track_p2.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p2.h"
#include "TrkEventTPCnv/TrkTrack/TrackInfo_p1.h"

//-----------------------------------------------------------------------------
//TrkTrackSummary
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrackSummary/TrackSummary_p2.h"

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirection_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParameters_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//-----------------------------------------------------------------------------
// TrkMaterialOnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBase_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLoss_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrack_p2.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrack_p1.h"

//-----------------------------------------------------------------------------
// TrkSurfaces
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurface_p1.h"

#include "TrkEventTPCnv/TrkSurfaces/CylinderBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiamondBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/DiscBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RectangleBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBounds_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBounds_p1.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurface_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/EllipseBounds_p1.h"     

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredAtaSurface_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p1.h"
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrack_p1.h"

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace Trk
{
  class TrackCollection_tlp4 
  {
   public:
   
     TrackCollection_tlp4() {
       // std::cout<<"TrackCollection_tlp4 Ctor this="<<this<<std::endl;
       // 
       //   void *array[30];
       //   size_t size = backtrace(array, 30);
       //   char** trace = backtrace_symbols(array,size);
       //   for (unsigned int i=0;i<size;i++){
       //       printf("%s\n",trace[i]);
       //   }
     }
     ~TrackCollection_tlp4() {
         //        
         // void *array[30];
         // size_t size = backtrace(array, 30);
         // char** trace = backtrace_symbols(array,size);
         // for (unsigned int i=0;i<size;i++){
         //     printf("%s\n",trace[i]);
         // }
       }
     
     TPCnvTokenList_p1                                  m_tokenList;

     std::vector< Trk::TrackCollection_p1 >             m_trackCollections;
     std::vector< Trk::Track_p2 >                       m_tracks;
     std::vector< Trk::TrackStateOnSurface_p2 >         m_trackStates;	

     std::vector< Trk::CompetingRIOsOnTrack_p1 >        m_competingRotsOnTrack;
     std::vector< Trk::PseudoMeasurementOnTrack_p1 >    m_pseudoMeasurementOnTrack;	
     std::vector< Trk::VertexOnTrack_p1 >               m_vertexOnTrack;	

     std::vector< Trk::TrackParameters_p1 >             m_parameters;
     std::vector< Trk::AtaSurface_p1 >                  m_ataSurfaces;
     std::vector< Trk::MeasuredAtaSurface_p1 >          m_measuredAtaSurfaces;
     std::vector< Trk::Perigee_p1 >                     m_perigees;
     std::vector< Trk::MeasuredPerigee_p1 >             m_measuredPerigees;
     
     std::vector< Trk::BoundSurface_p1 >                m_boundSurfaces;
     std::vector< Trk::Surface_p1 >                     m_surfaces;
     std::vector< Trk::CylinderBounds_p1 >              m_cylinderBounds;
     std::vector< Trk::DiamondBounds_p1 >               m_diamondBounds;
     std::vector< Trk::DiscBounds_p1 >                  m_discBounds;
     std::vector< Trk::RectangleBounds_p1 >             m_rectangleBounds;
     std::vector< Trk::TrapezoidBounds_p1 >             m_trapesoidBounds;
     std::vector< Trk::RotatedTrapezoidBounds_p1 >      m_rotatedTrapesoidBounds;
     std::vector< Trk::DetElementSurface_p1 >           m_detElementSurfaces; 
     std::vector< Trk::EllipseBounds_p1 >               m_ellipseBounds;

     std::vector< Trk::FitQuality_p1 >                  m_fitQualities;
     std::vector< Trk::HepSymMatrix_p1 >                m_hepSymMatrices;
     std::vector< Trk::MaterialEffectsBase_p1 >         m_matEffectsBases;
     std::vector< Trk::EnergyLoss_p1 >                  m_energyLosses;
     std::vector< Trk::MaterialEffectsOnTrack_p2 >      m_materialEffects;
     std::vector< Trk::EstimatedBremOnTrack_p1 >        m_estimatedBrems;
     std::vector< Trk::LocalDirection_p1 >              m_localDirections;
     std::vector< Trk::LocalPosition_p1 >               m_localPositions;
     std::vector< Trk::LocalParameters_p1 >             m_localParameters;
     std::vector< Trk::TrackInfo_p1 >                   m_trackInfos;
     std::vector< Trk::TrackSummary_p2>                 m_trackSummary;
  };
}

#endif // TRACK_COLLECTION_P4_TRK_H
