/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLocRecoEvCollection_h
#define AFP_TDLocRecoEvCollection_h

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AFP_LocRecoEv/AFP_TDLocRecoEvent.h"


class AFP_TDLocRecoEvCollection: public DataVector<AFP_TDLocRecoEvent> {
  
 public:
  
  AFP_TDLocRecoEvCollection() {}
};

typedef AFP_TDLocRecoEvCollection::iterator       AFP_TDLocRecoIterator;
typedef AFP_TDLocRecoEvCollection::const_iterator AFP_TDLocRecoConstIterator;

#ifndef __CINT__
CLASS_DEF( AFP_TDLocRecoEvCollection , 1214491929 , 1 )
#endif
  
#endif
