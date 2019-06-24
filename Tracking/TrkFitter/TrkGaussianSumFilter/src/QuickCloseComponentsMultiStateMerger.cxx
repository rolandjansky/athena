/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.cxx  -  description
      ---------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Implementation code for QuickCloseComponentsMultiStateMerger
                       class
*********************************************************************************/

#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/IComponentSeparationDistance.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/Chrono.h"
#include <limits>
#include <map>


#if !defined(__GNUC__)
# define __builtin_assume_aligned(X, N) X
#else
# if defined(__clang__)
#  if !__has_builtin(__builtin_assume_aligned)
#    define __builtin_assume_aligned(X, N) X
#  endif
# endif
#endif


Trk::QuickCloseComponentsMultiStateMerger::QuickCloseComponentsMultiStateMerger(const std::string& type, 
                                                                                const std::string& name, 
                                                                                const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_chronoSvc("ChronoStatSvc", name)
{

  declareInterface<IMultiComponentStateMerger>(this);
}

Trk::QuickCloseComponentsMultiStateMerger::~QuickCloseComponentsMultiStateMerger()
{}


StatusCode Trk::QuickCloseComponentsMultiStateMerger::initialize()
{
  ATH_MSG_VERBOSE("Initialising Close Components Merger");

  // Request the Chrono Service
  if ( m_chronoSvc.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_chronoSvc );
    return StatusCode::FAILURE;
  } else
     ATH_MSG_INFO( "Retrieved service " << m_chronoSvc );

  // Retrieve the distance tool
  if ( m_distance.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve component distance AlgTool ... Exiting!" );
    return StatusCode::FAILURE;
  }

  // Request an instance of the MultiComponentStateCombiner
  if ( m_stateCombiner.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve an instance of the multi-component state combiner... Exiting!" );
    return StatusCode::FAILURE;
  }

  // Request an instance of the MultiComponentStateAssembler
  if ( m_stateAssembler.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve an instance of the mutli-component state assembler... Exiting!" );
    return StatusCode::FAILURE;
  }

  if ( m_maximumNumberOfComponents <= 0 ){
    ATH_MSG_FATAL( "Attempting to merge multi-state into zero components... stop being silly!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO(  "Initialisation of " << type() << " under instance " << name() << " was successful" );
  return StatusCode::SUCCESS;

}

StatusCode Trk::QuickCloseComponentsMultiStateMerger::finalize()
{

  ATH_MSG_INFO( "Finalisation of " << type() << " under instance " << name() << " was successful" );

  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState* 
Trk::QuickCloseComponentsMultiStateMerger::merge(const Trk::MultiComponentState& unmergedState) const
{

  ATH_MSG_VERBOSE(  "Merging state with " << unmergedState.size() << " components" );
  //Assembler Cache
  IMultiComponentStateAssembler::Cache cache;
  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset(cache);

  if (unmergedState.size() <= m_maximumNumberOfComponents){
    ATH_MSG_VERBOSE( "State is already sufficiently small... no component reduction required" );
    return unmergedState.clone();
  }

  if ( !isAssemblerReset ){
    ATH_MSG_ERROR( "Could not reset the state assembler... returning 0" );
    return 0;
  }

  if ( unmergedState.empty() ){
    ATH_MSG_ERROR( "Attempting to merge multi-state with zero components" );
    return 0;
  }

  bool componentWithoutMeasurement = false;

  Trk::MultiComponentState::const_iterator component = unmergedState.begin();

  // Scan all components for covariance matricies. If one or more component 
  // is missing an error matrix, component reduction is impossible.
  for ( ; component != unmergedState.end(); ++component){
    const AmgSymMatrix(5)* measuredCov = component->first->covariance();
    if ( !measuredCov ){
      componentWithoutMeasurement = true;
      break;
    }
  }

  const Trk::TrackParameters* combinedState = unmergedState.begin()->first->clone();
  const Trk::ComponentParameters reducedState( combinedState, 1. );
  if ( componentWithoutMeasurement ){
    ATH_MSG_DEBUG(  "A track parameters object is without measurement... reducing state to single component" );
    return ( new Trk::MultiComponentState( reducedState ) );
  }

  delete combinedState;
  return mergeFullDistArray(cache,unmergedState);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Trk::MultiComponentState* 
Trk::QuickCloseComponentsMultiStateMerger::mergeFullDistArray(IMultiComponentStateAssembler::Cache& cache,
                                                              const Trk::MultiComponentState& mcs) const
{


  const int n = mcs.size();
  const int nn2 = (n+1)*n / 2;

  Aligned<float> distances(nn2);  // Array to store all of the distances between componants
  Aligned<int>   indexToI(nn2);   // The i  & J of each distances so that i don't have to calculate them
  Aligned<int>   indexToJ(nn2);

  Aligned<float> qonp(n);         // Array of qonp for each component
  Aligned<float> qonpCov(n);      // Array of Cov(qonp,qonp) for each component
  Aligned<float> qonpG(n);        // Array of 1/Cov(qonp,qonp) for each component


  // Initlise all values
  for(int i = 0; i < n; ++i ){
    qonp[i] = 0;
    qonpCov[i] = 0;
    qonpG[i] = 1e10;
  }

  for(int i = 0; i < nn2; ++i ){
    distances[i] = std::numeric_limits<float>::max();
    indexToI[i] = -1;
    indexToJ[i] = -1;
  }

  // Calculate indicies
  for( int i = 0; i < n; ++i ){
    int indexConst = (i+1)*i/2;
    for( int j = 0; j <= i; ++j ){
      int index =  indexConst + j ;
      indexToI[index] = i;
      indexToJ[index] = j;
    }
  }

  // Create an array of all components to be merged
  std::vector <const Trk::ComponentParameters*> statesToMerge(n);
  Trk::MultiComponentState::const_iterator component = mcs.begin();
  int  ii = 0;
  for ( ; component != mcs.end(); ++component ) {
    if (ii >= n ){
      ATH_MSG_ERROR("ii >= n ????");
      break;
    }

    const AmgSymMatrix(5)* measuredCov  = (*component).first->covariance();
    const Amg::VectorX&    parameters   = (*component).first->parameters();
    //If no covariance matrix we dont use component
    if (!measuredCov){
      qonpG[ii] = 1e10;
      continue;
    }

    //Fill in infomation
    statesToMerge[ii] = (*component).clone();
    qonp[ii]    =  parameters[Trk::qOverP];
    qonpCov[ii] =  (*measuredCov)(Trk::qOverP,Trk::qOverP);
    qonpG[ii]   =  qonpCov[ii] > 0 ? 1./qonpCov[ii] : 1e10;
    ++ii;
  }

  if (ii == 0){
    ATH_MSG_WARNING("This should not happen");
  }

  //Calculate distances for all pairs
  // This loop can be vectorised
  calculateAllDistances(  qonp, qonpCov, qonpG, distances, n);

  //Loop over all componants until you reach the target amount
  unsigned int numberOfComponents = n;
  int minIndex = -1;
  int nextMinIndex = -1;

  while (numberOfComponents > m_maximumNumberOfComponents){

    //Searching for the minimum distances is slowest part of the loop
    //lets try and speed it up by reducing the number searches by 2
    if( nextMinIndex < 1 ){
      //Run search again
      std::pair <int ,int > min = findMinimumIndex( distances, nn2 );
      minIndex     = min.first;
      nextMinIndex = min.second;
    } else {
      //Use new min index
      minIndex = nextMinIndex;
      nextMinIndex = -1;
    }

    int mini = indexToI[minIndex];
    int minj = indexToJ[minIndex];


    if(mini==minj) {
      ATH_MSG_ERROR(  "Err keys are equal key1 " << mini << " key2 " << minj );
      break;
    }

    //merge componants
    Trk::MultiComponentState* componentsToBeMerged =  new Trk::MultiComponentState();
    ATH_MSG_VERBOSE(  "key1 " << mini << " key2 " << minj );
    componentsToBeMerged->push_back( *statesToMerge[mini] );
    componentsToBeMerged->push_back( *statesToMerge[minj] );

    // Combine the closest distance components
    const Trk::ComponentParameters* combinedComponents = m_stateCombiner->combineWithWeight( *componentsToBeMerged );

    ATH_MSG_VERBOSE( "Weight of new component "<< combinedComponents->second );

    // Erase these components from the unmerged components map. 
    // These needs to be deleted also as new memory is assigned in the combiner
    // This will also delete the components in the vector statesToMerge
    delete componentsToBeMerged;
    delete statesToMerge[mini]; statesToMerge[mini]=0;
    delete statesToMerge[minj]; statesToMerge[minj]=0;


    //Replace components in vector
    statesToMerge[mini] = combinedComponents;

    const AmgSymMatrix(5)* measuredCov  = combinedComponents->first->covariance();
    const Amg::VectorX&    parameters   = combinedComponents->first->parameters();
    qonp[mini]    =  parameters[Trk::qOverP];
    qonpCov[mini] =  (*measuredCov)(Trk::qOverP,Trk::qOverP);
    qonpG[mini]   =  qonpCov[mini] > 0 ? 1./qonpCov[mini] : 1e10;
    qonp[minj]    = 0;
    qonpCov[minj] = 0;
    qonpG[minj]   = 1e10;


    //Reset old weights;
    resetDistances( distances, mini, n);

    //If that element has been removed already the next min pair is invalid and can't be used
    if( nextMinIndex > 0  &&
       (  mini == indexToI[nextMinIndex] || minj == indexToJ[nextMinIndex]
       || minj == indexToI[nextMinIndex] || mini == indexToJ[nextMinIndex] ) ){
       nextMinIndex = -1;
    }


    //Calculate weights for new component
    int possibleNextMin  = recalculateDistances(  qonp, qonpCov, qonpG, distances, mini, n);
    if( nextMinIndex > 0 && possibleNextMin > 0  && distances[ possibleNextMin ] < distances[ nextMinIndex ]){
      nextMinIndex = possibleNextMin;
    }


    //Reset old weights;
    resetDistances( distances, minj, n);

    // Decrement the number of components
    --numberOfComponents;
  }


  // Build final state containing both merged and unmerged components

  for (auto state: statesToMerge) {
    if(state == 0) continue;

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent( cache,*state );

    if ( !componentAdded )
      ATH_MSG_WARNING( "Component could not be added to the state in the assembler" );
  }

  const Trk::MultiComponentState* mergedState = m_stateAssembler->assembledState(cache);

  ATH_MSG_VERBOSE( "Number of components in merged state: " << mergedState->size() );

  // Memory clean up
  for(auto state:statesToMerge){
    if(state){
      delete state->first;
      delete state;
    }
  }
  statesToMerge.clear();

  return mergedState;
}




void Trk::QuickCloseComponentsMultiStateMerger::resetDistances( floatPtrRestrict  distancesIn, 
                                                                const int mini,
                                                                const  int  n)const{

  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);

  int j = mini;
  int indexConst = (j+1)*j/2;
  for( int i = 0; i < j; ++i ){
    distances[ indexConst+ i ] = std::numeric_limits<float>::max();
  }

  for( int i = j; i < n; ++i ){
    int index =  (i+1)*i/2+j;
    distances[ index ] = std::numeric_limits<float>::max();
  }

}

void Trk::QuickCloseComponentsMultiStateMerger::calculateAllDistances( floatPtrRestrict qonpIn,  
                                                                       floatPtrRestrict  qonpCovIn,  
                                                                       floatPtrRestrict qonpGIn, 
                                                                       floatPtrRestrict distancesIn, 
                                                                       int  n) const
{

  float *qonp = (float*)__builtin_assume_aligned(qonpIn, 32);
  float *qonpCov = (float*)__builtin_assume_aligned(qonpCovIn, 32);
  float *qonpG = (float*)__builtin_assume_aligned(qonpGIn, 32);
  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);

  for( int i = 0; i < n; ++i ){
    int indexConst = (i+1)*i/2;
    float qonpi =qonp[i];
    float qonpCovi = qonpCov[i];
    float qonpGi = qonpG[i];
    for( int j = 0; j < i; ++j ){
      float parametersDifference = qonpi - qonp[j];
      float covarianceDifference = qonpCovi - qonpCov[j];
      float G_difference = qonpG[j] - qonpGi;
      float G_sum        = qonpGi + qonpG[j];
      distances[ indexConst + j ] = covarianceDifference * G_difference + 
        parametersDifference * G_sum * parametersDifference;
    }
  }
}


int Trk::QuickCloseComponentsMultiStateMerger::recalculateDistances( floatPtrRestrict qonpIn,  
                                                                     floatPtrRestrict qonpCovIn,  
                                                                     floatPtrRestrict qonpGIn, 
                                                                     floatPtrRestrict distancesIn, 
                                                                     int mini, 
                                                                     int n) const
{

  float *qonp = (float*)__builtin_assume_aligned(qonpIn, 32);
  float *qonpCov = (float*)__builtin_assume_aligned(qonpCovIn, 32);
  float *qonpG = (float*)__builtin_assume_aligned(qonpGIn, 32);
  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);

  int j = mini;
  int indexConst = (j+1)*j/2;

  int minIndex = 0; // The distance will always be 1e30 for this entry;
  float  minDistance = std::numeric_limits<float>::max();

  for( int i = 0; i < j; ++i ){
    if(qonpCov[i] == 0){
      distances[indexConst + i] = std::numeric_limits<float>::max();
      continue;
    }
    float parametersDifference = qonp[i] - qonp[j];
    float covarianceDifference = qonpCov[i] - qonpCov[j];
    float G_difference = qonpG[j] - qonpG[i];
    float G_sum        = qonpG[i] + qonpG[j];
    int index = indexConst + i;
    distances[ index ] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;
    if(distances[ index ]< minDistance )
    {  
      minIndex  = index;
      minDistance = distances[ index ];
    }

  }

  for( int i = j+1; i < n; ++i ){
    int index =  (i+1)*i/2+j;
    if(qonpCov[i] == 0){
      distances[index] = std::numeric_limits<float>::max();
      continue;
    }
    float parametersDifference = qonp[i] - qonp[j];
    float covarianceDifference = qonpCov[i] - qonpCov[j];
    float G_difference = qonpG[j] - qonpG[i];
    float G_sum        = qonpG[i] + qonpG[j];
    distances[ index ] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;
    if(distances[ index ]< minDistance ){
      minIndex  = index;
      minDistance = distances[ index ];
    }
  }
  return minIndex;
}


std::pair<int, int>  
Trk::QuickCloseComponentsMultiStateMerger::findMinimumIndex(const floatPtrRestrict distancesIn, 
                                                            const int n) const
{

  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);
  int mini  = 0; // always 1e30
  int mini2 = -1;
  float minDistance = std::numeric_limits<float>::max(); 

  for( int i = 0; i < n; ++i ){
    if( distances[ i ] < minDistance ){
      mini2 = mini;
      mini  = i;
      minDistance = distances[ i ];
    }
  }

  return  std::make_pair(mini, mini2);
}


std::pair<int, int> 
Trk::QuickCloseComponentsMultiStateMerger::findMinimumPair(const floatPtrRestrict distancesIn,
                                                           const std::vector <const Trk::ComponentParameters*>& comps, 
                                                           const int n) const
{

  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);

  double minDistance = std::numeric_limits<float>::max();
  int mini = 0;
  int minj = 0;

  for( int i = 0; i < n; ++i ){
    if(comps[i] == 0)  continue;
    int indexConst = (i+1)*i/2;
    for( int j = 0; j < i; ++j ){
      if(comps[j] == 0)  continue;
      int index =  indexConst + j ;
      if( distances[ index ] < minDistance ){
        minDistance = distances[ index ];
        mini = i;
        minj = j;
      }
    }
  }

  return std::make_pair(mini, minj);
}
