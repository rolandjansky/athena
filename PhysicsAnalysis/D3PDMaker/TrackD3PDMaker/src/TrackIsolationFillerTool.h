// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackIsolationFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2010
 * @brief Fill track isolation variables.
 */


#ifndef TRACKD3PDMAKER_TRACKISOLATIONFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKISOLATIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

namespace xAOD {
class IParticle;
class ITrackIsolationTool;
class ICaloCellIsolationTool;
}


namespace D3PD {


/**
 * @brief Fill track isolation variables.
 */
class TrackIsolationFillerTool
  : public BlockFillerTool<xAOD::IParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackIsolationFillerTool (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);


  // Standard Gaudi initialize method.
  StatusCode initialize();


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
  virtual StatusCode fill (const xAOD::IParticle& p);


private:
  /**
   * @brief Return a vector of isolation type codes for the requested
   *        cone sizes and a specified flavor.
   * @param f The isolation flavor.
   */
  std::vector<xAOD::Iso::IsolationType>
  cones (xAOD::Iso::IsolationFlavour f) const;

  /// Parameters: Names for the tuple variables.
  /// If blank, the variables won't be filled.
  /// If it contains a `%%', this will be replaced by the
  /// cone size.  Otherwise, if more than one cone size is
  /// requested, the the cone size will be appended to the
  /// variable name.
  /// Unfortunate we have to do this, but we're trying to accomodate
  /// multiple different naming conventions.
  std::string m_trackIsoVar;
  std::string m_caloIsoVar;

  /// Parameter: List of isolation cone sizes.
  std::vector<double> m_coneSizes;

  /// Parameter: Isolation cone size.
  /// Only used if m_coneSizes is empty.
  float m_coneSize;

  /// Parameter: Track isolation tool.
  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsoTool;

  /// Parameter: Calorimeter isolation tool.
  ToolHandle<xAOD::ICaloCellIsolationTool> m_caloIsoTool;

  /// Variable: Tracking isolation parameter.
  std::vector<float*> m_trackIso;
  /// Variable: Calorimeter isolation parameter.
  std::vector<float*> m_caloIso;

  /// Helper to book isolation variables.
  template <class T>
  StatusCode bookIsoVar (std::string varName,
                         std::vector<T>& var,
                         const std::string& docString);

};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_TRACKISOLATIONFILLERTOOL_H
