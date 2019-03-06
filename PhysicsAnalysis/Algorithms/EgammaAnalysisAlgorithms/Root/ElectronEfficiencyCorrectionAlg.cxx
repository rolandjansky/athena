/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EgammaAnalysisAlgorithms/ElectronEfficiencyCorrectionAlg.h>

//
// method implementations
//

namespace CP
{
  ElectronEfficiencyCorrectionAlg ::
  ElectronEfficiencyCorrectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyCorrectionTool ("AsgElectronEfficiencyCorrectionTool", this)
  {
    declareProperty ("efficiencyCorrectionTool", m_efficiencyCorrectionTool, "the calibration and smearing tool we apply");
    declareProperty ("efficiencyDecoration", m_efficiencyDecoration, "the decoration for the electron efficiency");
  }



  StatusCode ElectronEfficiencyCorrectionAlg ::
  initialize ()
  {
    if (m_efficiencyDecoration.empty())
    {
      ANA_MSG_ERROR ("no efficiency decoration name set");
      return StatusCode::FAILURE;
    }
    m_efficiencyAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_efficiencyDecoration);

    ANA_CHECK (m_efficiencyCorrectionTool.retrieve());
    m_systematicsList.addHandle (m_electronHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_efficiencyCorrectionTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode ElectronEfficiencyCorrectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_efficiencyCorrectionTool->applySystematicVariation (sys));
        xAOD::ElectronContainer *electrons = nullptr;
        ANA_CHECK (m_electronHandle.getCopy (electrons, sys));
        for (xAOD::Electron *electron : *electrons)
        {
          if (m_preselection.getBool (*electron))
          {
            double eff = 0;
            ANA_CHECK_CORRECTION (m_outOfValidity, *electron, m_efficiencyCorrectionTool->getEfficiencyScaleFactor (*electron, eff));
            (*m_efficiencyAccessor) (*electron) = eff;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
