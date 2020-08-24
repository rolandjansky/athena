/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackFitInputPreparator, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkFitterUtils/TrackFitInputPreparator.h"
#include "TrkEventUtils/PrepRawDataComparisonFunction.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "TrkEventUtils/IdentifierExtractor.h"
#include "TrkFitterUtils/MeasBaseIndexComparisonFunction.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkFitterUtils/ProtoMaterialEffects.h"

// using __gnu_cxx::is_sorted;

Trk::TrackFitInputPreparator::TrackFitInputPreparator() :
  m_sortingRefPoint(0., 0., 0.),
  m_extrapolator(nullptr)
{
  m_TP_ComparisonFunction 
    = new Trk::TrkParametersComparisonFunction(m_sortingRefPoint);
}

Trk::TrackFitInputPreparator::TrackFitInputPreparator(const Amg::Vector3D& gp, const IExtrapolator* extrapolator) :
  m_sortingRefPoint(gp),
  m_extrapolator(extrapolator)
{
  m_TP_ComparisonFunction 
    = new Trk::TrkParametersComparisonFunction(m_sortingRefPoint);
}

Trk::TrackFitInputPreparator::~TrackFitInputPreparator()
{
  delete m_TP_ComparisonFunction;
}

// fill KF-internal trajectory from old track. Optionally sort and reset outliers.
//
StatusCode Trk::TrackFitInputPreparator::copyToTrajectory
(Trk::Trajectory& trajectory, const Trk::TrackParameters*& referenceParameters,
 const Trk::Track& inputTrk, const SortInputFlag doSorting,
 const bool reintegrateOutliers, const ParticleHypothesis&  partHypo) const
{
  if (!trajectory.empty() ) {
    std::cout << "TrackFitInputPreparator: WARNING bad usage, trajectory not empty!"
              << std::endl;
    return StatusCode::FAILURE;
  }
  if (inputTrk.trackParameters()->empty()) {
    std::cout <<"TrackFitInputPreparator: WARNING bad usage, no TrackParameters o"
              <<"bject on input track, hence can not get fitter seed."<<std::endl;
    return StatusCode::FAILURE;
  }

  // determine and give back reference parameters (the TPs to start filter)
  if (referenceParameters == nullptr) referenceParameters = *inputTrk.trackParameters()->begin();
  MB_IndexVector sortedHitSet;
  TS_iterator it = inputTrk.trackStateOnSurfaces()->begin();
  for ( int index=0 ; it!=inputTrk.trackStateOnSurfaces()->end(); ++it, ++index) {
    if ((*it)->measurementOnTrack()) {
      sortedHitSet.push_back(std::make_pair((*it)->measurementOnTrack(), index));
    }
  }
  Trk::MeasBaseIndexComparisonFunction MB_IndexCompFunc(referenceParameters->position(),
                                                        referenceParameters->momentum());

  int firstFittableStateIndex = (*std::min_element(sortedHitSet.begin(),
                                                   sortedHitSet.end(),
                                                   MB_IndexCompFunc)).second;
  referenceParameters = (*inputTrk.trackStateOnSurfaces())[firstFittableStateIndex]->trackParameters();
  if (referenceParameters == nullptr) {
    referenceParameters = doSorting
    ?  *(std::min_element(inputTrk.trackParameters()->begin(),
                          inputTrk.trackParameters()->end(),
                          *m_TP_ComparisonFunction))
    :  *inputTrk.trackParameters()->begin() ;
  }

  /* If sorting is ON by flag and inputs are discovered not in order, then sort. */
  if (doSorting) {
    bool worstCaseSorting = // invest n*(logN)**2 sorting time only for lowPt
      fabs((*inputTrk.trackParameters()->begin())->parameters()[Trk::qOverP]) > 0.002;
    if ( ! is_sorted( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc ) ) {
      worstCaseSorting ?
        stable_sort( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc ):
        sort( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc );
      /* std::cout << "input track was not sorted. Now the sequence is :"<<endmsg;
      MB_IndexVector::iterator itLog = sortedHitSet.begin();
      for ( ; itLog!=sortedHitSet.end(); ++itLog) std::cout <<" " << (*itLog).second;
      std::cout <<std::endl; */

      MB_IndexVector::iterator itIdx = sortedHitSet.begin();
      for (; itIdx!=sortedHitSet.end(); ++itIdx) {
        bool outlier = (*inputTrk.trackStateOnSurfaces())[(*itIdx).second]->type(TrackStateOnSurface::Outlier);
        const Trk::MeasurementBase* extractMB = 
          ((*inputTrk.trackStateOnSurfaces())[(*itIdx).second]->measurementOnTrack());
        if (extractMB) {
            insertStateIntoTrajectory(  trajectory,
                                        extractMB,
                                        (reintegrateOutliers?false:outlier),
                                        false,
                                        referenceParameters, partHypo );
//           trajectory.push_back
//             (ProtoTrackStateOnSurface(extractMB,
//                                       (reintegrateOutliers?false:outlier),
//                                       false,istate));
//           trajectory.back().identifier(Trk::IdentifierExtractor::extract(extractMB));
        } else std::cout << "TrackFitInputPreparator: WARNING, extraction problem."
                       << std::endl;
      }
    }
  }

  /* if inputs are already sorted or assumed so by convention, trajectory will not
     yet have been filled. In that case, just copy linearly. */
  if (trajectory.empty()) {

    // copy MeasurementBase pointers from input track
     TS_iterator it    = inputTrk.trackStateOnSurfaces()->begin();
    for (; it!=inputTrk.trackStateOnSurfaces()->end(); ++it) {
      if (((*it)->measurementOnTrack())) {
        bool outlier = (*it)->type(TrackStateOnSurface::Outlier);
        insertStateIntoTrajectory(  trajectory,
                                    (*it)->measurementOnTrack(),
                                    (reintegrateOutliers?false:outlier),
                                    false,
                                    referenceParameters, partHypo );
//         trajectory.push_back(ProtoTrackStateOnSurface(((*it)->measurementOnTrack()),
//                                                         (reintegrateOutliers?false:outlier),
//                                                         false,istate++));
//         trajectory.back().identifier(Trk::IdentifierExtractor::extract((*it)->measurementOnTrack()));
      } 
    }
  }

  return StatusCode::SUCCESS;
}

