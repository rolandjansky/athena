/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigL1CaloUpgrade/BCID.h"


int bcids_from_start(const int& bcid) {
    int normalized_bcid=0;
    for ( int i=1; i< 5; i++)
       if ( (bcid > first_bcid[i-1]) && (bcid < first_bcid[i]) )
          normalized_bcid=bcid-first_bcid[i-1];
    return normalized_bcid;
}

