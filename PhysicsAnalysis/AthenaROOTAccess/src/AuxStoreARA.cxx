/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAcccess/src/AuxStoreARA.cxx
 * @author marcin, scott
 * @date May, 2014
 * @brief Aux store implementation to manage dynamic aux variables.
 *
 * This is basically the same as the corresponding class in RootStorageSvc.
 * Duplicated here due to the lack of any suitable common packages
 * with the correct dependencies.
 */


#include "AuxStoreARA.h"
#include "AthContainersRoot/getDynamicAuxID.h"
#include "AthenaROOTAccess/branchSeek.h"
#include "AthContainers/tools/error.h"

#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"

#include "TROOT.h"
#include "TBranch.h"
#include "TClass.h"

#include <iostream>


using namespace AthenaROOTAccess;


AuxStoreARA::AuxStoreARA(SG::IAuxBranches &container, long long entry, bool standalone)
  : SG::AuxStoreRoot (container, entry, standalone)
{
  fillAuxIDs();
}


SG::auxid_t AuxStoreARA::resolveAuxID (SG::IAuxBranches& /*container*/,
                                       const std::type_info* ti,
                                       const std::string& name,
                                       const std::string& elem_type_name,
                                       const std::string& branch_type_name)
{
  if (!ti) {
    SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
    return r.findAuxID(name);
  }
  return SG::getDynamicAuxID (*ti, name,
                              elem_type_name, branch_type_name,
                              standalone());
 }


AuxStoreARA::~AuxStoreARA()
{
  for (TBranch* br : m_branches)
    br->SetAddress(nullptr);
}


void AuxStoreARA::GetEntry (long long entry)
{
  setEntry (entry);
  for (TBranch* br : m_branches)
    branchSeek (br, entry);
}


bool AuxStoreARA::doReadData (SG::IAuxBranches& container,
                              SG::auxid_t /*auxid*/,
                              TBranch& branch,
                              TClass* cl,
                              void* vector,
                              long long entry)
{
   void* data;

   if (standalone() && !cl)
     data = vector;
   else {
     m_ptrs.push_back (vector);
     data = &m_ptrs.back();
   }

   if( container.readAuxBranch(branch, data, entry).isSuccess() ) {
    m_branches.push_back (&branch);
    return true;
  }

  ATHCONTAINERS_ERROR("AuxStoreAPR::doReadData", std::string("Error reading branch ") + branch.GetName() );
  return false;
}