// fill KF-internal trajectory from track + additional meas'ts. Sort and reset outliers.
//
StatusCode Trk::TrackFitInputPreparator::copyToTrajectory
(Trk::Trajectory& trajectory, const Trk::TrackParameters*& referenceParameters,
 const Trk::Track& inputTrk, const Trk::MeasurementSet& inputMbs,
 const SortInputFlag doSorting, const bool reintegrateOutliers,
 const ParticleHypothesis&  partHypo) const
{
  if (!trajectory.empty() ) {
    std::cout << "TrackFitInputPreparator: WARNING bad usage, trajectory not empty!"
              << std::endl;
    return StatusCode::FAILURE;
  }
  if (inputTrk.trackParameters()->empty()) {
    std::cout <<"TrackFitInputPreparator: WARNING bad usage, no TrackParameters o"
              <<"bject on input track, hence can not get fitter seed."<<std::endl;
    return StatusCode::FAILURE;
  }

  // determine and give back reference parameters (the TPs to start filter)
  if (referenceParameters == nullptr) referenceParameters = *inputTrk.trackParameters()->begin();
  MB_IndexVector sortedHitSet;
  TS_iterator it = inputTrk.trackStateOnSurfaces()->begin();
  int index=0;
  for ( ; it!=inputTrk.trackStateOnSurfaces()->end(); ++it, ++index) {
    if ((*it)->measurementOnTrack()) {
      sortedHitSet.push_back(std::make_pair((*it)->measurementOnTrack(), index));
    }
  }
  Trk::MeasBaseIndexComparisonFunction MB_IndexCompFunc(referenceParameters->position(),
                                                        referenceParameters->momentum());
  int firstFittableStateIndex = (*std::min_element(sortedHitSet.begin(),
                                                   sortedHitSet.end(),
                                                   MB_IndexCompFunc)).second;
  referenceParameters = (*inputTrk.trackStateOnSurfaces())[firstFittableStateIndex]->trackParameters();
  if (referenceParameters == nullptr) {
    referenceParameters = doSorting
    ?  *(std::min_element(inputTrk.trackParameters()->begin(),
                          inputTrk.trackParameters()->end(),
                          *m_TP_ComparisonFunction))
    :  *inputTrk.trackParameters()->begin() ;
  }

  /* If sorting is ON by flag and inputs are discovered not in order, then sort. */
  if (doSorting) {
    bool worstCaseSorting = // invest n*(logN)**2 sorting time only for lowPt
      fabs((*inputTrk.trackParameters()->begin())->parameters()[Trk::qOverP]) > 0.002;
    int nInputStates = inputTrk.trackStateOnSurfaces()->size();
    for(MeasurementSet::const_iterator itMb    = inputMbs.begin();
        itMb!=inputMbs.end(); ++itMb, ++index)
      sortedHitSet.push_back(std::make_pair((*itMb),index));
    if ( ! is_sorted( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc ) ) {
      worstCaseSorting ?
      stable_sort( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc ):
      sort( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc );

      /* FIXME remove std output
      std::cout << "added MBset was not a pure track ext, needed to sort it." << std::endl;
      std::cout << "The sorted input sequence is :";
      MB_IndexVector::iterator itLog = sortedHitSet.begin();
      for ( ; itLog!=sortedHitSet.end(); ++itLog) std::cout <<" " << (*itLog).second;
      std::cout <<std::endl;
      */

      MB_IndexVector::iterator itIdx = sortedHitSet.begin();
      for ( ; itIdx!=sortedHitSet.end(); ++itIdx) {
        if ((*itIdx).second < nInputStates ) {
          bool outlier = (*inputTrk.trackStateOnSurfaces())[(*itIdx).second]->type(TrackStateOnSurface::Outlier);
          const Trk::MeasurementBase* extractMB = 
            ((*inputTrk.trackStateOnSurfaces())[(*itIdx).second]->measurementOnTrack());
            if (extractMB) {
                insertStateIntoTrajectory(  trajectory,
                                            extractMB,
                                            (reintegrateOutliers?false:outlier),
                                            false,
                                            referenceParameters, partHypo );
            }
//             if (extractMB) trajectory.push_back
//               (ProtoTrackStateOnSurface(extractMB,
//                                         (reintegrateOutliers?false:outlier),
//                                         false,istate));
            else std::cout << "TrackFitInputPreparator: WARNING, extraction problem" << std::endl;
        } else {
            insertStateIntoTrajectory(  trajectory,
                                        inputMbs[(*itIdx).second-nInputStates],
                                        false,
                                        false,
                                        referenceParameters, partHypo );
//           trajectory.push_back(ProtoTrackStateOnSurface
//                                (inputMbs[(*itIdx).second-nInputStates],false,false,istate));
        }
        // Phew! Can this be made with less effort?
//         trajectory.back().identifier(Trk::IdentifierExtractor::extract
//                                      (trajectory.back().measurement()));
      }
    }
  }

  
  /* if inputs are already sorted or assumed so by convention, trajectory will not
     yet have been filled. In that case, just copy linearly. */

  if (trajectory.empty()) {
    TS_iterator it    = inputTrk.trackStateOnSurfaces()->begin();
    for ( ; it!=inputTrk.trackStateOnSurfaces()->end(); ++it)
      if (((*it)->measurementOnTrack())) {
        bool outlier = (*it)->type(TrackStateOnSurface::Outlier);
        insertStateIntoTrajectory(  trajectory,
                                    (*it)->measurementOnTrack(),
                                    (reintegrateOutliers?false:outlier),
                                    false,
                                    referenceParameters, partHypo );
//         trajectory.push_back(ProtoTrackStateOnSurface(((*it)->measurementOnTrack()),
//                                                       (reintegrateOutliers?false:outlier),
//                                                       false,istate++));
//         trajectory.back().identifier(Trk::IdentifierExtractor::extract((*it)->measurementOnTrack()));
      }
    for(MeasurementSet::const_iterator it    = inputMbs.begin();
        it!=inputMbs.end(); ++it) {
        insertStateIntoTrajectory( trajectory, (*it), false, false, referenceParameters, partHypo );
//       trajectory.push_back(ProtoTrackStateOnSurface((*it),false,false,istate));
//       trajectory.back().identifier(Trk::IdentifierExtractor::extract((*it)));
    }
  }
  return StatusCode::SUCCESS;
}

