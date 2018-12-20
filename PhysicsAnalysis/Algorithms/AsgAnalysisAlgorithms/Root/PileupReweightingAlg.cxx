/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgAnalysisAlgorithms/PileupReweightingAlg.h>

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
    declareProperty ("correctedScaledAverageMuDecoration", m_correctedScaledAverageMuDecoration, "the decoration for the corrected and scaled average interactions per crossing");
    declareProperty ("correctedActualMuDecoration", m_correctedActualMuDecoration, "the decoration for the corrected actual interactions per crossing");
    declareProperty ("correctedScaledActualMuDecoration", m_correctedScaledActualMuDecoration, "the decoration for the corrected and scaled actual interactions per crossing");
  }



  StatusCode PileupReweightingAlg ::
  initialize ()
  {
    if (!m_correctedScaledAverageMuDecoration.empty())
    {
      m_correctedScaledAverageMuAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_correctedScaledAverageMuDecoration);
    }
    if (!m_correctedActualMuDecoration.empty())
    {
      m_correctedActualMuAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_correctedActualMuDecoration);
    }
    if (!m_correctedScaledActualMuDecoration.empty())
    {
      m_correctedScaledActualMuAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_correctedScaledActualMuDecoration);
    }

    ANA_CHECK (m_pileupReweightingTool.retrieve());
    m_systematicsList.addHandle (m_eventInfoHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_pileupReweightingTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode PileupReweightingAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_pileupReweightingTool->applySystematicVariation (sys));
        xAOD::EventInfo *eventInfo = nullptr;
        ANA_CHECK (m_eventInfoHandle.getCopy (eventInfo, sys));

        ATH_CHECK (m_pileupReweightingTool->apply (*eventInfo));

        // Add additional decorations
        if (m_correctedScaledAverageMuAccessor)
        {
          (*m_correctedScaledAverageMuAccessor) (*eventInfo)
            = m_pileupReweightingTool->getCorrectedAverageInteractionsPerCrossing (*eventInfo, true);
        }

        if (m_correctedActualMuAccessor)
        {
          (*m_correctedActualMuAccessor) (*eventInfo)
            = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing (*eventInfo);
        }

        if (m_correctedScaledActualMuAccessor)
        {
          (*m_correctedScaledActualMuAccessor) (*eventInfo)
            = m_pileupReweightingTool->getCorrectedActualInteractionsPerCrossing (*eventInfo, true);
        }

        //--- PRWHash to recalculate PU weights using analysis ntuples
        //--- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting#Using_PRWHash_to_change_pileup_w

        if (eventInfo->eventType (xAOD::EventInfo::IS_SIMULATION))
        {
          eventInfo->auxdata<ULong64_t>("PRWHash")
            = m_pileupReweightingTool->getPRWHash (*eventInfo);
        }

        // Must decorate the actual instance in the event store for
        // the electron tool to work
        evtStore()->retrieve<const xAOD::EventInfo>("EventInfo")
          ->auxdecor<unsigned int>("RandomRunNumber") =
          eventInfo->auxdecor<unsigned int> ("RandomRunNumber");
        return StatusCode::SUCCESS;
      });
  }
}
