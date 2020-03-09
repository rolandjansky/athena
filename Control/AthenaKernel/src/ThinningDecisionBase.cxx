/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file AthenaKernel/src/ThinningDecisionBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Hold thinning decisions for one container.
 */


#include "AthenaKernel/ThinningDecisionBase.h"
#include <stdexcept>


namespace SG {


/**
 * @brief Constructor.
 * @param sz Number of elements in the container.
 */
ThinningDecisionBase::ThinningDecisionBase (size_t sz /*= 0*/)
  : m_mask (sz)
{
}


/**
 * @brief Change the number of elements.
 * @param size The new number of elements.
 */
void ThinningDecisionBase::resize (const size_t size)
{
  m_mask.resize (size);
}


/**
 * @brief Return the size of the container being thinned after thinning.
 */
size_t ThinningDecisionBase::thinnedSize() const
{
  return m_mask.size() - m_mask.count();
}


/**
 * @brief Mark that all elements should be thinned away.
 */
void ThinningDecisionBase::thinAll()
{
  m_mask.set();
}


/**
 * @brief Mark that all elements should be kept (not thinned).
 */
void ThinningDecisionBase::keepAll()
{
  m_mask.reset();
}


/**
 * @brief Mark that index @c ndx in the container should be thinned away.
 * @param ndx Index of element to thin.
 */
void ThinningDecisionBase::thin (size_t ndx)
{
  if (ndx >= m_mask.size()) {
    throw std::out_of_range ("ThinningDecisionBase::thin");
  }
  m_mask.set (ndx, true);
}


/**
 * @brief Mark that index @c ndx in the container should be kept
 *        (not thinned away).
 * @param ndx Index of element to keep.
 */
void ThinningDecisionBase::keep (size_t ndx)
{
  if (ndx >= m_mask.size()) {
    throw std::out_of_range ("ThinningDecisionBase::thin");
  }
  m_mask.set (ndx, false);
}


/**
 * @brief Set thinning state for one element.
 * @param ndx Index of element to alter.
 * @param flag If true, thin this element; if false, keep it.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Thin if @c flag is true.
 *   And --- Thin if @c flag is true and element was originally thinned, else not.
 *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
 */
void ThinningDecisionBase::thin (size_t ndx, bool flag, Op op /*= Op::Set*/)
{
  if (ndx >= m_mask.size()) {
    throw std::out_of_range ("ThinningDecisionBase::thin");
  }
  if (op == Op::And)
    flag &= m_mask[ndx];
  else if (op == Op::Or)
    flag |= m_mask[ndx];
  m_mask.set (ndx, flag);
}


/**
 * @brief Set thinning state for one element.
 * @param ndx Index of element to alter.
 * @param flag If true, keep this element; if false, thin it it.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Keep if @c flag is true.
 *   And --- Keep if @c flag is true and element was originally kept, else not.
 *   Or  --- Keep if @c flag is true or element was originally kept, else not.
 */
void ThinningDecisionBase::keep (size_t ndx, bool flag, Op op /*= Op::Set*/)
{
  if (ndx >= m_mask.size()) {
    throw std::out_of_range ("ThinningDecisionBase::thin");
  }
  if (op == Op::And)
    flag &= !m_mask[ndx];
  else if (op == Op::Or)
    flag |= !m_mask[ndx];
  m_mask.set (ndx, !flag);
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param v Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be thinned if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Thin if @c flag is true.
 *   And --- Thin if @c flag is true and element was originally thinned, else not.
 *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
 */
void ThinningDecisionBase::thin (const std::vector<bool>& v, Op op /*= Op::Set*/)
{
  size_t sz = m_mask.size();
  if (sz != v.size()) {
    throw std::out_of_range ("ThinningDecisionBase::thin(): inconsistent vector sizes.");
  }
  for (size_t i = 0; i < sz; i++) {
    bool flag = v[i];
    if (op == Op::And)
      flag &= m_mask[i];
    else if (op == Op::Or)
      flag |= m_mask[i];
    m_mask.set (i, flag);
  }
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param v Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be kept if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Keep if @c flag is true.
 *   And --- Keep if @c flag is true and element was originally kept, else not.
 *   Or  --- Keep if @c flag is true or element was originally kept, else not.
 */
void ThinningDecisionBase::keep (const std::vector<bool>& v, Op op /*= Op::Set*/)
{
  size_t sz = m_mask.size();
  if (sz != v.size()) {
    throw std::out_of_range ("ThinningDecisionBase::keep(): inconsistent vector sizes.");
  }
  for (size_t i = 0; i < sz; i++) {
    bool flag = v[i];
    if (op == Op::And)
      flag &= !m_mask[i];
    else if (op == Op::Or)
      flag |= !m_mask[i];
    m_mask.set (i, !flag);
  }
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param other Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be thinned if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Thin if @c flag is true.
 *   And --- Thin if @c flag is true and element was originally thinned, else not.
 *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
 */
void ThinningDecisionBase::thin (const ThinningDecisionBase& other, Op op /*= Op::Set*/)
{
  size_t sz = m_mask.size();
  if (sz != other.size()) {
    throw std::out_of_range ("ThinningDecisionBase::thin(): inconsistent vector sizes.");
  }
  if (op == Op::And)
    m_mask &= other.m_mask;
  else if (op == Op::Or)
    m_mask |= other.m_mask;
  else
    m_mask = other.m_mask;
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param other Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be kept if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Keep if @c flag is true.
 *   And --- Keep if @c flag is true and element was originally kept, else not.
 *   Or  --- Keep if @c flag is true or element was originally kept, else not.
 */
void ThinningDecisionBase::keep (const ThinningDecisionBase& other, Op op /*= Op::Set*/)
{
  size_t sz = m_mask.size();
  if (sz != other.size()) {
    throw std::out_of_range ("ThinningDecisionBase::keep(): inconsistent vector sizes.");
  }
  if (op == Op::And)
    m_mask |= ~other.m_mask;
  else if (op == Op::Or)
    m_mask &= ~other.m_mask;
  else
    m_mask = ~other.m_mask;
}


/**
 * @brief Build the index map
 */
void ThinningDecisionBase::buildIndexMap()
{
  assert (m_indexMap.empty());
  size_t sz = m_mask.size();
  m_indexMap.resize (sz);
  size_t pos = 0;
  for (size_t i = 0; i < sz; i++) {
    m_indexMap[i] = thinned(i) ? RemovedIdx : pos++ ;
  }
}


} // namespace SG
