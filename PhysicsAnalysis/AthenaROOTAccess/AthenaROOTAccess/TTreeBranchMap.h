// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TTreeBranchMap.h,v 1.4 2008-12-23 05:01:34 ssnyder Exp $

/**
 * @file AthenaROOTAccess/TTreeBranchMap.h
 * @author scott snyder
 * @date October 2007
 * @brief A @c TTree that also keeps a map of persistent->transient
 *        branch names.  This will be used for the transient tree.
 */


#ifndef ATHENAROOTACCESS_TTREEBRANCHMAP_H
#define ATHENAROOTACCESS_TTREEBRANCHMAP_H


#include "GaudiKernel/ClassID.h"
#include "TTree.h"
#include <string>
#include <vector>
#include <map>


namespace AthenaROOTAccess {


/**
 * @brief A @c TTree that also keeps a map of persistent->transient
 *        branch names.  This will be used for the transient tree.
 */
class TTreeBranchMap
  : public TTree
{
public:
  /**
   * @brief Default constructor.
   */
  TTreeBranchMap () {}


  /**
   * @brief Constructor.
   * @param name The tree name.
   * @param title The tree title.
   */
  TTreeBranchMap (const char* name, const char* title);


  /**
   * @brief Add a new branch to the tree.
   * @param br The branch to add.
   * @param mapped_name The mapped (persistent) name of the branch.
   */
  void addBranch (TBranch* br, const std::string& mapped_name);


  /**
   * @brief Look up a branch by its mapped (persistent) name.
   * @param mapped_name The mapped (persistent) name of the branch.
   * @return The name of the corresponding branch, or an empty string.
   */
  const std::string& findBranch (const std::string& mapped_name) const;


  /**
   * @brief Dump out the branch map.  For debugging.
   */
  void dumpBranchMap() const;


  /**
   * @brief Add an alias for a branch.
   * @param br An existing branch.
   * @param alias_name An alternate name for the branch.
   */
  void addAlias (const TBranch* br, const std::string& alias_name);


  /**
   * @brief Retrieve the set of aliases for a branch.
   * @param branch_name Name of the branch.
   */
  std::vector<std::string> getAliases (const std::string& branch_name) const;


  /**
   * @brief Add an alias for a CLID.
   * @param clid Existing CLID.
   * @param alias_clid Alias available for that CLID.
   */
  void addCLIDAlias (CLID clid, CLID alias_clid);


  /**
   * @brief Retrieve the set of aliases for a CLID.
   * @param clid CLID to look up.
   */
  std::vector<CLID> getCLIDAliases (CLID clid) const;


  ClassDefOverride (AthenaROOTAccess::TTreeBranchMap, 1);


private:
  /// The branch name map.
  typedef std::map<std::string, std::string> map_t;
  map_t m_map;

  /// The alias name map.
  typedef std::multimap<std::string, std::string> aliasmap_t;
  aliasmap_t m_aliasmap;

  /// The CLID alias map.
  typedef std::multimap<CLID, CLID> clidaliasmap_t;
  clidaliasmap_t m_clidaliasmap;

  // Disallow copying.
  TTreeBranchMap& operator= (const TTreeBranchMap&);
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_TTREEBRANCHMAP_H
