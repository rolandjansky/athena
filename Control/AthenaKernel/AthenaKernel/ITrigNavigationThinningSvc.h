// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/ITrigNavigationThinningSvc.h
 * @author scott snyder <snyder@bnl.gov>
 *         Based on the original version from  TrigNavTools (T. Bold).
 * @date Apr, 2020
 * @brief Define interface for doing TrigNavigation thinning.
 */


#ifndef ATHENAKERNEL_ITRIGNAVIGATIONTHINNINGSVC_H
#define ATHENAKERNEL_ITRIGNAVIGATIONTHINNINGSVC_H


#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>


class EventContext;


/**
 * @brief Define interface for doing TrigNavigation thinning.
 */
class ITrigNavigationThinningSvc
  : virtual public IInterface
{
public:
  DeclareInterfaceID (ITrigNavigationThinningSvc, 1, 0);


  /**
   * @brief Return slimmed data for TrigNavigation.
   */
  virtual
  StatusCode doSlimming (const EventContext& ctx,
                         std::vector<uint32_t>& slimmed_and_serialized) const = 0;
};


#endif // not ATHENAKERNEL_ITRIGNAVIGATIONTHINNINGSVC_H
