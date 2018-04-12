/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator> // need it for advance



//#include <boost/cstdint.hpp>
//#include <stdint.h>
#include <boost/lexical_cast.hpp>

#include "TrigNavStructure/TypelessHolder.h"
#include "TrigNavStructure/TrigNavStructure.h"

//#define MLOG(x)   if (m_log->level()<=MSG::x+m_msgLvlOffset) *m_log << (m_msgLvlOffset+MSG::x)
//#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x

using namespace HLT;

template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v)
{
  s<< "[";for(auto i:v){s << i << ", ";} s << "]";
  return s;
}

std::string TrigNavStructure::m_unspecifiedLabel = "";
const TriggerElement* TrigNavStructure::m_unspecifiedTE = 0;


TrigNavStructure::~TrigNavStructure() {
  m_factory.reset();
}

/*****************************************************************************
 *
 * METHODS FOR NAVIGATION BUILDING
 *
 *****************************************************************************/
TriggerElement* TrigNavStructure::getInitialNode() {
  if ( m_factory.empty() )
    m_factory.produce(0);
  return m_factory.listOfProduced().front();
}

const TriggerElement* TrigNavStructure::getInitialNode() const {
  if ( not m_factory.empty() )
    return m_factory.listOfProduced().front();
  return 0;
}


TriggerElement* TrigNavStructure::addRoINode( TriggerElement* initial ) {
  if ( isInitialNode(initial)  ) {
    TriggerElement* te = m_factory.produce(0);

    // build seeds, seeededBy relations
    te->relate( initial, TriggerElement::seededByRelation );
    initial->relate( te, TriggerElement::seedsRelation );
    return te;
  }
  return 0;
}

const std::vector<TriggerElement*>& TrigNavStructure::getRoINodes( const TriggerElement* somenode ) {
  return somenode->getRelated(TriggerElement::sameRoIRelation);
}


// delegates all work to the more generic method
TriggerElement* TrigNavStructure::addNode( TriggerElement* seednode, unsigned int id ) {
  std::vector<TriggerElement*> list;
  list.push_back(seednode);
  return addNode(list, id);
}

TriggerElement* TrigNavStructure::addNode( std::vector<TriggerElement* >& seeds,  unsigned int id, bool ghost, bool nofwd ) {
  TriggerElement* te = m_factory.produce(id, ghost, nofwd);


  std::vector<TriggerElement* >::iterator it;
  for ( it = seeds.begin(); it != seeds.end(); ++it ) {
    // relate to it
    TriggerElement* seednode = *it;
    te->relate( seednode, TriggerElement::seededByRelation );
    seednode->relate( te, TriggerElement::seedsRelation );
    // add previous Features
    te->getPreviousFeatures().insert(te->getPreviousFeatures().end(),
                                     seednode->getPreviousFeatures().begin(),
                                     seednode->getPreviousFeatures().end());

    // copy RoI relations
    if ( isRoINode(seednode) ) {
      te->relate( seednode, TriggerElement::sameRoIRelation );
      seednode->relate( te, TriggerElement::sameRoIRelation );
    } else {
      std::vector<TriggerElement*>::const_iterator itbegin = seednode->getRelated( TriggerElement::sameRoIRelation ).begin();
      std::vector<TriggerElement*>::const_iterator itend   = seednode->getRelated( TriggerElement::sameRoIRelation ).end();

      for (  ; itbegin != itend; ++itbegin ) {
        te->relate( *itbegin, TriggerElement::sameRoIRelation );
        (*itbegin)->relate( te, TriggerElement::sameRoIRelation );
      }
    }
  }
  return te;
}

const std::vector<TriggerElement*>& TrigNavStructure::getDirectPredecessors(const TriggerElement* te)  {
  return te->getRelated(TriggerElement::seededByRelation);
}

const std::vector<TriggerElement*>& TrigNavStructure::getDirectSuccessors(const TriggerElement* te) {
  return te->getRelated(TriggerElement::seedsRelation);
}


