/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TVirtualConverter.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2006
 * @brief Base class for Root converters --- implementation.
 */


#include "RootConversions/TVirtualConverter.h"
#include "TROOT.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TDataMember.h"
#include "RootUtils/TClassEditRootUtils.h"
#include "TDataType.h"
#include "TBuffer.h"
#include "TError.h"
#include "TStreamerInfo.h"
#include <cassert>
#include <cstdlib>


namespace {


/**
 * @brief Calculate the Root checksum for a class, changing the name.
 * @param cl      Class of which the checksum is to be calculated.
 * @param name_in The class name to use for the checksum calculation.
 *
 * This is a slightly modified version of the checksum routine
 * from @c TClass.  It returns the checksum that Root would calculate
 * for a class that has name @c name_in an a body equivalent to @c cl.
 */
UInt_t CalculateCheckSum (TClass* cl, const char* name_in)
{
  UInt_t id = 0;
  int il;
  TString name = name_in;
  TString type;
  il = name.Length();
  for (int i=0; i<il; i++) id = id*3+name[i];

  TList *tlb = cl->GetListOfBases();
  if (tlb) {   // Loop over bases

    TIter nextBase(tlb);

    TBaseClass *tbc=0;
    while((tbc=(TBaseClass*)nextBase())) {
      name = tbc->GetName();
      il = name.Length();
      for (int i=0; i<il; i++) id = id*3+name[i];
    }/*EndBaseLoop*/
  }
  TList *tlm = cl->GetListOfDataMembers();
  if (tlm) {   // Loop over members
    TIter nextMemb(tlm);
    TDataMember *tdm=0;
    Long_t prop = 0;
    while((tdm=(TDataMember*)nextMemb())) {
      if (!tdm->IsPersistent())        continue;
      //  combine properties
      prop = (tdm->Property());
      TDataType* tdt = tdm->GetDataType();
      if (tdt) prop |= tdt->Property();

      if ( prop&kIsStatic)             continue;
      name = tdm->GetName(); il = name.Length();
      if (prop&kIsEnum) id = id*3 + 1;

      int i;
      for (i=0; i<il; i++) id = id*3+name[i];
      type = tdm->GetFullTypeName();
      if (TClassEdit::IsSTLCont(type) != ROOT::kNotSTL)
        type = TClassEdit::ShortType( type, TClassEdit::kDropStlDefault );

      il = type.Length();
      for (i=0; i<il; i++) id = id*3+type[i];

      int dim = tdm->GetArrayDim();
      if (prop&kIsArray) {
        for (int i=0;i<dim;i++) id = id*3+tdm->GetMaxIndex(i);
      }

    }/*EndMembLoop*/
  }
  return id;
}


} // anonymous namespace


/**
 * @brief Constructor, with checksum and class pointers.
 * @param checksum   The Root checksum of the persistent class we'll convert.
 * @param trans_cls  The transient class to which we will be converting.
 * @param pers_cls   The persistent class from which we will be converting.
 */
TVirtualConverter::TVirtualConverter (UInt_t checksum,
                                      TClass* trans_cls,
                                      TClass* pers_cls)
  : fTransClass (trans_cls),
    fPersClass (pers_cls),
    fCheckSum (checksum),
    fOldLast (-3),
    fPersObj (0)
{
  CheckClasses();
}


/**
 * @brief Constructor, with class pointers.
 * @param trans_cls  The transient class to which we will be converting.
 * @param pers_cls   The persistent class from which we will be converting.
 *
 * The checksum will be calculated automatically from the provided classes.
 */
TVirtualConverter::TVirtualConverter (TClass* trans_cls,
                                      TClass* pers_cls)
  : fTransClass (trans_cls),
    fPersClass (pers_cls),
    fCheckSum (fTransClass ?
                 CalculateCheckSum (fPersClass, fTransClass->GetName()) : 0),
    fOldLast (-3),
    fPersObj (0)
{
  CheckClasses();
}


/**
 * @brief Constructor, with checksum and class names.
 * @param checksum   The Root checksum of the persistent class we'll convert.
 * @param trans_cls  The name of the transient class to which
 *                   we will be converting.
 * @param pers_cls   The name of the persistent class from which
 *                   we will be converting.
 */
