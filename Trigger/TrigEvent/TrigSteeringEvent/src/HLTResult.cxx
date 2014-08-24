/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/HLTExtraData.h"

#include "CLIDSvc/CLASS_DEF.h"
#include <iostream> // remove me ???
#include <algorithm>
using namespace HLT;

///
/// \file HLTResult.cxx
/// \author andreas.hoecker@cern.ch till.eifert@cern.ch nicolas.berger@cern.ch tomasz.bold@cern.ch

/// version of HLTResult class: change this upon schema evolution
const uint32_t HLTResult::m_HLTResultClassVersion = 3;

/// constructor
HLTResult::HLTResult() :
  m_extraData(0)
{
  m_headerResult.resize(IndNumOfFixedBit);
  m_headerResult[IndHLTResultClassVersion]    = m_HLTResultClassVersion;
  //std::cerr << "HLTResult instantiated with version " << m_HLTResultClassVersion << std::endl; 
  // set all fixed bits, if there will be an error then they will be OK ??? mean showing an error
  setLvl1Id(0xffffffff);
  setAccepted(false);

  setHLTStatus(HLT::FATAL);

  setLvlConverterStatus(HLT::FATAL);

  setPassThrough(false);

  setHLTLevel(HLT::UNKNOWN);

  setNumOfSatisfiedSigs(0);

  setErrorCoordinates(0, 0xffffffff); // quite unique way of saying that this was not used

  setCreatedOutsideHLT(true);

  setHLTResultTruncated(false);
}

/// copy constructor
HLTResult::HLTResult( const HLTResult& hltResult )
  : GenericResult(),
    m_headerResult(hltResult.m_headerResult),
    m_chainsResult(hltResult.m_chainsResult),
    m_navigationResult(hltResult.m_navigationResult),
    m_navigationResult_DSonly(hltResult.m_navigationResult_DSonly),
    m_id_name(hltResult.m_id_name),
    m_id_name_DSonly(hltResult.m_id_name_DSonly),
    m_modID_id_name(hltResult.m_modID_id_name),
    m_navigationResultCuts(hltResult.m_navigationResultCuts),
    m_navigationResultCuts_DSonly(hltResult.m_navigationResultCuts_DSonly)
{
}

/// destructor
HLTResult::~HLTResult() {
  //  this->clearHLTResult();
  delete m_extraData;
}

void HLTResult::listOfModIDs(std::vector<unsigned int>& mod_ids) const{
  
  for (std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::const_iterator map_it = m_modID_id_name.begin(); 
       map_it != m_modID_id_name.end(); ++map_it)
    mod_ids.push_back((*map_it).first);
}

std::vector<unsigned int> HLTResult::listOfModIDs() const{
  
  std::vector<unsigned int> mod_ids;
  for (std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::const_iterator map_it = m_modID_id_name.begin(); 
       map_it != m_modID_id_name.end(); ++map_it)
    mod_ids.push_back((*map_it).first);
  return mod_ids;
}

bool HLTResult::error() const {
  return (getHLTStatus().action() >= HLT::Action::ABORT_CHAIN) ? true : false; 
}

uint32_t HLTResult::error_bits() const {
  uint32_t bits = 0;
  // result of erros in previous levels (not correct previous level result)
  if (getLvlConverterStatus().action() != HLT::Action::CONTINUE)  bits |= 0x1;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::NO_LVL1_ITEMS)     bits |= 0x2;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::NO_LVL2_CHAINS)    bits |= 0x4;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::WRONG_HLT_RESULT)  bits |= 0x8;
  if (getLvlConverterStatus().steeringInternalReason()  == HLT::SteeringInternalReason::NO_HLT_RESULT)     bits |= 0x10;

  // result of erros in this level
  if (getHLTStatus().action() == HLT::Action::ABORT_JOB)             bits |= 0x100;
  if (getHLTStatus().action() == HLT::Action::ABORT_EVENT)           bits |= 0x200;
  if (getHLTStatus().action() == HLT::Action::ABORT_CHAIN)           bits |= 0x400;
  if (getHLTStatus().reason() == HLT::Reason::TIMEOUT)               bits |= 0x800;
  if (getHLTStatus().reason() == HLT::Reason::NAV_ERROR)             bits |= 0x1000;
  return bits;
}



bool HLTResult::serialize( std::vector<uint32_t>& output ) {

  unsigned int mod_id = 0;
  return serialize(output, mod_id);
}

