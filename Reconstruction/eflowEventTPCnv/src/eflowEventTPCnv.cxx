/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "eflowEventTPCnv/eflowObject_p1.h"
#include "eflowEventTPCnv/eflowObjectCnv_p1.h"
#include "eflowEventTPCnv/eflowObjectContainer_p1.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p1.h"

#include "eflowEventTPCnv/eflowObject_p2.h"
#include "eflowEventTPCnv/eflowObjectCnv_p2.h"
#include "eflowEventTPCnv/eflowObjectContainer_p2.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p2.h"

#include "eflowEventTPCnv/eflowObject_p3.h"
#include "eflowEventTPCnv/eflowObjectCnv_p3.h"
#include "eflowEventTPCnv/eflowObjectContainer_p3.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p3.h"

#include "eflowEventTPCnv/eflowObject_p4.h"
#include "eflowEventTPCnv/eflowObjectCnv_p4.h"
#include "eflowEventTPCnv/eflowObjectContainer_p4.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p4.h"

#include "eflowEventTPCnv/eflowObject_p5.h"
#include "eflowEventTPCnv/eflowObjectCnv_p5.h"
#include "eflowEventTPCnv/eflowObjectContainer_p5.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p5.h"

DECLARE_TPCNV_FACTORY(eflowObjectContainerCnv_p1,
                      eflowObjectContainer,
                      eflowObjectContainer_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(eflowObjectContainerCnv_p2,
                      eflowObjectContainer,
                      eflowObjectContainer_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(eflowObjectContainerCnv_p3,
			eflowObjectContainer,
			eflowObjectContainer_p3,
			Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(eflowObjectContainerCnv_p4,
			eflowObjectContainer,
			eflowObjectContainer_p4,
			Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(eflowObjectContainerCnv_p5,
			eflowObjectContainer,
			eflowObjectContainer_p5,
			Athena::TPCnvVers::Current)
