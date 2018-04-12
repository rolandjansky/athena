#ifndef PATINTERFACES_SYSTEMATIC_SET_H
#define PATINTERFACES_SYSTEMATIC_SET_H

// Author: Steve Farrell (steven.farrell@cern.ch)

// This module implements a class that represents a set of systematics
// in a way to facilitate efficient lookups by systematic tools.
// A hash value is cached using the joined string name and boost::hash<string>.

#include <PATInterfaces/Global.h>

#include <set>
#include <vector>
#include <string>
#include <functional>

#include <PATInterfaces/SystematicCode.h>
#include <PATInterfaces/SystematicVariation.h>


namespace CP
{

  // Pulling global ops into CP. Not ideal.
  using ::operator<;
  using ::operator==;

  /// Class to wrap a set of SystematicVariations
  class SystematicSet
  {

  public:
    // Public constructors

    /// construct an empty set
    SystematicSet();
    /// construct set by splitting single systematics string
    SystematicSet(const std::string& systematics);
    /// construct by vector of systematic names
    SystematicSet(const std::vector<std::string>& systematics);
    /// construct by vector of SystematicVariation
    SystematicSet(const std::vector<SystematicVariation>& systematics);

    /// construct with an std::initializer_list
    #ifndef __CINT__
    #if __cplusplus >= 201100
    SystematicSet(const std::initializer_list<SystematicVariation>& systematics);
    #endif
    #endif

  public:
    // Public set-interface methods

    typedef std::set<SystematicVariation>::iterator iterator;
    typedef std::set<SystematicVariation>::const_iterator const_iterator;

    /// description: const iterator to the beginning of the set
    const_iterator begin() const
    { return m_sysVariations.begin(); }

    /// description: const iterator to the end of the set
    const_iterator end() const
    { return m_sysVariations.end(); }

    /// description: find an element in the set
    iterator find(const SystematicVariation& sys) const
    { return m_sysVariations.find(sys); }

    /// returns: whether the set is empty
    bool empty() const
    { return m_sysVariations.empty(); }

    /// returns: size of the set
    size_t size() const
    { return m_sysVariations.size(); }

    /// description: insert a systematic into the set
    void insert(const SystematicVariation& systematic);

    /// description: insert a systematic set into this set
    void insert(const SystematicSet& systematics);

    /// description: swap elements of a set
    void swap(SystematicSet& otherSet);

    /// description: clear the set
    void clear();

  public:
    // Specialized search and filtering methods

    /// description: match systematic or continuous version
    enum MATCHTYPE { FULL, FULLORCONTINUOUS };
    bool matchSystematic(const SystematicVariation& systematic,
                         MATCHTYPE type=FULL) const;

    /// description: get the subset of systematics matching basename
    /// Should this return a SystematicCode instead?
    SystematicSet filterByBaseName(const std::string& basename) const;

    /// description: get the set of base systematic names from this set
    std::set<std::string> getBaseNames() const;

    /// description: get the first systematic matching basename
    SystematicVariation
    getSystematicByBaseName(const std::string& basename) const;

    /// returns: the parameter value for the given basename
    float
    getParameterByBaseName(const std::string& basename) const;

    /// \brief the toy variation for the given basename
    ///
    /// in case there is no toy variation for the given base name,
    /// this returns (0,0)
    /// \sa SystematicVariation::getToyVariation
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   not a toy variation\n
    ///   parse errors\n
    ///   out of memory II
  public:
    std::pair<unsigned,float>
    getToyVariationByBaseName (const std::string& basename) const;

    /// description: filter the systematics for the affected systematics
    /// returns: success
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: requesting multiple variations on the same systematic
    ///   (e.g. up & down)
    /// failures: requesting an unsupported variation on an otherwise
    ///   supported systematic (e.g. a 2 sigma variation and the tool
    ///   only supports 1 sigma variations)
    static SystematicCode
    filterForAffectingSystematics (const SystematicSet& systConfig,
                                   const SystematicSet& affectingSystematics,
                                   SystematicSet& filteredSystematics);

  public:
    // Public name and hash methods

    /// returns: the systematics joined into a single string.
    ///   Caches the name if not already done
    std::string name() const;

    /// returns: hash value for the joined string.
    ///   Caches the hash if not already done
    std::size_t hash() const;

  private:
    // Private modification methods

    /// description: join systematic names into single string
    std::string joinNames() const;

    /// description: compute and store the hash value
    std::size_t computeHash() const;

  private:
    // Private members

    /// description: the set of systematics encapsulated in this class
    std::set<SystematicVariation> m_sysVariations;

    /// description: cache the joined string, useful for hash
    mutable std::string m_joinedName;
    mutable bool m_nameIsCached;

    /// description: cached hash value for quick retrieval in
    /// unordered containers
    mutable std::size_t m_hash;
    mutable bool m_hashIsCached;

  };


  /// SystematicSet hash function for general use
  struct SystematicSetHash
    : public std::unary_function<SystematicSet, std::size_t>
  {
    std::size_t operator()(const SystematicSet&);
  };

  /// Hash function specifically for boost::hash
  std::size_t hash_value(const SystematicSet&);

  /// Sorting operator for std::map
  //bool operator < (const SystematicSet& a, const SystematicSet& b);
  /// Equality operator for unordered containers
  //bool operator == (const SystematicSet& a, const SystematicSet& b);

}

namespace std
{
  /// Specialization of std::hash for std::unordered_map
  #if __cplusplus >= 201100
  template<> struct hash<CP::SystematicSet>
  {
    std::size_t operator()(const CP::SystematicSet& sysSet) const
    { return sysSet.hash(); }
  };
  #endif
}

#endif
