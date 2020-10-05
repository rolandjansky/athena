/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_MVFVXTRACKATVERTEX_H
#define VXMULTIVERTEX_MVFVXTRACKATVERTEX_H

#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxTrackAtVertex.h"
namespace Trk {

/**
 *   @class  Trk::MVFVxTrackAtVertex.h
 *
 * This class inherits from VxTrackAtVertex, adding for the "Multi Vertex
 * Fitter" the following information: the Trk::TrackToVtxLink object, which
 * stores the information about the association of a track to a set of vertices
 *
 * begin   : August 2006
 * @authors : Giacinto Piacquadio (Freiburg University)
 * giacinto.piacquadio@physik.uni-freiburg.de
 */

class TrackToVtxLink;
class Track;

class MVFVxTrackAtVertex : public VxTrackAtVertex
{
public:
  /**
   * Default constructor
   */
  MVFVxTrackAtVertex();

  /**
   * Constructor requiring a chi2 per track and a track parameters at vertex as
   * input
   */
  MVFVxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex);

  /**
   * Constructor requiring a chi2 per track, track parameters at vertex
   * and initial perigee as input.
   */
  MVFVxTrackAtVertex(double chi2PerTrk,
                     TrackParameters* perigeeAtVertex,
                     TrackParameters* initialPerigee);

  /**
   * Constructor requiring a chi2 per track, track parameters at vertex
   * and a linearized track state.
   */
  MVFVxTrackAtVertex(double chi2PerTrk,
                     TrackParameters* perigeeAtVertex,
                     double ndfPerTrk,
                     Trk::LinearizedTrack* linState = nullptr);

  /**
   *Constructor requiring a chi2 per track,initial track parameters, track
   *parameters at vertex and a linearized track state.
   */
  MVFVxTrackAtVertex(double chi2PerTrk,
                     TrackParameters* perigeeAtVertex,
                     TrackParameters* initialPerigee,
                     double ndfPerTrk,
                     Trk::LinearizedTrack* linState = nullptr);

  /**
   * Constructor with only trackOrParticleLink (no TrackToVtxLink);
   */

  MVFVxTrackAtVertex(Trk::ITrackLink* trackOrParticleLink);

  /**
   * Constructor using a link to vertices + trackParticle origin.
   */
  MVFVxTrackAtVertex(TrackToVtxLink* actuallink,
                     const Track* track,
                     const TrackCollection* trackTES);

  MVFVxTrackAtVertex(TrackToVtxLink* actuallink,
                     const Trk::TrackParticleBase* trackparticle,
                     const Trk::TrackParticleBaseCollection* trkpartTES);

  /**
   *
   */

  MVFVxTrackAtVertex(Trk::ITrackLink* trackOrParticleLink,
                     TrackToVtxLink* actuallink);

  /**
   * Destructor
   */
  virtual ~MVFVxTrackAtVertex();

  /**
   * Copy constructor
   */
  MVFVxTrackAtVertex(const MVFVxTrackAtVertex& rhs);

  /**
   * Assignement operator
   */
  MVFVxTrackAtVertex& operator=(const MVFVxTrackAtVertex&);

  /**
   * A clone method for the proper workings of the copy constructor in
   * VxCandidate
   */
  virtual MVFVxTrackAtVertex* clone() const override;

  /**
   * Unconst access to  linkToVertices object
   */
  Trk::TrackToVtxLink* linkToVertices(void);

  /**
   * Const access to  linkToVertices object
   */
  const Trk::TrackToVtxLink* linkToVertices(void) const;

  /**
   * Set method for linkToVertices object
   */
  void setLinkToVertices(TrackToVtxLink* trk);

  /**
   * Output Method for MsgStream, to be overloaded by child classes
   */
  virtual MsgStream& dump(MsgStream& sl) const override;

  /**
   * Output Method for std::ostream, to be overloaded by child classes
   */
  virtual std::ostream& dump(std::ostream& sl) const override;

protected:
  /**
   * Pointer to the TrackToVtxLink object, which contains the association of
   * this track with all the other vertices (multi vertex fit)
   */
  TrackToVtxLink* m_linkToVertices;
};

inline Trk::MVFVxTrackAtVertex*
Trk::MVFVxTrackAtVertex::clone() const
{
  return new MVFVxTrackAtVertex(*this);
}

inline Trk::TrackToVtxLink*
Trk::MVFVxTrackAtVertex::linkToVertices(void)
{
  return m_linkToVertices;
}

inline const Trk::TrackToVtxLink*
Trk::MVFVxTrackAtVertex::linkToVertices(void) const
{
  return m_linkToVertices;
}

}
#endif

