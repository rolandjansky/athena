/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetEMFractionFillerTool.cxx
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date September, 2009
 * @brief Block filler tool for Jet EM Fraction
 */


#include "JetEMFractionFillerTool.h"
#include "checkJet.h"
#include "JetEvent/Jet.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/errorcheck.h"

namespace {


double jetEMFraction(const Jet * jet)
{
  std::string base="energy_";
  unsigned int nsamp = CaloSampling::getNumberOfSamplings();
  double e_HAD = 0;
  double e_EM = 0;

  for (unsigned int i=0; i < nsamp; i++) {
    double e = jet->getShape(base + CaloSampling::getSamplingName(i));
    if (CaloSamplingHelper::isEMSampling ((CaloSampling::CaloSample)(i)))
      e_EM += e;
    else if (CaloSamplingHelper::isHADSampling ((CaloSampling::CaloSample)(i)))
      e_HAD += e;
  }
  
  if( (e_EM==0) || ((e_EM+e_HAD)==0) ) return 0.;
  return (e_EM / (e_EM+e_HAD));
}


}


namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetEMFractionFillerTool::JetEMFractionFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_checked (false),
      m_goodjets (false)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetEMFractionFillerTool::book()
{
  CHECK( addVariable ("emfrac",  m_emfrac)  );
 
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
StatusCode JetEMFractionFillerTool::fill(const Jet& p)
{
  if (!m_checked) {
    m_goodjets = checkJet (p);
    m_checked = true;
  }

  if (m_goodjets)
    *m_emfrac  = jetEMFraction(&p);
  return StatusCode::SUCCESS;
}


} // namespace D3PD
