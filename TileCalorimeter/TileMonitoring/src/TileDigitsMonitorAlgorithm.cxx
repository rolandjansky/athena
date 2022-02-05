/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileDigitsMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileInfo.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileDigitsMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  // initialize superclass
  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  std::sort(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end());

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_digitsContainerKey.initialize() );
  ATH_CHECK( m_rawChannelContainerKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_dqStatusKey.initialize() );
  ATH_CHECK( m_tileCondToolNoiseSample.retrieve(EnableTool(m_fillPedestalDifference)) );

  ATH_CHECK( detStore()->retrieve(m_tileInfo, m_tileInfoName) );
  m_ADCmaxMinusEpsilon = m_tileInfo->ADCmax() - 0.01;
  if (m_tileInfo->ADCmax() == 4095) {
    m_is12bit = true;
  }

  using Tile = TileCalibUtils;
  using namespace Monitored;


  if (m_fillErrorsHistograms) {
    m_errorsGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileDigitsErrors",
                                                                 Tile::MAX_ROS - 1, Tile::MAX_DRAWER,
                                                                 Tile::MAX_GAIN);

    m_bcidGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileDigitsBCID",
                                                               Tile::MAX_ROS - 1, Tile::MAX_DRAWER,
                                                               Tile::MAX_GAIN);

    m_bcidErrGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileDigitsBCIDErrors",
                                                                  Tile::MAX_ROS - 1, Tile::MAX_DRAWER,
                                                                  Tile::MAX_GAIN);

    m_crcErrGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsCRCErrors",
                                                    Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

    m_crcGlobalGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsGlobalCRC",
                                                       Tile::MAX_ROS - 1, Tile::MAX_DRAWER);

  }

  if (m_fillCorrelationsHistograms) {
    m_meanSampleGroups = buildToolMap<int>(m_tools, "TileDigitsMeanSample", Tile::MAX_GAIN);
    m_meanSampleProdGroups = buildToolMap<int>(m_tools, "TileDigitsMeanSampleProd", Tile::MAX_GAIN);
  }

  if (m_fillPedestalHistograms) {
    m_pedGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsSample0",
                                                 Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  }

  if (m_fillHighFrequencyNoiseHistograms) {
    m_hfnGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsMeanRMS",
                                                 Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  }

  if (m_fillSamplesHistograms) {
    m_samplesGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsSamples",
                                                     Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  }

  if (m_fillProfileHistograms) {
    m_profGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsProfile",
                                                  Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  }

  if (m_fillEventModule32Histograms) {
    m_evtMod32Groups = buildToolMap<std::vector<int>>(m_tools, "TileDigitsEvtMod32",
                                                      Tile::MAX_ROS - 1, Tile::MAX_DRAWER);
  }

  return StatusCode::SUCCESS;
}