const TriggerElement* TrigNavStructure::getSuccessor( const TriggerElement* te, unsigned int id ) {
  if ( te->getId() == id )
    return te;
  std::vector<TriggerElement*>::const_iterator it;
  const TriggerElement* found(0);
  for ( it = TrigNavStructure::getDirectSuccessors(te).begin() ; it !=  TrigNavStructure::getDirectSuccessors(te).end() ; ++it ) {
    found = getSuccessor(*it, id);
    if (found)
      break;
  }
  return found;
}




/*****************************************************************************
 *
 * PRETTY PRINTING
 *
 *****************************************************************************/


void TrigNavStructure::printASCIIArt (std::string& str, const TriggerElement* te, int offset) const {
  using namespace std;
  const TriggerElement* initialNode = getInitialNode();
  if ( initialNode == 0 )
    return;

  if ( !te )
    te = initialNode;

  if ( te == initialNode ) {
    // loop over all TEs which are not seeded by anything but are not initial nodes
    const std::vector< TriggerElement* >& allTEs = m_factory.listOfProduced();
    std::vector< TriggerElement* >::const_iterator allIt;
    for (allIt = allTEs.begin(); allIt != allTEs.end(); ++allIt) {
      if ((*allIt)->getRelated(TriggerElement::seededByRelation).size() == 0 && *allIt != getInitialNode() )
        printASCIIArt(str, *allIt);
    }
  }

  ostringstream ss;
  // pretty print
  for ( int i=0; i < offset; i++ ) {
    ss << "| ";
    //cout << "  ";
  }
  ss << "\\_";

  ss << "id:" << dec << te->getId() << ";"
     << "active:" <<(te->getActiveState() ? "1" : "0" ) << ";"
     << "error:" <<(te->getErrorState() ? "1" : "0" ) << ";"
     << "ptr:" << hex << te << ";"
     << "#seeds:" << dec << te->getRelated(TriggerElement::seedsRelation).size() << ",";
  // add all ptrs
  std::vector<TriggerElement*>::const_iterator seedIt;
  for ( seedIt = getDirectSuccessors(te).begin();  seedIt != getDirectSuccessors(te).end(); ++seedIt ) {
    ss << *seedIt <<" ";
  }

  ss << "#seededBy:" << dec << te->getRelated(TriggerElement::seededByRelation).size() << ";"
     << "#RoI:" << dec << te->getRelated(TriggerElement::sameRoIRelation).size()  << ";"
     << "#features:" << te->getFeatureAccessHelpers().size() << ";";
  for ( auto& fea: te->getFeatureAccessHelpers() ) {
    ss << fea;
  }
  ss << std::endl;

  std::string str2;
  str2 = ss.str();
  str += str2;

  if ( ! isTerminalNode(te) ) {
    std::vector < TriggerElement* >::const_iterator it = te->getRelated( TriggerElement::seedsRelation ).begin();
    std::vector < TriggerElement* >::const_iterator itEnd   = te->getRelated( TriggerElement::seedsRelation ).end();
    for (  ; it != itEnd; ++it ) {
      std::string str2;
      printASCIIArt(str, *it, offset+1);
      str += str2;
    }
  }
}

bool TrigNavStructure::serializeTEs( std::vector<uint32_t>& output ) const {
  ::HLTNavDetails::FillSize fs(output);

  const std::vector<TriggerElement*>& fullList =  m_factory.listOfProduced();
  std::vector<TriggerElement*> all;
  all.reserve(fullList.size());

  for(std::vector<TriggerElement*>::const_iterator iter = fullList.begin(); iter != fullList.end(); ++iter) {
    if(! (*iter)->transient() ) {
      all.push_back( *iter );
    }
  }

  // reserve space (in order to be efficient it should not be resized any more, this can be studied probably later to find optimum)
  output.reserve(10*all.size());

  // SERIALZE NAVIGATION STRUCTURE
  // size (measured in elements)
  output.push_back(all.size());

  // helper keys
  std::map<TriggerElement*, uint16_t> keys;

  const TriggerElement* previous = 0;
  std::vector<TriggerElement*>::const_iterator it;
  uint16_t indexForTe = 0;
  for ( it = all.begin(); it != all.end(); ++it ) {
    // first we stream pointer as it (this is already an unique key for this TE)
    //    output.push_back((unsigned int)(*it));
    (*it)->serialize(output, keys, previous);
    previous = *it;
    keys[*it] =  indexForTe;
    indexForTe++;
  }

  return true;
}

