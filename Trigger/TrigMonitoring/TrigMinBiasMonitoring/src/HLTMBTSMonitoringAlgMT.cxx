/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMBTSMonitoringAlgMT.h"

HLTMBTSMonitoringAlgMT::HLTMBTSMonitoringAlgMT(const std::string &name, ISvcLocator *pSvcLocator) : AthMonitorAlgorithm(name, pSvcLocator)
{
}

HLTMBTSMonitoringAlgMT::~HLTMBTSMonitoringAlgMT()
{
}

StatusCode HLTMBTSMonitoringAlgMT::initialize()
{
  using namespace Monitored;
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_TrigT2MbtsBitsContainerKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode HLTMBTSMonitoringAlgMT::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTMBTSMonitoringAlgMT::fillHistograms(const EventContext &context) const
{
  using namespace Monitored;

  const auto &trigDecTool = getTrigDecisionTool();

  //==============================================================================
  // MBTS BITS INFORMATION
  //==============================================================================

  auto mbtsbits = SG::makeHandle(m_TrigT2MbtsBitsContainerKey, context);
  ATH_MSG_DEBUG("MBTS monitoring, handle validity " << mbtsbits.isValid());
  if (!mbtsbits.isValid())
  {
    ATH_MSG_DEBUG("Could not retrieve mbtsbits");
    return StatusCode::SUCCESS;
  }
  if (mbtsbits->size() == 0)
  {
    return StatusCode::SUCCESS;
  }
  ATH_CHECK(mbtsbits->size() == 1);

  auto triggerWord = Scalar<unsigned int>("triggerWord", 0);
  auto timeWord = Scalar<unsigned int>("timeWord", 0);
  auto energyMean_A = Scalar<float>("MBTS_A_meanEnergy", 0.);
  auto energyMean_C = Scalar<float>("MBTS_C_meanEnergy", 0.);
  auto timeMean_A = Scalar<float>("MBTS_A_meanTime", 0.);
  auto timeMean_C = Scalar<float>("MBTS_C_meanTime", 0.);
  auto ebaCounters = Scalar<int>("energy_ebaCounters", 1);

  for (auto &trig : m_triggerList)
  {
    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      ATH_MSG_DEBUG("Chain " << trig << " is passed: YES");
      auto TrigCounts = Scalar<std::string>("TrigCounts", trig);

      fill("MBTSall", TrigCounts);

      for (const auto mbts_itr : *mbtsbits)
      {

        auto mbtsHitEnergies = mbts_itr->triggerEnergies(); // energy (in pC) of signal in a counter (relative to IP), vector for all counters
        auto mbtsHitTimes = mbts_itr->triggerTimes();       // time of signal in counter (relative to IP), vector for all counters

        ATH_MSG_DEBUG("MBTS hits times container size: " << mbtsHitTimes.size());

        if (mbtsHitTimes.size() != 32)
          ATH_MSG_WARNING("MBTS Cell Times are stored incorrectly. The array should have 32 elements (24 should be filled).");
        if (mbtsHitEnergies.size() != 32)
          ATH_MSG_WARNING("MBTS Cell Energies are stored incorrectly. The array should have 32 elements (24 should be filled).");

        for (unsigned i = 0; i < xAOD::TrigT2MbtsBits::NUM_MBTS; i++)
        {

          if (i < 16)
          { // A side
            energyMean_A += mbtsHitEnergies.at(i);
            timeMean_A += mbtsHitTimes.at(i);
            ebaCounters++;
            ATH_MSG_DEBUG("energy A side: " << mbtsHitEnergies.at(i));
            ATH_MSG_DEBUG("time A side: " << mbtsHitTimes.at(i));
            if (i == 15)
            {
              if (ebaCounters > 0)
              {
                energyMean_A /= ebaCounters;
                timeMean_A /= ebaCounters;
                ATH_MSG_DEBUG("MEAN energy A side: " << energyMean_A);
                ATH_MSG_DEBUG("MEAN time A side: " << timeMean_A);
                fill(trig + "_expert", energyMean_A, timeMean_A);
              }
            }
          }
          else // C side
          {
            energyMean_C += mbtsHitEnergies.at(i);
            timeMean_C += mbtsHitTimes.at(i);
            ebaCounters++;
            ATH_MSG_DEBUG("energy C side: " << mbtsHitEnergies.at(i));
            ATH_MSG_DEBUG("time C side: " << mbtsHitTimes.at(i));
            if (i == 31)
            {
              if (ebaCounters > 0)
              {
                energyMean_C /= ebaCounters;
                timeMean_C /= ebaCounters;
                ATH_MSG_DEBUG("MEAN energy C side: " << energyMean_C);
                ATH_MSG_DEBUG("MEAN time C side: " << timeMean_C);
                fill(trig + "_expert", energyMean_C, timeMean_C);
              }
            }
          }
        }

        auto sideA_hits = 0;
        auto sideC_hits = 0;
        auto MBTStime = Scalar<float>("MBTStime", 3.5);
        auto MBTSChannelID = Scalar<float>("MBTSChannelID", 1.0);
        auto MBTS_A_hits = Scalar<int>("MBTS_A_hits", sideA_hits);
        auto MBTS_C_hits = Scalar<int>("MBTS_C_hits", sideC_hits);
        fill(trig + "_shifter", MBTStime, MBTSChannelID, MBTS_A_hits, MBTS_C_hits);
      }
    }
  }
  return StatusCode::SUCCESS;
}
