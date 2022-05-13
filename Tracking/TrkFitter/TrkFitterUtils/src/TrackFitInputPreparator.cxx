/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackFitInputPreparator, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkEventUtils/IdentifierExtractor.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterUtils/MeasBaseIndexComparisonFunction.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"

// using __gnu_cxx::is_sorted;

Trk::TrackFitInputPreparator::TrackFitInputPreparator()
  : m_sortingRefPoint(0., 0., 0.)
{
}

Trk::TrackFitInputPreparator::TrackFitInputPreparator(
  const Amg::Vector3D& gp)
  : m_sortingRefPoint(gp)
{
}



// fill a new track object from track+measurements using flags for sorting and
// outliers.
//
Trk::Track*
Trk::TrackFitInputPreparator::copyToTrack(const Trk::Track& inputTrk,
                                          const Trk::MeasurementSet& inputMbs,
                                          const SortInputFlag doSorting,
                                          const bool reintegrateOutliers)
{

  auto newListOfStates = DataVector<const TrackStateOnSurface>();
  TS_iterator itStates = inputTrk.trackStateOnSurfaces()->begin();
  for (; itStates != inputTrk.trackStateOnSurfaces()->end(); ++itStates)
    if ((*itStates)->type(Trk::TrackStateOnSurface::Measurement) ||
        (*itStates)->type(Trk::TrackStateOnSurface::Outlier)) {
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
        typePattern(0);
      if ((*itStates)->type(Trk::TrackStateOnSurface::Outlier) &&
          !reintegrateOutliers)
        typePattern.set(TrackStateOnSurface::Outlier);
      else
        typePattern.set(TrackStateOnSurface::Measurement);
      newListOfStates.push_back(new TrackStateOnSurface(
        (*itStates)->measurementOnTrack()->uniqueClone(),
        ((*itStates)->trackParameters()
           ? (*itStates)->trackParameters()->uniqueClone()
           : nullptr),
        nullptr,
        ((*itStates)->materialEffectsOnTrack()
           ? (*itStates)->materialEffectsOnTrack()->uniqueClone()
           : nullptr),
        typePattern));
    }
  // add MBs from input list
  MeasurementSet::const_iterator itSet = inputMbs.begin();
  for (; itSet != inputMbs.end(); ++itSet)
    if ((*itSet)) {
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes>
        typePattern(0);
      typePattern.set(TrackStateOnSurface::Measurement);
      newListOfStates.push_back(new TrackStateOnSurface(
        (*itSet)->uniqueClone(), nullptr, nullptr, nullptr, typePattern));
    }

  if (doSorting) {
    Trk::TrackStateOnSurfaceComparisonFunction* CompFunc =
      new Trk::TrackStateOnSurfaceComparisonFunction(
        (*inputTrk.trackParameters()->begin())->momentum());
    if (!__gnu_cxx::is_sorted(
          newListOfStates.begin(), newListOfStates.end(), *CompFunc))
      std::sort(newListOfStates.begin(), newListOfStates.end(), *CompFunc);
    delete CompFunc;
  }
  TrackInfo info;
  return new Trk::Track(info, std::move(newListOfStates), nullptr);
}

// give back the Measurements stripped of a track+measurement input combination.
//
Trk::MeasurementSet
Trk::TrackFitInputPreparator::stripMeasurements(
  const Trk::Track& inputTrk,
  const Trk::MeasurementSet& inputMbs,
  const SortInputFlag /*doSorting*/,
  const bool /*reintegrateOutliers*/)
{
  // FIXME do sorting and outlier treatment

  MeasurementSet newMbSet;
  // collect MBs from Track (speed: assume use for extending track at end)
  DataVector<const MeasurementBase>::const_iterator it =
    inputTrk.measurementsOnTrack()->begin();
  for (; it != inputTrk.measurementsOnTrack()->end(); ++it)
    if ((*it))
      newMbSet.push_back(*it);
  // add MBs from input list
  MeasurementSet::const_iterator itSet = inputMbs.begin();
  for (; itSet != inputMbs.end(); ++itSet)
    if ((*itSet))
      newMbSet.push_back(*itSet);
  return newMbSet;
}

// Create a vector of PrepRawData* from the mixed input of track and single
// PRDs.
//
Trk::PrepRawDataSet
Trk::TrackFitInputPreparator::stripPrepRawData(
  const Trk::Track& inputTrk,
  const Trk::PrepRawDataSet& inputPrds,
  const SortInputFlag doSorting,
  const bool reintegrateOutliers)
{
  // apped PRDs to end of track. For pre-pend make a parameter
  PrepRawDataSet newPrdSet;

  // collect PrepRawData pointers from input track ROTs
  TS_iterator it = inputTrk.trackStateOnSurfaces()->begin();
  for (; it != inputTrk.trackStateOnSurfaces()->end(); ++it) {
    if ((*it)->measurementOnTrack() &&
        (!((*it)->type(TrackStateOnSurface::Outlier)) || reintegrateOutliers)) {
      const Trk::RIO_OnTrack* rot =
        dynamic_cast<const Trk::RIO_OnTrack*>((*it)->measurementOnTrack());
      if (rot) {
        const PrepRawData* prepRD = rot->prepRawData();
        if (prepRD) {
          newPrdSet.push_back(prepRD);
        }
      } 
    }
  }
  Trk::PrepRawDataSet::const_iterator itSet = inputPrds.begin();
  for (; itSet != inputPrds.end(); ++itSet)
    if ((*itSet))
      newPrdSet.push_back(*itSet);

  if (doSorting) {
    Trk::PrepRawDataComparisonFunction PRD_CompFunc(
      (*inputTrk.trackParameters()->begin())->position(),
      (*inputTrk.trackParameters()->begin())->momentum());

    if (!is_sorted(newPrdSet.begin(), newPrdSet.end(), PRD_CompFunc))
      std::sort(newPrdSet.begin(), newPrdSet.end(), PRD_CompFunc);
  }
  return newPrdSet;
}

