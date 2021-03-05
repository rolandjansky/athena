/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <iostream>

#include "TrigNavStructure/StandaloneNavigation.h"
#include "TrigNavStructure/TypelessHolder.h"

HLT::StandaloneNavigation::StandaloneNavigation(){
}

HLT::StandaloneNavigation::~StandaloneNavigation(){
}

bool HLT::StandaloneNavigation::serializeHolders( std::vector<uint32_t>& output ) const {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  const TrigHolderStructure& holderstorage = getHolderStorage();

  for ( auto holder: holderstorage.getAllHolders()) {
    // put size placeholder
    const size_t holderSizeIndex = output.size();
    output.push_back(0);

    holder->serialize(output);

    output[holderSizeIndex] = output.size() - holderSizeIndex -1;
  } 
  return true;
}


bool HLT::StandaloneNavigation::deserializeHolders(std::vector<uint32_t>::const_iterator& start,  const std::vector<uint32_t>::const_iterator& end) {  
  using namespace std;
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  TrigHolderStructure& holderstorage = getHolderStorage();

  do {
    if ( start == end ) // no holders at all
      break;
    
    const size_t holderPayloadSize = *start;    
    ++start;

    class_id_type clid;
    std::string label;
    sub_index_type sub;   
    std::vector<uint32_t>::const_iterator oneBlobIt = start;
    bool couldRead = BaseHolder::enquireSerialized(oneBlobIt, end, clid, label, sub);
    
    if ( couldRead == false ) // to few data words essentially
      break;

    //create holder
    auto holder = std::make_shared<TypelessHolder>(clid, label, sub);

    bool status = holderstorage.registerHolder(holder);
    if(!status){
      std::cerr << "ERROR registering a holder" << std::endl; 
    }

    std::advance(start, holderPayloadSize);
    
  } while(1);

  return true;
}

bool HLT::StandaloneNavigation::serialize( std::vector<uint32_t>& output ) const {
  const unsigned int version=3; //GetBack - shall we change it to 4
  output.push_back(version);

  ::HLTNavDetails::FillSize fs(output);

  if ( serializeTEs(output) == false ) {    
    return false;
  }

  if ( serializeHolders(output) == false ) {
    return false;
  }

  return true;
}
  
bool HLT::StandaloneNavigation::deserialize( const std::vector<uint32_t>& input ) {
  //  using namespace std;
  //  std::cerr << "deserializing navigation of size: " << input.size() << std::endl;
  std::vector<uint32_t>::const_iterator inputIt = input.begin();
  const unsigned int version = *inputIt++;

  //  cerr << "version " << version << endl;

  const size_t totalSize = *inputIt++;   

  //remove unused warning;
  (void)(version);
  (void)(totalSize);
  
  if (!deserializeTEs(inputIt,input.size())) {    
    return false;
  }

  if (!deserializeHolders(inputIt,input.end())) {
    return false;
  }
  return true;
}