// fill KF-internal trajectory from two input Tracks using flags for sort and outliers.
//
StatusCode Trk::TrackFitInputPreparator::copyToTrajectory
(Trk::Trajectory& trajectory, const Trk::TrackParameters*& referenceParameters,
 const Trk::Track& inputTrk1, const Trk::Track& inputTrk2,
 const SortInputFlag doSorting, const bool reintegrateOutliers, 
 const ParticleHypothesis&  partHypo) const
{
  if (!trajectory.empty() ) {
    std::cout << "TrackFitInputPreparator: WARNING bad usage, trajectory not empty!"
              << std::endl;
    return StatusCode::FAILURE;
  }
  if (inputTrk1.trackParameters()->empty()) {
    std::cout <<"TrackFitInputPreparator: WARNING bad usage, no TrackParameters o"
              <<"bject on input track, hence can not get fitter seed."<<std::endl;
    return StatusCode::FAILURE;
  }

  // determine and give back reference parameters (the TPs to start filter)
  if (referenceParameters == nullptr) referenceParameters = *inputTrk1.trackParameters()->begin();
  MB_IndexVector sortedHitSet;
  TS_iterator it   = inputTrk1.trackStateOnSurfaces()->begin();
  int nInputStates = inputTrk1.trackStateOnSurfaces()->size();
  int index=0;
  for ( ; it!=inputTrk1.trackStateOnSurfaces()->end(); ++it, ++index) {
    if ((*it)->measurementOnTrack()) {
      sortedHitSet.push_back(std::make_pair((*it)->measurementOnTrack(), index));
    }
  }
  it = inputTrk2.trackStateOnSurfaces()->begin();
  for ( ; it!=inputTrk2.trackStateOnSurfaces()->end(); ++it, ++index) {
    if ((*it)->measurementOnTrack()) {
      sortedHitSet.push_back(std::make_pair((*it)->measurementOnTrack(), index));
    }
  }
  Trk::MeasBaseIndexComparisonFunction FindFirstFunc(referenceParameters->position(),
                                                        referenceParameters->momentum());
  int firstMbIndex = (*std::min_element(sortedHitSet.begin(),
                                        sortedHitSet.end(),
                                        FindFirstFunc)).second;
  referenceParameters = (firstMbIndex < nInputStates)
    ? (*inputTrk1.trackStateOnSurfaces())[firstMbIndex]->trackParameters()
    : (*inputTrk2.trackStateOnSurfaces())[firstMbIndex]->trackParameters();
  if (referenceParameters == nullptr) {
    referenceParameters = doSorting
    ?  *(std::min_element(inputTrk1.trackParameters()->begin(),
                          inputTrk1.trackParameters()->end(),
                          *m_TP_ComparisonFunction))
    :  *inputTrk1.trackParameters()->begin() ;
  }

  /* If sorting is ON by flag and inputs are discovered not in order, then sort. */
  if (doSorting) {
    bool worstCaseSorting = // invest n*(logN)**2 sorting time only for lowPt
      fabs((*inputTrk1.trackParameters()->begin())->parameters()[Trk::qOverP]) > 0.002;

    Trk::MeasBaseIndexComparisonFunction MB_IndexCompFunc(referenceParameters->position(),
                                                          referenceParameters->momentum());
    if ( ! is_sorted( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc ) ) {
      worstCaseSorting ?
      stable_sort( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc ):
      sort( sortedHitSet.begin(), sortedHitSet.end(), MB_IndexCompFunc );

      /* FIXME remove std output
      std::cout << "hits on two tracks are not in proper order, "
                << "needed to sort them." << std::endl;
      std::cout << "The sorted input sequence is :";
      MB_IndexVector::iterator itLog = sortedHitSet.begin();
      for ( ; itLog!=sortedHitSet.end(); ++itLog) std::cout <<" " << (*itLog).second;
      std::cout <<std::endl;*/

      MB_IndexVector::iterator itIdx = sortedHitSet.begin();
      for ( ; itIdx!=sortedHitSet.end(); ++itIdx) {
        if ((*itIdx).second < nInputStates ) { // extract from track #1
          bool outlier = (*inputTrk1.trackStateOnSurfaces())[(*itIdx).second]->type(TrackStateOnSurface::Outlier);
          const Trk::MeasurementBase* extractMB = 
            ((*inputTrk1.trackStateOnSurfaces())[(*itIdx).second]->measurementOnTrack());
          if (extractMB) {
            insertStateIntoTrajectory(  trajectory,
                                        extractMB,
                                        (reintegrateOutliers?false:outlier),
                                        false,
                                        referenceParameters, partHypo );
//             trajectory.push_back
//               (ProtoTrackStateOnSurface(extractMB,
//                                         (reintegrateOutliers?false:outlier),
//                                         false,istate));
//             trajectory.back().identifier(Trk::IdentifierExtractor::extract(extractMB));
          } else std::cout << "TrackFitInputPreparator: WARNING, extraction "
                         << "problem with track #1" << std::endl;
        } else { // extract from track #2
          bool outlier = (*inputTrk2.trackStateOnSurfaces())[(*itIdx).second]->type(TrackStateOnSurface::Outlier);
          const Trk::MeasurementBase* extractMB = 
            ((*inputTrk2.trackStateOnSurfaces())[(*itIdx).second]->measurementOnTrack());
          if (extractMB) {
            insertStateIntoTrajectory(  trajectory,
                                        extractMB,
                                        (reintegrateOutliers?false:outlier),
                                        false,
                                        referenceParameters, partHypo );
//             trajectory.push_back
//               (ProtoTrackStateOnSurface(extractMB,
//                                         (reintegrateOutliers?false:outlier),
//                                         false,istate));
//             trajectory.back().identifier(Trk::IdentifierExtractor::extract(extractMB));
          } else std::cout << "TrackFitInputPreparator: WARNING, extraction "
                         << "problem with track #2" << std::endl;
        }
      }
    }
  }

  
  /* if inputs are already sorted or assumed so by convention, trajectory will not
     yet have been filled. In that case, just copy linearly. */

  if (trajectory.empty()) {
    TS_iterator it    = inputTrk1.trackStateOnSurfaces()->begin();
    for ( ; it!=inputTrk1.trackStateOnSurfaces()->end(); ++it)
      if (((*it)->measurementOnTrack())) {
        bool outlier = (*it)->type(TrackStateOnSurface::Outlier);
        insertStateIntoTrajectory(  trajectory,
                                    (*it)->measurementOnTrack(),
                                    (reintegrateOutliers?false:outlier),
                                    false,
                                    referenceParameters, partHypo );
//         trajectory.push_back(ProtoTrackStateOnSurface(((*it)->measurementOnTrack()),
//                                                       (reintegrateOutliers?false:outlier),
//                                                       false,istate++));
//         trajectory.back().identifier(Trk::IdentifierExtractor::extract((*it)->measurementOnTrack()));
      }
    it    = inputTrk2.trackStateOnSurfaces()->begin();
    for ( ; it!=inputTrk2.trackStateOnSurfaces()->end(); ++it)
      if (((*it)->measurementOnTrack())) {
        bool outlier = (*it)->type(TrackStateOnSurface::Outlier);
        insertStateIntoTrajectory(  trajectory,
                                    (*it)->measurementOnTrack(),
                                    (reintegrateOutliers?false:outlier),
                                    false,
                                    referenceParameters, partHypo );
//         trajectory.push_back(ProtoTrackStateOnSurface(((*it)->measurementOnTrack()),
//                                                       (reintegrateOutliers?false:outlier),
//                                                       false,istate++));
//         trajectory.back().identifier(Trk::IdentifierExtractor::extract((*it)->measurementOnTrack()));
      }
    // std::cout << "found list of " <<istate<<" measurements for fitting." << std::endl;
  }
  return StatusCode::SUCCESS;
}

