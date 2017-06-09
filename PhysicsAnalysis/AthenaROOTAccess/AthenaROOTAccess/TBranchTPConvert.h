// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TBranchTPConvert.h,v 1.24 2008-12-23 05:01:34 ssnyder Exp $

/**
 * @file AthenaROOTAccess/TBranchTPConvert.h
 * @author scott snyder
 * @date Apr 2007
 * @brief Branch to run T/P conversions.
 */

#ifndef ATHENAROOTACCESS_TBRANCHTPCONVERT_H
#define ATHENAROOTACCESS_TBRANCHTPCONVERT_H

#include "AthenaROOTAccess/ISetSGKey.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/IStringPool.h"
#include "TBranchObject.h"
#include <map>
#include <string>

class ITPCnvBase;
class AthenaPoolTopLevelTPCnvBase;
namespace SG {
class DataProxy;
class Arena;
}

namespace AthenaROOTAccess {


class TTreeTrans;


/**
 * @brief Branch to run T/P conversions.
 *
 * A TBranchTPConvert branch has the type of the transient object;
 * this is the instance which it buffers internally.  It also has
 * a reference to the branch containing the persistent object.
 * When @c GetEntry is called, TBranchTPConvert first forwards
 * the call to the persistent branch.  It then runs the P->T converter,
 * going from the persistent object to the transient object.
 *
 * There is further complication to handle `top-level' converters.
 * See comments in the source file.
 *
 * Each instance of this class maintains its own memory allocation arena.
 * This is set as the default arena while the converter is running.
 * The arena is cleared when moving to a new entry.
 *
 * Some objects in the pool file live outside the primary CollectionTree,
 * in separate tree objects.  These objects may also need to have
 * P->T conversions run.  In such a case, we build a new TTreeTrans
 * object for each of these external persistent trees; these are
 * called `aux' trees.  They are collected in a special directory,
 * so that they can be found.  A TBranchTPConvert in an `aux' tree
 * has a pointer to the `primary' persistent tree; the presence of this
 * pointer is what identifies a branch in such a tree.
 *
 * Instances of this object should not be created directly.
 * Use one of the @c addToTree() methods instead.
 */
class TBranchTPConvert
  : public TBranchObject,
    public ISetSGKey
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~TBranchTPConvert();


  /**
   * @brief Standard Root fill method.
   * @return The number of bytes written.
   *
   * This is a no-op; writing isn't supported.
   */
  virtual Int_t Fill();


  /**
   * @brief Standard Root method to read a new entry.
   * @param entry The entry number to be read.
   * @param getall If true, ignore branch disable flags.
   * @return The number of bytes read.
   */
  virtual Int_t GetEntry(Long64_t entry, Int_t getall = 0);


  /**
   * @brief Standard Root reset routine.
   *
   * We call this when the persistent tree has changed,
   * to invalidate caches.
   */
  void Reset (Option_t* = "");


  /**
   * @brief Standard Root notify routine.
   */
  virtual Bool_t      Notify();


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
  static TBranchTPConvert* addToTree (TTreeTrans* tree,
                                      const char* name,
                                      const char* key,
                                      CLID clid,
                                      ITPCnvBase* cnv,
                                      bool owncnv,
                                      TTree* perstree,
                                      TTree* perstree_primary,
                                      const char* persbranchname,
                                      const char* clsname = 0,
                                      void* obj = 0);


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
  static TBranchTPConvert* addToTree (TTreeTrans* tree,
                                      const char* name,
                                      const char* key,
                                      CLID clid,
                                      const char* cnvname,
                                      TTree* perstree,
                                      TTree* perstree_primary,
                                      const char* persbranchname,
                                      const char* clsname = 0,
                                      void* obj = 0);


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
   * @param obj The contained object instance.
   *            If defaulted, one will be created automatically.
   * @param pers_clsname The name of the persistent type.
   * @return The new branch, or 0 on failure.
   */
  static TBranchTPConvert* addToTree (TTreeTrans* tree,
                                      const char* name,
                                      const char* key,
                                      CLID clid,
                                      TTree* perstree,
                                      TTree* perstree_primary,
                                      const char* persbranchname,
                                      const char* clsname,
                                      const char* pers_clsname,
                                      void* obj = 0);


  /**
   * @brief Return the persistent branch name for this branch.
   * (For debugging only.)
   */
  const std::string& getPersBranchName() const;


  /**
   * @brief Change the hashed SG key/clid for a branch.
   * @param sgkey The new hashed key/clid.
   *
   * See ISetSGKey.h for the rationale for this.
   */
  virtual void setSGKey (IStringPool::sgkey_t sgkey);


