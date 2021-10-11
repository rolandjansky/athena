// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainersInterfaces/IAuxElement.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Flag that a class may have auxiliary data associated with it.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXELEMENT_H
#define ATHCONTAINERSINTERFACES_IAUXELEMENT_H


#include <cstdlib>
#include <cstdint>
#include <cassert>


namespace SG {


class ConstAuxElement;
class AuxElement;


/**
 * @brief Flag that a class may have auxiliary data associated with it.
 *
 * This this class really exists only to flag that a class may
 * have auxiliary data associated with it.  @c SG::AuxElement derives
 * from this, but one should use this class for tests rather
 * than @c SG::AuxElement to avoid dependency issues.
 *
 * This is an `interface' in the sense that it identifies a group of classes,
 * but it defines no functionality itself.  In fact, this class should
 * not have a virtual table.
 *
 * It turns out, however, that ROOT's branch splitting in TBranchElement
 * can get confused in the case of an empty base class deriving from
 * another base class.  We can work around this by moving some of the
 * data members from AuxElement here (even though they are transient).
 */
class IAuxElement
{
public:
  /**
   * @brief Default constructor.
   *        For an element not in a container.
   */
  IAuxElement();


  /**
   * @brief Constructor.
   * @param index The index of this element in its container.
   */
  IAuxElement(size_t index);


  /**
   * @brief Return the index of this element within its container.
   */
  size_t index() const;


protected:
  /**
   * @brief True if this element has no private data.
   */
  bool noPrivateData() const;


  /**
   * @brief True if this element currently has private data.
   */
  bool havePrivateData() const;


  /**
   * @brief True if this element had private data before it was added
   *        to its current container.
   */
  bool hadPrivateData() const;


 
private:
  // These functions are for the use of SG::*AuxElement, but should not
  // be accessible to any further derived classes.  So make them private
  // with friendship rather than protected.
  friend class SG::ConstAuxElement;
  friend class SG::AuxElement;


  /**
   * @brief Set the index of this element within its container.
   */
  void setIndex (size_t index);


  /**
   * @brief Record that this element does not have private data.
   */
  void setNoPrivateData();


  /**
   * @brief Record that this element currently has private data.
   */
  void setHavePrivateData();


  /**
   * @brief Record that this element used to have private data.
   */
  void setHadPrivateData();


  /// The index of this element within its container.
  /// Should be 0 if this object is not within a container.
  size_t m_index;

  /// The current private data state.
  enum class PrivateStoreState : uint8_t
  {
    NO_PRIVATE = 0,
    HAVE_PRIVATE = 1,
    HAD_PRIVATE = 2,
  };
  PrivateStoreState m_privateStoreState;

  // We have 7 bytes of padding here.  It's tempting to pack the two
  // fields above into 64 bits, but that's observed to spoil opimizations.
};


} // namespace SG


#include "AthContainersInterfaces/IAuxElement.icc"


#endif // not ATHCONTAINERSINTERFACES_IAUXELEMENT_H
