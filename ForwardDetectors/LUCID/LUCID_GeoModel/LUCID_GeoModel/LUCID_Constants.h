/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LucidConstants_h
#define LucidConstants_h 1

#define GASVES       1
#define GASTUB       2
#define QUARTZ       3

#define STEP_ALL     0
#define STEP_ALL_QEF 1
#define STEP_GAS     2
#define STEP_GAS_QEF 3
#define STEP_PMT     4
#define STEP_PMT_QEF 5
#define nSteps       6

#define nLayers           2
#define nPmtTubesPerLayer 8
#define nFiberTubes       4
#define nTubes            2*(nLayers*nPmtTubesPerLayer+nFiberTubes)

#define foranytub    for (int tub=0; tub<nTubes; tub++) 
#define foranyste    for (int ste=0; ste<nSteps; ste++) 

#endif
