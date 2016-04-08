/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLocRecoEvCollection_h
#define AFP_SIDLocRecoEvCollection_h

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "AFP_LocRecoEv/AFP_SIDLocRecoEvent.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

class AFP_SIDLocRecoEvCollection: public DataVector<AFP_SIDLocRecoEvent> {
  
 public:
  
  AFP_SIDLocRecoEvCollection() {}
};

typedef AFP_SIDLocRecoEvCollection::iterator       AFP_SIDLocRecoIterator;
typedef AFP_SIDLocRecoEvCollection::const_iterator AFP_SIDLocRecoConstIterator;

#ifndef __CINT__
CLASS_DEF( AFP_SIDLocRecoEvCollection , 1318318366 , 1 )
#endif
  
#endif
