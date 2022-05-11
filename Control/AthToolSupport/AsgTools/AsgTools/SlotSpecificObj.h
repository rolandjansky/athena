/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file AsgTools/SlotSpecificObj.h
 * @author Frank Winklmeier
 * @date April, 2022
 * @brief Dummy implementation of AthenaKernel/SlotSpecificObj.h
 */

#ifndef ASGTOOLS_SLOTSPECIFICOBJ_H
#define ASGTOOLS_SLOTSPECIFICOBJ_H

#ifndef XAOD_STANDALONE
#   include "AthenaKernel/SlotSpecificObj.h"
#else

#include <array>
#include <string>

class EventContext; /// dummy forward declaration (not actually needed)

namespace SG {

/**
 * @brief Asg version of SG::SlotSpecificObj from AthenaKernel
 *
 * To make code using SG::SlotSpecificObj compatible with AnalysisBase
 * provide a dummy implementation that mimics slot-specific objects with
 * one unique slot.
 *
 * See the original AthenaKernel/SlotSpecificObj.h for complete documentation.
 */
template <class T>
class SlotSpecificObj
{
public:
  SlotSpecificObj() = default;
  SlotSpecificObj (size_t /*nslots*/) : SlotSpecificObj() {}

  ///@{ Accessors to contained object
  T* get (const EventContext&) { return get(); }
  T* get() { return &m_slots[0]; }
  const T* get (const EventContext&) const { return get(); }
  const T* get() const { return &m_slots[0]; }
  ///@}

  ///@{ Dereference operators
  T& operator* ()              { return *get(); }
  const T& operator* () const  { return *get(); }
  T* operator-> ()             { return get(); }
  const T* operator-> () const { return get(); }
  ///@}

  ///@{ Iterator support
  typedef typename std::array<T,1>::iterator iterator;
  typedef typename std::array<T,1>::const_iterator const_iterator;
  typedef typename std::array<T,1>::value_type value_type;

  iterator begin()             { return m_slots.begin(); }
  const_iterator begin() const { return m_slots.begin(); }
  iterator end()               { return m_slots.end(); }
  const_iterator end() const   { return m_slots.end(); }
  ///@}

private:
  /// One unique slot only
  std::array<T,1> m_slots{};
};

} // namespace SG

#endif // XAOD_STANDALONE
#endif // ASGTOOLS_SLOTSPECIFICOBJ_H
