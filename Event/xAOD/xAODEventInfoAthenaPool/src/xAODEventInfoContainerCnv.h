// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H


#include "xAODEventInfo/EventInfoContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<xAOD::EventInfoContainer> xAODEventInfoContainerCnv;


#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H
