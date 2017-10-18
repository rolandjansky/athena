/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWEVDICT_H
#define AFP_RAWEVDICT_H

#include "AFP_RawEv/AFP_SiRawData.h"
#include "AFP_RawEv/AFP_SiRawCollection.h"

#include "AFP_RawEv/AFP_ToFRawData.h"
#include "AFP_RawEv/AFP_ToFRawCollection.h"

#include "AFP_RawEv/AFP_RawContainer.h"

namespace AFP_RawEvDict
{
  AFP_SiRawData siRawData;
  AFP_SiRawCollection siRawCollection;

  AFP_ToFRawData ToFRawData;
  AFP_ToFRawCollection ToFRawCollection;

  AFP_RawContainer RawContainer;
}

#endif


