/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleTruthParticleAssociationTool.cxx 353417 2011-03-22 10:16:22Z ssnyder $
/**
 * @file EventCommonD3PDMaker/GenParticleTruthParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2010
 * @brief Associate from a GenParticle to a corresponding TruthParticle.
 */


#include "GenParticleTruthParticleAssociationTool.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "AtlasHepMC/GenParticle.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
GenParticleTruthParticleAssociationTool::GenParticleTruthParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_getter ("D3PD::SGObjGetterTool", this),
      m_incSvc ("IncidentSvc", name),
      m_target (0),
      m_container (0)
{
  declareProperty ("Target",        m_targetLabel,
                   "Label of the collection getter defining the collection "
                   "within which find the TruthParticle instances.  "
                   "If this is blank, then Getter is used instead.");
  declareProperty ("Getter", m_getter,
                   "Getter to find the TruthParticle collection to use.  "
                   "Only used if Target is empty.");
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "The ICollectionGetterRegistryTool instance.");
  declareProperty ("IncidentSvc", m_incSvc,
                   "The incident service.");
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
GenParticleTruthParticleAssociationTool::configureD3PD
  (IAddVariable* tree,
   const std::type_info& ti)
{
  CHECK ( Base::configureD3PD (tree, ti) );

  // Is the collection specified via target?
  if (!m_targetLabel.empty()) {
    // Look up the target.
    CHECK (m_registry.retrieve() );
    ICollectionGetterTool* cget = 0;
    CHECK (m_registry->get (m_targetLabel, this, cget) );
    m_target = cget;
  }
  else {
    // Target was empty.  Use a private getter tool.
    CHECK (m_getter.retrieve() );
    m_target = &*m_getter;
  }

  // Set up to clear the cached container pointer at the end of the event.
  CHECK( m_incSvc.retrieve() );
  m_incSvc->addListener (this, "EndEvent");

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const TruthParticle*
GenParticleTruthParticleAssociationTool::get (const HepMC::GenParticle& p)
{
  if (!m_target)
    return 0;

  if (!m_container)
    m_container = m_target->get<TruthParticleContainer>();

  if (!m_container) {
    REPORT_MESSAGE (MSG::WARNING) << "Couldn't find TruthParticleContainer.";
    return 0;
  }

  return m_container->truthParticle (p.barcode());
}


/**
 * @brief Incident handler.
 * @param inc The incident.
 */
void GenParticleTruthParticleAssociationTool::handle (const Incident &inc)
{
  // Clear the cached container pointer at the end of the event.
  if (inc.type() == "EndEvent")
  {
    m_container = 0;
  }
}


} // namespace D3PD
