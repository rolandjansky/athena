/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonTruthClassificationFillerTool.h
 * @author Massimiliano Bellomo <massimiliano.bellomo@cern.ch>
 * @date February 2012
 * @brief Block filler for truth classification of muons - uses MCTruthClassifier
 */

#ifndef MUONTRUTHCLASSIFICATIONFILLERTOOL_H
#define MUONTRUTHCLASSIFICATIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODMuon/Muon.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {
  
  class MuonTruthClassificationFillerTool : public BlockFillerTool<xAOD::Muon> {
  public:
    typedef BlockFillerTool<xAOD::Muon> Base;

  MuonTruthClassificationFillerTool (const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent);

  StatusCode initialize();
  virtual StatusCode book();
  virtual StatusCode fill (const xAOD::Muon& p);


  private:
  // Variables from each stored
  int* m_type;
  int* m_origin;
  
  // other private
  ToolHandle<IMCTruthClassifier> m_classifier;
};


} // namespace D3PD


#endif // MUONTRUTHCLASSIFICATIONFILLERTOOL_H
