// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/MuonNumberOfSegmentsFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Fill number of muon segments.
 */


#ifndef MUOND3PDMAKER_MUONNUMBEROFSEGMENTSFILLERTOOL_H
#define MUOND3PDMAKER_MUONNUMBEROFSEGMENTSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODMuon/Muon.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Fill number of muon segments.
 */
class MuonNumberOfSegmentsFillerTool
  : public BlockFillerTool<xAOD::Muon>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonNumberOfSegmentsFillerTool (const std::string& type,
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
  virtual StatusCode fill (const xAOD::Muon& p);


private:
  /// Variable: Number of muon segments.
  bool* m_numberOfSegments;
};


} // namespace D3PD


#endif // not MUOND3PDMAKER_MUONNUMBEROFSEGMENTSFILLERTOOL_H
