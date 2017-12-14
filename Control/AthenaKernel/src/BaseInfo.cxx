/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BaseInfo.cxx,v 1.9 2008-11-25 21:45:04 ssnyder Exp $
/**
 * @file  AthenaKernel/src/BaseInfo.cxx
 * @author scott snyder
 * @date Nov 2005
 * @brief Provide an interface for finding inheritance information
 *        at run time.
 *        Out-of-line implementation.
 */

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLIDRegistry.h"
#include "GaudiKernel/System.h"
#include <mutex>
#include <map>
#include <unordered_map>
#include <algorithm>
#include "string.h"


namespace SG {


struct BaseInfoBaseImpl {
  /// Structure to hold information about one base.
  struct info {
    /// Constructor.  @a converter* and @a is_virtual are as for @a add_info.
    info (BaseInfoBase::castfn_t* converter = 0,
          BaseInfoBase::castfn_t* converterTo = 0,
          bool is_virtual = false);

    /// Converter function.  Takes a @a T* and converts to a pointer
    /// to this base.
    BaseInfoBase::castfn_t* m_converter;

    /// Converter function.  Takes a pointer to this base and converts
    /// to @a T*.
    /// to this base.
    BaseInfoBase::castfn_t* m_converterTo;

    /// True if the derivation from this base to @a T is via
    /// virtual derivation.
    bool m_is_virtual;
  };


  /// CLID of this class.
  CLID m_clid;

  /// Set to true when first created.
  /// Reset after we scan for init functions.
  bool m_needs_init;

  /// @c std::type_info of this class.
  const std::type_info* m_typeinfo;


  /// Hold base information indexed by @a type_info.
  typedef std::pair<const std::type_info*, info> ti_map_pair_type;
  typedef std::vector<ti_map_pair_type> ti_map_type;
  ti_map_type m_timap;


  /// Hold copy conversion information indexed by @ type_info.
  typedef std::pair<const std::type_info*, const CopyConversionBase*>
  ti_copyconversion_pair_type;
  typedef std::vector<ti_copyconversion_pair_type> ti_copyconversion_type;
  ti_copyconversion_type m_ti_copyconversion_map;


  /// Map of all @c type_info pointers to @c BaseInfoBase instances.
  typedef std::unordered_map<const std::type_info*, BaseInfoBase*> bi_by_ti_map_type;
  static bi_by_ti_map_type* s_bi_by_ti ATLAS_THREAD_SAFE;


  /// Used to canonicalize @c type_info instances.
  typedef std::unordered_map<std::string, const std::type_info*> ti_by_name_map_type;
  static ti_by_name_map_type* s_ti_by_name ATLAS_THREAD_SAFE;


  /// Holds @c BaseInfo classes awaiting initialization.
  /// This is used to defer initialization until everything's loaded.
  typedef std::unordered_multimap<const std::type_info*,
                                  BaseInfoBase::init_func_t*> init_list_t;
  static init_list_t* s_init_list ATLAS_THREAD_SAFE;


  // To make sure that the maps get deleted at program termination.
  struct Deleter {
    ~Deleter();
  };
  static Deleter s_deleter;

  /// For thread-safety.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  static mutex_t s_mutex ATLAS_THREAD_SAFE;   // For the static variables.
  mutable mutex_t m_mutex;  // For the class members.


  /**
   * @brief Find a base by @c type_info.
   * @param tinfo The @c type_info to find.
   *
   * Returns the @c info pointer for the base corresponding
   * to @c info, or nullptr if there is no match.
   */
  const info* findInfo (const std::type_info& tinfo) const
  {
    // We don't expect there to be many entries, so just use a linear search.
    for (const auto& i : m_timap) {
      if (i.first == &tinfo)
        return &i.second;
    }

    // Sometimes type_info's are not actually unique, depending on how libraries
    // get loaded.  Try again, comparing names.
    for (const auto& i : m_timap) {
      if (strcmp (i.first->name(), tinfo.name()) == 0)
        return &i.second;
    }
    return nullptr;
  }
};



/**
 * @brief Return the CLID for this class.
 */
CLID BaseInfoBase::clid() const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  return m_impl->m_clid;
}