StatusCode TileDigitsMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using Tile = TileCalibUtils;

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  std::vector<float> meanSample[Tile::MAX_GAIN];
  std::vector<float> meanSampleChannel[Tile::MAX_GAIN];

  std::vector<float> meanSampleProd[Tile::MAX_GAIN];
  std::vector<float> meanSampleProdChannel1[Tile::MAX_GAIN];
  std::vector<float> meanSampleProdChannel2[Tile::MAX_GAIN];

  const TileDQstatus* dqStatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  // array of CIS parameters
  const uint32_t* cispar = dqStatus->cispar();

  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
  ATH_CHECK( digitsContainer.isValid() );

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
    if (digitsCollection->empty() ) continue;

    HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    int partition = ros - 1;

    const std::vector<uint32_t>& headerWordsLow = digitsCollection->getFragChipHeaderWords();
    const std::vector<uint32_t>& headerWordsHigh = digitsCollection->getFragChipHeaderWordsHigh();
    std::vector<std::reference_wrapper<const std::vector<uint32_t>>> headerWordsAll{headerWordsLow};
    if (headerWordsHigh.empty()) {
      // For mono gain run fill the plots with the same info for high gain
      headerWordsAll.push_back(headerWordsLow);
    } else {
      headerWordsAll.push_back(headerWordsHigh);
    }

    std::vector<std::vector<bool>> corruptedData(Tile::MAX_GAIN, std::vector<bool>(MAX_DMU));
    uint32_t fragStatus = digitsCollection->getFragStatus();
    if (checkCorruptedData(headerWordsAll, fragStatus, ros, drawer, corruptedData)) {
      continue; // Dummy fragment or no fragment => nothing to check
    }

    if (m_fillErrorsHistograms) {
      uint32_t rodBCID = digitsCollection->getRODBCID();
      checkBCID(headerWordsAll, rodBCID, ros, drawer);

      bool isCalibMode = (dqStatus->calibMode() == 1);
      if (isCalibMode) {
        // global and DMU CRC check
        int nDMU = headerWordsLow.size();
        uint32_t crc32 = digitsCollection->getFragCRC();
        uint32_t crcMask = digitsCollection->getFragDMUMask();
        checkCRC(isCalibMode, crc32, crcMask, nDMU, ros, drawer);
      }
    }


    // Below, the charge conversion for 12 bit is just an approximation;
    //   4095. can be changed later to gain precision if needed.
    double charge = 0;
    if (m_is12bit) { // if 12 bit ADCs
      charge = cispar[6] * cispar[7] * (2. * 4.096 / 4095.);
    } else {
      charge = cispar[6] * cispar[7] * (2. * 4.096 / 1023.);
    }

    const TileDigits* moduleDigits[Tile::MAX_CHAN][Tile::MAX_GAIN] = {{nullptr}};

    for (const TileDigits* tile_digits : *digitsCollection) {

      adc_id = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      int adc = m_tileHWID->adc(adc_id);

      if (corruptedData[adc][channel / 3]) {
        continue;
      }

      moduleDigits[channel][adc] = tile_digits;

      std::string channelGainSuffix = "_" + std::to_string(channel) + "_" + std::to_string(adc);

      const std::vector<float>& digits = tile_digits->samples();
      unsigned int nSamples = digits.size();

      if (nSamples > 1) {

        if (m_fillPedestalHistograms) {
          double pedestal = digits[0];

          if(m_fillPedestalDifference) {
            pedestal -= m_tileCondToolNoiseSample->getPed(drawerIdx, channel, adc, TileRawChannelUnit::ADCcounts, ctx);
          }

          auto monSample0  = Monitored::Scalar<float>("sample0" + channelGainSuffix, pedestal);
          fill(m_tools[m_pedGroups[partition][drawer]], monSample0);
        }


        if (m_fillHighFrequencyNoiseHistograms) {
          double sampleMean = 0.0;
          double sampleRMS = 0.0;

          for (double sample : digits) {
            sampleMean += sample;
            sampleRMS += sample * sample;
            if (adc == 1 && sample > m_ADCmaxMinusEpsilon) {// overflow in high gain, find charge is it
              auto monCharge  = Monitored::Scalar<float>("charge", charge);
              fill("TileDigitsOverflow", monCharge);
            }
          }

          sampleMean /= nSamples;
          sampleRMS = sampleRMS / nSamples - sampleMean * sampleMean;
          sampleRMS = (sampleRMS > 0.0) ? sqrt(sampleRMS * nSamples / (nSamples - 1)) : 0.0;

          if (m_fillCorrelationsHistograms) {
            meanSample[adc].push_back(sampleMean);
            meanSampleChannel[adc].push_back(channel);
          }

          auto monMeanRMS  = Monitored::Scalar<float>("meanRMS" + channelGainSuffix, sampleRMS);
          fill(m_tools[m_hfnGroups[partition][drawer]], monMeanRMS);
        }

        if ((m_runType != CIS_RUN)
            || (cispar[6] > 0. && (adc < 1 || charge < 12.))) {
          // LF If CIS run, Protection to avoid zero amplitudes due to 0 injected charge

          if (m_fillSamplesHistograms) {
            auto monSamples = Monitored::Collection("samples" + channelGainSuffix, digits);
            fill(m_tools[m_samplesGroups[partition][drawer]], monSamples);
          }

          if (m_fillProfileHistograms) {
            auto monSamples = Monitored::Collection("samples" + channelGainSuffix, digits);
            std::vector<int> sampleNumbers(nSamples);
            std::iota(sampleNumbers.begin(), sampleNumbers.end(), 0);
            auto monSampleNumbers = Monitored::Collection("sampleNumbers" + channelGainSuffix, sampleNumbers);
            fill(m_tools[m_profGroups[partition][drawer]], monSampleNumbers, monSamples);
          }
        }

        if (m_fillEventModule32Histograms) {
          int evtMod32 = (ctx.evt() + 1) % 32;
          auto monEvtMod32 = Monitored::Scalar<int>("evtn_mod32" + channelGainSuffix, evtMod32);
          fill(m_tools[m_evtMod32Groups[partition][drawer]], monEvtMod32);
        }

      }
    } // digits

    if (m_fillCorrelationsHistograms) {
      for (unsigned int gain = 0; gain < Tile::MAX_GAIN; ++gain) {
        if (!meanSampleChannel[gain].empty()) {
          auto monChannel = Monitored::Collection("channel_" + Tile::getDrawerString(ros, drawer), meanSampleChannel[gain]);
          auto monMeanSample = Monitored::Collection("meanSample_" + Tile::getDrawerString(ros, drawer), meanSample[gain]);
          fill(m_tools[m_meanSampleGroups[gain]], monChannel, monMeanSample);

          for (int channel1 : meanSampleChannel[gain]) {
            const std::vector<float>& digits1 = moduleDigits[channel1][gain]->samples();
            for (int channel2 : meanSampleChannel[gain]) {
              const std::vector<float>& digits2 = moduleDigits[channel2][gain]->samples();
              double sampleProdMean = 0.;
              unsigned int nSamples = digits1.size();
              for (unsigned int i = 0; i < nSamples; ++i) {
                sampleProdMean += digits1[i] * digits2[i];
              }
              sampleProdMean /= nSamples;
              meanSampleProd[gain].push_back(sampleProdMean);
              meanSampleProdChannel1[gain].push_back(channel1);
              meanSampleProdChannel2[gain].push_back(channel2);
            }
          }
          auto monChannel1 = Monitored::Collection("channel1_" + Tile::getDrawerString(ros, drawer), meanSampleProdChannel1[gain]);
          auto monChannel2 = Monitored::Collection("channel2_" + Tile::getDrawerString(ros, drawer), meanSampleProdChannel2[gain]);
          auto monMeanSampleProd = Monitored::Collection("meanSampleProd_" + Tile::getDrawerString(ros, drawer), meanSampleProd[gain]);
          fill(m_tools[m_meanSampleProdGroups[gain]], monChannel1, monChannel2, monMeanSampleProd);

          meanSample[gain].clear();
          meanSampleChannel[gain].clear();

          meanSampleProd[gain].clear();
          meanSampleProdChannel1[gain].clear();
          meanSampleProdChannel2[gain].clear();

        }
      }
    }
  }

  if (dqStatus->calibMode() == 0 && !m_rawChannelContainerKey.empty()) {
    if (checkROD_CRC(dqStatus, ctx).isFailure()) {
      ATH_MSG_WARNING("Not possible to check CRC from ROD");
    }
  }

  fill("TileDigitsMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}

void TileDigitsMonitorAlgorithm::checkCRC(bool isCalibMode, uint32_t crc32, uint32_t crcMask,
                                          int nDMU, int ros, int drawer) const {
  /*
   * Author: Luca. F. November 06
   * This method provides CRC checks for Global and TileDMU chips.
   * Global CRC is taken from the trailer and it is correct if
   * the first 16 bits and the last 16 bits of the word match.
   * TileDMU CRC is taken from the DMU mask filled in the FPGA.
   * At the moment it makes sense only for monogain runs
   * (Physics, LED, MonoCIS). The least significant 16 bits are the result of
   * the CRC check in the FE (0xFFFF means all OK).  The most significant 16 bits
   * are the result of the CRC check in the ROD (0xFFFF means all OK).
   */

  int partition = ros - 1;

  // array to adjust order of DMU's of EB* FE CRC status in DMU chip mask word
  // this is needed because FE and ROD fill DMU mask word in different order
  int dmuFE_EB[16]         = { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 8, 9, 10, 11, 14, 15 };
  int dmuFE_EB_special[16] = { 11, 0, 1, 2, 3, 4, 5, 6, 12, 13, 7, 8, 9, 10, 14, 15 };

  // global CRC
  uint32_t crc0 = crc32 & 0xFFFF;
  uint32_t crc1 = crc32 >> 16;

  int crcGlobal = (crc32 == 0) ? 0 : (crc0 == crc1) ? 1 : 2;
  auto monGlobalCRC  = Monitored::Scalar<int>("CRC_global", crcGlobal);
  fill(m_tools[m_crcGlobalGroups[partition][drawer]], monGlobalCRC);

  // CRC per TileDMU. For mono gain we have it from ROD and FE.
  // In bi gain, it is just a placeholder with global CRC information.
  if (isCalibMode) { //!bigain: dummy information
    for (int dmu = 0; dmu < nDMU; ++dmu) {
      auto monCRCErr  = Monitored::Scalar<int>("CRC_err_" + std::to_string(dmu), crcGlobal);
      fill(m_tools[m_crcErrGroups[partition][drawer]], monCRCErr);
    }
  } else {
    /*
     * monogain: meaningful information.
     * Fill 0 if crc32 if 0
     * Fill 1 if both FE and ROD crc is 1
     * Fill 2 if DMU FE crc is 0
     * Fill 3 if DMU ROD crc is 0
     * Fill 4 is FE and ROD crc is 0
     */
    int crcError = -1;
    if (crc32 == 0) {
      crcError = 0;
      for (int dmu = 0; dmu < nDMU; ++dmu) {
        auto monCRCErr  = Monitored::Scalar<int>("CRC_err_" + std::to_string(dmu), crcError);
        fill(m_tools[m_crcErrGroups[partition][drawer]], monCRCErr);
      }
    } else if (crcMask == 0xFFFFFFFF) {
      crcError = 1;
      for (int dmu = 0; dmu < nDMU; ++dmu) {
        auto monCRCErr  = Monitored::Scalar<int>("CRC_err_" + std::to_string(dmu), crcError);
        fill(m_tools[m_crcErrGroups[partition][drawer]], monCRCErr);
      }
    } else {
      uint32_t fe_crc = crcMask & 0xFFFF;
      uint32_t rod_crc = crcMask >> 16;
      for (int dmu = 0; dmu < nDMU; ++dmu) {
        crcError = 1; // TileDMU is fine
        if (ros < 3) { // LB* use simple FE CRC map
          if ((fe_crc >> dmu & 0x1) == 0x0) {
            ++crcError; // add FE error
          }
        } else {
          // EB* use different FECRC map
          if ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17)) {
            // Special EBA15, EBC18
            if ((fe_crc >> dmuFE_EB_special[dmu] & 0x1) == 0x0) {
              ++crcError; // add FE error
            }
          } else {
            if ((fe_crc >> dmuFE_EB[dmu] & 0x1) == 0x0) {
              ++crcError; // add FE error
            }
          }
        }
        if ((rod_crc >> dmu & 0x1) == 0x0) {
          crcError += 2; // add ROD error
        }

        auto monCRCErr  = Monitored::Scalar<int>("CRC_err_" + std::to_string(dmu), crcError);
        fill(m_tools[m_crcErrGroups[partition][drawer]], monCRCErr);

      }
    }
  }
}


