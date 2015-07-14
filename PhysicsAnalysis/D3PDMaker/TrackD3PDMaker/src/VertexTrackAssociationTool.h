/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PD_VERTEXTRACKASSOCIATIONTOOL_H
#define TRACKD3PD_VERTEXTRACKASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkParameters/TrackParameters.h"


#include <vector>

namespace Trk{
class VxCandidate;
class VxTrackAtVertex;
}

namespace D3PD {

class VertexTrackAssociationTool
  : public MultiAssociationTool<Trk::VxCandidate, Trk::VxTrackAtVertex> 
{
public:
  typedef MultiAssociationTool<Trk::VxCandidate, Trk::VxTrackAtVertex> Base;

  VertexTrackAssociationTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent);

  virtual StatusCode initialize ();
  virtual StatusCode book();
 
  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Trk::VxCandidate& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const Trk::VxTrackAtVertex* next();
  virtual void releaseElement (const Trk::VxTrackAtVertex* track);

private:

  Trk::Perigee *getPerigeeAtVertex(const Trk::VxTrackAtVertex *);

  void clearData();

  std::vector<Trk::VxTrackAtVertex*>::const_iterator m_trkItr;
  std::vector<Trk::VxTrackAtVertex*>::const_iterator m_trkEnd;

  const Trk::VxCandidate *m_vx;

  /* Track to vertex extrapolation tools **/
  ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
  ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;

  bool m_fillUnbiasedIP;

  /* NTuple variables: **/
  float *m_weight;
  float *m_unbiased_d0;
  float *m_unbiased_z0;
  float *m_err_unbiased_d0;
  float *m_err_unbiased_z0;


};

} // namespace D3PD


#endif // not TRACKD3PD_VERTEXTRACKASSOCIATIONTOOL_H