bool HLTResult::serialize(std::vector<uint32_t>& output, const unsigned int mod_id){
  output.clear();
  return packForStorage(output, mod_id);
}



bool HLTResult::serialize( uint32_t*& output, int& data_size, const int max_size, const unsigned int mod_id ) {
  using namespace std;
  if (mod_id == 0) { 
    //  cerr << "HLTResult::serialize  max size asked: " << max_size << " estimate: " << estimateSize() << endl; 
    if ( max_size == -1 || (int)estimateSize() < max_size   ) {
      //    cerr << "HLTResult::serialize  truncation not needed " << endl; 
      setHLTResultTruncated(false);
      std::vector<uint32_t> rawResult;
      // Till: here we copy the rawResult 2 times! first, in packForStorage and then in the copy below ... ?
      
      packForStorage(rawResult, mod_id); // I do not care about the result
      
      data_size = rawResult.size();
      output = new uint32_t [data_size];
      copy(rawResult.begin(), rawResult.end(), output);
      //    cerr << "HLTResult::serialize  data size: " << data_size << endl;     
      return true; // full success
    }

    // below goes handling of truncation
    setHLTResultTruncated(true);
    data_size = 0;
    
    //  cerr << "HLTResult::serialize  truncation NEEDED " << endl; 
    
    // if max_size is smaller then the header, there is nothing we can do...
    if (static_cast<unsigned int>(max_size) < m_headerResult.size())
      return false;
    
    // add header
    output = new uint32_t [max_size];
    copy( m_headerResult.begin(), m_headerResult.end(), &output[0] );
    data_size += std::distance(m_headerResult.begin(), m_headerResult.end());
    
    //  cerr << "HLTResult::serialize w/o Chains size is : " << data_size << endl; 
    
    // check if chains can be added
    if ( data_size +   m_chainsResult.size() + 1 > static_cast<unsigned int>(max_size) )
      return false;
    
    output[data_size] =  m_chainsResult.size();
    data_size++;
    //  cerr << "HLTResult::serialize with Chains still fit : " << data_size << endl; 
    copy( m_chainsResult.begin(), m_chainsResult.end(), &output[data_size] );
    data_size += std::distance(m_chainsResult.begin(), m_chainsResult.end());
    
    //  cerr << "HLTResult::serialize with Chains size is : " << data_size << endl; 
    
    // check if navigation can be added  ( sure not, we werent be here if that would be the case )
    // find which cut will still be OK.
    //  copy(m_navigationResultCuts.rbegin(), m_navigationResultCuts.rend(),  ostream_iterator<uint32_t>(cerr, " "));
    
    std::vector<unsigned int>::reverse_iterator cut = std::find_if( m_navigationResultCuts.rbegin(),
								    m_navigationResultCuts.rend(),
								    bind2nd(std::less<int>(), max_size-data_size-1) );
    
    
    
    if ( cut == m_navigationResultCuts.rend() ) {
      //    cerr << "HLTResult::serialize navigation should be cut out totaly " << endl;    
      return false;
    }
    
    //  cerr << "HLTResult::serialize navigation should be cut at: " << *cut << endl;
    //  std::vector<uint32_t>::iterator endIt = m_navigationResult.begin();
    //  advance(endIt, *cut);
    
    // crop the navigation data now
    
    //m_navigationResult.resize(*cut);
    //output[data_size] = m_navigationResult.size();
    //data_size++;
    //copy(m_navigationResult.begin(), m_navigationResult.end(), &output[data_size]);
    //data_size += m_navigationResult.size();
    
    // create another vector to cut because you need navigation results for data-scouting
    std::vector<uint32_t> copied_results = m_navigationResult;
    copied_results.resize(*cut);
    output[data_size] = copied_results.size();
    data_size++;
    copy(copied_results.begin(), copied_results.end(), &output[data_size]);
    data_size += copied_results.size();
    
    // actuall copy
    //  output[data_size] = *cut;
    //  data_size++;
    //  copy( m_navigationResult.begin(), endIt, &output[data_size] );
    //  data_size += *cut; 
    //  cerr << "HLTResult::serialize final data size is: " << data_size << endl;
    
    return false;
  }
  
  else{
    if ( max_size == -1 || (int)estimateSize_DS(mod_id) < max_size ) {
      //    cerr << "HLTResult::serialize  truncation not needed " << endl; 
      setHLTResultTruncated(false);
      std::vector<uint32_t> rawResult;
      
      // Till: here we copy the rawResult 2 times! first, in packForStorage and then in the copy below ... ?
      packForStorage(rawResult, mod_id); // I do not care about the result
      
      data_size = rawResult.size();
      output = new uint32_t [data_size];
      copy(rawResult.begin(), rawResult.end(), output);
      //    cerr << "HLTResult::serialize  data size: " << data_size << endl;     
      return true; // full success
    }
    // below goes handling of truncation
    setHLTResultTruncated(true);
    data_size = 0;
    output = new uint32_t [max_size];

    std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::iterator map_it = m_modID_id_name.begin();
    map_it = m_modID_id_name.find(mod_id);
    
    if (map_it != m_modID_id_name.end()){
      // map from ScoutingInfo for ROB ID rob_id
      std::set<std::pair<CLID, std::string> >& collectionsForStorage = (*map_it).second;
      
      unsigned int position = 0;
      
      for (unsigned int pos = 0; pos < m_id_name.size(); ++pos) {
	if( collectionsForStorage.find(m_id_name.at(pos)) != collectionsForStorage.end() ) { // compares clid and coll name between scouting info collection and navigation filled collections
	  position = pos;
	  size_t starting_bin = m_navigationResultCuts.at(position + 1); // 1 is the number of bins used to store features
	  size_t ending_bin = m_navigationResultCuts.at(position + 1 + 1); 
	  
	  std::vector<uint32_t>::iterator starting_point = m_navigationResult.begin()+starting_bin; 
	  std::vector<uint32_t>::iterator ending_point = m_navigationResult.begin()+ending_bin; 
	  
	  if (ending_bin > static_cast<unsigned int>(max_size - data_size -1))
	    return false;
	  else{
	    copy( starting_point, ending_point, &output[data_size+1] );
	    data_size += std::distance(starting_point, ending_point);
	    output[0] = data_size;
	  }
	}
      }
      
      for (unsigned int pos = 0; pos < m_id_name_DSonly.size(); ++pos) {
	if(std::find(m_id_name.begin(), m_id_name.end(), m_id_name_DSonly.at(pos)) == m_id_name.end()) {
	  if( collectionsForStorage.find(m_id_name_DSonly.at(pos)) != collectionsForStorage.end() ) { // compares clid and coll name between scouting info collection and navigation filled collections that are only in the datascouting stream
	    position = pos;
	    size_t starting_bin = m_navigationResultCuts_DSonly.at(position);
	    size_t ending_bin = m_navigationResultCuts_DSonly.at(position + 1); 
	    
	    std::vector<uint32_t>::iterator starting_point = m_navigationResult_DSonly.begin()+starting_bin; 
	    std::vector<uint32_t>::iterator ending_point = m_navigationResult_DSonly.begin()+ending_bin; 
	    
	    if (ending_bin > static_cast<unsigned int>(max_size - data_size -1))
	      return false;
	    else{
	      copy( starting_point, ending_point, &output[data_size+1] );
	      data_size += std::distance(starting_point, ending_point);
	      output[0] = data_size;
	    }
	  }
	}
      }
      return false;
    }
    else{
      std::cerr << "ROB module ID " << mod_id << " not found as key of the map from ScoutingInfo. Nothing serialized."<< std::endl; 
      return false;
    }
  }
  return false;
}


