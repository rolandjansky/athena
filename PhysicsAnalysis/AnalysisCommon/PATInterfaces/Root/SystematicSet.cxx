// Author: Steve Farrell (steven.farrell@cern.ch)


//
// includes
//

#include <stdexcept>

#include <boost/functional/hash.hpp>

#include <PATInterfaces/SystematicSet.h>
#include <PATInterfaces/MessageCheck.h>

// Comparison operator for std::map sorting
bool operator < (const CP::SystematicSet& a, const CP::SystematicSet& b)
{
  return a.name() < b.name();
}

// Equality operator for unordered containers
bool operator == (const CP::SystematicSet& a, const CP::SystematicSet& b)
{
  return a.name() == b.name();
}


//
// method implementations
//

namespace CP
{

  // Default constructor
  SystematicSet::SystematicSet()
        : m_joinedName(""),
          m_nameIsCached(false),
          m_hash(0),
          m_hashIsCached(false)
  {
  }

  // Constructor for splitting a single systematics string
  SystematicSet::SystematicSet(const std::string& systematics)
        : SystematicSet()
  {
    std::string::size_type split = 0, split2 = 0;
    while ((split = systematics.find ("-", split2)) != std::string::npos) {
      m_sysVariations.insert (systematics.substr (split2, split - split2));
      split2 = split + 1;
    }
    m_sysVariations.insert (systematics.substr (split2, split - split2));
  }


  // Constructor for a vector of systematic names
  SystematicSet::SystematicSet(const std::vector<std::string>& systematics)
        : SystematicSet()
  {
    for (const auto & sys : systematics) {
      m_sysVariations.insert(sys);
    }
  }

  // Constructor for a vector of SystematicVariation
  SystematicSet::SystematicSet
  (const std::vector<SystematicVariation>& systematics)
        : SystematicSet()
  {
    for (const auto & sys : systematics) {
      m_sysVariations.insert(sys);
    }
  }


  // Constructor for an initializer_list of SystematicVariation
  SystematicSet::SystematicSet
  (const std::initializer_list<SystematicVariation>& systematics)
        : SystematicSet()
  {
    for (const auto & sys : systematics) {
      m_sysVariations.insert(sys);
    }
  }


  // Insert a systematic into the set
  void SystematicSet::insert(const SystematicVariation& systematic)
  {
    // If insert doesn't change the set, don't change cache flag
    if(m_sysVariations.insert(systematic).second) {
      m_nameIsCached = m_hashIsCached = false;
    }
  }


  // Insert a SystematicSet into this set
  void SystematicSet::insert(const SystematicSet& systematics)
  {
    for (const auto & sys : systematics) {
      insert(sys);
    }
  }


  // Swap elements of the set
  void SystematicSet::swap(SystematicSet& otherSet)
  {
    m_sysVariations.swap(otherSet.m_sysVariations);
    m_nameIsCached = m_hashIsCached = false;
    otherSet.m_nameIsCached = false;
    otherSet.m_hashIsCached = false;
  }


  // Clear the systematics and the rest of the state
  void SystematicSet::clear()
  {
    m_sysVariations.clear();
    m_joinedName.clear();
    m_hash = 0;
    m_nameIsCached = m_hashIsCached = false;
  }


  // Match full systematic or continuous systematic
  bool SystematicSet::matchSystematic(const SystematicVariation& systematic,
                                      MATCHTYPE type) const
  {
    if (m_sysVariations.find(systematic) != m_sysVariations.end()) {
      return true;
    }
    if (type == FULLORCONTINUOUS) {
      const SystematicVariation continuous(systematic.basename(),
                                           SystematicVariation::CONTINUOUS);
      if (m_sysVariations.find(continuous) != m_sysVariations.end()) {
        return true;
      }
    }
    return false;
  }


  // Get subset of systematics matching basename
  SystematicSet SystematicSet::filterByBaseName
  (const std::string& basename) const
  {
    SystematicSet filteredSysts;
    for (const auto & sys : m_sysVariations) {
      if (sys.basename() == basename)
        filteredSysts.insert(sys);
    }
    return filteredSysts;
  }


  // Get the set of systematic base names
  std::set<std::string> SystematicSet::getBaseNames() const
  {
    std::set<std::string> baseNames;
    for (const auto & sys : m_sysVariations) {
      baseNames.insert(sys.basename());
    }
    return baseNames;
  }


