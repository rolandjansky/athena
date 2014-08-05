/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthEtIsolationFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Block filler tool for truth isolation information.
 */

#include "TruthEtIsolationFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthEtIsolationFillerTool::TruthEtIsolationFillerTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
  : BlockFillerTool<McAod::EtIsolations> (type, name, parent),
    m_coneIndex (0)
{
  declareProperty ("ConeSize", m_coneSize = 0.2,
                   "Size of the cone to fill.");

  m_etcone = 0;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthEtIsolationFillerTool::book()
{
  // Find the proper cone index.
  for (m_coneIndex = 0;
       m_coneIndex < TruthParticleParameters::NbrOfCones;
       m_coneIndex++)
  {
    float sz = TruthParticleParameters::coneCut
      (static_cast<TruthParticleParameters::ConeSize> (m_coneIndex));
    if (std::abs(sz - m_coneSize) < 0.01)
      break;
  }
  if (m_coneIndex >= TruthParticleParameters::NbrOfCones) {
    REPORT_MESSAGE(MSG::ERROR) << "Can't find requested cone size "
                               << m_coneSize;
    return StatusCode::FAILURE;
  }

  int isize = static_cast<int> (m_coneSize*100 + 0.5);
  std::ostringstream sname;
  sname << "Etcone" << isize;
  CHECK( addVariable(sname.str(), m_etcone) );
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
StatusCode TruthEtIsolationFillerTool::fill (const McAod::EtIsolations& p)
{
  *m_etcone = p[m_coneIndex];
  return StatusCode::SUCCESS;
}


} // namespace D3PD