StatusCode TileDigitsMonitorAlgorithm::checkROD_CRC(const TileDQstatus* dqStatus, const EventContext& ctx) const {

  /*
   * Author: Luca. F. Mar 08
   * This method provides CRC checks for Global and TileDMU chips.
   * They are taken from the the ROD RawChannel Container
   * TileDMU CRC is taken from the DMU mask filled in the FPGA.
   * It makes sense only for monogain runs
   * (Physics, LED, MonoCIS). The least significant 16 bits are the result of
   * the CRC check in the FE (0xFFFF means all OK).  The most significant 16 bits
   * are the result of the CRC check in the ROD (0xFFFF means all OK).
   * Global CRC is taken also from the DQ Fragment. '0' means OK,
   * '1' means mismatch.
   */

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey, ctx);
  ATH_CHECK( rawChannelContainer.isValid() );

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    if (rawChannelCollection->empty()) continue;

    HWIdentifier hwid = rawChannelCollection->front()->adc_HWID(); // take the first channel in the drawer
    int ros = m_tileHWID->ros(hwid);  // take the ros and drawer from the first channel
    int drawer = m_tileHWID->drawer(hwid);
    uint32_t crc32 = rawChannelCollection->getFragGlobalCRC() & 1;

    if (crc32 == 0) {
      crc32 = 0xFFFFFFFF;
    } else { // means OK CRC match
      crc32 = 0xFFFF;
    } //means NOT OK, CRC mismatch

    uint32_t maskCRC = rawChannelCollection->getFragRODChipMask();
    maskCRC = maskCRC << 16; // ROD mask is stored in the 16 most significant bits ce
    maskCRC += rawChannelCollection->getFragFEChipMask();
    checkCRC(dqStatus, crc32, maskCRC, 16, ros, drawer); // reuse the same funcion used for digits
  }

  return StatusCode::SUCCESS;
}


