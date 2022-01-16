/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  , m_multiComponentState{}
  , m_mixtureModeQoverP(0.)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  std::unique_ptr<const Trk::MeasurementBase> measurementBase,
  MultiComponentState&& multiComponentState,
  std::unique_ptr<const Trk::FitQualityOnSurface> fitQualityOnSurface,
  std::unique_ptr<const MaterialEffectsBase> materialEffectsOnTrack,
  double modeQoverP)
  : TrackStateOnSurface(
      std::move(measurementBase),
      multiComponentState.front().first->uniqueClone(),
      std::move(fitQualityOnSurface),
      std::move(materialEffectsOnTrack),
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(
        1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  std::unique_ptr<const Trk::MeasurementBase> measurementBase,
  std::unique_ptr<const Trk::TrackParameters> trackParameters,
  MultiComponentState&& multiComponentState,
  std::unique_ptr<const Trk::FitQualityOnSurface> fitQualityOnSurface,
  std::unique_ptr<const MaterialEffectsBase> materialEffectsOnTrack,
  double modeQoverP)
  : TrackStateOnSurface(
      std::move(measurementBase),
      std::move(trackParameters),
      std::move(fitQualityOnSurface),
      std::move(materialEffectsOnTrack),
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>(
        1 << (int)TrackStateOnSurface::Measurement))
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  std::unique_ptr<const Trk::MeasurementBase> measurementBase,
  MultiComponentState&& multiComponentState,
  std::unique_ptr<const Trk::FitQualityOnSurface> fitQualityOnSurface,
  std::unique_ptr<const MaterialEffectsBase> materialEffectsOnTrack,
  const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
  double modeQoverP)
  : TrackStateOnSurface(std::move(measurementBase),
                        multiComponentState.front().first->uniqueClone(),
                        std::move(fitQualityOnSurface),
                        std::move(materialEffectsOnTrack),
                        types)
  , m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  std::unique_ptr<const Trk::MeasurementBase> measurementBase,
  std::unique_ptr<const Trk::TrackParameters> trackParameters,
  MultiComponentState&& multiComponentState,
  std::unique_ptr<const Trk::FitQualityOnSurface> fitQualityOnSurface,
  std::unique_ptr<const MaterialEffectsBase> materialEffectsOnTrack,
  const std::bitset<NumberOfTrackStateOnSurfaceTypes>& types,
  double modeQoverP)
  : TrackStateOnSurface(std::move(measurementBase),
                        std::move(trackParameters),
                        std::move(fitQualityOnSurface),
                        std::move(materialEffectsOnTrack),
                        types)
  ,

  m_multiComponentState(std::move(multiComponentState))
  , m_mixtureModeQoverP(modeQoverP)
{}

Trk::MultiComponentStateOnSurface::MultiComponentStateOnSurface(
  std::unique_ptr<const Trk::MeasurementBase> measurementBase,
  MultiComponentState multiComponentState)
  : TrackStateOnSurface(
      std::move(measurementBase),
      multiComponentState.front().first->uniqueClone(),
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
      Trk::MultiComponentStateHelpers::clone(other.components()))
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
      Trk::MultiComponentStateHelpers::clone(other.components());
  }
  return *this;
}

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
