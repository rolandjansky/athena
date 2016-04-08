/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_DELETEOBJECT_H
#define MUONCALIB_DELETEOBJECT_H

namespace MuonCalib {
/**
@class DeleteObject
An operator to delete the object pointed by a pointer to a template class. 
*/

  struct DeleteObject {
    template<class T>
    void operator()(const T* p){
      delete p;
    }
  };
}

#endif
