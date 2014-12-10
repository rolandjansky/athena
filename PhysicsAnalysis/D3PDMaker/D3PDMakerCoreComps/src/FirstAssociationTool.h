// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/FirstAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Choose the first result from a multiple association.
 */


#ifndef D3PDMAKERCORECOMPS_FIRSTASSOCIATIONTOOL_H
#define D3PDMAKERCORECOMPS_FIRSTASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ISingleAssociationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class IMultiAssociationTool;


/**
 * @brief Choose the first result from a multiple association.
 *
 * This wrapper will turn a multiple association tool into a single
 * association tool by only using the first result from the association.
 */
class FirstAssociationTool
  : public extends1<AthAlgTool, ISingleAssociationTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  FirstAssociationTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /**
   * @brief Standard Gaudi @c initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c resetUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti) override;


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book() override;


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   * Should be of the type given by @c typeinfo.
   */
  virtual const void* getUntyped (const void* p) override;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c getUntyped().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObjectUntyped (const void* p) override;


private:
  /// The wrapped multiple association tool.
  ToolHandle<IMultiAssociationTool> m_associator;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_FIRSTASSOCIATIONTOOL_H
