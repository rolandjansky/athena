/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecEvCollection_h
#define ALFA_GloRecEvCollection_h

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "ALFA_GloRecEv/ALFA_GloRecEvent.h"


class ALFA_GloRecEvCollection: public DataVector<ALFA_GloRecEvent> {
  
 public:
  
  ALFA_GloRecEvCollection() {}
};

typedef ALFA_GloRecEvCollection::iterator       ALFA_GloRecIterator;
typedef ALFA_GloRecEvCollection::const_iterator ALFA_GloRecConstIterator;

#ifndef __CINT__
CLASS_DEF( ALFA_GloRecEvCollection , 1278415727 , 1 )
#endif
  
#endif
