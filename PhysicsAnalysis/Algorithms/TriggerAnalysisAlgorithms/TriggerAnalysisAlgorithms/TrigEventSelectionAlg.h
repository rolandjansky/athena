/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef TRIGGER_ANALYSIS_ALGORITHMS__TRIG_EVENT_SELECTION_ALG_H
#define TRIGGER_ANALYSIS_ALGORITHMS__TRIG_EVENT_SELECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/ToolHandle.h> 
#include <AthContainers/AuxElement.h>
#include <TrigDecisionInterface/ITrigDecisionTool.h>

namespace CP
{
  class TrigEventSelectionAlg : public EL::AnaAlgorithm
  {
  public:
    TrigEventSelectionAlg(const std::string &name,
                          ISvcLocator *svcLoc = nullptr);

    virtual StatusCode initialize() final;
    virtual StatusCode execute() final;
    virtual StatusCode finalize() final;

  private:
    /// \brief trigger decision tool handle
    ToolHandle<Trig::ITrigDecisionTool> m_trigDecisionTool;

    /// \brief list of triggers or trigger chains
    std::vector<std::string> m_trigList;
    
    /// \brief the decoration for trigger selection
    std::string m_selectionDecoration;

    /// \brief the accessors for \ref m_selectionDecoration and \ref m_trigList combination
    std::vector<SG::AuxElement::Decorator<bool>> m_selectionAccessors;

    /// \brief counter for passed events
    long long m_passed = 0;

    /// \brief counter for total events
    long long m_total = 0;

  };
}

#endif