/**
 * @brief Return the @c std::type_info for this class.
 */
const std::type_info& BaseInfoBase::typeinfo() const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  return *m_impl->m_typeinfo;
}


/**
 * @brief Cast to a base pointer.
 * @param p The pointer to cast (a @a T* cast to a @a void*).
 * @param clid ID of the class to which to cast.
 * @return The pointer cast to the requested type, returned
 *         as a @a void*.  @a clid must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 */
void* BaseInfoBase::cast (void* p, CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->cast (p, *ti);
  return 0;
}


/**
 * @brief Cast to a base pointer.
 * @param p The pointer to cast (a @a T* cast to a @a void*).
 * @param clid @a type_info of the class to which to cast.
 * @return The pointer cast to the requested type, returned
 *         as a @a void*.  @a tinfo must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 */
void* BaseInfoBase::cast (void* p, const std::type_info& tinfo) const
{
  if (BaseInfoBase::castfn_t* converter = castfn (tinfo)) {
    return converter (p);
  }
  return nullptr;
}


/**
 * @brief Cast to a derived pointer.
 * @param p The pointer to cast (a @a B* cast to a @a void*).
 * @param clid ID of the class @a B from which to cast.
 * @return The pointer cast to the requested type, returned
 *         as a @a void*.  @a clid must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 *         0 will also be returned if the @a dynamic_cast fails.
 */
void* BaseInfoBase::castTo (void* p, CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->castTo (p, *ti);
  return 0;
}


/**
 * @brief Cast to a derived pointer.
 * @param p The pointer to cast (a @a B* cast to a @a void*).
 * @param clid @a type_info of the class @a B from which to cast.
 * @return The pointer cast to the requested type, returned
 *         as a @a void*.  @a tinfo must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 *         0 will also be returned if the @a dynamic_cast fails.
 */
void* BaseInfoBase::castTo (void* p, const std::type_info& tinfo) const
{
  if (BaseInfoBase::castfn_t* converterTo = castfnTo (tinfo)) {
    return converterTo (p);
  }
  return nullptr;
}


/**
 * @brief Return a function for casting to a base pointer.
 * @param clid ID of the class to which to cast.
 * @return A function to convert a pointer to a @c T to a pointer
 *         to the type identified by @a clid.
 *         @a clid must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 */
BaseInfoBase::castfn_t* BaseInfoBase::castfn (CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->castfn (*ti);
  return 0;
}


/**
 * @brief Return a function for casting to a base pointer.
 * @param clid @a type_info of the class to which to cast.
 * @return A function to convert a pointer to a @c T to a pointer
 *         to the type identified by @a tinfo.
 *         @a tinfo must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 */
BaseInfoBase::castfn_t*
BaseInfoBase::castfn (const std::type_info& tinfo) const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  const BaseInfoBaseImpl::info* i = m_impl->findInfo (tinfo);
  if (i)
    return i->m_converter;
  return nullptr;
}


/**
 * @brief Return a function for casting to a derived pointer.
 * @param clid ID of the class @a B from which to cast.
 * @return A function to convert a pointer to a @a B to a pointer
 *         to a @a T.  @a B must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 *         0 will also be returned if the @a dynamic_cast fails.
 */
BaseInfoBase::castfn_t* BaseInfoBase::castfnTo (CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->castfnTo (*ti);
  return 0;
}


/**
 * @brief Return a function for casting to a derived pointer.
 * @param clid @a type_info of the class @a B from which to cast.
 * @return A function to convert a pointer to a @a B to a pointer
 *         to a @a T.  @a B must be known to be a base
 *         of @a T; otherwise, 0 will be returned.
 *         0 will also be returned if the @a dynamic_cast fails.
 */
