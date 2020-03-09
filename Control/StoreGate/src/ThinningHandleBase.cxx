/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/src/ThinningHandleBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Handle for requesting thinning: factor out type-independent code.
 */


#include "StoreGate/ThinningHandleBase.h"
#include "StoreGate/exceptions.h"
#include "AthenaKernel/getMessageSvc.h"


namespace SG {


/** 
 * @brief Constructor.
 * @param dkey Handle key to use to write the ThinningDecision.
 * @param sgkey StoreGate key of the object being thinned.
 * @param ctx Event context.
 */
ThinningHandleBase::ThinningHandleBase
  (const WriteHandleKey<ThinningDecision>& dkey,
   const std::string& sgkey,
   const EventContext& ctx)
    : m_decisionHandle (dkey, ctx),
      m_decision (std::make_unique<ThinningDecision> (sgkey))
{
}


/**
 * @brief Destructor.
 *
 * Records the thinning decision object in StoreGate.
 */
ThinningHandleBase::~ThinningHandleBase()
{
  if (m_decisionHandle.record (std::move (m_decision)).isFailure()) {
    MsgStream msg (Athena::getMessageSvc(), "ThinningHandleBase");
    msg << MSG::ERROR
        << "Can't record SG::ThinningDecision object: "
        << m_decisionHandle.store() << "+" << m_decisionHandle.key()
        << endmsg;
  }
}


/**
 * @brief Mark that index @c ndx in the container should be thinned away.
 * @param ndx Index of element to thin.
 */
void ThinningHandleBase::thin (size_t ndx)
{
  m_decision->thin (ndx);
}


/**
 * @brief Mark that index @c ndx in the container should be kept
 *        (not thinned away).
 * @param ndx Index of element to keep.
 */
void ThinningHandleBase::keep (size_t ndx)
{
  m_decision->keep (ndx);
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param v Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be thinned if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Thin if @c flag is true (overwriting anything originally set for this element).
 *   And --- Thin if @c flag is true and element was originally thinned, else not.
 *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
 */
void ThinningHandleBase::thin (const std::vector<bool>& v, Op op /*= Op::Set*/)
{
  m_decision->thin (v, op);
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param v Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be kept if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Keep if @c flag is true (overwriting anything originally set for this element).
 *   And --- Keep if @c flag is true and element was originally kept, else not.
 *   Or  --- Keep if @c flag is true or element was originally kept, else not.
 */
void ThinningHandleBase::keep (const std::vector<bool>& v, Op op /*= Op::Set*/)
{
  m_decision->keep (v, op);
}

/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param other Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be thinned if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Thin if @c flag is true (overwriting anything originally set for this element).
 *   And --- Thin if @c flag is true and element was originally thinned, else not.
 *   Or  --- Thin if @c flag is true or element was originally thinned, else not.
 */
void ThinningHandleBase::thin (const ThinningDecisionBase& other, Op op /*= Op::Set*/)
{
  m_decision->thin (other, op);
}


/**
 * @brief Set the thinning state for the container from a bitmask.
 * @param other Thinning state mask; should have the same size as the container.
 *          Element @c ndx should be kept if bit @c ndx is set in the map.
 * @param op Logical operation for combining with existing thinning state.
 *   Set --- Keep if @c flag is true (overwriting anything originally set for this element).
 *   And --- Keep if @c flag is true and element was originally kept, else not.
 *   Or  --- Keep if @c flag is true or element was originally kept, else not.
 */
void ThinningHandleBase::keep (const ThinningDecisionBase& other, Op op /*= Op::Set*/)
{
  m_decision->keep (other, op);
}


/**
 * @brief Mark that all elements should be thinned away.
 */
void ThinningHandleBase::thinAll()
{
  return m_decision->thinAll();
}


/**
 * @brief Mark that all elements should be kept (not thinned).
 */
void ThinningHandleBase::keepAll()
{
  return m_decision->keepAll();
}


/**
 * @brief Return the thinning object we're building.
 */
const ThinningDecision& ThinningHandleBase::decision() const
{
  return *m_decision;
}

  
} // namespace SG
