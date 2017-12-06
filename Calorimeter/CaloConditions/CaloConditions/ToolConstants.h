// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ToolConstants.h,v 1.5 2009-04-09 14:41:17 ssnyder Exp $
/**
 * @file  ToolConstants.h
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Container for the tool constants managed by @c ToolWithConstants.
 */


#ifndef CALOREC_TOOLCONSTANTS_H
#define CALOREC_TOOLCONSTANTS_H


#include "AthenaKernel/CLASS_DEF.h" 
#include "CxxUtils/Array.h"
#include <string>
#include <map>

namespace CaloRec {


/**
 * @class ToolConstants
 * @brief Container for the tool constants managed by @c ToolWithConstants.
 *
 * This is just a simple map from constant name to @c Arrayrep.
 */
class ToolConstants
{
public:
  /// Type of the map from keys to values.
  typedef std::map<std::string, CxxUtils::Arrayrep> Maptype;

  /// Default constructor.
  ToolConstants();


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
  const CxxUtils::Arrayrep& newrep (const std::string& context,
                                    const std::string& key,
                                    const std::string& val);

  /**
   * @brief Look up an entry.
   * @param context The context name, for error reporting.
   * @param key The key of the new entry.
   *
   * Looks up @a key and returns its @c Arrayrep.
   * Raises an exception if @a key isn't found.
   */
  const CxxUtils::Arrayrep& getrep (const std::string& context,
                                    const std::string& key) const;


  /**
   * @brief Set an entry.
   * @param key The key of the entry to set.
   * @param rep The value of the new entry.
   */
  void setrep (const std::string& key,
               const CxxUtils::Arrayrep& rep);


  /**
   * @brief Test to see if a given key is present.
   */
  bool hasrep (const std::string& key) const;

  /**
   * @brief Writes out constants in a python-like format
   * @param stream Stream to which to write (file or cout)
   * @param name Name of the Maker-Algorithm (used only for output) 
   */
  void writeConstants(std::ostream& stream, const std::string& name) const;


  /**
   * @brief Return the name of the C++ class that operates on these constants.
   */
  const std::string& clsname() const;


  /**
   * @brief Set the name of the C++ class that operates on these constants.
   */
  void clsname (const std::string& name);


  /**
   * @brief Return the version of the C++ class that operates
   *        on these constants.
   */
  int version() const;


  /**
   * @brief Set the version of the C++ class that operates on these constants.
   */
  void version (int version);


  /// Return the key -> value map.
  const Maptype& map() const;


private:
  /**
   * @brief Report an error.
   * @param context Context string for the error.
   * @param key Key involved in the error.
   * @param msg Error message.
   */
  static void error (const std::string& context,
                     const std::string& key,
                     const std::string& msg);

  /// The map of keys to values.
  Maptype m_map;

  /// Name of the C++ class that operates on these constants.
  std::string m_clsname;

  /// Version number of the C++ class that operates on these constants.
  int m_version;
};


} // namespace CaloRec


CLASS_DEF(CaloRec::ToolConstants,250904980 ,1)
#endif // not CALOREC_TOOLCONSTANTS_H
