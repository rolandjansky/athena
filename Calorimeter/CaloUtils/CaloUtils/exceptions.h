// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloUtils/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Exceptions that can be thrown from CaloUtils.
 */


#ifndef CALOUTILS_EXCEPTIONS_H
#define CALOUTILS_EXCEPTIONS_H


#include <stdexcept>
#include <string>


namespace CaloUtils {


/**
 * @brief Exception --- version mismatch for tool.
 *
 * The version of the constants read from COOL was larger than the
 * version of the tool being used.
 */
class ExcBadToolConstantVersion
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor
   * @param toolName Name of the tool being used.
   * @param constName Name of the constant being accessed.
   * @param toolVersion Version of the tool.
   * @param constVersion Version of the constants read from COOL.
   */
  ExcBadToolConstantVersion (const std::string& toolName,
                             const std::string& constName,
                             unsigned int toolVersion,
                             unsigned int constVersion);
};


/**
 * @brief Exception --- constant not set
 *
 * The constant was not set from JO, not found in COOL, and has no default.
 */
class ExcConstantNotSet
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor
   * @param toolName Name of the tool being used.
   * @param constName Name of the constant being accessed.
   */
  ExcConstantNotSet (const std::string& toolName,
                     const std::string& constName);
};


/**
 * @brief Exception --- constant set more than once.
 *
 * A constant should only be set once from JO.
 */
class ExcConstantReset
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor
   * @param toolName Name of the tool being used.
   * @param constName Name of the constant being set.
   */
  ExcConstantReset (const std::string& toolName,
                    const std::string& constName);
};


/**
 * @brief Exception --- Bad contextless retrieve.
 *
 * The Constant::operator() overload that does not take a Context argument
 * may be used only for constants that were initialzed from job options.
 */
class ExcBadContextlessRetrieve
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor
   * @param toolName Name of the tool being used.
   * @param constName Name of the constant being retrieved.
   */
  ExcBadContextlessRetrieve (const std::string& toolName,
                             const std::string& constName);
};



/**
 * @brief Throw a CaloUtils::ExcBadContextlessRetrieve exception.
 * @param toolName Name of the tool being used.
 * @param constName Name of the constant being retrieved.
 */
[[noreturn]]
void throwExcBadContextlessRetrieve (const std::string& toolName,
                                     const std::string& constName);


} // namespace CaloUtils


#endif // not CALOUTILS_EXCEPTIONS_H
