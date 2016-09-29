/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETJetsInfoFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Jets cleaning variable objects.
 */


#include "MissingETD3PDMaker/MissingETJetsInfoFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MissingETJetsInfoFillerTool::MissingETJetsInfoFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : BlockFillerTool<JetCollection> (type, name, parent)
{
declareProperty ("TrackParticleContainer",       m_trackContainerKey        = "TrackParticleCandidate");
book().ignore(); // Avoid coverity warning.
}


/**
 * @brief Book variables for this block.
 */
StatusCode MissingETJetsInfoFillerTool::book()
{
  CHECK( addVariable ("JetPtWeightedEventEMFraction", m_ev_emf) );
  CHECK( addVariable ("JetPtWeightedNumAssociatedTracks", m_ev_numtrks) );
  CHECK( addVariable ("JetPtWeightedSize", m_ev_jetsize) );
  CHECK( addVariable ("LeadingJetEt", m_leadingJetEt) );
  CHECK( addVariable ("LeadingJetEta", m_leadingJetEta) );
  
  return StatusCode::SUCCESS;
}


StatusCode MissingETJetsInfoFillerTool::fill (const JetCollection& jets)
{ 
  *m_ev_emf = m_jetVariables->JetPtWeightedEventEMfraction(&jets);
  *m_ev_jetsize = m_jetVariables->JetPtWeightedSize(&jets);
  *m_leadingJetEt = m_jetVariables->leadingJetEt(&jets);
  *m_leadingJetEta = m_jetVariables->leadingJetEta(&jets);
  
  m_jetVariables->setJetCollection(&jets);
  ATH_CHECK( m_jetVariables->retrieveTrackContainer(m_trackContainerKey) );

  *m_ev_numtrks = m_jetVariables->JetPtWeightedNumAssociatedTracks();

  return StatusCode::SUCCESS;
}//end of fill tool


} // namespace D3PD
