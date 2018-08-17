/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDDigiCollection_h
#define AFP_TDDigiCollection_h

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AFP_DigiEv/AFP_TDDigi.h"
#include "HitManagement/AtlasHitsVector.h"

typedef AtlasHitsVector<AFP_TDDigi> AFP_TDDigiCollection;
typedef AtlasHitsVector<AFP_TDDigi>::iterator AFP_TDDigiIter;
typedef AtlasHitsVector<AFP_TDDigi>::const_iterator AFP_TDDigiConstIter;

#ifndef __CINT__
 CLASS_DEF(AFP_TDDigiCollection, 1154879695, 1)
#endif
  
#endif

