/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/Chrono.h"

#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#include <map>

typedef boost::bimap<boost::bimaps::multiset_of<float>, boost::bimaps::set_of<int>> bimap_t;


#if !defined(__GNUC__)
# define __builtin_assume_aligned(X, N) X
#else
# if defined(__clang__)
#  if !__has_builtin(__builtin_assume_aligned)
#    define __builtin_assume_aligned(X, N) X
#  endif
# endif
#endif


Trk::QuickCloseComponentsMultiStateMerger::QuickCloseComponentsMultiStateMerger(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),
  m_maximumNumberOfComponents(5),
  m_distance("Trk::KullbackLeiblerComponentDistance/KullbackLeiblerComponentDistance"),
  m_stateCombiner("Trk::MultiComponentStateCombiner/CloseComponentsCombiner"),
  m_stateAssembler("Trk::MultiComponentStateAssembler/CloseComponentsStateAssembler"),
  m_chronoSvc("ChronoStatSvc", name),
  m_useFullDistanceCalcArray(true),
  m_useFullDistanceCalcVector(true),
  m_useMap{}
  
{

  declareInterface<IMultiComponentStateMerger>(this);

  declareProperty("CombinerTool",m_stateCombiner);
  declareProperty("MaximumNumberOfComponents", m_maximumNumberOfComponents);
  declareProperty("DistanceType", m_distance);
  declareProperty("FullDistanceCalcArray", m_useFullDistanceCalcArray);
  declareProperty("FullDistanceCalcVector", m_useFullDistanceCalcVector);

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
    return 0;
  }

  ATH_MSG_INFO(  "Initialisation of " << type() << " under instance " << name() << " was successful" );

  if (m_useFullDistanceCalcArray)
    ATH_MSG_INFO("Using full KL-distance based merging : Array");
  else if (m_useFullDistanceCalcVector)
    ATH_MSG_INFO("Using full KL-distance based merging : Vector");
  else
    ATH_MSG_INFO("Using default distance based merging");

  return StatusCode::SUCCESS;

}

StatusCode Trk::QuickCloseComponentsMultiStateMerger::finalize()
{

  ATH_MSG_INFO( "Finalisation of " << type() << " under instance " << name() << " was successful" );

  return StatusCode::SUCCESS;

}

