/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TTreeBranchMap.cxx,v 1.3 2008-07-14 21:02:50 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/TTreeBranchMap.cxx
 * @author scott snyder
 * @date October 2007
 * @brief A @c TTree that also keeps a map of persistent->transient
 *        branch names.  This will be used for the transient tree.
 */


#include "AthenaROOTAccess/TTreeBranchMap.h"
#include <utility>
#include <iostream>


namespace AthenaROOTAccess {


/**
 * @brief Constructor.
 * @param name The tree name.
 * @param title The tree title.
 */
TTreeBranchMap::TTreeBranchMap (const char* name, const char* title)
  : TTree (name, title)
{
}


/**
 * @brief Add a new branch to the tree.
 * @param br The branch to add.
 * @param mapped_name The mapped (persistent) name of the branch.
 */
void TTreeBranchMap::addBranch (TBranch* br, const std::string& mapped_name)
{
  m_map[mapped_name] = br->GetName();

  TObjArray* branches = this->GetListOfBranches();
  Int_t nb = branches->GetEntriesFast();
  for (Int_t i = 0; i < nb; i++) {
    TBranch* branch = (TBranch*) branches->UncheckedAt(i);
    if (!strcmp(branch->GetName(), br->GetName())) {
      if (br == branch)
        return;
      break;
    }
  }
  
  branches->Add (br);
}


/**
 * @brief Look up a branch by its mapped (persistent) name.
 * @param mapped_name The mapped (persistent) name of the branch.
 * @return The name of the corresponding branch, or an empty string.
 */
const std::string&
TTreeBranchMap::findBranch (const std::string& mapped_name) const
{
  map_t::const_iterator i = m_map.find (mapped_name);
  if (i != m_map.end())
    return i->second;
  static std::string dum;
  return dum;
}


/**
 * @brief Dump out the branch map.  For debugging.
 */
void TTreeBranchMap::dumpBranchMap() const
{
  for (const map_t::value_type& p : m_map)
    std::cout << p.first << " -> " << p.second <<"\n";
}


/**
 * @brief Add an alias for a branch.
 * @param br An existing branch.
 * @param alias_name An alternate name for the branch.
 */
void TTreeBranchMap::addAlias (const TBranch* br,
                               const std::string& alias_name)
{
  m_aliasmap.insert (std::make_pair (std::string (br->GetName()), alias_name));
}


/**
 * @brief Retrieve the set of aliases for a branch.
 * @param branch_name Name of the branch.
 */
std::vector<std::string>
TTreeBranchMap::getAliases (const std::string& branch_name) const
{
  std::pair<aliasmap_t::const_iterator, aliasmap_t::const_iterator> its =
    m_aliasmap.equal_range (branch_name);
  std::vector<std::string> ret;
  while (its.first != its.second) {
    ret.push_back (its.first->second);
    ++its.first;
  }
  return ret;
}


/**
 * @brief Add an alias for a CLID.
 * @param clid Existing CLID.
 * @param alias_clid Alias available for that CLID.
 */
void TTreeBranchMap::addCLIDAlias (CLID clid, CLID alias_clid)
{
  m_clidaliasmap.insert (std::make_pair (clid, alias_clid));
}


/**
 * @brief Retrieve the set of aliases for a CLID.
 * @param clid CLID to look up.
 */
std::vector<CLID>
TTreeBranchMap::getCLIDAliases (CLID clid) const
{
  std::pair<clidaliasmap_t::const_iterator,clidaliasmap_t::const_iterator> its=
    m_clidaliasmap.equal_range (clid);
  std::vector<CLID> ret;
  while (its.first != its.second) {
    ret.push_back (its.first->second);
    ++its.first;
  }
  return ret;
}

} // namespace AthenaROOTAccess
