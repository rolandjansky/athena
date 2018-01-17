/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEHITCONTAINERCNV_H
#define EXAMPLEHITCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "ExampleHitContainerCnv_p1.h"


typedef T_AthenaPoolTPCnvCnv<ExampleHitContainer, ExampleHitContainerCnv_p1>
  ExampleHitContainerCnv;


#endif
