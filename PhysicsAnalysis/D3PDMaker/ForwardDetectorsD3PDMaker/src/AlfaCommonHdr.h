/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_ALFACOMMONHDR_H
#define FORWARDDETECTORSD3PDMAKER_ALFACOMMONHDR_H

// STL include(s):
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// EDM include(s):
#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"
#include "GeneratorObjects/McEventCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

#define RPOTSCNT 8
#define MDLAYERSCNT	2
#define MDPLATESCNT	10
#define MDFIBERSCNT	64
#define ODLAYERSCNT	2
#define ODPLATESCNT	3
#define ODFIBERSCNT	15

#define TRIGPATCNT 16
#define BLMCNT 6
#define HVCHANNELCNT 216
#define RADMONCNT 4
#define FECNFTHRESHLOLDCNT 3
#define FECNFGAINCNT 5
#define TRIGSETCNT 6
#define TRIGSETLATENCYCNT 3

#define MAXNUMTRACKS 100
#define MAXNUMGLOBTRACKS 100
#define MAXNUMGENPARTICLES 100
#define MAXPILEUP 500

#define UNDEFFLOATVALUE (-9999.0)
#define UNDEFINTVALUE (-1)

namespace D3PD
{

   // Convenience type definition:
   typedef D3PD::BlockFillerTool<ALFA_CLinkEvent> AlfaDataEventFillerBase;
   typedef D3PD::BlockFillerTool<McEventCollection> AlfaTruthInfoFillerBase;
}

#endif // FORWARDDETECTORSD3PDMAKER_ALFACOMMONHDR_H