BaseInfoBase::castfn_t*
BaseInfoBase::castfnTo (const std::type_info& tinfo) const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  const BaseInfoBaseImpl::info* i = m_impl->findInfo (tinfo);
  if (i)
    return i->m_converterTo;
  return nullptr;
}


/**
 * @brief Return the class IDs of all known bases of @a T (that
 *        have class IDs).  The list will include @a T itself.
 */
std::vector<CLID> BaseInfoBase::get_bases() const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  std::vector<CLID> v;
  v.reserve (map.size());
  for (const auto& p : map) {
    CLID clid = CLIDRegistry::typeinfoToCLID (*p.first);
    if (clid != CLID_NULL)
      v.push_back (clid);
  }
  return v;
}


/**
 * @brief Return the @c type_info's of all known bases of @a T.
 *        The list will include @a T itself.
 */
std::vector<const std::type_info*> BaseInfoBase::get_ti_bases() const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  std::vector<const std::type_info*> v;
  v.reserve (map.size());
  for (const auto& i : map)
    v.push_back (i.first);
  return v;
}


/**
 * @brief Return true if @a clid is the ID of a class that
 *        is known to be a base of @a T.  @a T is considered
 *        to be its own base for this purpose.
 * @param clid The ID of the class to test.
 */
bool BaseInfoBase::is_base (CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->is_base (*ti);
  return 0;
}


/**
 * @brief Return true if @a tinfo is the @a type_info of a class that
 *        is known to be a base of @a T.  @a T is considered
 *        to be its own base for this purpose.
 * @param clid The ID of the class to test.
 */
bool BaseInfoBase::is_base (const std::type_info& tinfo) const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  const BaseInfoBaseImpl::info* i = m_impl->findInfo (tinfo);
  return i != 0;
}


/**
 * @brief Return true if @a clid is the ID of a class that
 *        is known to be a virtual base of @a T.  (This will always
 *        be false for @a T itself.)
 * @param clid The ID of the class to test.
 */
bool BaseInfoBase::is_virtual (CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->is_virtual (*ti);
  return false;
}


/**
 * @brief Return true if @a tinfo is the @a std::type_info of a class that
 *        is known to be a virtual base of @a T.  (This will always
 *        be false for @a T itself.)
 * @param tinfo The @a std::type_info of the class to test.
 */
bool BaseInfoBase::is_virtual (const std::type_info& tinfo) const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  const BaseInfoBaseImpl::info* i = m_impl->findInfo (tinfo);
  if (i)
    return i->m_is_virtual;
  return false;
}


/**
 * @brief Search for a copy conversion to @c tinfo.
 * @param tinfo The class to which we want to convert.
 *
 * Returns the conversion instance or 0.
 */
const CopyConversionBase*
BaseInfoBase::copy_conversion (const std::type_info& tinfo) const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  for (const auto& p : m_impl->m_ti_copyconversion_map) {
    if (p.first == &tinfo)
      return p.second;
  }
  return 0;
}


/**
 * @brief Search for a copy conversion to @c clid.
 * @param clid The class to which we want to convert.
 *
 * Returns the conversion instance or 0.
 */
const CopyConversionBase*
BaseInfoBase::copy_conversion (CLID clid) const
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return this->copy_conversion (*ti);
  return 0;
}


/**
 * @brief Add a new copy conversion.
 * @param tinfo The @c std::type_info of the target class.
 * @param cnv A @c CopyConversionBase instance describing the conversion.
 *
 * The @c BaseInfoBase takes ownership of the @c cnv object.
 */
void
BaseInfoBase::add_copy_conversion (const std::type_info& tinfo,
                                   const CopyConversionBase* cnv)
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  m_impl->m_ti_copyconversion_map.emplace_back (&tinfo, cnv);
}


/**
 * @brief Return known copy conversions.
 *
 * Returns the CLIDs of all target classes that have been registered
 * with this one for copy conversion.
 */
