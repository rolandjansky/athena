// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaBaseComps/src/AthAlgorithmDHUpdate.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Update output dependencies to include symlinks.
 */


#ifndef ATHENABASECOMPS_ATHALGORITHMDHUPDATE_H
#define ATHENABASECOMPS_ATHALGORITHMDHUPDATE_H


#include "GaudiKernel/IDataHandleHolder.h"
#include "GaudiKernel/DataObjID.h"
#include "GaudiKernel/ClassID.h"
#include <memory>


namespace AthenaBaseComps {


/**
 * @brief Update output dependencies to include symlinks.
 *
 * This is intended to be installed in an algorithm via the
 * m_updateDataHandles hook, to run before Algorithm::sysInitialize
 * merges the sets of input and output dependencies.
 *
 * It makes a first pass over all the output dependences.  For each,
 * it examines the CLID to see if there are any other CLIDs to which
 * it is convertable, and if so, adds them to the @c linkedObjs list.
 * The intention is that the algorithm will then override
 * @c extraOutputDeps so that it will include these extra entries.
 */
class AthAlgorithmDHUpdate
  : public IDataHandleVisitor
{
public:
  /**
   * @brief Constructor.
   * @param linkedObjs Set to which entries for symlinks will be added.
   * @param chain Hook to call after this one completes.
   */
  AthAlgorithmDHUpdate (DataObjIDColl& linkedObjs,
                        std::unique_ptr<IDataHandleVisitor> chain);
  

  /**
   * @brief Walk over the dependencies of an algorithm.
   * @param dhh The algorithm object.
   *
   * Finds output dependencies of the algorithms that have symlinks
   * available and enters these links in @c linkedObjs.
   */
  virtual void visit(const IDataHandleHolder* dhh) override;

  
private:
  /**
   * @brief Handle one output dependency.
   * @param clid The CLID of the output.
   * @param key The SG key of the output.
   *
   * If CLID has symlinks available, enter these links in @c linkedObjs.
   */
  void handle (CLID clid, const std::string& key);


  /// Set of additional output dependencies represented by links.
  DataObjIDColl& m_linkedObjs;

  /// Hook to call after this one.
  std::unique_ptr<IDataHandleVisitor> m_chain;
};


} // namespace AthenaBaseComps


#endif // not ATHENABASECOMPS_ATHALGORITHMDHUPDATE_H
