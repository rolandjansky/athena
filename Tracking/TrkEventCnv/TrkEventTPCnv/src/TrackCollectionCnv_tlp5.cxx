/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"

TrackCollectionCnv_tlp5::TrackCollectionCnv_tlp5( )
{
// Add all converters defined in this top level converter:
// never change the order of adding converters!

  addMainTPConverter();

  addTPConverter( &m_tracksCnv );                
  addTPConverter( &m_trackStatesCnv );          

  addTPConverter( &m_errorMatricesCnv );

   // Measurements                  
  addTPConverter( &m_pseudoMeasurementOnTrackCnv ); 
  addTPConverter( &m_crotCnv );
  addTPConverter( &m_vertexOnTrackCnv );
   // Other measurements are stored in the extending objects, i.e. the InDetTrack, and MuonMeasurements

   // Parameters
  addTPConverter( &m_parametersCnv );            

   // Surfaces
  addTPConverter( &m_coneSurfacesCnv       );
  addTPConverter( &m_cylSurfacesCnv        );
  addTPConverter( &m_discSurfacesCnv       );
  addTPConverter( &m_perigeeSurfacesCnv    );
  addTPConverter( &m_planeSurfacesCnv      );
  addTPConverter( &m_lineSurfacesCnv       );
  addTPConverter( &m_saggedLineSurfacesCnv );

  addTPConverter( &m_matEffectsBaseCnv );
  addTPConverter( &m_energyLossCnv );
  addTPConverter( &m_materialEffectsCnv );
  addTPConverter( &m_estimatedBremCnv );

  addTPConverter( &m_fitQualitiesCnv );
  addTPConverter( &m_localDirectionsCnv );
  addTPConverter( &m_localPositionsCnv );
  addTPConverter( &m_localParametersCnv );
}

void TrackCollectionCnv_tlp5::setPStorage( Trk::TrackCollection_tlp5 *storage )
{	
  setMainCnvPStorage( &storage->m_trackCollections );

// for all converters defined in this top level converter
  m_tracksCnv.               setPStorage( &storage->m_tracks );
  m_trackStatesCnv.          setPStorage( &storage->m_trackStates );

  m_errorMatricesCnv.        setPStorage( &storage->m_hepSymMatrices );

  m_pseudoMeasurementOnTrackCnv.setPStorage( &storage->m_pseudoMeasurementOnTrack );
  m_crotCnv.                 setPStorage( &storage->m_competingRotsOnTrack );
  m_vertexOnTrackCnv.        setPStorage( &storage->m_vertexOnTrack );

  m_parametersCnv.           setPStorage( &storage->m_parameters );

  m_coneSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_cylSurfacesCnv           .setPStorage( &storage->m_surfaces );
  m_discSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_perigeeSurfacesCnv       .setPStorage( &storage->m_surfaces );
  m_planeSurfacesCnv         .setPStorage( &storage->m_surfaces );
  m_lineSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_saggedLineSurfacesCnv    .setPStorage( &storage->m_surfaces );

  m_matEffectsBaseCnv.       setPStorage( &storage->m_matEffectsBases );
  m_energyLossCnv.           setPStorage( &storage->m_energyLosses );
  m_materialEffectsCnv.      setPStorage( &storage->m_materialEffects );
  m_estimatedBremCnv.        setPStorage( &storage->m_estimatedBrems );

  m_fitQualitiesCnv.         setPStorage( &storage->m_fitQualities );
  m_localDirectionsCnv.      setPStorage( &storage->m_localDirections );
  m_localPositionsCnv.       setPStorage( &storage->m_localPositions );
  m_localParametersCnv.      setPStorage( &storage->m_localParameters );
}
