/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauD3PDMaker/src/TruthEgammaFillerTool.h
 * @author Almut Pingel <apingel@cern.ch>
 * @date December 2012
 * @brief Block filler for truth electrons - uses MCTruthClassifier
 */

#ifndef TAUD3PDMAKER_TRUTHEGAMMAFILLERTOOL_H
#define TAUD3PDMAKER_TRUTHEGAMMAFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODTruth/TruthParticle.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {

class TruthEgammaFillerTool : public BlockFillerTool<xAOD::TruthParticle> {
public:
  typedef BlockFillerTool<xAOD::TruthParticle> Base;

  TruthEgammaFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);

  StatusCode initialize();
  virtual StatusCode book();
  virtual StatusCode fill (const xAOD::TruthParticle& p);


  private:
  // Variables from each stored
  int* m_type;
  int* m_origin;
  int* m_outcome;
  int* m_motherPDG;
  int* m_motherBarcode;

  // other private
  ToolHandle<IMCTruthClassifier> m_classifier;
};


} // namespace D3PD


#endif // TAUD3PDMAKER_TRUTHEGAMMAFILLERTOOL_H
