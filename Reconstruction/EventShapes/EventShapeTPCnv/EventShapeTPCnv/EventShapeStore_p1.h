/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETPCNV_EVENTSHAPESTORE_P1_H
#define EVENTSHAPETPCNV_EVENTSHAPESTORE_P1_H

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