const Trk::MultiComponentState* Trk::QuickCloseComponentsMultiStateMerger::merge(const Trk::MultiComponentState& unmergedState) const
{

  // Start the timer
  //Chrono chrono( &(*m_chronoSvc), "CloseComponentMerger" );

  ATH_MSG_VERBOSE(  "Merging state with " << unmergedState.size() << " components" );

  unsigned int numberOfComponents = unmergedState.size();

  // Check that the assember is reset
  bool isAssemblerReset = m_stateAssembler->reset();

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

  // Scan all components for covariance matricies. If one or more component is missing an error matrix, component reduction is impossible.
  for ( ; component != unmergedState.end(); ++component){

    const AmgSymMatrix(5)* measuredCov = component->first->covariance();

    if ( !measuredCov )
      componentWithoutMeasurement = true;
  }
  
  
  const Trk::TrackParameters* combinedState = unmergedState.begin()->first->clone(); 
  const Trk::ComponentParameters reducedState( combinedState, 1. );
  if ( componentWithoutMeasurement ){
    ATH_MSG_DEBUG(  "A track parameters object is without measurement... reducing state to single component" );
    return ( new Trk::MultiComponentState( reducedState ) );
  }

  // Options to use full KL distance-based algorithm.
  if (m_useFullDistanceCalcArray) {
    delete combinedState;
    return mergeFullDistArray(unmergedState);
  } else if(m_useFullDistanceCalcVector){
    delete combinedState;
    return mergeFullDistVector(unmergedState);
  }
 
  
 
 
  m_chronoSvc->chronoStart("QCCM::merge");

  // These should be sorted by key....
  SortBySmallerWeight comparator;
  MultiComponentStateMap unmergedComponentsMap(comparator);
  
  // Clone unmerged state as STL methods deletes components
  Trk::MultiComponentState* clonedUnmergedState = const_cast<Trk::MultiComponentState*>( unmergedState.clone() );

  component = clonedUnmergedState->begin();

  for ( ; component != clonedUnmergedState->end(); ++component ){

    // Map components of multi-component state list into multi-map
    double  distance  = calculateDistance( reducedState, *component );
   
    if (distance == 0)      
      ATH_MSG_DEBUG( "Distance  == 0 hmmm interesting" );

    unmergedComponentsMap.insert( std::make_pair(distance, *component) );
  }


  ATH_MSG_VERBOSE("  Start Reducing Components   " );


  while (numberOfComponents > m_maximumNumberOfComponents){

    // Reset the merged components multi-map for next iteration
      
    double minDistance(9.9e12);
    double key1(-99999999),key2(-99999999);
    MultiComponentStateMap::iterator mapComponent = unmergedComponentsMap.begin();
    MultiComponentStateMap::iterator mapComponentEnd = unmergedComponentsMap.end();
    --mapComponentEnd;
    MultiComponentStateMap::iterator tempIter;
   
    MultiComponentStateMap::iterator iterKey1,iterKey2;
    
    for ( ; mapComponent != mapComponentEnd; ++mapComponent){
      tempIter = mapComponent;
      ++tempIter;
    
      double distance  =  tempIter->first - mapComponent->first ;
      //std::cout << "1st " << mapComponent->first <<" 2nd " << tempIter->first <<" = "<< distance << std::endl;
   
      if ( distance < minDistance )
  {
    key1 = mapComponent->first;
    key2 = tempIter->first;
    iterKey1 = mapComponent;
    iterKey2 = tempIter;
    minDistance = distance;
  }
    }
    
    if ( key1 != -99999999 && key2 != -99999999 && iterKey1 != iterKey2 ){
    
      Trk::MultiComponentState* componentsToBeMerged = new Trk::MultiComponentState();
   
      ATH_MSG_VERBOSE( "key1 " << key1 << " key2 " << key2 );
   
      componentsToBeMerged->push_back( iterKey1->second );
      componentsToBeMerged->push_back( iterKey2->second );
      
      // Combine the closest distance components
      const Trk::ComponentParameters* combinedComponents = m_stateCombiner->combineWithWeight( *componentsToBeMerged );
      
      if (msgLvl(MSG::VERBOSE)) msg() << "Weight of new component"<< combinedComponents->second << endmsg;
            
      // Erase these components from the unmerged components map. These need to be deleted also as new memory is assigned in the combiner
      delete componentsToBeMerged;
      
      unmergedComponentsMap.erase( iterKey1 );
      unmergedComponentsMap.erase( iterKey2 );
   
      double  distance  = calculateDistance( reducedState, *combinedComponents );
    
      if (distance == 0)      
        ATH_MSG_DEBUG("Distance  == 0 hmmm interesting");

      unmergedComponentsMap.insert( std::make_pair(distance, *combinedComponents) );
    
      // Delete memory allocated to component parameters pair. This does not delete the inserted copy
      delete combinedComponents;
  
      // Decrement the number of components
      --numberOfComponents;
  
    } else {
      
      ATH_MSG_DEBUG( " Keys are either equal or totally nonsensical " );
   
      // Something has gone amiss...... what shall i do
      clonedUnmergedState->clear();
      delete clonedUnmergedState;

      m_chronoSvc->chronoStop("QCCM::merge");
      return ( new Trk::MultiComponentState( reducedState ) );     
      
    }
    
    ATH_MSG_VERBOSE( "Number of components: " << numberOfComponents );
  
  }

  // std::cout << "   Finished Reducing Components   " <<std::endl;
  delete combinedState;

  // Build final state containing both merged and unmerged components

  MultiComponentStateMap::const_iterator mapComponent = unmergedComponentsMap.begin();

  for ( ; mapComponent != unmergedComponentsMap.end(); ++mapComponent) {

    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(mapComponent->second);

    if ( !componentAdded )
      ATH_MSG_WARNING( "Component could not be added to the state in the assembler" );

    // Free up memory from track parameters in unmerged components map
    delete mapComponent->second.first;

  }
  
  const Trk::MultiComponentState* mergedState = m_stateAssembler->assembledState();

  ATH_MSG_VERBOSE( "Number of components in merged state: " << mergedState->size() );

  // Memory clean up
  clonedUnmergedState->clear();
  delete clonedUnmergedState;

  m_chronoSvc->chronoStop("QCCM::merge");
  return mergedState;

}





