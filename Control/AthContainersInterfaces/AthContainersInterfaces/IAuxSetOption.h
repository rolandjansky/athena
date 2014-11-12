// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file IAuxSetOption.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2014
 * @brief Abstract interface for setting a option on a aux data container.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXSETOPTION_H
#define ATHCONTAINERSINTERFACES_IAUXSETOPTION_H


namespace SG {


class AuxDataOption;


/**
 * @brief Abstract interface for setting a option on a aux data container.
 *
 * A container used to hold auxiliary data can optionally provide this
 * interface.  This allows user code to set options for the container
 * via the @c setOption interface.
 *
 * This is used, for example, to set packing parameters for @c PackedContainer.
 */
class IAuxSetOption
{
public:
  /// Make sure this class has a vtable.
  virtual ~IAuxSetOption() {}


  /**
   * @brief Make an option setting.
   * @param option The option to set.
   *
   * The interpretation of the option depends on the concrete class.
   *
   * Returns true if the option setting was successful; false otherwise.
   */
  virtual bool setOption (const AuxDataOption& option) = 0;
};


} // namespace SG


#endif // not ATHCONTAINERSINTERFACES_IAUXSETOPTION_H
