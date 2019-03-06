/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/SelectionData.h>

#include <AsgTools/StatusCode.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/SelectionChoice.h>
#include <QuickAna/SelectionCut.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void SelectionData ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  SelectionData ::
  SelectionData ()
    : m_state (State::INITIAL), m_particle (nullptr)
  {
    RCU_NEW_INVARIANT (this);
  }



  void SelectionData ::
  registerCut (SelectionStep step, const std::string& name,
               SelectionCut& cut)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (!isInitialized());

    CutInfo info;
    info.step = step;
    info.name = name;
    m_cuts.push_back (info);

    cut.configure (this, size()-1);
  }



  std::size_t SelectionData ::
  size () const
  {
    RCU_READ_INVARIANT (this);
    return m_cuts.size();
  }



  StatusCode SelectionData ::
  initialize (const InternalConfiguration& configuration,
              const std::string& workingPoint)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (!isInitialized());
    m_state = State::BROKEN;

    // the postfix for the overall selections
    std::string postfix;
    if (!workingPoint.empty())
      postfix = "_" + workingPoint;

    for (unsigned iter = 0; iter != EnumSize<SelectionStep>::dataSize; ++ iter)
    {
      SelectionStep step = SelectionStep (iter);

      m_selections[iter].accessor
        = configuration.selectionName (step) + postfix;
    }

    // the prefix for individual cuts
    std::string prefix
      = configuration.selectionName (SelectionStep::ANALYSIS) + "_";
    if (!workingPoint.empty())
      prefix += workingPoint + "_";

    m_firstStep = SelectionStep::ANALYSIS;
    for (auto& cut : m_cuts)
    {
      cut.accessor = prefix + cut.name;
      if (cut.step < m_firstStep)
        m_firstStep = cut.step;
    }

    m_state = State::RUNNING;
    RCU_PROVIDE (isInitialized());
    return StatusCode::SUCCESS;
  }



  bool SelectionData ::
  isInitialized () const
  {
    RCU_READ_INVARIANT (this);
    return m_state >= State::RUNNING;
  }



  StatusCode SelectionData ::
  prepare (SG::AuxElement *particle)
  {
    using namespace msgSelection;

    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (isInitialized());
    m_particle = particle;

    for (auto& cut : m_cuts)
      cut.selection = SelectionChoice::UNASSIGNED;

    for (auto& selection : m_selections)
    {
      if (!selection.accessor.isAvailable (*m_particle))
      {
        ANA_MSG_ERROR ("input decoration unavailable: " << (&selection-m_selections));
        return StatusCode::FAILURE;
      }
      selection.select = selection.accessor (*m_particle);
    }
    return StatusCode::SUCCESS;
  }



  bool SelectionData ::
  needSelectionTool ()
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (isInitialized());
    return m_selections[unsigned (m_firstStep)].select;
  }



  SelectionChoice SelectionData ::
  getCut (std::size_t cut)
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (isInitialized());
    RCU_REQUIRE (cut < size());
    return m_cuts[cut].selection;
  }



  void SelectionData ::
  setCut (std::size_t cut, SelectionChoice choice)
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE (isInitialized());
    RCU_REQUIRE (cut < size());
    m_cuts[cut].selection = choice;
  }



  void SelectionData ::
  apply ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (isInitialized());

    for (auto& cut : m_cuts)
    {
      cut.accessor (*m_particle) = (cut.selection == SelectionChoice::PASS);
      if (cut.selection == SelectionChoice::FAIL)
      {
        for (std::size_t iter = std::size_t (cut.step),
               end = EnumSize<SelectionStep>::dataSize;
             iter != end; ++ iter)
        {
          m_selections[iter].select = false;
        }
      }
    }

    for (auto& selection : m_selections)
    {
      selection.accessor (*m_particle) = selection.select;
    }
  }



  SelectionData::CutInfo ::
  CutInfo ()
    : accessor ("dummy")
  {}



  SelectionData::SelectionInfo ::
  SelectionInfo ()
    : accessor ("dummy"),
      select(false) // TODO is this the right default value to use??
  {}
}
