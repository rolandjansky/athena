/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ScanForAbstract.cxx,v 1.4 2008-05-22 00:18:29 ssnyder Exp $
/**
 * @file  RootUtils/src/ScanForAbstract.cxx
 * @author scott snyder
 * @date Oct 2007
 * @brief Work around Root problems with virtual bases.
 */

#include "RootUtils/ScanForAbstract.h"
#include "Rtypes.h"
#include "TGenericClassInfo.h"
#include "TClass.h"
#include "TRealData.h"
#include "TROOT.h"
#include "TError.h"
#include "TClassTable.h"
#include <ctype.h>
#include <cassert>


#if ROOT_VERSION_CODE < ROOT_VERSION(6,5,0)
using ROOT::TInitBehavior;
using ROOT::TDefaultInitBehavior;
using ROOT::DefineBehavior;
#else
using ROOT::Internal::TInitBehavior;
using ROOT::Internal::TDefaultInitBehavior;
using ROOT::Internal::DefineBehavior;
#endif


//****************************************************************************
// This is the class creation hook.
//


namespace {


/**
 * @brief Class creation hook.
 */
class SFAInitBehavior
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
TDefaultInitBehavior SFAInitBehavior::oldBehavior;
TDefaultInitBehavior* SFAInitBehavior::oldBehaviorPtr = 0;


/**
 * @brief Class creation hook.
 * We arrange for this to be called by root when a new class is created.
 */
TClass *SFAInitBehavior::CreateClass(const char *cname,
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
  RootUtils::ScanForAbstract::scan_one (cl);

  // Return it.
  return cl;
}


} // anonymous namespace


//****************************************************************************
// Local helper functions.
//


namespace {


// This holds information about the fRealData member of TClass.
// We also use this to prevent running initialize twice.
TRealData* s_RealData = 0;


/**
 * @brief Test to see if a class name looks abstract.
 * @param clsname The name to test.
 *
 * We consider a name `abstract' if it starts with `I' followed
 * by an uppercase letter.
 */
bool name_is_abstract (const char* clsname)
{
  return (clsname[0] == 'I' && !islower (clsname[1]));
}


/**
 * @brief Try to fix up a single class.
 * @param cls The class to patch.
 */
void maybe_fix_one (TClass* cls)
{
  // Test to see that it has the abstract property set,
  // and that it is has no data members.
  if ((cls->Property() & kIsAbstract) != 0 && cls->GetNdata() == 0) {
    // Ok, we'll patch it.  Make a pointer to the fRealData member of cls.
    TList** tmp = (TList**)((char*)cls + s_RealData->GetThisOffset());

    // If it's empty, plug it up with an empty list.
    if (!*tmp)
      *tmp = new TList;
  }
}


/**
 * @brief Scan all known classes, and fix up those that look to be abstract.
 */
void scan_for_abstract()
{
  TClassTable::Init();
  while (const char* clsname = TClassTable::Next()) {
    if (name_is_abstract (clsname)) {
      TClass* cls = gROOT->GetClass (clsname, kFALSE);
      if (cls)
        maybe_fix_one (cls);
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
void ScanForAbstract::initialize()
{
  // Return if we've already initialized.
  if (s_RealData != 0) return;

  TClassTable::Init();

  // Look up the information for TClass::fRealData.
  TClass* clcl = gROOT->GetClass ("TClass");
  s_RealData = clcl->GetRealData ("fRealData");
  if (s_RealData == 0) {
    ::Error ("ScanForAbstract", "Can't find TClass::fRealData");
    return;
  }

  // Scan all classes.
  scan_for_abstract();

  // Register ourselves as a hook,
  // to be called in the future on class creation.
  SFAInitBehavior::oldBehaviorPtr = &SFAInitBehavior::oldBehavior;
  TInitBehavior* ib =
    const_cast<TInitBehavior*>(DefineBehavior(0,0));
  assert (sizeof(TDefaultInitBehavior) == sizeof (SFAInitBehavior));
  memcpy ((char*)&SFAInitBehavior::oldBehavior, (char*)ib,
          sizeof(TDefaultInitBehavior));
  static SFAInitBehavior sfaib;
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
void ScanForAbstract::scan_one (TClass* cls)
{
  if (name_is_abstract (cls->GetName()))
    maybe_fix_one (cls);
}


} // namespace RootUtils
