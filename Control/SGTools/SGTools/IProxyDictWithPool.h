// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IProxyDictWithPool.h,v 1.2 2008-04-18 03:18:24 ssnyder Exp $
/**
 * @file SGTools/IProxyDictWithPool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2007
 * @brief Combine IProxyDict and IStringPool.
 *
 * Make a class that combines @c IProxyDict and @c IStringPool,
 * so we can use both through a single pointer without having
 * to dynamic_cast between them.
 */


#ifndef SGTOOLS_IPROXYDICTWITHPOOL_H
#define SGTOOLS_IPROXYDICTWITHPOOL_H

#include "AthenaKernel/IProxyDict.h"
#include "SGTools/IStringPool.h"


/// Interface definition.
static const InterfaceID IID_IProxyDictWithPool ("IProxyDictWithPool", 1, 0);


/**
 * @brief Combine IProxyDict and IStringPool.
 */
/// TEMPORARY: Everying's moved to IProxyDict.  Get rid of this eventually.
class IProxyDictWithPool
  : public IProxyDict
{
public:
  /// Gaudi interface definition.
  static const InterfaceID& interfaceID() { return IID_IProxyDictWithPool; }
};


#endif // not SGTOOLS_IPROXYDICTWITHPOOL_H
