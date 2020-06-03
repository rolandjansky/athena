/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloUtils/src/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Exceptions that can be thrown from CaloUtils.
 */


#include "CaloUtils/exceptions.h"
#include <sstream>


namespace CaloUtils {


/// Helper: Format exception string.
std::string excBadToolConstantVersion_format (const std::string& toolName,
                                              const std::string& constName,
                                              unsigned int toolVersion,
                                              unsigned int constVersion)
{
  std::ostringstream os;
  os << "ExcBadToolConstantVersion: version mismatch for tool " << toolName
     << " constant " << constName
     << " constant version " << constVersion
     << " is larger than tool version " << toolVersion;
  return os.str();
}


/**
 * @brief Constructor
 * @param toolName Name of the tool being used.
 * @param constName Name of the constant being accessed.
 * @param toolVersion Version of the tool.
 * @param constVersion Version of the constants read from COOL.
 */
ExcBadToolConstantVersion::ExcBadToolConstantVersion
  (const std::string& toolName,
   const std::string& constName,
   unsigned int toolVersion,
   unsigned int constVersion)
    : std::runtime_error (excBadToolConstantVersion_format (toolName,
                                                            constName,
                                                            toolVersion,
                                                            constVersion))
{
}


//*************************************************************************


/// Helper: Format exception string.
std::string excConstantNotSet_format (const std::string& toolName,
                                      const std::string& constName)
{
  std::ostringstream os;
  os << "ExcConstantNotSet: constant " << constName << " in tool " << toolName
     << " was not set.";
  return os.str();
}


/**
 * @brief Constructor
 * @param toolName Name of the tool being used.
 * @param constName Name of the constant being accessed.
 */
ExcConstantNotSet::ExcConstantNotSet
  (const std::string& toolName,
   const std::string& constName)
    : std::runtime_error (excConstantNotSet_format (toolName,
                                                    constName))
{
}


//*************************************************************************


/// Helper: Format exception string.
std::string excConstantReset_format (const std::string& toolName,
                                     const std::string& constName)
{
  std::ostringstream os;
  os << "ExcConstantReset: constant " << constName << " in tool " << toolName
     << " set more than once.";
  return os.str();
}


/**
 * @brief Constructor
 * @param toolName Name of the tool being used.
 * @param constName Name of the constant being set.
 */
ExcConstantReset::ExcConstantReset
  (const std::string& toolName,
   const std::string& constName)
    : std::runtime_error (excConstantReset_format (toolName,
                                                   constName))
{
}


} // namespace CaloUtils
