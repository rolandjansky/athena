/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileCell.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileRecUtils/TileCellNoiseFilter.h"

// Calo includes
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include <cmath>
#include <algorithm>
#include <functional>

static const InterfaceID IID_ITileCellNoiseFilter("TileCellNoiseFilter", 1, 0);

const InterfaceID& TileCellNoiseFilter::interfaceID() {
  return IID_ITileCellNoiseFilter;
}

//========================================================
// constructor
TileCellNoiseFilter::TileCellNoiseFilter(const std::string& type,
    const std::string& name, const IInterface* parent)
    : base_class(type, name, parent)
    , m_tileID(0)
    , m_tileHWID(0)
    , m_noiseTool("CaloNoiseTool")
    , m_truncationThresholdOnAbsEinSigma(4.0) // 4 sigma of ADC HF noise by default
    , m_minimumNumberOfTruncatedChannels(0.6) // at least 60% of channels should be below threshold
    , m_useTwoGaussNoise(false) // do not use 2G - has no sense for ADC HF noise for the moment
    , m_useTileNoiseDB(true)         // Tile DB with ADC HF noise by defaul
{

  declareInterface<TileCellNoiseFilter>(this);

  declareProperty("CaloNoiseTool", m_noiseTool);

  declareProperty("UseTwoGaussNoise", m_useTwoGaussNoise);
  declareProperty("UseTileNoiseDB", m_useTileNoiseDB);
  declareProperty("TruncationThresholdOnAbsEinSigma", m_truncationThresholdOnAbsEinSigma);
  declareProperty("MinimumNumberOfTruncatedChannels", m_minimumNumberOfTruncatedChannels);
  declareProperty("MaxNoiseSigma", m_maxNoiseSigma = 5.0, "Channels with noise more than that value are igonred in calculation of correction");
}

