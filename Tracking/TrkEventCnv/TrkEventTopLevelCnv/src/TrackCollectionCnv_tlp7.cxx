/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTopLevelCnv/TrackCollectionCnv_tlp7.h"

/*
  Converter for TrackCollection v tlp7.
  Direct copy of TLP5 but with MuonMeasurements and InDet::Track tlp converters
  integrated directly
*/

TrackCollectionCnv_tlp7::TrackCollectionCnv_tlp7( )
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
   // Other measurements are stored in the other tlp objects, i.e. the InDetTrack, and MuonMeasurements

   // Parameters
  addTPConverter( &m_parametersCnv );            

   // Surfaces
  addTPConverter( &m_perigeeSurfacesCnv       );
  addTPConverter( &m_coneSurfacesCnv       );
  addTPConverter( &m_cylSurfacesCnv        );
  addTPConverter( &m_discSurfacesCnv       );
  addTPConverter( &m_planeSurfacesCnv      );
  addTPConverter( &m_lineSurfacesCnv       );
  addTPConverter( &m_saggedLineSurfacesCnv );

  // Bounds
  addTPConverter( &m_coneBoundsCnv );        
  addTPConverter( &m_cylinderBoundsCnv );        
  addTPConverter( &m_diamondBoundsCnv );         
  addTPConverter( &m_rotatedDiamondBoundsCnv );         
  addTPConverter( &m_discBoundsCnv );            
  addTPConverter( &m_rectangleBoundsCnv );       
  addTPConverter( &m_trapesoidBoundsCnv );  
  addTPConverter( &m_rotatedTrapesoidBoundsCnv );    
  addTPConverter( &m_ellipseBoundsCnv );    

  addTPConverter( &m_fitQualitiesCnv );
  addTPConverter( &m_matEffectsBaseCnv );
  addTPConverter( &m_energyLossCnv );
  addTPConverter( &m_materialEffectsCnv );
  addTPConverter( &m_estimatedBremCnv );

  addTPConverter( &m_localDirectionsCnv );
  addTPConverter( &m_localPositionsCnv );
  addTPConverter( &m_localParametersCnv );

  //Multi Component state on Surface
  addTPConverter(&m_multiTrackStatesCnv);

  // adding extending TLP converters,
  // they don't interfere with the order of adding regular converters (above)
  m_muonMeasurementsCnv.addTPConvertersTo( this );
  m_inDetTrackCnv      .addTPConvertersTo( this );
  m_muonCaloEnergiesCnv.addTPConvertersTo( this );

}


void TrackCollectionCnv_tlp7::setPStorage( PERS *storage )
{	
  setMainCnvPStorage( &storage->m_trackCollections );

// for all converters defined in this top level converter
  m_tracksCnv.               setPStorage( &storage->m_tracks );
  m_trackStatesCnv.          setPStorage( &storage->m_trackStates );
  m_multiTrackStatesCnv.     setPStorage( &storage->m_trackStates );

  m_pseudoMeasurementOnTrackCnv.setPStorage( &storage->m_pseudoMeasurementOnTrack );
  m_crotCnv.                 setPStorage( &storage->m_competingRotsOnTrack );
  m_vertexOnTrackCnv.        setPStorage( &storage->m_vertexOnTrack );

  m_parametersCnv.           setPStorage( &storage->m_parameters );

  // Unbound Surfaces
  m_perigeeSurfacesCnv       .setPStorage( &storage->m_surfaces );

  // (potentially) bound surfaces
  m_coneSurfacesCnv          .setPStorage( &storage->m_boundSurfaces );
  m_cylSurfacesCnv           .setPStorage( &storage->m_boundSurfaces );
  m_discSurfacesCnv          .setPStorage( &storage->m_boundSurfaces );
  m_planeSurfacesCnv         .setPStorage( &storage->m_boundSurfaces );
  m_lineSurfacesCnv          .setPStorage( &storage->m_boundSurfaces );  
  m_saggedLineSurfacesCnv    .setPStorage( &storage->m_boundSurfaces );
  
  // Bound
  m_coneBoundsCnv            .setPStorage( &storage->m_coneBounds );            
  m_cylinderBoundsCnv        .setPStorage( &storage->m_cylinderBounds );
  m_diamondBoundsCnv         .setPStorage( &storage->m_diamondBounds );
  m_rotatedDiamondBoundsCnv  .setPStorage( &storage->m_rotatedDiamondBounds );
  m_discBoundsCnv            .setPStorage( &storage->m_discBounds );
  m_rectangleBoundsCnv       .setPStorage( &storage->m_rectangleBounds );
  m_trapesoidBoundsCnv       .setPStorage( &storage->m_trapesoidBounds );
  m_rotatedTrapesoidBoundsCnv.setPStorage( &storage->m_rotatedTrapesoidBounds );
  m_ellipseBoundsCnv         .setPStorage( &storage->m_ellipseBounds );

  m_errorMatricesCnv.        setPStorage( &storage->m_hepSymMatrices );
  m_matEffectsBaseCnv.       setPStorage( &storage->m_matEffectsBases );
  m_energyLossCnv.           setPStorage( &storage->m_energyLosses );
  m_materialEffectsCnv.      setPStorage( &storage->m_materialEffects );
  m_estimatedBremCnv.        setPStorage( &storage->m_estimatedBrems );

  m_fitQualitiesCnv.         setPStorage( &storage->m_fitQualities );
  m_localDirectionsCnv.      setPStorage( &storage->m_localDirections );
  m_localPositionsCnv.       setPStorage( &storage->m_localPositions );
  m_localParametersCnv.      setPStorage( &storage->m_localParameters );

  // when reading (createTrans) these will be non-null if extension is present
  // when writing, they will be null at first end set afterwards in createPersistentWithKey()
  if( storage->m_inDetTrackExt )       m_inDetTrackCnv      .setPStorage( storage->m_inDetTrackExt );
  if( storage->m_muonMeasurementsExt ) m_muonMeasurementsCnv.setPStorage( storage->m_muonMeasurementsExt );
  if( storage->m_muonCaloEnergiesExt ) m_muonCaloEnergiesCnv.setPStorage( storage->m_muonCaloEnergiesExt );
}


BaseTrackCollectionCnv_tlp7::PERS*
TrackCollectionCnv_tlp7::createPersistentWithKey(const TRANS* transObj,
                                                 const std::string& key,
                                                 MsgStream &log)
{
   PERS *pers = BaseTrackCollectionCnv_tlp7::createPersistentWithKey(transObj, key, log);

   // get extending objects (nullptr if there was no extending data types)
   pers->m_muonMeasurementsExt = m_muonMeasurementsCnv.releaseTLPersObject();
   pers->m_inDetTrackExt       = m_inDetTrackCnv.releaseTLPersObject();
   pers->m_muonCaloEnergiesExt = m_muonCaloEnergiesCnv.releaseTLPersObject();

   return pers;
}
