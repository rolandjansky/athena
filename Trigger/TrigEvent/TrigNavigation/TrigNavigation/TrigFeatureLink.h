/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNavigation_TrigFeatureLink
#define TrigNavigation_TrigFeatureLink
#include <stdint.h>

#include "GaudiKernel/ClassID.h"


class TrigFeatureLink {

 public:
  TrigFeatureLink(CLID clid, uint16_t subtype, uint32_t index);  
  TrigFeatureLink();  
  bool isValid() const { return m_clid != 0; }
  
  CLID clid() const { return m_clid; }
  uint16_t subTypeIndex() const { return m_subTypeIndex; }
  uint32_t index() const { return m_index; }
  
 private:

  CLID m_clid;
  uint16_t m_subTypeIndex;
  uint32_t m_index;
};

//TrigFeatureLink getFeatureLink( const TriggerElement* te, const std::string& label, const T* object, const C* container)
//const T* getFeature(TrigFeatureLink*)


#endif
