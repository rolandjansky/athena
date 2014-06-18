/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BaseInfo.cxx,v 1.9 2008-11-25 21:45:04 ssnyder Exp $
/**
 * @file  SGTools/BaseInfo.cxx
 * @author scott snyder
 * @date Nov 2005
 * @brief Provide an interface for finding inheritance information
 *        at run time.
 *        Out-of-line implementation.
 */

#include "SGTools/BaseInfo.h"
//#include "boost/thread/mutex.hpp"
#include <map>


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


  /// Hold base information indexed by class ID.
  typedef std::map<CLID, info> clid_map_type;
  clid_map_type m_clidmap;

  /// Hold base information indexed by @a type_info.
  typedef std::map<const std::type_info*, info> ti_map_type;
  ti_map_type m_timap;


  /// Map of all CLIDs to @c BaseInfoBase instances.
  typedef std::map<CLID, BaseInfoBase*> bi_by_clid_map_type;
  static bi_by_clid_map_type* s_bi_by_clid;

  /// Map of all @c type_info names to @c BaseInfoBase instances.
  typedef std::map<const std::type_info*, BaseInfoBase*> bi_by_ti_map_type;
  static bi_by_ti_map_type* s_bi_by_ti;


  /// Hold copy conversion information indexed by class ID.
  typedef std::map<CLID, const CopyConversionBase*> clid_copyconversion_type;
  clid_copyconversion_type m_clid_copyconversion_map;

  /// Hold copy conversion information indexed by @ type_info.
  typedef std::map<const std::type_info*, const CopyConversionBase*> 
    ti_copyconversion_type;
  ti_copyconversion_type m_ti_copyconversion_map;

  /// Holds @c BaseInfo classes awaiting initialization.
  /// This is used to defer initialization until everything's loaded.
  struct init_elem_t {
    CLID m_clid;
    const std::type_info* m_ti;
    BaseInfoBase::init_func_t* m_init_func;
  };
  static std::vector<init_elem_t>* s_init_list;


  // To make sure that the maps get deleted at program termination.
  struct Deleter {
    ~Deleter();
  };
  static Deleter s_deleter;

  /// For thread-safety.
  //static boost::mutex s_mutex;
};



/**
 * @brief Return the CLID for this class.
 */
CLID BaseInfoBase::clid() const
{
  return m_impl->m_clid;
}


/**
 * @brief Return the @c std::type_info for this class.
 */
const std::type_info& BaseInfoBase::typeinfo() const
{
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
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  BaseInfoBaseImpl::clid_map_type::const_iterator i = map.find (clid);
  if (i != map.end())
    return i->second.m_converter (p);
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
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  BaseInfoBaseImpl::ti_map_type::const_iterator i = map.find (&tinfo);
  if (i != map.end())
    return i->second.m_converter (p);
  return 0;
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
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  BaseInfoBaseImpl::clid_map_type::const_iterator i = map.find (clid);
  if (i != map.end() && i->second.m_converterTo)
    return i->second.m_converterTo (p);
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
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  BaseInfoBaseImpl::ti_map_type::const_iterator i = map.find (&tinfo);
  if (i != map.end() && i->second.m_converterTo)
    return i->second.m_converterTo (p);
  return 0;
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
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  BaseInfoBaseImpl::clid_map_type::const_iterator i = map.find (clid);
  if (i != map.end())
    return i->second.m_converter;
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
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  BaseInfoBaseImpl::ti_map_type::const_iterator i = map.find (&tinfo);
  if (i != map.end())
    return i->second.m_converter;
  return 0;
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
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  BaseInfoBaseImpl::clid_map_type::const_iterator i = map.find (clid);
  if (i != map.end())
    return i->second.m_converterTo;
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
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  BaseInfoBaseImpl::ti_map_type::const_iterator i = map.find (&tinfo);
  if (i != map.end())
    return i->second.m_converterTo;
  return 0;
}


/**
 * @brief Return the class IDs of all known bases of @a T (that
 *        have class IDs).  The list will include @a T itself.
 */
