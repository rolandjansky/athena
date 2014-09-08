/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/TrackCollectionCnv_tlp2.h"


TrackCollectionCnv_tlp2::TrackCollectionCnv_tlp2( )
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!
   addMainTPConverter();

   addTPConverter( &m_tracksCnv );                
   addTPConverter( &m_trackStatesCnv );           
   
   addTPConverter( &m_RIOsCnv );                  
   addTPConverter( &m_pseudoMeasurementOnTrackCnv );           
   
   addTPConverter( &m_measuredAtaPlanesCnv );   
   addTPConverter( &m_measuredAtaDiscsCnv );   
   addTPConverter( &m_measuredAtaCylindersCnv );   
   addTPConverter( &m_measuredAtaStraightLinesCnv );   
   
   addTPConverterForReading( &m_ataPlanesCnv );           
   addTPConverterForReading( &m_ataDiscsCnv );           
   addTPConverterForReading( &m_ataCylindersCnv );           
   addTPConverterForReading( &m_ataStraightLinesCnv );           
   
   addTPConverter( &m_perigeeSurfacesCnv );      
   addTPConverter( &m_perigeesCnv );      
   addTPConverterForReading( &m_measuredPerigeesCnv );      
   addTPConverter( &m_parametersCnv );            
   
   addTPConverter( &m_planeSurfacesCnv );         
   addTPConverter( &m_discSurfacesCnv );         
   addTPConverter( &m_cylinderSurfacesCnv );         
   addTPConverter( &m_straightLineSurfacesCnv );         
   addTPConverter( &m_surfacesCnv );
   
   addTPConverter( &m_cylinderBoundsCnv );        
   addTPConverter( &m_diamondBoundsCnv );         
   addTPConverter( &m_discBoundsCnv );            
   addTPConverter( &m_rectangleBoundsCnv );       
   addTPConverter( &m_trapesoidBoundsCnv );  
   addTPConverter( &m_rotatedTrapesoidBoundsCnv );           
   
   addTPConverter( &m_fitQualitiesCnv );
   addTPConverter( &m_errorMatricesCnv );
   addTPConverter( &m_covarianceMatricesCnv );
   addTPConverter( &m_matEffectsBaseCnv );
   addTPConverter( &m_energyLossCnv );
   addTPConverter( &m_materialEffectsCnv );
   addTPConverter( &m_estimatedBremCnv );
   addTPConverter( &m_localDirectionsCnv );
   addTPConverter( &m_localPositionsCnv );
   addTPConverter( &m_localParametersCnv );

   addTPConverter( &m_crotCnv );
   
   // Added with Tag TrkEventTPCnv-00-16-00
   addTPConverter( &m_detElSurfCnv );      
}



void TrackCollectionCnv_tlp2::setPStorage( Trk::TrackCollection_tlp2 *storage )
{
   // for implicit TrackCollection_p1 from the base class:
     setMainCnvPStorage( &storage->m_trackCollections );

     // for all converters defined in this top level converter
     m_tracksCnv.               setPStorage( &storage->m_tracks );
     m_trackStatesCnv.          setPStorage( &storage->m_trackStates );

     m_RIOsCnv.                 setPStorage( &storage->m_RIOs );
     m_pseudoMeasurementOnTrackCnv.setPStorage( &storage->m_pseudoMeasurementOnTrack );

     m_measuredAtaDiscsCnv.     setPStorage( &storage->m_measuredAtaSurfaces );
     m_measuredAtaPlanesCnv.    setPStorage( &storage->m_measuredAtaSurfaces );
     m_measuredAtaCylindersCnv. setPStorage( &storage->m_measuredAtaSurfaces );
     m_measuredAtaStraightLinesCnv.setPStorage( &storage->m_measuredAtaSurfaces );
     
     m_ataDiscsCnv.             setPStorage( &storage->m_ataSurfaces );
     m_ataPlanesCnv.            setPStorage( &storage->m_ataSurfaces );
     m_ataCylindersCnv.         setPStorage( &storage->m_ataSurfaces );
     m_ataStraightLinesCnv.     setPStorage( &storage->m_ataSurfaces );
                                
     m_perigeeSurfacesCnv.      setPStorage( &storage->m_surfaces );
     m_parametersCnv.           setPStorage( &storage->m_parameters );
     m_perigeesCnv.             setPStorage( &storage->m_perigees );
     m_measuredPerigeesCnv.     setPStorage( &storage->m_measuredPerigees );

     m_discSurfacesCnv.         setPStorage( &storage->m_boundSurfaces );
     m_planeSurfacesCnv.        setPStorage( &storage->m_boundSurfaces );
     m_cylinderSurfacesCnv.     setPStorage( &storage->m_boundSurfaces );
     m_straightLineSurfacesCnv. setPStorage( &storage->m_boundSurfaces );
     m_surfacesCnv.             setPStorage( &storage->m_surfaces );
     m_cylinderBoundsCnv.       setPStorage( &storage->m_cylinderBounds );
     m_diamondBoundsCnv.        setPStorage( &storage->m_diamondBounds );
     m_discBoundsCnv.           setPStorage( &storage->m_discBounds );
     m_rectangleBoundsCnv.      setPStorage( &storage->m_rectangleBounds );
     m_trapesoidBoundsCnv.      setPStorage( &storage->m_trapesoidBounds );
     m_rotatedTrapesoidBoundsCnv.   setPStorage( &storage->m_rotatedTrapesoidBounds);

     m_fitQualitiesCnv.         setPStorage( &storage->m_fitQualities );
     m_covarianceMatricesCnv.   setPStorage( &storage->m_hepSymMatrices );
     m_errorMatricesCnv.        setPStorage( &storage->m_hepSymMatrices );
     m_matEffectsBaseCnv.       setPStorage( &storage->m_matEffectsBases );
     m_energyLossCnv.           setPStorage( &storage->m_energyLosses );
     m_materialEffectsCnv.      setPStorage( &storage->m_materialEffects );
     m_estimatedBremCnv.        setPStorage( &storage->m_estimatedBrems );
     m_localDirectionsCnv.      setPStorage( &storage->m_localDirections );
     m_localPositionsCnv.       setPStorage( &storage->m_localPositions );
     m_localParametersCnv.      setPStorage( &storage->m_localParameters );
     m_crotCnv.                 setPStorage( &storage->m_competingRotsOnTrack );
     m_detElSurfCnv.            setPStorage( &storage->m_detElementSurfaces );     
     
}   
 
