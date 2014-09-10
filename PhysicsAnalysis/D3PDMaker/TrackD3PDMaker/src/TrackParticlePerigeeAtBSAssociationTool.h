// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticlePerigeeAtBSAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackParticlePerigeeAtBSAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrackParticle to its perigee at the beam spot.
 */
#ifndef TRACKD3PDMAKER_TRACKPARTICLEPERIGEEATBSASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEPERIGEEATBSASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticle.h" 
#include "GaudiKernel/ToolHandle.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include <vector>


namespace Rec {
  class TrackParticle;
}

namespace D3PD {

/**
 * @brief Associate from a VxCandidate to its perigee at the beam spot.
 */
class TrackParticlePerigeeAtBSAssociationTool
  : public SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>, Trk::TrackParameters>
{
public:
  typedef SingleAssociationTool<Types<Rec::TrackParticle, xAOD::TrackParticle>, Trk::TrackParameters> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticlePerigeeAtBSAssociationTool (const std::string& type,
					   const std::string& name,
					   const IInterface* parent);


  virtual StatusCode initialize() override;
  

  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackParameters* get (const Rec::TrackParticle& p) override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackParameters* get (const xAOD::TrackParticle& p) override;


  virtual void releaseObject (const Trk::TrackParameters* p) override;


 private:

  /* the beam condition service **/
  IBeamCondSvc* m_iBeamCondSvc;
  std::string   m_beamCondSvcName;

  /* Track to vertex extrapolator **/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRACKPARTICLEPERIGEEATBSASSOCIATIONTOOL_H
