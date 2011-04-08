/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETPCNV_EVENTSHAPESTORE_P2_H
#define EVENTSHAPETPCNV_EVENTSHAPESTORE_P2_H

#include "CLIDSvc/CLASS_DEF.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <vector>

class EventShapeStore_p2
{
  template <class T>
    friend class EventShapeStoreConverterBase;
  
public:
  
  EventShapeStore_p2() { };
  virtual ~EventShapeStore_p2() { };
  
 protected:
  
  std::vector<unsigned int>        m_mtypV;
  std::vector<std::vector<float> > m_mvalM;
};

#endif
