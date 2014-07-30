// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/DRAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate to particle closest in DR.
 */


#ifndef EVENTCOMMOND3PDMAKER_DRASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_DRASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "xAODBase/IParticle.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Associate to particle closest in DR.
 *
 * This is a single association tool.
 * Given an @c INavigable4Momentum object and a container of such objects,
 * return the object in the container closest in DR to the input object.
 * Optionally, a DR cut may be set; the association fails if there
 * are no objects within that cut.
 *
 * Parameters:
 *  Getter - ICollectionGetterTool instance defining the target collection.
 *  DRCut  - DR cut for the association.  (default: 999)
 *
 * This tool will create a tuple variable `dr' giving the distance
 * in DR to the associated object.
 */
class DRAssociationTool
  : public SingleAssociationTool<Types<INavigable4Momentum, xAOD::IParticle> >
{
public:
  typedef SingleAssociationTool<Types<INavigable4Momentum, xAOD::IParticle> > Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  DRAssociationTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c fillUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti) override;


  /**
   * @brief Create any needed tuple variables.
   */
  virtual StatusCode book() override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const INavigable4Momentum* get (const INavigable4Momentum& p) override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::IParticle* get (const xAOD::IParticle& p) override;


private:
  /// Property: The getter for the target collection.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: DR cut.
  float m_drcut;

  /// Variable: DR to associated object.
  float* m_dr;
};


} // namespace D3PD



#endif // not EVENTCOMMOND3PDMAKER_DRASSOCIATIONTOOL_H
