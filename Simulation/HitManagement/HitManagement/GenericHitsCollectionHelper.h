/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericHitsCollectionHelper_H
#define GenericHitsCollectionHelper_H

#include <string>
// Generic interface to a hit collection. it should be used to
// remove any dependency between sensitive detectors and the 
// underlying storing system..

class GenericHitsCollectionHelper {
public:
  GenericHitsCollectionHelper()
  virtual ~GenericHitsCollectionHelper()
  // following can't be virtual
  template <typename collectionType> 
  void ExportCollection(collectionType* collection)
  {      
  }
private:
};

#endif
