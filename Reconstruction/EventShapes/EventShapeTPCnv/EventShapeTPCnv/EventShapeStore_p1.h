/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETPCNV_EVENTSHAPESTORE_P1_H
#define EVENTSHAPETPCNV_EVENTSHAPESTORE_P1_H

#include "CLIDSvc/CLASS_DEF.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>

class EventShapeStore_p1
{
  template <class T>
    friend class EventShapeStoreConverterBase;
  
public:
  
  EventShapeStore_p1() { };
  virtual ~EventShapeStore_p1() { };
  
 protected:
  
  std::vector<unsigned int>  m_mtypV;
  std::vector<float>         m_mvalV;
  
};

#endif
