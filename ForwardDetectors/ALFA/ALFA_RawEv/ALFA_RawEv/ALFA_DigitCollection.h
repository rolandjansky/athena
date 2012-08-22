/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DigitCollection_h
#define ALFA_DigitCollection_h

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ALFA_RawEv/ALFA_Digit.h"

#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif


class ALFA_DigitCollection: public DataVector<ALFA_Digit> {
  
 public:
  
  ALFA_DigitCollection() {}
};


//typedef AthenaHitsVector<ALFA_Digit> ALFA_DigitCollection;

//typedef AthenaHitsVector<ALFA_Digit>::iterator ALFA_DigitIterator;
//typedef AthenaHitsVector<ALFA_Digit>::const_iterator ALFA_DigitConstIterator;


typedef ALFA_DigitCollection::iterator       ALFA_DigitIterator;
typedef ALFA_DigitCollection::const_iterator ALFA_DigitConstIterator;


#ifndef __CINT__
CLASS_DEF(ALFA_DigitCollection, 1307251830, 1)
#endif
  
#endif