bool TileDigitsMonitorAlgorithm::checkCorruptedData(const std::vector<std::reference_wrapper<const std::vector<uint32_t>>>& headerWordsAll,
                                                    uint32_t fragStatus, int ros, int drawer, std::vector<std::vector<bool>>& corruptedData) const {

  using Tile = TileCalibUtils;
  int partition = ros - 1;

  std::vector<int> errors;
  auto errorsInDMUs = Monitored::Collection("Error", errors);

  std::vector<int> dmus;
  auto drawerDMUs = Monitored::Collection("DMU", dmus);

  TileDigiErrors error(OK);
  if (fragStatus != TileFragStatus::ALL_OK) {
    if (fragStatus & (TileFragStatus::ALL_FF | TileFragStatus::ALL_00)) {
      error = DUMMY_FRAG;
    } else if (fragStatus & (TileFragStatus::NO_FRAG | TileFragStatus::NO_ROB)) {
      error = NO_RECO_FRAG;
    }
    for (int dmu = 0; dmu < MAX_DMU; ++dmu) {
      dmus.push_back(dmu);
      errors.push_back(error);
    }
    if (m_fillErrorsHistograms) {
      for (unsigned int gain = 0; gain < Tile::MAX_GAIN; ++gain) {
        fill(m_tools[m_errorsGroups[partition][drawer][gain]], drawerDMUs, errorsInDMUs);
      }
    }
    return true;
  }

  /*
   * DMU header format as defined in http://www.sysf.physto.se/~klere/tile-dmu/header.html
   * Bits: 1pllllesdrvvvv0mmgggbbbbbbbbbbbb
   *
   * The header starts with bit 31 set to indicate a header. The other bits as follows:
   * p	-	Parity (odd)
   * l	-	Derandomizer length (number of samples).
   * e	-	Parity error. A parity error was detected from the memory in the last readout.
   * s	-	SEstr Single Error Strobe recieved from the ttc.
   * d	-	DEstr Double Error Strobe recieved from the ttc.
   * r	-	Register parity. Parity from the registers in the chip.
   * v	-	Variable parity. Parity from the variables in the chip.
   * 0	-	Not used. (Set to 0)
   * m	-	Mode.	00 - Normal mode
   *                     01 - Calibration mode
   *                     10 - Test mode
   *                     11 - Not used
   * g	-	High/low gain. Indicates high(1) or low(0) amplification from the 3-in-1 cards.
   * b	-	Bunch Crossing.
   */

  int gain = 0;
  for (auto headerWords : headerWordsAll) {
    int dmu = 0;
    dmus.clear();
    errors.clear();
    for (uint32_t header : headerWords.get() ) {
      error = OK;
      if (checkHeaderFormatError(header)) {
        corruptedData[gain][dmu] = true;
        dmus.push_back(dmu);
        errors.push_back(HEADER_FORM);
      } else if (checkHeaderParityError(header)) {
        corruptedData[gain][dmu] = true;
        dmus.push_back(dmu);
        errors.push_back(HEADER_PAR);
      } else {
        if ((header >> 25) & 0x1) {
          dmus.push_back(dmu);
          error = MEMO_PAR;
          errors.push_back(error);
        }
        if ((header >> 24) & 0x1) {
          dmus.push_back(dmu);
          error = SINGLE_STB;
          errors.push_back(error);
        }
        if ((header >> 23) & 0x1) {
          dmus.push_back(dmu);
          error = DOUBLE_STB;
          errors.push_back(error);
        }
        if (error == OK) {
          dmus.push_back(dmu);
          errors.push_back(OK);
        } else {
          int fragId = m_tileHWID->frag(ros, drawer);
          if (!std::binary_search(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end(), fragId)) {
            corruptedData[gain][dmu] = true;
          }
        }
      }
      ++dmu;
    }
    if (m_fillErrorsHistograms) {
      fill(m_tools[m_errorsGroups[partition][drawer][gain]], drawerDMUs, errorsInDMUs);
    }
    ++gain;
  }
  return false;
}

void TileDigitsMonitorAlgorithm::checkBCID(const std::vector<std::reference_wrapper<const std::vector<uint32_t>>>& headerWordsAll,
                                           uint32_t rodBCID, int ros, int drawer) const {

  int gain = 0;
  int partition = ros - 1;
  for (auto headerWords : headerWordsAll) {
    int dmu = 0;
    for (uint32_t header : headerWords.get() ) {
      uint32_t dmuBCID = (header & 0xFFF);
      auto monBCID  = Monitored::Scalar<int>("BCID_" + std::to_string(dmu), dmuBCID);
      fill(m_tools[m_bcidGroups[partition][drawer][gain]], monBCID);

      if ((dmuBCID == rodBCID) || (dmuBCID == rodBCID - 1)
          || ((rodBCID == 0) && ((dmuBCID == 3563) || (dmuBCID == 3564)))) {

        auto monBCIDErr = Monitored::Scalar<int>("BCID_err_" + std::to_string(dmu), 1);
        fill(m_tools[m_bcidErrGroups[partition][drawer][gain]], monBCIDErr);
      }
      ++dmu;
    }
    ++gain;
  }
}
