/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPCOMMONHDR_H
#define FORWARDDETECTORSD3PDMAKER_AFPCOMMONHDR_H

// STL include(s):
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// EDM include(s):
#include "GeneratorObjects/McEventCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

#define EVCOLLNAME_SIDDIGI "AFP_SiDigiCollection"
#define EVCOLLNAME_TDDIGI "AFP_TDDigiCollection"
#define EVCOLLNAME_SIDSIMHIT "AFP_SIDSimHitCollection"

#define UNDEFFLOATVALUE (-9999.0)
#define UNDEFINTVALUE (-1)

#define AFP_STATIONSCNT 4
#define AFP_SID_LAYERSCNT 6

#define MAXPILEUP 500

namespace D3PD
{
    enum eSIDCountingLevel { ESCL_STATION, ESCL_LAYER, ESCL_PIXEL };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPCOMMONHDR_H
