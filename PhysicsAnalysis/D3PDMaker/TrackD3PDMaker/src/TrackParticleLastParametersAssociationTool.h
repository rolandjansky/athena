// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParticleLastParametersAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Associate from a track particle to the last set of parameters.
 */


#ifndef TRACKD3PDMAKER_TRACKPARTICLELASTPARAMETERSASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLELASTPARAMETERSASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkParameters/TrackParameters.h" // typedef; can't be fwd declared
namespace Rec {
  class TrackParticle;
}


namespace D3PD {


/**
 * @brief Associate from a track particle to the last set of parameters.
 */
class TrackParticleLastParametersAssociationTool
  : public SingleAssociationTool<Rec::TrackParticle, Trk::TrackParameters>
{
public:
  typedef SingleAssociationTool<Rec::TrackParticle, Trk::TrackParameters> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticleLastParametersAssociationTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackParameters* get (const Rec::TrackParticle& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRACKPARTICLELASTPARAMETERSASSOCIATIONTOOL_H
