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
#include "AthenaROOTAccess/TBranchTPConvert.h"
#include "AthenaROOTAccess/TTreeTrans.h"
#include "AthenaROOTAccess/branchSeek.h"
#include "AthenaROOTAccess/IARAFixup.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "RootUtils/TBranchElementClang.h"
#include "TTree.h"
#include "TError.h"
#include "TList.h"
#include "TLeafObject.h"


#if ! (ROOT_VERSION_CODE >= ROOT_VERSION(6,1,0) || (ROOT_VERSION_CODE>=ROOT_VERSION(5,34,22) && ROOT_VERSION_CODE<ROOT_VERSION(6,0,0)))
R__EXTERN TTree* gTree;
#endif


namespace AthenaROOTAccess {


void mergeAuxStore (const SG::IConstAuxStore& orig,
                    SG::IAuxStore& copy)
{
  size_t size = orig.size();
  if (size == 0) return;

  // Access the auxiliary type registry:
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  // The auxiliary IDs that the original container has:
  SG::auxid_set_t auxids = orig.getAuxIDs();

  copy.resize (size);
  
  // Loop over all the variables of the original container:
  for (SG::auxid_t auxid : auxids) {
    // Skip null auxids (happens if we don't have the dictionary)
    if(auxid == SG::null_auxid) continue;

    // Access the source variable:
    const void* src = orig.getData (auxid);

    if (!src) continue;

    // Create the target variable:
    void* dst = copy.getData (auxid, size, size);

    // Copy over all elements.
    for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
      r.copy (auxid, dst, idst, src, isrc);
      ++idst;
    }
  }
}


class TBranchAccess : public TBranch
{
public:
  static int doNotUseBufferMap() { return kDoNotUseBufferMap; }
};



/**
 * @brief Fix up branch status bits after possible schema evolution
 *        from std::vector to SG::PackedContainer.
 * @param br The branch to process.
 *
 * Some background is needed here.
 *
 * ROOT can keep a map associated with a buffer giving the location of objects
 * in the root file.  This is used to handle C pointer resolution on I/O.
 * The use of this is controlled by two flags in fBits.
 * If kDoNotUseBufferMap is _clear_, then the map get cleared on each
 * event (when reading); otherwise, it is not.  This bit is set by default
 * when a branch is created.  When Fill() is called, then it is cleared
 * if the map is empty.  The bit is then saved and restored along
 * with the TBranch.
 *
 * But a branch also has the kBranchAny bit.  If this is set, then ROOT
 * will actually try to make entries in the map.  This is set when
 * a branch is initialized and also when SetAddress is called.  The bit
 * is set if the class _might_ contain pointers.  Specifically, the bit
 * is clear for branches with fundamental types, set::string, and std::vector
 * of any of these.  It is turned on for anything else, in particular
 * for any user types.
 *
 * This logic causes a problem when we schema evolve from std::vector
 * to SG::PackedContainer.  When the branch is written, the map is not
 * used.  So kDoNotUseBufferMap is set, and kBranchAny is clear.
 * The branch is saved, and read back in with the same flag settings.
 * But then when we go to do the SetAddress, ROOT sees that the type
 * we have is SG::PackedContainer --- a user type.  So kBranchAny
 * is turned on.  This means that the map gets filled when we read.
 * But because kDoNotUseBufferMap is set, the map is not cleared
 * when we go to a new event.  This results in errors from TExMap
 * about duplicate keys.
 *
 * The easiest thing to do to resolve this seems to be to turn off
 * the kDoNotUseBufferMap so that the map will in fact get cleared.
 * (The arguably more consistent scheme of turning off kBranchAny
 * is not preferred since we'd have to do that every time after
 * SetAddress is called.)  Ideally, we'd like to be able to tell
 * ROOT that a given class does not use pointers, but there's no way
 * to do that at the moment.
 */
