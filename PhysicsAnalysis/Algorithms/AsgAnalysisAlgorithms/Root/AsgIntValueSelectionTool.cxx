/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/// @author Miha Muskinja

//
// includes
//

#include <AsgAnalysisAlgorithms/AsgIntValueSelectionTool.h>

//
// method implementations
//

namespace CP
{
    AsgIntValueSelectionTool ::
        AsgIntValueSelectionTool(const std::string &name)
        : AsgTool(name)
    {
        declareProperty("selectionFlag", m_selFlag, "the flag used as selection criteria");
        declareProperty("acceptedValues", m_acceptedValues, "list of accepted values of the selection flag");
    }

    StatusCode AsgIntValueSelectionTool ::
        initialize()
    {
        m_accept.addCut(m_selFlag, m_selFlag);
        m_selFlagAccessor = std::make_unique<SG::AuxElement::Accessor<int>>(m_selFlag);

        return StatusCode::SUCCESS;
    }

    const Root::TAccept &AsgIntValueSelectionTool ::
        getTAccept() const
    {
        return m_accept;
    }

    const Root::TAccept &AsgIntValueSelectionTool ::
        accept(const xAOD::IParticle *particle) const
    {
        m_accept.clear();
        bool accepted = false;
        int val = (*m_selFlagAccessor)(*particle);
        if (std::find(m_acceptedValues.begin(), m_acceptedValues.end(), val) != m_acceptedValues.end())
        {
            accepted = true;
        }
        m_accept.setCutResult(0, accepted);
        ATH_MSG_VERBOSE("Result: " << m_accept << " for value " << val);

        return m_accept;
    }
} // namespace CP
