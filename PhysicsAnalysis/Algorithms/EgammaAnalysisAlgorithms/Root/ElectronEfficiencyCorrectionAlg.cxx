/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    declareProperty ("scaleFactorDecoration", m_scaleFactorDecoration, "the decoration for the electron scale factor");
  }



  StatusCode ElectronEfficiencyCorrectionAlg ::
  initialize ()
  {
    if (m_scaleFactorDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor decoration name set");
      return StatusCode::FAILURE;
    }
    m_scaleFactorAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_scaleFactorDecoration);

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
            double sf = 0;
            ANA_CHECK_CORRECTION (m_outOfValidity, *electron, m_efficiencyCorrectionTool->getEfficiencyScaleFactor (*electron, sf));
            (*m_scaleFactorAccessor) (*electron) = sf;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
