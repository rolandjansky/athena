/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEHITCONTAINERCNV_P1_H
#define EXAMPLEHITCONTAINERCNV_P1_H

/**
 *  @file ExampleHitContainerCnv_p1.h
 *  @brief This file contains the class definition for the ExampleHitContainerCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHitContainerCnv_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#define private public
#define protected public
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "AthenaPoolExampleConverter/ExampleHitContainer_p1.h"
#undef private
#undef protected
#include "ExampleHitCnv_p1.h"

typedef T_AthenaPoolTPCnvVector<ExampleHitContainer, ExampleHitContainer_p1, ExampleHitCnv_p1> ExampleHitContainerCnv_p1;

template<>
class T_TPCnv<ExampleHitContainer, ExampleHitContainer_p1> : public ExampleHitContainerCnv_p1 {
public:
};

#endif