  // Get the first systematic that matches basename
  SystematicVariation
  SystematicSet::getSystematicByBaseName(const std::string& basename) const
  {
    const SystematicVariation* sysMatched = NULL;
    for (const auto & sys : m_sysVariations) {
      if(sys.basename() == basename) {
        if(!sysMatched) sysMatched = &sys;
        else {
          std::string error = "SystematicSet::getSystematicByBaseName ERROR: ";
          error += "Multiple matches for requested basename ";
          error += basename;
          // Redundant?
          //std::cerr << error << std::endl;
          throw std::runtime_error(error);
        }
      }
    }
    if(sysMatched) return *sysMatched;
    return SystematicVariation();
  }


  float SystematicSet ::
  getParameterByBaseName(const std::string& basename) const
  {
    return getSystematicByBaseName (basename).parameter();
  }



  std::pair<unsigned,float> SystematicSet ::
  getToyVariationByBaseName (const std::string& basename) const
  {
    const auto var = getSystematicByBaseName (basename);
    if (var.empty())
      return std::make_pair (0, 0);
    return var.getToyVariation();
  }



  // Filter requested systematics with affecting systematics
  SystematicCode SystematicSet::filterForAffectingSystematics
  (const SystematicSet& systConfig, const SystematicSet& affectingSysts,
   SystematicSet& filteredSysts)
  {
    using namespace msgSystematics;

    // the final filtered systematics to report
    SystematicSet result;

    // the map of all requested systematics by base name
    std::map<std::string,SystematicVariation> requestedMap;

    // the list of all inconsistent systematics we encountered
    std::set<SystematicVariation> inconsistentList;

    // fill requestedMap, reporting errors in case of duplication
    for (auto& sys : systConfig)
    {
      std::string basename = sys.basename();
      auto iter = requestedMap.find (basename);
      if (iter != requestedMap.end())
      {
	ANA_MSG_ERROR ("inconsistent systematic variations requested: " << sys << " and " << iter->second);
	return SystematicCode::Unsupported;
      }
      requestedMap.insert (std::make_pair (basename, sys));
    }

    // check which of the systematics match the affecting
    for (auto& sys : affectingSysts)
    {
      std::string basename = sys.basename();
      auto iter = requestedMap.find (basename);
      if (iter != requestedMap.end())
      {
	if (iter->second == sys ||
	    sys.ensembleContains (iter->second))
	{
	  result.insert (iter->second);
	} else
	{
	  // let's remember this as a potential problem
	  inconsistentList.insert (iter->second);
	}
      }
    }

    // check whether any of of the requested variations matched the
    // base names of our systematics, but not the systematics
    // supported
    for (auto& sys : inconsistentList)
    {
      if (result.find (sys) == result.end())
      {
	ANA_MSG_ERROR ("unsupported systematic variation " << sys << " requested for systematic " << sys.basename());
	return SystematicCode::Unsupported;
      }
    }

    // everything worked out, let's commit now
    result.swap (filteredSysts);
    return SystematicCode::Ok;
  }


  // Return the cached joined systematic name
  std::string SystematicSet::name() const
  {
    if(!m_nameIsCached)
    {
      m_joinedName = joinNames();
      m_nameIsCached = true;
    }
    return m_joinedName;
  }


  // Return the cached hash value
  std::size_t SystematicSet::hash() const
  {
    if(!m_hashIsCached) {
      m_hash = computeHash();
      m_hashIsCached = true;
    }
    return m_hash;
  }


  // Join systematics into a single string
  std::string SystematicSet::joinNames() const
  {
    std::string joinedName;
    for (const auto & sys : m_sysVariations) {
      if (!joinedName.empty()) {
        joinedName += "-";
      }
      joinedName += sys.name();
    }
    return joinedName;
  }


  // Compute the hash value for this SystematicSet
  std::size_t SystematicSet::computeHash() const
  {
    static std::hash<std::string> hashFunction;
    //static boost::hash<std::string> hashFunction;
    return hashFunction(name());
  }

  // General hash function for SystematicSet
  std::size_t SystematicSetHash::operator()(const SystematicSet& sysSet)
  {
    return sysSet.hash();
  }

  // Hash function for boost hash
  std::size_t hash_value(const SystematicSet& sysSet)
  {
    return sysSet.hash();
  }

}