std::vector<CLID>
BaseInfoBase::get_copy_conversions() const
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  std::vector<CLID> out;
  out.reserve (m_impl->m_ti_copyconversion_map.size());
  for (const auto& i : m_impl->m_ti_copyconversion_map) {
    CLID clid = CLIDRegistry::typeinfoToCLID (*i.first);
    if (clid != CLID_NULL)
      out.push_back (clid);
  }
  return out;
}


/**
 * @brief Add information about one base class.
 * @param tinfo The @a std::type_info of the base.
 * @param converter Converter function.  This should be able to
 *                  convert a @a T* to a pointer to this base.
 * @param converterTo Converter function.  This should be able to
 *                    convert a pointer to this base to a @a T*.
 * @param is_virtual True if the derivation from this base to @a T
 *                   is via virtual derivation.
 */
void BaseInfoBase::add_info (const std::type_info& tinfo,
                             castfn_t* converter,
                             castfn_t* converterTo,
                             bool is_virtual)
{
  BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
  {
    const BaseInfoBaseImpl::info* i = m_impl->findInfo (tinfo);
    if (!i) {
      m_impl->m_timap.emplace_back (&tinfo,
                                    BaseInfoBaseImpl::info (converter, converterTo, is_virtual));
    }
  }

  auto i = BaseInfoBaseImpl::s_bi_by_ti->find (&tinfo);
  if (i != BaseInfoBaseImpl::s_bi_by_ti->end()) {
    BaseInfoBaseImpl& impl = *i->second->m_impl;
    if (impl.m_clid == CLID_NULL)
      impl.m_clid = CLIDRegistry::typeinfoToCLID (tinfo);
  }
}


/**
 * @brief Convenience constructor.
 * @param converter Converter function.  This should be able to
 *                  convert a @a T* to a pointer to this base.
 * @param converterTo Converter function.  This should be able to
 *                    convert a pointer to this base to a @a T*.
 * @param is_virtual True if the derivation from this base to @a T
 *                   is via virtual derivation.
 *
 * The defaults are there just so this class can be used with STL
 * containers that require a default ctor.
 */
BaseInfoBaseImpl::info::info (BaseInfoBase::castfn_t* converter /*= 0*/,
                              BaseInfoBase::castfn_t* converterTo /*= 0*/,
                              bool is_virtual /*= false*/)
  : m_converter (converter),
    m_converterTo (converterTo),
    m_is_virtual (is_virtual)
{
}


/**
 * @brief Constructor.
 * @param tinfo The @c std::type_info for this class.
 */
BaseInfoBase::BaseInfoBase (const std::type_info& tinfo)
  : m_impl (new BaseInfoBaseImpl)
{
  m_impl->m_clid = CLIDRegistry::typeinfoToCLID (tinfo);
  m_impl->m_typeinfo = &tinfo;
  m_impl->m_needs_init = true;

  BaseInfoBaseImpl::lock_t lock (BaseInfoBaseImpl::s_mutex);
  if (!BaseInfoBaseImpl::s_bi_by_ti)
    BaseInfoBaseImpl::s_bi_by_ti   = new BaseInfoBaseImpl::bi_by_ti_map_type;
  if (!BaseInfoBaseImpl::s_ti_by_name)
    BaseInfoBaseImpl::s_ti_by_name   = new BaseInfoBaseImpl::ti_by_name_map_type;

  // Register this instance in the static maps.
  (*BaseInfoBaseImpl::s_bi_by_ti)[&tinfo] = this;
  (*BaseInfoBaseImpl::s_ti_by_name)[tinfo.name()] = &tinfo;
}


/**
 * @brief Destructor.
 */
BaseInfoBase::~BaseInfoBase()
{
  for (BaseInfoBaseImpl::ti_copyconversion_type::iterator it =
         m_impl->m_ti_copyconversion_map.begin();
       it != m_impl->m_ti_copyconversion_map.end();
       ++it)
  {
    delete it->second;
  }
  delete m_impl;
}


