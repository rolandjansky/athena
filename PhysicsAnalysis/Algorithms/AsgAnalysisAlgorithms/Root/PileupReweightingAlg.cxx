/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//
#include "AsgAnalysisAlgorithms/PileupReweightingAlg.h"

/// Anonymous namespace for helpers
namespace {
  const static SG::AuxElement::Decorator<unsigned int> decRRN("RandomRunNumber");
  const static SG::AuxElement::Decorator<unsigned int> decRLBN("RandomLumiBlockNumber");
  const static SG::AuxElement::Decorator<unsigned long long> decHash("PRWHash");
}

//
// method implementations
//

namespace CP
{
  PileupReweightingAlg ::
  PileupReweightingAlg (const std::string& name, 
                        ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_pileupReweightingTool ("CP::PileupReweightingTool", this)
  {
    declareProperty ("pileupReweightingTool", m_pileupReweightingTool, "the pileup reweighting tool we apply");
    declareProperty ("baseEventInfo", m_baseEventInfoName,
      "The name of the original event info. The non-systematic dependent decorations will be applied to this "
      "object so it should be at least a base of the shallow copies read in by the 'eventInfo' handle. "
      "The default (and strongly recommended behaviour) is to leave all of these pointed at the central 'EventInfo' object!"
    );
    declareProperty ("correctedScaledAverageMuDecoration", m_correctedScaledAverageMuDecoration, "the decoration for the corrected and scaled average interactions per crossing");
    declareProperty ("correctedActualMuDecoration", m_correctedActualMuDecoration, "the decoration for the corrected actual interactions per crossing");
    declareProperty ("correctedScaledActualMuDecoration", m_correctedScaledActualMuDecoration, "the decoration for the corrected and scaled actual interactions per crossing");
  }



  StatusCode PileupReweightingAlg ::
  initialize ()
  {
    if (!m_correctedScaledAverageMuDecoration.empty())
    {
      m_correctedScaledAverageMuDecorator = std::make_unique<SG::AuxElement::Decorator<float> > (m_correctedScaledAverageMuDecoration);
    }
    if (!m_correctedActualMuDecoration.empty())
    {
      m_correctedActualMuDecorator = std::make_unique<SG::AuxElement::Decorator<float> > (m_correctedActualMuDecoration);
    }
    if (!m_correctedScaledActualMuDecoration.empty())
    {
      m_correctedScaledActualMuDecorator = std::make_unique<SG::AuxElement::Decorator<float> > (m_correctedScaledActualMuDecoration);
    }

    ANA_CHECK (m_eventInfoHandle.initialize(m_systematicsList));
    ANA_CHECK (m_weightDecorator.initialize(m_systematicsList, m_eventInfoHandle, SG::AllowEmpty));
    ANA_CHECK (m_pileupReweightingTool.retrieve());
    ANA_CHECK (m_systematicsList.addSystematics (*m_pileupReweightingTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode PileupReweightingAlg ::
  execute ()
  {

    const xAOD::EventInfo* evtInfo = nullptr;
    ANA_CHECK(evtStore()->retrieve(evtInfo, m_baseEventInfoName));

    // Add additional decorations - these apply to data (and on MC just redecorate the same value as
    // before)
    if (m_correctedScaledAverageMuDecorator)
    {
      (*m_correctedScaledAverageMuDecorator) (*evtInfo)
        = m_pileupReweightingTool->getCorrectedAverageInteractionsPerCrossing (*evtInfo, true);
    }

    if (m_correctedActualMuDecorator)
    {
      (*m_correctedActualMuDecorator) (*evtInfo)
        = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing (*evtInfo);
    }

    if (m_correctedScaledActualMuDecorator)
    {
      (*m_correctedScaledActualMuDecorator) (*evtInfo)
        = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing (*evtInfo, true);
    }

    if (!evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION))
      // The rest of the PRW tool only applies to MC
      return StatusCode::SUCCESS;

    // Deal with the parts that aren't related to systematics
    // Get random run and lumi block numbers
    unsigned int rrn = m_pileupReweightingTool->getRandomRunNumber(*evtInfo, true);
    // If it returns 0, try again without the mu dependence
    if (rrn == 0)
      rrn = m_pileupReweightingTool->getRandomRunNumber(*evtInfo, false);
    decRRN(*evtInfo) = rrn;
    decRLBN(*evtInfo) = (rrn == 0) ? 0 : m_pileupReweightingTool->GetRandomLumiBlockNumber(rrn);
    // Also decorate with the hash, this can be used for rerunning PRW (but usually isn't)
    decHash(*evtInfo) = m_pileupReweightingTool->getPRWHash(*evtInfo);

    // Take care of the weight (which is the only thing depending on systematics)
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::EventInfo* systEvtInfo = nullptr;
      ANA_CHECK( m_eventInfoHandle.retrieve(systEvtInfo, sys));
      ANA_CHECK (m_pileupReweightingTool->applySystematicVariation (sys));
      if (m_weightDecorator)
        // calculate and set the weight. The 'true' argument makes the tool treat unrepresented data
        // correctly if the corresponding property is set
        m_weightDecorator.set(*systEvtInfo, m_pileupReweightingTool->getCombinedWeight(*evtInfo, true), sys);

    };
    return StatusCode::SUCCESS;
  }
}
