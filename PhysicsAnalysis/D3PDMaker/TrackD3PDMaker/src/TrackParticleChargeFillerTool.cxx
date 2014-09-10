/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParticleChargeFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Fill charge from a track.
 */


#include "TrackParticleChargeFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticleChargeFillerTool::TrackParticleChargeFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<xAOD::TrackParticle> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}



/**
 * Book variables for this block.
 */
StatusCode TrackParticleChargeFillerTool::book()
{
  CHECK( addVariable ("charge",    m_charge) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TrackParticleChargeFillerTool::fill (const xAOD::TrackParticle& p)
{
  *m_charge = p.charge();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
