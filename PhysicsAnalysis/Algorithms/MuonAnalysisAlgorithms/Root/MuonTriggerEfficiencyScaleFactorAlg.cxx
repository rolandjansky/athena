/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak



//
// includes
//

#include <MuonAnalysisAlgorithms/MuonTriggerEfficiencyScaleFactorAlg.h>

#include <AthContainers/ConstDataVector.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace CP
{
  MuonTriggerEfficiencyScaleFactorAlg ::
  MuonTriggerEfficiencyScaleFactorAlg (const std::string& name, ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyScaleFactorTool ("CP::MuonTriggerScaleFactors", this)
  {
    declareProperty ("efficiencyScaleFactorTool", m_efficiencyScaleFactorTool, "the trigger efficiency scale factor tool we apply");
    declareProperty ("trigger", m_trigger, "trigger or trigger leg to calculate efficiency for");
    declareProperty ("minRunNumber", m_minRunNumber = 0, "trigger or trigger leg to calculate efficiency for");
    declareProperty ("maxRunNumber", m_maxRunNumber = 999999, "trigger or trigger leg to calculate efficiency for");
  }



  StatusCode MuonTriggerEfficiencyScaleFactorAlg ::
  initialize ()
  {
    if (m_trigger.empty())
    {
      ANA_MSG_ERROR ("trigger name needs to be set");
      return StatusCode::FAILURE;
    }

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



  StatusCode MuonTriggerEfficiencyScaleFactorAlg ::
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

        unsigned int randomRunNumber = eventInfo->auxdecor<unsigned int>("RandomRunNumber");
        bool validEvent = m_minRunNumber <= randomRunNumber && m_maxRunNumber >= randomRunNumber;

        for (xAOD::Muon *muon : *muons)
        {
          if (validEvent && m_preselection.getBool (*muon))
          {
            if (m_scaleFactorDecoration) {
              double sf = 0;
              ConstDataVector<xAOD::MuonContainer> singleMuonContainer(SG::VIEW_ELEMENTS);
              singleMuonContainer.push_back(muon);
              ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getTriggerScaleFactor (*singleMuonContainer.asDataVector(), sf, m_trigger));
              m_scaleFactorDecoration.set (*muon, sf, sys);
            }

            if (m_mcEfficiencyDecoration) {
              double eff = 0;
              ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getTriggerEfficiency (*muon, eff, m_trigger, false));
              m_mcEfficiencyDecoration.set (*muon, eff, sys);
            }

            if (m_dataEfficiencyDecoration) {
              double eff = 0;
              ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getTriggerEfficiency (*muon, eff, m_trigger, true));
              m_dataEfficiencyDecoration.set (*muon, eff, sys);
            }
          } else {
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
