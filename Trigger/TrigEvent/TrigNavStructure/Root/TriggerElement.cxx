/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdexcept>
#include <iterator>
#include <functional>
#include <boost/lexical_cast.hpp>
#include "TrigNavStructure/Types.h"
#include "TrigNavStructure/TriggerElement.h"

using namespace std;
using namespace HLT;
TriggerElement::TriggerElement()
    : m_state(activeState), m_id(0), m_transient(0) {
}

TriggerElement::TriggerElement(te_id_type id, bool ghost, bool nofwd)
    : m_state(activeState), m_id(id), m_transient(0) {
    if ( ghost )
	m_state |= ghostState;
    if ( nofwd )
	m_state |= nofwdState;
}

TriggerElement::~TriggerElement() {
}
/*
  void TriggerElement::reset() {
  //
  m_uses.clear();
  m_prev.clear();
  m_relations.clear(); //< do not worry! map is clearing recursivelly lists also
  }
*/
void TriggerElement::addFeature( class_id_type clid, const ObjectIndex& index, bool forget) {
  addFeature(FeatureAccessHelper(clid, index, forget));
}

void TriggerElement::addFeature( const FeatureAccessHelper& f) {
  const static size_t max_features_per_te = 0xfff;
  if ( m_uses.size() == max_features_per_te-1 ) {
    std::cout << "ERROR -  count of features per TE exceeds the limitation, further attach operations are ignored" << std::endl;
  }
  if ( m_uses.size() == max_features_per_te ) {
    return;
  }
  //  if ( m_uses.size() > 100 ) 
  //    std::cout << "INFO - many features per TE " << f.getCLID() << std::endl;
  m_uses.push_back( f );
  m_prev.push_back( f );
}

void TriggerElement::setActiveState( bool state ) {
    if ( m_state & nofwdState ) // this TE is immune to state changes 
	return;

    state ? m_state |= activeState :  m_state &= ~activeState;
    //  std::cerr << "Deactivating " << m_id  << std::endl;
    // propagate forward the state of this TE if deactivated
    // if activated then that attempt is illogical, theredore not done

    if ( state == true )
	return;

    // this node is terminal or it is special TE type which is holding fwd deactivation
    if ( getRelated(seedsRelation).empty() )
	return;

    //  std::cerr << "Propagating deactivation "  << std::endl;
    std::vector<TriggerElement*>::const_iterator it;
    for ( it = getRelated(seedsRelation).begin(); it != getRelated(seedsRelation).end(); ++it ) {
	(*it)->setActiveState(false);
    }
}

void TriggerElement::setErrorState(bool state) {
    state ? m_state |= errorState : m_state &= ~errorState; 
}

void TriggerElement::setGhostState( bool state) {
  state ? m_state |= ghostState : m_state &= ~ghostState;  
}

void TriggerElement::relate( TriggerElement* te, Relation r ) {
    // secure form multiple relations of the same type
    // silently ignoring if already related
    // his helps if one wants the Topo TE to merge several RoIs
    if ( find(m_relations[r].begin(), m_relations[r].end(), te) ==  m_relations[r].end() )
	m_relations[r].push_back(te);
}


void TriggerElement::relate( const std::vector<TriggerElement*>& tes, Relation r ) {
    // secure form multiple relations of the same type
    std::vector<TriggerElement*>::const_iterator it;
    for ( it = tes.begin(); it != tes.end(); ++it ) {
	relate(*it, r);
    }
}



/*****************************************************************************
 *
 * SERIALIZATION
 *
 *****************************************************************************/

unsigned int TriggerElement::enquireId( std::vector<uint32_t>::const_iterator& inputIt ) {
    return unsigned(*inputIt);
}

void insertUint16ToUint32Vector(std::vector<uint32_t>& v, uint16_t val, unsigned count) {
    uint32_t s=0;
    if (count % 2 == 0 ) {
	s = (uint32_t(val) << 16) & 0xffff0000;
	v.push_back(s);
    } else {
	v.back() |= uint32_t(val & 0xffff);
    }
}

void extractUint16FromUint32Vector( std::vector<uint32_t>::const_iterator& it, uint16_t& val, unsigned count) {
    if ( count % 2 == 0 ) { // do need to increment iterator
	val = ((*it) >> 16 ) & 0xffff;
    } else {
	val = (*it) & 0xffff;
	++it;
    }
}

