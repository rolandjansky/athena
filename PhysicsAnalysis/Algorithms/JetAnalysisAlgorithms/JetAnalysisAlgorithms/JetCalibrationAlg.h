/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef JET_ANALYSIS_ALGORITHMS__JET_CALIBRATION_ALG_H
#define JET_ANALYSIS_ALGORITHMS__JET_CALIBRATION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <JetCalibTools/IJetCalibrationTool.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IJetCalibrationTool

  class JetCalibrationAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    JetCalibrationAlg (const std::string& name, 
                       ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the calibration tool
  private:
    ToolHandle<IJetCalibrationTool> m_calibrationTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the jet collection we run on
  private:
    SysCopyHandle<xAOD::JetContainer> m_jetHandle {
      this, "jets", "AntiKt4EMTopoJets", "the jet collection to run on"};
  };
}

#endif