bool HLTResult::deserialize( const std::vector<uint32_t>& source ) {
  return unpackFromStorable(source);
}


bool HLTResult::deserialize(  uint32_t* source, const int data_size ) {
  if (data_size == 0 ) return false;
  std::vector<uint32_t> rawResult(&source[0], &source[data_size]);
  //  rawResult.clear();
  //  std::back_insert_iterator< std::vector<uint32_t> > appendIterator(rawResult);
  // nope this is not error, we copy up to next to the last
  //  copy(&source[0], &source[data_size], appendIterator);
  return unpackFromStorable(rawResult);
}



void HLTResult::clearHLTResult() {
  // this things are done automatically at deletion
  //  m_rawResult.clear();
  //  m_serializedObjects.clear();
  //  for (uint32_t ires=0; ires<HLTResult::IndNumOfResultTypes; ires++) m_rawPartialResults[ires].clear();
}

unsigned int HLTResult::estimateSize() {
  // If the extraData object has been used, erase the content of m_extras
  if (m_extraData) {
    m_extras.clear();
    m_extraData->serialize(m_extras);
  }
  return (IndNumOfFixedBit
	  + m_chainsResult.size() + 1      // size(one word) and payload
	  + m_navigationResult.size() + 1 
	  + m_extras.size() +1+1);           
}

