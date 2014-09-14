/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavStructure/StringSerializer.h"
#include "TrigNavStructure/BaseHolder.h"
#include <iostream>
using namespace HLT;

BaseHolder::~BaseHolder() {}

bool BaseHolder::enquireSerialized(const std::vector<uint32_t>& blob,
				   std::vector<uint32_t>::const_iterator& it,
				   class_id_type& c, std::string& label,
				   sub_index_type& subtypeIndex ) {
  using namespace std;
  if ( it == blob.end() )  return false;  

  c = *it++;
  if ( it == blob.end() )  return false;

  subtypeIndex = *it++;
  if ( it == blob.end() ) return false;
  
  unsigned labelSize = *it++;
  if ( it == blob.end() ) return false;
  if ( it+labelSize > blob.end() ) return false;
  
  std::vector<uint32_t>::const_iterator stringEnd = it+labelSize;
  HLT::StringSerializer::deserialize(it, stringEnd, label);

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
