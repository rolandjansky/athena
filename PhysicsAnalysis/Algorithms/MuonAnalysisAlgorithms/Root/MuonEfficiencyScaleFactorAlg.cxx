/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MuonAnalysisAlgorithms/MuonEfficiencyScaleFactorAlg.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace CP
{
  MuonEfficiencyScaleFactorAlg ::
  MuonEfficiencyScaleFactorAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyScaleFactorTool ("CP::MuonEfficiencyScaleFactors", this)
  {
    declareProperty ("efficiencyScaleFactorTool", m_efficiencyScaleFactorTool, "the calibration and smearing tool we apply");
  }



  StatusCode MuonEfficiencyScaleFactorAlg ::
  initialize ()
  {
    if (m_scaleFactorDecoration.empty() && m_mcEfficiencyDecoration.empty() && m_dataEfficiencyDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor or efficiency decoration name set");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_efficiencyScaleFactorTool.retrieve());
    m_systematicsList.addHandle (m_muonHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_efficiencyScaleFactorTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode MuonEfficiencyScaleFactorAlg ::
  execute ()
  {
    if (m_scaleFactorDecoration) {
      ANA_CHECK (m_scaleFactorDecoration.preExecute (m_systematicsList));
    }
    if (m_mcEfficiencyDecoration) {
      ANA_CHECK (m_mcEfficiencyDecoration.preExecute (m_systematicsList));
    }
    if (m_dataEfficiencyDecoration) {
      ANA_CHECK (m_dataEfficiencyDecoration.preExecute (m_systematicsList));
    }

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_efficiencyScaleFactorTool->applySystematicVariation (sys));
        xAOD::MuonContainer *muons = nullptr;
        ANA_CHECK (m_muonHandle.getCopy (muons, sys));
        const xAOD::EventInfo *eventInfo = nullptr;
        ANA_CHECK (m_eventInfoHandle.retrieve (eventInfo, sys));
        for (xAOD::Muon *muon : *muons)
        {
          if (m_preselection.getBool (*muon))
          {
            if (m_scaleFactorDecoration) {
              float sf = 0;
              ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getEfficiencyScaleFactor (*muon, sf, eventInfo));
              m_scaleFactorDecoration.set (*muon, sf, sys);
            }

            if (m_mcEfficiencyDecoration) {
              float eff = 0;
              ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getMCEfficiency (*muon, eff, eventInfo));
              m_mcEfficiencyDecoration.set (*muon, eff, sys);
            }

            if (m_dataEfficiencyDecoration) {
              float eff = 0;
              ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getDataEfficiency (*muon, eff, eventInfo));
              m_dataEfficiencyDecoration.set (*muon, eff, sys);
            }
          } else
          {
            if (m_scaleFactorDecoration) {
              m_scaleFactorDecoration.set (*muon, invalidScaleFactor(), sys);
            }

            if (m_mcEfficiencyDecoration) {
              m_mcEfficiencyDecoration.set (*muon, invalidEfficiency(), sys);
            }

            if (m_dataEfficiencyDecoration) {
              m_dataEfficiencyDecoration.set (*muon, invalidEfficiency(), sys);
            }
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
