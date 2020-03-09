// $Id$
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/src/WriteDecorHandleKey.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        WriteDecorHandle is made.
 */


#include "StoreGate/WriteDecorHandleKey.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IDataHandleHolder.h"
#include "GaudiKernel/Algorithm.h"


namespace SG {
namespace detail {


/**
 * @brief Test to see if HOLDER has registered a handle key
 *        matching CONTHANDLEKEY, as either input or output.
 * @param contHandleKey The handle key to test.
 * @param holder The component to search.
 */
bool handleInHolder (const DataObjID& contHandleKey,
                     const IDataHandleHolder& holder)
{
  for (const Gaudi::DataHandle* h : holder.inputHandles()) {
    if (h->fullKey() == contHandleKey) {
      return true;
    }
  }

  for (const Gaudi::DataHandle* h : holder.outputHandles()) {
    if (h->fullKey() == contHandleKey) {
      return true;
    }
  }

  return false;
}


/**
 * @brief Find the top-level Gaudi component for H.
 * @param H The component at which to start the search.
 *
 * We're given a Gaudi component H as an @c IDataHandleHolder.
 * If this is an algorithm, return it.
 * Otherwise, search up the list of parents to find either the owning
 * algorithm or a top-level public tool or service.
 */
const IDataHandleHolder* findParent (const IDataHandleHolder* h)
{
  while (true) {
    const IAlgTool* astool = dynamic_cast<const IAlgTool*> (h);
    if (!astool) return h;
    const IInterface* parent = astool->parent();
    if (!parent) return h;
    auto pdh = dynamic_cast<const IDataHandleHolder*> (parent);
    if (!pdh) return h;
    if (dynamic_cast<const Gaudi::Algorithm*> (parent)) return pdh;
    h = pdh;
  }
}


/**
 * @brief Helper to scan a tree of components to search for another handle key
 *        potentially conflicting with the container key part of a
 *        @c WriteDecorHandleKey.  Given a key, a tree of Gaudi components,
 *        and one (`owner') component within that tree, we search to see
 *        if any components occuring in the tree _before_ `owner' have
 *        a key that matches the one we're searching for.
 */
class WDHScan
  : public IDataHandleVisitor
{
public:
  /**
   * @brief Constructor.
   * @param contHandleKey key to search for.
   * @param owner The `owner' component, at which we stop the search.
   */
  WDHScan (const DataObjID& contHandleKey, const IDataHandleHolder* owner)
    : m_contHandleKey (contHandleKey),
      m_owner (owner)
  {
  }

  /// Called for each component in the tree.
  virtual void visit (const IDataHandleHolder* h) override;

  /// The key we're searching for.
  const DataObjID& m_contHandleKey;

  /// Stop the search when we reach this component.
  const IDataHandleHolder* m_owner;

  /// Did we find the key?
  bool m_found = false;

  /// Have we reached m_owner yet?
  bool m_sawOwner = false;
};


/**
 * @brief Called for each component in the tree.
 * @param H Component to visit.
 */
void WDHScan::visit (const IDataHandleHolder* h)
{
  // Don't do anything if we've already found the key, or if we've seen m_owner.
  if (m_found || m_sawOwner) return;
  if (h == m_owner) {
    m_sawOwner = true;
    return;
  }

  // Test to see if this component has a key matching the one we're looking for.
  m_found = handleInHolder (m_contHandleKey, *h);
}


/**
 * @brief Optionally register read dependency of a @c WriteDecorHandleKey.
 *
 * A @c WriteDecorHandle logically has a read dependency on the container itself.
 * However, just declaring this as a usual @c ReadHandleKey will cause problems
 * if used in an algorithm that also creates the container: we'll get complaints
 * from the @c Algorithm base class about circular dependencies.
 *
 * To deal with this, we don't declare the container @c ReadHandleKey in
 * @c declareProperty.  Instead, we wait until @c initialize and declare
 * the dependency with @c addDependency only if it is not already
 * on a handle list.
 *
 * This issue also comes up across multiple tools in a single algorithm.
 * For example, if an algorithm that has one tool that creates an container
 * and another tool with a WriteDecorHandleKey, we can get a warning about
 * a circular dependency.  To suppress this for the common case, we also
 * search the component tree under the algorithm and avoid adding the
 * @c ReadHandleKey if there's a matching component in the tree _before_
 * the one owning the WriteDecorHandleKey.  (We only do it for components
 * earlier in the tree walk to make it less likely that we'll suppress
 * the warning for a genuine use-before-write misconfiguration.)
 */
void registerWriteDecorHandleKey (IDataHandleHolder* owner,
                                  const DataObjID& contHandleKey)
{
  if (!owner) return;

  // Does this component already have a key matching @c contHandleKey?
  bool found = handleInHolder (contHandleKey, *owner);

  if (!found) {
    // No --- now search the complete component tree up to @c owner for a match.
    const IDataHandleHolder* parent = findParent (owner);
    if (parent && parent != owner) {
      WDHScan scan (contHandleKey, owner);
      parent->acceptDHVisitor (&scan);
      found = scan.m_found;
    }
  }

  if (!found) {
    owner->addDependency (contHandleKey, Gaudi::DataHandle::Reader);
  }
}


} // namespace detail
} // namespace SG
