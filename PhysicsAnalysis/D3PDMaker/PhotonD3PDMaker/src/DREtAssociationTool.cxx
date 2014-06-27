/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DREtAssociationTool.cxx 547010 2013-05-07 07:44:50Z jmarchan $
/**
 * @file PhotonD3PDMaker/src/DREtAssociationTool.cxx
 * @author Mike Hance, based on DRAssociationTool by Scott Snyder
 * @date Mar. 2010
 * @brief Associate to highest E_T particle in a cone of size dR.
 */


#include "DREtAssociationTool.h"
#include "FourMomUtils/P4Helpers.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"

#include "egammaEvent/Photon.h"
#include "CaloEvent/CaloCluster.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
DREtAssociationTool::DREtAssociationTool (const std::string& type,
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
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode DREtAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c INavigable4Momentum objects from it.
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<INavigable4Momentum>() );
  return Base::initialize();
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode DREtAssociationTool::book ()
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
DREtAssociationTool::get (const INavigable4Momentum& p)
{
  *m_dr = -1;

  if (m_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  const Analysis::Photon* pho = dynamic_cast< const Analysis::Photon* >(&p);

  const INavigable4Momentum* pout = 0;
  float max_pt = 0.;
  while ( const INavigable4Momentum* obj =
          m_getter->next<INavigable4Momentum>())
  {
    float dr = (pho && pho->cluster()) ? static_cast<float> (P4Helpers::deltaR (*pho->cluster(), *obj)) : static_cast<float> (P4Helpers::deltaR (p, *obj));
 
    if (dr < m_drcut && obj->pt() > max_pt) {
      max_pt = obj->pt();
      * m_dr = dr;
      pout = obj;
    }
  }
  return pout;
}


} // namespace D3PD
