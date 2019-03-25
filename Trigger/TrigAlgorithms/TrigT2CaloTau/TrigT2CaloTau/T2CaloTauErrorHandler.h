/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOTAU_T2CALOTAUERRORHANDLER_H 
#define TRIGT2CALOTAU_T2CALOTAUERRORHANDLER_H

namespace TAUCLUSTERROR {

    /** enumerate tau-specific errors */
    enum TAUCLUSTERROR{
      FAILPRESEED=31,
      FAILSEED=30,
      HADS1E0=11,
      HADS2E0=10,
      HADS3E0=9,
      EMS0E0=15,
      EMS1E0=14,
      EMS2E0=13,
      EMS3E0=12
    };


}
#endif
