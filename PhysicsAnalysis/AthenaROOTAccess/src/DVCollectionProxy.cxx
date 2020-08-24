/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  AthenaROOTAccess/src/DVCollectionProxy.cxx
 * @author scott snyder
 * @date April 2007
 * @brief A Root collection proxy for @c DataVector containers.
 */


#include "AthenaROOTAccess/DVCollectionProxy.h"
#include "AthContainers/DataVector.h"
#include "CxxUtils/no_sanitize_undefined.h"
#include "TError.h"
#include "RootUtils/TClassEditRootUtils.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TMethodCall.h"
#include "TROOT.h"
# include "TCollectionProxyInfo.h"
#include <cassert>


namespace AthenaROOTAccess {


/**
 * @brief Find the offset to the unique DV base class.
 * @param cls Class in which to find the base.
 * @returns The offset of the base in @a cls.
 *
 * Can't use @c TClass::GetBaseClassOffset, because it doesn't
 * handle virtual derivation.  @a cls must have a default constructor.
 */
int safe_get_base_offset (TClass* cls)
{
  TMethodCall getbase;
  getbase.InitWithPrototype (cls, "auxbase", "");
  if (!getbase.IsValid()) {
    ::Warning ("AthenaROOTAccess::DVCollectionProxy", "Can't get auxbase "
               " for type `%s'.", cls->GetName());
    return 0;
  }
  void* obj = cls->New();
  Long_t baseaddr;
  getbase.Execute (obj, baseaddr);
  int ret = reinterpret_cast<char*>(baseaddr) - reinterpret_cast<char*>(obj);
  cls->Destructor (obj);
  return ret;
}


/**
 * @brief Find the contained class in a @c DataVector.
 * @param dvclass The @c TClass for a @c DataVector class.
 * @return The @c TClass for the @c DataVector's element type.
 */
TClass* class_from_dvclass (TClass* dvclass)
{
  // Split up the class name into template arguments.
  std::vector<std::string> args;
  int tailloc;
  TClassEdit::GetSplit (dvclass->GetName(), args, tailloc);
  assert (args.size() > 1);

  // This should be the element type name.
  std::string elname = args[1];
  assert (elname.size() > 0);

  // Look up the element class.
  TClass* elclass = gROOT->GetClass (elname.c_str());
  if (!elclass)
    ::Error ("AthenaROOTAccess::DVCollectionProxy",
             "Cannot find class %s", elname.c_str());
  return elclass;
}


/**
 * @brief Find the unique base @c DataVector class.
 * @param dvclass The @c TClass for a @c DataVector class.
 * @return The @c TClass for the unique base @c DataVector class.
 *         Returns 0 if there is no such base, or if it
 *         is the same as @a dvclass.
 *
 * Note: @c DataVector\<T> can derive from @c DataVector\<U>.
 * But there can also be helper classes in between
 * that start with `DataVector' (note: no `<').
 */
TClass* find_dv_base (TClass* dvclass)
{
  TIter next (dvclass->GetListOfBases());
  while (TBaseClass* bcl = dynamic_cast<TBaseClass*>(next())) {
    TClass* cc = bcl->GetClassPointer();
    if (cc && strncmp (cc->GetName(), "DataVector", 10) == 0) {
      TClass* bdv = find_dv_base (cc);
      if (bdv) return bdv;
      if (strncmp (cc->GetName(), "DataVector<", 11) == 0)
        return cc;
    }
  }
  return 0;
}


/**
 * @brief Helper functions for accessing the container data via the proxy.
 */
class DVCollectionFuncs
{
public:
  /// @brief The container type.
  /// We alias this with the real vector that lives inside the 
  /// @c DataVector.
  typedef std::vector<char*>     Cont_t;

  /**
   * @brief Proxy environment buffer.
   *
   * The first 64 bytes of the environment structure are a scratch area.
   * The Root proxies put a collection iterator there, but we instead
   * lay it out like this.
   *
   * This is slightly different in newer versions of root.
   * Rather than having the 64-byte buffer, Environ takes a template
   * argument giving the payload.
   */
  struct env_buff
  {
    /// The index of the last element retrieved.
    size_t m_index;

    /// @brief Pointer to the container.
    /// (Note that the object pointer from the environment will point at
    /// the top-level @c DataVector class; this points directly at the vector.)
    Cont_t* m_cont;

    /// The last element pointer to have been returned.
    void* m_eltptr;

    /// Method to cast from the DV representation of an element
    /// to the declared element type.
    TMethodCall m_caster;
  };


  /// The Root proxy environment structure.
  typedef ROOT::TCollectionProxyInfo::Environ<env_buff>  Env_t;
# define ENVBUFF(e) ((e).fIterator)
# define FSIZE fSize
# define FIDX fIdx

