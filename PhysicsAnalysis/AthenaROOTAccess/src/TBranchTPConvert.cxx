/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/src/TBranchTPConvert.cxx
 * @author scott snyder
 * @date Apr 2007
 * @brief Branch to run T/P conversions.
 */

#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"
#include "AthenaROOTAccess/TBranchTPConvert.h"
#include "AthenaROOTAccess/TTreeBranchMap.h"
#include "AthenaROOTAccess/ProxyMap.h"
#include "AthenaROOTAccess/TTreeTrans.h"
#include "AthenaROOTAccess/branchSeek.h"
#include "AthenaROOTAccess/addDVProxy.h"
#include "AthenaROOTAccess/tpcnvLoad.h"
#include "AuxStoreARA.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthAllocators/Arena.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
#include "RootUtils/TBranchElementClang.h"
#include "CxxUtils/no_sanitize_undefined.h"
#include "TROOT.h"
#include "TTree.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TError.h"
#include "TTree.h"
#include <stdexcept>


#if ! (ROOT_VERSION_CODE >= ROOT_VERSION(6,1,0) || (ROOT_VERSION_CODE>=ROOT_VERSION(5,34,22) && ROOT_VERSION_CODE<ROOT_VERSION(6,0,0)))
R__EXTERN TTree* gTree;
#endif


namespace {
// Helper to increment/decrement a value with exception protection.
struct Inc
{
  Inc (int& target) : m_target (target) { ++m_target; }
  ~Inc () { --m_target; }
  int& m_target;
};

bool is_p_class (const TClass* cl)
{
  const char* clname = cl->GetName();
  const char* p = clname + strlen(clname);
  while (p > clname && isdigit(p[-1]))
    --p;
  if (p - clname > 2 && p[-1] == 'p' && p[-2] == '_')
    return true;
  return false;
}
}



