/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TBranchAlias.cxx,v 1.9 2009-02-11 02:18:49 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/TBranchAlias.cxx
 * @author scott snyder
 * @date Aug 2007
 * @brief Redirect queries to another branch.
 */


#include "AuxStoreARA.h"
#include "AthenaROOTAccess/TBranchAlias.h"
#include "AthenaROOTAccess/TTreeTrans.h"
#include "AthenaROOTAccess/branchSeek.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "TTree.h"
#include "TError.h"


R__EXTERN TTree* gTree;


namespace AthenaROOTAccess {


/**
 * @brief Helper function, to get the class name for a tree/branch.
 * @param targ_tree The target tree.
 * @param targ_branchname The name of the target branch within the tree.
 */
const char*
alias_fetch_classname (TTree* targ_tree, const char* targ_branchname)
{
  TBranch* br = targ_tree->GetBranch (targ_branchname);
  if (br) {
    return br->GetClassName();
  }
  return "";
}


/**
 * @brief Destructor.
 */
TBranchAlias::~TBranchAlias()
{
  setStore (true);
}


/**
 * @brief Create a new TBranchAlias object and add it to a tree.
 * @param tree The tree to hold the new branch.
 * @param name The name of the new branch.
 * @param key The SG key for the object we're managing.
 * @param clid The primary SG clid.
 * @param targ_tree The tree containing the branch to be aliased.
 * @param targ_branchname The name of the branch being allocated.
 * @param aux_branchname Branch name of associated aux store, or
 *                       a null string if none.
 * @return The new branch.
 */
TBranchAlias* TBranchAlias::addToTree (TTreeTrans* tree,
                                       const char* name,
                                       const char* key,
                                       CLID clid,
                                       TTree* targ_tree,
                                       const char* targ_branchname,
                                       const char* aux_branchname /*= ""*/)
{
  // Needed by the Root code...
  gTree = tree;

  // May be needed by constructors.
  TTreeTrans::Push save_tree (tree);

  // Make the new branch.
  TBranchAlias* br = new TBranchAlias (tree, name,
                                       key, clid,
                                       targ_tree, targ_branchname,
                                       aux_branchname);

  // Add it to the tree.
  tree->GetListOfBranches()->Add (br);
  tree->addBranch (br, targ_branchname);

  return br;
}


/**
 * @brief Standard Root method to read a new entry.
 * @param entry The entry number to be read.
 * @param getall If true, ignore branch disable flags.
 * @return The number of bytes read.
 */
Int_t TBranchAlias::GetEntry (Long64_t entry, Int_t /*getall = 0*/)
{
  if (TestBit(kDoNotProcess))
    return 0;

  // Translate to the proper local entry number (in case of a chain).
  Long64_t local_entry = m_targ_tree->LoadTree (entry);

  fReadEntry = local_entry;

  // Find the branch, if we haven't done so yet.
  if (!checkBranch())
    return 0;

  // Forward to the target branch.
  TTreeTrans::Push push (m_trans_tree);
  m_trans_tree->setEntry (entry);

  // Read aux branch if needed.
  if (m_aux_branch)
    branchSeek (m_aux_branch, local_entry);
  if (m_dyn_store)
    m_dyn_store->GetEntry (local_entry);

  m_targ_branch->SetStatus (true);
  Int_t ret = branchSeek (m_targ_branch, local_entry);
  if (ret == AthenaROOTAccess::NOREAD)
    ret = 1;

  // Update our cached address if needed.
  if (GetAddress() != m_targ_branch->GetAddress()) {
    TBranchObject::SetAddress (m_targ_branch->GetAddress());
    fReadEntry = local_entry;
  }
  setStore (false);
  m_targ_branch->SetStatus (false);

  // Clear cached data.
  m_trans_tree->resetBranch (m_sgkey);

  if (m_clearCache_method.IsValid()) {
    void* thisobj = *reinterpret_cast<void**> (this->GetAddress());
    if (thisobj)
      m_clearCache_method.Execute(thisobj);
  }

  // Done.
  return ret;
}


/**
 * @brief Change the buffer address for this branch.
 * @param addr The new buffer address.  (Pointer-to-pointer to buffer.)
 *
 * This will be forwarded to the aliased branch.
 */
void TBranchAlias::SetAddress (void* addr)
{
  if (checkBranch()) {
    m_targ_branch->SetStatus (true);
    TBranchObject::SetAddress (addr);
    m_targ_branch->SetAddress (addr);
    m_targ_branch->SetStatus (false);
    setStore(false);
  }

  // Clear cached data.
  m_trans_tree->resetBranch (m_sgkey);
}


/**
 * @brief Root notification callback.
 *
 * We'll arrange for this to be called whenever we move to a new
 * file for the target tree.  (Only applicable if it's a chain.)
 */
Bool_t TBranchAlias::Notify()
{

  if (m_aux_branch){
    //setStore (true);
    m_aux_branch = 0;
    m_dyn_branch_map.clear();
    m_dyn_store = 0;
  }

  // Clear out the cached branch pointers.
  m_targ_branch = 0;

  // Call any previous notification routine.
  if (m_notify_chain) 
    return m_notify_chain->Notify();

  return true;
}


/**
 * @brief Fetch our cached branch address, if needed.
 * @return True on success; false if the branch couldn't be found.
 */
bool TBranchAlias::checkBranch ()
{
  if (!m_targ_branch) {
    m_targ_branch = m_targ_tree->GetBranch (m_targ_branchname.c_str());
    if (!m_targ_branch) {
      ::Error ("TBranchAlias::GetEntry",
               "Can't find branch %s in tree %s",
               m_targ_branchname.c_str(),
               m_targ_tree->GetName());
      return false;
    }

    // Look for an aux branch.
    if (!m_aux_branch && !m_aux_branchname.empty())
      findAux();
  }

  return true;
}


/**
 * @brief Constructor.
 * @param tree The transient tree.
 * @param name The name of the new branch.
 * @param key The SG key for the object we're managing.
 * @param clid The primary SG clid.
 * @param targ_tree The tree containing the branch to be aliased.
 * @param targ_branchname The name of the branch being allocated.
 * @param aux_branchname Branch name of associated aux store, or
 *                       a null string if none.
 */
TBranchAlias::TBranchAlias (TTreeTrans* tree,
                            const char* name,
                            const char* key,
                            CLID clid,
                            TTree* targ_tree,
                            const char* targ_branchname,
                            const char* aux_branchname)
  : TBranchObject (tree,
                   name,
                   alias_fetch_classname (targ_tree, targ_branchname),
                   &(m_addrtmp = 0)),
    m_trans_tree (tree),
    m_targ_tree (targ_tree),
    m_targ_branchname (targ_branchname),
    m_targ_branch (0),
    m_notify_chain (targ_tree->GetNotify()),
    m_sgkey (tree->stringToKey (key, clid)),
    m_aux_branchname (aux_branchname),
    m_aux_branch (0),
    m_dyn_store (0),
    m_aux_offset (-1),
    m_holder_offset (-1)
{
  // Set up the notification callback.
  targ_tree->SetNotify (this);

  // Check if this type uses aux data.
  setupAux();
}


/**
 * @brief Set up for reading aux data, if this type has any.
 */
void TBranchAlias::setupAux()
{
  TClass* cl = TClass::GetClass (this->GetClassName());
  if (!cl) return;

  m_setStore_method.InitWithPrototype (cl, "setStore",
                                       "const SG::IConstAuxStore*");
  if (!m_setStore_method.IsValid()) return;

  m_clearCache_method.InitWithPrototype (cl, "clearCache", "");

  if (m_aux_branchname.empty())
    m_aux_branchname = std::string (this->GetName()) + "Aux.";

  m_dyn_branch_prefix = 
    m_aux_branchname.substr (0, m_aux_branchname.size()-1) + "Dyn.";
}


/**
 * @brief Look for an aux data branch.
 */
void TBranchAlias::findAux()
{
  TBranch* br = m_targ_tree->GetBranch (m_aux_branchname.c_str());
  if (!br) return;

  TClass* cl = TClass::GetClass (br->GetClassName());
  if (!cl) return;

  const TClass* auxcl = TClass::GetClass ("SG::IConstAuxStore");
  if (!auxcl) return;
  m_aux_offset = cl->GetBaseClassOffset (auxcl);
  if (m_aux_offset < 0) return;
  m_aux_branch = br;

  const TClass* holder_cl = TClass::GetClass ("SG::IAuxStoreHolder");
  if (holder_cl)
    m_holder_offset = cl->GetBaseClassOffset (holder_cl);

  setStore (false);
}


/**
 * @brief Set/clear the aux store for our object.
 * @param clear If true, clear the store; otherwise set it if possible.
 */
void TBranchAlias::setStore (bool clear)
{
  // Create objects if needed.
  if (m_targ_branch->GetAddress() == 0)
    m_targ_branch->SetAddress(0);
  if (m_aux_branch && m_aux_branch->GetAddress() == 0)
    m_aux_branch->SetAddress(0);

  // Update our cached address if needed.
  if (GetAddress() != m_targ_branch->GetAddress() &&
      m_targ_branch->GetAddress() != 0)
  {
    TBranchObject::SetAddress (m_targ_branch->GetAddress());
  }

  if (!m_aux_branch || !m_setStore_method.IsValid()) return;
  void* thisobj = *reinterpret_cast<void**> (this->GetAddress());
  if (!thisobj) return;

  if (m_dyn_store && clear) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch->GetAddress());
    if (auxptr && *auxptr) {
      SG::IAuxStoreHolder* holder =
        reinterpret_cast<SG::IAuxStoreHolder*> (*auxptr + m_holder_offset);
      holder->setStore (0);
    }
    m_dyn_store = 0;
  }

  void* auxobj = 0;
  if (!clear) {
    auxobj =
      *reinterpret_cast<char**> (m_aux_branch->GetAddress()) + m_aux_offset;
  }
  m_setStore_method.SetParamPtrs (&auxobj, -1);
  m_setStore_method.Execute (thisobj);

  if (!clear && m_holder_offset >= 0 && !m_dyn_store) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch->GetAddress());
    if (auxptr && *auxptr) {
      SG::IAuxStoreHolder* holder =
        reinterpret_cast<SG::IAuxStoreHolder*> (*auxptr + m_holder_offset);
      bool standalone =
        holder->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore;
      m_dyn_store = new AuxStoreARA (*this,
                                     this->GetReadEntry(),
                                     standalone);
      holder->setStore (m_dyn_store);
    }
  }
}