  /**
   * @brief Fetch the container from a proxy environment.
   * @param env The proxy environment.
   */
  static Cont_t* cont (void* env)
  {
    Env_t& e = *reinterpret_cast<Env_t*> (env);
    env_buff& buff = ENVBUFF(e);
    return buff.m_cont;
  }


  static void* do_cast (env_buff& buff, void* p)
  {
    if (buff.m_caster.IsValid()) {
      buff.m_caster.ResetParam();
      buff.m_caster.SetParam (reinterpret_cast<Long_t> (p));
      Long_t ret = 0;
      buff.m_caster.Execute (ret);
      buff.m_eltptr = reinterpret_cast<void*> (ret);
    }
    else
      buff.m_eltptr = p;
    return &buff.m_eltptr;
  }


  /**
   * @brief Return the first element of the container.
   * @param env The proxy environment.
   * @return A pointer to the first element, or 0 if the container is empty.
   *
   * This resets the internal pointer to 0.
   */
  static void* first(void* env)  {
    Env_t&  e = *reinterpret_cast<Env_t*> (env);
    Cont_t& c = *cont(env);
    env_buff& buff = ENVBUFF(e);
    buff.m_index = 0;
    e.FSIZE  = c.size();
    if ( 0 == e.FSIZE )
      return 0;
    return do_cast (buff, c[0]);
  }


  /**
   * @brief Return a following element of the container.
   * @param env The proxy environment.
   * @return A pointer to the following element, or 0 if we're past the end.
   *
   * The internal pointer will be advanced by the value of @c e.idx
   * (after which @c e.idx will be reset to 0).  A pointer to the element
   * referenced by this new index will be returned.
   */
  static void* next(void* env)  {
    Env_t&  e = *reinterpret_cast<Env_t*> (env);
    Cont_t& c = *cont(env);
    env_buff& buff = ENVBUFF(e);
    buff.m_index += e.FIDX;
    e.FIDX = 0;
    if (buff.m_index >= e.FSIZE) return 0;
    return do_cast (buff, c[buff.m_index]);
  }


  /**
   * @brief Return the size of the container.
   * @param env The proxy environment.
   */
  static void* size(void* env)  {
    Env_t&  e = *reinterpret_cast<Env_t*> (env);
    e.FSIZE    = cont(env)->size();
    return &e.FSIZE;
  }


  /**
   * @brief Erase the container.
   * @param env The proxy environment.
   */
  static void* clear(void* env)  {
    cont(env)->clear();
    return 0;
  }


  /**
   * @brief Return a new environment structure.
   */
  static void* create_env() {
    return new Env_t;
  }


  //*************************************************************************
  // These methods are not needed for AthenaROOTAccess,
  // and are not implemented.

