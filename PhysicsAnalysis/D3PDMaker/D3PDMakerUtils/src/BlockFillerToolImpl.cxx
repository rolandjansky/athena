/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/BlockFillerToolImpl.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c BlockFillerTool.
 */


#include "D3PDMakerUtils/BlockFillerToolImpl.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
BlockFillerToolImpl::BlockFillerToolImpl (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : base_class (type, name, parent),
    AddVariable (m_prefix, m_blockName, m_suffix)
{
  declareProperty ("Prefix", m_prefix = "",
                   "Tuple variable prefix for this block.");
  declareProperty ("Suffix", m_suffix = "",
                   "Tuple variable suffix for this block.");
  declareProperty ("BlockName", m_blockName = this->name(),
                   "The name of this block.");
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c fillUntyped.
 * @param fill_ti is the type of object that the user code will expect.
 *
 * This is the common implementation for the @c configureD3PD method
 * of @c IBlockFillerTool.
 * It checks that the type of the object coming as input
 * is compatible with what the user code is  expecting,
 * and raises an error otherwise.
 */
StatusCode
BlockFillerToolImpl::configureImpl (IAddVariable* tree,
                                    const std::type_info& ti,
                                    const std::type_info& fill_ti)
{
  CHECK( AddVariable::configureD3PD (tree) );
  CHECK( m_converter.init (ti, fill_ti) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c fillUntyped.
 * @param tis List of possible input types that we can accept.
 * @param[out] which Index of the accepted type.
 *
 * This is the common implementation for the @c configureD3PD method
 * of @c IBlockFillerTool.
 * It checks that the type of the object coming as input
 * is compatible with what the user code is expecting,
 * and raises an error otherwise.
 *
 * This version allows for one of a set of types to match.
 * If successful, WHICH is set to the index (0-based) of the matching type.
 */
StatusCode
BlockFillerToolImpl::configureImpl (IAddVariable* tree,
                                    const std::type_info& ti,
                                    const std::vector<const std::type_info*>& tis,
                                    size_t& which)
{
  CHECK( AddVariable::configureD3PD (tree) );
  CHECK( m_converter.init (ti, tis, which) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Do type conversion.
 * @param p[in,out] Pointer to convert.
 *
 * On entry, @a p is a pointer to an object of the type
 * that was passed into @c configureImpl as @c ti.  Convert
 * the pointer to point to an object of the type that was
 * given as @c fill_ti.  If the conversion fails, the pointer
 * is set to 0 (and @c FAILURE is returned).
 */
StatusCode
BlockFillerToolImpl::convert (void const* & p) const
{
  p = m_converter.convertUntyped (p);
  if (!p) {
    REPORT_MESSAGE (MSG::ERROR)
      << "Pointer conversion from " << m_converter.srcName() << " to "
      << m_converter.dstName() << "failed.";
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}



} // namespace D3PD
