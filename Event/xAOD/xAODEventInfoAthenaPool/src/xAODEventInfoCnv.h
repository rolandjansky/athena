// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCNV_H
#define XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCNV_H

#include "xAODEventInfo/EventInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<xAOD::EventInfo> xAODEventInfoCnv;


#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCNV_H
