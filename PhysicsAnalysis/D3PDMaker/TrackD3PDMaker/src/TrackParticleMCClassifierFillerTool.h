// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackParticleMCClassifierFillerTool.h
 * @author scott snyder
 * @date Oct 2011
 * @brief Fill in MCTruthClassifier results for TrackParticle.
 */


#ifndef TRACKD3PDMAKER_TRACKPARTICLEMCCLASSIFIERFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEMCCLASSIFIERFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODTracking/TrackParticle.h"
#include "GaudiKernel/ToolHandle.h"


class IMCTruthClassifier;


namespace D3PD {


/**
 * @brief Fill in MCTruthClassifier results for TrackParticle.
 */
class TrackParticleMCClassifierFillerTool
  : public BlockFillerTool<xAOD::TrackParticle>
{
public:
  typedef BlockFillerTool<xAOD::TrackParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackParticleMCClassifierFillerTool (const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


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
  /// Property: Classifier tool.
  ToolHandle<IMCTruthClassifier> m_classifier;


  /// Variable: Particle type.
  int* m_type;

  /// Variable: Particle origin.
  int* m_origin;
};


} // namespace D3PD


#endif // TRACKD3PDMAKER_TRACKPARTICLEMCCLASSIFIERFILLERTOOL_H
