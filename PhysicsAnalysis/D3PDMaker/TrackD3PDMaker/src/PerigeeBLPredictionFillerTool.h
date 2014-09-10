// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/PerigeeBLPredictionFillerTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Apr, 2010
 * @brief Fill BLayer prediction Info from a Perigee.
 */
#ifndef TRACKD3PDMAKER_PERIGEEBLPREDICTIONFILLERTOOL_H
#define TRACKD3PDMAKER_PERIGEEBLPREDICTIONFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"

class PixelID;

namespace InDet {
  class IInDetTestBLayerTool;
}


namespace D3PD {

/**
 * @brief Fill expectHitInBLayer from a Perigee.
 */
class PerigeeBLPredictionFillerTool
  : public BlockFillerTool<Trk::Perigee>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
   PerigeeBLPredictionFillerTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent);

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();

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
  virtual StatusCode fill (const Trk::Perigee& p);

private:

  void clearData();

  /// Parameter: Tool to test if the track crosses a dead module on the B-Layer.
  ToolHandle< InDet::IInDetTestBLayerTool > m_inDetTestBLayerTool;

  const PixelID* m_pixId; 

  /// Variables:

  // Track predictions on the B-layer
  std::vector<float> *m_x;
  std::vector<float> *m_y;
  std::vector<float> *m_z;
  std::vector<float> *m_locX;
  std::vector<float> *m_locY;
  std::vector<float> *m_err_locX;
  std::vector<float> *m_err_locY;
  std::vector<float> *m_etaDistToEdge;
  std::vector<float> *m_phiDistToEdge;
  std::vector<Identifier32::value_type> *m_detElementId; // unique identifier for a detector element
  std::vector<int> *m_row;
  std::vector<int> *m_col;
  std::vector<int> *m_type;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_PERIGEEBLPREDICTIONFILLERTOOL_H