/*****************************************************************************
 *
 * DESERIALIZATION
 *
 *****************************************************************************/

bool TrigNavStructure::deserializeTEs(std::vector<uint32_t>::const_iterator& start, unsigned int totalSize) {
  m_factory.reset();
  
  std::vector<uint32_t>::const_iterator& inputIt = start;
  const size_t payloadSize = *inputIt++; 
  
  if ( totalSize < payloadSize ) { // not all TEs were recorded
    std::cerr << "size of the navigation payload smaller than the one required to save TEs. " << totalSize << " < " << payloadSize << std::endl;
    return false;
  }

  
  const unsigned int size = *inputIt++; // size in terms of number of TEs
  TriggerElement* previous = 0;
  std::map<uint16_t, TriggerElement* > keys;
  
  for ( unsigned int i = 0; i < size; ++i ) {
    // create new TE
    TriggerElement* te = m_factory.produce(TriggerElement::enquireId(inputIt)); //
    te->deserialize(inputIt, keys, previous);
    previous = te;
    // keys table for deserialization of other TEs
    keys[i] = te;
  }
  
  if ( not m_factory.empty() ) {
    // rebuild  sameRoI relations (this can't be done by TEs deserialization)
    TriggerElement* initialNode = getInitialNode();
    std::vector<TriggerElement*>::const_iterator roiTEit;
    for ( roiTEit = getDirectSuccessors(initialNode).begin(); 
	  roiTEit != getDirectSuccessors(initialNode).end(); ++roiTEit ) {
      fillSameRoIRelation((*roiTEit), (*roiTEit));
    }
  } // else this is empty structure, but this shoudl not bother us as it is may well be valid case
  
  return true;
}



void TrigNavStructure::fillSameRoIRelation ( TriggerElement* roi, TriggerElement* te ) {
  if ( ! isRoINode(te) ) {
    te->relate(roi, TriggerElement::sameRoIRelation);
    roi->relate(te, TriggerElement::sameRoIRelation);
  }
  if ( isTerminalNode(te) )
    return;
  else { // traverse all sub nodes
    std::vector<TriggerElement*>::const_iterator it;
    for ( it = getDirectSuccessors(te).begin(); it != getDirectSuccessors(te).end(); ++it ) {
      fillSameRoIRelation(roi, *it);
    }
  }
}

/*****************************************************************************
 *
 * QUERIES
 *
 *****************************************************************************/

void TrigNavStructure::getAllRoIThresholdTEs( std::vector< TriggerElement* >& output ) const
{
  const TriggerElement* initNode = getInitialNode();
  // get first layer of TEs representing the RoIs themselves (not the thresholds!)
  const std::vector<TriggerElement*>& rois = getDirectSuccessors(initNode);
  for ( std::vector<TriggerElement*>::const_iterator roi_te = rois.begin();
        roi_te != rois.end(); ++roi_te) {
    // get threshold-like TEs:
    const std::vector<TriggerElement*>& tes = getDirectSuccessors( (const TriggerElement*)(*roi_te) );
    std::vector<TriggerElement*>::const_iterator te = tes.begin();
    for ( ; te != tes.end(); ++te) {
      output.push_back( *te );
    }
  }
}

void TrigNavStructure::getAllOfType ( const te_id_type id,
				      std::vector< TriggerElement* >& output,
				      const bool activeOnly) const {
  if ( not m_factory.listOfProduced(id).empty() ) {
    std::back_insert_iterator<std::vector<TriggerElement*> > outputIt( output );

    // 2 cases: only active ones, and all
    // one can consider sorting the TEs according to the activation status and then assume that collection is sorted
    // the same functor can be used as in copy_if
    if (activeOnly)
      remove_copy_if ( m_factory.listOfProduced(id).begin(), m_factory.listOfProduced(id).end(), outputIt, isNotActive );
    else
      copy ( m_factory.listOfProduced(id).begin(), m_factory.listOfProduced(id).end(), outputIt );
  }
}

