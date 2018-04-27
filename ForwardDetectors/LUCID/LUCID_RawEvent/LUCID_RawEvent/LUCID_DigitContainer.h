/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCONTAINER_H
#define LUCID_DIGITCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LUCID_RawEvent/LUCID_Digit.h"


class LUCID_DigitContainer: public DataVector<LUCID_Digit> {
  
 public:
  
  LUCID_DigitContainer() {}
};

typedef LUCID_DigitContainer::iterator       LUCID_DigitIterator;
typedef LUCID_DigitContainer::const_iterator LUCID_DigitConstIterator;

#ifndef __CINT__
CLASS_DEF(LUCID_DigitContainer, 17191, 1)
#endif
  
#endif
  
