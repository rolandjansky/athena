// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaTruthClassificationFillerTool.h
 * @author scott snyder
 * @date Jan, 2010
 * @brief Fill in type/origin from MC classifier tool for an egamma.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMATRUTHCLASSIFICATIONFILLERTOOL_H
#define EGAMMAD3PDMAKER_EGAMMATRUTHCLASSIFICATIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODTruth/TruthParticle.h"
#include "egammaEvent/egammaParamDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <string>


namespace D3PD {


/**
 * @brief Fill in type/origin from MC classifier tool for an egamma.
 */
class egammaTruthClassificationFillerTool
  : public BlockFillerTool<xAOD::Egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaTruthClassificationFillerTool (const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent);


  /// Standard Gaudi initialize method.
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
  virtual StatusCode fill (const xAOD::Egamma& p);


private:
  /// Property: classifier tool.
  ToolHandle<IMCTruthClassifier> m_classifier;


  /// Property: Should we fill origin of background electrons?
  bool m_doBkgElecOrigin;


  /// Variable: Particle type.
  int* m_type;


  /// Variable: Particle origin.
  int* m_origin;


  /// Variable: Type of photon for background electron from conversions.
  int* m_typebkg;


  /// Variable: Origin of photon for background electron from conversions.
  int* m_originbkg;
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_EGAMMATRUTHCLASSIFICATIONFILLERTOOL_H