  /**
   * @brief Register an external TL converter for this branch.
   * @param guid The guid for this converter.
   * @param cnv The converter instance.
   *
   * The branch takes ownership of the converter instance.
   */
  void addExtCnv (const std::string& guid,
                  AthenaPoolTopLevelTPCnvBase* cnv);


  // Declared via ClassDef() below.
  /**
   * @brief Standard ROOT @c IsA method.
   *
   * This is needed to be able to get these objects back from a @c TTree
   * in PyROOT.  (But also see @c GetBrowsables below.)
   */
  virtual TClass *IsA() const;


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
  virtual TList *GetBrowsables();


  /**
   * @brief Return true if the last conversion had a recoverable error.
   */
  virtual bool sawErr() const;


  /**
   * @brief Dummy constructor.
   *        Here only to prevent annoying warnings from root.
   */
  TBranchTPConvert();



  // ClassDef includes a definition for IsA().
  // But we want to use a different definition.
#define IsA root_IsA
  ClassDef (AthenaROOTAccess::TBranchTPConvert, 1);
#undef IsA


private:
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
  TBranchTPConvert (TTreeTrans* tree,
                    const char* name,
                    const char* key,
                    CLID clid,
                    ITPCnvBase* cnv,
                    bool owncnv,
                    TTree* perstree,
                    TTree* perstree_primary,
                    const char* persbranchname,
                    const char* clsname,
                    void* obj);


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
  static
  void* addObjInit (const char* clsname,
                    void* obj,
                    TTreeTrans* transtree,
                    void*& obj_ptr,
                    bool& own_obj,
                    TClass*& cls);


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
  bool handleTopLevel (AthenaPoolTopLevelTPCnvBase* cnv,
                       void* pers,
                       Int_t getall);


  /**
   * @brief Given a container from a pool token, find the corresponding branch.
   * @param cnt The container string.
   * @return The branch, or 0.
   */
  TBranch* branchFromCnt (const std::string& cnt);


  /// The transient tree holding this branch.
  TTreeTrans* m_trans_tree;

  /// The persistent tree.
  TTree* m_pers_tree;

  /// The name of the persistent tree.
  std::string m_pers_tree_name;

  /// @brief If this branch is for an external tree,
  /// the primary persistent tree.
  /// 0 if this branch is not for an external tree.
  TTree* m_pers_tree_primary;

  /// Name of the persistent branch.
  std::string m_pers_branch_name;

  /// The persistent branch.
  TBranch* m_pers_branch;

  /// Pointer-to-pointer to the transient object.
  void* m_obj_ptr;

  /// True if we own the transient object.
  bool m_own_obj;

  /// True if we own the converter.
  bool m_own_cnv;

  /// The converter instance.
  ITPCnvBase* m_cnv;

  /// Message stream to pass to the converter.
  MsgStream m_msg;

  /// Class of the transient object.
  TClass* m_class;

  /// Memory allocation pool.
  SG::Arena* m_arena;

  /// For propagating notification requests.
  TObject* m_notify_chain;

  /// The StoreGate key for this object.
  std::string m_key;

  /// For top-level conversions: map from container strings to branches.
  typedef std::map<std::string, TBranch*> extmap_t;
  /// For top-level conversions: map from container strings to branches.
  extmap_t m_extmap;

  /// Hashed SG key/clid for this branch.
  IStringPool::sgkey_t m_sgkey;

  /// Map from GUIDs to external TL converters.
  std::map<std::string, AthenaPoolTopLevelTPCnvBase*> m_extCnv;

  /// Flag if we have two external TL converters with the same ID.
  bool m_duplicateExtConverterID;

  /// Flag if the last conversion had a recoverable error.
  bool m_saw_err;

  /// Count of nested browse invocations.
  int m_browse_count;

  /// Protection against recursion.
  Long64_t m_reading_entry;
  struct ReadingEntry {
    TBranchTPConvert* m_br;
    Long64_t m_old_entry;
    ReadingEntry (TBranchTPConvert* br, Long64_t entry)
      : m_br (br), m_old_entry (br->m_reading_entry)
    {
      if (br->m_reading_entry != entry && br->m_reading_entry != -1)
        std::abort();
      br->m_reading_entry = entry;
    }
    ~ReadingEntry()
    {
      m_br->m_reading_entry = m_old_entry;
    }
  };
  friend struct ReadingEntry;

  /// Aux store handling.
  int m_trans_holder_offset;
  int m_pers_holder_offset;

  // Disallow copying.
  TBranchTPConvert& operator= (const TBranchTPConvert&);
  TBranchTPConvert (const TBranchTPConvert&);
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_TBRANCHTPCONVERT_H

