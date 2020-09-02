/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  ToolConstants.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Container for the tool constants managed by @c ToolWithConstants.
 */


#include "CaloConditions/ToolConstants.h"
#include "GaudiKernel/GaudiException.h"
#include <sstream>


namespace CaloRec {


/**
 * @brief Default constructor.
 */
ToolConstants::ToolConstants()
  : m_version (0)
{
}


/**
 * @brief Make a new entry.
 * @param context The context name, for error reporting.
 * @param key The key of the new entry.
 * @param val The value of the new entry, as a string.
 *
 * Converts @a val to an @c Arrayrep and stores it.
 * Raises an exception if the key already exists or if there's
 * a conversion error.
 */
const CxxUtils::Arrayrep&
ToolConstants::newrep (const std::string& context,
                       const std::string& key,
                       const std::string& val)
{
  Maptype::const_iterator i = m_map.find (key);
  if (i != m_map.end())
    error (context, key, "Duplicate key");
  return m_map[key] = CxxUtils::Arrayrep (val, context+":"+key);
}
 
/**
 * @brief Look up an entry.
 * @param context The context name, for error reporting.
 * @param key The key of the new entry.
 *
 * Looks up @a key and returns its @c Arrayrep.
 * Raises an exception if @a key isn't found.
 */
const CxxUtils::Arrayrep&
ToolConstants::getrep (const std::string& context,
                       const std::string& key) const
{
  Maptype::const_iterator i = m_map.find (key);
  if (i == m_map.end()) {
    std::ostringstream ss;
    for (const auto& p : m_map) {
      ss << " " << p.first;
    }
    error (context, key, "Can't find key in" + ss.str());
  }
  return i->second;
}


/**
 * @brief Set an entry.
 * @param key The key of the entry to set.
 * @param rep The value of the new entry.
 */
void ToolConstants::setrep (const std::string& key,
                            const CxxUtils::Arrayrep& rep)
{
  m_map[key] = rep;
}


/**
 * @brief Set an entry.
 * @param key The key of the entry to set.
 * @param rep The value of the new entry.
 */
void ToolConstants::setrep (const std::string& key,
                            CxxUtils::Arrayrep&& rep)
{
  m_map[key] = std::move (rep);
}


/**
 * @brief Test to see if a given key is present.
 */
bool ToolConstants::hasrep (const std::string& key) const
{
  return m_map.find (key) != m_map.end();
}


/**
 * @brief Report an error.
 * @param context Context string for the error.
 * @param key Key involved in the error.
 * @param msg Error message.
 */
void ToolConstants::error (const std::string& context,
                           const std::string& key,
                           const std::string& msg)
{
  throw GaudiException (msg, context+":"+key, StatusCode::FAILURE);
}

/**
 * @brief Writes out constants in a python-like format
 * @param stream Stream to wirte to (file or cout)
 * @param name Name of the Maker-Algorithm (used only for output) 
 */
void ToolConstants::writeConstants(std::ostream& stream,
                                   const std::string& name) const
{
  Maptype::const_iterator it=m_map.begin();
  Maptype::const_iterator it_e=m_map.end();
  for (;it!=it_e;it++) {
    stream << name << "." << it->first << " = ";
    it->second.write_array(stream);
  }
  stream << std::endl;
}


/**
 * @brief Return the data as a formatted string.
 * @param name Name of the Maker-Algorithm.
 */
std::string ToolConstants::toString (const std::string& name) const
{
  std::ostringstream ss;
  writeConstants (ss, name);
  return ss.str();
}


/**
 * @brief Return the name of the C++ class that operates on these constants.
 */
const std::string& ToolConstants::clsname() const
{
  return m_clsname;
}


/**
 * @brief Set the name of the C++ class that operates on these constants.
 */
void ToolConstants::clsname (const std::string& name)
{
  m_clsname = name;
}


/**
 * @brief Return the version of the C++ class that operates on these constants.
 */
int ToolConstants::version() const
{
  return m_version;
}


/**
 * @brief Set the version of the C++ class that operates on these constants.
 */
void ToolConstants::version (int version)
{
  m_version = version;
}


/**
 * @brief Return the key -> value map.
 */
const ToolConstants::Maptype& ToolConstants::map() const
{
  return m_map;
}


} // namespace CaloRec
