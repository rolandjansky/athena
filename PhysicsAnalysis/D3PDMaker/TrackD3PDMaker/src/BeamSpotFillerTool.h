// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexPositionFillerTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/BeamSpotFillerTool.h
 * @author remi zaidan <zaidan@cern.ch>
 * @date Jan, 2011
 * @brief Block filler tool for beam spot.
 */

#ifndef TRACKD3PDMAKER_BEAMSPOTFILLERTOOL_H
#define TRACKD3PDMAKER_BEAMSPOTFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Trk {
  class RecVertex;
}


namespace D3PD {


/**
 * @brief Block filler tool for vertex position.
 */
class BeamSpotFillerTool
  : public BlockFillerTool<Trk::RecVertex>
{
public:
  typedef BlockFillerTool<Trk::RecVertex> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  BeamSpotFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Trk::RecVertex& p);

private:

  /// Variables: 
  // Beam spot position.
  float* m_x;
  float* m_y;
  float* m_z;
  // Beam spot size:
  float* m_sigma_x;
  float* m_sigma_y;
  float* m_sigma_z;
  // Beam spot tilt
  float* m_tilt_x;
  float* m_tilt_y;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_BEAMSPOTFILLERTOOL_H
