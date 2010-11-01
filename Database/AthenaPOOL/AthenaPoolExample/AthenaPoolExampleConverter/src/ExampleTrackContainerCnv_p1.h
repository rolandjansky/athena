/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLETRACKCONTAINERCNV_P1_H
#define EXAMPLETRACKCONTAINERCNV_P1_H

/**
 *  @file ExampleTrackContainerCnv_p1.h
 *  @brief This file contains the class definition for the ExampleTrackContainerCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleTrackContainerCnv_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#define private public
#define protected public
#include "AthenaPoolExampleData/ExampleTrackContainer.h"
#include "AthenaPoolExampleConverter/ExampleTrackContainer_p1.h"
#undef private
#undef protected
#include "ExampleTrackCnv_p1.h"

typedef T_AthenaPoolTPCnvVector<ExampleTrackContainer, ExampleTrackContainer_p1, ExampleTrackCnv_p1> ExampleTrackContainerCnv_p1;

template<>
class T_TPCnv<ExampleTrackContainer, ExampleTrackContainer_p1> : public ExampleTrackContainerCnv_p1 {
public:
};

#endif
