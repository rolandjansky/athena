/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainedVectorMultiAssociationFillerTool.cxx 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/ContainedVectorMultiAssociationFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Represent a multiple association by containment inside vectors.
 */


#include "ContainedVectorMultiAssociationFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ContainedVectorMultiAssociationFillerTool::ContainedVectorMultiAssociationFillerTool
   (const std::string& type,
    const std::string& name,
    const IInterface* parent)
     : base_class (type, name, parent),
       m_associator (this)
{
  declareProperty ("Associator",   m_associator,
                   "The IMultiAssociationTool instance.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
ContainedVectorMultiAssociationFillerTool::initialize()
{
  CHECK( VectorFillerToolBase::initialize() );
  CHECK( m_associator.retrieve() );
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
ContainedVectorMultiAssociationFillerTool::configureD3PD (IAddVariable* tree,
                                                          const std::type_info& ti)
{
  CHECK( m_associator->configureD3PD (this, ti) );
  CHECK( VectorFillerToolBase::doConfigure (tree, *m_associator) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode ContainedVectorMultiAssociationFillerTool::book()
{
  CHECK( m_associator->book () );
  CHECK( VectorFillerToolBase::doBook() );

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
 */
StatusCode
ContainedVectorMultiAssociationFillerTool::fillUntyped (const void* p,
                                                        bool /*again = false*/)
{
  // Start the association.
  CHECK( m_associator->resetUntyped (p) );

  CHECK( doFill (*m_associator, 0) );

  return StatusCode::SUCCESS;
}



} // namespace D3PD