namespace AthenaROOTAccess {


/**
 * @brief Dummy constructor.
 *        Here only to prevent annoying warnings from root.
 */
TBranchTPConvert::TBranchTPConvert()
  : m_trans_tree (0),
    m_pers_tree (0),
    m_pers_tree_primary (0),
    m_pers_branch (0),
    m_obj_ptr (0),
    m_own_obj (false),
    m_own_cnv (false),
    m_cnv (0),
    m_msg (0, ""),
    m_class (0),
    m_arena (0),
    m_notify_chain (0),
    m_sgkey (0),
    m_duplicateExtConverterID (false),
    m_saw_err (false),
    m_browse_count (0),
    m_reading_entry (-1),
    m_trans_holder_offset (-1),
    m_pers_holder_offset (-1)
{
}


/**
 * @brief Destructor.
 */
TBranchTPConvert::~TBranchTPConvert()
{
  // Maybe delete our container object.
  if (m_own_obj) {
    void* obj = *(void**)(this->GetAddress());
    m_class->Destructor (obj);
  }

  // Maybe delete the converter instance.
  if (m_own_cnv)
    delete m_cnv;

  // Delete this branch's memory pool.
  delete m_arena;

  // Delete external converters.
  for (std::map<std::string, AthenaPoolTopLevelTPCnvBase*>::iterator it =
         m_extCnv.begin();
       it != m_extCnv.end();
       ++it)
  {
    delete it->second;
  }
}


/**
 * @brief Standard Root fill method.
 * @return The number of bytes written.
 *
 * This is a no-op; writing isn't supported.
 */
Int_t TBranchTPConvert::Fill()
{
  return 0;
}


/**
 * @brief Standard Root method to read a new entry.
 * @param entry The entry number to be read.
 * @param getall If true, ignore branch disable flags.
 * @return The number of bytes read.
 */
Int_t TBranchTPConvert::GetEntry(Long64_t entry, Int_t getall)
{
  m_saw_err = false;

  if (TestBit(kDoNotProcess))
    return 0;
  
  // Exit early if we're already at the correct entry.
  if (fReadEntry == entry)
    return 1;

  // Protect against recursion.
  if (m_reading_entry == entry)
    return 1;
  ReadingEntry re (this, entry);

  // Make our tree current, and tell it the requested entry number.
  TTreeTrans::Push save_tree (m_trans_tree);
  m_trans_tree->setEntry (entry);

  // Find the local entry number within the current persistent tree.
  Long64_t local_entry = m_pers_tree->LoadTree (entry);

  // Find the persistent branch, if we don't have it cached.
  if (!m_pers_branch)
    m_pers_branch = m_pers_tree->GetBranch (m_pers_branch_name.c_str());

  // Seek to the proper entry in the persistent branch.
  Int_t stat = branchSeek (m_pers_branch, local_entry);
  if (stat == NOREAD)
    stat = 1;

  if (stat > 0) {
    // Now get ready to run the converter.  First, clear our memory pool,
    // and make it current.
    m_arena->reset();
    SG::Arena::Push push (*m_arena);

    // Find the addresses of the transient and persistent objects.
    void* pers  = *(void**)(m_pers_branch->GetAddress());
    void* trans = *(void**)(this->GetAddress());

    // Handle aux stores.
    if (m_pers_holder_offset >= 0) {
      SG::IAuxStoreHolder* h =
        reinterpret_cast<SG::IAuxStoreHolder*>
        (reinterpret_cast<char*> (pers) + m_pers_holder_offset);
      AuxStoreARA* s = dynamic_cast<AuxStoreARA*> (h->getStore());
      if (s) {
        s->GetEntry (local_entry);
        s->clearDecorations();
      }
    }

    if (m_trans_holder_offset >= 0) {
      SG::IAuxStoreHolder* h =
        reinterpret_cast<SG::IAuxStoreHolder*>
        (reinterpret_cast<char*> (trans) + m_trans_holder_offset);
      bool standalone =
        h->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore;
      h->setStore (new SG::AuxStoreInternal (standalone));
    }

    bool cnvstat = true;

    // If this is a top-level converter, we need to do extra work
    // before calling it.
    AthenaPoolTopLevelTPCnvBase* tlcnv =
      dynamic_cast<AthenaPoolTopLevelTPCnvBase*> (m_cnv);
    if (tlcnv)
      cnvstat = handleTopLevel (tlcnv, pers, getall);

    // Call the converter.
    if (cnvstat) {
      try {
        m_cnv->persToTransUntyped (pers, trans, m_msg);
      }
      catch (const std::runtime_error& e) {
        if (strcmp (e.what(), "NULL pointer to McEventCollection !!") == 0) {
          m_saw_err = true;
        }
        else
          throw;
      }
    }
    else
      stat = 0;

    // Remember which entry we're on.
    fReadEntry = entry;
  }

  // Clear any cached data.
  m_trans_tree->resetBranch (m_sgkey);

  return stat;
}


/**
 * @brief Standard Root reset routine.
 *
 * We call this when the persistent tree has changed,
 * to invalidate caches.
 */
void TBranchTPConvert::Reset (Option_t* /*=""*/)
{
  m_pers_branch = 0;
  m_extmap.clear();
}


/**
 * @brief Standard Root notify routine.
 */
Bool_t TBranchTPConvert::Notify()
{
  Reset();

  if (m_pers_tree_primary) {
    // Find our new persistent tree.
    TDirectory* dir = m_pers_tree_primary->GetTree()->GetDirectory();
    m_pers_tree = dynamic_cast<TTree*> (dir->Get (m_pers_tree_name.c_str()));
    if (!m_pers_tree) {
      // This can happen for external trees.
      if (strncmp (m_pers_tree_name.c_str(), "POOLContainer_", 14) != 0) {
        ::Error ("TBranchTPConvert",
                 "Can't find aux pers tree %s", m_pers_tree_name.c_str());
      }
    }
  }

  // Propagate the notification.
  if (m_notify_chain)
    return m_notify_chain->Notify();

  return true;
}


/**
 * @brief Add a new TBranchTPConvert to a tree.
 * @param tree The transient tree.
 * @param name The name of the new branch.
 * @param key The SG key for the object we're managing.
 * @param clid The primary SG clid.
 * @param cnv The T/P converter instance.
 * @param owncnv If true, the branch should take ownership of the converter.
 * @param perstree The persistent tree.
 * @param perstree_primary The primary persistent tree (see above), or 0.
 * @param persbranchname The name of the persistent branch.
 * @param clsname The name of the branch's class (the transient type).
 *                If defaulted, it's taken from the converter.
 * @param obj The contained object instance.
 *            If defaulted, one will be created automatically.
 * @return The new branch, or 0 on failure.
 */
TBranchTPConvert*
TBranchTPConvert::addToTree (TTreeTrans* tree,
                             const char* name,
                             const char* key,
                             CLID clid,
                             ITPCnvBase* cnv,
                             bool owncnv,
                             TTree* perstree,
                             TTree* perstree_primary,
                             const char* persbranchname,
                             const char* clsname /*= 0*/,
                             void* obj /*= 0*/)
{
  // If the class name was defaulted, find it from the converter.
  if (clsname == 0) {
    const std::type_info& ti = cnv->transientTInfo();
    TClass* cl = gROOT->GetClass (ti);
    if (cl == 0) {
      ::Error ("TBranchTPConvert", "Can't find converter's transient class");
      return 0;
    }
    clsname = cl->GetName();
  }

#if ! (ROOT_VERSION_CODE >= ROOT_VERSION(6,1,0) || (ROOT_VERSION_CODE>=ROOT_VERSION(5,34,22) && ROOT_VERSION_CODE<ROOT_VERSION(6,0,0)))
  // Required for creating a new TBranch.
  gTree = tree;
#endif

  // May be needed by constructors.
  TTreeTrans::Push save_tree (tree);

  // Make the branch.
  TBranchTPConvert* br = new TBranchTPConvert (tree,
                                               name,
                                               key,
                                               clid,
                                               cnv,
                                               owncnv,
                                               perstree,
                                               perstree_primary,
                                               persbranchname,
                                               clsname,
                                               obj);

  // Add it to the tree.
  tree->addBranch (br, persbranchname);

  // If the class is a DataVector instantiation, we may need to set
  // up a collection proxy fix it.
  AthenaROOTAccess::addDVProxy::add (gROOT->GetClass (clsname));

  return br;
}


/**
 * @brief Add a new TBranchTPConvert to a tree.
 * @param tree The transient tree.
 * @param name The name of the new branch.
 * @param key The SG key for the object we're managing.
 * @param clid The primary SG clid.
 * @param cnvname The class name for the T/P converter.
 * @param perstree The persistent tree.
 * @param perstree_primary The primary persistent tree (see above), or 0.
 * @param persbranchname The name of the persistent branch.
 * @param clsname The name of the branch's class (the transient type).
 *                If defaulted, it's taken from the converter.
 * @param obj The contained object instance.
 *            If defaulted, one will be created automatically.
 * @return The new branch, or 0 on failure.
 */
TBranchTPConvert*
TBranchTPConvert::addToTree (TTreeTrans* tree,
                             const char* name,
                             const char* key,
                             CLID clid,
                             const char* cnvname,
                             TTree* perstree,
                             TTree* perstree_primary,
                             const char* persbranchname,
                             const char* clsname /*= 0*/,
                             void* obj /*= 0*/)
{
  // We need to create an instance of the converter, then chain
  // to the previous addToTree.

  // get the ITPCnvBase instance...
  ITPCnvBase* cnv = AthenaROOTAccess::TPCnv::Registry::instance()
    .load_tpcnv(std::string(cnvname));

  // Add to the tree.
  return addToTree (tree,
                    name,
                    key,
                    clid,
                    cnv,
                    false,   // We do NOT own the converter object.
                    perstree,
                    perstree_primary,
                    persbranchname,
                    clsname,
                    obj);
}


/**
 * @brief Add a new TBranchTPConvert to a tree.
 * @param tree The transient tree.
 * @param name The name of the new branch.
 * @param key The SG key for the object we're managing.
 * @param clid The primary SG clid.
 * @param perstree The persistent tree.
 * @param perstree_primary The primary persistent tree (see above), or 0.
 * @param persbranchname The name of the persistent branch.
 * @param clsname The name of the branch's class (the transient type).
 * @param pers_clsname The name of the persistent type.
 * @param obj The contained object instance.
 *            If defaulted, one will be created automatically.
 * @return The new branch, or 0 on failure.
 */
TBranchTPConvert*
TBranchTPConvert::addToTree (TTreeTrans* tree,
                             const char* name,
                             const char* key,
                             CLID clid,
                             TTree* perstree,
                             TTree* perstree_primary,
                             const char* persbranchname,
                             const char* clsname,
                             const char* pers_clsname,
                             void* obj /*= 0*/)
{
  // get the ITPCnvBase instance...
  ITPCnvBase* cnv = AthenaROOTAccess::TPCnv::Registry::instance()
    .p2t_cnv(std::string(pers_clsname));

  if (!cnv) {
    ::Error ("TBranchTPConvert",
             "Could not load T/P-cnv for persistent class [%s]", pers_clsname);
    return 0;
  }

  // Add to the tree.
  return addToTree (tree,
                    name,
                    key,
                    clid,
                    cnv,
                    false,   // We do NOT own the converter object.
                    perstree,
                    perstree_primary,
                    persbranchname,
                    clsname,
                    obj);
}


/**
 * @brief Return the persistent branch name for this branch.
 * (For debugging only.)
 */
const std::string& TBranchTPConvert::getPersBranchName() const
{
  return m_pers_branch_name;
}


/**
 * @brief Change the hashed SG key/clid for a branch.
 * @param sgkey The new hashed key/clid.
 *
 * See ISetSGKey.h for the rationale for this.
 */
void TBranchTPConvert::setSGKey (IStringPool::sgkey_t sgkey)
{
  m_sgkey = sgkey;
}


/**
 * @brief Constructor.
 * @param tree The transient tree.
 * @param name The name of the new branch.
 * @param key The SG key for the object we're managing.
 * @param clid The primary SG clid.
 * @param cnv The T/P converter instance.
 * @param owncnv If true, the branch should take ownership of the converter.
 * @param perstree The persistent tree.
 * @param perstree_primary The primary persistent tree (see above), or 0.
 * @param persbranchname The name of the persistent branch.
 * @param clsname The name of the branch's class (the transient type).
 * @param obj The contained object instance.
 *            If 0, one will be created automatically.
 */
TBranchTPConvert::TBranchTPConvert (TTreeTrans* tree,
                                    const char* name,
                                    const char* key,
                                    CLID clid,
                                    ITPCnvBase* cnv,
                                    bool owncnv,
                                    TTree* perstree,
                                    TTree* perstree_primary,
                                    const char* persbranchname,
                                    const char* clsname,
                                    void* obj)

