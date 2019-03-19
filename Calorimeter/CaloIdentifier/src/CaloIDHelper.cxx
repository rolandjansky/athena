/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/CaloIDHelper.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Base class to factor out code common among Calo ID helpers.
 */


#include "CaloIdentifier/CaloIDHelper.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/Range.h"
#include "GaudiKernel/MsgStream.h"
#include "boost/foreach.hpp"



/**
 * @brief Constructor.
 *        Starts in an uninitialized state.
 */
CaloIDHelper::HashGroup::HashGroup ()
  : m_end_index(99999)
{
}


/**
 * @brief Initialize.
 * @param name Name of this group (only for error reporting).
 * @param ids Set of Identifiers comprising this group.
 * @param end_index The ending index for the context for this group.
 * @parma msgSvc The global message service
 *               (may be 0, only for error reporting).
 * @param full_range The @c MultiRange corresponding to this group.
 *                   If supplied, the side of the @c ids set is checked
 *                   against the range's cardinality.
 * @return 0 on success; non-zero on failure.
 */
int CaloIDHelper::HashGroup::init (const std::string& name,
                                   const std::set<Identifier>& ids,
                                   size_type end_index,
                                   IMessageSvc* msgSvc,
                                   const MultiRange* full_range)
{
  m_end_index = end_index;
  if (full_range && full_range->cardinality() != ids.size()) {
    MsgStream log (msgSvc, "CaloIDHelper");
    log << MSG::ERROR  << name << " set size " << ids.size()
        << " not equal to hash max " << full_range->cardinality() << endmsg;
    return 1;
  }
  m_id_vec.assign (ids.begin(), ids.end());
  return 0;
}


/**
 * @brief Look up the hash code corresponding ot an Identifier.
 *        Uses binary search.
 * @param id The identifier to look up.
 * @param[out] hash_id The corresponding hash code.
 * @return 0 if the Identifier was found; non-zero otherwise.
 */
int CaloIDHelper::HashGroup::get_hash (Identifier id,
                                       IdentifierHash& hash_id) const
{
  std::vector<Identifier>::const_iterator it = std::lower_bound(m_id_vec.begin(),m_id_vec.end(),id);
  if ( it != m_id_vec.end() ){
    hash_id = it - m_id_vec.begin();
    return 0;
  }
  return 1;
}


/** 
 * @brief Return the context for this group.
 */
IdContext CaloIDHelper::HashGroup::context() const
{
  ExpandedIdentifier id;
  return IdContext (id, 0, m_end_index);
}


//*************************************************************************


/**
 * @brief Constructor.  Makes an uninitialized helper.
 *        To initialize, call @c initialize_base_from_dictionary,
 *        followed by @c init_channels and @c init_regions.
 * @param name Name of this helper (for error reporting).
 */
CaloIDHelper::CaloIDHelper (const std::string& name)
  : m_name (name),
    m_dict(0)
{
}


/**
 * @brief Convert a hash code to an Identifier for either channels or regions,
 *        depending on the context.
 *        If you know the context at compile-time, @c channel_id and
 *        @c region_id will be faster (but they don't do range checking).
 * @param hash_id The hash code to convert.
 * @param[out] id The resulting Identifier.  Will be invalid if there
 *                was an error.
 * @param context The context in which to do the conversion
 *                (specifies whether we want channels or regions).
 * @return 0 for success, non-zero for failure (unrecognized context,
 *         or hash code out of range).
 */
int CaloIDHelper::get_id  (const IdentifierHash& hash_id,
                           Identifier& id,
                           const IdContext* context ) const
{
  int result = 1;
  id.clear();

  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) { 
    const HashGroup* hg = 0;
    if (m_channels.end_index() == end) hg = &m_channels;
    else if (m_regions.end_index() == end) hg = &m_regions;

    if (hg) {
      if (hash_id < hg->hash_max()) {
	id = hg->id (hash_id);
	result = 0;
      }
    }
  }

  return(result);
}


