/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/**
 * @file egammaD3PDMaker/src/TruthParticleAssociationTool.cxx
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date December, 2011
 * @brief Associate an object to PAU egamma object
 */


#include "TruthParticleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"
#include "FourMomUtils/P4Helpers.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleAssociationTool::TruthParticleAssociationTool (const std::string& type,
							const std::string& name,
							const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("PAUtruthTool", m_PAUtruthTool,
		   "PAU truth tool instance");

}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TruthParticleAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_PAUtruthTool.retrieve() );
  return Base::initialize();
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode TruthParticleAssociationTool::book ()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const TruthParticle*
TruthParticleAssociationTool::get (const INavigable4Momentum& p)
{
  // added a disambiguation function in PAU, so I can just pass in an IN4M.
  // the trick is the second argument set to "true".
  //
  const TruthParticle* tp = m_PAUtruthTool->getTruthMatch(&p, true);
  return tp;
}


} // namespace D3PD
