// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersInterfaces/AuxDataOption.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2014
 * @brief Hold information about an option setting request.
 */


#ifndef ATHCONTAINERSINTERFACES_AUXDATAOPTION_H
#define ATHCONTAINERSINTERFACES_AUXDATAOPTION_H


#include <string>


namespace SG {


/**
 * @brief Hold information about an option setting request.
 *
 * This is used to pass an option setting through the @c setOption
 * interface, which is used to decouple the user interface from the
 * auxiliary store representation classes.
 *
 * This class holds a name and a value, which can be either an int
 * or a float.
 */
class AuxDataOption
{
public:
  /**
   * @brief Constructor, with an integer value.
   * @param name The option name.
   * @param val The option value.
   */
  AuxDataOption (const std::string& name, int val);


  /**
   * @brief Constructor, with a float value.
   * @param name The option name.
   * @param val The option value.
   */
  AuxDataOption (const std::string& name, float val);


  /**
   * @brief Constructor, with a double value.
   * @param name The option name.
   * @param val The option value.
   */
  AuxDataOption (const std::string& name, double val);


  /**
   * @brief The name of the option.
   */
  std::string name() const;


  /**
   * @brief Return the option value as an integer.
   */
  int intVal() const;


  /**
   * @brief Return the option value as a float.
   */
  float floatVal() const;


private:
  /// The option name.
  std::string m_name;

  /// Are we holding an int or a float?
  bool m_isInt;

  /// The stored value
  union {
    float f;
    int i;
  } m_val;
};


} // namespace SG


#include "AthContainersInterfaces/AuxDataOption.icc"


#endif // not ATHCONTAINERSINTERFACES_AUXDATAOPTION_H
