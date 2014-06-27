// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PhotonPVFillerTool.h
 * @author Jean-Francois Marchand <jean-francois.marchand@cern.ch>
 * @date April 2011
 * @brief Block filler tool for primary vertex kinematics, from attached tracks. Adapted from TrackD3PDMaker/src/VertexKineFillerTool.h
 */

#ifndef PHOTOND3PDMAKER_PRIMARYVERTEXFILLERTOOL_H
#define PHOTOND3PDMAKER_PRIMARYVERTEXFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

namespace D3PD {

/**
 * @brief Block filler tool for vertex kinematics, from attached tracks.
 */
class PhotonPVFillerTool
  : public BlockFillerTool<Trk::VxCandidate>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PhotonPVFillerTool (const std::string& type,
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


private:

  /// Variables: Vertex position.
  float* m_px;
  float* m_py;
  float* m_pz;
  float* m_E;
  float* m_m;
  int*   m_nTracks;
  float* m_sumPt;
  float* m_sumPt2;
  int*   m_type;
  float* m_ptmiss;
  float* m_meanpt;
  int* m_nTracks05GeV;
  int* m_nTracks07GeV;
  int* m_nTracks1GeV;
  int* m_nTracks2GeV;

};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_PRIMARYVERTEXFILLERTOOL_H
