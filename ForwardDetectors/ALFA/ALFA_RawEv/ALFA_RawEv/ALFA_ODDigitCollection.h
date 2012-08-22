/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODDigitCollection_h
#define ALFA_ODDigitCollection_h

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ALFA_RawEv/ALFA_ODDigit.h"

#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif


class ALFA_ODDigitCollection: public DataVector<ALFA_ODDigit> {
  
 public:
  
  ALFA_ODDigitCollection() {}
};


typedef ALFA_ODDigitCollection::iterator       ALFA_ODDigitIterator;
typedef ALFA_ODDigitCollection::const_iterator ALFA_ODDigitConstIterator;

// CLID
#ifndef __CINT__
CLASS_DEF( ALFA_ODDigitCollection , 1269800463 , 1 )
#endif
  
#endif
