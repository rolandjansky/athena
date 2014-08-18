/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelAthenaPool/src/ElementLinkStreamer.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Set up a streamer for @c ElementLinkBase to implement T->P conversion.
 */


#include "ElementLinkStreamer.h"
#include "AthLinks/ElementLinkBase.h"
#include "CxxUtils/override.h"
#include "TClassStreamer.h"
#include "TBuffer.h"
#include "TClass.h"
#include "TROOT.h"
#include <cstring>
#include <cassert>


namespace DataModelAthenaPool {


/**
 * @brief Streamer for @c ElementLinkBase.
 *
 * This streamer is used to implement what is essentially
 * a transient->persistent conversion for ElementLinkBase.
 * For reading, we simply use the @c TStreamerInfo machinery.
 * For writing, we make a temporary copy of the object,
 * call @c thin() on it, and then call to the @c TStreamerInfo machinery.
 */
class ElementLinkBaseStreamer
  : public TClassStreamer
{
public:
  /// Constructor.
  ElementLinkBaseStreamer (TClass* cl);

  /// Run streamer.
  virtual void operator() (TBuffer& b, void* objp) ATH_OVERRIDE;

private:
  /// The class that this streamer is for (@c ElementLinkBase).
  TClass* m_class;
};


/**
 * @brief Constructor.
 * @param cl The class object for @c ElementLinkBase.
 */
ElementLinkBaseStreamer::ElementLinkBaseStreamer (TClass* cl)
  : m_class (cl)
{
}


/**
 * @brief Run streamer.
 * @param b TBuffer from/into which we're streaming.
 * @param objp Pointer to the object instance.
 */
void ElementLinkBaseStreamer::operator() (TBuffer& b, void* objp)
{
  if (b.IsReading()) {
    // Reading: just call @c ReadBuffer, which does the @c TStreamerInfo thing.
    m_class->ReadBuffer(b, objp);
  }
  else {
    // Writing: make a copy call thin() on it, which does toPersistent(),
    // remapping, and thinning.
    ElementLinkBase obj = *reinterpret_cast<ElementLinkBase*> (objp);
    obj.thin();

    // Write the copy using the @c TStreamerInfo machinery.
    m_class->WriteBuffer (b, &obj);
  }
}


/**
 * @brief Class creation hook.
 */
class ELInitBehavior
  : public ROOT::TDefaultInitBehavior
{
public:
  virtual TClass *CreateClass(const char *cname, Version_t id,
                              const type_info &info, TVirtualIsAProxy *isa,
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
                              ShowMembersFunc_t show,
#endif
                              const char *dfil, const char *ifil,
                              Int_t dl, Int_t il) const;

  static ROOT::TDefaultInitBehavior oldBehavior;
  static ROOT::TDefaultInitBehavior* oldBehaviorPtr;
};
ROOT::TDefaultInitBehavior ELInitBehavior::oldBehavior;
ROOT::TDefaultInitBehavior* ELInitBehavior::oldBehaviorPtr = 0;


/**
 * @brief Class creation hook.
 * We arrange for this to be called by root when a new class is created.
 */
TClass *ELInitBehavior::CreateClass(const char *cname,
                                    Version_t id,
                                    const type_info &info,
                                    TVirtualIsAProxy *isa,
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
                                    ShowMembersFunc_t show,
#endif
                                    const char *dfil,
                                    const char *ifil,
                                    Int_t dl,
                                    Int_t il) const
{
  // Do the default root behavior.
  TClass* cl = oldBehaviorPtr->CreateClass(cname, id, info, isa,
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
                                           show,
#endif
                                           dfil, ifil, dl, il);

  // Scan the new class.
  if (strncmp (cname, "ElementLink<", 12) == 0)
    cl->SetCanSplit (kFALSE);

  // Return it.
  return cl;
}


/**
 * @brief Set up a streamer for @c ElementLinkBase to implement T->P conversion.
 */
void installElementLinkBaseStreamer()
{
  // Install the streamer for @c ElementLinkBase, and prohibit splitting.
  TClass* cl = TClass::GetClass ("ElementLinkBase");
  if (cl) {
    cl->SetCanSplit (kFALSE);
    cl->AdoptStreamer (new ElementLinkBaseStreamer (cl));
  }

  // We also need to prohibit splitting for ElementLink<> classes,
  // or we hit a root bug in TBranchElement::InitializeOffsets.
  TIter next (gROOT->GetListOfClasses());
  while (TClass* cls = dynamic_cast<TClass*> (next())) {
    if (std::strncmp (cls->GetName(), "ElementLink<", 12) == 0) {
      cls->SetCanSplit (kFALSE);
    }
  }

  // That took care of all ElementLink<> classes created so far.
  // Set up a class creation hook so that this will get done for any
  // ElementLink<> classes created in the future.
  ELInitBehavior::oldBehaviorPtr = &ELInitBehavior::oldBehavior;
  ROOT::TInitBehavior* ib =
    const_cast<ROOT::TInitBehavior*>(ROOT::DefineBehavior(0,0));
  assert (sizeof(ROOT::TDefaultInitBehavior) == sizeof (ELInitBehavior));
  memcpy ((char*)&ELInitBehavior::oldBehavior, (char*)ib,
          sizeof(ROOT::TDefaultInitBehavior));
  static ELInitBehavior sfaib;
  memcpy ((char*)ib, (char*)&sfaib, sizeof(ROOT::TDefaultInitBehavior));

  // Make sure the TClass's for these are built.
  // Otherwise, CreateClass can get called while global dtors are running.
  gROOT->GetClass("TObjString");
  gROOT->GetClass("TTreeCache");
}


} // namespace DataModelAthenaPool
