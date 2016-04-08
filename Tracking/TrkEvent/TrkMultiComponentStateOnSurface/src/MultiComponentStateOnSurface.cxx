/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkEventPrimitives/FitQuality.h" 
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <iostream>


Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface()
    :
    TrackStateOnSurface( 0, 0, 0, 0, 
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>( 1<<(int)TrackStateOnSurface::Measurement) ),
    m_multiComponentState(0),
    m_mixtureModeQoverP(0.)
{}



Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface( const Trk::MeasurementBase* measurementBase,
                 const Trk::MultiComponentState* multiComponentState,
                 const Trk::FitQualityOnSurface* fitQualityOnSurface,
                 const MaterialEffectsBase* materialEffectsOnTrack,
                 double modeQoverP)
  :
  TrackStateOnSurface(  measurementBase,
      multiComponentState->front().first->clone(),
      fitQualityOnSurface,         
      materialEffectsOnTrack,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>( 1<<(int)TrackStateOnSurface::Measurement) ),
  
  m_multiComponentState(multiComponentState),
  m_mixtureModeQoverP(modeQoverP)
{}



Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface( const Trk::MeasurementBase* measurementBase,
                 const Trk::TrackParameters* trackParameters,
                 const Trk::MultiComponentState* multiComponentState,
                 const Trk::FitQualityOnSurface* fitQualityOnSurface,
                 const MaterialEffectsBase* materialEffectsOnTrack,
                 double modeQoverP)
  :
  TrackStateOnSurface( measurementBase,
           trackParameters,
           fitQualityOnSurface,
           materialEffectsOnTrack,
           std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>( 1<<(int)TrackStateOnSurface::Measurement) ),
  
  m_multiComponentState(multiComponentState),
  m_mixtureModeQoverP(modeQoverP)
{}



Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface( const Trk::MeasurementBase* measurementBase,
                 const Trk::MultiComponentState* multiComponentState,
                 const Trk::FitQualityOnSurface* fitQualityOnSurface,
                 const MaterialEffectsBase* materialEffectsOnTrack,
                 const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
                 double modeQoverP)
  :
  TrackStateOnSurface( measurementBase,
           multiComponentState->front().first->clone(),
           fitQualityOnSurface,
           materialEffectsOnTrack,
           types),
  m_multiComponentState(multiComponentState),
  m_mixtureModeQoverP(modeQoverP)
{}



Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface( const Trk::MeasurementBase* measurementBase,
                 const Trk::TrackParameters* trackParameters,
                 const Trk::MultiComponentState* multiComponentState,
                 const Trk::FitQualityOnSurface* fitQualityOnSurface,
                 const MaterialEffectsBase* materialEffectsOnTrack,
                 const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
                 double modeQoverP)
  :
  TrackStateOnSurface( measurementBase,
           trackParameters,
           fitQualityOnSurface,
           materialEffectsOnTrack,
           types),
  
  m_multiComponentState(multiComponentState),
  m_mixtureModeQoverP(modeQoverP)
{}



Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface( const Trk::MeasurementBase* measurementBase,
                 const Trk::MultiComponentState* multiComponentState )
  :
  TrackStateOnSurface( measurementBase,
           multiComponentState->front().first->clone(),
           0,
           0,
           std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>( 1<<(int)TrackStateOnSurface::Measurement) ),  
           m_multiComponentState(multiComponentState),
           m_mixtureModeQoverP(0.)
{}



Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface( const Trk::MultiComponentStateOnSurface& multiComponentStateOnSurface )
  :
  TrackStateOnSurface( ( multiComponentStateOnSurface.measurementOnTrack()     ? multiComponentStateOnSurface.measurementOnTrack()->clone() : 0 ),
           ( multiComponentStateOnSurface.trackParameters()        ? multiComponentStateOnSurface.trackParameters()->clone() : 0 ),
           ( multiComponentStateOnSurface.fitQualityOnSurface()    ? new Trk::FitQualityOnSurface( *( multiComponentStateOnSurface.fitQualityOnSurface() ) ) : 0 ),
           ( multiComponentStateOnSurface.materialEffectsOnTrack() ? multiComponentStateOnSurface.materialEffectsOnTrack()->clone() : 0 ),
             multiComponentStateOnSurface.types() ),
  
  m_multiComponentState( multiComponentStateOnSurface.components() ? multiComponentStateOnSurface.components()->clone() : 0 ),
  m_mixtureModeQoverP( multiComponentStateOnSurface.mixtureModeQoverP() )
{}



Trk::MultiComponentStateOnSurface::~MultiComponentStateOnSurface()
{
  delete m_multiComponentState;
}



Trk::TrackStateOnSurface* Trk::MultiComponentStateOnSurface::clone() const
{

    const Trk::MeasurementBase *measurementBase                  = this->measurementOnTrack()  ? this->measurementOnTrack()->clone() : 0;
    const Trk::TrackParameters *trackParameters                  = this->trackParameters()     ? this->trackParameters()->clone()    : 0;
    const Trk::MultiComponentState *multiComponentState          = this->components()  ? this->components()->clone()         : 0;
    const Trk::FitQualityOnSurface *fitQualityOnSurface          = this->fitQualityOnSurface() ? new Trk::FitQualityOnSurface( *( this->fitQualityOnSurface() ) )  : 0;
    const Trk::MaterialEffectsBase *materialEffectsOnTrack       = this->materialEffectsOnTrack() ? this->materialEffectsOnTrack()->clone() : 0;
    double modeQoverP                                            = this->mixtureModeQoverP();

  return new Trk::MultiComponentStateOnSurface( measurementBase,
                        trackParameters,
                        multiComponentState,
                        fitQualityOnSurface,
                        materialEffectsOnTrack,
                        this->types(),
                        modeQoverP );

}



MsgStream& Trk::operator << ( MsgStream& log, const Trk::MultiComponentStateOnSurface& )
{ return log; }



std::ostream& Trk::operator << ( std::ostream& log, const Trk::MultiComponentStateOnSurface& )
{ return log; }
