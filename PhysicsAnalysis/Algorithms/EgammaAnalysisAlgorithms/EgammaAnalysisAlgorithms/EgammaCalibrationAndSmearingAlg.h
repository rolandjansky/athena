/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EGAMMA_ANALYSIS_ALGORITHMS__EGAMMA_CALIBRATION_AND_SMEARING_ALG_H
#define EGAMMA_ANALYSIS_ALGORITHMS__EGAMMA_CALIBRATION_AND_SMEARING_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h>
#include <EgammaAnalysisAlgorithms/CopyHelpers.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref
  /// CP::IEgammaCalibrationAndSmearingTool

  class EgammaCalibrationAndSmearingAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    EgammaCalibrationAndSmearingAlg (const std::string& name, 
                                     ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_calibrationAndSmearingTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the egamma collection we run on
  private:
    SysCopyHandle<xAOD::EgammaContainer> m_egammaHandle {
      this, "egammas", "Electrons", "the egamma collection to run on"};

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
