/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

//
// includes
//

#include <AsgAnalysisAlgorithms/AsgCutBookkeeperAlg.h>

#include <RootCoreUtils/StringUtil.h>
#include <TH1.h>

#include <xAODCutFlow/CutBookkeeper.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODMetaData/FileMetaData.h>

//
// method implementations
//

namespace CP
{
  AsgCutBookkeeperAlg ::
  AsgCutBookkeeperAlg (const std::string &name,
                      ISvcLocator *pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("runNumber", m_runNumber, "the run number we are processing");
    declareProperty ("histPattern", m_histPattern, "the pattern for histogram names");
  }



  StatusCode AsgCutBookkeeperAlg ::
  initialize ()
  {
    if (m_runNumber == 0)
    {
      ANA_MSG_ERROR ("Run number should be set");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (requestFileExecute ());

    uint32_t mcChannelNumber{};
    const xAOD::FileMetaData *fmd{};
    ANA_CHECK (inputMetaStore()->retrieve(fmd, "FileMetaData"));

    float _flt_channel_number{};
    if (fmd->value(xAOD::FileMetaData::mcProcID, _flt_channel_number)) {
      mcChannelNumber = static_cast<uint32_t>(_flt_channel_number);
    }
    if (mcChannelNumber == 0) {
      ANA_MSG_WARNING ("MC channel number could not be read from the FileMetaData.");
    }

    std::string name = RCU::substitute(m_histPattern, "%DSID%", std::to_string(mcChannelNumber));
    name = RCU::substitute(name, "%RUN%", std::to_string(m_runNumber));

    ANA_CHECK (book(TH1F(name.c_str(), "CutBookkeeper Information", 3, 0.5, 3.5)));
    m_hist = hist(name);
    assert(m_hist != nullptr);

    m_hist->GetXaxis()->SetBinLabel(1, "Initial events");
    m_hist->GetXaxis()->SetBinLabel(2, "Initial sum of weights");
    m_hist->GetXaxis()->SetBinLabel(3, "Initial sum of weights squared");

    ANA_MSG_INFO ("CutBookkeeper information will be stored in " << name);

    return StatusCode::SUCCESS;
  }



  StatusCode AsgCutBookkeeperAlg ::
  fileExecute ()
  {
    ANA_MSG_DEBUG ("Updating CutBookkeeper information");

    // Retrieve complete CutBookkeeperContainer
    const xAOD::CutBookkeeperContainer *completeCBC{};
    ANA_CHECK (inputMetaStore()->retrieve(completeCBC, "CutBookkeepers"));

    // Find the ones we are interested in
    const xAOD::CutBookkeeper *allEvents{};
    int maxCycle{-1};
    for (const xAOD::CutBookkeeper *cbk : *completeCBC)
    {
      ANA_MSG_DEBUG ("Complete cbk name: " << cbk->name() << " - stream: " << cbk->inputStream());
  
      if (cbk->cycle() > maxCycle && cbk->name() == "AllExecutedEvents" && cbk->inputStream() == "StreamAOD")
      {
        allEvents = cbk;
        maxCycle = cbk->cycle();
      }
    }

    if (allEvents == nullptr)
    {
      ANA_MSG_ERROR ("Could not find AllExecutedEvents CutBookkeeper information.");
      return StatusCode::FAILURE;
    }

    uint64_t nEventsProcessed  = allEvents->nAcceptedEvents();
    double sumOfWeights        = allEvents->sumOfEventWeights();
    double sumOfWeightsSquared = allEvents->sumOfEventWeightsSquared();

    // Write CutBookkeeper information to the histogram
    ANA_MSG_INFO ("CutBookkeeper information from this file:");
    ANA_MSG_INFO ("Initial events                 = " << nEventsProcessed);
    ANA_MSG_INFO ("Initial sum of weights         = " << sumOfWeights);
    ANA_MSG_INFO ("Initial sum of weights squared = " << sumOfWeightsSquared);

    m_hist->Fill(1, nEventsProcessed);
    m_hist->Fill(2, sumOfWeights);
    m_hist->Fill(3, sumOfWeightsSquared);

    return StatusCode::SUCCESS;
  }
} // namespace CP
