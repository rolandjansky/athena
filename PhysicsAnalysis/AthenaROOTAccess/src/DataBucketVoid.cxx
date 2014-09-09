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
 *
 * Note: this had originally been written using @c TClass, etc.
 * However, we ran into problems because @c TClass does not properly
 * represent virtual derivation, which we require in some cases.
 * Thus, this was rewritten to use Reflex directly (which does represent
 * virtual derivation correctly.)
 */


#include "AthenaROOTAccess/DataBucketVoid.h"
#include "AthContainers/OwnershipPolicy.h"

#include "Reflex/Base.h"
#include "TError.h"
#include "TClass.h"


using ROOT::Reflex::Type;
using ROOT::Reflex::Member;
using ROOT::Reflex::Object;
using ROOT::Reflex::Member_Iterator;
using ROOT::Reflex::Base_Iterator;


namespace AthenaROOTAccess {


/**
 * @brief Constructor.
 * @param cl The Root class of the held object.
 * @param ptr The pointer to hold.
 */
DataBucketVoid::DataBucketVoid (TClass* cl, void* ptr)
  : m_ptr (Type::ByTypeInfo (*cl->GetTypeInfo()), ptr)
{
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
    ent.m_obj.Destruct();
  }
}


/**
 * @brief Return the held object.
 */
void* DataBucketVoid::object()
{
  return m_ptr.Address();
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
                            bool /*isConst = true*/) const
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
                            bool /*isConst = true*/) const
{
  // If the requested type is the same as the held type,
  //no conversion is needed.
  if (tinfo == m_ptr.TypeOf().TypeInfo())
    return m_ptr.Address();

  // Look up the requested type.
  Type cl = Type::ByTypeInfo (tinfo);
  if (!cl) {
    ::Error ("DataBucketVoid", "Cannot find definition for class %s",
             tinfo.name());
    return 0;
  }

  // Find the offset.
  Object castobj = m_ptr.CastObject (cl);

  if (!castobj) {
    std::string mclname = m_ptr.TypeOf().Name();
    std::string clname = cl.Name();

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
               m_ptr.TypeOf().Name_c_str(), cl.Name_c_str());
      return 0;
    }
  }

  // Convert the pointer.
  return castobj.Address();
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
  SG::OwnershipPolicy pol = SG::VIEW_ELEMENTS;
  args.push_back (&pol);
  for (size_t i = 0; i < sz; i++) {
    dvconv_t& ent = m_dvconv[i];
    ent.m_methclear.Invoke (ent.m_obj, 0, args);
  }
}


/**
 * @brief Try to do a copying DV conversion.
 * @param cldest Desired destination class.
 * @return Pointer to converted object, or null.
 */
void* DataBucketVoid::tryCopyingConversion (const Type& cldest)
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
  if (strncmp (cldest.Name_c_str(), "DataVector<", 11) != 0)
    return 0;

  // Is the source class a DataVector, or does it derive from one?
  if (!m_dvcl)
    m_dvcl = findDVBase (m_ptr.TypeOf());
  if (!m_dvcl)
    return 0;

  m_dvptr = m_ptr.CastObject (m_dvcl);
  if (!m_dvptr)
    return 0;

  m_methsize = getMethod (m_dvcl, "size");
  {
    Member_Iterator beg = m_dvcl.FunctionMember_Begin();
    Member_Iterator end = m_dvcl.FunctionMember_End();
    for (; beg != end; ++beg) {
      if (beg->Name() == "operator[]" && beg->IsConst()) {
        m_methat = *beg;
        break;
      }
    }
  }

  // Find the element types.
  Type eltdest = cldest.TemplateArgumentAt (0);
  if (!eltdest)
    return 0;

  Type eltsource = m_dvcl.TemplateArgumentAt (0);
  if (!eltsource)
    return 0;

  // Is eltdest a base of eltsource?
  // (We're only allowing D->B conversions for now.)
  if (!eltsource.HasBase (eltdest))
    return 0;

  // Make a new table entry.
  m_dvconv.resize (sz + 1);
  dvconv_t& ent = m_dvconv.back();
  ent.m_cldest = cldest;
  ent.m_eltsource = eltsource;
  ent.m_eltdest = eltdest;
  ent.m_methsize = getMethod (cldest, "size");
  ent.m_methclear = getMethod (cldest, "clear",
                               Type::ByName("void (SG::OwnershipPolicy)"));
  ent.m_methreserve = getMethod (cldest, "reserve");
  ent.m_methpush_back = cldest.FunctionMemberByName ("push_back");

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
    ent.m_obj = ent.m_cldest.Construct();
  }

  // If the sizes of the source and destination are different, recopy pointers.
  size_t sz_source = 0;
  m_methsize.Invoke (m_dvptr, sz_source);

  size_t sz_dest;
  ent.m_methsize.Invoke (ent.m_obj, sz_dest);

  // Refill the destination container if the sizes don't match.
  if (sz_source != sz_dest)
    refillDest (ent, sz_source);

  return ent.m_obj.Address();
}


/**
 * @brief Refill the destination container from the source container.
 * @param ent Table entry for the destination container.
 * @param sz_source Size of the source container.
 */
void DataBucketVoid::refillDest (dvconv_t& ent, size_t sz_source)
{
  // Clear the destination container.
  std::vector<void*> args;
  SG::OwnershipPolicy pol = SG::VIEW_ELEMENTS;
  args.push_back (&pol);
  ent.m_methclear.Invoke (ent.m_obj, 0, args);

  args[0] = &sz_source;
  ent.m_methreserve.Invoke (ent.m_obj, 0, args);

  for (size_t i = 0; i < sz_source; i++) {
    args[0] = &i;
    void* addr = 0;
    m_methat.Invoke (m_dvptr, addr, args);
    Object obj (ent.m_eltsource, addr);
    Object castobj = obj.CastObject (ent.m_eltdest);

    args[0] = castobj.Address();

    ent.m_methpush_back.Invoke (ent.m_obj, 0, args);
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
Type DataBucketVoid::findDVBase (const Type& cl)
{
  // If this class is a DataVector, return it.
  if (strncmp (cl.Name_c_str(), "DataVector<", 11) == 0)
    return cl;

  Type dvbase;

  // Scan base classes too.
  Base_Iterator beg = cl.Base_Begin();
  Base_Iterator end = cl.Base_End();
  for (; beg != end; ++beg) {
    dvbase = findDVBase (beg->ToType());
    if (dvbase)
      break;
  }

  return dvbase;
}


/**
 * @brief Helper to look up a member function in @c typ or a base.
 * @param typ The class in which to search.
 * @param name The name of the desired function.
 * @param signature Signature of the desired function.
 *
 * Searches @a typ and its bases for the function member @a name.
 * Warning: ??? The base search order is depth-first, which isn't
 * really right as per C++, but it should suffice for our purposes here.
 */
Member DataBucketVoid::getMethod (const Type& typ,
                                  const std::string& name,
                                  const Type& signature /*= Type(0,0)*/)
{
  Member mem = typ.FunctionMemberByName (name, signature);
  if (!mem) {
    Base_Iterator beg = typ.Base_Begin();
    Base_Iterator end = typ.Base_End();
    for (; beg != end; ++beg) {
      mem = getMethod (beg->ToType(), name, signature);
      if (mem) break;
    }
  }
  return mem;
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
