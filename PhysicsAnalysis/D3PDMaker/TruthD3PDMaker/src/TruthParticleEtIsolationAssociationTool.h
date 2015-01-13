// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/TruthParticleEtIsolationAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Associate from a TruthParticle to the set of isolation cones.
 */


#ifndef TRUTHD3PDMAKER_TRUTHPARTICLEETISOLATIONASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLEETISOLATIONASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "McParticleEvent/TruthParticleParamDefs.h"
#include "McParticleEvent/TruthParticle.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Associate from a TruthParticle to the corresponding GenParticle.
 */
class TruthParticleEtIsolationAssociationTool
  : public SingleAssociationTool<TruthParticle, McAod::EtIsolations>
{
public:
  typedef SingleAssociationTool<TruthParticle, McAod::EtIsolations> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleEtIsolationAssociationTool (const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const McAod::EtIsolations* get (const TruthParticle& p);


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c fillUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti);


private:
  // Property: The getter used to find the isolations container.
  ToolHandle<ICollectionGetterTool> m_getter;
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHPARTICLEETISOLATIONASSOCIATIONTOOL_H
