// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file IOVSvc/src/ICondSvcSetupDone.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2020
 * @brief Interface to tell CondSvc to cache conditions containers.
 */


#ifndef IOVSVC_ICONDSVCSETUPDONE_H
#define IOVSVC_ICONDSVCSETUPDONE_H


#include "GaudiKernel/IInterface.h"


class ICondSvcSetupDone
  : virtual public IInterface
{
public:
  DeclareInterfaceID( ICondSvcSetupDone, 1, 0 );

  /// To be called after creating conditions containers
  /// in the conditions store.
  virtual StatusCode setupDone() = 0;
};


#endif // not IOVSVC_ICONDSVCSETUPDONE_H
