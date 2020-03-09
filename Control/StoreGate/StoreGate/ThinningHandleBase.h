// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/ThinningHandleBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Handle for requesting thinning: factor out type-independent code.
 */


#ifndef STOREGATE_THINNINGHANDLEBASE_H
#define STOREGATE_THINNINGHANDLEBASE_H


#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/WriteHandle.h"
#include "AthContainers/ThinningDecision.h"


namespace SG {


/**
 * @brief Handle for requesting thinning: factor out type-independent code.
 * 
 * Base class for ThinningHandle, which factors out the code that doesn't
 * depend on the type of object being thinned.
 */
class ThinningHandleBase
{
public:
  using Op = ThinningDecision::Op;


  /** 
   * @brief Constructor.
   * @param dkey Handle key to use to write the ThinningDecision.
   * @param sgkey StoreGate key of the object being thinned.
   * @param ctx Event context.
   */
  explicit ThinningHandleBase (const WriteHandleKey<ThinningDecision>& dkey,
                               const std::string& sgkey,
                               const EventContext& ctx);

  /**
   * @brief Destructor.
   *
   * Records the thinning decision object in StoreGate.
   */
  ~ThinningHandleBase();


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
   * @brief Set the thinning state for the container from a bitmask.
   * @param v Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be thinned if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Thin if @c flag is true (overwriting anything originally set for this element).
   *   And --- Thin if @c flag is true and element was originally thinned, else not.
   *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
   */
  void thin (const std::vector<bool>& v, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param v Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be kept if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Keep if @c flag is true (overwriting anything originally set for this element).
   *   And --- Keep if @c flag is true and element was originally kept, else not.
   *   Or  --- Keep if @c flag is true or element was originally kept, else not.
   */
  void keep (const std::vector<bool>& v, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param other Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be thinned if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Thin if @c flag is true (overwriting anything originally set for this element).
   *   And --- Thin if @c flag is true and element was originally thinned, else not.
   *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
   */
  void thin (const ThinningDecisionBase& other, Op op = Op::Set);


  /**
   * @brief Set the thinning state for the container from a bitmask.
   * @param other Thinning state mask; should have the same size as the container.
   *          Element @c ndx should be kept if bit @c ndx is set in the map.
   * @param op Logical operation for combining with existing thinning state.
   *   Set --- Keep if @c flag is true (overwriting anything originally set for this element).
   *   And --- Keep if @c flag is true and element was originally kept, else not.
   *   Or  --- Keep if @c flag is true or element was originally kept, else not.
   */
  void keep (const ThinningDecisionBase& other, Op op = Op::Set);


  /**
   * @brief Mark that all elements should be thinned away.
   */
  void thinAll();


  /**
   * @brief Mark that all elements should be kept (not thinned).
   */
  void keepAll();


  /**
   * @brief Return the thinning object we're building.
   */
  const ThinningDecision& decision() const;

  
private:
  /// Handle for writing the decision object.
  WriteHandle<ThinningDecision> m_decisionHandle;

  /// The thinning object we're building.
  std::unique_ptr<ThinningDecision> m_decision;
};


} // namespace SG


#endif // not STOREGATE_THINNINGHANDLEBASE_H