// fill a new track object from track+measurements using flags for sorting and outliers.
//
Trk::Track* Trk::TrackFitInputPreparator::copyToTrack
(const Trk::Track& inputTrk, const Trk::MeasurementSet& inputMbs,
 const SortInputFlag doSorting, const bool reintegrateOutliers) const
{

  // FIXME do i need this?
  //Amg::Vector3D startGP  = (*inputTrk.trackParameters()->begin())->position();
  //Amg::Vector3D refDir = (*(inputTrk.trackParameters()->end()-1))->position() - startGP;

  DataVector<const TrackStateOnSurface> *newListOfStates
    = new DataVector<const TrackStateOnSurface>;
  TS_iterator itStates = inputTrk.trackStateOnSurfaces()->begin();
  for (;itStates!=inputTrk.trackStateOnSurfaces()->end();++itStates) 
    if ( (*itStates)->type(Trk::TrackStateOnSurface::Measurement) ||
         (*itStates)->type(Trk::TrackStateOnSurface::Outlier)     ) {
      std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
      if ( (*itStates)->type(Trk::TrackStateOnSurface::Outlier) &&
           ! reintegrateOutliers ) typePattern.set(TrackStateOnSurface::Outlier);
        else typePattern.set(TrackStateOnSurface::Measurement);
      newListOfStates->push_back(new TrackStateOnSurface
                           ((*itStates)->measurementOnTrack()->clone(),
                           ( (*itStates)->trackParameters()           ? 
                             (*itStates)->trackParameters()->clone()  :
                             nullptr ),
                           nullptr,
                           ( (*itStates)->materialEffectsOnTrack()    ?
                             (*itStates)->materialEffectsOnTrack()->clone() :
                             nullptr ),typePattern));
    }
  // add MBs from input list
  MeasurementSet::const_iterator itSet    = inputMbs.begin();
  for ( ; itSet!=inputMbs.end(); ++itSet) if ((*itSet)) {
    std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(TrackStateOnSurface::Measurement);
    newListOfStates->push_back(new TrackStateOnSurface((*itSet)->clone(),nullptr,nullptr,nullptr,typePattern));
  }

  if (doSorting) {
    Trk::TrackStateOnSurfaceComparisonFunction* CompFunc = 
      new Trk::TrackStateOnSurfaceComparisonFunction
      ( (*inputTrk.trackParameters()->begin())->momentum() ) ;
    if(!__gnu_cxx::is_sorted(newListOfStates->begin(),newListOfStates->end(), *CompFunc))
      std::sort(newListOfStates->begin(),newListOfStates->end(), *CompFunc);
    delete CompFunc;
  }
  TrackInfo info;
  return new Trk::Track(info,newListOfStates,nullptr);
}

