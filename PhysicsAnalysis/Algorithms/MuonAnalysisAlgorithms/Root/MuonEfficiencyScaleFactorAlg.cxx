/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MuonAnalysisAlgorithms/MuonEfficiencyScaleFactorAlg.h>

#include <MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h>
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
    , m_efficiencyScaleFactorTool ("MuonEfficiencyScaleFactorTool", this)
  {
    declareProperty ("efficiencyScaleFactorTool", m_efficiencyScaleFactorTool, "the calibration and smearing tool we apply");
    declareProperty ("efficiencyDecoration", m_efficiencyDecoration, "the decoration for the muon efficiency");
    declareProperty ("isData", m_isData, "whether we run on data");
  }



  StatusCode MuonEfficiencyScaleFactorAlg ::
  initialize ()
  {
    if (m_efficiencyDecoration.empty())
    {
      ANA_MSG_ERROR ("no efficiency decoration name set");
      return StatusCode::FAILURE;
    }
    m_efficiencyAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_efficiencyDecoration);

    ANA_CHECK (m_efficiencyScaleFactorTool.retrieve());
    m_systematicsList.addHandle (m_muonHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_efficiencyScaleFactorTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode MuonEfficiencyScaleFactorAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_efficiencyScaleFactorTool->applySystematicVariation (sys));
        xAOD::MuonContainer *muons = nullptr;
        ANA_CHECK (m_muonHandle.getCopy (muons, sys));
        const xAOD::EventInfo *eventInfo = nullptr;
        ANA_CHECK (m_eventInfoHandle.retrieve (eventInfo, sys));
        for (xAOD::Muon *muon : *muons)
        {
          float eff = 0;
          if (m_isData)
          {
            ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getDataEfficiency (*muon, eff, eventInfo));
          } else
          {
            ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_efficiencyScaleFactorTool->getMCEfficiency (*muon, eff, eventInfo));
          }
          (*m_efficiencyAccessor) (*muon) = eff;
        }
        return StatusCode::SUCCESS;
      });
  }
}
