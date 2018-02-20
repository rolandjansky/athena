/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TOFRAWCOLLECTION_H
#define AFP_TOFRAWCOLLECTION_H

#include "AFP_RawEv/AFP_ToFRawData.h"
#include "AFP_RawEv/AFP_RawCollection.h"

#include "CLIDSvc/CLASS_DEF.h"

/// Class representing collection of silicon detector data
typedef AFP_RawCollection<AFP_ToFRawData> AFP_ToFRawCollection;

CLASS_DEF(AFP_ToFRawCollection, 1242654793, 1 ) 

#endif
