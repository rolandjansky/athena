/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/ElectronTrackParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009, updated for xAOD Jun 2014
 * @brief Associate from an Electron to its TrackParticle.
 */


#include "ElectronTrackParticleAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ElectronTrackParticleAssociationTool::ElectronTrackParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const xAOD::TrackParticle*
ElectronTrackParticleAssociationTool::get (const xAOD::Electron& p)
{
  return p.trackParticle();
}


} // namespace D3PD
