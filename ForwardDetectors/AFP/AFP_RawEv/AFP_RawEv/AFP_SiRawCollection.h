/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIRAWCOLLECTION_H
#define AFP_SIRAWCOLLECTION_H

#include "AFP_RawEv/AFP_SiRawData.h"
#include "AFP_RawEv/AFP_RawCollection.h"

#include "CLIDSvc/CLASS_DEF.h"

/// Class representing collection of silicon detector data
typedef AFP_RawCollection<AFP_SiRawData> AFP_SiRawCollection;

CLASS_DEF( AFP_SiRawCollection, 1319088211, 1 ) 

#endif