// give back the Measurements stripped of a track+measurement input combination.
//
Trk::MeasurementSet Trk::TrackFitInputPreparator::stripMeasurements
(const Trk::Track& inputTrk, const Trk::MeasurementSet& inputMbs,
 const SortInputFlag /*doSorting*/, const bool /*reintegrateOutliers*/) const
{
  // FIXME do sorting and outlier treatment

  MeasurementSet newMbSet;
  // collect MBs from Track (speed: assume use for extending track at end)
  DataVector<const MeasurementBase>::const_iterator it
    = inputTrk.measurementsOnTrack()->begin(); 
  for ( ; it!=inputTrk.measurementsOnTrack()->end(); ++it) 
    if ((*it)) newMbSet.push_back ( *it );
  // add MBs from input list
  MeasurementSet::const_iterator itSet    = inputMbs.begin();
  for ( ; itSet!=inputMbs.end(); ++itSet) 
    if ((*itSet)) newMbSet.push_back ( *itSet );
  return newMbSet;

}

// Create a vector of PrepRawData* from the mixed input of track and single PRDs.
//
Trk::PrepRawDataSet Trk::TrackFitInputPreparator::stripPrepRawData
(const Trk::Track& inputTrk, const Trk::PrepRawDataSet& inputPrds,
 const SortInputFlag doSorting, const bool reintegrateOutliers) const
{
  // apped PRDs to end of track. For pre-pend make a parameter
  PrepRawDataSet newPrdSet;

  // collect PrepRawData pointers from input track ROTs
  TS_iterator it = inputTrk.trackStateOnSurfaces()->begin();
  for ( ; it!=inputTrk.trackStateOnSurfaces()->end(); ++it) {
    if ( (*it)->measurementOnTrack() &&
         ( !((*it)->type(TrackStateOnSurface::Outlier)) || reintegrateOutliers) )
      {
        const Trk::RIO_OnTrack* rot
          = dynamic_cast<const Trk::RIO_OnTrack*>((*it)->measurementOnTrack());
        if (rot) {
          const PrepRawData* prepRD = rot->prepRawData();
          if (!prepRD) {
            std::cout << "TrackFitInputPreparator: WARNING, RIO_OnTrack->prepRawRata() "
                      << "returns no object, ignore... " << std::endl;
          } else newPrdSet.push_back( prepRD );
        } else {
          std::cout << "ITrackFitter::fit(TRK,PRDset) implementation "
                    << "is not designed for other than track with 100% RIO_OnTrack."
                    << std::endl;
        }
      }
  }
  Trk::PrepRawDataSet::const_iterator itSet    = inputPrds.begin();
  for ( ; itSet!=inputPrds.end(); ++itSet)
    if ((*itSet)) newPrdSet.push_back ( *itSet );


  if (doSorting) {
    // sort PrepRawData Set in increasing distance from origin using STL sorting
    Trk::PrepRawDataComparisonFunction* PRD_CompFunc = new Trk::PrepRawDataComparisonFunction
      ((*inputTrk.trackParameters()->begin())->position(),
       (*inputTrk.trackParameters()->begin())->momentum());
    if ( ! is_sorted( newPrdSet.begin(), newPrdSet.end(), *PRD_CompFunc ) )
      std::sort( newPrdSet.begin(), newPrdSet.end(), *PRD_CompFunc );
    delete PRD_CompFunc;
  }
  return newPrdSet;
}