void fixupPackedConversion (TBranch* br)
{
  TIter next( br->GetListOfBranches() );
  while ( TBranch* b = (TBranch*)next() ) {
    fixupPackedConversion (b);
  }

  TBranchElement* belt = dynamic_cast<TBranchElement*> (br);
  if (belt) {
    TClass* cl = belt->GetCurrentClass();
    if (cl && strncmp (cl->GetName(), "SG::PackedContainer<", 20) == 0) {
      br->ResetBit (TBranchAccess::doNotUseBufferMap());
    }
  }
}


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
  //setStore (true, -1);
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
#if ! (ROOT_VERSION_CODE >= ROOT_VERSION(6,1,0) || (ROOT_VERSION_CODE>=ROOT_VERSION(5,34,22) && ROOT_VERSION_CODE<ROOT_VERSION(6,0,0)))
  // Needed by the Root code...
  gTree = tree;
#endif

  // May be needed by constructors.
  TTreeTrans::Push save_tree (tree);

  // Make the new branch.
  TBranchAlias* br = new TBranchAlias (tree, name,
                                       key, clid,
                                       targ_tree, targ_branchname,
                                       aux_branchname);

  // Add it to the tree.
  tree->GetListOfBranches()->Add (br);

  std::string targ_name = targ_branchname;
  TBranchTPConvert* br_targ =
    dynamic_cast<TBranchTPConvert*>(targ_tree->GetBranch (targ_branchname));
  if (br_targ)
    targ_name = br_targ->getPersBranchName();
  tree->addBranch (br, targ_name);

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
  //if (m_dyn_store)
  //m_dyn_store->GetEntry (local_entry);
  if (m_holder_offset_orig >= 0) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch_orig->GetAddress());
    if (auxptr && *auxptr) {
      SG::IAuxStoreHolder* holder =
        reinterpret_cast<SG::IAuxStoreHolder*> (*auxptr + m_holder_offset_orig);
      AuxStoreARA* store = dynamic_cast<AuxStoreARA*>(holder->getStore());
      if (store) store->GetEntry (local_entry);
    }
  }

  if (m_aux_branch) {
    Long64_t ent = entry;
    if (m_aux_branch == m_aux_branch_orig &&
        dynamic_cast<TBranchTPConvert*>(m_aux_branch) == nullptr)
      ent = local_entry;
    branchSeek (m_aux_branch, ent);
  }

  // Update our cached address if needed.
  if (m_targ_branch->GetAddress() == 0)
    m_targ_branch->SetAddress(0);
  if (GetAddress() != m_targ_branch->GetAddress()) {
    TBranch::SetAddress (m_targ_branch->GetAddress());
    fReadEntry = local_entry;
  }

  if (m_setStore_method.IsValid()) {
    void* thisobj = *reinterpret_cast<void**> (this->GetAddress());
    if (thisobj) {
      void* auxobj = 0;
      m_setStore_method.SetParamPtrs (&auxobj, 1);
      m_setStore_method.Execute (thisobj);
    }
  }

  // Let this be done by the branchSeek; otherwise, it might not
  // go to the correct entry.
  //m_targ_branch->SetStatus (true);
  Int_t ret;
  {
    Long64_t ent = local_entry;
    if (dynamic_cast<TBranchTPConvert*> (m_targ_branch))
      ent = entry;
    ret = branchSeek (m_targ_branch, ent);
  }
  if (ret == AthenaROOTAccess::NOREAD)
    ret = 1;

  // Update our cached address if needed.
  if (GetAddress() != m_targ_branch->GetAddress()) {
    TBranch::SetAddress (m_targ_branch->GetAddress());
    fReadEntry = local_entry;
  }

  // Run fixup, if needed.
  if (m_fixup) {
    void** objptr = reinterpret_cast<void**> (this->GetAddress());
    if (objptr && *objptr)
      m_fixup->fixupBeforeAux (*objptr, entry);
  }

  setStore (false, local_entry);
  m_targ_branch->SetStatus (false);

  // Clear cached data.
  m_trans_tree->resetBranch (m_sgkey);

  if (m_clearCache_method.IsValid()) {
    void* thisobj = *reinterpret_cast<void**> (this->GetAddress());
    if (thisobj)
      m_clearCache_method.Execute(thisobj);
  }

  // Run fixup, if needed.
  if (m_fixup) {
    void** objptr = reinterpret_cast<void**> (this->GetAddress());
    if (objptr && *objptr) {
      m_fixup->fixupAfterAux (*objptr, entry);
      m_dyn_store = 0;  // Force this to be remade next time.
    }
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
    if (addr) {
      m_targ_branch->SetStatus (true);
      TBranch::SetAddress (addr);
      m_targ_branch->SetAddress (addr);
      m_targ_branch->SetStatus (false);
    }
    setStore(false, this->GetReadEntry());
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
    m_aux_branch_orig = 0;
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


const char* TBranchAlias::GetClassName() const
{
  return m_className.c_str();
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
  : TBranch(),
    m_trans_tree (tree),
    m_targ_tree (targ_tree),
    m_targ_branchname (targ_branchname),
    m_targ_branch (nullptr),
    m_addrtmp (nullptr),
    m_notify_chain (targ_tree->GetNotify()),
    m_sgkey (tree->stringToKey (key, clid)),
    m_aux_branchname (aux_branchname),
    m_aux_branch (nullptr),
    m_dyn_store (nullptr),
    m_aux_offset (-1),
    m_holder_offset (-1),
    m_aux_branch_orig(nullptr),
    m_holder_offset_orig(-1),
    m_fixup(nullptr)
{
  fTree = tree;
  fMother = this;
  fParent = nullptr;

  const char* classname = alias_fetch_classname (targ_tree, targ_branchname);
  TClass* cl = TClass::GetClass (classname);
  if (!cl) {
    Error("TBranchAlias", "Cannot find class:%s", classname);
    return;
  }

  SetName (name);
  SetTitle (name);

  fAddress = (char*) &m_addrtmp;
  m_className = classname;

  TLeaf* leaf = new TLeafObject (this, name, classname);
  leaf->SetAddress (&m_addrtmp);
  fNleaves = 1;
  fLeaves.Add (leaf);
  tree->GetListOfLeaves()->Add(leaf);

  SetAutoDelete (kTRUE);
  fDirectory = tree->GetDirectory();
  fFileName = "";
  
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
  TBranch* br_orig = m_targ_tree->GetBranch (m_aux_branchname.c_str());
  if (!br_orig) return;
  TBranch* br = m_trans_tree->GetBranch (m_aux_branchname.c_str());
  // cppcheck-suppress incorrectLogicOperator
  if (!br || dynamic_cast<TBranchAlias*>(br) != 0)
    br = br_orig;

  fixupPackedConversion (br);

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

  m_aux_branch_orig = m_aux_branch;
  m_holder_offset_orig = m_holder_offset;
  if (br_orig) {
    TClass* cl_orig = TClass::GetClass (br_orig->GetClassName());
    if (cl_orig) {
      int offs = cl_orig->GetBaseClassOffset (auxcl);
      if (offs >= 0) {
        m_aux_branch_orig = br_orig;
        if (holder_cl)
          m_holder_offset_orig = cl_orig->GetBaseClassOffset (holder_cl);
      }
    }
  }

  setStore (false, this->GetReadEntry());
}


/**
 * @brief Set/clear the aux store for our object.
 * @param clear If true, clear the store; otherwise set it if possible.
 * @param entry Entry number being read.
 */
void TBranchAlias::setStore (bool clear, long long entry)
{
  // Create objects if needed.
  if (m_targ_branch->GetAddress() == 0)
    m_targ_branch->SetAddress(0);
  if (m_aux_branch && m_aux_branch->GetAddress() == 0)
    m_aux_branch->SetAddress(0);
  if (m_aux_branch_orig && m_aux_branch_orig->GetAddress() == 0)
    m_aux_branch_orig->SetAddress(0);

  // Update our cached address if needed.
  if (GetAddress() != m_targ_branch->GetAddress() &&
      m_targ_branch->GetAddress() != 0)
  {
    TBranch::SetAddress (m_targ_branch->GetAddress());
  }

  if (!m_aux_branch || !m_setStore_method.IsValid()) return;
  void* thisobj = *reinterpret_cast<void**> (this->GetAddress());
  if (!thisobj) return;

  if (m_dyn_store && clear) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch_orig->GetAddress());
    if (auxptr && *auxptr) {
      SG::IAuxStoreHolder* holder =
        reinterpret_cast<SG::IAuxStoreHolder*> (*auxptr + m_holder_offset_orig);
      holder->setStore (0);
    }
    m_dyn_store = 0;
  }

  void* auxobj = 0;
  if (!clear) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch->GetAddress());
    auxobj = *auxptr + m_aux_offset;
  }
  m_setStore_method.SetParamPtrs (&auxobj, 1);
  m_setStore_method.Execute (thisobj);

  if (!clear && m_holder_offset_orig >= 0 && !m_dyn_store) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch_orig->GetAddress());
    if (auxptr && *auxptr) {
      SG::IAuxStoreHolder* holder =
        reinterpret_cast<SG::IAuxStoreHolder*> (*auxptr + m_holder_offset_orig);
      bool standalone =
        holder->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore;
      m_dyn_store = new AuxStoreARA (*this,
                                     this->GetReadEntry(),
                                     standalone);
      holder->setStore (m_dyn_store);

      // If the aux branch is the result of a conversion, may need to rerun
      // the conversion.
      if (m_aux_branch)
        m_aux_branch->ResetReadEntry();
    }
  }

  if (m_holder_offset >= 0) {
    char** auxptr = reinterpret_cast<char**> (m_aux_branch->GetAddress());
    if (auxptr && *auxptr) {
      SG::IAuxStoreHolder* holder =
        reinterpret_cast<SG::IAuxStoreHolder*> (*auxptr + m_holder_offset);
      SG::IAuxStore* store = holder->getStore();
      if (store) store->clearDecorations();

      // cppcheck-suppress incorrectLogicOperator
      if (store && dynamic_cast<AuxStoreARA*> (store) == nullptr && entry >= 0 &&
          m_aux_branch == m_aux_branch_orig)
      {
        bool standalone =
          holder->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore;
        AuxStoreARA tmpstore (*this,
                              entry,
                              standalone);
        tmpstore.GetEntry (entry);
        for (SG::auxid_t id : tmpstore.getAuxIDs()) {
          tmpstore.getData(id);
        }

        SG::IConstAuxStore* auxobj =
          reinterpret_cast<SG::IConstAuxStore*> (*auxptr + m_aux_offset);
        mergeAuxStore (tmpstore, *static_cast<SG::IAuxStore*>(auxobj));
      }
    }
  }
}


