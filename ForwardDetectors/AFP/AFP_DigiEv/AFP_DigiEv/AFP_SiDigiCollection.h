/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SiDigiCollection_h
#define AFP_SiDigiCollection_h

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "AFP_DigiEv/AFP_SiDigi.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AtlasHitsVector<AFP_SiDigi> AFP_SiDigiCollection;
typedef AtlasHitsVector<AFP_SiDigi>::iterator AFP_SiDigiIter;
typedef AtlasHitsVector<AFP_SiDigi>::const_iterator AFP_SiDigiConstIter;

#ifndef __CINT__
 CLASS_DEF(AFP_SiDigiCollection, 1311274787, 1)
#endif
  
#endif