double Trk::QuickCloseComponentsMultiStateMerger::calculateDistance(const Trk::ComponentParameters referenceParameters , const Trk::ComponentParameters variableParameters  ) const
{
  
  double distance(0);
  
  distance = (*m_distance)( referenceParameters, variableParameters);
   
  return distance; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS TO USE INDEX DISTANCE MAP //

void Trk::QuickCloseComponentsMultiStateMerger::addComponentDistances(std::vector<const ComponentParameters*> &tempComponents) const
{

  //ATH_MSG_VERBOSE("Adding component distances to map");
  
  std::vector<const ComponentParameters*>::iterator newDist = (tempComponents.end()-1);
  std::vector<const ComponentParameters*>::iterator tcIter  = (tempComponents.begin());

  for (; tcIter != tempComponents.end(); ++tcIter) {

    if (tcIter == newDist) continue;

    float distance(calculateDistance(*(*tcIter), *(*newDist)));

    //ATH_MSG_VERBOSE("Adding new distance: " << distance << " to map key: " << std::distance(tempComponents.begin(),tcIter));
    
    m_indexDistanceMap[std::distance(tempComponents.begin(),tcIter)].push_back(distance);
    
  }

  //Need to resize to account for the fact we have a new entry now.
  m_indexDistanceMap.resize(m_indexDistanceMap.size()+1);
  // m_indexDistanceMap.push_back(std::vector<float>());

  //ATH_MSG_VERBOSE("Done adding component distances to map");

}

std::pair<int,int> Trk::QuickCloseComponentsMultiStateMerger::getMinDistanceIndicesFromMap() const
{

  std::pair<int,int> minIndices(std::make_pair(-1,-1));

  IndexDistanceMap::iterator idIter = m_indexDistanceMap.begin();

  float minDistance(10E10);
  for (; idIter != m_indexDistanceMap.end(); ++idIter) {

    if (idIter == m_indexDistanceMap.end()-1) continue;
    
    std::vector<float>::iterator distIter = std::min_element(idIter->begin(),idIter->end());
    float localMin(*distIter);
    if (localMin < minDistance) {
      minDistance = localMin;
      int index1(std::distance(m_indexDistanceMap.begin(),idIter)),
          index2(std::distance(idIter->begin(),distIter)+index1+1);
      minIndices = std::make_pair(index1, index2);
    }
  }

  //ATH_MSG_VERBOSE("From map, get minimum indices: " << minIndices.first << " and " << minIndices.second);
  
  
  return minIndices;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float Trk::QuickCloseComponentsMultiStateMerger::getMinimumKLDistanceComponent(const ComponentParameters*     &ref,
                    std::vector<const ComponentParameters*> &mcs,
                    int& minIndex) const
{

  //ATH_MSG_VERBOSE("Getting minimum KL distance component");
  
  float minDistance(10E10);

  //Find location of ref in mcs to skip it.
  std::vector<const ComponentParameters*>::iterator refIter = std::find(mcs.begin(),mcs.end(),ref);
  if (refIter == mcs.end()) {
    ATH_MSG_FATAL("Couldn't find reference parameter in the component vector???");
  }
  
  //We can just start with the next component over, since otherwise we'd be
  //doing the same calculation 2x for every component.
  std::vector<const ComponentParameters*>::iterator mcsIter = refIter + 1;

  for (; mcsIter != mcs.end(); ++mcsIter) {

    float distance(calculateDistance(*ref, *(*mcsIter)));

    //Save the value in the map for later.
    m_indexDistanceMap[std::distance(mcs.begin(),refIter)].push_back(distance);
    
    if (distance < minDistance) {
      minDistance = distance;
      minIndex    = std::distance(mcs.begin(), mcsIter);
    }
    
  }

  // ATH_MSG_VERBOSE("Minimum distance is:    " << minDistance);
  // ATH_MSG_VERBOSE("Corresponding index is: " << minIndex);

  return minDistance;

}

//Goal of this routine is to iterate over each element of the multicomponent state, and get the absolute
//minimum distance pair of components. These two will be merged.
std::pair<int,int> Trk::QuickCloseComponentsMultiStateMerger::getPairOfMinimumDistanceComponents(std::vector<const ComponentParameters*> &tempComponents) const
{

  //If the index distance map isn't empty, use that instead.
  if (m_useMap) {
    return getMinDistanceIndicesFromMap();    
  }

  //Otherwise, we have to calculate everything from scratch.
  //ATH_MSG_VERBOSE("Getting indices of nearest components");
  
  std::multimap<float, std::pair<int,int>, SortBySmallerWeight> distIndexMap;
  
  std::vector<const ComponentParameters*>::iterator mcsIter = tempComponents.begin();
  for (; mcsIter != tempComponents.end(); ++mcsIter) {

    //ATH_MSG_VERBOSE("Getting minimum for element: " << std::distance(tempComponents.begin(),mcsIter));
    
    int minIndex(-1), refIndex(std::distance(tempComponents.begin(),mcsIter));

    float dist(getMinimumKLDistanceComponent(*mcsIter,tempComponents,minIndex));

    distIndexMap.insert(std::make_pair(dist, std::make_pair(refIndex,minIndex)));
  }

  //Now that all the distances are filled, we can start using the map.
  m_useMap = true;
   
  return (*distIndexMap.begin()).second;

}

void Trk::QuickCloseComponentsMultiStateMerger::deleteStateComponents(int ind1, int ind2, std::vector<const ComponentParameters*> &mcs) const
{

  //ATH_MSG_VERBOSE("Erasing merged components ...");

  int indexA(-1), indexB(-1);

  if (ind2>ind1) {
    indexA = ind2;
    indexB = ind1;
  } else {
    indexA = ind1;
    indexB = ind2;
  }

  //Best way to do this is probably get iterators corresponding to the
  //indices, and then use 'erase' to remove them from the MCS.
  std::vector<const ComponentParameters*>::iterator iterA = mcs.begin(), iterB = mcs.begin();

  //Shift iterators to right places.
  std::advance(iterA,indexA);
  std::advance(iterB,indexB);

  /////////////////////////////////////////////////////
  // COULD THESE LINES BE CAUSING THE MEMORY LEAK???
  /////////////////////////////////////////////////////

  //Need to delete track parameters from two iterators.
  
  // //Remove elements.
  // if (ind2 > ind1) {
  //   mcs.erase(iter2);
  //   mcs.erase(iter1);
  // } else {
  //   mcs.erase(iter1);
  //   mcs.erase(iter2);
  // }

  ATH_MSG_VERBOSE("Deleting component A ...");
  if (mcs[indexA]) {
    // if (mcs[indexA]->first)
    //   delete mcs[indexA]->first;
    delete mcs[indexA];
  }

  ATH_MSG_VERBOSE("Deleting component B ...");
  if (mcs[indexB]) {
    // if (mcs[indexB]->first)
    //   delete mcs[indexB]->first;
    delete mcs[indexB];
  }

  ATH_MSG_VERBOSE("Erasing iterators from vector ...");
  mcs.erase(iterA);
  mcs.erase(iterB);

  //ATH_MSG_VERBOSE("Done with erasing");

}

void Trk::QuickCloseComponentsMultiStateMerger::mergeStateComponents (std::vector<const ComponentParameters*> &mcs) const
{
  
  //Step 1: Get the components to merged.
  std::pair<int,int> mergeIndices(getPairOfMinimumDistanceComponents(mcs));

  //ATH_MSG_VERBOSE("Indices are: " << mergeIndices.first << " and " << mergeIndices.second);
  
  //Step 2: Merge them.
  Trk::MultiComponentState* componentsToBeMerged = new Trk::MultiComponentState();

  // std::vector<ComponentParameters>::iterator iter1 = mcs.begin(), iter2 = mcs.begin();

  // std::advance(iter1,mergeIndices.first);
  // std::advance(iter2,mergeIndices.second);

  //ATH_MSG_VERBOSE("Momenta of min. distance components: " << iter1->first->momentum().mag() << ", " << iter2->first->momentum().mag());
  
  // componentsToBeMerged->push_back( (*iter1) );
  // componentsToBeMerged->push_back( (*iter2) );
  
  componentsToBeMerged->push_back( *mcs[mergeIndices.first]  );
  componentsToBeMerged->push_back( *mcs[mergeIndices.second] );

  const Trk::ComponentParameters* combinedComponents = m_stateCombiner->combineWithWeight( *componentsToBeMerged );
  
  //Step 3: Delete the old components from the map.
  //componentsToBeMerged->clear();
  delete componentsToBeMerged;
  
  deleteStateComponents(mergeIndices.first, mergeIndices.second, mcs);

  if (mergeIndices.first > mergeIndices.second)
    deleteStoredDistances(mergeIndices.second, mergeIndices.first);
  else
    deleteStoredDistances(mergeIndices.first,  mergeIndices.second);

  //Step 4: Insert the newly merged component into the MCS.
  mcs.push_back(combinedComponents);

  //delete combinedComponents;

  //Step 5: Add new distance calculations to index distance map.
  addComponentDistances(mcs);

}

// Method to take an input MCS and minimize it from 'X' components to
// 'm_maximumNumberOfComponents' components.
const Trk::MultiComponentState* Trk::QuickCloseComponentsMultiStateMerger::mergeFullDistVector(const Trk::MultiComponentState& mcs) const
{

  m_chronoSvc->chronoStart("QCCM::mergeFullDist");

  //NEW: Clear storage for indices / distances.
  clearIndexMap();
  m_indexDistanceMap.resize(mcs.size());

  // m_indexDistanceMap.reserve(mcs.size());
  // for (unsigned int i(0); i < mcs.size(); i++)
  //   m_indexDistanceMap.push_back(std::vector<float>());

  // IndexDistanceMap map(mcs.size());
  // m_indexDistanceMap = map;

  m_useMap = false;
  
  ATH_MSG_VERBOSE("Cloning unmerged state and getting components");
  
  // Clone unmerged state as STL methods deletes components.
  // Trk::MultiComponentState* clonedUnmergedState      = const_cast<Trk::MultiComponentState*>( mcs.clone() );
  Trk::MultiComponentState::const_iterator component = mcs.begin();

  std::vector<const ComponentParameters*> tempComponents;
  //for ( ; component != clonedUnmergedState->end(); ++component ) {
  for ( ; component != mcs.end(); ++component ) {
    //tempComponents.push_back(&(*component));
    tempComponents.push_back((*component).clone());
  }

  ATH_MSG_VERBOSE("Starting component merging");

  //Do component merging.
  while (tempComponents.size() > m_maximumNumberOfComponents) {
    mergeStateComponents(tempComponents);
  }

  ATH_MSG_VERBOSE("Done component merging, adding components to state assembler");

  //Make a new MCS out of merged components.
  std::vector<const ComponentParameters*>::iterator mcsIter = tempComponents.begin();  

  for ( ; mcsIter != tempComponents.end(); ++mcsIter){
    
    // Add component to state being prepared for assembly and check that it is valid
    bool componentAdded = m_stateAssembler->addComponent(*(*mcsIter));

    if ( !componentAdded )
      msg(MSG::WARNING) << "Component could not be added to the state in the assembler" << endmsg;

  }

  ATH_MSG_VERBOSE("Got states, doing final assembly of merged state");
  
  const Trk::MultiComponentState* mergedState = m_stateAssembler->assembledState();

  // Memory clean up. Gotta also clean up those pesky track parameters.
  // for (unsigned int ic(0); ic < clonedUnmergedState->size(); ic++)
  //   if (clonedUnmergedState->at(ic).first)
  //     delete clonedUnmergedState->at(ic).first;
    
  // clonedUnmergedState->clear();
  // delete clonedUnmergedState;
    
  //Delete remaining track components, clear out the vector of states.
  for (unsigned int it(0); it < tempComponents.size(); it++) {
    if (tempComponents[it]) {
      if (tempComponents[it]->first)
  delete tempComponents[it]->first;
      delete tempComponents[it];
    }
  }
  tempComponents.clear();
  
  //std::vector<Trk::ComponentParameters>().swap(tempComponents);
  //delete tempComponents;

  //if (msgLvl(MSG::VERBOSE)) msg() << "Number of components in merged state: " << mergedState->size() << endmsg;

  m_chronoSvc->chronoStop("QCCM::mergeFullDist");
  
  return mergedState;

}

//Routine to get rid of all the distance calculations made with
//merged components.
void Trk::QuickCloseComponentsMultiStateMerger::deleteStoredDistances(int i1, int i2) const {

  IndexDistanceMap::iterator iter1 = m_indexDistanceMap.begin();
  IndexDistanceMap::iterator iter2 = m_indexDistanceMap.begin();

  std::advance(iter1,i1);
  std::advance(iter2,i2);

  //Erase larger iterator entries first to avoid index shifting.
  m_indexDistanceMap.erase(iter2);

  for (int ik(0); ik < i2; ik++) {
    m_indexDistanceMap[ik].erase(m_indexDistanceMap[ik].begin()+(i2-ik)-1);
  }

  //Erase smaller iterator entries.
  m_indexDistanceMap.erase(iter1);
  for (int ik(0); ik < i1; ik++) {
    m_indexDistanceMap[ik].erase(m_indexDistanceMap[ik].begin()+(i1-ik)-1);
  }
      
}

const Trk::MultiComponentState* Trk::QuickCloseComponentsMultiStateMerger::mergeFullDistArray(const Trk::MultiComponentState& mcs) const
{
  //m_chronoSvc->chronoStart("QCCM::mergeFullDistArray");


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
    distances[i] = 1e30;
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
  
    //Find minimum
    //std::pair <int ,int > min = findMinimumPair( distances,statesToMerge, n );

    //Searching for the minimum distances is slowest part of the loop 
    // lets try and speed it up by reducing the number searches by 2
    //Note that a boost::bimap could be used but it is really slow.
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
    
    //std::cout << mini << " "  <<  minj << std::endl;
    
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

    // Erase these components from the unmerged components map. These needs to be deleted also as new memory is assigned in the combiner
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
    bool componentAdded = m_stateAssembler->addComponent( *state );

    if ( !componentAdded )
      ATH_MSG_WARNING( "Component could not be added to the state in the assembler" );
  }
  
  const Trk::MultiComponentState* mergedState = m_stateAssembler->assembledState();

  ATH_MSG_VERBOSE( "Number of components in merged state: " << mergedState->size() );

  // Memory clean up
  for(auto state:statesToMerge){
    if(state){
      delete state->first; 
      delete state;
    }
  }
  statesToMerge.clear();

  //m_chronoSvc->chronoStop("QCCM::mergeFullDistArray");
   
  return mergedState;

}



 
void Trk::QuickCloseComponentsMultiStateMerger::resetDistances( floatPtrRestrict  distancesIn, const int mini,const  int  n)const{

  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);

  int j = mini;
  int indexConst = (j+1)*j/2;
  for( int i = 0; i < j; ++i ){
    distances[ indexConst+ i ] = 1e30;
  }

  for( int i = j; i < n; ++i ){
    int index =  (i+1)*i/2+j;
    distances[ index ] = 1e30;
  }

}