void TrigNavStructure::getAll ( std::vector< TriggerElement* >& output, const bool activeOnly) const {
  if ( not m_factory.listOfProduced().empty() ) {
    std::back_insert_iterator<std::vector<TriggerElement*> > outputIt( output );
    if (activeOnly)
      remove_copy_if ( m_factory.listOfProduced().begin(), m_factory.listOfProduced().end(), outputIt, isNotActive );
    else
      copy ( m_factory.listOfProduced().begin(), m_factory.listOfProduced().end(), outputIt ); 
  }
}

unsigned int TrigNavStructure::countAllOfType( const te_id_type id, const bool activeOnly ) const {
  if ( activeOnly )
    return m_factory.listOfProduced(id).size()
      - count_if(m_factory.listOfProduced(id).begin(), m_factory.listOfProduced(id).end(), isNotActive);
  //else
  return m_factory.listOfProduced(id).size();
}


// need to deal with comples TopoTEs
// algorithm is realying on navigation i.e. does not check anything like
// RoIDescriptor etc. only just navigation nodes
// it packs all the RoINodes in to one vector does sort on it and the unique STL algos
// if size of temp vector is unchanged (all were unique) the there are no common RoIs
// Since this method is prepared to get RoI nodes one can use it for checking if the
// given TE is originating from given RoI, assume noone wil lquery it with initialNode
//
// in future on can revise this algorithm depending on usecases
// if most cases the NON Topo TEs will be chcked (only one RoI for each) then this can be simplified
// if RoI nodes will not be used here then security checks fro that can be abandonned)
// however this algo is N log(N) in current shape


bool TrigNavStructure::haveCommonRoI ( const TriggerElement* te1, const TriggerElement* te2 ){
  std::vector<const TriggerElement*> vecRoINodes;

  if ( isRoINode(te1) ) {
    vecRoINodes.push_back(te1);
  } else {
    std::back_insert_iterator<std::vector<const TriggerElement*> > outputIt( vecRoINodes );
    copy(te1->getRelated(TriggerElement::sameRoIRelation).begin(), te1->getRelated(TriggerElement::sameRoIRelation).end(),  outputIt);
  }
  if ( isRoINode(te2) ) {
    vecRoINodes.push_back(te2);
  } else {
    std::back_insert_iterator<std::vector<const TriggerElement*> > outputIt( vecRoINodes );
    copy(te2->getRelated(TriggerElement::sameRoIRelation).begin(), te2->getRelated(TriggerElement::sameRoIRelation).end(),  outputIt);
  }
  // sort (by pointers, it is OK do not be scared)
  sort(vecRoINodes.begin(), vecRoINodes.end());

  std::vector<const TriggerElement*>::const_iterator it;
  it = adjacent_find(vecRoINodes.begin(), vecRoINodes.end());

  if ( it == vecRoINodes.end() )
    return false;
  
  return true;
}


bool TrigNavStructure::haveDistinctRoI (const TriggerElement* te1, const TriggerElement* te2 ) {
  return ! haveCommonRoI(te1, te2);
}


bool TrigNavStructure::haveAllCommonRoIs ( const TriggerElement* te1, const TriggerElement* te2 ) {
  std::vector<TriggerElement*> vecRoINodes1 = getRoINodes(te1);
  std::vector<TriggerElement*> vecRoINodes2 = getRoINodes(te2);
  sort(vecRoINodes1.begin(), vecRoINodes1.end());
  sort(vecRoINodes2.begin(), vecRoINodes2.end());
  return vecRoINodes1 == vecRoINodes2;
}


// similar to haveCommonRoI but other relation is queried
// and is simpler (no RoI special case)

