// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/exceptions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Exceptions that can be thrown from AthAllocators.
 */


#ifndef ATHALLOCATORS_EXCEPTIONS_H
#define ATHALLOCATORS_EXCEPTIONS_H


#include <stdexcept>


namespace SG {


/**
 * @brief Exception --- Attempt to assign between ArenaSharedHeapSTLAllocators 
 *                      for different arenas.
 */
class ExcDifferentArenas
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   */
  ExcDifferentArenas();
};


/**
 * @brief Exception --- Attempt to change memory protection failed.
 */
class ExcProtection
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   * @param errnum The system error code.
   */
  ExcProtection (int errnum);
};


/**
 * @brief Exception --- Attempt to change protected arena.
 */
class ExcProtected
  : public std::runtime_error
{
public:
  /**
   * @brief Constructor.
   */
  ExcProtected();
};


} // namespace SG


#endif // not ATHALLOCATORS_EXCEPTIONS_H
