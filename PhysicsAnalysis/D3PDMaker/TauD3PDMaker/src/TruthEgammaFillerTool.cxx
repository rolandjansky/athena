/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TauD3PDMaker/src/TruthEgammaFillerTool.h
 * @author Almut Pingel <apingel@cern.ch>
 * @date December 2012
 * @brief Block filler for truth electrons - uses MCTruthClassifier
 */

#include "TruthEgammaFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODTruth/TruthParticle.h"

namespace D3PD {

TruthEgammaFillerTool::TruthEgammaFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : Base(type, name, parent),
    m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");

  book().ignore(); // Avoid coverity warnings.
}

StatusCode TruthEgammaFillerTool::initialize() {
  CHECK( m_classifier.retrieve() );
  return Base::initialize();
}


StatusCode TruthEgammaFillerTool::book() {
  CHECK( addVariable ("type",    m_type,   "MC particle type, from classifier tool."   )  );
  CHECK( addVariable ("origin",  m_origin, "MC particle origin, from classifier tool." )  );
  CHECK( addVariable ("outcome",  m_outcome, "MC particle outcome (final state), from classifier tool." )  );
  CHECK( addVariable ("motherPDGID",  m_motherPDG, "mother pdg id, from classifier tool." )  );
  CHECK( addVariable ("motherBarcode",  m_motherBarcode, "mother barcode, from classifier tool." )  );

  return StatusCode::SUCCESS;
}

StatusCode TruthEgammaFillerTool::fill (const xAOD::TruthParticle& p) {
  std::pair<unsigned int, unsigned int> Classify=m_classifier->particleTruthClassifier( &p );
  *m_type = Classify.first;
  *m_origin = Classify.second;

  *m_outcome = (int)( m_classifier->getParticleOutCome() );
  *m_motherPDG = m_classifier->getMotherPDG();
  *m_motherBarcode = m_classifier->getMotherBarcode();

  return StatusCode::SUCCESS;
}

} // namespace D3PD
