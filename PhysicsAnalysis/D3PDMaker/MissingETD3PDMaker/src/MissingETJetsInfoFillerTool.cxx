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
  : BlockFillerTool<JetCollection> (type, name, parent),
    mLog(msgSvc(), name )
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
  StatusCode sc = StatusCode::SUCCESS;

  *m_ev_emf = _jetVariables->JetPtWeightedEventEMfraction(&jets);
  *m_ev_jetsize = _jetVariables->JetPtWeightedSize(&jets);
  *m_leadingJetEt = _jetVariables->leadingJetEt(&jets);
  *m_leadingJetEta = _jetVariables->leadingJetEta(&jets);
  
  _jetVariables->setJetCollection(&jets);
  sc = _jetVariables->retrieveTrackContainer(m_trackContainerKey);
  if ( !sc.isSuccess() ) {
    mLog << MSG::ERROR << "Can't retrieve track container." << endreq;
    return sc;
  }

  *m_ev_numtrks = _jetVariables->JetPtWeightedNumAssociatedTracks();

  
  mLog << MSG::DEBUG << "MissingETD3PDMaker::MissingETJetsInfoFillerTool has filled Jets Info" << endreq;
  
  return sc;
}//end of fill tool


} // namespace D3PD
