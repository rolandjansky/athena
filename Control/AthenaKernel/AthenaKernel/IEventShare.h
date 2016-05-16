// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
/**
 * @file  IEventShare.h
 * @author peter van gemmeren <gemmeren@anl.gov>
 * @date August, 2011
 */

#ifndef ATHENAKERNEL_IEVENTSHARE_H
#define ATHENAKERNEL_IEVENTSHARE_H 1

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/INamedInterface.h"

/**
 * @class IEventShare
 * @brief Abstract interface for sharing within an event stream.
 */

static const InterfaceID IID_IEventShare("IEventShare", 1 , 0);

class IEventShare : virtual public INamedInterface
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~IEventShare ();

  static const InterfaceID& interfaceID() { return IID_IEventShare; }

  /**
   * @brief Make this a server.
   * @param num  The number for the server.
   */
   virtual StatusCode makeServer(int num) = 0;

  /**
   * @brief Make this a client.
   * @param num  The number for the client.
   */
   virtual StatusCode makeClient(int num) = 0;

  /**
   * @brief Request to share a given event.
   * @param evtnum  The event sequence number to share.
   */
  virtual StatusCode share (int evtnum) = 0;

  /**
   * @brief Read the next maxevt events.
   * @param maxevt  The number of events to read.
   */
  virtual StatusCode readEvent(int maxevt) = 0;
};

#endif // not ATHENAKERNEL_IEVENTSHARE_H