std::vector<IAuxBranches::auxpair_t> TBranchAlias::auxBranches() const
{
  std::vector<IAuxBranches::auxpair_t> out;
  TIter bit (m_targ_tree->GetListOfBranches());
  while (TObject* br = bit()) {
    if (strncmp (br->GetName(),
                 m_dyn_branch_prefix.c_str(),
                 m_dyn_branch_prefix.size()) == 0)
    {
      out.emplace_back(std::string (br->GetName() + m_dyn_branch_prefix.size()),
                       static_cast<TBranch*>(br));
    }
  }
  return out;
}


TBranch* TBranchAlias::findAuxBranch (SG::auxid_t auxid) const
{
  auto abr_it = m_dyn_branch_map.find(auxid);
  if( abr_it != m_dyn_branch_map.end() ) {
    return abr_it->second;
  }

  std::string attr_name = SG::AuxTypeRegistry::instance().getName(auxid);
  TBranch* branch = m_targ_tree->GetBranch( (m_dyn_branch_prefix + attr_name).c_str() );
  // remember the result (even null) to not call GetBranch again
  m_dyn_branch_map[auxid] = branch;
  return branch;
}


StatusCode TBranchAlias::readAuxBranch (TBranch& br, void* p, long long entry)
{
  TTreeTrans::Push save_tree (m_trans_tree);

  if (p)  br.SetAddress(p);    // 0 for basic types
  int nbytes = br.GetEntry(entry);
  if( nbytes < 1 ) {
    ::Error ("TBranchAlias",
             "Error reading AUX branch %s for entry No. %lld",
             br.GetName(), entry);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}



//***********************************************************************
//  Dummy methods required by root.
//


TClass* TBranchAlias::Class()
{
  if (!fgIsA)
    fgIsA = TClass::GetClass ("AthenaROOTAccess::TBranchAlias");
  return fgIsA;
}


void TBranchAlias::ShowMembers (TMemberInspector& R__insp)
{
  TBranchObject::ShowMembers (R__insp);
}


void TBranchAlias::Streamer (TBuffer& b)
{
  TBranchObject::Streamer (b);
}


TClass* TBranchAlias::fgIsA = 0;


} // namespace AthenaROOTAccess
