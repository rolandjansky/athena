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


// Now everything's in IProxyDict.
#include "AthenaKernel/IProxyDict.h"
typedef IProxyDict IProxyDictWithPool;


#endif // not SGTOOLS_IPROXYDICTWITHPOOL_H
