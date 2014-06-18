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
class IProxyDictWithPool
  : public IProxyDict,
    public IStringPool
{
public:
  /// Gaudi interface definition.
  static const InterfaceID& interfaceID() { return IID_IProxyDictWithPool; }


  /**
   * @brief Test to see if the target of an ElementLink has moved.
   * @param sgkey_in Original hashed key of the EL.
   * @param index_in Original index of the EL.
   * @param sgkey_out[out] New hashed key for the EL.
   * @param index_out[out] New index for the EL.
   * @return True if there is a remapping; false otherwise.
   *
   * The default implementation here always returns false.
   */
  virtual bool tryELRemap (sgkey_t sgkey_in, size_t index_in,
                           sgkey_t& sgkey_out, size_t& index_out);
};


#endif // not SGTOOLS_IPROXYDICTWITHPOOL_H