  :
  // The signature of the TBranchObject ctor changed in 5.17.  Bleh.
  // The object address needs to be provided to the TBranchObject.
  // So we break out the object creation to addObjInit.
  TBranchObject (tree, name, clsname, addObjInit(clsname, obj, tree,
                                                 m_obj_ptr,
                                                 m_own_obj,
                                                 m_class),
                 100, -1),
    m_trans_tree (tree),
    m_pers_tree (perstree),
    m_pers_tree_name (perstree->GetName()),
    m_pers_tree_primary (perstree_primary),
    m_pers_branch_name (persbranchname),
    m_pers_branch (perstree->GetBranch (persbranchname)),
    m_own_cnv (owncnv),
    m_cnv (cnv),
    m_msg (0, name),
    m_arena (new SG::Arena (name)),
    m_notify_chain (0),
    m_key (key),
    m_sgkey (m_trans_tree->stringToKey (key, clid)),
    m_duplicateExtConverterID (false),
    m_saw_err (false),
    m_browse_count (0),
    m_reading_entry (-1)
{
  // Set up the notification hook.
  if (m_pers_tree_primary) {
    m_notify_chain = perstree_primary->GetNotify();
    perstree_primary->SetNotify (this);
  }
  else {
    m_notify_chain = perstree->GetNotify();
    perstree->SetNotify (this);
  }

  // Set up aux store handling.
  const TClass* holder_cl = TClass::GetClass ("SG::IAuxStoreHolder");
  m_trans_holder_offset = -1;
  m_pers_holder_offset = -1;
  if (holder_cl) {
    TClass* cl = TClass::GetClass (clsname);
    if (cl)
      m_trans_holder_offset = cl->GetBaseClassOffset (holder_cl);
  }
  if (holder_cl) {
    TClass* cl = TClass::GetClass (m_pers_branch->GetClassName());
    if (cl && !is_p_class (cl))
      m_pers_holder_offset = cl->GetBaseClassOffset (holder_cl);
  }
}


/**
 * @brief Constructor helper to create the object instance.
 * @param clsname The name of the object class.
 * @param obj Object pointer passed into the constructor.
 *            If 0, we'll create one.
 * @param transtree The tree we're being created in.
 * @param obj_ptr Reference to m_obj_ptr.
 * @param obj_own Reference to m_obj_own.
 * @param cls Reference to m_class.
 * @return Pointer-to-pointer to the object.
 *
 * Note: This is run at the start of object construction;
 * don't assume anything about the members.
 */
void* TBranchTPConvert::addObjInit (const char* clsname,
                                    void* obj,
                                    TTreeTrans* transtree,
                                    void*& obj_ptr,
                                    bool& own_obj,
                                    TClass*& cls)
{
  // Start with nothing.
  obj_ptr = nullptr;
  own_obj = false;
  cls = nullptr;

  // If an object pointer was supplied, use it; we don't own it.
  if (obj)
    obj_ptr = obj;
  else {
    // Otherwise, we need to create the object.
    // We own it in this case.
    cls = gROOT->GetClass (clsname);
    if (cls) {
      // Need to make sure that this tree is made current while
      // the object's constructor is running.
      TTreeTrans::Push save_tree (transtree);
      obj_ptr = cls->New();
      own_obj = true;
    }
  }

  return &obj_ptr;
}


/**
 * @brief Helper: Extract a field from a token.
 * @param tok The token, as a string.
 * @param field The name of the field for which to search.
 * @return The value of the field, or an empty string.
 */
std::string extract_from_token (const std::string& tok,
                                const std::string& field)
{
  std::string target = "[" + field + "=";
  std::string::size_type beg = tok.find (target);
  if (beg == std::string::npos) return "";
  beg += target.size();
  std::string::size_type end = tok.find ("]", beg);
  if (beg == std::string::npos) return "";
  return tok.substr (beg, end-beg);
}


/**
 * @brief Helper: Extract the OID portion of a token.
 * @param tok The token, as a string.
 * @return The OID part of the token, or -1.
 */
Long64_t extract_oid (const std::string& tok)
{
  std::string oid = extract_from_token (tok, "OID");
  std::string::size_type end = oid.find ("-");
  if (end != std::string::npos)
    oid.erase (0, end+1);
  // Note that OID is in hex!
  return strtoll (oid.c_str(), 0, 16);
}


// See comments in the function below.
namespace {
  class ExtHack: public AthenaPoolTopLevelTPCnvBase
  {
  public:
    void override_add (AthenaPoolTopLevelTPCnvBase *cnv)
    {
      for( convIdMap_t::const_iterator itr = m_convIdMap.begin();
           itr != m_convIdMap.end(); itr++ ) {
        cnv->addExtTPConverterForReading( itr->second );
      }
    }
  };
}


/**
 * @brief Do extra processing for `top-level' converters.
 * @param cnv The converter.
 * @param pers The persistent obj.
 * @param getall The getall flag to @c GetEntry.
 * @return True on success, false on failure.
 *
 * There is extra work we need to do for `top-level' converters.
 * In this case, the persistent object contains a list of pool
 * tokens, which we must retrieve using a method of the converter.
 * We then need to read the corresponding object for each token,
 * and register its address with the appropriate converter (which we
 * get from the `top-level' converter).
 */
// Disable ubsan due to ExtHack.
bool TBranchTPConvert::handleTopLevel NO_SANITIZE_UNDEFINED
   (AthenaPoolTopLevelTPCnvBase* cnv,
    void* pers,
    Int_t /*getall*/)
{
  // Get the token list from the object.  If it's empty, there's nothing to do.
  TPCnvTokenList_p1* tokens = cnv->getTokenListVarFrom (pers);
  if (!tokens || tokens->empty())
    return true;

  // Loop over the tokens.
  size_t sz = tokens->size();
  for (size_t i = 0; i < sz; i++) {
    const TPCnvToken_p1& tok = (*tokens)[i];

    // Get the container from the token.
    std::string cnt = extract_from_token (tok.token(), "CNT");
    if (cnt.empty()) {
      ::Error ("TBranchTPConvert",
               "Bad token syntax %s", tok.token().c_str());
      return false;
    }

    // Find the branch from the token.
    // We cache this to speed things up.
    TBranch* br = 0;
    extmap_t::iterator it = m_extmap.find (cnt);
    if (it != m_extmap.end())
      br = it->second;
    else {
      br = branchFromCnt (cnt);
      if (!br) {
        ::Error ("TBranchTPConvert",
                 "Can't find branch for %s", cnt.c_str());
        return false;
      }
      m_extmap[cnt] = br;
    }

    // Now find, from the `top-level' converter, the converter
    // with which we'll need to register the object address.
    TPObjRef::typeID_t typ (tok.converterID(), 0);
    ITPConverter* extcnv = cnv->converterForRef (TPObjRef (typ, 0));
    AthenaPoolTopLevelTPCnvBase* extcnvbase =
      dynamic_cast<AthenaPoolTopLevelTPCnvBase*> (extcnv);
    
    if (!extcnv) {
      std::string tok_guid = extract_from_token (tok.token(), "CLID");
      std::map<std::string, AthenaPoolTopLevelTPCnvBase*>::const_iterator it =
        m_extCnv.find (tok_guid);
      if (it != m_extCnv.end())
        extcnvbase = it->second;
      if (!extcnvbase) {
        ::Error ("TBranchTPConvert",
                 "%s Can't find ext converter for token %s id %d",
                 this->GetName(),
                 tok.token().c_str(), tok.converterID());
        return false;
      }
    }
    if (!extcnvbase) {
      ::Error ("TBranchTPConvert",
               "Wrong converter type for token %s id %d",
               tok.token().c_str(), tok.converterID());
      return false;
    }

    // Very ugly.
    // JetTagInfoTPCnv now has JetTagInfoCnv_tlp[1,2],
    // both with the same converter id.
    // When this happens, the TP infrastructure will end
    // up picking whichever converter was registered last ...
    // which, as Murphy says, will usually be the wrong one.
    // Here, however, we can find the correct one via the
    // guids.  So, if we see that there are duplicate IDs
    // in play, we just re-add the correct converters
    // each time.  However, we can't do that with the
    // public interface (they're `protected' against
    // duplicates), so we have to introduce a dummy derived
    // class in order to access protected members.
    if (m_duplicateExtConverterID)
    {
      ExtHack* eh = reinterpret_cast<ExtHack*> (extcnvbase);
      eh->override_add (cnv);
    }

    // Extract the OID from the token.
    // This is the entry in the branch of the object we want.
    Long64_t oid = extract_oid (tok.token());
    if (oid < 0) {
      ::Error ("TBranchTPConvert",
               "Error interpreting OID in %s",
               tok.token().c_str());
      return false;
    }

    // ??? Should skip this entirely if there are no redirects?
    std::string dbid = extract_from_token (tok.token(), "DB");
    oid += m_trans_tree->getRedirect (dbid, cnt);

    // Seek to the proper event.
    if (br->GetReadEntry() != oid) {
      Int_t stat = branchSeek (br, oid);
      if (stat == NOREAD)
        stat = 1;
      if (stat < 0) {
        ::Error ("TBranchTPConvert",
                 "Can't read ext branch %s", br->GetName());
        return false;
      }
    }

    // Register the object with the converter.
    // Note: the converter takes ownership!
    // So we steal the object from the branch.
    TBranchElement* bre = dynamic_cast<TBranchElement*> (br);
    if (!bre) std::abort();
    void* obj = bre->GetObject();
    bre->ResetDeleteObject();
    bre->SetAddress(0);
    extcnvbase->setTLPersObject (obj);
  }

  return true;
}


/**
 * @brief Given a container from a pool token, find the corresponding branch.
 * @param cnt The container string.
 * @return The branch, or 0.
 */
TBranch* TBranchTPConvert::branchFromCnt (const std::string& cnt)
{
  // Split up the string into the tree/branch names.
  std::string tree_name;
  std::string branch_name;
  if (!ProxyMap::parseToken (cnt, m_key, tree_name, branch_name))
    return 0;

  // Find the tree in the same directory as the persistent tree.
  TTree* tree = m_pers_tree->GetTree();
  if (tree_name != tree->GetName()) {
    tree = dynamic_cast<TTree*>(tree->GetTree()->
                                GetDirectory()->Get (tree_name.c_str()));
    if (!tree) {
      ::Error ("TBranchTPConvert",
               "Can't find tree %s", tree_name.c_str());
      return 0;
    }
  }

  // Find the branch in the tree.
  TBranch* br = tree->GetBranch (branch_name.c_str());
  if (br) return br;
  TIter bit (tree->GetListOfBranches());
  while (TObject* bobj = bit()) {
    const char* bname = bobj->GetName();
    const char* pos = strstr (bname, "__");
    if (pos && strcmp (pos+2, branch_name.c_str()) == 0)
      return dynamic_cast<TBranch*> (bobj);
  }
  return 0;
}


/**
 * @brief Register an external TL converter for this branch.
 * @param guid The guid for this converter.
 * @param cnv The converter instance.
 *
 * The branch takes ownership of the converter instance.
 */
void TBranchTPConvert::addExtCnv (const std::string& guid,
                                  AthenaPoolTopLevelTPCnvBase* cnv)
{
  if (!m_duplicateExtConverterID) {
    for (std::map<std::string, AthenaPoolTopLevelTPCnvBase*>::iterator it =
           m_extCnv.begin();
         it != m_extCnv.end();
         ++it)
    {
      if (it->second->getConverterID() == cnv->getConverterID()) {
        m_duplicateExtConverterID = true;
        break;
      }
    }
  }
  if (AthenaPoolTopLevelTPCnvBase* c2 =
      dynamic_cast<AthenaPoolTopLevelTPCnvBase*> (m_cnv))
    cnv->addTPConvertersForReadingTo (c2);
  m_extCnv[guid] = cnv;
}


/**
 * @brief Standard ROOT @c IsA method.
 *
 * This is needed to be able to get these objects back from a @c TTree
 * in PyROOT.  (But also see @c GetBrowsables below.)
 */
TClass* TBranchTPConvert::IsA() const
{
  // Return the base class IsA if a browse is active.
  if (m_browse_count)
    return TBranchObject::IsA();

  static TClass* cls = 0;
  if (!cls)
    cls = gROOT->GetClass ("AthenaROOTAccess::TBranchTPConvert");
  return cls;
}


/**
 * @brief Standard ROOT @c GetBrowsables method.
 *
 * The reason we need to override this is that when ROOT puts
 * class members in the browser, it doesn't recognize types
 * that derive from TBranchObject --- IsA for the branch needs
 * to return the TBranchObject TClass.  However, in order for
 * a TBranchTPConvert to show up as such in PyROOT, IsA needs
 * to return the TBranchTPConvert TClass.  So, we have IsA return
 * the derived class --- except when a call to GetBrowsables
 * is active.  In that case, we return the base TClass instead.
 */
TList *TBranchTPConvert::GetBrowsables()
{
  Inc inc (m_browse_count);
  return TBranchObject::GetBrowsables ();
}


/**
 * @brief Return true if the last conversion had a recoverable error.
 */
bool TBranchTPConvert::sawErr() const
{
  return m_saw_err;
}


} // namespace AthenaROOTAccess
