/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecODEvCollection_h
#define ALFA_GloRecODEvCollection_h

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "ALFA_GloRecEv/ALFA_GloRecODEvent.h"


class ALFA_GloRecODEvCollection: public DataVector<ALFA_GloRecODEvent> {
  
 public:
  
  ALFA_GloRecODEvCollection() {}
};

typedef ALFA_GloRecODEvCollection::iterator       ALFA_GloRecODIterator;
typedef ALFA_GloRecODEvCollection::const_iterator ALFA_GloRecODConstIterator;

#ifndef __CINT__
CLASS_DEF( ALFA_GloRecODEvCollection , 1172634496 , 1 )
#endif
  
#endif
