/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/TruthParticleEtIsolationAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Associate from a TruthParticle to the set of isolation cones.
 */


#include "TruthParticleEtIsolationAssociationTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleEtIsolationAssociationTool::TruthParticleEtIsolationAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
    m_getter ("D3PD::SGDataVectorGetterTool", this)
{
  declareProperty ("Getter", m_getter,
                   "Getter to find the isolations container.");
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
TruthParticleEtIsolationAssociationTool::configureD3PD
  (IAddVariable* tree,
   const std::type_info& ti)
{
  CHECK ( Base::configureD3PD (tree, ti) );

  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<TruthEtIsolations>() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const McAod::EtIsolations*
TruthParticleEtIsolationAssociationTool::get (const TruthParticle& p)
{
  if ( !m_getter->reset().isSuccess() ) {
    REPORT_MESSAGE (MSG::WARNING) << "Can't find truth isolations.";
    return 0;
  }

  while (const TruthEtIsolations* isoc = m_getter->next<TruthEtIsolations>()) {
    const McAod::EtIsolations* iso = isoc->etIsolations (p.barcode());
    if (iso) return iso;
  }
  return 0;
}


} // namespace D3PD
