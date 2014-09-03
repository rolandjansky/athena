// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICollectionSize.h,v 1.1 2005-05-05 00:15:02 calaf Exp $
/**
 * @file  ICollectionSize.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2005
 * @brief Abstract interface for finding the size of an event collection.
 */

#ifndef ATHENAKERNEL_ICOLLECTIONSIZE_H
#define ATHENAKERNEL_ICOLLECTIONSIZE_H


#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/INamedInterface.h"


static const InterfaceID IID_ICollectionSize("ICollectionSize", 1 , 0);


/**
 * @class ICollectionSize
 * @brief Abstract interface for finding the size of an event collection.
 */
class ICollectionSize
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~ICollectionSize () {};


  static const InterfaceID& interfaceID() { return IID_ICollectionSize; }


  /**
   * @brief Return the size of the collection.
   */
  virtual int size () = 0;
};


#endif // not ATHENAKERNEL_ICOLLECTIONSIZE_H
