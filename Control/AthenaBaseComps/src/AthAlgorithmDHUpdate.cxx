/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaBaseComps/src/AthAlgorithmDHUpdate.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Update output dependencies to include symlinks.
 */


#include "AthAlgorithmDHUpdate.h"
#include "AthenaKernel/BaseInfo.h"


namespace AthenaBaseComps {


/**
 * @brief Constructor.
 * @param linkedObjs Set to which entries for symlinks will be added.
 * @param chain Hook to call after this one completes.
 */
AthAlgorithmDHUpdate::AthAlgorithmDHUpdate (DataObjIDColl& linkedObjs,
                                            std::unique_ptr<IDataHandleVisitor> chain)
  : m_linkedObjs (linkedObjs),
    m_chain (std::move (chain))
{
}


/**
 * @brief Walk over the dependencies of an algorithm.
 * @param dhh The algorithm object.
 *
 * Finds output dependencies of the algorithms that have symlinks
 * available and enters these links in @c linkedObjs.
 */
void AthAlgorithmDHUpdate::visit (const IDataHandleHolder* dhh)
{
  // Make a copy, as usually linkedObjs will be extraOutputDeps().
  DataObjIDColl ex = dhh->extraOutputDeps();

  // Process all output dependencies.
  for (const Gaudi::DataHandle* h : dhh->outputHandles()) {
    if (!h->objKey().empty())
      handle (h->fullKey().clid(), h->objKey());
  }

  for (const DataObjID& dobj : ex) {
    if (!dobj.key().empty())
      handle (dobj.clid(), dobj.key());
  }

  for (const DataObjID& dobj : dhh->outputDataObjs()) {
    if (!dobj.key().empty())
      handle (dobj.clid(), dobj.key());
  }

  // If we put anything in linkedObjs, also add the contents
  // of extraOutputDeps.
  if (!m_linkedObjs.empty()) {
    m_linkedObjs.insert (ex.begin(), ex.end());
  }

  // Call the next hook function, if any.
  if (m_chain) {
    m_chain->visit (dhh);
  }
}


/**
 * @brief Handle one output dependency.
 * @param clid The CLID of the output.
 * @param key The SG key of the output.
 *
 * If CLID has symlinks available, enter these links in @c linkedObjs.
 */
void AthAlgorithmDHUpdate::handle (CLID clid, const std::string& key)
{
  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find (clid);
  if (!bib) return;
  for (CLID clid2 : bib->get_bases()) {
    if (clid2 != clid) {
      m_linkedObjs.emplace (clid2, key);
    }
  }
}


} // namespace AthenaBaseComps