unsigned int HLTResult::estimateSize_DS(const unsigned int mod_id ) {
  // Check size for Scouting data
  std::vector<uint32_t> raw;
  std::back_insert_iterator< std::vector<uint32_t> > appendIterator(raw);

  std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::iterator map_it = m_modID_id_name.begin();
  map_it = m_modID_id_name.find(mod_id);

  if (map_it != m_modID_id_name.end()){
    // map from ScoutingInfo for ROB ID rob_id
    std::set<std::pair<CLID, std::string> >& collectionsForStorage = (*map_it).second;
    
    unsigned int position = 0;
    
    for (unsigned int pos = 0; pos < m_id_name.size(); ++pos) {
      if( collectionsForStorage.find(m_id_name.at(pos)) != collectionsForStorage.end() ) { // compares clid and coll name between scouting info collection and navigation filled collections
	position = pos;
	size_t starting_bin = m_navigationResultCuts.at(position + 1); // 1 is the number of bins used to store features
	size_t ending_bin = m_navigationResultCuts.at(position + 1 + 1); 
	
	std::vector<uint32_t>::iterator starting_point = m_navigationResult.begin()+starting_bin; 
	std::vector<uint32_t>::iterator ending_point = m_navigationResult.begin()+ending_bin; 
	
	raw.reserve(ending_bin - starting_bin);
	copy(starting_point, ending_point, appendIterator); // headerblob + datablob
      }
    }
    
    for (unsigned int pos = 0; pos < m_id_name_DSonly.size(); ++pos) {
	if(std::find(m_id_name.begin(), m_id_name.end(), m_id_name_DSonly.at(pos)) == m_id_name.end()) {
	  if( collectionsForStorage.find(m_id_name_DSonly.at(pos)) != collectionsForStorage.end() ) { // compares clid and coll name between scouting info collection and navigation filled collections that are only in the datascouting stream
	    position = pos;
	    size_t starting_bin = m_navigationResultCuts_DSonly.at(position);
	    size_t ending_bin = m_navigationResultCuts_DSonly.at(position + 1); 
	    
	    std::vector<uint32_t>::iterator starting_point = m_navigationResult_DSonly.begin()+starting_bin; 
	    std::vector<uint32_t>::iterator ending_point = m_navigationResult_DSonly.begin()+ending_bin; 
	    
	    raw.reserve(ending_bin - starting_bin);
	    copy(starting_point, ending_point, appendIterator); // headerblob + datablob
	  }
	}
    }
    return raw.size() + 1; // size(one word) and payload
  }
  else{
    std::cerr << "ROB module ID " << mod_id << " not found as key of the map from ScoutingInfo. HLTResult::estimateSize_DS() Returns 0 as expected size."<< std::endl; 
    return 0;
  }
  
}

