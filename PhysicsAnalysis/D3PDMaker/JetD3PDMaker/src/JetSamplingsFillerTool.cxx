/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetChargeFillerTool.cxx
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date September, 2009
 * @brief Block filler tool for Jet Energy Samplings
 */

#include "JetSamplingsFillerTool.h"
#include "checkJet.h"
#include "JetEvent/Jet.h"
//#include "JetUtils/JetCaloHelper.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloGeoHelpers/CaloSampling.h"

namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetSamplingsFillerTool::JetSamplingsFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_checked (false),
      m_goodjets (true)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetSamplingsFillerTool::book()
{
  for(int i=0; i<CaloSampling::Unknown; i++)
    CHECK( addVariable ("e_" + CaloSampling::getSamplingName(i), m_esampling[i]));
  
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
StatusCode JetSamplingsFillerTool::fill(const Jet& p)
{
  if (!m_checked) {
    m_goodjets = checkJet (p);
    m_checked = true;
  }

  if (m_goodjets) {
    for(int i=0; i<CaloSampling::Unknown; i++)
      *m_esampling[i]  = p.getShape ("energy_" + CaloSampling::getSamplingName(i));
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
