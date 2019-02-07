/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef TAU_ANALYSIS_ALGORITHMS__DI_TAU_CALIBRATION_AND_SMEARING_ALG_H
#define TAU_ANALYSIS_ALGORITHMS__DI_TAU_CALIBRATION_AND_SMEARING_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <TauAnalysisTools/IDiTauSmearingTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODTau/DiTauJetContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IDiTauSmearingTool

  class DiTauSmearingAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    DiTauSmearingAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<TauAnalysisTools::IDiTauSmearingTool> m_smearingTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the tau collection we run on
  private:
    SysCopyHandle<xAOD::DiTauJetContainer> m_tauHandle {
      this, "taus", "DiTauJets", "the tau collection to run on"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};
  };
}

#endif
