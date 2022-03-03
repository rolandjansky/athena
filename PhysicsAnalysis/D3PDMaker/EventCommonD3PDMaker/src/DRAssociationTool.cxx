/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonD3PDMaker/src/DRAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate to particle closest in DR.
 */

// STL include(s):
#include <cmath>

#include "DRAssociationTool.h"
#include "FourMomUtils/P4Helpers.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/SystemOfUnits.h"


using Gaudi::Units::MeV;


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
DRAssociationTool::DRAssociationTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : Base (type, name, parent),
    m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "ICollectionGetterTool instance defining "
                   "the target collection");
  declareProperty ("DRCut", m_drcut = 999,
                   "DR cut for the association");

  DRAssociationTool::book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode DRAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c INavigable4Momentum objects from it.
  CHECK( m_getter.retrieve() );
  //CHECK( m_getter->configureD3PD<INavigable4Momentum>() );
  //CHECK( m_getter->configureElementTypeinfo
  return Base::initialize();
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c fillUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode DRAssociationTool::configureD3PD (IAddVariable* tree,
                                             const std::type_info& ti)
{
  CHECK( Base::configureD3PD (tree, ti) );
  CHECK( m_getter->configureElementTypeinfo (this->typeinfo()));
  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode DRAssociationTool::book ()
{
  CHECK( addVariable ("dr", m_dr) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const INavigable4Momentum*
DRAssociationTool::get (const INavigable4Momentum& p)
{
  // Protect against null 4-momenta --- they'll crash deltaR.
  if (p.p() < 1e-3*MeV) return 0;
  // Protect against infinite eta. It appears in some circumstances.
  if( std::isinf(p.eta()) ) return 0;

  *m_dr = -1;

  if (m_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  const INavigable4Momentum* pout = 0;
  float min_dr = m_drcut;
  while ( const INavigable4Momentum* obj =
          m_getter->next<INavigable4Momentum>())
  {
    if (obj->pt() < 1e-2*MeV) continue;
    if( std::isinf( obj->eta() ) ) continue;
    float dr = static_cast<float> (P4Helpers::deltaR (p, *obj));
    if (dr < min_dr) {
      *m_dr = dr;
      min_dr = dr;
      pout = obj;
    }
  }
  return pout;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const xAOD::IParticle*
DRAssociationTool::get (const xAOD::IParticle& p)
{
  // Protect against null 4-momenta --- they'll crash deltaR.
  if (p.e() < 1e-3*MeV) return 0;
  // Protect against infinite eta. It appears in some circumstances.
  if( std::isinf(p.eta()) ) return 0;

  *m_dr = -1;

  if (m_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  const xAOD::IParticle* pout = 0;
  float min_dr = m_drcut;
  while ( const xAOD::IParticle* obj =
          m_getter->next<xAOD::IParticle>())
  {
    if (obj->pt() < 1e-2*MeV) continue;
    if( std::isinf( obj->eta() ) ) continue;
    float dr = p.p4().DeltaR (obj->p4());
    if (dr < min_dr) {
      *m_dr = dr;
      min_dr = dr;
      pout = obj;
    }
  }
  return pout;
}


} // namespace D3PD