bool TrigNavStructure::haveCommonSeed ( const TriggerElement* te1, const TriggerElement* te2 ) {
  std::vector<const TriggerElement*> vecRoINodes;
  std::back_insert_iterator<std::vector<const TriggerElement*> > outputIt( vecRoINodes );

  copy(te1->getRelated(TriggerElement::seededByRelation).begin(),
       te1->getRelated(TriggerElement::seededByRelation).end(),  outputIt);

  copy(te2->getRelated(TriggerElement::seededByRelation).begin(),
       te2->getRelated(TriggerElement::seededByRelation).end(),  outputIt);

  // sort (by pointers, it is OK do not be scared)  & uniq
  sort(vecRoINodes.begin(), vecRoINodes.end());

  std::vector<const TriggerElement*>::iterator it;
  it = adjacent_find(vecRoINodes.begin(), vecRoINodes.end());
  if ( it == vecRoINodes.end() ) // after uniquing we still are
    return false;
  return true;
}

bool TrigNavStructure::haveDistinctSeed ( const TriggerElement* te1, const TriggerElement* te2 ) {
  return ! haveCommonSeed(te1, te2);
}

bool TrigNavStructure::increasingOrderByTEId (const TriggerElement* te1, const TriggerElement* te2) {
  return te1->getId() < te2->getId();
}

bool TrigNavStructure::decreasingOrderByTEId (const TriggerElement* te1, const TriggerElement* te2) {
  return te1->getId() > te2->getId();
}

bool TrigNavStructure::hasIdFromTheSet ( const TriggerElement* te, std::vector<unsigned int> ids ) {
  if ( find(ids.begin(), ids.end(), te->getId()) != ids.end() )
    return true;
  return false;
}



bool TrigNavStructure::isInitialNode( const TriggerElement* te ) {
  if ( te->getRelated(TriggerElement::seededByRelation).empty() and te->getId() == 0 )
    return true;
  return false;
}

bool TrigNavStructure::isRoINode( const TriggerElement* te )  {
  if ( te->getRelated( TriggerElement::seededByRelation ).empty() ) // this s initial node
    return false;

  TriggerElement* whoSeedsMe = *(te->getRelated( TriggerElement::seededByRelation ).begin());
  if ( isInitialNode(whoSeedsMe) )
    return true;
  return false;
}

bool TrigNavStructure::isTerminalNode( const TriggerElement* te ) {
  if ( te->getRelated( TriggerElement::seedsRelation ).empty()  )
    return true;
  return false;
}




bool TrigNavStructure::isCompatibleTree( const TriggerElement* te1, const TriggerElement* te2 ) {
  using namespace std;
  if ( te1->getId() != te2->getId() ) {
    return false; // distinc Id
  }

  if ( ( isTerminalNode(te1) && isTerminalNode(te2) ) )
    return true;  // no more checks needs to be done
  else if (  isTerminalNode(te1) || isTerminalNode(te2)  )
    return false; // one of both is terminal while another is not (case when both are terminal is covered above)

  // continue subchecks otherwise (both nonterminal)

  // folow seeds otherwise
  if ( te1->getRelated(TriggerElement::seedsRelation).size() !=  te2->getRelated(TriggerElement::seedsRelation).size() )
    return false;

  std::vector<TriggerElement*>::const_iterator seededByTe1 = te1->getRelated(TriggerElement::seedsRelation).begin();
  std::vector<TriggerElement*>::const_iterator seededByTe2 = te2->getRelated(TriggerElement::seedsRelation).begin();

  for ( ; seededByTe1 != te1->getRelated(TriggerElement::seedsRelation).end(); ++seededByTe1, ++seededByTe2 ) {
    if (isCompatibleTree(*seededByTe1, *seededByTe2) == false) // grrr,wrrr recursive call here
      return false;
  }
  // there I should probably check as well the Features attached to the TEs

  return true;
}



bool TrigNavStructure::propagateDeactivation(const TrigNavStructure* nav) {
  // basic checks first
  if (  nav->m_factory.listOfProduced().size() > m_factory.listOfProduced().size() )
    return false;

  std::vector< TriggerElement* >::const_iterator old = nav->m_factory.listOfProduced().begin();
  std::vector< TriggerElement* >::iterator me = m_factory.listOfProduced().begin();
  //  int i = 0;
  do {
    (*me)->setActiveState((*old)->getActiveState());
    ++me;
    ++old;
  } while ( old != nav->m_factory.listOfProduced().end() );
  return true;
}





