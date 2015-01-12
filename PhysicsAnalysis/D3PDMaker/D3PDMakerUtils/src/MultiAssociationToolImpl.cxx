/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/MultiAssociationToolImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c MultiAssociationTool.
 */


#include "D3PDMakerUtils/MultiAssociationToolImpl.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MultiAssociationToolImpl::MultiAssociationToolImpl
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
  : base_class (type, name, parent),
    AddVariable (m_prefix, m_blockName)
{
  declareProperty ("Prefix", m_prefix = "",
                   "Tuple variable prefix for this block.");
  declareProperty ("BlockName", m_blockName = this->name(),
                   "The name of this block.");
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c resetUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * (to @c getUntyped)
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode
MultiAssociationToolImpl::configureD3PD (IAddVariable* tree,
                                         const std::type_info& ti)
{
  CHECK( AddVariable::configureD3PD (tree) );
  CHECK( m_inputConverter.init (ti, fromTypeinfo()) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 *        Alternate implementation for the case where we have to choose
 *        among multiple possible input types.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c getUntyped.
 * @param tis List of possible input types that we can accept.
 * @param[out] which Index of the accepted type.
 *
 * @c configureMulti should check that the type of the object coming as input
 * (to @c getUntyped)
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode
MultiAssociationToolImpl::configureMulti (D3PD::IAddVariable* tree,
                                          const std::type_info& ti,
                                          const std::vector<const std::type_info*>& tis,
                                          size_t& which)
{
  CHECK( AddVariable::configureD3PD (tree) );
  CHECK( m_inputConverter.init (ti, tis, which) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 *
 * This is called at the start of the first event.
 * The default implementation is a no-op.
 */
StatusCode
MultiAssociationToolImpl::book()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Helper to convert pointers to source objects.
 * @param p Source object, as a pointer to the type passed to @c configureD3PD.
 * return Source object, as a pointer to the type given by
 * @c fromTypeinfo.
 */
const void* MultiAssociationToolImpl::doConversion (const void* p)
{
  if (!p) return 0;
  const void* pp = m_inputConverter.convertUntyped (p);
  if (!pp) {
    REPORT_MESSAGE (MSG::WARNING)
      << "Pointer conversion from " << m_inputConverter.srcName() << " to "
      << m_inputConverter.dstName() << "failed.";
    return 0;
  }
  return pp;
}


} // namespace D3PD
