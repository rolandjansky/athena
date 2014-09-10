// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/ErrorMatrixCovarianceFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Fill covariance elements from ErrorMatrix.
 */


#ifndef TRACKD3PDMAKER_ERRORMATRIXCOVARIANCEFILLERTOOL_H
#define TRACKD3PDMAKER_ERRORMATRIXCOVARIANCEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "EventPrimitives/EventPrimitives.h"
#include <vector>



namespace D3PD {


/**
 * @brief Fill covariance elements from ErrorMatrix.
 */
class ErrorMatrixCovarianceFillerTool
  : public D3PD::BlockFillerTool<AmgSymMatrix(5)>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ErrorMatrixCovarianceFillerTool (const std::string& type,
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
  virtual StatusCode fill (const AmgSymMatrix(5) &p);


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


  struct Var {
    /// The D3PD variable being filled.
    float* var;

    /// First parameter code for this variable.
    Trk::ParamDefs v1;

    /// Second parameter code for this variable.
    Trk::ParamDefs v2;

    Var() : var(0), v1(Trk::d0), v2(Trk::z0) {}
  };


  /// Variables being filled.
  std::vector<Var> m_vars;
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_ERRORMATRIXCOVARIANCEFILLERTOOL_H
