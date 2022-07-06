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

  std::bitset<32> triggerWord;
  std::bitset<32> timeWord;
  std::bitset<32> mask = 0xFFFFFFFF;

  auto energyMean_A = Scalar<float>("MBTS_A_meanEnergy", 0.);
  auto energyMean_C = Scalar<float>("MBTS_C_meanEnergy", 0.);
  auto timeMean_A = Scalar<float>("MBTS_A_meanTime", 0.);
  auto timeMean_C = Scalar<float>("MBTS_C_meanTime", 0.);
  auto energyWeightedTime_A = Scalar<float>("MBTS_A_EWTime", 0.);
  auto energyWeightedTime_C = Scalar<float>("MBTS_C_EWTime", 0.);
  auto channelID = Scalar<int>("MBTS_channelID", 0);
  auto mbtsEnergy = Scalar<float>("MBTS_energy", 0.);
  auto mbtsTime = Scalar<float>("MBTS_time", 0.);
  // Parameters to be tuned to correspond to trigger threshold:
  //const double timeCut = 20.;
  //const double energyCut = 40. / 222.;
  const double timeCut = 10e-4;         // minimal cut-off : copy from TileMBTSMonitorAlg
  const double energyCut = 60. / 222.;      // copy from TileMBTSMonitorAlgorithm
  int ebaCounters = 0;
  int ebcCounters = 0;

  for (auto &trig : m_triggerList)
  {
    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      ATH_MSG_DEBUG("Chain " << trig << " is passed: YES");
      auto TrigCounts = Scalar<std::string>("TrigCounts", trig);

      fill("MBTSall", TrigCounts);

      auto sideA_hits = 0;
      auto sideC_hits = 0;

      for (const auto mbts_itr : *mbtsbits) // this loop has one iteration
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
          channelID = i;
          mbtsTime = mbtsHitTimes.at(i);
          mbtsEnergy = mbtsHitEnergies.at(i);

          if (mbtsEnergy > energyCut)
            triggerWord.set(i);
	  if (std::abs(mbtsTime) > timeCut)
            timeWord.set(i);
          if ( not (triggerWord[i] and timeWord[i]) ) continue;

          ATH_MSG_DEBUG( "MBTS module " << i << " time " <<  mbtsHitTimes.at(i) << " energies: " << mbtsHitEnergies.at(i));
	  fill(trig + "_shifter", channelID, mbtsTime, mbtsEnergy);
          if (i < 16)
          { // A side
            energyMean_A += mbtsHitEnergies.at(i);
            timeMean_A += mbtsHitTimes.at(i);
            energyWeightedTime_A += mbtsHitEnergies.at(i) * mbtsHitTimes.at(i);

            ebaCounters++;
            if (i == 15)
            {
              if (ebaCounters > 0)
              {
                energyMean_A /= ebaCounters;
                timeMean_A /= ebaCounters;
                if (energyMean_A > 0)   energyWeightedTime_A /= energyMean_A*ebaCounters;
                fill(trig + "_expert", energyMean_A, timeMean_A);
              } else {
                energyWeightedTime_A = -999; // out of range

              }
            }
          }
          else // C side
          {
            energyMean_C += mbtsHitEnergies.at(i);
            timeMean_C += mbtsHitTimes.at(i);
            energyWeightedTime_C = mbtsHitEnergies.at(i) * mbtsHitTimes.at(i);

            ebcCounters++;
            if (i == 31)
            {
              if (ebcCounters > 0)
              {
                energyMean_C /= ebcCounters;
                timeMean_C /= ebcCounters;
                if (energyMean_C > 0) energyWeightedTime_C /= energyMean_C*ebcCounters;
                fill(trig + "_expert", energyMean_C, timeMean_C);
              } else {
                energyWeightedTime_C = -999; // out of range
              }

            }
          }
        }
        timeWord &= triggerWord;

        std::bitset<8> InnerEbaBitset((triggerWord & timeWord & mask).to_ulong());
        std::bitset<8> OuterEbaBitset((((triggerWord & timeWord) >> 8) & mask).to_ulong());
        std::bitset<8> InnerEbcBitset((((triggerWord & timeWord) >> 16) & mask).to_ulong());
        std::bitset<8> OuterEbcBitset((((triggerWord & timeWord) >> 24) & mask).to_ulong());

        sideA_hits = InnerEbaBitset.count() + OuterEbaBitset.count();
        sideC_hits = InnerEbcBitset.count() + OuterEbcBitset.count();

        auto MBTS_A_hits = Scalar<int>("MBTS_A_hits", sideA_hits);
        auto MBTS_C_hits = Scalar<int>("MBTS_C_hits", sideC_hits);

        fill(trig + "_shifter", MBTS_A_hits, MBTS_C_hits, energyWeightedTime_A, energyWeightedTime_C);
      }
    }
  }
  return StatusCode::SUCCESS;
}
