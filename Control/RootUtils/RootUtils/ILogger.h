// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ILogger.h,v 1.1 2007-12-15 07:01:00 ssnyder Exp $

/**
 * @file  RootUtils/ILogger.h
 * @author scott snyder
 * @date Dec 2007
 * @brief Very simple interface for writing log messages.
 */

#ifndef ROOTUTILS_ILOGGER_H
#define ROOTUTILS_ILOGGER_H


namespace RootUtils {


/**
 * @brief Very simple interface for writing log messages.
 *
 * In a few places, we want to be able to generate logging messages,
 * but we also want to avoid dependencies on Gaudi.  This interface
 * can be used in such cases.
 *
 * The @c RootUtils::PyLogger class is a concrete implementation of this
 * that calls back to Python code.
 */
class ILogger
{
public:
  /// Make the destructor virtual.
  virtual ~ILogger() {}


  /**
   * @brief Log a debugging message.
   * @param msg The message to log.
   */
  virtual void debug (const char* msg) = 0;


  /**
   * @brief Log an error message.
   * @param msg The message to log.
   */
  virtual void error (const char* msg) = 0;
};


} // namespace RootUtils


#endif // not ROOTUTILS_ILOGGER_H
