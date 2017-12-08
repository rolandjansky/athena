/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLETRACKCONTAINERCNV_H
#define EXAMPLETRACKCONTAINERCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "ExampleTrackContainerCnv_p1.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"


typedef T_AthenaPoolTPCnvCnv<ExampleTrackContainer, ExampleTrackContainerCnv_p1>
  ExampleTrackContainerCnv;


#endif
