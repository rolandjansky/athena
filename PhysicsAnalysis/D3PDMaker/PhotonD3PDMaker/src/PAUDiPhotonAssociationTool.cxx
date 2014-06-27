/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUDiPhotonAssociationTool.cxx
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date August 2010
 * @brief Associate PAU DiPhoton object to two photons
 */


#include "PAUDiPhotonAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "AthenaKernel/errorcheck.h"

#include "egammaEvent/Photon.h"
#include "PhotonAnalysisUtils/PAUDiPhoton.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUDiPhotonAssociationTool::PAUDiPhotonAssociationTool (const std::string& type,
							const std::string& name,
							const IInterface* parent)
  : Base (type, name, parent),
    m_photonCounter (0),
    m_photon_1 (0),
    m_photon_2 (0)
{
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode PAUDiPhotonAssociationTool::reset (const PAU::DiPhoton& dp)
{

  const std::vector<PAU::egamma*>* photons = dp.getPhotons();
  if(photons->size() != 2){
    m_photon_1 = 0;
    m_photon_2 = 0;
  }
  else{
    m_photon_1 = photons->at(0);
    m_photon_2 = photons->at(1);
  }
  m_photonCounter = 0;

  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Analysis::Photon*
PAUDiPhotonAssociationTool::next ()
{
  if(m_photonCounter > 1)
    return 0;

  m_photonCounter++;
  if     (m_photonCounter == 1)   return m_photon_1->photon();
  else if(m_photonCounter == 2)   return m_photon_2->photon();
  
  return 0;
}


} // namespace D3PD
