/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#ifndef JET_ANALYSIS_ALGORITHMS__JET_GHOST_MUON_ASSOCIATION_ALG_H
#define JET_ANALYSIS_ALGORITHMS__JET_GHOST_MUON_ASSOCIATION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODJet/JetContainer.h>

namespace CP
{
  /// \brief an algorithm for adding ghost muons to jets

  class JetGhostMuonAssociationAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    JetGhostMuonAssociationAlg (const std::string& name, 
                                ISvcLocator* pSvcLocator);

  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;


    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the jet collection we run on
  private:
    SysCopyHandle<xAOD::JetContainer> m_jetHandle {
      this, "jets", "AntiKt4EMTopoJets", "the jet collection to run on"};
  };
}

#endif // JET_ANALYSIS_ALGORITHMS__JET_GHOST_MUON_ASSOCIATION_ALG_H
