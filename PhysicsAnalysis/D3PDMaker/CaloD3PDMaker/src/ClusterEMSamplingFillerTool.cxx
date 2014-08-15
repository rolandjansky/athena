/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/ClusterEMSamplingFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */


#include "ClusterEMSamplingFillerTool.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ClusterEMSamplingFillerTool::ClusterEMSamplingFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("WriteEnergy", m_writeEnergy = true,
                   "Should energies be written?");
  declareProperty ("WriteEta", m_writeEta = true,
                   "Should eta values be written?");
  declareProperty ("WritePhi", m_writePhi = true,
                   "Should phi values be written?");
  declareProperty ("Samplings", m_samplings,
                   "List of samplings to write (in the range 0-3).  "
                   "An empty list means to write everything.");

  std::fill (m_Es,   m_Es + NEMSAMP,   (float*)0);
  std::fill (m_etas, m_etas + NEMSAMP, (float*)0);
  std::fill (m_phis, m_phis + NEMSAMP, (float*)0);
}


/**
 * @brief Book variables for this block.
 */
StatusCode ClusterEMSamplingFillerTool::book()
{
  if (m_samplings.empty()) {
    m_samplings.resize (NEMSAMP);
    for (unsigned int i = 0; i < NEMSAMP; i++)
      m_samplings[i] = i;
  }

  for (unsigned int i = 0; i < m_samplings.size(); i++) {
    unsigned int samp = m_samplings[i];
    if (samp >= NEMSAMP) {
      REPORT_MESSAGE(MSG::ERROR) << "Bad sampling index " << samp;
      return StatusCode::FAILURE;
    }

    if (m_writeEnergy) {
      std::ostringstream ename;
      ename << "Es" << samp;
      CHECK( addVariable (ename.str(),   m_Es[samp]) );
    }

    if (m_writeEta) {
      std::ostringstream etaname;
      etaname << "etas" << samp;
      CHECK( addVariable (etaname.str(), m_etas[samp]) );
    }

    if (m_writePhi) {
      std::ostringstream phiname;
      phiname << "phis" << samp;
      CHECK( addVariable (phiname.str(), m_phis[samp]) );
    }
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
StatusCode ClusterEMSamplingFillerTool::fill (const CaloCluster& p)
{
  CHECK( fillT (p) );
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
StatusCode ClusterEMSamplingFillerTool::fill (const xAOD::CaloCluster& p)
{
  CHECK( fillT (p) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block.
 * @param p The input object.
 *
 * This handles either cluster type.
 */
template <class T>
StatusCode ClusterEMSamplingFillerTool::fillT (const T& p)
{
  for (unsigned int i = 0; i < m_samplings.size(); i++) {
    unsigned int samp = m_samplings[i];
    if (m_writeEnergy)
      *m_Es[samp]   = static_cast<float> (p.energyBE (samp));
    if (m_writeEta) {
      *m_etas[samp] = static_cast<float> (p.etaBE (samp));
      if (*m_etas[samp] == -999) *m_etas[samp] = 0; // temp compat
    }
    if (m_writePhi) {
      *m_phis[samp] = static_cast<float> (p.phiBE (samp));
      if (*m_phis[samp] == -999) *m_phis[samp] = 0; // temp compat
    }
  }
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
