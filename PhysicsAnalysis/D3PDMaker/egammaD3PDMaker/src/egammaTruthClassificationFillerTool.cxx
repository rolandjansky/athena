/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaTruthClassificationFillerTool.cxx
 * @author scott snyder
 * @date Jan, 2010
 * @brief Fill in type/origin from MC classifier tool for an egamma.
 */


#include "egammaTruthClassificationFillerTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "AthenaKernel/errorcheck.h"
#include <cstdlib>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaTruthClassificationFillerTool::egammaTruthClassificationFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<xAOD::Egamma> (type, name, parent),
      m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");

  declareProperty ("DoBkgElecOrigin", m_doBkgElecOrigin = false,
                   "If true, fill in variables for the origin and type "
                   " of a photon for background electrons from conversions.");

  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode egammaTruthClassificationFillerTool::initialize()
{
  CHECK( BlockFillerTool<xAOD::Egamma>::initialize() );
  CHECK( m_classifier.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode egammaTruthClassificationFillerTool::book()
{
  CHECK( addVariable ("type",      m_type,   
                      "MC particle type, from classifier tool.") );
  CHECK( addVariable ("origin",    m_origin,
                      "MC particle origin, from classifier tool.") );

  if (m_doBkgElecOrigin) {
    CHECK( addVariable ("typebkg", m_typebkg,
                        "Type of photon for background electron "
                        "from conversions, from classifier tool") );
    CHECK( addVariable ("originbkg", m_originbkg,
                        "Origin of photon for background electron "
                        "from conversions, from classifier tool.") );
  }

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
StatusCode egammaTruthClassificationFillerTool::fill (const xAOD::Egamma& p)
{
  std::pair<MCTruthPartClassifier::ParticleType,
    MCTruthPartClassifier::ParticleOrigin> res;

  IMCTruthClassifier::Info info;
  if (const xAOD::Electron* q =
      dynamic_cast<const xAOD::Electron*>(&p))
  {
    res = m_classifier->particleTruthClassifier (q, &info);
  }
  else if (const xAOD::Photon* q =
           dynamic_cast<const xAOD::Photon*>(&p))
  {
    res = m_classifier->particleTruthClassifier (q, &info);
  }
  else
    std::abort();

  *m_type   = res.first;
  *m_origin = res.second;

  if (m_doBkgElecOrigin) {
    if (res.first == MCTruthPartClassifier::BkgElectron && 
        res.second == MCTruthPartClassifier::PhotonConv &&
        info.genPart)
    {
      res = m_classifier->checkOrigOfBkgElec(info.genPart, &info);
      *m_typebkg   = res.first;
      *m_originbkg = res.second;
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
