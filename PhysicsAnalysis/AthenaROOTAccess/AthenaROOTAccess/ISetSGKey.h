// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ISetSGKey.h,v 1.2 2008-06-04 14:52:59 ssnyder Exp $

/**
 * @file AthenaROOTAccess/ISetSGKey.h
 * @author scott snyder
 * @date May 2008
 * @brief Provide interface for changing the hashed SG key/clid of a branch.
 */

#ifndef ATHENAROOTACCESS_ISetSGKey_H
#define ATHENAROOTACCESS_ISetSGKey_H


#include "AthenaKernel/IStringPool.h"


namespace AthenaROOTAccess {


/**
 * @brief Provide interface for changing the hashed SG key/clid of a branch.
 *
 * When a branch moves to a new entry, we need to clear any data cached
 * in the @c DataBucket.  This is done by calling @c ProxyMap::resetBranch
 * with the SG key.  For branches in CollectionTree, this is easy: each
 * branch remembers its sgkey (set when the branch is constructed), and then
 * calls this method from its @c GetEntry.
 *
 * For external @c POOLContainer_ trees, though, things are more complicated.
 * Here, a single branch may correspond to a set of multiple sgkeys.
 * Further, we can't find the complete set by looking at the first event;
 * there may be additional keys used later in the file.
 *
 * For now, these branches can be accessed only by dereferencing a DL/EL.
 * Thus, for this case, what we'll do is have @c ProxyMap, when it seeks
 * in a target branch, also tell the branch its sgkey.  The branch can
 * then use this for the next reset.  (@c ProxyMap knows whether the
 * target is in an external tree, so we can restrict this to that case.)
 * We need to break this out into a separate interface to avoid dependency
 * issues.
 *
 * The @c sawErr method is also stuck here to avoid dependency issues.
 */
class ISetSGKey
{
public:
  /// Empty destructor.
  virtual ~ISetSGKey() {}


  /**
   * @brief Change the hashed SG key/clid for a branch.
   * @param sgkey The new hashed key/clid.
   */
  virtual void setSGKey (IStringPool::sgkey_t sgkey) = 0;


  /**
   * @brief Return true if the last conversion had a recoverable error.
   */
  virtual bool sawErr() const = 0;
};


} // AthenaROOTAccess


#endif // not ATHENAROOTACCESS_ISetSGKey_H