bool TrigNavStructure::combine(std::vector<unsigned int>& types,
                             std::vector<std::vector<TriggerElement*> >& output,
                             unsigned int maxResults, bool onlyActive)
{
  // It should be empty anyways, but just in case...
  output.clear();

  std::vector< std::vector<TriggerElement*> > input;
  std::vector< std::vector<std::pair<unsigned int, unsigned int> > >  newIdxs;

  for (std::vector<unsigned int>::iterator type = types.begin();
       type !=  types.end(); ++type) {
    // Use the previous iteration's output as the input for this one
    std::vector< std::vector<TriggerElement*> > input = output;
    std::vector< std::vector<std::pair<unsigned int, unsigned int> > >  currentIdxs = newIdxs;

    // Remove the old n-tuples
    output.clear();
    newIdxs.clear();
    // Re-populate output with the (n+1)-tuples. If error return false immediately...
    if (not addOneLevel(input, currentIdxs, *type, output, newIdxs,
			maxResults, onlyActive)) return false;
  }

  return true;
}


bool TrigNavStructure::addOneLevel(std::vector< std::vector<TriggerElement*> >& currentCombs,
				   std::vector< std::vector<std::pair<unsigned int, unsigned int> > >& currentIdxs,
				   unsigned int type,
				   std::vector<std::vector<TriggerElement*> >& newCombs,
				   std::vector< std::vector<std::pair<unsigned int, unsigned int> > >& newIdxs,
				   unsigned int maxResults, bool onlyActive)
{
  // Get the list of TEs of the desired type from the event
  std::vector<TriggerElement*> newTEs;
  getAllOfType(type, newTEs, onlyActive);

  if (currentCombs.size() == 0) {


    for (unsigned int j = 0; j < newTEs.size(); j++) {

      std::vector<TriggerElement*> cbs;
      cbs.push_back(newTEs[j]);
      newCombs.push_back(cbs);

      // Do the same for the auxiliary data
      std::vector<std::pair<unsigned int, unsigned int> > ti;
      ti.push_back(std::pair<unsigned int, unsigned int>(type, j));
      newIdxs.push_back(ti);
    }
    return 1;
  }

  // Loop over existing combinations, adding the newTEs one by one
  for (unsigned int i = 0; i < currentCombs.size(); i++) {

    // Get the current combinations
    std::vector<TriggerElement*>& comb = currentCombs[i];

    // Get the auxiliary data that stores the type and index of each TE.
    // "index" is the index of the TE in the vector of TEs returned by getallOfType - it
    // is assumed to stay the same between calls of addOneLevel within the same combine() call
    std::vector<std::pair<unsigned int, unsigned int> >& ti = currentIdxs[i];

    // The auxiliary data is used to avoid double-counting: we make sure that if multiple
    // TEs of the same type appear in the n-tuple, they are always such that
    // their indices in the getAllOfType vector increases with position in the n-tuple.
    // I.E if we have TEs (e0, m1, e1, e2) and e0, e1 and e2 are of the same type,
    // we only take the combination such that e0 comes before e1 and e1 before e2 in
    // the getAllOfType vector of "e".

    // To implement this, go through the auxiliary data first and get the index
    // of the last TE of the current type that is already in the ntuple:
    int first = 0;

    for (unsigned int j = comb.size(); j > 0; j--)
      if (ti[j - 1].first == type) {
        first = ti[j - 1].second;
        break;
      }

    // Now instead of considering all the TEs in newTEs, only loop over those with
    // index > first.

    for (unsigned int j = first + 1; j < newTEs.size(); j++) {

      TriggerElement* te = newTEs[j];

      // Continue if the TE overlaps with any of the TEs in the existing combination
      if (overlaps(te, comb)) continue;

      // Add the new TE to the current combination
      comb.push_back(te);

      // Add the current combination to the list for the next level
      newCombs.push_back(comb);

      // Restore the current combination for the next iteration
      comb.pop_back();

      // Do the same for the auxiliary data
      ti.push_back(std::pair<unsigned int, unsigned int>(type, j));
      newIdxs.push_back(ti);
      ti.pop_back();

      // If the limit is exceeded, return immediately.
      if (maxResults > 0 && newCombs.size() >= maxResults) {
        return true;
      }
    }
  }

  return true;
}

