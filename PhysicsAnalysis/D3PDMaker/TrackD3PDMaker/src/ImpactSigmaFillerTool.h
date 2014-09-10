// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ImpactSigmaFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Fill d0/z0 uncertainties from an ErrorMatrix.
 *
 * This is essentially redundant with CovarianceFillerTool.
 * However, it's added as a separate class to avoid cluttering
 * CovarianceFillerTool with yet more configuration options.
 */


#ifndef TRACKD3PDMAKER_IMPACTSIGMAFILLERTOOL_H
#define TRACKD3PDMAKER_IMPACTSIGMAFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include <string>



namespace D3PD {


/**
 * @brief Fill d0/z0 uncertainties from an ErrorMatrix.
 */
//class ImpactSigmaFillerTool
//  : public BlockFillerTool<AmgSymMatrix(5)>
class ImpactSigmaFillerTool
  : public BlockFillerTool<AmgSymMatrix(5)>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ImpactSigmaFillerTool (const std::string& type,
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
  //virtual StatusCode fill (const AmgSymMatrix(5)& p);
  virtual StatusCode fill (const AmgSymMatrix(5) &p);


private:
  /// Variables: d0 uncertainty.
  float* m_sigd0;

  /// Variables: z0 uncertainty.
  float* m_sigz0;
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_IMPACTSIGMAFILLERTOOL_H