void Trk::QuickCloseComponentsMultiStateMerger::calculateAllDistances( floatPtrRestrict qonpIn,  floatPtrRestrict  qonpCovIn,  floatPtrRestrict qonpGIn, floatPtrRestrict distancesIn, int  n) const
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
      distances[ indexConst + j ] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;
    }
  }
}


int Trk::QuickCloseComponentsMultiStateMerger::recalculateDistances( floatPtrRestrict qonpIn,  floatPtrRestrict  qonpCovIn,  floatPtrRestrict qonpGIn, floatPtrRestrict distancesIn, int mini, int  n) const
{

  float *qonp = (float*)__builtin_assume_aligned(qonpIn, 32);
  float *qonpCov = (float*)__builtin_assume_aligned(qonpCovIn, 32);
  float *qonpG = (float*)__builtin_assume_aligned(qonpGIn, 32);
  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);

  int j = mini;
  int indexConst = (j+1)*j/2;
  
  int minIndex = 0; // The distance will always be 1e30 for this entry;
  
  for( int i = 0; i < j; ++i ){
    if(qonpCov[i] == 0){
      distances[indexConst + i] = 1e30;
      continue;
    }
    float parametersDifference = qonp[i] - qonp[j];
    float covarianceDifference = qonpCov[i] - qonpCov[j];
    float G_difference = qonpG[j] - qonpG[i];
    float G_sum        = qonpG[i] + qonpG[j];      
    int index = indexConst + i;
    distances[ index ] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;   
    if(distances[ index ]< distances[ minIndex ] )
      minIndex  = index;
  }

  for( int i = j+1; i < n; ++i ){
    int index =  (i+1)*i/2+j;
    if(qonpCov[i] == 0){
      distances[index] = 1e30;
      continue;
    }
    float parametersDifference = qonp[i] - qonp[j];
    float covarianceDifference = qonpCov[i] - qonpCov[j];
    float G_difference = qonpG[j] - qonpG[i];
    float G_sum        = qonpG[i] + qonpG[j];      
    distances[ index ] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;   
    if(distances[ index ]< distances[ minIndex ] )
      minIndex  = index;
  }
  return minIndex;
}


std::pair<int, int>  Trk::QuickCloseComponentsMultiStateMerger::findMinimumIndex( const floatPtrRestrict distancesIn, const int n) const
{

  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);  
  int mini  = 0; // always 1e30
  int mini2 = -1;

  for( int i = 0; i < n; ++i ){
    if( distances[ i ] < distances[ mini ] ){
      mini2 = mini;
      mini  = i;
    }
  } 
  
  return  std::make_pair(mini, mini2);
}


std::pair<int, int> Trk::QuickCloseComponentsMultiStateMerger::findMinimumPair( const floatPtrRestrict distancesIn,const std::vector <const Trk::ComponentParameters*>& comps, const int n) const
{

  float *distances = (float*)__builtin_assume_aligned(distancesIn, 32);
  
  double minDistance = 1e20;
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

