/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrODEvCollection_h
#define ALFA_LocRecCorrODEvCollection_h

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvent.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

class ALFA_LocRecCorrODEvCollection: public DataVector<ALFA_LocRecCorrODEvent> {
  
 public:
  
  ALFA_LocRecCorrODEvCollection() {}
};

typedef ALFA_LocRecCorrODEvCollection::iterator       ALFA_LocRecCorrODIterator;
typedef ALFA_LocRecCorrODEvCollection::const_iterator ALFA_LocRecCorrODConstIterator;

#ifndef __CINT__
CLASS_DEF( ALFA_LocRecCorrODEvCollection , 1338153820 , 1 )
#endif
  
#endif
