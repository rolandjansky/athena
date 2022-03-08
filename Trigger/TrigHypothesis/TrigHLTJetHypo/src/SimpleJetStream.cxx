/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./SimpleJetStream.h"

std::ostream& operator << (std::ostream& os ,
			   const SimpleJetStream& js) {

  os << "SimpleJetStream id " << js.m_id
     << " m_valid "  << std::boolalpha << js.m_valid
     << " no of jets: " << js.m_jets.size()
     << " m_ind "  << js.m_ind;
  return os;
}

std::stringstream& operator << (std::stringstream& os ,
				const SimpleJetStream& js) {

  os << "SimpleJetStream id " << js.m_id
     << " m_valid "  << std::boolalpha << js.m_valid
     << " no of jets: " << js.m_jets.size()
     << " m_ind "  << js.m_ind;
  return os;
}

