/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef TAU_ANALYSIS_ALGORITHMS__TAU_CALIBRATION_AND_SMEARING_ALG_H
#define TAU_ANALYSIS_ALGORITHMS__TAU_CALIBRATION_AND_SMEARING_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <TauAnalysisTools/ITauSmearingTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODTau/TauJetContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref ITauSmearingTool

  class TauSmearingAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    TauSmearingAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<TauAnalysisTools::ITauSmearingTool> m_smearingTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the tau collection we run on
  private:
    SysCopyHandle<xAOD::TauJetContainer> m_tauHandle {
      this, "taus", "TauJets", "the tau collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};
  };
}

#endif
