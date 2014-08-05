/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleClassificationFillerTool.cxx
 * @author sss
 * @date Jul 2014
 * @brief Block filler for truth classification
 */


#include "TruthParticleClassificationFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleClassificationFillerTool::TruthParticleClassificationFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
 : Base(type, name, parent),
   m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TruthParticleClassificationFillerTool::initialize()
{
  CHECK( m_classifier.retrieve() );
  return Base::initialize();
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthParticleClassificationFillerTool::book()
{
  CHECK( addVariable ("type",  m_type, "MC particle type, from classifier tool" )  );
  CHECK( addVariable ("origin",  m_origin, "MC particle origin, from classifier tool" )  );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode
TruthParticleClassificationFillerTool::fill (const xAOD::TruthParticle& p)
{
  std::pair<MCTruthPartClassifier::ParticleType,
    MCTruthPartClassifier::ParticleOrigin> res;

  res = m_classifier->particleTruthClassifier(&p);
   
  *m_type   = res.first;
  *m_origin = res.second;
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
