// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file AthenaKernel/ThinningDecisionBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Hold thinning decisions for one container.
 */


#ifndef ATHENAKERNEL_THINNINGDECISIONBASE_H
#define ATHENAKERNEL_THINNINGDECISIONBASE_H


#include "AthenaKernel/ILockable.h"
#include "boost/dynamic_bitset.hpp"
#include <vector>
#include <string>


namespace SG {


/**
 * @brief Hold thinning decisions for one container.
 *
 * This class is a wrapper around a bitmap that holds a set of thinning
 * decisions for one container.  If bit @c i is set in the map, then
 * element @c i of the container has been thinned.
 *
 * This class also holds a mapping of container indices before and after
 * thinning.  However, this is not available until @ buildIndexMap()
 * has been called (which usually happens implicitly when the derived
 * @c ThinningDecision object is recorded in StoreGate).
 */
class ThinningDecisionBase
{
public:
  /// Flag used to show that an index has been thinned away.
  static const std::size_t RemovedIdx = static_cast<std::size_t>(-1);

  enum class Op {
    Set = 0,
    And = 1,
    Or = 2
  };


  /**
   * @brief Constructor.
   * @param sz Number of elements in the container.
   *
   * Initialized with all elements kept.
   */
  ThinningDecisionBase (size_t sz = 0);


  /**
   * @brief Change the number of elements.
   * @param size The new number of elements.
   */
  void resize (const size_t size);


  /**
   * @brief Return true if element @c ndx should be thinned.
   */
  bool thinned (size_t ndx) const;


  /**
   * @brief Return the total size of the container being thinned.
   */
  size_t size() const;


  /**
   * @brief Return the size of the container being thinned after thinning.
   */
  size_t thinnedSize() const;


  /**
   * @brief Mark that all elements should be thinned away.
   */
  void thinAll();


  /**
   * @brief Mark that all elements should be kept (not thinned).
   */
  void keepAll();


  /**
   * @brief Mark that index @c ndx in the container should be thinned away.
   * @param ndx Index of element to thin.
   */
  void thin (size_t ndx);


  /**
   * @brief Mark that index @c ndx in the container should be kept
   *        (not thinned away).
   * @param ndx Index of element to keep.
   */
  void keep (size_t ndx);


  /**
   * @brief Set thinning state for one element.
   * @param ndx Index of element to alter.
   * @param flag If true, thin this element; if false, keep it.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Thin if @c flag is true.
   *   And --- Thin if @c flag is true and element was originally thinned, else not.
   *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
   */
  void thin (size_t ndx, bool flag, Op op = Op::Set);


  /**
   * @brief Set thinning state for one element.
   * @param ndx Index of element to alter.
   * @param flag If true, keep this element; if false, thin it it.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Keep if @c flag is true.
   *   And --- Keep if @c flag is true and element was originally kept, else not.
   *   Or  --- Keep if @c flag is true or element was originally kept, else not.
   */
  void keep (size_t ndx, bool flag, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param v Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be thinned if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Thin if @c flag is true.
   *   And --- Thin if @c flag is true and element was originally thinned, else not.
   *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
   */
  void thin (const std::vector<bool>& v, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param v Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be kept if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Keep if @c flag is true.
   *   And --- Keep if @c flag is true and element was originally kept, else not.
   *   Or  --- Keep if @c flag is true or element was originally kept, else not.
   */
  void keep (const std::vector<bool>& v, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param other Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be thinned if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Thin if @c flag is true.
   *   And --- Thin if @c flag is true and element was originally thinned, else not.
   *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
   */
  void thin (const ThinningDecisionBase& other, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param other Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be kept if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Keep if @c flag is true.
   *   And --- Keep if @c flag is true and element was originally kept, else not.
   *   Or  --- Keep if @c flag is true or element was originally kept, else not.
   */
  void keep (const ThinningDecisionBase& other, Op op = Op::Set);


  /**
   * @brief Build the index map
   */
  void buildIndexMap();


  /**
   * @brief Return the index corresponding to @c ndxOrig after thinning.
   * @param ndxOrig Original container index.
   *
   * Returns the index at which element @c ndxOrig ends up after thinning.
   * If the element was thinned away, returns @c RemovedIdx.
   *
   * This information is available only after @c buildIndexMap has been called.
   */
  size_t index (size_t ndxOrig) const;


private:
  /// Thinning map.  Set to 1 for thinned elements.
  boost::dynamic_bitset<> m_mask;

  /// Mapping from original indices to thinned indices.
  std::vector<size_t> m_indexMap;
};


} // namespace SG


#include "AthenaKernel/ThinningDecisionBase.icc"


#endif // not ATHENAKERNEL_THINNINGDECISIONBASE_H
