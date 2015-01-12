// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/MultiAssociationToolImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Non-template parts of @c MultiAssociationTool.
 */


#ifndef D3PDMAKERUTILS_MULTIASSOCIATIONTOOLIMPL_H
#define D3PDMAKERUTILS_MULTIASSOCIATIONTOOLIMPL_H


#include "D3PDMakerUtils/AddVariable.h"
#include "D3PDMakerInterfaces/IMultiAssociationTool.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace D3PD {


/**
 * @brief Non-template parts of @c MultiAssociationTool.
 *
 * The class @c MultiAssociationTool provides a type-safe wrapper for the
 * @c IMultiAssociationTool interface; it is templated on the types of
 * the source and target of the association.
 * This class serves as a common base
 * class for the @c MultiAssociationTool instantiations, and factors
 * out all the code that doesn't depend on the template argument.
 *
 * Derived classes should define @c fromTypeinfo (this is normally
 * done by @c MultiAssociationTool).  The @book method may also
 * be overridden if the tool should itself fill any tuple variables.
 */
class MultiAssociationToolImpl
  : public extends1<AthAlgTool, IMultiAssociationTool>,
    public AddVariable
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MultiAssociationToolImpl (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c resetUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * (to @c getUntyped)
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti);


  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   * The default implementation is a no-op.
   */
  virtual StatusCode book();


  /**
   * @brief Return the @c std::type_info for the source of the association.
   */
  virtual const std::type_info& fromTypeinfo() const = 0;



protected:
  /**
   * @brief Helper to convert pointers to source objects.
   * @param p Source object, as a pointer to the type passed to @c configureD3PD.
   * return Source object, as a pointer to the type given by
   * @c fromTypeinfo.
   */
  const void* doConversion (const void* p);


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
  StatusCode configureMulti (D3PD::IAddVariable* tree,
                             const std::type_info& ti,
                             const std::vector<const std::type_info*>& tis,
                             size_t& which);


private:
  /// Parameter: source object prefix.
  std::string m_prefix;

  /// Property: the name of this block.
  std::string m_blockName;

  /// Helper to convert source object pointers.
  TypeConverter m_inputConverter;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_MULTIASSOCIATIONTOOLIMPL_H