//========================================================
// Initialize
StatusCode TileCellNoiseFilter::initialize() {
  ATH_MSG_INFO("Initializing...");

  ATH_CHECK( detStore()->retrieve(m_tileID));
  ATH_CHECK( detStore()->retrieve(m_tileHWID));

  //=== get TileCondToolEmscale
  ATH_CHECK( m_tileToolEmscale.retrieve() );

  if (m_useTileNoiseDB) {
    //=== get TileCondToolNoiseSample
    ATH_CHECK( m_tileToolNoiseSample.retrieve());

    //=== get TileBadChanTool
    ATH_CHECK( m_tileBadChanTool.retrieve() );

    m_noiseTool.disable();

  } else {
    //=== CaloNoiseTool
    ATH_CHECK( m_noiseTool.retrieve());

    m_tileToolNoiseSample.disable();
    m_tileBadChanTool.disable();
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// process container
StatusCode TileCellNoiseFilter::process (CaloCellContainer* cellcoll,
                                         const EventContext& /*ctx*/) const
{
  ATH_MSG_DEBUG("in process()");

  int nCells = cellcoll->nCellsCalo(s_caloIndex);
  if (nCells <= 0) {
    ATH_MSG_DEBUG("No TileCells in the container - nothing to do");
    return StatusCode::SUCCESS;
  }

  // common-mode shift calculation
  ATH_MSG_DEBUG("Calculating common-mode shift...");
  cmdata_t commonMode = {{{0}}};
  int ncorr = this->calcCM(cellcoll, commonMode);
  if (ncorr <= 0) {
    ATH_MSG_DEBUG( "Failed to calculate common-mode shift - no corrections applied");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("common-mode shift calculation ended");
  }

  size_t cellItr = cellcoll->indexFirstCellCalo(s_caloIndex);
  size_t lastCell = cellcoll->indexLastCellCalo(s_caloIndex);

  for (; cellItr != lastCell; ++cellItr) {
    CaloCell* cell = (*cellcoll)[cellItr];
    TileCell* tilecell = dynamic_cast<TileCell*>(cell);
    if (tilecell == 0) continue;
    if (tilecell->badcell()) continue;

    setCMSEnergy(commonMode, tilecell);
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
// finalize
StatusCode TileCellNoiseFilter::finalize() {
  return StatusCode::SUCCESS;
}

// ============================================================================
// correct energy
void TileCellNoiseFilter::setCMSEnergy(const cmdata_t& commonMode,
                                       TileCell* tilecell) const {
  //Identifier id  = tilecell->ID(); 
  bool good1 = !tilecell->badch1();
  bool good2 = !tilecell->badch2();
  int gain1 = tilecell->gain1();
  int gain2 = tilecell->gain2();
  const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
  IdentifierHash hash1 = caloDDE->onl1();
  IdentifierHash hash2 = caloDDE->onl2();

  float e1 = tilecell->ene1();
  float e2 = tilecell->ene2();

  if (gain1 == TileID::HIGHGAIN && good1 && hash1 != TileHWID::NOT_VALID_HASH) {
    HWIdentifier adc_id = m_tileHWID->adc_id(hash1, gain1);
    int partition = m_tileHWID->ros(adc_id); // 1-4
    int drawer = m_tileHWID->drawer(adc_id); // 0-63
    int chan = m_tileHWID->channel(adc_id);  // 0-47
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(partition, drawer);
    e1 -= m_tileToolEmscale->channelCalib(drawerIdx, chan, gain1,
        this->getCMShift(commonMode, partition - 1, drawer, chan),
        TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);
  }

  if (gain2 == TileID::HIGHGAIN && good2 && hash2 != TileHWID::NOT_VALID_HASH) {
    HWIdentifier adc_id = m_tileHWID->adc_id(hash2, gain2);
    int partition = m_tileHWID->ros(adc_id); // 1-4
    int drawer = m_tileHWID->drawer(adc_id); // 0-63
    int chan = m_tileHWID->channel(adc_id);  // 0-47
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(partition, drawer);
    e2 -= m_tileToolEmscale->channelCalib(drawerIdx, chan, gain2,
        this->getCMShift(commonMode, partition - 1, drawer, chan),
        TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);
  }

  if ((good1 && good2)
      || hash1 == TileHWID::NOT_VALID_HASH
      || hash2 == TileHWID::NOT_VALID_HASH) {

    // either both channels are good or there is only one channel in a cell
    tilecell->setEnergy(e1, e2);
  } else if (!good1 && good2) {
    // first channel is bad, so second channel is used twice
    tilecell->setEnergy(e2, e2);
  } else if (good1 && !good2) {
    // second channel is bad, so first channel is used twice
    tilecell->setEnergy(e1, e1);
  } else {
    // both are bad - nothing to do
    // but should not be here, because this function is not called for completely bad cells
  }

  return;
}

// ============================================================================
// calculate correction
int TileCellNoiseFilter::calcCM(const CaloCellContainer* cellcoll,
                                cmdata_t& commonMode) const
{
  bool useCaloNoise = (!m_useTileNoiseDB);

  int nEmptyChan[s_maxPartition][s_maxDrawer][s_maxMOB] = {{{0}}};
  int nGoodChan[s_maxPartition][s_maxDrawer][s_maxMOB] = {{{0}}};

  size_t cellItr = cellcoll->indexFirstCellCalo(s_caloIndex);
  size_t lastCell = cellcoll->indexLastCellCalo(s_caloIndex);

  for (; cellItr != lastCell; ++cellItr) {
    const CaloCell* cell = (*cellcoll)[cellItr];
    const TileCell* tilecell = dynamic_cast<const TileCell*>(cell);
    if (tilecell == 0) continue;
    if (tilecell->badcell()) continue;

    float noise_sigma = 1.5, significance = 0.0;

    if (useCaloNoise) {
      if (m_useTwoGaussNoise) {
        noise_sigma = m_noiseTool->getEffectiveSigma(cell
                                                     , ICalorimeterNoiseTool::MAXSYMMETRYHANDLING
                                                     , ICalorimeterNoiseTool::ELECTRONICNOISE);

      } else {
        noise_sigma = m_noiseTool->getNoise(cell, ICalorimeterNoiseTool::ELECTRONICNOISE);
      }

      significance = (noise_sigma != 0.0) ? fabs(cell->energy() / noise_sigma) : 999.999;

      ATH_MSG_VERBOSE( "ID " << m_tileID->to_string(tilecell->ID())
                       << " ene " << cell->energy()
                       << " noise " << noise_sigma
                       << " significance " << significance );
    }

    //Identifier id  = tilecell->ID(); 
    bool good1 = !tilecell->badch1();
    bool good2 = !tilecell->badch2();
    int gain1 = tilecell->gain1();
    int gain2 = tilecell->gain2();
    const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
    IdentifierHash hash1 = caloDDE->onl1();
    IdentifierHash hash2 = caloDDE->onl2();

    if (good1 && hash1 != TileHWID::NOT_VALID_HASH) {
      HWIdentifier adc_id = m_tileHWID->adc_id(hash1, gain1);
      int partition = m_tileHWID->ros(adc_id); // 1-4
      int drawer = m_tileHWID->drawer(adc_id); // 0-63
      int chan = m_tileHWID->channel(adc_id); // 0-47
      int mob = (int) (chan / s_maxChannel);

      ++nGoodChan[partition - 1][drawer][mob];

      if (gain1 == TileID::HIGHGAIN) {

        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(partition, drawer);
        float chanCalMeV = m_tileToolEmscale->channelCalib(drawerIdx, chan,
            gain1, 1., TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);

        float amp = tilecell->ene1() / chanCalMeV;
        if (amp != 0.0) { // second iteration (in case there is non-linear correction)
          chanCalMeV = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain1,
              amp, TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts) / amp;

          amp = tilecell->ene1() / chanCalMeV;
        }

        if (m_useTileNoiseDB) {
          if (m_useTwoGaussNoise) {
            // nothing for the moment - keep 1.5 ADC counts
          } else {
            noise_sigma = m_tileToolNoiseSample->getHfn(drawerIdx, chan, gain1);
          }
          
          significance = 999.999;
          if ((noise_sigma != 0.0) 
              && (noise_sigma < m_maxNoiseSigma)
              /* && (!m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain1).isNoisy()) */) {
            
            significance = fabs(amp / noise_sigma); // caluclate signal/noise ratio
          }

          ATH_MSG_VERBOSE( "HWID " << m_tileHWID->to_string(adc_id)
                          << " calib " << chanCalMeV
                          << " amp " << amp
                          << " noise " << noise_sigma
                          << " significance " << significance );
        }

        // use only empty channels with less significance
        if (significance <= m_truncationThresholdOnAbsEinSigma) {
          commonMode[partition - 1][drawer][mob] += amp;
          nEmptyChan[partition - 1][drawer][mob]++;
        }
      }
    }

    if (good2 && hash2 != TileHWID::NOT_VALID_HASH) {
      HWIdentifier adc_id = m_tileHWID->adc_id(hash2, gain2);
      int partition = m_tileHWID->ros(adc_id); // 1-4
      int drawer = m_tileHWID->drawer(adc_id); // 0-63
      int chan = m_tileHWID->channel(adc_id); // 0-47
      int mob = (int) (chan / s_maxChannel);

      ++nGoodChan[partition - 1][drawer][mob];

      if (gain2 == TileID::HIGHGAIN) {

        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(partition, drawer);
        float chanCalMeV = m_tileToolEmscale->channelCalib(drawerIdx, chan
              , gain2, 1., TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);

        float amp = tilecell->ene2() / chanCalMeV;
        if (amp != 0.0) { // second iteration (in case there is non-linear correction)
          chanCalMeV = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain2,
              amp, TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts) / amp;

          amp = tilecell->ene2() / chanCalMeV;
        }

        if (m_useTileNoiseDB) {
          if (m_useTwoGaussNoise) {
            // nothing for the moment - keep 1.5 ADC counts
          } else {
            noise_sigma = m_tileToolNoiseSample->getHfn(drawerIdx, chan, gain2);
          }

          // use only empty channels with less significance
          significance = 999.999;
          if ((noise_sigma != 0.0) 
              && (noise_sigma < m_maxNoiseSigma)
              /* && (!m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain2).isNoisy()) */) {
            
            significance = fabs(amp / noise_sigma); // caluclate signal/noise ratio
          }


          ATH_MSG_VERBOSE( "HWID " << m_tileHWID->to_string(adc_id)
                          << " calib " << chanCalMeV
                          << " amp " << amp
                          << " noise " << noise_sigma
                          << " significance " << significance );
        }

        // use only empty channels with less significance
        if (significance <= m_truncationThresholdOnAbsEinSigma) {
          commonMode[partition - 1][drawer][mob] += amp;
          nEmptyChan[partition - 1][drawer][mob]++;
        }
      }
    }
  }

  int ncorr = 0;
  int nchmin = m_minimumNumberOfTruncatedChannels;

  for (int partition = 0; partition < s_maxPartition; partition++) {
    for (int drawer = 0; drawer < s_maxDrawer; drawer++) {
      for (int mob = 0; mob < s_maxMOB; mob++) {

        if (m_minimumNumberOfTruncatedChannels < 1.0) {
          nchmin = ceil(m_minimumNumberOfTruncatedChannels
                        * nGoodChan[partition][drawer][mob]);
          if (nchmin < 2) nchmin = 2;
        }

        if (nEmptyChan[partition][drawer][mob] >= nchmin) {
          commonMode[partition][drawer][mob] /= nEmptyChan[partition][drawer][mob];
          ++ncorr;
          ATH_MSG_VERBOSE( "ros " << partition + 1 << std::setw(2)
                          << " drawer " << std::setw(2) << drawer
                          << " mb " << mob
                          << " mean " << commonMode[partition][drawer][mob]
                          << " taken from " << nEmptyChan[partition][drawer][mob] << " channels"
                          << " nchgood " << nGoodChan[partition][drawer][mob]
                          << " nchmin " << nchmin );

        } else {
          if (msgLvl(MSG::VERBOSE)) {
            if (commonMode[partition][drawer][mob] != 0.0) {
              msg(MSG::VERBOSE) << "ros " << partition + 1 << std::setw(2)
                                << " drawer " << std::setw(2) << drawer
                                << " mb " << mob
                                << " mean is zero instead of " << commonMode[partition][drawer][mob]
                                << " / " << nEmptyChan[partition][drawer][mob]
                                << " nchgood " << nGoodChan[partition][drawer][mob]
                                << " nchmin " << nchmin
                                << endmsg;
            } else {
              msg(MSG::VERBOSE) << "ros " << partition + 1
                                << " drawer " << std::setw(2) << drawer
                                << " mb " << mob
                                << " mean is zero - nothing to correct"
                                << " nchgood " << nGoodChan[partition][drawer][mob]
                                << " nchmin " << nchmin
                                << endmsg;
            }
          }
          commonMode[partition][drawer][mob] = 0.0;
        }
      }
    }
  }

  return ncorr;
}