std::vector<CLID> BaseInfoBase::get_bases() const
{
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  std::vector<CLID> v;
  v.reserve (map.size());
  for (BaseInfoBaseImpl::clid_map_type::const_iterator i = map.begin();
       i != map.end();
       ++i)
    v.push_back (i->first);
  return v;
}


/**
 * @brief Return the @c type_info's of all known bases of @a T.
 *        The list will include @a T itself.
 */
std::vector<const std::type_info*> BaseInfoBase::get_ti_bases() const
{
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  std::vector<const std::type_info*> v;
  v.reserve (map.size());
  for (BaseInfoBaseImpl::ti_map_type::const_iterator i = map.begin();
       i != map.end();
       ++i)
    v.push_back (i->first);
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
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  BaseInfoBaseImpl::clid_map_type::const_iterator i = map.find (clid);
  return i != map.end();
}


/**
 * @brief Return true if @a tinfo is the @a type_info of a class that
 *        is known to be a base of @a T.  @a T is considered
 *        to be its own base for this purpose.
 * @param clid The ID of the class to test.
 */
bool BaseInfoBase::is_base (const std::type_info& tinfo) const
{
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  BaseInfoBaseImpl::ti_map_type::const_iterator i = map.find (&tinfo);
  return i != map.end();
}


/**
 * @brief Return true if @a clid is the ID of a class that
 *        is known to be a virtual base of @a T.  (This will always
 *        be false for @a T itself.)
 * @param clid The ID of the class to test.
 */
bool BaseInfoBase::is_virtual (CLID clid) const
{
  const BaseInfoBaseImpl::clid_map_type& map = m_impl->m_clidmap;
  BaseInfoBaseImpl::clid_map_type::const_iterator i = map.find (clid);
  if (i != map.end())
    return i->second.m_is_virtual;
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
  const BaseInfoBaseImpl::ti_map_type& map = m_impl->m_timap;
  BaseInfoBaseImpl::ti_map_type::const_iterator i = map.find (&tinfo);
  if (i != map.end())
    return i->second.m_is_virtual;
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
  const BaseInfoBaseImpl::ti_copyconversion_type& map =
    m_impl->m_ti_copyconversion_map;
  BaseInfoBaseImpl::ti_copyconversion_type::const_iterator i =
    map.find (&tinfo);
  if (i != map.end())
    return i->second;
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
  const BaseInfoBaseImpl::clid_copyconversion_type& map =
    m_impl->m_clid_copyconversion_map;
  BaseInfoBaseImpl::clid_copyconversion_type::const_iterator i =
    map.find (clid);
  if (i != map.end())
    return i->second;
  return 0;
}


/**
 * @brief Add a new copy conversion.
 * @param clid The @c CLID of the target class.
 * @param tinfo The @c std::type_info of the target class.
 * @param cnv A @c CopyConversionBase instance describing the conversion.
 *
 * The @c BaseInfoBase takes ownership of the @c cnv object.
 */
void
BaseInfoBase::add_copy_conversion (CLID clid,
                                   const std::type_info& tinfo,
                                   const CopyConversionBase* cnv)
{
  if (clid != CLID_NULL)
    m_impl->m_clid_copyconversion_map[clid] = cnv;
  m_impl->m_ti_copyconversion_map[&tinfo] = cnv;
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
  std::vector<CLID> out;
  out.reserve (m_impl->m_clid_copyconversion_map.size());
  for (BaseInfoBaseImpl::clid_copyconversion_type::const_iterator it =
         m_impl->m_clid_copyconversion_map.begin();
       it != m_impl->m_clid_copyconversion_map.end();
       ++it)
  {
    out.push_back (it->first);
  }
  return out;
}