/**
 * @brief Find the @c BaseInfoBase instance for @c clid.
 * @param clid The class ID of the class for which we want information.
 *
 * Returns 0 if no @c BaseInfoBase instance is available.
 */
const BaseInfoBase* BaseInfoBase::find (CLID clid)
{
  const std::type_info* ti = CLIDRegistry::CLIDToTypeinfo (clid);
  if (ti)
    return BaseInfoBase::find (*ti);
  return 0;
}


/**
 * @brief Helper for @c find.
 * @param tinfo The @c std::type_info of the class
 *              for which we want information.
 *
 * Returns 0 if no @c BaseInfoBase instance is available.
 */
BaseInfoBase* BaseInfoBase::find1 (const std::type_info& tinfo)
{
  BaseInfoBase* bib = nullptr;
  {
    BaseInfoBaseImpl::lock_t lock (BaseInfoBaseImpl::s_mutex);
    if (!BaseInfoBaseImpl::s_bi_by_ti) return 0;
    BaseInfoBaseImpl::bi_by_ti_map_type::iterator i = 
      BaseInfoBaseImpl::s_bi_by_ti->find (&tinfo);
    if (i != BaseInfoBaseImpl::s_bi_by_ti->end()) {
      bib = i->second;
      BaseInfoBaseImpl::lock_t lock (bib->m_impl->m_mutex);
      if (!bib->m_impl->m_needs_init)
        return bib;
      bib->m_impl->m_needs_init = false;
    }
  }

  // Try the initlist.
  while (true) {
    BaseInfoBase::init_func_t* init = nullptr;
    {
      BaseInfoBaseImpl::lock_t lock (BaseInfoBaseImpl::s_mutex);
      if (!BaseInfoBaseImpl::s_init_list) break;
      BaseInfoBaseImpl::bi_by_ti_map_type::iterator i = 
        BaseInfoBaseImpl::s_bi_by_ti->find (&tinfo);
      if (i != BaseInfoBaseImpl::s_bi_by_ti->end())
        bib = i->second;
      BaseInfoBaseImpl::init_list_t::iterator it = 
        BaseInfoBaseImpl::s_init_list->find (&tinfo);
      if (it == BaseInfoBaseImpl::s_init_list->end()) break;
      init = it->second;
      BaseInfoBaseImpl::s_init_list->erase (it);
    }
    init (bib);
  }

  return bib;
}


/**
 * @brief Find the @c BaseInfoBase instance for @c tinfo.
 * @param tinfo The @c std::type_info of the class
 *              for which we want information.
 *
 * Returns 0 if no @c BaseInfoBase instance is available.
 */
const BaseInfoBase* BaseInfoBase::find (const std::type_info& tinfo)
{
  BaseInfoBase* bib = find1 (tinfo);

  // If we didn't find it, try looking up by name.
  // This to deal with the issue of sometimes getting duplicate
  // @c std::type_info instances.
  if (!bib) {
    const std::type_info* tinfo2 = nullptr;
    {
      BaseInfoBaseImpl::lock_t lock (BaseInfoBaseImpl::s_mutex);
      if (BaseInfoBaseImpl::s_ti_by_name) {
        BaseInfoBaseImpl::ti_by_name_map_type::iterator i = 
          BaseInfoBaseImpl::s_ti_by_name->find (tinfo.name());
        if (i != BaseInfoBaseImpl::s_ti_by_name->end() && i->second != &tinfo) {
          tinfo2 = i->second;
        }
      }
    }
    if (tinfo2) {
      bib = find1 (*tinfo2);
    }
  }

  if (bib) {
    BaseInfoBaseImpl& impl = *bib->m_impl;
    BaseInfoBaseImpl::lock_t lock (impl.m_mutex);
    if (impl.m_clid == CLID_NULL)
      impl.m_clid = CLIDRegistry::typeinfoToCLID (*impl.m_typeinfo);
  }

  return bib;
}


