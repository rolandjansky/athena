/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef BYTESTREAMDATA_ROBDATA_H
#define BYTESTREAMDATA_ROBDATA_H

/**
 * @file ROBData.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch>André Rabello dos
 * ANJOS</a>
 * $Author: gemmeren $
 * $Revision: 1.18 $
 * $Date: 2009-03-31 22:21:05 $
 *
 * @brief Defines the ROB data entity. The ROB data is an abstract
 * entity that is used to discase the raw event format from the HLT
 * software.

 *  -------------------
 *  Modified:
 *     Copied this class from l2up package in DC release
 *       Make it a template class for TStorage
 */

#include "eformat/SourceIdentifier.h"
#include "ByteStreamData/RawEvent.h"

  /**
     @class ROBData_T
     @brief templete data class to wrap ROB fragment for accessing ROD data

   * This entity represents an abstraction of a ROB to the High-Level
   * Trigger Algorithms layer.
   * FOr L2UP implementation, the ROBData contains, internally, a
   * "smart pointer" scheme that guarantees the underlying (buffer) data
   * persitency until you don't need the ROBData anymore.
   * In the offline/EF scheme, the Fragments are owned by the Event.
   * no need for smart_ptr.
   */


template <typename ROBFragment, typename PointerType>
class ROBData_T {

public:
  /// forward
  class iterator;


  /// Builds a "high-level" ROB Data object
  ROBData_T(const ROBFragment* rob);

  /// Copy Constructor
  ROBData_T(const ROBData_T& other);

  /// Default Constructor
  ROBData_T();

  /// Instantiates an iterator over 32-bit integers to the first element
  iterator begin(void) const;

  /// Instantiates an iterator over 32-bit integers to the last+1 element
  iterator end(void) const;

  /// Assignment Operator
  ROBData_T& operator=(const ROBData_T& other);

  /// Return the current subdetector type.
  eformat::SubDetector sub_detector() const;

  /// Return the current module identifier.
  uint16_t module_identifier() const;

  /// Return the number of 32-bit words inside the ROD.
  uint32_t no_of_elements() const;

  /// Return the ROBFragment
  const ROBFragment* getROBFragment() const;

public:
  /**
   * @class iterator
   * @brief This class defines how to iterate over the ROBData_T, in different formats.
   */
  class iterator {
  private:
    /**
     * The sole constructor was made private. Only a call to
     * ROBData_T::begin() can instantiate an iterator.
     *
     * @param rod_it The underlying RODFragment iterator to use.
     */
    iterator(const PointerType& it);

  public:
    /// Copy Constructor
    iterator(const iterator& other);

    /// Assignment Operator
    iterator& operator=(const iterator& other);

    /// Dereferencing
    uint32_t operator*(void);

    /**
     * winding.
     *
     * This method will advance the iterator <code>pos</code> 32-bit
     * words.
     *
     * @param pos How many positions to advance.
     */
    iterator& operator+=(const unsigned int& pos);

    /// Increment
    iterator& operator++(void);

    /// Is not equal.
    bool operator!=(const iterator& other) const;

  private: //Representation
    friend class ROBData_T<ROBFragment, PointerType>;
    PointerType m_it; ///< iterator
  };

private: //Representation
  const ROBFragment* m_rob; ///< daddy
  PointerType m_data; ///< data
};

template <class ROBFragment, typename PointerType>
ROBData_T<ROBFragment, PointerType>::ROBData_T(const ROBFragment* rob) : m_rob(rob), m_data(0) {
  m_rob->rod_data(m_data);
}

template <class ROBFragment, typename PointerType>
ROBData_T<ROBFragment, PointerType>::ROBData_T() : m_rob(0), m_data(0) {
}

template <class ROBFragment, typename PointerType>
ROBData_T<ROBFragment, PointerType>::ROBData_T(const ROBData_T<ROBFragment, PointerType>& other) :
	m_rob(other.m_rob), m_data(other.m_data) {
}

template <class ROBFragment, typename PointerType>
ROBData_T<ROBFragment, PointerType>& ROBData_T<ROBFragment, PointerType>::operator=(const ROBData_T<ROBFragment, PointerType>& other) {
  m_rob = other.m_rob;
  m_data = other.m_data;
  return *this;
}

template <class ROBFragment, typename PointerType>
typename ROBData_T<ROBFragment, PointerType>::iterator ROBData_T<ROBFragment, PointerType>::begin(void) const {
  return iterator(m_data);
}

template <class ROBFragment, typename PointerType>
typename ROBData_T<ROBFragment, PointerType>::iterator ROBData_T<ROBFragment, PointerType>::end(void) const {
  return iterator(m_data + m_rob->rod_ndata());
}

template <class ROBFragment, typename PointerType>
eformat::SubDetector ROBData_T<ROBFragment, PointerType>::sub_detector() const {
  eformat::helper::SourceIdentifier sid(m_rob->rod_source_id());
  return sid.subdetector_id();
}

template <class ROBFragment, typename PointerType>
uint16_t ROBData_T<ROBFragment, PointerType>::module_identifier() const {
  // m_data->header()->source_id();  //FIXME: WHY AM I NEEDED?
  eformat::helper::SourceIdentifier sid(m_rob->rod_source_id());
  return sid.module_id();
}

template <class ROBFragment, typename PointerType>
uint32_t ROBData_T<ROBFragment, PointerType>::no_of_elements() const {
  return m_rob->rod_ndata();
}

template <class ROBFragment, typename PointerType>
const ROBFragment* ROBData_T<ROBFragment, PointerType>::getROBFragment() const {
  return m_rob;
}

template <class ROBFragment, typename PointerType>
ROBData_T<ROBFragment, PointerType>::iterator::iterator(const PointerType& it) : m_it(it) {
}

template <class ROBFragment, typename PointerType>
ROBData_T<ROBFragment, PointerType>::iterator::iterator(const ROBData_T<ROBFragment, PointerType>::iterator& other) :
	m_it(other.m_it) {
}

template <class ROBFragment, typename PointerType>
typename ROBData_T<ROBFragment, PointerType>::iterator& ROBData_T<ROBFragment, PointerType>::iterator::operator=(const  typename ROBData_T<ROBFragment, PointerType>::iterator& other) {
  m_it = other.m_it;
  return *this;
}

template <class ROBFragment, typename PointerType>
uint32_t ROBData_T<ROBFragment, PointerType>::iterator::operator*(void) {
  return *m_it;
}

template <class ROBFragment, typename PointerType>
typename ROBData_T<ROBFragment, PointerType>::iterator& ROBData_T<ROBFragment, PointerType>::iterator::operator+=(const unsigned int& pos) {
  m_it += pos;
  return *this;
}

template <class ROBFragment, typename PointerType>
typename ROBData_T<ROBFragment, PointerType>::iterator& ROBData_T<ROBFragment, PointerType>::iterator::operator++() {
  ++m_it;
  return *this;
}

template <class ROBFragment, typename PointerType>
bool  ROBData_T<ROBFragment, PointerType>::iterator::operator!=(const typename ROBData_T<ROBFragment, PointerType>::iterator& other) const {
  return m_it != other.m_it;
}

typedef ROBData_T<OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment, OFFLINE_FRAGMENTS_NAMESPACE::PointerType> ROBData;

#endif //BYTESTREAMDATA_ROBDATA_H

#endif
