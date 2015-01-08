// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/PackedContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2014
 * @brief Container to hold aux data to be stored in a packed form.
 */


#ifndef ATHCONTAINERS_PACKEDCONTAINER_H
#define ATHCONTAINERS_PACKEDCONTAINER_H


#include "AthContainersInterfaces/IAuxSetOption.h"
#include "AthContainers/PackedParameters.h"
#include "CxxUtils/override.h"
#include <vector>


namespace SG {


/**
 * @brief Container to hold aux data to be stored in a packed form.
 *
 * This class acts as a @c std::vector<T> (in fact, it derives from
 * that type).  However, we set up a special ROOT streamer for this
 * type so that when it is saved, it is written in a packed format.
 * The details of the packing are given by the @c PackedParameters (which see)
 * instance that we hold.
 *
 * User code should generally not interact with this class directly.
 *
 * @c PackedContainer may be used as a static auxiliary variable by
 * using it instead of the corresponding vector class in the auxiliary
 * store class.  Packing parameters may then be set by calling @c setOption
 * from the constructor for the aux store object.
 *
 * For dynamic auxiliary variables, the stored vector is automatically
 * converted to a @c PackedContainer when @c setOption is called for the
 * variable.
 */
template <class T>
class PackedContainer
  : public std::vector<T>, public IAuxSetOption
{
public:
  /// We act like this type.
  typedef std::vector<T> vector_type;


  /**
   * @brief Constructor.
   *
   * The packing parameters will be initialized to defaults appropriate
   * for type @c T.  (See @c PackedParameters.)
   */
  PackedContainer();


  /**
   * @brief Set a packing option.
   * @param option The option to set.
   *
   * Returns true on success, false otherwise.
   *
   * See @c PackedParameters::setOptions for details.
   */
  virtual bool setOption (const AuxDataOption& option) ATH_OVERRIDE;


  /**
   * @brief Set a packing option.
   * @param name The option name.
   * @param val The option value.
   *
   * Returns true on success, false otherwise.
   *
   * See @c PackedParameters::setOptions for details.
   */
  bool setOption (const std::string& name, int val);


  /**
   * @brief Set a packing option.
   * @param name The option name.
   * @param val The option value.
   *
   * Returns true on success, false otherwise.
   *
   * See @c PackedParameters::setOptions for details.
   */
  bool setOption (const std::string& name, float val);


  /**
   * @brief Set a packing option.
   * @param name The option name.
   * @param val The option value.
   *
   * Returns true on success, false otherwise.
   *
   * See @c PackedParameters::setOptions for details.
   */
  bool setOption (const std::string& name, double val);


  /**
   * @brief Return the packing parameters for this container.
   */
  const PackedParameters& parms() const;


  /**
   * @brief Set the packing parameters.
   * @param parms The new packing parameters.
   */
  void setParms (const PackedParameters& parms);


private:
  /// The packing parameters.
  PackedParameters m_parms;
};


} // namespace SG


#include "AthContainers/PackedContainer.icc"


#endif // not ATHCONTAINERS_PACKEDCONTAINER_H
