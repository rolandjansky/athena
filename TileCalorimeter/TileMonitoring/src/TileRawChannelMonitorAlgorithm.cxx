/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawChannelMonitorAlgorithm.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileInfo.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileRawChannelMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  // initialize superclass
  ATH_CHECK( TileCalibMonitorAlgorithm::initialize() );

  ATH_CHECK( m_digitsContainerKey.initialize() );
  ATH_CHECK( m_rawChannelContainerKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_dspRawChannelContainerKey.initialize(m_fillHistogramsForDSP) );
  ATH_CHECK( m_emScaleKey.initialize() );

  m_dac2Charge[0] = 100.* 2.0 * 4.096 / m_tileInfo->ADCmax(); // 100 pF * 2 for legacy or 200 pF for demonstrator
  m_dac2Charge[1] = 5.2 * 2.0 * 4.096 / m_tileInfo->ADCmax(); // use the same number 5.2 pF as in TileCisDefaultCalibTool

  if ((m_calibUnit >= 0) && (m_calibUnit <= 3)) {
    m_finalRawChannelUnit = static_cast<TileRawChannelUnit::UNIT>(m_calibUnit.value());
  }

  if (m_calibUnit == TileRawChannelUnit::Invalid) {
    m_finalRawChannelUnit = (m_runType == PHYS_RUN) ? TileRawChannelUnit::CesiumPicoCoulombs
                                                    : TileRawChannelUnit::PicoCoulombs;
  }

  ATH_MSG_INFO("Final Units: " << m_finalRawChannelUnit);

  using Tile = TileCalibUtils;
  using namespace Monitored;

  m_runTypeIsNotCIS = !((m_runType == CIS_RUN) || (m_runType == CIS_RAMP_RUN));

  if (m_runTypeIsNotCIS) {
    m_ampGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelAmp",
                                                 Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_timeGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTime",
                                                  Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_timeCorrGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTimeCorr",
                                                      Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  } else { // CIS run
    // For 5 pF capacitor
    m_ampOverQ5Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelAmpOverQ5",
                                                       Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_time5Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTime5",
                                                   Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    // For 100 pF capacitor
    m_ampOverQ100Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelAmpOverQ100",
                                                         Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_time100Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTime100",
                                                     Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    if (m_fill2DHistograms) {
      // For 5 pF capacitor
      m_ampVsQ5Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelAmpVsQ5",
                                                       Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

      m_timeVsTime5Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTimeVsTime5",
                                                           Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

      // For 100 pF capacitor
      m_ampVsQ100Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelAmpVsQ100",
                                                         Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

      m_timeVsTime100Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTimeVsTime100",
                                                             Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    }
  }


  if (m_fillHistogramsForDSP) {
    m_dspAmpGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelDspAmp",
                                                  Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_dspTimeGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelDspTime",
                                                     Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_dspChi2Groups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelDspChi2",
                                                     Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_dspChi2VsAmpGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelDspChi2VsAmp",
                                                          Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_ampDiffGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelAmpDiff",
                                                     Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_timeDiffGroups = buildToolMap<std::vector<int>>(m_tools, "TileRawChannelTimeDiff",
                                                      Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  }


  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using Tile = TileCalibUtils;

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  if (ctx.evt() % 1000 == 0) {
    ATH_MSG_ALWAYS(ctx.evt() << " events processed so far");
  }

  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey, ctx);
  ATH_CHECK( emScale.isValid() );

  SG::ReadHandle<TileDQstatus> dqStatusHandle(m_dqStatusKey, ctx);
  ATH_CHECK( dqStatusHandle.isValid() );
  const TileDQstatus* dqStatus = dqStatusHandle.get();

  // array of CIS parameters
  const uint32_t* cispar = dqStatus->cispar();

  if (cispar[8] != 0) return StatusCode::SUCCESS;

  int cap = (cispar[7] > 10) ? 0 : 1; // 100 pF or 5 pF
  double injectionTime = cispar[5] * 0.104;
  double chargeForNormalModule = (cispar[6] < 1024) ? cispar[6] * m_dac2Charge[cap] : 0.;
  // Effective value of small capacitor is twice smaller for demonstrator
  double chargeForDemonstrator = cap ? chargeForNormalModule * 0.5 : chargeForNormalModule;
  double invChargeForNormalModule = (chargeForNormalModule != 0) ? 1.0 / chargeForNormalModule : 0.;
  double invChargeForDemonstrator = (chargeForDemonstrator != 0) ? 1.0 / chargeForDemonstrator : 0.;

  // Ignore charges below 1 pC in HG
  // Ignore charges below 10 pC for small capacitor and below 15 pC for big capacitor in LG
  std::array<double, 2> minChargeForNormalModule = {(cap ? 10. : 15.), 1.};
  std::array<double, 2> minChargeForDemonstrator = {(cap ? 10. : 15.), 2.};

  // Ignore charges above 11.5 pC in HG (full range is 12.5 pC)
  // Ignore charges above 750. pC in LG (full range is 800. pC)
  std::array<double, 2> maxChargeForNormalModule = {750., 11.5};
  std::array<double, 2> maxChargeForDemonstrator = {750., 23.0};

  const std::vector<std::vector<int>>& ampOverQCapGroups = cap ? m_ampOverQ5Groups : m_ampOverQ100Groups;
  const std::vector<std::vector<int>>& ampVsQCapGroups = cap ? m_ampVsQ5Groups : m_ampVsQ100Groups;
  const std::vector<std::vector<int>>& timeCapGroups = cap ? m_time5Groups : m_time100Groups;
  const std::vector<std::vector<int>>& timeVsTimeCapGroups = cap ? m_timeVsTime5Groups : m_timeVsTime100Groups;

  bool corruptedData[Tile::MAX_ROS - 1][Tile::MAX_DRAWER][Tile::MAX_GAIN][MAX_DMU]={{{{false}}}};

  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
  ATH_CHECK( digitsContainer.isValid() );

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
    if (digitsCollection->empty() ) continue;

    HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    int partition = ros - 1;

    std::vector<uint32_t> headers = digitsCollection->getFragChipHeaderWords();
    int nDMU = headers.size();
    if (nDMU > MAX_DMU) nDMU = MAX_DMU;

    int fragId = digitsCollection->identify();
    if (!std::binary_search(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end(), fragId)) {
      for (int dmu = 0; dmu < nDMU; ++dmu) {
        bool isDataCorrupted = isDmuHeaderError(headers[dmu]);
        corruptedData[partition][drawer][0][dmu] = isDataCorrupted;
        corruptedData[partition][drawer][1][dmu] = isDataCorrupted;
      }
    }
  }

  int nChannels[Tile::MAX_ROS-1] = {0};
  float partitionTimeSum[Tile::MAX_ROS-1] = {0};

  float offlineTime[Tile::MAX_ROS-1][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN] = {{{{0}}}};
  float offlineAmplitude[Tile::MAX_ROS-1][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN] = {{{{0}}}};


  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey, ctx);
  ATH_CHECK( rawChannelContainer.isValid() );

  TileRawChannelUnit::UNIT rawChannelUnit = rawChannelContainer->get_unit();

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    if (rawChannelCollection->empty()) continue;

    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    int partition = ros - 1;

    int fragId = rawChannelCollection->identify();
    bool demonstrator = (std::binary_search(m_fragIDsDemonstrators.begin(), m_fragIDsDemonstrators.end(), fragId));

    std::array<double, 2>& minCharge = demonstrator ? minChargeForDemonstrator : minChargeForNormalModule;
    std::array<double, 2>& maxCharge = demonstrator ? maxChargeForDemonstrator : maxChargeForNormalModule;
    double charge = demonstrator ? chargeForDemonstrator : chargeForNormalModule;
    double invCharge = demonstrator ? invChargeForDemonstrator : invChargeForNormalModule;

    for (const TileRawChannel* rawChannel : *rawChannelCollection) {
      adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      int adc = m_tileHWID->adc(adc_id);

      if (corruptedData[partition][drawer][adc][channel / 3]) continue;

      std::string channelGainSuffix = "_" + std::to_string(channel) + "_" + std::to_string(adc);

      float amplitude = rawChannel->amplitude();
      if (rawChannelUnit != m_finalRawChannelUnit) {
        // Put everything in PicoCoulomb by default for all run types
        // For Laser and Physcs calibrate in CesiumPicoCoulomb for all channels, but the MBTS channels,
        // for which we keep the calibration in PicoCoulombCesium pC for consistency (no Cs calibration is possible)
        if ((m_finalRawChannelUnit == TileRawChannelUnit::CesiumPicoCoulombs) && (ros > 3)) {
          // if EB then check that they are not MBTS channel
          int index, pmt;
          rawChannel->cell_ID_index(index, pmt);
          if (index < -1) { // MBTS channel
            amplitude = emScale->calibrateChannel(drawerIdx, channel, adc, amplitude, rawChannelUnit, TileRawChannelUnit::PicoCoulombs);
          }
        }
        amplitude = emScale->calibrateChannel(drawerIdx, channel, adc, amplitude, rawChannelUnit, m_finalRawChannelUnit);
      } // no need to calibrate

      offlineAmplitude[partition][drawer][channel][adc] = amplitude;

      float time = rawChannel->time();
      offlineTime[partition][drawer][channel][adc] = time;

      if (m_runTypeIsNotCIS) {
        auto monAmplitude = Monitored::Scalar<float>("amp" + channelGainSuffix, amplitude);
        fill(m_tools[m_ampGroups[partition][drawer]], monAmplitude);

        if (time != 0) {
          // Don't fill the time when it is exactly 0, which is a conventional value to say that it is not
          // calculated when the difference between the max(samples) - min(samples) < threshold
          auto monTime = Monitored::Scalar<float>("time" + channelGainSuffix, time);
          fill(m_tools[m_timeGroups[partition][drawer]], monTime);
        }

        if (m_cabling->isDisconnected(ros, drawer, channel)
            || amplitude < m_minAmpForCorrectedTime
            // In EB exclude some channels (most likely single PMT) from calculating average time
            || ((ros > 2) && (channel < 6 || channel == 12 || channel == 13 || channel == 18 || channel == 19))) {
          continue;
        } else {
          partitionTimeSum[partition] += time;
          nChannels[partition] += 1;
        }

      } else { // It's CIS run

        if (m_fill2DHistograms) {
          auto monAmplitude = Monitored::Scalar<float>("amp" + channelGainSuffix, amplitude);
          auto monCharge = Monitored::Scalar<float>("charge" + channelGainSuffix, charge);
          fill(m_tools[ampVsQCapGroups[partition][drawer]], monCharge, monAmplitude);
        }

        if ((minCharge[adc] < charge) && (charge < maxCharge[adc])) {
          double ratio = amplitude * invCharge;
          auto monRatio = Monitored::Scalar<float>("amp_ratio" + channelGainSuffix, ratio);
          fill(m_tools[ampOverQCapGroups[partition][drawer]], monRatio);

          auto monTime = Monitored::Scalar<float>("time" + channelGainSuffix, time);
          fill(m_tools[timeCapGroups[partition][drawer]], monTime);

          if (m_fill2DHistograms) {
            auto monInjTime = Monitored::Scalar<float>("inj_time" + channelGainSuffix, injectionTime);
            fill(m_tools[timeVsTimeCapGroups[partition][drawer]], monInjTime, monTime);
          }
        }
      }
    }
  }

  if (m_runTypeIsNotCIS) {
    for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
      if (nChannels[partition] > 0) {
        float averagePartitionTime = partitionTimeSum[partition] / nChannels[partition];
        for (unsigned int drawer = 0; drawer < Tile::MAX_DRAWER; ++drawer) {
          for (unsigned int channel = 0; channel < Tile::MAX_CHAN; ++channel) {
            for (unsigned int gain = 0; gain < Tile::MAX_GAIN; ++gain) {
              float time = offlineTime[partition][drawer][channel][gain];
              if (time != 0) {
                std::string channelGainSuffix = "_" + std::to_string(channel) + "_" + std::to_string(gain);
                float timeCorrected = time - averagePartitionTime;
                auto monTimeCorr = Monitored::Scalar<float>("time_corr" + channelGainSuffix, timeCorrected);
                fill(m_tools[m_timeCorrGroups[partition][drawer]], monTimeCorr);
              }
            }
          }
        }
      }
    }
  }


  if (m_fillHistogramsForDSP) {

    SG::ReadHandle<TileRawChannelContainer> dspRawChannelContainer(m_dspRawChannelContainerKey, ctx);
    ATH_CHECK( dspRawChannelContainer.isValid() );

    TileRawChannelUnit::UNIT dspRawChannelUnit = dspRawChannelContainer->get_unit();

    for (const TileRawChannelCollection* rawChannelCollection : *dspRawChannelContainer) {
      if (rawChannelCollection->empty() ) continue;

      HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      int partition = ros - 1;

      for (const TileRawChannel* rawChannel : *rawChannelCollection) {
        adc_id = rawChannel->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);

        if (corruptedData[partition][drawer][adc][channel / 3]) continue;

        std::string channelGainSuffix = "_" + std::to_string(channel) + "_" + std::to_string(adc);

        float amplitude = rawChannel->amplitude();
        if (dspRawChannelUnit != m_finalRawChannelUnit) {
          // Put everything in PicoCoulomb by default for all run types
          // For Laser and Physcs calibrate in CesiumPicoCoulomb for all channels, but the MBTS channels,
          // for which we keep the calibration in PicoCoulombCesium pC for consistency (no Cs calibration is possible)
          if ((m_finalRawChannelUnit == TileRawChannelUnit::CesiumPicoCoulombs) && (ros > 3)) {
            // if EB then check that they are not MBTS channel
            int index, pmt;
            rawChannel->cell_ID_index(index, pmt);
            if (index < -1) { // MBTS channel
              amplitude = emScale->calibrateChannel(drawerIdx, channel, adc, amplitude, dspRawChannelUnit, TileRawChannelUnit::PicoCoulombs);
            }
          }
          amplitude = emScale->calibrateChannel(drawerIdx, channel, adc, amplitude, dspRawChannelUnit, m_finalRawChannelUnit);
        } // no need to calibrate

        auto monAmplitude = Monitored::Scalar<float>("dsp_amp" + channelGainSuffix, amplitude);
        fill(m_tools[m_dspAmpGroups[partition][drawer]], monAmplitude);

        float offline_amplitude = offlineAmplitude[partition][drawer][channel][adc];
        if (offline_amplitude > m_minOfflineAmp) {
          float amplitudeDiff = (amplitude - offline_amplitude) / offline_amplitude;
          auto monAmplitudeDiff = Monitored::Scalar<float>("dsp-fit_amp_diff" + channelGainSuffix, amplitudeDiff);
          fill(m_tools[m_ampDiffGroups[partition][drawer]], monAmplitudeDiff);
        }

        float time = rawChannel->time();
        auto monTime = Monitored::Scalar<float>("dsp_time" + channelGainSuffix, time);
        fill(m_tools[m_dspTimeGroups[partition][drawer]], monTime);

        float offline_time = offlineTime[partition][drawer][channel][adc];
        if (offline_time != 0.) {
          float timeDiff = time - offline_time;
          auto monTimeDiff = Monitored::Scalar<float>("dsp-fit_time_diff" + channelGainSuffix, timeDiff);
          fill(m_tools[m_timeDiffGroups[partition][drawer]], monTimeDiff);
        }

        float chi2 = rawChannel->quality();
        auto monChi2 = Monitored::Scalar<float>("dsp_chi2" + channelGainSuffix, chi2);
        fill(m_tools[m_dspChi2Groups[partition][drawer]], monChi2);

        fill(m_tools[m_dspChi2VsAmpGroups[partition][drawer]], monAmplitude, monChi2);
      }
    }
  }

  fill("TileRawChannelMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}

bool TileRawChannelMonitorAlgorithm::isDmuHeaderError(uint32_t header) const {

  bool error = false;
  if (isHeaderFormatError(header)) {
    error = true;
  } else if (isHeaderParityError(header)) {
    error = true;
  } else if ((header >> 25) & 0x1) {
    // Memory Parity Error
    error = true;
  } else if ((header >> 24) & 0x1) {
    // Single Strobe Error
    error = true;
  } else if ((header >> 23) & 0x1) {
    // Double Strobe Error
    error = true;
  }

  return error;
}
