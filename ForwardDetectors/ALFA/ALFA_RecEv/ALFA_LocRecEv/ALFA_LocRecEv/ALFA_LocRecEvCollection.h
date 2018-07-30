/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecEvCollection_h
#define ALFA_LocRecEvCollection_h

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"


class ALFA_LocRecEvCollection: public DataVector<ALFA_LocRecEvent> {
  
 public:
  
  ALFA_LocRecEvCollection() {}
};

typedef ALFA_LocRecEvCollection::iterator       ALFA_LocRecIterator;
typedef ALFA_LocRecEvCollection::const_iterator ALFA_LocRecConstIterator;

#ifndef __CINT__
CLASS_DEF( ALFA_LocRecEvCollection , 1314854717 , 1 )
#endif
  
#endif
