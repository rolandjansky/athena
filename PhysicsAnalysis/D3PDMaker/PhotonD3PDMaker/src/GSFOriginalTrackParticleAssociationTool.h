// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/**
 * @file egammaD3PDMaker/src/GSFOriginalTrackParticleAssociationTool.h
 * @author Mike Hance <mike.hance@cern.ch>
 * @date March, 2012
 * @brief Associate from a GSF fitted track to the original trackparticle
 */


#ifndef EGAMMAD3PDMAKER_GSFOLDTRACKPARTICLEASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_GSFOLDTRACKPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Rec {
  class TrackParticle;
}


namespace D3PD {

class IObjGetterTool;


/**
 * @brief Associate from a GSF fitted track to the original trackparticle
 */
class GSFOriginalTrackParticleAssociationTool
  : public SingleAssociationTool<Rec::TrackParticle, Rec::TrackParticle>
{
public:
  typedef SingleAssociationTool<Rec::TrackParticle, Rec::TrackParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GSFOriginalTrackParticleAssociationTool (const std::string& type,
					   const std::string& name,
					   const IInterface* parent);


  /// @brief Standard Gaudi initialize method.
  virtual StatusCode initialize();

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Rec::TrackParticle* get (const Rec::TrackParticle& track);

private:
  /// Property: Getter for the association object.
  ToolHandle<IObjGetterTool> m_assocGetter;
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_GSFORIGINALTRACKPARTICLEASSOCIATIONTOOL_H
