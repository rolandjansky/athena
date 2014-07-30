/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/DRConeAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate all particles within a DR cut.
 */

// STL include(s):
#include <cmath>

#include "DRConeAssociationTool.h"
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
DRConeAssociationTool::DRConeAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_getter (this),
      m_p (0)
{
  declareProperty ("Getter", m_getter,
                   "ICollectionGetterTool instance defining "
                   "the target collection");
  declareProperty ("DRCut", m_drcut = 999,
                   "DR cut for the association");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode DRConeAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c INavigable4Momentum objects from it.
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<INavigable4Momentum>() );
  return Base::initialize();
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode DRConeAssociationTool::reset (const INavigable4Momentum& p)
{
  m_p = &p;
  CHECK( m_getter->reset() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const INavigable4Momentum* DRConeAssociationTool::next()
{
  // Protect against null 4-momenta --- they'll crash deltaR.
  if (m_p->p() < 1e-3*MeV) {
    m_p = 0;
    return 0;
  }
  // Protect against infinite eta. It appears in some circumstances.
  if( std::isinf( m_p->eta() ) ) {
     m_p = 0;
     return 0;
  }

  while ( const INavigable4Momentum* obj =
          m_getter->next<INavigable4Momentum>())
  {
    if (obj->p() < 1e-3*MeV) continue;
    if( std::isinf( obj->eta() ) ) continue;
    float dr = static_cast<float> (P4Helpers::deltaR (*m_p, *obj));
    if (dr < m_drcut)
      return obj;
  }
  m_p = 0;
  return 0;
}


} // namespace D3PD
