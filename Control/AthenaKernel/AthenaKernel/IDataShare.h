// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
/**
 * @file  IDataShare.h
 * @author peter van gemmeren <gemmeren@anl.gov>
 * @date March, 2015
 */

#ifndef ATHENAKERNEL_IDATASHARE_H
#define ATHENAKERNEL_IDATASHARE_H 1

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/INamedInterface.h"

/**
 * @class IDataShare
 * @brief Abstract interface for sharing data
 */

static const InterfaceID IID_IDataShare("IDataShare", 1 , 0);

class IDataShare : virtual public INamedInterface
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~IDataShare ();

  static const InterfaceID& interfaceID() { return IID_IDataShare; }

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
   * @brief Read the data
   */
  virtual StatusCode readData() = 0;
};


#endif // not ATHENAKERNEL_IDATASHARE_H
