/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainedMultiAssociationFillerTool.cxx 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/ContainedMultiAssociationFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Represent a multiple association by containment.
 */


#include "ContainedMultiAssociationFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ContainedMultiAssociationFillerTool::ContainedMultiAssociationFillerTool
   (const std::string& type,
    const std::string& name,
    const IInterface* parent)
     : base_class (type, name, parent),
       AddVariable (m_prefix, m_blockName),
       m_associator (this),
       m_blockFillers (this),
       m_next (0)
{
  declareProperty ("Associator",   m_associator,
                   "The IMultiAssociationTool instance.");
  declareProperty ("BlockFillers", m_blockFillers,
                   "List of contained block filler tools.");
  declareProperty ("BlockName", m_blockName = this->name(),
                   "The name of this block.");
  declareProperty ("Prefix",       m_prefix = "",
                   "Variable name prefix for the contained blocks.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
ContainedMultiAssociationFillerTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_associator.retrieve() );
  CHECK( m_blockFillers.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode
ContainedMultiAssociationFillerTool::finalize()
{
  CHECK( AlgTool::finalize() );
  if (m_next) {
    m_associator->releaseElementUntyped (m_next);
    m_next = 0;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c fillUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode
ContainedMultiAssociationFillerTool::configureD3PD (IAddVariable* tree,
                                                    const std::type_info& ti)
{
  CHECK( AddVariable::configureD3PD (tree) );
  CHECK( m_associator->configureD3PD (this, ti) );
  const std::type_info& ati = m_associator->elementTypeinfo();
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->configureD3PD (this, ati ) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode ContainedMultiAssociationFillerTool::book()
{
  CHECK( m_associator->book () );
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->book ( ) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block.
 * @param p The input object.
 * @param again Set if this is a subsequent call requested by an AGAIN return
 *
 * This is called once per object.  The type of the object at which @c p
 * points is given by the @c ti argument to @c configureD3PD.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 *
 * If the return status is the special code @c AGAIN (defined above),
 * then this filler tool wants to make multiple entries.
 * The parent should set up to capture a new `row' and run
 * through the list of block filler
 * tools again, but for this tool call @c fillAgainUntyped
 * instead of @c fillUntyped.  This should be repeated as long
 * as @c fillAgainUntyped returns @c AGAIN.
 *
 * Once @c fillUntyped returns @c AGAIN, the parent should
 * call @c fillUntyped with the same @a p argument and @c again
 * set to @c true.  This continues until @c fillUntyped returns something
 * other than @c AGAIN.
 *
 * Not all parents may support this.  In that case, returning
 * @c AGAIN will be treated as an error.
 *
 * The return code may also be EMPTY if this tool did not fill in anything.
 * If all tools return empty, the parent may omit the row.
 */
StatusCode
ContainedMultiAssociationFillerTool::fillUntyped (const void* p,
                                                  bool again /*= false*/)
{
  // ??? Doesn't handle nested AGAIN.

  if (!again) {
    if (m_next) {
      m_associator->releaseElementUntyped (m_next);
      m_next = 0;
    }

    // Start the association.
    CHECK( m_associator->resetUntyped (p) );

    // Get the first result.
    m_next = m_associator->nextUntyped();
  }

  if (!m_next)
    return EMPTY;

  for (size_t i = 0; i < m_blockFillers.size(); i++) {
    StatusCode sc = m_blockFillers[i]->fillUntyped (m_next);
    CHECK( IBlockFillerTool::againok (sc) );
    if (!sc.isSuccess()) {
      REPORT_MESSAGE (MSG::WARNING)
        << "Got AGAIN/EMPTY: "
        << "Nested ContainedMultiAssociationFillerTool not supported.";
    }
  }

  m_associator->releaseElementUntyped (m_next);
  m_next = m_associator->nextUntyped();

  // Always return AGAIN for the first object, even if it's the last one.
  // We'll return EMPTY on the next call.
  // If we don't do this, then we can have problems if multiple
  // ContainedMultiAssociationFillerTools are used; any that return
  // a list of size 1 will have that object duplicated in all rows.
  if (m_next || !again)
    return AGAIN;

  return StatusCode::SUCCESS;
}



} // namespace D3PD
