// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAcccess/AuxStoreARA.h
 * @author marcin, scott
 * @brief Aux store implementation to manage dynamic aux variables.
 *
 * This is basically the same as the corresponding class in RootStorageSvc.
 * Duplicated here due to the lack of any suitable common packages
 * with the correct dependencies.
 */


#ifndef ATHENAROOTACCESS_AUXSTOREARA
#define ATHENAROOTACCESS_AUXSTOREARA

#include "AthContainersRoot/AuxStoreRoot.h" 
#include "AthContainersRoot/IAuxBranches.h"

#include "AthContainersInterfaces/IAuxTypeVector.h"
#include <vector>
#include <list>
#include "TClass.h"
class TBranch;


namespace AthenaROOTAccess {


/**
 * @brief Aux store implementation to manage dynamic aux variables.
 */
class AuxStoreARA : public SG::AuxStoreRoot
{
public:
  AuxStoreARA(SG::IAuxBranches &container, long long entry, bool standalone=false);
  virtual ~AuxStoreARA() override;

  void GetEntry (long long entry);
  void SetEntry (long long entry) { setEntry (entry); }

  virtual SG::auxid_t resolveAuxID (SG::IAuxBranches& container,
                                    const std::type_info* ti,
                                    const std::string& name,
                                    const std::string& elem_type_name,
                                    const std::string& branch_type_name) override;

  virtual bool doReadData (SG::IAuxBranches& container,
                           SG::auxid_t auxid,
                           TBranch& branch,
                           TClass* cl,
                           void* vector,
                           long long entry) override;

private:
  std::list<void*> m_ptrs;
  std::vector<TBranch*> m_branches;
};


} // namespace AthenaROOTAccess

#endif