bool HLTResult::packForStorage( std::vector<uint32_t>& raw, const unsigned int mod_id )
{

  std::back_insert_iterator< std::vector<uint32_t> > appendIterator(raw );

  if (mod_id==0){

    raw.reserve( estimateSize() );
    copy(m_headerResult.begin(), m_headerResult.end(), appendIterator ); // fixed header
    
    raw.push_back( m_chainsResult.size() );
    copy( m_chainsResult.begin(), m_chainsResult.end(), appendIterator ); // chains
    
    raw.push_back( m_navigationResult.size() );
    copy( m_navigationResult.begin(), m_navigationResult.end(), appendIterator ); // navigation
    
    // If the extraData object has been used, erase the content of m_extras
    if (m_extraData) {
      m_extras.clear();
      m_extraData->serialize(m_extras);
    }
  
    raw.push_back( m_extras.size() );
    copy( m_extras.begin(), m_extras.end(), appendIterator ); // extras
    
    return HLT::OK;
    
  }else{

    std::map<unsigned int, std::set<std::pair<CLID, std::string> > >::iterator map_it;
    map_it = m_modID_id_name.find(mod_id);

    std::vector<uint32_t> result;
    std::back_insert_iterator< std::vector<uint32_t> > appendIteratorResult(result);    
        
    if (map_it != m_modID_id_name.end()){
      
      raw.reserve( estimateSize_DS(mod_id) ); // this is size + payload
      result.reserve( estimateSize_DS(mod_id)-1); // this is only payload
      
      // map from ScoutingInfo for ROB ID rob_id
      std::set<std::pair<CLID, std::string> >& collectionsForStorage = (*map_it).second;
      
      unsigned int position = 0;
      
      for (unsigned int pos = 0; pos < m_id_name.size(); ++pos) {
	if( collectionsForStorage.find(m_id_name.at(pos)) != collectionsForStorage.end() ) { // compares clid and coll name between scouting info collection and navigation filled collections
	  position = pos;
	  size_t starting_bin = m_navigationResultCuts.at(position + 1); // 1 is the number of bins used to store features
	  size_t ending_bin = m_navigationResultCuts.at(position + 1 + 1); 
	  
	  std::vector<uint32_t>::iterator starting_point = m_navigationResult.begin()+starting_bin; 
	  std::vector<uint32_t>::iterator ending_point = m_navigationResult.begin()+ending_bin; 
	  
	  result.reserve(ending_bin - starting_bin);
	  copy(starting_point, ending_point, appendIteratorResult); // headerblob + datablob
	}
      }
      
      for (unsigned int pos = 0; pos < m_id_name_DSonly.size(); ++pos) {
	if(std::find(m_id_name.begin(), m_id_name.end(), m_id_name_DSonly.at(pos)) == m_id_name.end()) {
	  if( collectionsForStorage.find(m_id_name_DSonly.at(pos)) != collectionsForStorage.end() ) { // compares clid and coll name between scouting info collection and navigation filled collections that are only in the datascouting stream
	    position = pos;
	    size_t starting_bin = m_navigationResultCuts_DSonly.at(position);
	    size_t ending_bin = m_navigationResultCuts_DSonly.at(position + 1); 
	    
	    std::vector<uint32_t>::iterator starting_point = m_navigationResult_DSonly.begin()+starting_bin; 
	    std::vector<uint32_t>::iterator ending_point = m_navigationResult_DSonly.begin()+ending_bin; 
	    
	    result.reserve(ending_bin - starting_bin);
	    copy(starting_point, ending_point, appendIteratorResult); // headerblob + datablob
	  }
	}
      }
      
      raw.push_back( result.size() );  // size
      copy( result.begin(), result.end(), appendIterator ); // payload

      return HLT::OK;
    }
    else{
      std::cerr << "ROB module ID " << mod_id << " not found as key of the map from ScoutingInfo. Nothing serialized."<< std::endl; 
      return HLT::OK;
    }
  }
}



