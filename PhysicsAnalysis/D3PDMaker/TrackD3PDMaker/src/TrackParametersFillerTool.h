// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/TrackParametersFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2012
 * @brief Fill variables from track parameters.
 */


#ifndef TRACKD3PDMAKER_TRACKPARAMETERSFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKPARAMETERSFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrkParameters/TrackParameters.h" // typedef; can't be fwd declared


namespace D3PD {


/**
 * @brief Fill variables from track parameters.
 */
class TrackParametersFillerTool
  : public D3PD::BlockFillerTool<Trk::TrackParameters>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParametersFillerTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent);

  /**
   * @brief Book variables for this block.
   */
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const Trk::TrackParameters &p);


private:
  /// Parameter: Type of local variables: 
  /// CYLINDRICAL -- d0/z0; CARTESIAN -- locX/loxY; NONE -- not written.
  std::string m_localType;

  /// Parameter: Should theta and qoverp be filled?
  bool m_fillThetaAndQoverP;

  /// Parameter: Should the phi variable be filled?
  bool m_fillPhi;

  /// Parameter: Default value to which to set unfilled variables.
  float m_defaultValue;


  /// Variable: First local variable.
  float* m_loc1;

  /// Variable: Second local variable.
  float* m_loc2;

  /// Variable: aximuthal angle.
  float* m_phi;

  /// Variable: polar angle.
  float* m_theta;

  /// Variable: Charge over momentum.
  float* m_qoverp;
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRACKPARAMETERSFILLERTOOL_H
