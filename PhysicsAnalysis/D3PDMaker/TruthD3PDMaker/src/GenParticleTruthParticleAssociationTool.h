// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleTruthParticleAssociationTool.h 639304 2015-01-13 20:08:39Z ssnyder $
/**
 * @file EventCommonD3PDMaker/GenParticleTruthParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2010
 * @brief Associate from a GenParticle to a corresponding TruthParticle.
 */


#ifndef EVENTCOMMOND3PDMAKER_GENPARTICLETRUTHPARTICLEASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_GENPARTICLETRUTHPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "AtlasHepMC/GenParticle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>
class TruthParticle;
class TruthParticleContainer;
class Incident;
class IIncidentSvc;


namespace D3PD {


class IObjGetterTool;
class ICollectionGetterRegistryTool;


/**
 * @brief Associate from a GenParticle to a corresponding TruthParticle.
 *
 * To do this, we need to specify the collection containing the TruthParticles.
 * This can be done either by specifying an existing getter via a target
 * string, or by configuring a new getter.
 */
class GenParticleTruthParticleAssociationTool
  : public SingleAssociationTool<HepMC::GenParticle, TruthParticle>,
    public IIncidentListener
{
public:
  typedef SingleAssociationTool<HepMC::GenParticle, TruthParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GenParticleTruthParticleAssociationTool (const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent);


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c fillUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  StatusCode
  virtual configureD3PD (IAddVariable* tree,
                         const std::type_info& ti);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const TruthParticle* get (const HepMC::GenParticle& p);


private:
  /**
   * @brief Incident handler.
   * @param inc The incident.
   */
  virtual void handle (const Incident &inc);


  /// Property: The label of the collection getter defining the collection
  /// within which to look for TruthParticle instances.
  /// If empty, we instead use m_getter.
  std::string m_targetLabel;

  /// Property: The getter used to find the TruthParticle collection.
  /// Only used of m_targetLabel is empty.
  ToolHandle<IObjGetterTool> m_getter;

  /// Property: The ICollectionGetterRegistryTool instance.
  ToolHandle<ICollectionGetterRegistryTool> m_registry;

  /// Property: The incident service.
  ServiceHandle<IIncidentSvc> m_incSvc;

  /// Pointer to the getter we're using.
  IObjGetterTool* m_target;

  /// Cached TruthParticle collection.
  const TruthParticleContainer* m_container;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_GENPARTICLETRUTHPARTICLEASSOCIATIONTOOL_H