const std::string& TBranchAlias::getName() const
{
  return m_targ_branchname;
}


std::vector<SG::IAuxBranches::auxpair_t> TBranchAlias::auxBranches() const
{
  std::vector<IAuxBranches::auxpair_t> out;
  TTree* tree = m_targ_tree;
  if (TTreeTrans* tt = dynamic_cast<TTreeTrans*> (tree))
    tree = tt->getPersTree();
  TIter bit (tree->GetListOfBranches());
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


TBranch* TBranchAlias::findAuxBranch (SG::auxid_t auxid)
{
  auto abr_it = m_dyn_branch_map.find(auxid);
  if( abr_it != m_dyn_branch_map.end() ) {
    return abr_it->second;
  }

  std::string attr_name = SG::AuxTypeRegistry::instance().getName(auxid);
  TBranch* branch = m_targ_tree->GetBranch( (m_dyn_branch_prefix + attr_name).c_str() );
  if (!branch) {
    TTreeTrans* trans = dynamic_cast<TTreeTrans*> (m_targ_tree);
    if (trans) {
      TTree* pers = trans->getPersTree();
      if (pers) branch = pers->GetBranch( (m_dyn_branch_prefix + attr_name).c_str() );
    }
  }
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


StatusCode TBranchAlias::readAuxBranch (TBranch&, void*, const std::type_info&, long long)
{
  std::abort();
}


/**
 * @brief Set a fixup to be run on this branch after GetEntry.
 * @param fixup The fixup to run.  (Ownership is not transferred.)
 */
void TBranchAlias::setFixup (IARAFixup* fixup)
{
  m_fixup = std::unique_ptr<IARAFixup> (fixup);
}

} // namespace AthenaROOTAccess
