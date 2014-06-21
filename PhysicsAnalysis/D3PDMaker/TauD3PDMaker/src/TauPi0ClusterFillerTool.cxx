/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauSumPi0FillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date Sept, 2009
 * @brief Block filler tool for sim of pi-zero clusters
 */


#include "TauPi0ClusterFillerTool.h"
#include "tauEvent/TauJet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauPi0ClusterFillerTool::TauPi0ClusterFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<Analysis::TauJet> (type, name, parent),
    m_pt(0),m_eta(0),m_phi(0)
{
}


/**
 * @brief Book variables for this block.
 */
StatusCode TauPi0ClusterFillerTool::book()
{
    CHECK( addVariable ("Pi0Cluster_pt",  m_pt)  );
    CHECK( addVariable ("Pi0Cluster_eta", m_eta)  );
    CHECK( addVariable ("Pi0Cluster_phi", m_phi)  );

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
StatusCode TauPi0ClusterFillerTool::fill (const Analysis::TauJet& p)
{
  // get the common details
  const Analysis::TauCommonDetails*  p_taudetails(0);
  p_taudetails = p.details<const Analysis::TauCommonDetails>();
  if (p_taudetails==0){
    REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
    return StatusCode::SUCCESS;
  }  
  for(int i=0; i < (int)p_taudetails->nPi0(); i++)
    {
      m_pt->push_back(p_taudetails->pi0(i)->pt());
      m_eta->push_back(p_taudetails->pi0(i)->eta());
      m_phi->push_back(p_taudetails->pi0(i)->phi());
    }
  return StatusCode::SUCCESS;
}
} // namespace D3PD