void Trk::TrackFitInputPreparator::insertStateIntoTrajectory(Trajectory& trajectory,
                            const MeasurementBase* measurement,
                            bool  isOutlier,
                            bool  ownsMeasurement,
                            const TrackParameters* initialParameters,
                            const ParticleHypothesis&  partHypo ) const {    
    const TrackParameters* trkPar = nullptr;
    Trk::ProtoMaterialEffects* matEffOnMeasurementSurface = nullptr;
    //std::cout << "InputPrep: partHypo:" << partHypo << std::endl;
    /// collect material layers between previous state and state to insert:
    if (m_extrapolator) {
        if (!trajectory.empty()) {
            /// get previous state:
            const TrackParameters* prevPar = trajectory.back().referenceParameters();
            if (prevPar->covariance()) prevPar = CREATE_PARAMETERS(*prevPar,prevPar->parameters(),nullptr);
            const std::vector< const Trk::TrackStateOnSurface * >* collectedTSOS = m_extrapolator->extrapolateM(
                        *prevPar,
                        measurement->associatedSurface(),
                        Trk::alongMomentum,
                        false,
                        //Trk::nonInteracting
                        partHypo
                        //Trk::pion   // FIXME: decide on particle hypothesis to use!
                        //track->info().particleHypothesis()
            );
            if (trajectory.back().referenceParameters()->covariance()) delete prevPar; // balance CREATE_PARS from a few lines earlier 
            if (collectedTSOS) {
                // copy into ProtoTrackStateOnSurface for memory management, ignoring the last:
                for (unsigned int i = 0 ; i < collectedTSOS->size() -1; i++) {
//                 for (auto tsosIter = collectedTSOS->begin() ; tsosIter != collectedTSOS->end(); tsosIter++) {
//                     if (tsosIter==collectedTSOS->end()-1) break;
//                     const Trk::TrackStateOnSurface* tsos = (*tsosIter);
                    const Trk::TrackStateOnSurface* tsos = collectedTSOS->at(i);
//                     std::cout << "InputPrep:" << tsos->dumpType() << std::endl;
                    if (tsos->materialEffectsOnTrack()) {
                        const MaterialEffectsOnTrack* meot = dynamic_cast<const MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
                        if (meot) {
                            trajectory.push_back( ProtoTrackStateOnSurface( nullptr,
                                                    false,
                                                    false,
                                                    trajectory.size()+1,
                                                    nullptr,
                                                    tsos->trackParameters()->clone()
                                                  ));
                            trajectory.back().checkinMaterialEffects(new Trk::ProtoMaterialEffects(meot));
                            trajectory.back().isOutlier(TrackState::Scatterer);
        //                     Identifier id =  tsos->surface().associatedDetectorElementIdentifier();
        //                     msg ( MSG::INFO ) << "added material TSoS:" << id << " " <<  &(tsos->surface()) << " " << m_idHelper->print_to_string(id) << " " << tsos->dumpType() << endmsg;
        //                     msg ( MSG::INFO ) << "    trk pos=" <<tsos->trackParameters()->position() << " mom=" << tsos->trackParameters()->momentum() << " rho=" << tsos->trackParameters()->position().perp() << endmsg;
                        } // end if meot
                    }
                    delete tsos;
                }
//                 std::cout << "InputPrep:" << collectedTSOS->back()->dumpType() << std::endl;
                trkPar = collectedTSOS->back()->trackParameters()->clone();
                if (collectedTSOS->back()->materialEffectsOnTrack()) {
                    const MaterialEffectsOnTrack* meot = dynamic_cast<const MaterialEffectsOnTrack*>(collectedTSOS->back()->materialEffectsOnTrack());
                    if (meot) {
                        matEffOnMeasurementSurface = new Trk::ProtoMaterialEffects(meot);
                    }
                } // end if last TSoS has material effects
                delete collectedTSOS->back();
                delete collectedTSOS; collectedTSOS = nullptr;
            } // end if collectedTSOS
        } // end if trajectory.size()>0
        if (!trkPar) {
            /// extrapolate to first state in trajectory
            trkPar = m_extrapolator->extrapolate(   *initialParameters,
                                                    measurement->associatedSurface(),
                                                    Trk::alongMomentum,
                                                    false,
                                                    partHypo
                                                    //Trk::nonInteracting
                                                    //Trk::pion   // FIXME: decide on particle hypothesis to use!
                                                    //track->info().particleHypothesis()
                                                );
            if (!trkPar) {
                std::cout << "TrackFitInputPreparator: WARNING, extrapolation problem." << std::endl;
            }
        }//end if (!trkPar)
    } // end if m_extrapolator
    /// Fill the measurement into the trajectory:
    trajectory.push_back( ProtoTrackStateOnSurface( measurement,
                                                    isOutlier,
                                                    ownsMeasurement,
                                                    trajectory.size()+1,
                                                    nullptr,
                                                    trkPar
                                                  ));
    trajectory.back().identifier(Trk::IdentifierExtractor::extract(measurement));
    if (matEffOnMeasurementSurface) {
        trajectory.back().checkinMaterialEffects(matEffOnMeasurementSurface);
    }

}
