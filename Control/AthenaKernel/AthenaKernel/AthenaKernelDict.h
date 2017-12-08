// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ATHENAKERNELDICT_H
#define ATHENAKERNEL_ATHENAKERNELDICT_H 1

#include <vector>
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaKernel/ICollectionSize.h"
#include "AthenaKernel/IAthenaSummarySvc.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaKernel/ILockable.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "AthenaKernel/BaseInfo.h"

#include "AthenaKernel/DsoDb.h"
#include "AthenaKernel/AthDsoUtils.h"
#include "AthenaKernel/tools/AthenaPackageInfo.h"

#include "GaudiKernel/MsgStream.h"


namespace AthenaKernelDict {
  struct tmp {
    Ath::DsoDb::DsoMap_t m_0;
    Ath::DsoDb::DsoMap_t::iterator m_1;
    Ath::DsoDb::DsoMap_t::const_iterator m_2;
  };
} //> namespace AthenaKernelDict

#endif // !ATHENAKERNEL_ATHENAKERNELDICT_H
