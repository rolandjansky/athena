/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"

#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <iostream>
#include <string>

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface()
  : TrackStateOnSurface(
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(nullptr)
  , m_mixtureModeQoverP(0.)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(const Trk::MeasurementBase* measurementBase,
                                                                const Trk::MultiComponentState* multiComponentState,
                                                                const Trk::FitQualityOnSurface* fitQualityOnSurface,
                                                                const MaterialEffectsBase* materialEffectsOnTrack,
                                                                double modeQoverP)
  : TrackStateOnSurface(
      measurementBase,
      multiComponentState->front().first->clone(),
      fitQualityOnSurface,
      materialEffectsOnTrack,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(1 << (int)TrackStateOnSurface::Measurement))
  ,

  m_multiComponentState(multiComponentState)
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(const Trk::MeasurementBase* measurementBase,
                                                                const Trk::TrackParameters* trackParameters,
                                                                const Trk::MultiComponentState* multiComponentState,
                                                                const Trk::FitQualityOnSurface* fitQualityOnSurface,
                                                                const MaterialEffectsBase* materialEffectsOnTrack,
                                                                double modeQoverP)
  : TrackStateOnSurface(
      measurementBase,
      trackParameters,
      fitQualityOnSurface,
      materialEffectsOnTrack,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(1 << (int)TrackStateOnSurface::Measurement))
  ,

  m_multiComponentState(multiComponentState)
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  const Trk::MultiComponentState* multiComponentState,
  const Trk::FitQualityOnSurface* fitQualityOnSurface,
  const MaterialEffectsBase* materialEffectsOnTrack,
  const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
  double modeQoverP)
  : TrackStateOnSurface(measurementBase,
                        multiComponentState->front().first->clone(),
                        fitQualityOnSurface,
                        materialEffectsOnTrack,
                        types)
  , m_multiComponentState(multiComponentState)
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  const Trk::TrackParameters* trackParameters,
  const Trk::MultiComponentState* multiComponentState,
  const Trk::FitQualityOnSurface* fitQualityOnSurface,
  const MaterialEffectsBase* materialEffectsOnTrack,
  const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
  double modeQoverP)
  : TrackStateOnSurface(measurementBase, trackParameters, fitQualityOnSurface, materialEffectsOnTrack, types)
  ,

  m_multiComponentState(multiComponentState)
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(const Trk::MeasurementBase* measurementBase,
                                                                const Trk::MultiComponentState* multiComponentState)
  : TrackStateOnSurface(
      measurementBase,
      multiComponentState->front().first->clone(),
      nullptr,
      nullptr,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(multiComponentState)
  , m_mixtureModeQoverP(0.)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(const Trk::MultiComponentStateOnSurface& other)
  : TrackStateOnSurface(other)
  , m_multiComponentState(other.components() ? other.components()->clone() : nullptr)
  , m_mixtureModeQoverP(other.mixtureModeQoverP())
{}

Trk::MultiComponentStateOnSurface::~MultiComponentStateOnSurface()
{
  delete m_multiComponentState;
}

Trk::TrackStateOnSurface*
Trk::MultiComponentStateOnSurface::clone() const
{

  const Trk::MeasurementBase* measurementBase =
    this->measurementOnTrack() ? this->measurementOnTrack()->clone() : nullptr;
  const Trk::TrackParameters* trackParameters = this->trackParameters() ? this->trackParameters()->clone() : nullptr;
  const Trk::MultiComponentState* multiComponentState = this->components() ? this->components()->clone() : nullptr;
  const Trk::FitQualityOnSurface* fitQualityOnSurface =
    this->fitQualityOnSurface() ? new Trk::FitQualityOnSurface(*(this->fitQualityOnSurface())) : nullptr;
  const Trk::MaterialEffectsBase* materialEffectsOnTrack =
    this->materialEffectsOnTrack() ? this->materialEffectsOnTrack()->clone() : nullptr;
  double modeQoverP = this->mixtureModeQoverP();

  return new Trk::MultiComponentStateOnSurface(measurementBase,
                                               trackParameters,
                                               multiComponentState,
                                               fitQualityOnSurface,
                                               materialEffectsOnTrack,
                                               this->types(),
                                               modeQoverP);
}

MsgStream&
Trk::operator<<(MsgStream& log, const Trk::MultiComponentStateOnSurface&)
{
  return log;
}

std::ostream&
Trk::operator<<(std::ostream& log, const Trk::MultiComponentStateOnSurface&)
{
  return log;
}
