/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

//
// includes
//

#include <JetAnalysisAlgorithms/JetGhostMuonAssociationAlg.h>
#include <METUtilities/METHelpers.h>
#include <xAODMuon/MuonContainer.h>

//
// method implementations
//

namespace CP
{
  JetGhostMuonAssociationAlg ::
  JetGhostMuonAssociationAlg (const std::string& name, 
                              ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
  }


  StatusCode JetGhostMuonAssociationAlg ::
  initialize ()
  {
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }


  StatusCode JetGhostMuonAssociationAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
      xAOD::JetContainer *jets = nullptr;
      ANA_CHECK (m_jetHandle.getCopy (jets, sys));

      // associate the ghost muons to the jets (needed by MET muon-jet OR later)
      const xAOD::MuonContainer* muons = nullptr;
      ATH_CHECK( evtStore()->retrieve(muons, "Muons") );
      met::addGhostMuonsToJets(*muons, *jets);

      return StatusCode::SUCCESS;
    });
  }
}
