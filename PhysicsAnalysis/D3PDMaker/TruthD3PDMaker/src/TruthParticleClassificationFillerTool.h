// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleClassificationFillerTool.h
 * @author sss
 * @date Jul 2014
 * @brief Block filler for truth classification
 */

#ifndef TRUTHD3PDMAKER_TRUTHPARTICLECLASSIFICATIONFILLERTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLECLASSIFICATIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODTruth/TruthParticle.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {
  

class TruthParticleClassificationFillerTool
  : public BlockFillerTool<xAOD::TruthParticle>
{
public:
  typedef BlockFillerTool<xAOD::TruthParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleClassificationFillerTool (const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent);


  /// Standard Gaudi initialize method.
  StatusCode initialize() override;


  /// Book variables for this block.
  virtual StatusCode book() override;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::TruthParticle& p) override;


  private:
  /// Property: The classifier tool.
  ToolHandle<IMCTruthClassifier> m_classifier;

  /// Variable: type from classifier tool
  int* m_type;

  /// Variable: origin from classifier tool
  int* m_origin;
};


} // namespace D3PD


#endif // TRUTHD3PDMAKER_TRUTHPARTICLECLASSIFICATIONFILLERTOOL_H
