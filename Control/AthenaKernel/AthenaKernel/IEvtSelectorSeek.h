// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file  AthenaKernel/IEvtSelectorSeek.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2017
 * @brief Extension to IEvtSelector to allow for seeking.
 */

#ifndef ATHENAKERNEL_IEVTSELECTORSEEK_H
#define ATHENAKERNEL_IEVTSELECTORSEEK_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IEvtSelector.h"

/**
 * @class IEvtSelectorSeek
 * @brief Abstract interface for seeking for an event selector.
 */

class IEvtSelectorSeek : virtual public INamedInterface
{
public:
  DeclareInterfaceID(IEvtSelectorSeek, 1, 0);


  /**
   * @brief Seek to a given event number.
   * @param evtnum  The event number to which to seek.
   */
  virtual StatusCode seek (IEvtSelector::Context& c, int evtnum) const = 0;

  
  /**
   * @brief return the current event number.
   * @return The current event number.
   */
  virtual int curEvent (const IEvtSelector::Context& c) const = 0;


  /**
   * @brief Return the size of the collection, or -1 if we can't get the size.
   */
  virtual int size (IEvtSelector::Context& c) const = 0;
};


#endif // not ATHENAKERNEL_IEVTSELECTORSEEK_H

