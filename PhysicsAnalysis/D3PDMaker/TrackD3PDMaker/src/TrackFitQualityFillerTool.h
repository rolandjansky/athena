// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FitQualityFillerTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrackFitQualityFillerTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for generic fit quality.
 */
#ifndef TRACKD3PDMAKER_TRACKFITQUALITYFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKFITQUALITYFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

namespace Trk {
  class FitQuality;
}


namespace D3PD {


/**
 * @brief Block filler tool for vertex position.
 */
class TrackFitQualityFillerTool
  : public BlockFillerTool<Trk::FitQuality>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackFitQualityFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Trk::FitQuality& p);


private:

  void clearData();

  /// Variables:
  float* m_chi2;
  int*   m_ndof;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_TRACKFITQUALITYFILLERTOOL_H
