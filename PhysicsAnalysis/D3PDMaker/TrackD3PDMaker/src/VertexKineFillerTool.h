// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/VertexKineFillerTool.h
 * @author maarten boonekamp <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for vertex kinematics, from attached tracks.
 */

#ifndef TRACKD3PDMAKER_VERTEXKINEFILLERTOOL_H
#define TRACKD3PDMAKER_VERTEXKINEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODTracking/Vertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

namespace D3PD {


/**
 * @brief Block filler tool for vertex kinematics, from attached tracks.
 */
class VertexKineFillerTool
  : public BlockFillerTool<Types<Trk::VxCandidate, xAOD::Vertex> >
{
public:
  typedef BlockFillerTool<Types<Trk::VxCandidate, xAOD::Vertex> > Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VertexKineFillerTool (const std::string& type,
			const std::string& name,
			const IInterface* parent);
  

  /// Book variables for this block.
  virtual StatusCode book();
  

  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Trk::VxCandidate& p);


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::Vertex& p);


private:

  /// Variables: Vertex position.
  float* m_px;
  float* m_py;
  float* m_pz;
  float* m_E;
  float* m_m;
  int*   m_nTracks;
  float* m_sumPt;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_VERTEXKINEFILLERTOOL_H
