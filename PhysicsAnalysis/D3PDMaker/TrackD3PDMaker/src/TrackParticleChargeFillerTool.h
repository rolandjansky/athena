// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParticleChargeFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Fill charge from a track.
 */


#ifndef TRACKD3PDMAKER_TRACKPARTICLECHARGEFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLECHARGEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODTracking/TrackParticle.h"


namespace D3PD {


class TrackParticleChargeFillerTool
  : public BlockFillerTool<xAOD::TrackParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticleChargeFillerTool (const std::string& type,
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
  virtual StatusCode fill (const xAOD::TrackParticle& p);


private:
  /// Variable: Track charge.
  float* m_charge;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_TRACKPARTICLECHARGEFILLERTOOL_H
