/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_DELETEOBJECT_H
#define DCMATH_DELETEOBJECT_H

/**
@class DeleteObject
An operator to delete the object pointed by a pointer to a template class. 
*/

namespace TrkDriftCircleMath {
  struct DeleteObject {
    template<class T>
    void operator()(const T* p){
      delete p;
    }
  };
}
#endif