void TriggerElement::serialize( std::vector<uint32_t>& output, const std::map<TriggerElement*, uint16_t>& keys,
				const TriggerElement* /*previous*/ ) const {

    output.push_back(m_id);   // record the TE id first
    unsigned summaryIndex = output.size();  // reserve space for vaious caounters and state
    output.push_back(0);

    auto it = m_relations.find (seededByRelation);
    std::vector<TriggerElement*> dumvec;
    const std::vector<TriggerElement*>& relvec =
      (it != m_relations.end() ? it->second : dumvec);

    std::vector<TriggerElement*>::const_iterator teIt;
    unsigned relationsCount = 0;
    // go over all TE which seed me and record link to them
    for ( teIt = relvec.begin(); teIt != relvec.end(); ++teIt ) {
	std::map<TriggerElement*, uint16_t>::const_iterator key = keys.find(*teIt);
	if ( key != keys.end() ) {
	    insertUint16ToUint32Vector(output, key->second, relationsCount);
	    relationsCount++;
	} 
	// other are forgotten silently .... joke, there will be none
	// due to fact that the to create TE one needs to know all the TEs seeding it
	// so they are before on the list and in the "keys" map
    }

    // here one should put only this features which are not to to be forget (we do not use this now)
    std::vector< FeatureAccessHelper >::const_iterator featuresIt;
    unsigned featuresCount = 0;

    for ( featuresIt = m_uses.begin(); featuresIt != m_uses.end(); ++featuresIt ) {
	if ( ! featuresIt->forget() ) {
	    output.push_back ( featuresIt->getCLID() );
	    featuresIt->getIndex().serialize(output);
	    featuresCount++;
	}
    }
    uint32_t seedingUsesStateWord = ((relvec.size() << 20)) | (featuresCount << 8) | (m_state & 0xf);
    output[summaryIndex] = seedingUsesStateWord;
    /*
    std::cerr << "Serialized TE id: " << m_id << " fea: " << featuresCount 
	      <<  " forg: "  << m_uses.size() - featuresCount
	      << " rel: "<< m_relations[seededByRelation].size() << " raw: " << seedingUsesStateWord<< std::endl;
    */
}



void TriggerElement::deserialize( std::vector<uint32_t>::const_iterator& inputIt,
				  const std::map<uint16_t, TriggerElement*>& keys, const TriggerElement* /*previous*/) {

    m_id = *inputIt++;
    uint32_t seedingUsesStateWord = *inputIt++;
    m_state = seedingUsesStateWord & 0xf;
    unsigned int size = (seedingUsesStateWord & 0xfff00000) >> 20;
    unsigned int featuresAttached = (seedingUsesStateWord & 0x000fff00) >> 8;
    //    std::cerr << "Try DeSerialize TE id: " << m_id << " fea: " << featuresAttached << " rel: "<< size << " raw " << seedingUsesStateWord << std::endl;



    std::map<uint16_t, TriggerElement*>::const_iterator id;
    unsigned relationsCount = 0;
    uint16_t key{0};
    for ( unsigned int i = 0 ; i < size; ++i ) {
	extractUint16FromUint32Vector(inputIt, key, relationsCount); // extract the key from the payload
	++relationsCount;
	// look for that key
	id = keys.find(key);
	if ( id != keys.end() ) {
	    TriggerElement* te = id->second;
	    //	    relate ( te, seededByRelation );
	    te->relate ( this, seedsRelation );
	    this->relate ( te, seededByRelation ); // seededBy and seeds are reflexive ...
 	    m_prev.reserve(m_prev.size() + te->getPreviousFeatures().size());
	    m_prev.insert(m_prev.end(), te->getPreviousFeatures().begin(), te->getPreviousFeatures().end() ); // rebuild previous features list
	}
    }
    if ( relationsCount %2 == 1 )  inputIt++; // we need to do this because if number of features is odd the payload iterator is not incremented by extract...

    // unpack features holders info
    m_uses.reserve(m_uses.size() + featuresAttached);
    m_prev.reserve(m_prev.size() + featuresAttached);
    for ( unsigned int i = 0 ; i < featuresAttached; ++i ) {
	unsigned int clid = *inputIt++;
	ObjectIndex coordinate;
	coordinate.deserialize(inputIt);
	addFeature(clid, coordinate);
    }
}

/*
  void TriggerElement::appendFeaturesKeyPrint(const std::vector<FeatureAccessHelper>& k) {
  m_keyPrint.insert(m_keyPrint.end(), k.begin(), k.end());
  }
*/

//////////////////////////////////////////////////////////////////
//
// Object index

TriggerElement::ObjectIndex::ObjectIndex() 
  : m_subTypeIndex(invalid_sub_index), m_objIndexBegin(invalid_index), m_objIndexEnd(invalid_index) {}


TriggerElement::ObjectIndex::ObjectIndex(sub_index_type subType, index_type begin, index_type end) 
  : m_subTypeIndex(subType), m_objIndexBegin(begin), m_objIndexEnd(end)
{
    if (subType >= invalid_sub_index)
	throw std::runtime_error("To many collections of one type: " + boost::lexical_cast<std::string>(subType)+" >= 0xffff" );
}



uint16_t TriggerElement::ObjectIndex::subTypeIndex() const {
    return m_subTypeIndex;
}
/*
uint32_t TriggerElement::ObjectIndex::objectIndex() const {
  return m_objIndexBegin;
}
*/

