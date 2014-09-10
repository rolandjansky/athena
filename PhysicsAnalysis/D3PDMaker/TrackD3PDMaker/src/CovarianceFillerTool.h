// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CovarianceFillerTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/CovarianceFillerTool.h
 * @author maarten boonekamp <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for error and covariance matrix.
 */
#ifndef TRACKD3PDMAKER_COVARIANCEFILLERTOOL_H
#define TRACKD3PDMAKER_COVARIANCEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerToolMulti.h"
#include "EventPrimitives/EventPrimitives.h"

#include <vector>

namespace D3PD {


/**
 * @brief Block filler tool for error and covariance matrix.
 */
class CovarianceFillerTool
  : public BlockFillerTool<Types<AmgSymMatrix(5),AmgSymMatrix(3),Amg::MatrixX>>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CovarianceFillerTool (const std::string& type,
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
  virtual StatusCode fill (const AmgSymMatrix(5) &p);

  virtual StatusCode fill (const AmgSymMatrix(3) &p);

  virtual StatusCode fill (const Amg::MatrixX &p);

private:

  /// parameters
  bool m_writeError;
  bool m_writeDiagCovariance;
  bool m_writeOffDiagCovariance;
  bool m_isPosition;
  bool m_isTrackPerigee;
  int m_nParameters;

  /// Variables: uncertainty.
  std::vector<float*> m_err;

  /// Variables: covariance matrix
  std::vector<float*> m_cov;
  
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_COVARIANCEFILLERTOOL_H
