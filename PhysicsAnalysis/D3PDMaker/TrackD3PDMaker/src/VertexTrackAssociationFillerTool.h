/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_VERTEXTRACKASSOCIATIONFILLERTOOL_H
#define TRACKD3PDMAKER_VERTEXTRACKASSOCIATIONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

namespace Trk{
class VxTrackAtVertex;
}

namespace D3PD {

class VertexTrackAssociationFillerTool
  : public BlockFillerTool<Trk::VxTrackAtVertex>
{
public:
  typedef BlockFillerTool<Trk::VxTrackAtVertex> Base;

  VertexTrackAssociationFillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent);

  virtual StatusCode book();

  virtual StatusCode fill (const Trk::VxTrackAtVertex &track);

private:

  void clearData();

  /* NTuple variables: **/
  float *m_chi2;

  float *m_d0;
  float *m_z0;
  float *m_phi;
  float *m_theta;

}; // class VertexTrackAssociationFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_VERTEXTRACKASSOCIATIONFILLERTOOL_H