/**
 * @brief Add information about one base class.
 * @param clid The class ID of the base.  May be @a CLID_NULL if no
 *             class ID is available.
 * @param tinfo The @a std::type_info of the base.
 * @param converter Converter function.  This should be able to
 *                  convert a @a T* to a pointer to this base.
 * @param converterTo Converter function.  This should be able to
 *                    convert a pointer to this base to a @a T*.
 * @param is_virtual True if the derivation from this base to @a T
 *                   is via virtual derivation.
 */
void BaseInfoBase::add_info (CLID clid,
                             const std::type_info& tinfo,
                             castfn_t* converter,
                             castfn_t* converterTo,
                             bool is_virtual)
{
  if (clid == CLID_NULL)
    clid = clid_from_initlist (tinfo);
  if (m_impl->m_timap.find (&tinfo) == m_impl->m_timap.end()) {
    if (clid != CLID_NULL)
      m_impl->m_clidmap[clid] =
        BaseInfoBaseImpl::info (converter, converterTo, is_virtual);
    m_impl->m_timap[&tinfo] =
      BaseInfoBaseImpl::info (converter, converterTo, is_virtual);
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
 * @param clid The class ID of this class.  May be @c CLID_NULL if no
 *             ID is available.
 * @param tinfo The @c std::type_info for this class.
 */
BaseInfoBase::BaseInfoBase (CLID clid, const std::type_info& tinfo)
  : m_impl (new BaseInfoBaseImpl)
{
  if (clid == CLID_NULL)
    clid = clid_from_initlist (tinfo);
  m_impl->m_clid = clid;
  m_impl->m_typeinfo = &tinfo;
  m_impl->m_needs_init = true;

  //boost::mutex::scoped_lock lock (BaseInfoBaseImpl::s_mutex);
  if (!BaseInfoBaseImpl::s_bi_by_clid)
    BaseInfoBaseImpl::s_bi_by_clid = new BaseInfoBaseImpl::bi_by_clid_map_type;
  if (!BaseInfoBaseImpl::s_bi_by_ti)
    BaseInfoBaseImpl::s_bi_by_ti   = new BaseInfoBaseImpl::bi_by_ti_map_type;

  // Register this instance in the static maps.
  if (clid != CLID_NULL)
    (*BaseInfoBaseImpl::s_bi_by_clid)[clid] = this;
  (*BaseInfoBaseImpl::s_bi_by_ti)[&tinfo] = this;
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
  //boost::mutex::scoped_lock lock (BaseInfoBaseImpl::s_mutex);
  if (!BaseInfoBaseImpl::s_bi_by_clid) return 0;
  BaseInfoBaseImpl::bi_by_clid_map_type::iterator i =
    BaseInfoBaseImpl::s_bi_by_clid->find (clid);
  if (i != BaseInfoBaseImpl::s_bi_by_clid->end()) {
    if (!i->second->m_impl->m_needs_init)
      return i->second;
    i->second->m_impl->m_needs_init = false;
  }

  // Try the initlist.
  if (BaseInfoBaseImpl::s_init_list) {
    for (size_t j = 0; j < BaseInfoBaseImpl::s_init_list->size(); j++) {
      if ((*BaseInfoBaseImpl::s_init_list)[j].m_clid == clid) {
        BaseInfoBase::init_func_t* init = 
          (*BaseInfoBaseImpl::s_init_list)[j].m_init_func;
        BaseInfoBaseImpl::s_init_list->erase
          (BaseInfoBaseImpl::s_init_list->begin() + j);
        --j;
        init();
        i = BaseInfoBaseImpl::s_bi_by_clid->find (clid);
      }
    }
  }
  if (i != BaseInfoBaseImpl::s_bi_by_clid->end())
    return i->second;

  return 0;
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
  //boost::mutex::scoped_lock lock (BaseInfoBaseImpl::s_mutex);
  if (!BaseInfoBaseImpl::s_bi_by_ti) return 0;
  BaseInfoBaseImpl::bi_by_ti_map_type::iterator i = 
    BaseInfoBaseImpl::s_bi_by_ti->find (&tinfo);
  if (i != BaseInfoBaseImpl::s_bi_by_ti->end()) {
    if (!i->second->m_impl->m_needs_init)
      return i->second;
    i->second->m_impl->m_needs_init = false;
  }

  // Try the initlist.
  if (BaseInfoBaseImpl::s_init_list) {
    for (size_t j = 0; j < BaseInfoBaseImpl::s_init_list->size(); j++) {
      if ((*BaseInfoBaseImpl::s_init_list)[j].m_ti == &tinfo) {
        BaseInfoBase::init_func_t* init = 
          (*BaseInfoBaseImpl::s_init_list)[j].m_init_func;
        BaseInfoBaseImpl::s_init_list->erase
          (BaseInfoBaseImpl::s_init_list->begin() + j);
        --j;
        init();
        i = BaseInfoBaseImpl::s_bi_by_ti->find (&tinfo);
      }
    }
  }
  if (i != BaseInfoBaseImpl::s_bi_by_ti->end())
    return i->second;

  return 0;
}


/**
 * @brief Register an initialization function.
 * @param clid The class ID of the class being registered.
 * @param tinfo The @c std::type_info for the class being registered.
 * @param init_func Function to initialize @c BaseInfo for the class.
 */
void BaseInfoBase::addInit (CLID clid,
                            const std::type_info* tinfo,
                            init_func_t* init_func)
{
  //boost::mutex::scoped_lock lock (BaseInfoBaseImpl::s_mutex);
  if (!BaseInfoBaseImpl::s_init_list)
    BaseInfoBaseImpl::s_init_list =
      new std::vector<BaseInfoBaseImpl::init_elem_t>;
  BaseInfoBaseImpl::init_elem_t elem;
  elem.m_clid = clid;
  elem.m_ti = tinfo;
  elem.m_init_func = init_func;
  BaseInfoBaseImpl::s_init_list->push_back (elem);

  if (BaseInfoBaseImpl::s_bi_by_ti) {
    BaseInfoBaseImpl::bi_by_ti_map_type::iterator i = 
      BaseInfoBaseImpl::s_bi_by_ti->find (tinfo);
    if (i != BaseInfoBaseImpl::s_bi_by_ti->end() && 
        !i->second->m_impl->m_needs_init)
      i->second->m_impl->m_needs_init = true;
  }
}


/**
 * @brief Run initializations for this class, if needed.
 */
void BaseInfoBase::maybeInit()
{
  if (m_impl->m_needs_init)
    find (*m_impl->m_typeinfo);
}



/**
 * @brief Try to translate a type_info to a CLID by looking in the initlist.
 * @param tinfo The type to translate.
 * @returns The corresponding CLID, or CLID_NULL.
 */
CLID BaseInfoBase::clid_from_initlist (const std::type_info& tinfo)
{
  if (!BaseInfoBaseImpl::s_init_list) return CLID_NULL;
  size_t sz = BaseInfoBaseImpl::s_init_list->size();
  for (size_t i = 0; i < sz; i++) {
    const BaseInfoBaseImpl::init_elem_t& elt =
      (*BaseInfoBaseImpl::s_init_list)[i];
    if (elt.m_ti == &tinfo)
      return elt.m_clid;
  }
  return CLID_NULL;
}


/// Declare the static members of @c BaseInfoBaseImpl.
BaseInfoBaseImpl::bi_by_clid_map_type*      BaseInfoBaseImpl::s_bi_by_clid = 0;
BaseInfoBaseImpl::bi_by_ti_map_type*        BaseInfoBaseImpl::s_bi_by_ti = 0;
std::vector<BaseInfoBaseImpl::init_elem_t>* BaseInfoBaseImpl::s_init_list = 0;
//boost::mutex                                BaseInfoBaseImpl::s_mutex;

// To get them deleted.
BaseInfoBaseImpl::Deleter BaseInfoBaseImpl::s_deleter;
BaseInfoBaseImpl::Deleter::~Deleter()
{
  delete s_bi_by_clid;
  delete s_bi_by_ti;
  delete s_init_list;
}

} // namespace SG
