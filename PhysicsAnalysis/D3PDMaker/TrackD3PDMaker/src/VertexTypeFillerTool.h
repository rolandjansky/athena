// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexTypeFillerTool.h 456341 2011-08-31 11:33:30Z wildauer $
/**
 * @file TrackD3PDMaker/src/VertexTypeFillerTool.h
 * @author Andreas Wildauer
 * @date Aug, 2011
 * @brief Block filler tool for vertex type. The vertex type does not fit into any other filler tool. Hence it gets its own.
 */

#ifndef TRACKD3PDMAKER_VERTEXTYPEFILLERTOOL_H
#define TRACKD3PDMAKER_VERTEXTYPEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"

#include "VxVertex/VxCandidate.h"

namespace D3PD {


/**
 * @brief Block filler tool for vertex type. The vertex type does not fit into any other filler tool. Hence it gets its own.
 */
class VertexTypeFillerTool
  : public BlockFillerTool<Trk::VxCandidate>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VertexTypeFillerTool (const std::string& type,
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

  int*   m_type;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_VERTEXKINEFILLERTOOL_H