TVirtualConverter::TVirtualConverter (UInt_t checksum,
                                      const std::string& trans_cls,
                                      const std::string& pers_cls)
  : fTransClass (ToClass (trans_cls)),
    fPersClass (ToClass (pers_cls)),
    fCheckSum (checksum),
    fOldLast (-3),
    fPersObj (0)
{
  CheckClasses();
}


/**
 * @brief Constructor, with class names.
 * @param trans_cls  The name of the transient class to which
 *                   we will be converting.
 * @param pers_cls   The name of the persistent class from which
 *                   we will be converting.
 *
 * The checksum will be calculated automatically from the provided classes.
 */
TVirtualConverter::TVirtualConverter (const std::string& trans_cls,
                                      const std::string& pers_cls)
  : fTransClass (ToClass (trans_cls)),
    fPersClass (ToClass (pers_cls)),
    fCheckSum (CalculateCheckSum (fPersClass, fTransClass->GetName())),
    fOldLast (-3),
    fPersObj (0)
{
  CheckClasses();
}


/**
 * @brief Check that the class pointers got defined.
 */
void TVirtualConverter::CheckClasses()
{
  if (fTransClass == 0) {
    if (fPersClass == 0)
      Error ("TVirtualConverter",
             "Both transient and persistent classes are null.");
    else
      Error ("TVirtualConverter",
             "Transient class is null for persistent class `%s'",
             fPersClass->GetName());
  }
  else if (fPersClass == 0)
    Error ("TVirtualConverter",
           "Persistent class is null for transient class `%s'",
           fTransClass->GetName());
}


/**
 * @brief Destructor.
 */
TVirtualConverter::~TVirtualConverter()
{
  // If we made an instance of the persistent class, get rid of it here.
  if (fPersObj)
    DeletePersObj (fPersObj);
}


/**
 * @brief Return the transient class, to which we're converting.
 */
TClass* TVirtualConverter::GetTransClass()
{
  return fTransClass;
}


/**
 * @brief Return the persistent class, from which we're converting.
 */
TClass* TVirtualConverter::GetPersClass()
{
  return fPersClass;
}


/**
 * @brief Return the checksum of the class from which we're converting.
 */
int TVirtualConverter::GetCheckSum() const
{
  return fCheckSum;
}


/**
 * @brief Create a new instance of the persistent class.
 *
 * The provided implementation of this uses @c TClass::New, but
 * you can override it if need be.
 */
void* TVirtualConverter::NewPersObj ()
{
  return fPersClass->New();
}


/**
 * @brief Destroy an instance of the persistent class.
 * @param persobj Pointer to the instance to destroy.
 *
 * The provided implementation of this uses @c TClass::Destructor, but
 * you can override it if need be.
 */
void TVirtualConverter::DeletePersObj (void* persobj)
{
  return fPersClass->Destructor (persobj);
}


/**
 * @brief Read object in non-split mode.
 * @param b       The source of the data.
 * @param obj     The object into which we're reading.
 *                (This will be the transient object.)
 * @param version The version number of the class.
 * @param R__s    The starting position in the buffer.
 * @param R__c    The number of bytes for this object in the buffer.
 *
 * You normally shouldn't have to worry about this, but it's exposed
 * here so that it can be overridden if needed.
 * It's only used when reading in non-split mode.
 */
void TVirtualConverter::ReadBuffer(TBuffer& b,
                                   void* obj,
                                   Version_t version,
                                   UInt_t R__s,
                                   UInt_t R__c)
{
  // Make sure that we've done automatic schema evolution on the
  // persistent side.
  CheckStreamInfos();

  // Make an instance of the persistent class, if we haven't done so already.
  if (fPersObj == 0)
    fPersObj = NewPersObj();

  // Read in the object, as an instance of the persistent class.
  fPersClass->ReadBuffer (b, fPersObj, version, R__s, R__c);

  // Convert to the transient representation.
  ConvertVoid (obj, fPersObj);
}


/**
 * @brief Helper to convert a class name to a Root class pointer.
 * @param name  The name of the class to convert.
 *
 * Returns 0 on failure.
 */