/**
 * @brief Register an initialization function.
 * @param tinfo The @c std::type_info for the class being registered.
 * @param init_func Function to initialize @c BaseInfo for the class.
 */
void BaseInfoBase::addInit (const std::type_info* tinfo,
                            init_func_t* init_func)
{
  BaseInfoBaseImpl::lock_t lock (BaseInfoBaseImpl::s_mutex);
  if (!BaseInfoBaseImpl::s_init_list)
    BaseInfoBaseImpl::s_init_list =
      new BaseInfoBaseImpl::init_list_t;
  BaseInfoBaseImpl::s_init_list->insert (std::make_pair (tinfo, init_func));

  if (BaseInfoBaseImpl::s_bi_by_ti) {
    auto i = BaseInfoBaseImpl::s_bi_by_ti->find (tinfo);
    if (i != BaseInfoBaseImpl::s_bi_by_ti->end()) {
      BaseInfoBaseImpl::lock_t lock (i->second->m_impl->m_mutex);
      BaseInfoBaseImpl& impl = *i->second->m_impl;
      impl.m_needs_init = true;
      if (impl.m_clid == CLID_NULL)
        impl.m_clid = CLIDRegistry::typeinfoToCLID (*tinfo);
    }
  }
}


/**
 * @brief Run initializations for this class, if needed.
 */
void BaseInfoBase::maybeInit()
{
  // This may be null during initialization, if we're initializing a
  // BaseInfo<T>::s_instance instance which references another type
  // for which the s_instance constructor has not yet been run.
  if (!m_impl) return;

  bool needs_init = false;
  const std::type_info* ti = nullptr;
  {
    BaseInfoBaseImpl::lock_t lock (m_impl->m_mutex);
    needs_init = m_impl->m_needs_init;
    ti = m_impl->m_typeinfo;
  }
  
  if (needs_init)
    find (*ti);
}



/// Declare the static members of @c BaseInfoBaseImpl.
BaseInfoBaseImpl::bi_by_ti_map_type*        BaseInfoBaseImpl::s_bi_by_ti = 0;
BaseInfoBaseImpl::ti_by_name_map_type*        BaseInfoBaseImpl::s_ti_by_name = 0;
BaseInfoBaseImpl::init_list_t*               BaseInfoBaseImpl::s_init_list = 0;
BaseInfoBaseImpl::mutex_t                    BaseInfoBaseImpl::s_mutex;

// To get them deleted.
BaseInfoBaseImpl::Deleter BaseInfoBaseImpl::s_deleter;
BaseInfoBaseImpl::Deleter::~Deleter()
{
  delete s_bi_by_ti;
  delete s_ti_by_name;
  delete s_init_list;
}


// Helper for dumping within the debugger.
void dumpBaseInfo()
{
  BaseInfoBaseImpl::lock_t lock (BaseInfoBaseImpl::s_mutex);
  std::cout << "map:\n";
  if (BaseInfoBaseImpl::s_bi_by_ti) {
    std::vector<const std::type_info*> vv;
    for (const auto& x : *BaseInfoBaseImpl::s_bi_by_ti)
      vv.push_back (x.first);
    std::sort (vv.begin(), vv.end());
    for (const std::type_info* ti : vv)
    {
      const BaseInfoBase* bib = (*BaseInfoBaseImpl::s_bi_by_ti)[ti];
      std::cout << ti << " " << bib->clid() << " [" << System::typeinfoName (*ti)
                << "]\n";
    }
  }

  std::cout << "\ninitlist:\n";
  if (BaseInfoBaseImpl::s_init_list) {
    for (const auto& x : *BaseInfoBaseImpl::s_init_list)
      std::cout << x.first << " " << x.second << " ["
                << System::typeinfoName (*x.first) << "]\n";
  }
}


} // namespace SG
