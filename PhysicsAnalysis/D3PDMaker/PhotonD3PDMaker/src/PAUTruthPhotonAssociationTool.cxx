/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PAUTruthPhotonAssociationTool.cxx
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date March, 2010
 * @brief Associate photon to PAU egamma object
 */


#include "PAUTruthPhotonAssociationTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"
#include "FourMomUtils/P4Helpers.h"

#include "PhotonAnalysisUtils/PAUnamespace.h"
#include "PhotonAnalysisUtils/PAUegammaTruth.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUTruthPhotonAssociationTool::PAUTruthPhotonAssociationTool (const std::string& type,
							      const std::string& name,
							      const IInterface* parent)
  : Base (type, name, parent),
    m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "ICollectionGetterTool instance defining "
                   "the target collection");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode PAUTruthPhotonAssociationTool::initialize()
{
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c PAU::egamma objects from it.
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<PAU::egammaTruth>() );
  return Base::initialize();
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode PAUTruthPhotonAssociationTool::book ()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const PAU::egammaTruth*
PAUTruthPhotonAssociationTool::get (const INavigable4Momentum& p)
{
  if (m_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  const TruthParticle* p_orig = dynamic_cast<const TruthParticle*>(&p);
  if(p_orig == 0){
    REPORT_MESSAGE (MSG::ERROR) << "Can't cast input object to a TruthParticle.";
    return 0;
  }

  const PAU::egammaTruth* out = 0;
  while ( const PAU::egammaTruth* obj =
          m_getter->next<PAU::egammaTruth>())
  {
    const TruthParticle* tp = obj->matchedTruthParticle;
    if(!tp) continue;

    if(p_orig->barcode() == tp->barcode()){  // is this really the best way to compare?
      out = obj;
      break;
    }
  }

  return out;
}


} // namespace D3PD
