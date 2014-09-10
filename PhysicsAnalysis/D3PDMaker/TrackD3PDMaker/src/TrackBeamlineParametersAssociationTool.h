// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackBeamlineParametersAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Associate from a track to the corresponding parameters
 *        evaluated at the beamline.
 *
 * The input object can be any of @c Track, @c TrackParameters,
 * or @c TrackParticle.
 */


#ifndef TRACKD3PDMAKER_TRACKBEAMLINEPARAMETERSASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRACKBEAMLINEPARAMETERSASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkParameters/TrackParameters.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "GaudiKernel/ToolHandle.h"
namespace Trk {
  class Track;
}
namespace Rec {
  class TrackParticle;
}


namespace D3PD {


/**
 * @brief Associate from a track to the corresponding parameters
 *        evaluated at the beamline.
 *
 * The input object can be any of @c Track, @c TrackParameters,
 * or @c TrackParticle.
 */
class TrackBeamlineParametersAssociationTool
  : public SingleAssociationTool<Types<Trk::Track,
                                       Trk::TrackParameters,
                                       Rec::TrackParticle>,
                                       Trk::TrackParameters>
{
public:
  typedef SingleAssociationTool<Types<Trk::Track,
                                      Trk::TrackParameters,
                                      Rec::TrackParticle>,
                                      Trk::TrackParameters> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackBeamlineParametersAssociationTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackParameters* get (const Trk::Track& p);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackParameters* get (const Rec::TrackParticle& p);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::TrackParameters* get (const Trk::TrackParameters& p);


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObject (const Trk::TrackParameters* p);


private:
  /// Vertex extrapolator.
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_TRACKBEAMLINEPARAMETERSASSOCIATIONTOOL_H
