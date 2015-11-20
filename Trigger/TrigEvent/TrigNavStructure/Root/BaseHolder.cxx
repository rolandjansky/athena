/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavStructure/StringSerializer.h"
#include "TrigNavStructure/BaseHolder.h"
#include <iostream>
using namespace HLT;

BaseHolder::~BaseHolder() {}

bool BaseHolder::enquireSerialized(std::vector<uint32_t>::const_iterator& fromHere,
				   const std::vector<uint32_t>::const_iterator& end,
				   class_id_type& c, std::string& label,
				   sub_index_type& subtypeIndex ) {
  using namespace std;
  if ( fromHere == end )  return false;  

  c = *fromHere++;
  if ( fromHere == end )  return false;

  subtypeIndex = *fromHere++;
  if ( fromHere == end ) return false;
  
  unsigned labelSize = *fromHere++;
  if ( fromHere == end ) return false;
  if ( fromHere+labelSize > end ) return false;
  
  std::vector<uint32_t>::const_iterator stringEnd = fromHere+labelSize;
  HLT::StringSerializer::deserialize(fromHere, stringEnd, label);

  //advance iterator to end of label
  std::advance(fromHere,labelSize);

  return true;
}

bool BaseHolder::serialize(std::vector<uint32_t>& output) const {  
  output.push_back( typeClid() );
  output.push_back( subTypeIndex() );

  const size_t labelSizeIndex = output.size();
  output.push_back( 0 );
  const size_t beforeSS = output.size(); 
  StringSerializer::serialize(label(), output);
  output[labelSizeIndex] = output.size() - beforeSS;  
  return true;
}
