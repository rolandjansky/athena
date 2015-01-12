// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/BlockFillerToolImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c BlockFillerTool.
 */

#ifndef D3PDMAKERUTILS_BLOCKFILLERTOOLIMPL_H
#define D3PDMAKERUTILS_BLOCKFILLERTOOLIMPL_H


#include "D3PDMakerInterfaces/IBlockFillerTool.h"
#include "D3PDMakerUtils/AddVariable.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/AlgTool.h"
#include <string>


namespace D3PD {


/**
 * @brief Non-template parts of @c BlockFillerTool.
 *
 * The class @c BlockFillerTool provides a type-safe wrapper for the
 * @c IBlockFillerTool interface; it is templated on the type of
 * object on which it operates.  This class serves as a common base
 * class for the @c BlockFillerTool instantiations, and factors
 * out all the code that doesn't depend on the template argument.
 *
 * We also derive from @c AddVariable, so we provide the @c IAddVariable
 * interface.
 */       
class BlockFillerToolImpl
  : public extends1<AthAlgTool, IBlockFillerTool>, public AddVariable
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  BlockFillerToolImpl (const std::string& type,
                       const std::string& name,
                       const IInterface* parent);


protected:
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
  StatusCode configureImpl (IAddVariable* tree,
                            const std::type_info& ti,
                            const std::type_info& fill_ti);


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
  StatusCode configureImpl (IAddVariable* tree,
                            const std::type_info& ti,
                            const std::vector<const std::type_info*>& tis,
                            size_t& which);


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
  StatusCode convert (void const* & p) const;


private:
  /// Property: the variable prefix for this block.
  std::string m_prefix;

  /// Property: the variable prefix for this block.
  std::string m_suffix;

  /// Property: the name of this block.
  std::string m_blockName;

  /// Helper to do type conversions.
  TypeConverter m_converter;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_BLOCKFILLERTOOLIMPL_H
