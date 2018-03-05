/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <JetAnalysisAlgorithms/JvtEfficiencyAlg.h>

#include <SystematicsHandles/SelectionHelpers.h>

//
// method implementations
//

namespace CP
{
  JvtEfficiencyAlg ::
  JvtEfficiencyAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyTool ("", this)
  {
    declareProperty ("efficiencyTool", m_efficiencyTool, "the efficiency tool we apply");
    declareProperty ("selection", m_selection, "the decoration for the JVT selection");
    declareProperty ("efficiency", m_efficiency, "the decoration for the JVT efficiency");
    declareProperty ("skipBadEfficiency", m_skipBadEfficiency, "whether to skip efficiency calculation if the selection failed");
  }



  StatusCode JvtEfficiencyAlg ::
  initialize ()
  {
    ANA_CHECK (m_efficiencyTool.retrieve());
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_efficiencyTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());

    if (!m_selection.empty())
      m_selectionAccessor = std::make_unique<SG::AuxElement::Accessor<SelectionType> > (m_selection);

    if (!m_efficiency.empty())
      m_efficiencyAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_efficiency);

    return StatusCode::SUCCESS;
  }



  StatusCode JvtEfficiencyAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_efficiencyTool->applySystematicVariation (sys));
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        for (xAOD::Jet *jet : *jets)
        {
          bool goodJet = true;
          if (m_selectionAccessor || m_skipBadEfficiency)
          {
            goodJet = m_efficiencyTool->passesJvtCut (*jet);
            if (m_selectionAccessor)
              (*m_selectionAccessor) (*jet) = selectionFromBool (goodJet);
          }
          if (m_efficiencyAccessor && (goodJet || !m_skipBadEfficiency))
          {
            float efficiency = 1;
            ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_efficiencyTool->getEfficiencyScaleFactor (*jet, efficiency));
            (*m_efficiencyAccessor) (*jet) = efficiency;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