bool TrigNavStructure::overlaps(const TriggerElement* te1, const std::vector<TriggerElement*>& teVec) const {
  for (std::vector<TriggerElement*>::const_iterator te2 = teVec.begin(); te2 !=  teVec.end(); ++te2)
    if (overlaps(te1, *te2)) return true;

  return false;
}

bool TrigNavStructure::overlaps(const TriggerElement* te1, const TriggerElement* te2) const {
  if (te1 == te2) return true;
  if (haveCommonRoI(te1, te2)) return true;

  return false;
}

bool TrigNavStructure::getTopologicallySpannedBy( const TriggerElement* te, const std::vector<unsigned int>& types,
                                                std::vector<TriggerElement*>& children, const bool activeOnly ) const {
  if ( getDirectPredecessors(te).size() != types.size() ) {
    return false;
  }

  unsigned i;
  for ( i = 0 ; i < types.size(); ++i ) {
    TriggerElement* child = const_cast<TriggerElement*>(getSuccessor( getDirectPredecessors(te)[i], types[i]));
    if(child!=0) {
      if(activeOnly && !child->getActiveState()) {
        children.clear();
        return true;
      }
      children.push_back(child);
    } else {
      children.clear();
      return true;
    }
  }
  return true;
}

unsigned int TrigNavStructure::copyAllFeatures( const TriggerElement* sourceTE, TriggerElement* destTE)
{
  unsigned int n_features = 0;
  // push back all featureAccessHelpers from source to destiny TE
  for ( std::vector< TriggerElement::FeatureAccessHelper >::const_iterator it = sourceTE->getFeatureAccessHelpers().begin();
        it != sourceTE->getFeatureAccessHelpers().end(); ++it ) {
    destTE->addFeature(*it);
    n_features++;
  }
  return n_features;

}



/*****************************************************************************
 *
 * very important RESET
 *
 *****************************************************************************/
void TrigNavStructure::reset() {
  //  std::cerr << "resetting" << std::endl;
  m_factory.reset();
  m_holderstorage.reset();
}

sub_index_type TrigNavStructure::subType(class_id_type clid, const index_or_label_type& sti_or_label) const {
  return m_holderstorage.getSubTypeIndex(clid,sti_or_label);
}

std::string TrigNavStructure::label(class_id_type clid, const index_or_label_type& sti_or_label) const {
  return m_holderstorage.getLabel(clid,sti_or_label);
}


TriggerElement::FeatureAccessHelper TrigNavStructure::getFeature(const TriggerElement* te, class_id_type clid, const index_or_label_type& index_or_label) const {


  TriggerElement::FeatureVec features;
  bool single = true; bool cache_rec = false; bool recursively = false;
  bool status = getFeatureAccessors(te, clid,index_or_label,single,features,cache_rec, recursively);

  if(status && !features.empty()){
    return features.front();
  }
  return TriggerElement::FeatureAccessHelper(); // not found
}


TriggerElement::FeatureAccessHelper TrigNavStructure::getFeatureRecursively(const TriggerElement* startTE, class_id_type clid,
									    const index_or_label_type& index_or_label, const TriggerElement*& sourceTE) const {  

  TriggerElement::FeatureVec features;
  bool single = true; bool cache_rec = false; bool recursively = true;
  bool status = getFeatureAccessors(startTE, clid,index_or_label,single,features,cache_rec, recursively,sourceTE);

  if(status && !features.empty()){
    return features.front();
  }
  return TriggerElement::FeatureAccessHelper(); // not found
}

