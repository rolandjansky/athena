/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigNavStructure/TriggerElementFactory.h"


HLT::TriggerElementFactory::TriggerElementFactory() {
  //  std::cout << "TriggerElementFactory::TriggerElementFactory() " << std::endl;
}

HLT::TriggerElementFactory::~TriggerElementFactory() {
}

HLT::TriggerElement* HLT::TriggerElementFactory::produce(unsigned int id, bool ghost, bool nofwd) {
  HLT::TriggerElement *te = new HLT::TriggerElement(id, ghost, nofwd); 
  m_list.push_back(te); 
  m_groupedById[id].push_back(te);
  return te;
}


bool HLT::TriggerElementFactory::empty() const {
  return m_list.empty();
}

void HLT::TriggerElementFactory::reset() {
  std::vector< HLT::TriggerElement* >::iterator i;
  for ( i = m_list.begin(); i != m_list.end(); ++i ) {
    delete (*i);
  }
  m_list.clear();
  m_groupedById.clear();
}