/**
 * @brief Convert an Identifier to a hash code for either channels or regions,
 *        depending on the context.
 *        Uses binary search.
 *        Some of the subdetector-specific helpers define faster
 *        versions of @c channel_hash and @c region_hash (that,
 *        however, do less checking).
 * @param id The identifier to convert.
 * @param[out] hash_id The resulting hash code.  Will be invalid if there
 *                was an error.
 * @param context The context in which to do the conversion
 *                (specifies whether we want channels or regions).
 * @return 0 for success, non-zero for failure (unrecognized context
 *         or Identifier.
 */
int CaloIDHelper::get_hash  (const Identifier& id, IdentifierHash& hash_id, const IdContext* context ) const
{
  hash_id = IdentifierHash();

  size_t begin = (context) ? context->begin_index(): 0;
  size_t end   = (context) ? context->end_index()  : 0; 

  if (0 == begin) { 
    const HashGroup* hg = 0;
    if (m_channels.end_index() == end) hg = &m_channels;
    else if (m_regions.end_index() == end) hg = &m_regions;

    if (hg)
      return hg->get_hash (id, hash_id);
  }

  return 1;
}


/**
 * @brief Return the eta granularity of a region, or @c NOT_VALID.
 * @param regHash REGION hash code.
 */
float CaloIDHelper::etaGranularity(const IdentifierHash regHash) const
{
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return m_vecOfDictRegions[regHash]->m_deta;
}


/**
 * @brief Return the phi granularity of a region, or @c NOT_VALID.
 * @param regHash REGION hash code.
 */
float CaloIDHelper::phiGranularity(const IdentifierHash regHash) const
{
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return  2. * M_PI / m_vecOfDictRegions[regHash]->m_dphi ;
}


/**
 * @brief Return the minimum eta of region, or @c NOT_VALID.
 * @param regHash REGION hash code.
 */
float CaloIDHelper::eta0(const IdentifierHash regHash) const
{
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return (m_vecOfDictRegions[regHash]->m_eta0);
}


/**
 * @brief Return the minimum phi of region, or @c NOT_VALID.
 * @param regHash REGION hash code.
 */
float CaloIDHelper::phi0(const IdentifierHash regHash) const
{
  if (regHash >= m_vecOfDictRegions.size()) return NOT_VALID;
  return (m_vecOfDictRegions[regHash]->m_phi0);
}


/**
 * @brief Do basic initialization of the helper.
 * @param dict_mgr  The dictionary manager returned from the parser.
 * @param dict_name The dictionary name (eg, `LArCalorimeter').
 * @return 0 on success, non-zero on failure.
 */
int
CaloIDHelper::initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                               const std::string& dict_name)
{
  // init base object
  if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

  // Register version of dictionary 
  if (register_dict_tag(dict_mgr, dict_name)) return(1);

  m_dict = dict_mgr.find_dictionary (dict_name);
  if(!m_dict) {
    MsgStream log(m_msgSvc, "CaloIDHelper" );
    log << MSG::ERROR << " cannot access " << dict_name <<  "dictionary"
        << endmsg;
    return 1;
  }
  return 0;
}


/**
 * @brief Initialize the list of detector regions.
 *        @c init_regions should have been called first.
 * @param group_name The group for this helper.
 * @return 0 on success; non-zero on failure.
 */
int
CaloIDHelper::fill_vec_of_dict_regions (const std::string& group_name /*= ""*/)
{
  m_vecOfDictRegions.clear();
  m_vecOfDictRegions.reserve (regions().hash_max());
  IdContext region_cntxt = region_context();
  ExpandedIdentifier expRegId;
  BOOST_FOREACH (Identifier id, regions().range()) {
    if(!get_expanded_id(id, expRegId, &region_cntxt)) {
      m_vecOfDictRegions.push_back (m_dict->find_region(expRegId,group_name));
    }
  }
  // cppcheck-suppress assertWithSideEffect
  assert (m_vecOfDictRegions.size() == regions().hash_max());
  return 0;
}


