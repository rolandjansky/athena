/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataBucketVoid.cxx,v 1.12 2009-01-23 16:07:41 ssnyder Exp $
/**
 * @file  AthenaROOTAccess/src/DataBucketVoid.cxx
 * @author scott snyder
 * @date April 2007
 * @brief Concrete @c DataBucket that holds the object via a @c void*
 *        and uses the Root dictionary to do conversions.
 */


#include "AthenaROOTAccess/DataBucketVoid.h"
#include "AthContainers/OwnershipPolicy.h"

#include "TError.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TList.h"
#include "TClassEdit.h"


namespace {


/**
 * @brief Return the element type for a DataVector class.
 * @param dvcls The input DataVector class.
 *
 * Given the TClass for DataVector<T>, return the TClass for T.
 */
TClass* getElementType (TClass* dvcls)
{
  TClassEdit::TSplitType sp (dvcls->GetName());
  if (sp.fElements.size() < 2)
    return 0;
  return TClass::GetClass (sp.fElements[1].c_str());
}


} // anonymous namespace


namespace AthenaROOTAccess {


/**
 * @brief Constructor.
 * @param cl The Root class of the held object.
 * @param ptr The pointer to hold.
 */
DataBucketVoid::DataBucketVoid (TClass* cl, void* ptr)
  : m_cl (cl),
    m_ptr (ptr),
    m_dvcl (nullptr),
    m_dvptr (nullptr)
{
  m_dvcl = findDVBase (m_cl);
  m_dvptr = ptr;
  if (m_dvcl) {
    int dvoffs = cl->GetBaseClassOffset (m_dvcl);
    if (dvoffs > 0)
      m_dvptr = reinterpret_cast<char*> (ptr) + dvoffs;

    std::string proto = "const char*, const ";
    proto += m_dvcl->GetName();
    proto += "&, const std::type_info&";
    m_baseOffsetMeth.InitWithPrototype (m_dvcl, "baseOffset1", proto.c_str());
    if (!m_baseOffsetMeth.IsValid()) {
      ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::baseOffset1 ",
               m_dvcl->GetName());
    }
  }
}


/**
 * @brief Destructor.
 */
DataBucketVoid::~DataBucketVoid()
{
  // Delete any cached copy-converted DV objects.
  size_t sz = m_dvconv.size();
  for (size_t i = 0; i < sz; i++) {
    dvconv_t& ent = m_dvconv[i];
    ent.m_cldest->Destructor (ent.m_obj);
  }
}


/**
 * @brief Return the held object.
 */
void* DataBucketVoid::object()
{
  return m_ptr;
}


/**
 * @brief Return the @c type_info for the stored object.
 */
const std::type_info& DataBucketVoid::tinfo() const
{
  return *m_cl->GetTypeInfo();
}


/**
 * @brief Return the contents of the @c DataBucket,
 *        converted to type given by @a clid.  Note that only
 *        derived->base conversions are allowed here.
 * @param clid The class ID to which to convert.
 * @param irt To be called if we make a new instance.
 * @param isConst True if the object being converted is regarded as const.
 */
void* DataBucketVoid::cast (CLID /*clid*/,
                            SG::IRegisterTransient* /*itr = 0*/,
                            bool /*isConst = true*/)
{
  // ??? unimplemented
  abort();
}


/**
 * @brief Return the contents of the @c DataBucket,
 *        converted to type given by @c std::type_info.  Note that only
 *        derived->base conversions are allowed here.
 * @param tinfo The @c std::type_info of the type to which to convert.
 * @param irt To be called if we make a new instance.
 * @param isConst True if the object being converted is regarded as const.
 */
