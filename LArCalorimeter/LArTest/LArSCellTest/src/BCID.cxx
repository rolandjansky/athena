/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BCID.h"


int bcids_from_start(const int& bcid) {
    int normalized_bcid=-1;
    for ( int i=1; i< 53; i++)
       if ( (bcid > first_bcid[i-1]) && (bcid < first_bcid[i]) )
          normalized_bcid=bcid-first_bcid[i-1];
    return normalized_bcid;
}

