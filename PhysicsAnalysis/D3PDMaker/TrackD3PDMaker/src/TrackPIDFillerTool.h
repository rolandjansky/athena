// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackPIDFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2010
 * @brief Fill in @c getPID items from @c TrackSummary.
 */


#ifndef TRACKD3PDMAKER_TRACKPIDFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKPIDFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include <vector>
#include <string>


namespace D3PD {


/**
 * @brief Fill in @c getPID items from @c TrackSummary.
 */
class TrackPIDFillerTool
  : public BlockFillerTool<Trk::TrackSummary>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackPIDFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Trk::TrackSummary& p);


private:
  /// Property: Pairs of PID,NAME.
  std::vector<std::string> m_pid;

  struct Var
  {
    /// PID type for this variable.
    Trk::eProbabilityType key;

    /// The variable being filled.
    float* x;
  };

  /// List of variables.
  std::vector<Var> m_vars;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_TRACKPIDFILLERTOOL_H