uint32_t TriggerElement::ObjectIndex::objectsBegin() const {
  return m_objIndexBegin;
}

uint32_t TriggerElement::ObjectIndex::objectsEnd() const {
  return m_objIndexEnd;
}

void TriggerElement::ObjectIndex::updateBeginAndEnd( index_type begin, index_type end) {
  m_objIndexBegin = begin; 
  m_objIndexEnd   = end;
}

bool TriggerElement::ObjectIndex::operator<(const ObjectIndex& obj) const {
  if ( m_subTypeIndex != obj.m_subTypeIndex )
    return m_subTypeIndex < obj.m_subTypeIndex;
  if ( m_objIndexBegin != obj.m_objIndexBegin )
    return m_objIndexBegin < obj.m_objIndexBegin;
  
  return m_objIndexEnd < obj.m_objIndexEnd;
}

void TriggerElement::ObjectIndex::serialize(std::vector<uint32_t>& output) const {
    // here comec the compression 
    // check if we can fit into one int
    // ie. subtypeIndex fits into the 4 bits (0-8)
    // if the begin  indexes fit into the 13 bits (0-8192 )
    // if the end    indexes fit into the 14 bits (0-16383 )
  
    if ( (0 == (m_subTypeIndex & ~0xf)) && (0 == (objectsBegin() & ~0x1fff)) && ( 0 == (objectsEnd() & ~0x3fff)) ) {
	output.push_back(0x80000000 | m_subTypeIndex | objectsEnd()<< 4 | objectsBegin() << 18);
	//	std::cerr << "feature idx ser: " << std::hex << output.back() << std::dec << std::endl;

    } else if ( (0 == (objectsBegin() & ~0xffff)) && (0 == (objectsEnd() & ~0xffff)) ) {
      // check if we can fit into two ints
      // ie. subtypeIndex fits into the 16 bits 
      // if the begin  indexes fit into the 16 bits (0-65536 )
      // if the end    indexes fit into the 16 bits (0-65536)
      
      //	std::cerr << "feature idx2: " << std::hex << m_subTypeIndex << " " << m_objIndex << std::dec << std::endl;
	output.push_back(m_subTypeIndex);
	output.push_back((objectsBegin() << 16) |  (objectsEnd()) );
    } else {
      // we have to use 3 words
      output.push_back(m_subTypeIndex | 0x40000000); // second most significant bit marks that
      output.push_back(objectsBegin());
      output.push_back(objectsEnd());
    }
}

void TriggerElement::ObjectIndex::deserialize( std::vector<uint32_t>::const_iterator& inputIt) {
    // check if compressed reference
    uint32_t w = *inputIt++; // pickup and advance
    if ( w & 0x80000000 ) {
      // small numbers (compressed to one word)
	m_subTypeIndex  = w & 0xf;
	m_objIndexBegin =  ( (w>>18) &  0x1fff );
	m_objIndexEnd   =  (w>>4) & 0x3fff;
	//	std::cerr << "feature idx deser: " << std::hex << w << std::dec << " " << std::endl;
    } else if ( (w & 0x40000000) == 0 ) {   
      // medium numbers (compressed to 2 words)
	m_subTypeIndex  = w & 0xffff; 
	uint32_t w2 = *inputIt++;
	m_objIndexBegin = w2 >> 16; 
	m_objIndexEnd   = w2 & 0xffff; 
	//	std::cerr << "feature idx2: " << std::hex << m_subTypeIndex << " " << m_objIndex << std::dec << std::endl;
    } else {
      // huge numbers (3 word involved)
      m_subTypeIndex  = w & 0xffff;
      m_objIndexBegin = *inputIt++;
      m_objIndexEnd   = *inputIt++;

    }
    // at the end the inputIt is pointing outside the ObjectIndex
}

bool TriggerElement::ObjectIndex::isSameOrWithin ( const ObjectIndex* idx) const {
    if ( this->subTypeIndex() != idx->subTypeIndex() )
	return false;
    if ( this->objectsBegin() > idx->objectsBegin() )
	return false;
    if ( this->objectsEnd() < idx->objectsEnd() )
	return false;
    return true;
}

bool TriggerElement::ObjectIndex::valid() const {
  if ( m_subTypeIndex == invalid_sub_index && m_objIndexBegin == invalid_index && m_objIndexEnd == invalid_index )
    return false;
  return true;
}

/*
MsgStream& operator<< ( MsgStream& m, const HLT::TriggerElement::ObjectIndex& i ) {
  m << "SubTypeIdx: " << i.subTypeIndex() << " begin: " << i.objectsBegin() << " end: " << i.objectsEnd();
  return m;
    
}

MsgStream& operator<< ( MsgStream& m, const HLT::TriggerElement& te ) {
  m << "TE id: " << te.getId() << " ac: " << te.getActiveState() << " err: " << te.getErrorState();
  return m;
}
*/
