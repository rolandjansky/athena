// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/IConditionsCleanerSvc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief Interface for doing garbage collection of conditions objects.
 */


#ifndef ATHENAKERNEL_ICONDITIONSCLEANERSVC_H
#define ATHENAKERNEL_ICONDITIONSCLEANERSVC_H


#include "GaudiKernel/IInterface.h"


class EventContext;
class CondContBase;


namespace Athena {


/**
 * @brief Interface for doing garbage collection of conditions objects.
 */
class IConditionsCleanerSvc
  : virtual public IInterface
{
public:
  DeclareInterfaceID (IConditionsCleanerSvc, 1, 0);


  /**
   * @brief Called at the start of each event.
   * @param ctx The current event context.
   * @param allowAsync If true, then cleaning may be run
   *                   in an asynchronous TBB task.
   */
  virtual StatusCode event (const EventContext& ctx,
                            bool allowAsync) = 0;


  /**
   * @brief Called after a conditions object has been added.
   * @param ctx The current event context.
   * @param cc The container to which the object was added.
   */
  virtual StatusCode condObjAdded (const EventContext& ctx,
                                   CondContBase& cc) = 0;
};


} // namespace Athena


#endif // not ATHENAKERNEL_ICONDITIONSCLEANERSVC_H
