// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexPositionFillerTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/VertexPositionFillerTool.h
 * @author maarten boonekamp <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for vertex position.
 */

#ifndef TRACKD3PDMAKER_VERTEXPOSITIONFILLERTOOL_H
#define TRACKD3PDMAKER_VERTEXPOSITIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
namespace Trk {
  class VxCandidate;
}


namespace D3PD {


/**
 * @brief Block filler tool for vertex position.
 */
class VertexPositionFillerTool
  : public BlockFillerTool<Trk::VxCandidate>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VertexPositionFillerTool (const std::string& type,
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
  float* m_x;
  float* m_y;
  float* m_z;
  
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_VERTEXPOSITIONFILLERTOOL_H
