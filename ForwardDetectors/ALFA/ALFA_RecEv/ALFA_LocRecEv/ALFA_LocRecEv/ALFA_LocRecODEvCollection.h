/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecODEvCollection_h
#define ALFA_LocRecODEvCollection_h

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ALFA_LocRecEv/ALFA_LocRecODEvent.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

class ALFA_LocRecODEvCollection: public DataVector<ALFA_LocRecODEvent> {
  
 public:
  
  ALFA_LocRecODEvCollection() {}
};

typedef ALFA_LocRecODEvCollection::iterator       ALFA_LocRecODIterator;
typedef ALFA_LocRecODEvCollection::const_iterator ALFA_LocRecODConstIterator;

#ifndef __CINT__
CLASS_DEF( ALFA_LocRecODEvCollection , 1122443126 , 1 )
#endif
  
#endif
