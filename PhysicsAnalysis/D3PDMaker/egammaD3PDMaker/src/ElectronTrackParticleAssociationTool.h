// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/EgammaTrackParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009, updated for xAOD Jun 2014
 * @brief Associate from an Electron to its TrackParticle.
 */


#ifndef EGAMMAD3PDMAKER_ELECTRONTRACKPARTICLEASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_ELECTRONTRACKPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"


namespace D3PD {


/**
 * @brief Associate from an egamma to its TrackParticle.
 */
class ElectronTrackParticleAssociationTool
  : public SingleAssociationTool<xAOD::Electron, xAOD::TrackParticle>
{
public:
  typedef SingleAssociationTool<xAOD::Electron, xAOD::TrackParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ElectronTrackParticleAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::TrackParticle* get (const xAOD::Electron& p);
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_ELECTRONTRACKPARTICLEASSOCIATIONTOOL_H
