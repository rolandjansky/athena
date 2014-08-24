// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaConversion0FillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for photon conversion information, for LOD 0.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMACONVERSION0FILLERTOOL_H
#define EGAMMAD3PDMAKER_EGAMMACONVERSION0FILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEgamma/Photon.h"


namespace D3PD {


/**
 * @brief Block filler tool for photon conversion information, for LOD 0.
 */
class egammaConversion0FillerTool
  : public BlockFillerTool<xAOD::Photon>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaConversion0FillerTool (const std::string& type,
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
  virtual StatusCode fill (const xAOD::Photon& p);


private:
  /// Variable: Conversion flag.
  int* m_convFlag;

  /// Variable: Is there a conversion?
  int* m_isConv;

  /// Variable: Number of conversion vertices.
  int* m_nConv;

  /// Variable: Number of conversion vertices with exactly one track.
  int* m_nSingleTrackConv;

  /// Variable: Number of conversion vertices with exactly two tracks.
  int* m_nDoubleTrackConv;
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_EGAMMACONVERSION0FILLERTOOL_H
