// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/IdentityAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief A generic identity association; i.e., one that simply returns
 *        its input.
 */


#ifndef D3PDMAKERCORECOMPS_IDENTITYASSOCIATIONTOOL_H
#define D3PDMAKERCORECOMPS_IDENTITYASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationToolImpl.h"
#include <typeinfo>


namespace D3PD {


/**
 * @brief A generic identity association; i.e., one that simply returns
 *        its input.
 */
class IdentityAssociationTool
  : public SingleAssociationToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  IdentityAssociationTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c getUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * (to @c getUntyped)
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (D3PD::IAddVariable* tree,
                                    const std::type_info& ti);


  /**
   * @brief Return the @c std::type_info for the source of the association.
   */
  virtual const std::type_info& fromTypeinfo() const;

  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;
  

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   * Should be of the type given by @c typeinfo.
   */
  virtual const void* getUntyped (const void* p);


private:
  const std::type_info* m_ti;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_IDENTITYASSOCIATIONTOOL_H
