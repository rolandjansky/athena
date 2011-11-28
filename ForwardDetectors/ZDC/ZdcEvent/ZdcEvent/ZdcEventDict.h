/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCEVENTDICT_H
#define ZDCEVENTDICT_H

#include "ZdcEvent/ZdcRdo.h"
#include "ZdcEvent/ZdcRdoCollection.h"

#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
//#include "ZdcEvent/ZdcDigitsContainer.h"

#include "ZdcEvent/ZdcCalibEvent.h"
#include "ZdcEvent/ZdcRawChannel.h"
#include "ZdcEvent/ZdcRawChannelCollection.h"

#include "DataModel/DataVector.h"

/*
void dummyZdcForTVCollTypedef(DataVector<ZdcRdo> a,
			      DataVector<ZdcRdoCollection> b) {
  DataVector<ZdcRdo> aa = a;
  DataVector<ZdcRdoCollection> bb = b;
}
*/

/*
void dummyZdcForTVCollTypedef(ZdcDigitsCollection a)
{
  ZdcDigitsCollection aa = a;
}
*/

#endif
