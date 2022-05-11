/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "CombinationsJetStream.h"

std::ostream& operator << (std::ostream& os, const CombinationsJetStream& str){
  os << "CombinationsJetStream m_combgen " << str.m_id << '\n' 
     << *str.m_combgen << '\n';
  return os;
}

std::stringstream& operator << (std::stringstream& os, const CombinationsJetStream& str){
  os << "CombinationsJetStream id: " << str.m_id
     << " m_combgen: " << *str.m_combgen
     <<" data: ";
  for (const auto& d : str.m_data){os << d << " ";}
  os <<'\n';
  
  return os;
}
