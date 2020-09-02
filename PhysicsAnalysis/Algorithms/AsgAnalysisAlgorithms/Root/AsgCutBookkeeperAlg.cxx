/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

//
// includes
//

#include <AsgAnalysisAlgorithms/AsgCutBookkeeperAlg.h>
#include <SystematicsHandles/Helpers.h>

#include <RootCoreUtils/StringUtil.h>
#include <TH1.h>

#include <xAODCutFlow/CutBookkeeper.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODMetaData/FileMetaData.h>
#include <xAODTruth/TruthMetaData.h>
#include <xAODTruth/TruthMetaDataContainer.h>

#include <regex>

//
// method implementations
//

namespace CP
{
  AsgCutBookkeeperAlg ::
  AsgCutBookkeeperAlg (const std::string &name,
                      ISvcLocator *pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_truthWeightTool ("PMGTools::PMGTruthWeightTool", this)
  {
    declareProperty ("runNumber", m_runNumber, "the run number we are processing");
    declareProperty ("histPattern", m_histPattern, "the pattern for histogram names");
    declareProperty ("truthWeightTool", m_truthWeightTool, "the truth weight tool");
    declareProperty ("enableSystematics", m_enableSystematics, "enable systematics");
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

    // Read the channel number from FileMetaData
    const xAOD::FileMetaData *fmd{};
    ANA_CHECK (inputMetaStore()->retrieve(fmd, "FileMetaData"));

    float _flt_channel_number{};
    if (fmd->value(xAOD::FileMetaData::mcProcID, _flt_channel_number))
    {
      m_mcChannelNumber = static_cast<uint32_t>(_flt_channel_number);
    }
    if (m_mcChannelNumber == 0)
    {
      ANA_MSG_WARNING ("MC channel number could not be read from the FileMetaData.");

      // Try also TruthMetaData
      const xAOD::TruthMetaDataContainer *tmd{};
      if (inputMetaStore()->contains<xAOD::TruthMetaDataContainer>("TruthMetaData"))
      {
        ATH_CHECK (inputMetaStore()->retrieve(tmd, "TruthMetaData"));
        ATH_MSG_DEBUG("Loaded xAOD::TruthMetaDataContainer");

        // If we only have one metadata item take MC channel from there if needed
        if (tmd->size() == 1) {
          m_mcChannelNumber = tmd->at(0)->mcChannelNumber();
          ATH_MSG_WARNING("... MC channel number taken from the metadata as " << m_mcChannelNumber);
        }
      }
    }

    // Prepare for systematics
    ANA_CHECK (m_truthWeightTool.retrieve());

    return StatusCode::SUCCESS;
  }



  StatusCode AsgCutBookkeeperAlg ::
  fileExecute ()
  {
    ANA_MSG_DEBUG ("Updating CutBookkeeper information");

    // Update MC channel number if needed
    if (m_mcChannelNumber == 0)
    {
      const xAOD::FileMetaData *fmd{};
      ANA_CHECK (inputMetaStore()->retrieve(fmd, "FileMetaData"));

      float _flt_channel_number{};
      if (fmd->value(xAOD::FileMetaData::mcProcID, _flt_channel_number)) {
        m_mcChannelNumber = static_cast<uint32_t>(_flt_channel_number);
      }
    }

    // Retrieve complete CutBookkeeperContainer
    const xAOD::CutBookkeeperContainer *completeCBC{};
    ANA_CHECK (inputMetaStore()->retrieve(completeCBC, "CutBookkeepers"));

    // Find the max cycle
    int maxCycle{-1};
    const xAOD::CutBookkeeper *allEvents{};
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

    size_t counter{};
    for (const xAOD::CutBookkeeper *cbk : *completeCBC)
    {
      if (cbk->cycle() == maxCycle && cbk->name().find("AllExecutedEvents") == 0 && cbk->inputStream() == "StreamAOD")
      {
        static const std::regex re ("AllExecutedEvents.*_([0-9]+)");
        // Get the CBK index
        size_t index{0};
        std::smatch match;
        if (std::regex_match(cbk->name(), match, re))
        {
          index = std::stoi(match[1]);
        }

        uint64_t nEventsProcessed  = cbk->nAcceptedEvents();
        double sumOfWeights        = cbk->sumOfEventWeights();
        double sumOfWeightsSquared = cbk->sumOfEventWeightsSquared();

        // Write CutBookkeeper information to the info
        ANA_MSG_VERBOSE ("CutBookkeeper information from the current file for index " << index << ":");
        ANA_MSG_VERBOSE ("Initial events                 = " << nEventsProcessed);
        ANA_MSG_VERBOSE ("Initial sum of weights         = " << sumOfWeights);
        ANA_MSG_VERBOSE ("Initial sum of weights squared = " << sumOfWeightsSquared);

        auto it = m_weights.emplace(index, WeightsGroup()).first;
        it->second.nEventsProcessed    += nEventsProcessed;
        it->second.sumOfWeights        += sumOfWeights;
        it->second.sumOfWeightsSquared += sumOfWeightsSquared;

        counter++;
      }
    }

    if (counter == 1 && m_enableSystematics) {
      ANA_MSG_WARNING ("This sample does not support CutBookkeeper systematics. Disabling...");
      m_enableSystematics = false;
    }

    return StatusCode::SUCCESS;
  }



  StatusCode AsgCutBookkeeperAlg ::
  finalize ()
  {
    // Temporarily handle systematics directly here
    std::vector<CP::SystematicSet> systematics;
    systematics.emplace_back();
    if (m_enableSystematics)
    {
      for (const CP::SystematicVariation &variation : m_truthWeightTool->affectingSystematics())
      {
        systematics.emplace_back(CP::SystematicSet({variation}));
      }
    }

    for (const CP::SystematicSet &sys : systematics)
    {
      std::string name = RCU::substitute(m_histPattern, "%DSID%", std::to_string(m_mcChannelNumber));
      name = RCU::substitute(name, "%RUN%", std::to_string(m_runNumber));
      name = makeSystematicsName (name, sys);

      ANA_CHECK (book(TH1F(name.c_str(), "CutBookkeeper Information", 3, 0.5, 3.5)));
      TH1 *h = hist(name);
      assert(h != nullptr);

      h->GetXaxis()->SetBinLabel (1, "Initial events");
      h->GetXaxis()->SetBinLabel (2, "Initial sum of weights");
      h->GetXaxis()->SetBinLabel (3, "Initial sum of weights squared");

      ANA_MSG_INFO ("CutBookkeeper information will be stored in " << name);

      ANA_CHECK (m_truthWeightTool->applySystematicVariation (sys));

      ANA_MSG_VERBOSE ("Running systematics " << sys.name() << " with index " << m_truthWeightTool->getSysWeightIndex());

      const WeightsGroup &weights = m_weights.at (m_truthWeightTool->getSysWeightIndex());
      h->SetBinContent (1, weights.nEventsProcessed);
      h->SetBinContent (2, weights.sumOfWeights);
      h->SetBinContent (3, weights.sumOfWeightsSquared);
    }

    return StatusCode::SUCCESS;
  }
} // namespace CP
