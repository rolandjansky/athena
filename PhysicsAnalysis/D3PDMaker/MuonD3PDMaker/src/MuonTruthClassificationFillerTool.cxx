/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonTruthClassificationFillerTool.h
 * @author Massimiliano Bellomo <massimiliano.bellomo@cern.ch>
 * @date February 2012
 * @brief Block filler for truth classification of muons - uses MCTruthClassifier
 */

#include "MuonTruthClassificationFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODMuon/Muon.h"

namespace D3PD {

MuonTruthClassificationFillerTool::MuonTruthClassificationFillerTool (const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
  : Base(type, name, parent),
    m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");

  book().ignore(); // Avoid coverity warnings.
}

StatusCode MuonTruthClassificationFillerTool::initialize() {
  CHECK( m_classifier.retrieve() );
  return Base::initialize();
}


StatusCode MuonTruthClassificationFillerTool::book() {
  CHECK( addVariable ("type",  m_type, "MC particle type, from classifier tool" )  );
  CHECK( addVariable ("origin",  m_origin, "MC particle origin, from classifier tool" )  );

  return StatusCode::SUCCESS;
}

StatusCode MuonTruthClassificationFillerTool::fill (const xAOD::Muon& p) {

  std::pair<MCTruthPartClassifier::ParticleType,
    MCTruthPartClassifier::ParticleOrigin> res;

  res = m_classifier->particleTruthClassifier(&p);
   
  *m_type   = res.first;
  *m_origin = res.second;
  
  return StatusCode::SUCCESS;
}

} // namespace D3PD