  /// Not implemented for AthenaROOTAccess.
  static void resize(void* /*obj*/, size_t /*size*/)  {
    ::Fatal("DVCollectionProxy", "resize not implemented");
  }
  /// Not implemented for AthenaROOTAccess.
  static void* construct(void* /*from*/, size_t /*size*/)  {
    ::Fatal("DVCollectionProxy", "construct not implemented");
    return 0;
  }
  /// Not implemented for AthenaROOTAccess.
  static void destruct(void* /*obj*/, size_t /*size*/)  {
    ::Fatal("DVCollectionProxy", "destruct not implemented");
  }
  /// Not implemented for AthenaROOTAccess.
  static void* feed(void* /*from*/, void* /*to*/, size_t /*size*/)  {
    ::Fatal("DVCollectionProxy", "feed not implemented");
    return 0;
  }
  /// Not implemented for AthenaROOTAccess.
  static void* collect(void* /*from*/, void* /*to*/)  {
    ::Fatal("DVCollectionProxy", "collect not implemented");
    return 0;
  }
};


//**************************************************************************



/**
 * @brief Constructor.
 * @param elttype The name of the contained type (including a trailing '*').
 * @param conttype The name of the container type we're proxying.
 *                 Note that this may be different from
 *                 a @c DataVector of @a elttype for classes which
 *                 derive from @c DataVector.
 */
DVCollectionProxy::DVCollectionProxy (const char* elttype,
                                      const char* conttype)
  : TGenCollectionProxy (typeid (DataVector<DVCollectionProxyDummy>),
                         sizeof (char*))
{
  // Find the container and element offsets.
  FindOffsets (elttype, conttype);

  // Set up the element size.  No offset, since this isn't a map.
  fValDiff        = sizeof (void*);
  fValOffset      = 0;

  // Set up the worker functions.
  fSize.call      = DVCollectionFuncs::size;
  fNext.call      = DVCollectionFuncs::next;
  fFirst.call     = DVCollectionFuncs::first;
  fClear.call     = DVCollectionFuncs::clear;
  fCollect        = DVCollectionFuncs::collect;
  fCreateEnv.call = DVCollectionFuncs::create_env;
  fResize         = DVCollectionFuncs::resize;
  fConstruct      = DVCollectionFuncs::construct;
  fDestruct       = DVCollectionFuncs::destruct;
  fFeed           = DVCollectionFuncs::feed;

  // Do the base class initialization.
  CheckFunctions();
  Initialize(false);

  // Need to override what that set up for fValue and fVal.
  delete static_cast<TGenCollectionProxy::Value*>(fValue);
  delete fVal;
  fValue = new TGenCollectionProxy::Value (elttype, false);
  fVal = new TGenCollectionProxy::Value (*fValue);

  fClass = gROOT->GetClass (conttype);

  // This container holds pointers.
  fPointers = 1;
}


/**
 * @brief Copy constructor.
 * @param rhs The proxy to copy.
 */
DVCollectionProxy::DVCollectionProxy (const DVCollectionProxy& rhs)
  : TGenCollectionProxy (rhs),
    m_contoff (rhs.m_contoff),
    m_caster (rhs.m_caster)
{
}


/**
 * @brief Start working with a new collection.
 * @param objstart The address of the collection.
 */
void DVCollectionProxy::PushProxy NO_SANITIZE_UNDEFINED (void *objstart)
{
  // Do the base class stuff.
  // This will create an environment buffer if needed.
  ::TGenCollectionProxy::PushProxy (objstart);

  // Save the calculated element offset in the environment buffer.
  DVCollectionFuncs::env_buff& buff =
    ENVBUFF(*reinterpret_cast<DVCollectionFuncs::Env_t*>(fEnv));

  buff.m_caster = m_caster;

  // Save the address of the underlying vector of the DataVector.
  // First, adjust the address to the base DataVector.
  char* dvstart = reinterpret_cast<char*>(objstart) + m_contoff;
  // Cast to DV.
  DataVector<char>* dv = reinterpret_cast<DataVector<char>*> (dvstart);
  // Find the underlying vector.
  // This gives a ubsan warning.
  // However, this is deliberate, so suppress ubsan warnings for this function.
  const std::vector<char*>& vec = dv->stdcont();
  // And store its address.
  buff.m_cont = const_cast<std::vector<char*>*> (&vec);
}


/**
 * @brief Clone this object.
 */
TVirtualCollectionProxy* DVCollectionProxy::Generate() const
{
  return new DVCollectionProxy(*this);
}


/**
 * @brief Initialize the cached pointer offsets.
 * @param elttype The name of the contained type (including a trailing '*').
 * @param conttype The name of the container type we're proxying.
 *                 Note that this may be different from
 *                 a @c DataVector of @a elttype for classes which
 *                 derive from @c DataVector.
 *
 * Suppose we have @c D deriving from @c B, and thus
 * @c DataVector\<D> deriving from @c DataVector\<B>.
 * In general, inheritance may be multiple or virtual,
 * so we need to adjust the pointer offsets.
 *
 * Suppose we're setting up the proxy for @c DataVector\<D>.
 * Then @c m_contoff will be set to the offset of the @c DataVector\<B> base
 * within @c DataVector\<D> --- this is the amount we need to shift the
 * container pointer by before applying the proxy.
 * Originally, we cached an offset for the element conversions
 * as well.  But that doesn't work for the case of virtual derivation.
 * Instead, we save the Reflex types for the base and derived types,
 * and use Reflex to convert.  (We can get away with caching the
 * offset for the container because we know what the fully-derived
 * type will be.  We don't know that for the elements.)
 */
void DVCollectionProxy::FindOffsets (const char* elttype,
                                     const char* conttype)
{
  // Start by assuming no offsets.
  m_contoff = 0;

  // Find its TClass.
  TClass* dvclass = gROOT->GetClass (conttype);
  if (!dvclass) {
    ::Error ("AthenaROOTAccess::DVCollectionProxy",
             "Cannot find class %s", conttype);
    return;
  }

  // Find the TClass for the base DataVector class.
  TClass* dvbase = find_dv_base (dvclass);
  if (!dvbase) {
    // No inheritance --- offsets are zero.
    return;
  }

  // Find the container offset.
  m_contoff = safe_get_base_offset (dvclass);

  // Now, find the base and derived element classes.
  std::string elttype2 = elttype;
  if (elttype2[elttype2.size()-1] == '*')
    elttype2.erase (elttype2.size()-1);
  TClass* elclass = gROOT->GetClass (elttype2.c_str());
  TClass* baseclass = class_from_dvclass (dvbase);

  if (!elclass || !baseclass)
    return;

  m_caster.InitWithPrototype (dvclass, "do_cast", elttype);
  if (!m_caster.IsValid()) {
    ::Warning ("AthenaROOTAccess::DVCollectionProxy", "Can't get do_cast "
               " for type `%s' (%s).", dvclass->GetName(), elttype);
  }
}


} // namespace AthenaROOTAccess
