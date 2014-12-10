/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IndexMultiAssociationFillerTool.cxx 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/IndexMultiAssociationFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Represent a multiple association by index.
 */


#include "IndexMultiAssociationFillerTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
IndexMultiAssociationFillerTool::IndexMultiAssociationFillerTool
   (const std::string& type,
    const std::string& name,
    const IInterface* parent)
     : base_class (type, name, parent),
       AddVariable (m_prefix, m_blockName),
       m_associator (this),
       m_index (0),
       m_map (this, m_targetLabel, m_registry, m_allowMissing)
{
  declareProperty ("Associator",    m_associator,
                   "The ISingleAssociationTool instance.");
  declareProperty ("Target",        m_targetLabel,
                   "Label of the collection getter defining the collection "
                   "within which to index.");
  declareProperty ("Prefix",        m_prefix,
                   "Variable name prefix for the index variable.");
  declareProperty ("BlockName", m_blockName = this->name(),
                   "The name of this block.");
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "The ICollectionGetterRegistryTool instance.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, then it is not considered an error for the "
                   "target getter to fail to find the input objects.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
IndexMultiAssociationFillerTool::initialize()
{
  REPORT_MESSAGE(MSG::WARNING)
    << "IndexMultiAssociationFillerTool is deprecated; use instead "
    << "ContainedVectorMultiAssociationFillerTool with IndexFillerTool. "
    << "IndexMultiAssociation.py should do that for you.";

  CHECK( AthAlgTool::initialize() );
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
IndexMultiAssociationFillerTool::configureD3PD (IAddVariable* tree,
                                                const std::type_info& ti)
{
  CHECK( AddVariable::configureD3PD (tree) );
  CHECK( m_associator->configureD3PD (this, ti) );
  CHECK( m_map.configureD3PD() );

  if (m_map.target()) {
    CHECK( m_converter.init (m_associator->elementTypeinfo(),
                             m_map.target()->elementTypeinfo()) );
  }
  else {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                 "D3PD::IndexAssociationFillerTool")
     << "Not booking index variable because association target does not exist.";
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode IndexMultiAssociationFillerTool::book()
{
  CHECK( m_associator->book () );
  if (m_map.target())
    CHECK( addVariable ("index", m_index) );
  else
    m_index = 0;

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
IndexMultiAssociationFillerTool::fillUntyped (const void* p,
                                              bool /*again = false*/)
{
  // Build the map if it doesn't exist.
  CHECK( m_map.reset() );

  // Iterate over the result of the association.
  CHECK( m_associator->resetUntyped (p) );
  while (const void* pout = m_associator->nextUntyped ())
  {
    if (m_index) {
      // Convert to the proper pointer type to search in the container.
      const void* pnew = m_converter.convertUntyped (pout);
      if (!pnew)
      {
        REPORT_MESSAGE (MSG::WARNING)
          << "Pointer conversion from " << m_converter.srcName() << " to "
          << m_converter.dstName() << "failed.";
        m_associator->releaseElementUntyped (pout);
        continue;
      }

      m_index->push_back (m_map.findIndex (pnew));
    }
    m_associator->releaseElementUntyped (pout);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
