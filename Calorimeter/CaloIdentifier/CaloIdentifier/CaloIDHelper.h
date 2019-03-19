// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/CaloIDHelper.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Base class to factor out code common among Calo ID helpers.
 */


#ifndef CALOIDENTIFIER_CALOIDHELPER_H
#define CALOIDENTIFIER_CALOIDHELPER_H


#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "boost/range/iterator_range.hpp"
#include <vector>
#include <set>
class MultiRange;
class IMessageSvc;
class IdDictRegion;


/**
 * @brief Base class to factor out code common among Calo ID helpers.
 */
class CaloIDHelper
  : public AtlasDetectorID
{
public:
  enum { NOT_VALID=999999 };


  typedef Identifier::size_type  size_type ;

  /** Type for iterators over identifiers. */
  typedef std::vector<Identifier>::const_iterator id_iterator;
  /** Type for range over identifiers. */
  typedef boost::iterator_range<id_iterator> id_range;



  /**
   * @brief Manage a list of identifiers.
   *        The identifiers are kept in sorted order, and the index
   *        of an identifier in this list defines its hash value.
   *        We supply methods for converting between identifiers
   *        and the hash value; we also keep track of the corresponding
   *        ID context.
   *
   *        Most calo ID helpers will use two of these: one for
   *        channels (cells) and one for regiosn.
   */
  class HashGroup
  {
  public:
    /**
     * @brief Constructor.
     *        Starts in an uninitialized state.
     */
    HashGroup();


    /**
     * @brief Initialize.
     * @param parent The parent ID helper (in which @c idfunc is defined).
     * @param type A string to add on to the name.
     * @param full_range The @c MultiRange for to this group.
     * @param idfunc Function to convert from an @c ExpandedIdentifier
     *               to an @c Identifier for this group.
     * @param end_index The ending index for the context for this group.
     * @return 0 on success; non-zero on failure.
     */
    template <class T>
    int init (T& parent,
              const std::string& type,
              const MultiRange& full_range,
              Identifier (T::*idfunc) (const ExpandedIdentifier&) const,
              size_type end_index);
              

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
    int init (const std::string& name,
              const std::set<Identifier>& ids,
              size_type end_index,
              IMessageSvc* msgSvc,
              const MultiRange* full_range = 0);


    /// Return one more than the largest hash code.
    size_type hash_max() const;

    /// Return a begin iterator over the group's Identifiers.
    id_iterator begin() const;
    /// Return an end iterator over the group's Identifiers.
    id_iterator end()   const;
    /// Return an iterator range over the group's Identifiers.
    id_range    range() const;

    /// Return the identifier for a given hash code (no checking).
    Identifier id (IdentifierHash hashId) const;

    
    /**
     * @brief Look up the hash code corresponding to an Identifier.
     *        Uses binary search.
     * @param id The identifier to look up.
     * @param[out] hash_id The corresponding hash code.
     * @return 0 if the Identifier was found; non-zero otherwise.
     */
    int get_hash (Identifier id, IdentifierHash& hash_id) const;


    /**
     * @brief Look up the hash code corresponding to an Identifier.
     *        Uses binary search.
     * @param id The identifier to look up.
     * @return The corresponding hash code.
     *         An invalid IdentifierHash will be returned if the Identifier
     *         is not found.
     */
    IdentifierHash hash (Identifier id) const;

    /// Return a vector of all Identifiers for this group.
    const std::vector<Identifier>& ids() const;

    /// Return the ending index of the context for this group.
    size_type end_index() const;

    /// Return the context for this group.
    IdContext context() const;


  private:
    /// List of Identifiers for this group.
    std::vector<Identifier> m_id_vec;

    /// Ending index for this group's context.
    size_type m_end_index;
  };


  /**
   * @brief Constructor.  Makes an uninitialized helper.
   *        To initialize, call @c initialize_base_from_dictionary,
   *        followed by @c init_channels and @c init_regions,
   *        and optionally @c fill_vec_of_dict_regions.
   * @param name Name of this helper (for error reporting).
   */
  CaloIDHelper (const std::string& name);


  /// Return the @c HashGroup for channels (cells).
  const HashGroup& channels() const;

  /// Return the @c HashGroup for regions.
  const HashGroup& regions() const;

  /// Return the channel (cell) Identifier for a given hash code (no checking).
  Identifier channel_id (IdentifierHash hashId) const;
  /// Return the region Identifier for a given hash code (no checking).
  Identifier region_id (IdentifierHash hashId) const;


  /**
   * @brief Convert a connected channel (cell) Identifier to a hash code.
   *        Some subdetector helpers may override this with a faster version.
   */
  IdentifierHash channel_hash (Identifier channelId) const;


  /**
   * @brief Convert a connected region Identifier to a hash code.
   *        Some subdetector helpers may override this with a faster version.
   */
  IdentifierHash region_hash (Identifier regionId) const;


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
  virtual int get_id (const IdentifierHash& hash_id,
                      Identifier& id,
                      const IdContext* context = 0 ) const;


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
  virtual int  get_hash  (const Identifier& id,
                          IdentifierHash& hash_id,
                          const IdContext* context = 0 ) const;


  /// One more than the largest channel (cell) hash code.
  size_type channel_hash_max() const;
  /// One more than the largest region hash code.
  size_type region_hash_max() const;


  /// Return the context for channels (cells).
  IdContext channel_context() const;
  /// Return the context for regions.
  IdContext region_context() const;


  /// Return the vector of @c IdDictRegion, accessed via region hash.
  const std::vector<const IdDictRegion*>& dictRegions() const;


  /**
   * @brief Return the eta granularity of a region, or @c NOT_VALID.
   * @param regHash REGION hash code.
   */
  float etaGranularity(const IdentifierHash regHash) const;


  /**
   * @brief Return the phi granularity of a region, or @c NOT_VALID.
   * @param regHash REGION hash code.
   */
  float phiGranularity(const IdentifierHash regHash) const;


  /**
   * @brief Return the minimum eta of region, or @c NOT_VALID.
   * @param regHash REGION hash code.
   */
  float eta0(const IdentifierHash regHash) const;


  /**
   * @brief Return the minimum phi of region, or @c NOT_VALID.
   * @param regHash REGION hash code.
   */
  float phi0(const IdentifierHash regHash) const;


  /// Return the name for this helper.
  std::string name() const;

  /// Return the message service for this helper (may be null).
  IMessageSvc* msgSvc();



protected:
  /**
   * @brief Do basic initialization of the helper.
   * @param dict_mgr  The dictionary manager returned from the parser.
   * @param dict_name The dictionary name (eg, `LArCalorimeter').
   * @return 0 on success, non-zero on failure.
   */
  int initialize_base_from_dictionary (const IdDictMgr& dict_mgr,
                                       const std::string& dict_name);


  /// Return the @c HashGroup for channels (cells).  non-const.
  HashGroup& channels();
  /// Return the @c HashGroup for regions.  non-const.
  HashGroup& regions();


  /// Return the dictionary for this subdetector.
  const IdDictDictionary* dict() const;


  /**
   * @brief Initialize the list of detector regions.
   *        @c init_regions should have been called first.
   * @param group_name The group for this helper.
   * @return 0 on success; non-zero on failure.
   */
  int fill_vec_of_dict_regions (const std::string& group_name = "");


  /**
   * @brief Create an expanded Identifier from an Identifier.
   * @return 0 on success; non-zero on failure.
   * @param id Identifier to convert.
   * @param[out] exp_id The expanded Identifier.
   * @param context The context in which to do the conversion
   *                (specifies whether we want channels or regions).
   */
  virtual int get_expanded_id (const Identifier& id,
                               ExpandedIdentifier& exp_id,
                               const IdContext* context) const = 0;


private:
  /// Name of this helper.
  std::string m_name;

  /// Group of channel (cell) Identifiers.
  HashGroup m_channels;

  /// Group of region Identifiers.
  HashGroup m_regions;

  /// The dictionary for this helper.
  const IdDictDictionary* m_dict;

  /// List of @c IdDictRegion objects.
  std::vector<const IdDictRegion*> m_vecOfDictRegions;
};


#include "CaloIdentifier/CaloIDHelper.icc"


#endif // not CALOIDENTIFIER_CALOIDHELPER_H
