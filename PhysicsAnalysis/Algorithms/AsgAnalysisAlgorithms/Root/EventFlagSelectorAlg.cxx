/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include "AnaAlgorithm/FilterReporter.h"
#include <AsgAnalysisAlgorithms/EventFlagSelectionAlg.h>
#include <xAODEventInfo/EventInfo.h>

CP::EventFlagSelectionAlg::EventFlagSelectionAlg(const std::string &name,
                                             ISvcLocator *svcLoc)
  : EL::AnaAlgorithm(name, svcLoc)
{
  declareProperty ("selectionFlags", m_selFlags, "list of flags to use as selection criteria");
  declareProperty ("invertFlags",    m_invertFlags, "toggles for inverting the selection (index-parallel to selectionFlags)");
}

StatusCode CP::EventFlagSelectionAlg::initialize()
{
  if (m_invertFlags.size() != m_selFlags.size() && !m_invertFlags.empty()) {
    ATH_MSG_ERROR("Property invertFlags has different size to selectionFlags. Please check your configuration");
    return StatusCode::FAILURE;
  }

  for (size_t index = 0; index < m_selFlags.size(); ++index) {
    const std::string& thisflag = m_selFlags[index];
    if (thisflag.empty()) {
      ATH_MSG_ERROR("Empty string passed as selection flag!");
      return StatusCode::FAILURE;
    } else {
      // Extend m_invertFlags until the size matches m_selectionFlags
      // Only done in the case that m_invert was empty
      if (m_invertFlags.size() < index + 1) { m_invertFlags.push_back(false); }

      std::unique_ptr<ISelectionAccessor> accessor;
      ANA_CHECK (makeSelectionAccessor (m_selFlags[index], accessor));
      m_accessors.push_back(std::move(accessor));
    }
  }

  ANA_CHECK(m_filterParams.initialize());

  return StatusCode::SUCCESS;
}

StatusCode CP::EventFlagSelectionAlg::execute()
{
  EL::FilterReporter filter (m_filterParams, false);

  const xAOD::EventInfo *evtInfo = 0;
  ANA_CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));

  for (size_t index = 0; index < m_selFlags.size(); ++index) {
    // Test against the opposite of the invert value
    bool testval = !m_invertFlags[index];
    ATH_MSG_VERBOSE("Now testing flag \"" << m_selFlags[index] << "\" requiring value " << testval);

    if (m_accessors[index]->getBool(*evtInfo) != testval) {
      ATH_MSG_VERBOSE("Event failed.");
      filter.setPassed(false);
      return StatusCode::SUCCESS;
    }
  }

  ATH_MSG_VERBOSE("Event passed all flags.");
  filter.setPassed(true);

  return StatusCode::SUCCESS;
}

StatusCode CP::EventFlagSelectionAlg::finalize()
{
  ANA_CHECK (m_filterParams.finalize());

  return StatusCode::SUCCESS;
}