bool HLTResult::unpackFromStorable(const std::vector<uint32_t>& raw)
{
  if (raw.empty())
    return false;

  // default assumption: fixed bits of raw data are same as present class
  unsigned int rawIndNumOfFixedBit = HLTResult::IndNumOfFixedBit;

  unsigned version = raw[0];

  // different version handling
  if ( version != m_HLTResultClassVersion){
    if (version == 1) {
      // version 1 had only 12 InitBits
      rawIndNumOfFixedBit = 12;
    } else if (version == 2 ) {
      // nothing to be done
    } else { // case we don't know about
      std::cerr << "HLTResult::unpackFromStorable has noticed a class version mismatch and does not know how to translate: raw data version " << raw[0] << " current class version " << m_HLTResultClassVersion << std::endl; 
      return false;
    }

  }
  if ( raw.size() < HLTResult::IndNumOfFixedBit )
    return false;
  m_headerResult.clear();
  m_headerResult.reserve(HLTResult::IndNumOfFixedBit);
  /*
    std::cout <<  "HLTResult::unpackFromStorable DEBUG " 
	    << " raw.size()=" << raw.size() 
	    << " HLTResult::IndNumOfFixedBit=" << HLTResult::IndNumOfFixedBit 
	    << " rawIndNumOfFixedBit=" << rawIndNumOfFixedBit
	    << " m_headerResult.size()=" << m_headerResult.size()
	    << std::endl;
  */
  m_headerResult.insert(m_headerResult.end(), &raw[0], &raw[rawIndNumOfFixedBit]);
  /*
  std::cout <<  "HLTResult::unpackFromStorable DEBUG " 
	    << " m_headerResult.size()=" << m_headerResult.size()
	    << std::endl;
  */

  // fill up with zeros so use of HLTResult::IndNumOfFixedBit of other indices past the end doesn't break
  m_headerResult.insert(m_headerResult.end(), HLTResult::IndNumOfFixedBit-rawIndNumOfFixedBit, 0);
  /*
    std::cout <<  "HLTResult::unpackFromStorable DEBUG " 
	    << " m_headerResult.size()=" << m_headerResult.size()
	    << std::endl;
  */

  if ( raw.size() ==  rawIndNumOfFixedBit )
    return true; // that's OK, we have just empty event, no processing started

  // chains
  uint32_t offset = rawIndNumOfFixedBit;
  uint32_t sizeOfChains = raw[offset];
  offset++;

  uint32_t readEnd = offset + sizeOfChains;
  bool truncation = false;
  
  if ( readEnd > raw.size() ){
    readEnd = raw.size();
    truncation = true;
  }


  m_chainsResult.clear();
  m_chainsResult.reserve( sizeOfChains );
  m_chainsResult.insert( m_chainsResult.end(), &raw[offset], &raw[readEnd] );
  offset += sizeOfChains;

  if (truncation) {
    if ( isHLTResultTruncated() ) // if truncation was marked by creator it is OK
      return true;
    else // we need to report problems as there was extra truncation in the way which might have been done w/o any care about content
      return false;
  }

  // navigation
  uint32_t sizeOfNavigation = raw[offset];
  offset++;
  uint32_t trueSizeOfNavigation = sizeOfNavigation;
  // check if offset is not beyond size of raw result  


  readEnd = offset + sizeOfNavigation;
  if (readEnd > raw.size()) {
    readEnd = raw.size();
    truncation = true;
    trueSizeOfNavigation = raw.size() - offset;
  }


  if ( offset > readEnd ) 
    return true;

  m_navigationResult.clear();
  m_navigationResult.reserve(trueSizeOfNavigation);
  m_navigationResult.insert(m_navigationResult.end(), &raw[offset], &raw[readEnd]);

  
  if (truncation) {
    if ( isHLTResultTruncated() )
      return true;
    else
      return false;
  }
  offset += sizeOfNavigation;

  if (version >= 3) {
    // extras
    uint32_t sizeOfExtras = raw[offset];
    offset++;

    readEnd = offset + sizeOfExtras;
    if (readEnd > raw.size()) {
      readEnd = raw.size();
      truncation = true;
    }

    if ( offset > readEnd )
      return true;
    
    m_extras.clear();
    m_extras.reserve(sizeOfExtras);
    m_extras.insert(m_extras.end(),  &raw[offset], &raw[readEnd]);
    if (truncation) {
      if ( isHLTResultTruncated() )
	return true;
      else
	return false;
    } 
  }

  return true;
}


unsigned int HLTResult::size() const {
  return m_headerResult.size() +  m_chainsResult.size() + m_navigationResult.size() + m_extras.size();
}

std::vector<unsigned int> HLTResult::partSizes() const {
  std::vector<unsigned int> sizes;
  sizes.reserve(4);
  sizes.push_back(m_headerResult.size());
  sizes.push_back(m_chainsResult.size());
  sizes.push_back(m_navigationResult.size());
  sizes.push_back(m_extras.size());
  return sizes;
}

bool HLTResult::isEmpty() const
{
  return (m_chainsResult.empty() && m_navigationResult.empty());
}


void HLTResult::setConfigSuperMasterKey(uint32_t key) {
  if (m_HLTResultClassVersion >=2 )
    m_headerResult[IndConfigSuperMasterKey] = key;
}


uint32_t HLTResult::getConfigSuperMasterKey() const {
  if ( m_headerResult[IndHLTResultClassVersion] >=2 )
    return m_headerResult[IndConfigSuperMasterKey];
  else
    return 0;
}

void HLTResult::setConfigPrescalesKey(uint32_t key) {
  if (m_HLTResultClassVersion >=2 )
    m_headerResult[IndConfigPrescalesKey] = key;
}

uint32_t HLTResult::getConfigPrescalesKey() const {
  if ( m_headerResult[IndHLTResultClassVersion] >=2 )
    return m_headerResult[IndConfigPrescalesKey];
  else
    return 0;      
}


HLTExtraData& HLTResult::getExtraData()
{
  // On-demand deserialization of m_extras
  if (m_extraData==0) {
    m_extraData = new HLTExtraData(m_extras);
  }  
  return *m_extraData;
}


