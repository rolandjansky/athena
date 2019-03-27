/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file RootUtils/src/pyroot/PyROOTTypePatch.cxx
 * @author scott snyder
 * @date Mar 2019
 * @brief Work around pyroot problem with __pair_base.
 */

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "RootUtils/PyROOTTypePatch.h"
#include "Rtypes.h"
#include "TGenericClassInfo.h"
#include "TClass.h"
#include "TROOT.h"
#include "TError.h"
#include "TClassTable.h"
#include <ctype.h>
#include <cassert>


using ROOT::Internal::TInitBehavior;
using ROOT::Internal::TDefaultInitBehavior;
using ROOT::Internal::DefineBehavior;


//****************************************************************************
// This is the class creation hook.
//


namespace {


/**
 * @brief Class creation hook.
 */
class TypePatchInitBehavior
  : public TDefaultInitBehavior
{
public:
  virtual TClass *CreateClass(const char *cname, Version_t id,
                              const std::type_info &info, TVirtualIsAProxy *isa,
                              const char *dfil, const char *ifil,
                              Int_t dl, Int_t il) const;

  static TDefaultInitBehavior oldBehavior;
  static TDefaultInitBehavior* oldBehaviorPtr;
};
TDefaultInitBehavior  TypePatchInitBehavior::oldBehavior;
TDefaultInitBehavior* TypePatchInitBehavior::oldBehaviorPtr = 0;


/**
 * @brief Class creation hook.
 * We arrange for this to be called by root when a new class is created.
 */
TClass* TypePatchInitBehavior::CreateClass(const char *cname,
                                           Version_t id,
                                           const std::type_info &info,
                                           TVirtualIsAProxy *isa,
                                           const char *dfil,
                                           const char *ifil,
                                           Int_t dl,
                                           Int_t il) const
{
  // Do the default root behavior.
  TClass* cl = oldBehaviorPtr->CreateClass(cname, id, info, isa,
                                           dfil, ifil, dl, il);

  // Scan the new class.
  RootUtils::PyROOTTypePatch::scan_one (cl);

  // Return it.
  return cl;
}


} // anonymous namespace


//****************************************************************************
// Local helper functions.
//


namespace {


/**
 * @brief Test to see if a class name is that of a std::pair.
 * @param clsname The name to test.
 */
bool name_is_pair (const char* clsname)
{
  return (strncmp (clsname, "pair<", 5) == 0 || 
          strncmp (clsname, "std::pair<", 10) == 0);
}


/**
 * @brief Scan all known classes, and fix up any pairs.
 */
void scan_for_pair()
{
  TClassTable::Init();
  while (const char* clsname = TClassTable::Next()) {
    if (name_is_pair (clsname)) {
      TClass* cls = gROOT->GetClass (clsname, kFALSE);
      if (cls)
        cls->GetListOfBases()->Clear();
    }
  }
}


} // anonymous namespace


namespace RootUtils {


/**
 * @brief Initialize the workaround.
 * Scan all known classes, and also hook into class creation
 * so that we'll scan all future classes.
 */
void PyROOTTypePatch::initialize()
{
  // Return if we've already initialized.
  static bool initialized = false;
  if (initialized) return;
  initialized = true;

  // Scan all classes.
  scan_for_pair();

  // Register ourselves as a hook,
  // to be called in the future on class creation.
  TypePatchInitBehavior::oldBehaviorPtr = &TypePatchInitBehavior::oldBehavior;
  TInitBehavior* ib =
    const_cast<TInitBehavior*>(DefineBehavior(0,0));
  assert (sizeof(TDefaultInitBehavior) == sizeof (TypePatchInitBehavior));
  memcpy ((char*)&TypePatchInitBehavior::oldBehavior, (char*)ib,
          sizeof(TDefaultInitBehavior));
  static TypePatchInitBehavior sfaib;
  memcpy ((char*)ib, (char*)&sfaib, sizeof(TDefaultInitBehavior));

  // Make sure the TClass's for these are built.
  // Otherwise, CreateClass can get called while global dtors are running.
  gROOT->GetClass("TObjString");
  gROOT->GetClass("TTreeCache");
}


/**
 * @brief Scan a single class.
 * @param cls The class to scan.
 */
void PyROOTTypePatch::scan_one (TClass* cls)
{
  if (name_is_pair (cls->GetName())) {
    cls->GetListOfBases()->Clear();
  }
}


} // namespace RootUtils
