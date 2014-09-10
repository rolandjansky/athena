/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackParticleMCClassifierFillerTool.cxx
 * @author scott snyder
 * @date Oct 2011
 * @brief Fill in MCTruthClassifier results for TrackParticle.
 */


#include "TrackParticleMCClassifierFillerTool.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODTracking/TrackParticle.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackParticleMCClassifierFillerTool::TrackParticleMCClassifierFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TrackParticleMCClassifierFillerTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_classifier.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TrackParticleMCClassifierFillerTool::book()
{
  CHECK( addVariable ("type",  m_type,
                      "MC particle type, from classifier tool.")  );
  CHECK( addVariable ("origin",  m_origin,
                      "MC particle origin, from classifier tool.")  );

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
TrackParticleMCClassifierFillerTool::fill (const xAOD::TrackParticle& p)
{
  std::pair<unsigned int, unsigned int> cl =
    m_classifier->particleTruthClassifier (&p);
  *m_type = cl.first;
  *m_origin = cl.second;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
