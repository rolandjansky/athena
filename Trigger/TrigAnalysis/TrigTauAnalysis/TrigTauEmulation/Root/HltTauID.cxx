/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauEmulation/HltTauSelection.h"
// #include "TrigTauEmulation/HltTauID.h"
#include "TrigTauEmulation/Utils.h"

// Default constructor
HltTauID::HltTauID() :
  m_id_level("medium"),
  m_use_tauid(true)
{
  m_accept.addCut("TauID", "TauID");

}

const asg::AcceptInfo& HltTauID::getAcceptInfo() const
{
  return m_accept;
}


// Accept method -- Rely on the already computed decision for now
asg::AcceptData HltTauID::accept(const xAOD::TauJet * hltau) const

{
  asg::AcceptData acceptData (&m_accept);
  acceptData.setCutResult("TauID", false);

  if (not m_use_tauid) {
    // passthrough mode
    acceptData.setCutResult("TauID", true);
    return acceptData;
  }

  // Select only 1,2,3 core tracks candidate
  if (hltau->nTracks() < 1 or hltau->nTracks() > 3)
    return acceptData;


  bool pass = false;
  if (m_id_level == "loose")
    pass = hltau->isTau(xAOD::TauJetParameters::JetBDTSigLoose);
  else if (m_id_level == "medium")
    pass = hltau->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
  else if (m_id_level == "tight")
    pass = hltau->isTau(xAOD::TauJetParameters::JetBDTSigTight);
  
  if (not pass) 
    return acceptData;

  acceptData.setCutResult("TauID", true);
  return acceptData;
}