TClass* TVirtualConverter::ToClass (const std::string& name)
{
  return gROOT->GetClass (name.c_str());
}


/**
 * @brief Helper to convert a @c type_info to a Root class pointer.
 * @param name  The @c type_info of the class to convert.
 *
 * Returns 0 on failure.
 */
TClass* TVirtualConverter::ToClass (const std::type_info& id)
{
  TClass* cl = gROOT->GetClass (id);
  if (cl) return cl;

  // This may fail if the dictionary hasn't been loaded yet,
  // since we can auto-load only on the name.
  // Make some attempt to get a name from the type_info.
  const char* name = id.name();
  cl = gROOT->GetClass (name);
  if (cl) return cl;

  const char* n2 = name;
  while (isdigit(*n2))
    ++n2;
  cl = gROOT->GetClass (n2);
  if (cl) return cl;

  int errorCode = 0;
  char* demname = TClassEdit::DemangleTypeIdName (id, errorCode);
  if (demname) {
    cl = gROOT->GetClass (demname);
    free (demname);
  }
  return cl;
}


/**
 * @brief Helper to allow for automatic schema evolution
 *        for persistent classes.
 *
 * Automatic schema evolution doesn't work for persistent classes
 * `out-of-the-box' because we're reading the `persistent class',
 * but the relevant StreamerInfo stored in the file will have the
 * name of the `transient class'.
 *
 * This gets called each time we try to read an object (in non-split mode).
 */
void TVirtualConverter::CheckStreamInfos()
{
  // First check to see if the list of TStreamerInfo's for the transient
  // class has changed since the last time we were called.  If not,
  // then don't do anything.
  const TObjArray* trans_sis = fTransClass->GetStreamerInfos();
  if (trans_sis->GetLast() != fOldLast) {
    // There was a change.  Compare the TStreamerInfo list for the
    // persistent class and the transient class.  Look for entries
    // where the transient class has a TStreamerInfo but the persistent
    // class does not.
    //
    // Note: don't do this for `version' -1.
    // That should always be built from the internal dictionary.
    // (Otherwise, root can crash.)
    const TObjArray* pers_sis = fPersClass->GetStreamerInfos();
    assert (trans_sis->LowerBound() == pers_sis->LowerBound());
    for (Int_t v = trans_sis->LowerBound(); v <= trans_sis->GetLast(); ++v) {
      if (v != -1 &&
          trans_sis->At(v) != 0 &&
          (v > pers_sis->GetLast() || pers_sis->At(v) == 0))
      {
        // Ok, at this index, the transient class has a TStreamerInfo
        // but the persistent class does not.  Copy the TStreamerInfo
        // (changing the class that it applies to to the persistent
        // class), and add it to the persistent class's list.
        TStreamerInfo* new_si =(TStreamerInfo*)(trans_sis->At(v)->Clone());
        new_si->SetClass (fPersClass);
        fPersClass->RegisterStreamerInfo (new_si);
      }
    }

    // Remember how many entries there were in the list for next time.
    fOldLast = trans_sis->GetLast();
  }
}


/**
 * @brief Default constructor.
 *
 * Intended for use with @c T_AthenaRootConverter only!
 * You must call @c SetCtorInfo after this!
 */
TVirtualConverter::TVirtualConverter()
  : fTransClass (0),
    fPersClass (0),
    fCheckSum (0),
    fOldLast (-3),
    fPersObj (0)
{
}


/**
 * @brief Set the parameters that would normally be passed to the constructor.
 * @param checksum   The Root checksum of the persistent class we'll convert.
 * @param trans_cls  The transient class to which we will be converting.
 * @param pers_cls   The persistent class from which we will be converting.
 *
 * Intended for use with @c T_AthenaRootConverter only!
 */
void TVirtualConverter::SetCtorInfo (UInt_t checksum,
                                     TClass* trans_cls,
                                     TClass* pers_cls)
{
  assert (fTransClass == 0);

  fCheckSum = checksum;
  fTransClass = trans_cls;
  fPersClass = pers_cls;
  CheckClasses();
}
