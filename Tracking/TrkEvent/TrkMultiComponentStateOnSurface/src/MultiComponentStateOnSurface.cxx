/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"

#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include <iostream>
#include <string>

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface()
  : TrackStateOnSurface(
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(
        1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(nullptr)
  , m_mixtureModeQoverP(0.)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  std::unique_ptr<MultiComponentState> multiComponentState,
  const Trk::FitQualityOnSurface* fitQualityOnSurface,
  const MaterialEffectsBase* materialEffectsOnTrack,
  double modeQoverP)
  : TrackStateOnSurface(
      measurementBase,
      multiComponentState->front().first->clone(),
      fitQualityOnSurface,
      materialEffectsOnTrack,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(
        1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  const Trk::TrackParameters* trackParameters,
  std::unique_ptr<MultiComponentState> multiComponentState,
  const Trk::FitQualityOnSurface* fitQualityOnSurface,
  const MaterialEffectsBase* materialEffectsOnTrack,
  double modeQoverP)
  : TrackStateOnSurface(
      measurementBase,
      trackParameters,
      fitQualityOnSurface,
      materialEffectsOnTrack,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(
        1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  std::unique_ptr<MultiComponentState> multiComponentState,
  const Trk::FitQualityOnSurface* fitQualityOnSurface,
  const MaterialEffectsBase* materialEffectsOnTrack,
  const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
  double modeQoverP)
  : TrackStateOnSurface(measurementBase,
                        multiComponentState->front().first->clone(),
                        fitQualityOnSurface,
                        materialEffectsOnTrack,
                        types)
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  const Trk::TrackParameters* trackParameters,
  std::unique_ptr<MultiComponentState> multiComponentState,
  const Trk::FitQualityOnSurface* fitQualityOnSurface,
  const MaterialEffectsBase* materialEffectsOnTrack,
  const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
  double modeQoverP)
  : TrackStateOnSurface(measurementBase,
                        trackParameters,
                        fitQualityOnSurface,
                        materialEffectsOnTrack,
                        types)
  ,

  m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MeasurementBase* measurementBase,
  std::unique_ptr<MultiComponentState> multiComponentState)
  : TrackStateOnSurface(
      measurementBase,
      multiComponentState->front().first->clone(),
      nullptr,
      nullptr,
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(
        1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(0.)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  const Trk::MultiComponentStateOnSurface& other)
  : TrackStateOnSurface(other)
  , m_multiComponentState(
      other.components()
        ? Trk::MultiComponentStateHelpers::clone(*(other.components()))
        : nullptr)
  , m_mixtureModeQoverP(other.mixtureModeQoverP())
{}

Trk::MultiComponentStateOnSurface&
Trk::MultiComponentStateOnSurface::operator=(
  const MultiComponentStateOnSurface& other)
{
  if (this != &other) {
    TrackStateOnSurface::operator=(other);
    m_mixtureModeQoverP = other.m_mixtureModeQoverP;
    m_multiComponentState =
      (other.components()
         ? Trk::MultiComponentStateHelpers::clone(*(other.components()))
         : nullptr);
  }
  return *this;
}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  MultiComponentStateOnSurface&& other) noexcept = default;

Trk::MultiComponentStateOnSurface&
Trk::MultiComponentStateOnSurface::operator=(
  MultiComponentStateOnSurface&& other) noexcept = default;

Trk::MultiComponentStateOnSurface::~MultiComponentStateOnSurface() = default;

Trk::TrackStateOnSurface*
Trk::MultiComponentStateOnSurface::clone() const
{
  return new MultiComponentStateOnSurface(*this);
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
