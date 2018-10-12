/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrEvCollection_h
#define ALFA_LocRecCorrEvCollection_h

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"


class ALFA_LocRecCorrEvCollection: public DataVector<ALFA_LocRecCorrEvent> {
  
 public:
  
  ALFA_LocRecCorrEvCollection() {}
};

typedef ALFA_LocRecCorrEvCollection::iterator       ALFA_LocRecCorrIterator;
typedef ALFA_LocRecCorrEvCollection::const_iterator ALFA_LocRecCorrConstIterator;

#ifndef __CINT__
CLASS_DEF( ALFA_LocRecCorrEvCollection , 1332258635 , 1 )
#endif
  
#endif