void* DataBucketVoid::cast (const std::type_info& tinfo,
                            SG::IRegisterTransient* /*itr = 0*/,
                            bool /*isConst = true*/)
{
  // If the requested type is the same as the held type,
  //no conversion is needed.
  if (tinfo == *m_cl->GetTypeInfo())
    return m_ptr;

  // Look up the requested type.
  TClass* cl = TClass::GetClass (tinfo);
  if (!cl) {
    ::Error ("DataBucketVoid", "Cannot find definition for class %s",
             tinfo.name());
    return 0;
  }

  // Find the offset.
  void* castobj = 0;
  for (const auto& p : m_offsets) {
    if (p.first == &tinfo) {
      castobj = reinterpret_cast<char*>(m_dvptr) + p.second;
      break;
    }
  }

  if (castobj == 0 && m_baseOffsetMeth.IsValid()) {
    m_baseOffsetMeth.ResetParam();
    m_baseOffsetMeth.SetParam (reinterpret_cast<Long_t> (m_ptr));
    m_baseOffsetMeth.SetParam (reinterpret_cast<Long_t> (m_dvptr));
    m_baseOffsetMeth.SetParam (reinterpret_cast<Long_t> (&tinfo));
    Long_t ret = 0;
    m_baseOffsetMeth.Execute (ret);
    if (ret >= 0) {
      castobj = reinterpret_cast<char*>(m_dvptr) + ret;
      m_offsets.emplace_back (&tinfo, ret);
    }
  }

  if (castobj == 0) {
    int offs = m_cl->GetBaseClassOffset (cl);
    if (offs >= 0) {
      castobj = reinterpret_cast<char*>(m_dvptr) + offs;
      m_offsets.emplace_back (&tinfo, offs);
    }
  }

  if (!castobj) {
    std::string mclname = m_cl->GetName();
    std::string clname = cl->GetName();

    // Handle some special cases.
    // These containers don't have the data vector inheritance macros
    // defined, but we need these conversions anyway.
    if (mclname == "TruthParticleContainer" &&
        clname== "DataVector<INavigable4Momentum>")
    {
      castobj = m_ptr;
    }
    else if (mclname == "Analysis::MuonContainer" &&
             clname == "DataVector<INavigable4Momentum>")
    {
      castobj = m_ptr;
    }
    else if (mclname == "ElectronContainer" &&
             clname == "DataVector<INavigable4Momentum>")
    {
      castobj = m_ptr;
    }
    else if (mclname == "PhotonContainer" &&
             clname == "DataVector<INavigable4Momentum>")
    {
      castobj = m_ptr;
    }

    else {
      // Try for a DV copying conversion.
      void* ret = const_cast<DataBucketVoid*>(this)->tryCopyingConversion (cl);
      if (ret)
        return ret;

      // Not a legal conversion.
      ::Error ("DataBucketVoid", "Cannot cast class %s to %s",
               m_cl->GetName(), cl->GetName());
      return 0;
    }
  }

  // Convert the pointer.
  return castobj;
}

/**
 * @brief Return a new @c DataBucket whose payload has been cloned from the
 *        original one.
 */
DataBucketVoid* DataBucketVoid::clone () const
{
  // ??? unimplemented (yet?)
  ::Error ("DataBucketVoid", "::clone not (yet?) implemented !");
  abort();
  return 0;
}


/**
 * @brief Reset any cached data for a new event.
 */
void DataBucketVoid::reset()
{
  size_t sz = m_dvconv.size();
  if (!sz) return;
  std::vector<void*> args;
  for (size_t i = 0; i < sz; i++) {
    dvconv_t& ent = m_dvconv[i];
    ent.m_methclear.Execute (ent.m_obj);
  }
}


/**
 * @brief Try to do a copying DV conversion.
 * @param cldest Desired destination class.
 * @return Pointer to converted object, or null.
 */
void* DataBucketVoid::tryCopyingConversion (TClass* cldest)
{
  // See if this destination is already in our table.
  size_t sz = m_dvconv.size();
  for (size_t i = 0; i < sz; ++i) {
    dvconv_t& ent = m_dvconv[i];
    if (ent.m_cldest == cldest)
      return doCopyingConversion (ent);
  }

  // See if this qualifies as a copying conversion.
  // Is the destination class a DataVector?
  if (strncmp (cldest->GetName(), "DataVector<", 11) != 0)
    return 0;

  // Is the source class a DataVector, or does it derive from one?
  if (!m_dvcl)
    return 0;

  if (!m_dvptr)
    return 0;

  m_methsize.InitWithPrototype (m_dvcl, "size", "");
  if (!m_methsize.IsValid()) {
    ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::size ",
             m_dvcl->GetName());
  }

  m_methat.InitWithPrototype (m_dvcl, "get", "unsigned int");
  if (!m_methat.IsValid()) {
    ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::operator[] ",
             m_dvcl->GetName());
  }

  // Find the element types.
  TClass* eltdest = getElementType (cldest);
  if (!eltdest)
    return 0;

  TClass* eltsource = getElementType (m_dvcl);
  if (!eltsource)
    return 0;

  const SG::BaseInfoBase* bib = SG::BaseInfoBase::find (*eltsource->GetTypeInfo());
  if (!bib)
    return 0;

  // Is eltdest a base of eltsource?
  // (We're only allowing D->B conversions for now.)
  if (!bib->is_base (*eltdest->GetTypeInfo()))
    return 0;

  // Make a new table entry.
  m_dvconv.resize (sz + 1);
  dvconv_t& ent = m_dvconv.back();
  ent.m_obj = nullptr;
  ent.m_cldest = cldest;
  ent.m_eltsource = eltsource;
  ent.m_eltdest = eltdest;
  ent.m_castfn = bib->castfn (*eltdest->GetTypeInfo());
  ent.m_methsize.InitWithPrototype (cldest, "size", "");
  if (!ent.m_methsize.IsValid()) {
    ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::size ",
             cldest->GetName());
  }

  ent.m_methclear.InitWithPrototype (cldest, "clear",
                                     "SG::OwnershipPolicy");
  if (!ent.m_methclear.IsValid()) {
    ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::clear ",
             cldest->GetName());
  }
  ent.m_methclear.ResetParam();
  ent.m_methclear.SetParam (static_cast<Long_t>(SG::VIEW_ELEMENTS));

  ent.m_methreserve.InitWithPrototype (cldest, "reserve", "unsigned long");
  if (!ent.m_methreserve.IsValid()) {
    ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::reserve ",
             cldest->GetName());
  }

  std::string proto = eltdest->GetName();
  proto += "*";
  ent.m_methpush_back.InitWithPrototype (cldest, "push_back", proto.c_str());
  if (!ent.m_methpush_back.IsValid()) {
    ::Error ("AthenaROOTAccess::DataBucketVoid", "Can't find %s::push_back ",
             cldest->GetName());
  }

  // Do the conversion.
  return doCopyingConversion (ent);
}