bool TrigNavStructure::getFeatureAccessorsSingleTE( const TriggerElement* te, class_id_type clid,
						    const index_or_label_type& index_or_label,
						    bool only_single_feature,
						    TriggerElement::FeatureVec& features,
						    bool with_cache_recording,
						    const TriggerElement*& source,
						    std::string& sourcelabel ) const {

  // ATH_MSG_VERBOSE("getFeatureAccessorsSingleTE: looking for:" << (only_single_feature ? "one object" : "many objects" ) << " of CLID: " << clid
  // 		  << " label: \"" << label << "\"" << " starting from TE: " << te->getId());

  //remove unused warning
  (void)(with_cache_recording);
  
  int size = te->getFeatureAccessHelpers().size(), it;
  
  // loop the feature access helper in order depending of type of request (i.e. if single featyure needed then loop from back, if all then loop from the front)
  for ( it = ( only_single_feature ? size-1 : 0 ); it != (only_single_feature ? -1 : size ); only_single_feature ? it--: it++ ) {
    auto& fea = te->getFeatureAccessHelpers().at(it);
    //ATH_MSG_VERBOSE("getFeatureAccessors: in a loop over FeatureAccessHelpers got ObjectIndex " << fea);

    if(matchFeature(fea,clid,index_or_label)){
      sourcelabel = label(clid,fea.getIndex().subTypeIndex());
      source = te;
      features.push_back(fea);

      // ATH_MSG_DEBUG("getFeatureAccessors: matching feature found in te: " << *te << " index: " << fea);
      // now the ending (depends on the "single" flag)
      if ( only_single_feature )
	break;
    }
  } // end of loop over feature access helpers of this TE
  
  return true;
}

bool TrigNavStructure::getFeatureAccessors( const TriggerElement* te, class_id_type clid,
					    const index_or_label_type& index_or_label,
					    bool only_single_feature,
					    TriggerElement::FeatureVec& features,
					    bool with_cache_recording,
					    bool travel_backward_recursively,
					    const TriggerElement*& source,
					    std::string& sourcelabel ) const {

  bool singleTEstatus = getFeatureAccessorsSingleTE(te,clid,index_or_label,only_single_feature,features,with_cache_recording,source,sourcelabel);

   if(!singleTEstatus){
     // MLOG(WARNING) << "getFeatureAccessorsSingleTE() returned false" << endreq;
   }

 

  if ( ! travel_backward_recursively ) {
    return true;
  }

  // stop digging deeper if this is an RoI node already
  if ( isRoINode(te) ) {
    return true;
  }

  // return if a feature(s) is/are found
  if ( ! features.empty() ) {
    return true;
  }

  // recurse deeper
  bool recursion_status = true;
  for( TriggerElement* predecessor: getDirectPredecessors(te) ) {
     
    TriggerElement::FeatureVec features_in_branch;

    recursion_status = recursion_status && getFeatureAccessors( predecessor, clid, index_or_label,
								only_single_feature, features_in_branch,
                                                                with_cache_recording, travel_backward_recursively,
								source, sourcelabel);
    features.insert(features.end(),  features_in_branch.begin(), features_in_branch.end());
  }
    
  if ( only_single_feature &&  ( features.size() > 1 || recursion_status == false) ) {
    // MLOG(DEBUG) << "getFeatureAccessors: looking for object of CLID: " << clid
    //             << " label: \"" << label << "\"" << " found several objects matching criteria while can only return back one, this is ambiguous" << endreq;
      
    if ( getDirectPredecessors(te).size() > 1 ) // mark bifurcation point as to where one can start again
      source = te;

    return false;
  }
  return true;
}


bool TrigNavStructure::matchFeature(const TriggerElement::FeatureAccessHelper& fea, class_id_type clid, const index_or_label_type& index_or_label) const {
  //we always require the CLID to match
  if(fea.getCLID() != clid) return false;

  if(index_or_label.which() == 0){
    //subtype index case: if argument is invalid_sub_index we always match, else require exact match
    auto index = boost::get<sub_index_type>(index_or_label) ;
    return (index == invalid_sub_index || index == fea.getIndex().subTypeIndex());
  }

  if(index_or_label.which() == 1){
    //label case: if argument is "" we always match, else require exact match (via)
    auto label = boost::get<std::string>(index_or_label);
    if(label.empty()){
      return true;
    }
    label = (label == "!") ? "" : label;
    auto sub = subType(clid,label);
    if(sub == invalid_sub_index) return false;
    return matchFeature(fea,clid,sub);
  }
  return false;
}

const BaseHolder* TrigNavStructure::getHolder(const TriggerElement::FeatureAccessHelper& fea) const { 
  return m_holderstorage.getHolderForFeature(fea);
}
