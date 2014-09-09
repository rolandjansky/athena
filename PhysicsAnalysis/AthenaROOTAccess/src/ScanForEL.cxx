/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ScanForEL.cxx,v 1.5 2008-05-24 04:03:15 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/ScanForEL.cxx
 * @author scott snyder
 * @date Oct 2007
 * @brief Set up streamer hooks for @c ElementLink classes.
 */


#include "AthenaROOTAccess/ScanForEL.h"
#include "AthenaROOTAccess/ELStreamer.h"
#include "Rtypes.h"
#include "TGenericClassInfo.h"
#include "TClass.h"
#include "TROOT.h"
#include "TClassTable.h"
#include <cstring>
#include <cassert>


namespace AthenaROOTAccess {


/**
 * @brief Test to see if a class name is an Element/DataLink.
 * @param s The name to test.
 */
bool has_el_name (const char* s)
{
  if (strncmp (s, "ElementLink<", 12) == 0)
    return true;
  if (strncmp (s, "ElementLinkVector<", 18) == 0)
    return true;
  if (strncmp (s, "DataLink<", 9) == 0)
    return true;
  return false;
}


/**
 * @brief Add a streamer to a class.
 * @param cl The class to which we should add the streamer.
 */
void add_el_streamer (TClass* cl)
{
  // Create the streamer.
  // This will implicitly add it to the class.
  // The class will then own the new streamer.
  new AthenaROOTAccess::ELStreamer (cl);
}


/**
 * @brief Helper class for setting the streamer for classes that are
 * created after our first scan.
 * See comment in @c SFELInitBehavior::CreateClass below.
 */
class TClassEL
  : public TClass
{
public:
  // Constructor.
  TClassEL (const char *name, Version_t cversion,
            const type_info &info, TVirtualIsAProxy *isa,
            ShowMembersFunc_t showmember,
            const char *dfil, const char *ifil,
            Int_t dl, Int_t il);

  // Called after the class is fully constructed.
  virtual void PostLoadCheck();
};


/**
 * @brief Constructor.
 */
TClassEL::TClassEL (const char *name, Version_t cversion,
                    const type_info &info, TVirtualIsAProxy *isa,
                    ShowMembersFunc_t showmember,
                    const char *dfil, const char *ifil,
                    Int_t dl, Int_t il)
  : TClass (name, cversion, info, isa,
            showmember, dfil, ifil, dl, il)
{
}


/**
 * @brief This is called by Root after the class is completely constructed.
 */
void TClassEL::PostLoadCheck()
{
  // Do the base class stuff.
  TClass::PostLoadCheck();

  // Install the streamer.
  add_el_streamer (this);
}


/**
 * @brief Class creation hook.
 */
class SFELInitBehavior
  : public ROOT::TDefaultInitBehavior
{
public:
  virtual TClass *CreateClass(const char *cname, Version_t id,
                              const type_info &info, TVirtualIsAProxy *isa,
                              ShowMembersFunc_t show,
                              const char *dfil, const char *ifil,
                              Int_t dl, Int_t il) const;

  static ROOT::TDefaultInitBehavior oldBehavior;
  static ROOT::TDefaultInitBehavior* oldBehaviorPtr;
};
ROOT::TDefaultInitBehavior SFELInitBehavior::oldBehavior;
ROOT::TDefaultInitBehavior* SFELInitBehavior::oldBehaviorPtr =
  &SFELInitBehavior::oldBehavior;


/**
 * @brief Class creation hook.
 * We arrange for this to be called by Root when a new class is created.
 *
 * If this is an EL class, then the class we create should
 * actually be of class @c TClassEL; this will take care of getting
 * the streamer set properly.
 * Note that it does not work to simply make a @c TClass here and
 * then change the streamer before returning: the function
 * (in @c TGenericClassInfo) that we get called from will immediately
 * overwrite the streamer.  So we have to defer setting the streamer
 * until the class is completely constructed.  We use
 * @c TClass::PostLoadCheck for this.
 */
TClass *SFELInitBehavior::CreateClass(const char *cname,
                                      Version_t id,
                                      const type_info &info,
                                      TVirtualIsAProxy *isa,
                                      ShowMembersFunc_t show,
                                      const char *dfil,
                                      const char *ifil,
                                      Int_t dl,
                                      Int_t il) const
{
  if (has_el_name (cname))
    return new TClassEL (cname, id, info, isa, show, dfil,
                         ifil, dl, il);

  // Otherwise, we just do the default Root thing.
  return oldBehaviorPtr->CreateClass (cname, id, info, isa, show, dfil,
                                      ifil, dl, il);
}


/**
 * @brief Initialize the streamers.
 * Scan all known classes, and also hook into class creation
 * so that we'll scan all future classes.
 */
void ScanForEL::initialize()
{
  // Return if we've already initialized.
  static bool initialized = false;
  if (initialized)
    return;
  initialized = true;

  // Scan all classes.
  TClassTable::Init();
  while (const char* clsname = TClassTable::Next()) {
    if (has_el_name (clsname)) {
      TClass* cls = gROOT->GetClass (clsname, kFALSE);
      if (cls)
        add_el_streamer (cls);
    }
  }

  // Register ourselves as a hook,
  // to be called in the future on class creation.
  ROOT::TInitBehavior* ib =
    const_cast<ROOT::TInitBehavior*>(ROOT::DefineBehavior(0,0));
  assert (sizeof(ROOT::TDefaultInitBehavior) == sizeof (SFELInitBehavior));
  memcpy ((char*)&SFELInitBehavior::oldBehavior, (char*)ib,
          sizeof(ROOT::TDefaultInitBehavior));
  static SFELInitBehavior sfelib;
  memcpy ((char*)ib, (char*)&sfelib, sizeof(ROOT::TDefaultInitBehavior));

  // Make sure the TClass's for these are built.
  // Otherwise, CreateClass can get called while global dtors are running.
  gROOT->GetClass("TObjString");
  gROOT->GetClass("TTreeCache");
}


} // namespace AthenaROOTAccess