/**
 * @brief Perform a copying DV conversion.
 * @param ent The @c dvconv_t entry describing the conversion.
 * @return Pointer to converted object, or null.
 */
void* DataBucketVoid::doCopyingConversion (dvconv_t& ent)
{
  // Make a new instance if needed.
  if (!ent.m_obj) {
    ent.m_obj = ent.m_cldest->New();
  }

  // If the sizes of the source and destination are different, recopy pointers.
  Long_t ret = 0;
  m_methsize.Execute (m_dvptr, ret);
  size_t sz_source = ret;

  ent.m_methsize.Execute (ent.m_obj, ret);
  size_t sz_dest = ret;

  // Refill the destination container if the sizes don't match.
  if (sz_source != sz_dest)
    refillDest (ent, sz_source);

  return ent.m_obj;
}


/**
 * @brief Refill the destination container from the source container.
 * @param ent Table entry for the destination container.
 * @param sz_source Size of the source container.
 */
void DataBucketVoid::refillDest (dvconv_t& ent, size_t sz_source)
{
  // Clear the destination container.
  ent.m_methclear.Execute (ent.m_obj);

  ent.m_methreserve.ResetParam();
  ent.m_methreserve.SetParam (static_cast<Long_t> (sz_source));
  ent.m_methreserve.Execute (ent.m_obj);

  for (size_t i = 0; i < sz_source; i++) {
    m_methat.ResetParam();
    m_methat.SetParam (static_cast<Long_t> (i));
    Long_t ret;
    m_methat.Execute (m_dvptr, ret);
    void* addr = reinterpret_cast<void*> (ret);

    void* castobj = ent.m_castfn (addr);

    ent.m_methpush_back.ResetParam();
    ent.m_methpush_back.SetParam (reinterpret_cast<Long_t> (castobj));
    ent.m_methpush_back.Execute (ent.m_obj);
  }
}


/**
 * @brief Return any DV base class of @a cl.
 * @param cl The class to test.
 * @return @a cl or a base class of it, that derives from DV.
 *
 * If @a cl is a @c DataVector instantiation, return it.
 * Otherwise, walk the base classes.  If we find a base class
 * that's a @c DataVector instantiation, return it.
 */
TClass* DataBucketVoid::findDVBase (TClass* cl)
{
  // If this class is a DataVector, return it.
  if (strncmp (cl->GetName(), "DataVector<", 11) == 0)
    return cl;

  TIter next (cl->GetListOfBases());
  while (TBaseClass* bcl = dynamic_cast<TBaseClass*>(next())) {
    TClass* cc = bcl->GetClassPointer();
    TClass* dvbase = findDVBase (cc);
    if (dvbase)
      return dvbase;
  }

  return nullptr;
}


/**
 * @brief Give up ownership of the  @c DataBucket contents.
 *        This leaks the contents and it is useful mainly for error handling.
 *
 *        This is a no-op for @c DataBucketVoid (which doesn't own
 *        its target).
 */
void DataBucketVoid::relinquish()
{
}


/**
 * If the held object derives from @c ILockable, call @lock() on it.
 *
 * This is a no-op for ARA.
 */
void DataBucketVoid::lock()
{
}


} // namespace SG

// m_dvptr wrong?
