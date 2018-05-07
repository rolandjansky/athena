/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_COLLECTION_TLP6_TRK_H
#define TRACK_COLLECTION_TLP6_TRK_H


//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCollection_p1.h"
#include "TrkEventTPCnv/TrkTrack/Track_p4.h"
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurface_p3.h"

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
#include "TrkEventTPCnv/TrkSurfaces/Surface_p2.h"   

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p2.h"

//-----------------------------------------------------------------------------
// TrkRIO_OnTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack_p1.h"
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p2.h"
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrack_p1.h"

//-----------------------------------------------------------------------------
// Top Level Pers Objects from InnerDetector and MuonSpectrometer
// Previously stored as separate Extening TP objects, now integrated
// including full declarations for dictionary's sake

#include "InDetEventTPCnv/InDetTrack_tlp2.h"
#include "MuonEventTPCnv/MuonMeasurements_tlp2.h"
#include "RecTPCnv/MuonCaloEnergyContainerCnv_tlp1.h"

namespace Trk
{
  class TrackCollection_tlp6
  {
   public:
    TrackCollection_tlp6() { }
    
    ~TrackCollection_tlp6() {
       delete m_inDetTrackExt; 
       delete m_muonMeasurementsExt;
       delete m_muonCaloEnergiesExt;
    }

    TrackCollection_tlp6 (const TrackCollection_tlp6&) = delete;
    TrackCollection_tlp6& operator= (const TrackCollection_tlp6&) = delete; 

     std::vector< Trk::TrackCollection_p1 >             m_trackCollections;
     std::vector< Trk::Track_p4 >                       m_tracks;
     std::vector< Trk::TrackStateOnSurface_p3 >         m_trackStates;	

     std::vector< Trk::CompetingRIOsOnTrack_p1 >        m_competingRotsOnTrack;
     std::vector< Trk::PseudoMeasurementOnTrack_p2 >    m_pseudoMeasurementOnTrack;	
     std::vector< Trk::VertexOnTrack_p1 >               m_vertexOnTrack;	

     std::vector< Trk::TrackParameters_p2 >             m_parameters;

     std::vector< Trk::Surface_p2 >                     m_surfaces;

     std::vector< Trk::HepSymMatrix_p1 >                m_hepSymMatrices;
     std::vector< Trk::MaterialEffectsBase_p1 >         m_matEffectsBases;
     std::vector< Trk::EnergyLoss_p1 >                  m_energyLosses;
     std::vector< Trk::MaterialEffectsOnTrack_p2 >      m_materialEffects;
     std::vector< Trk::EstimatedBremOnTrack_p1 >        m_estimatedBrems;

     std::vector< Trk::FitQuality_p1 >                  m_fitQualities;
     std::vector< Trk::LocalDirection_p1 >              m_localDirections;
     std::vector< Trk::LocalPosition_p1 >               m_localPositions;
     std::vector< Trk::LocalParameters_p1 >             m_localParameters;

     // TLP converters from other packages 
     // for subclass types found in Tracking polymorphic collections
     InDet::Track_tlp2                   *m_inDetTrackExt = nullptr;
     TPCnv::MuonMeasurements_tlp2        *m_muonMeasurementsExt = nullptr;
     MuonCaloEnergyContainer_tlp1        *m_muonCaloEnergiesExt = nullptr;
  };
}

#endif // TRACK_COLLECTION_P6_TRK_H
