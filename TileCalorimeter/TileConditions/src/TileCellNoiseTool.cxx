/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// Tile includes
#include "TileConditions/TileCellNoiseTool.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/Exception.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

#include <cmath>
#include <algorithm>


//
//____________________________________________________________________
TileCellNoiseTool::TileCellNoiseTool(const std::string& type, const std::string& name,
                                     const IInterface* parent)
  : base_class(type, name, parent)
  , m_tileCabling("TileCablingSvc", name)
{
}

//
//____________________________________________________________________
TileCellNoiseTool::~TileCellNoiseTool() {
}

//
//____________________________________________________________________
StatusCode TileCellNoiseTool::initialize() {

  ATH_MSG_DEBUG( "In initialize()" );

  //=== TileCablingSvc
  CHECK( m_tileCabling.retrieve() );

  //=== TileCondIdTransforms
  CHECK( m_tileIdTrans.retrieve() );

  //=== TileCondToolNoise
  CHECK( m_tileToolNoise.retrieve() );

  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
StatusCode TileCellNoiseTool::finalize() {
  ATH_MSG_DEBUG( "finalize called" );
  return StatusCode::SUCCESS;
}

//
//____________________________________________________________________
float TileCellNoiseTool::getCellNoise(const Identifier& cell_id,
                                      CaloGain::CaloGain caloGain,
                                      const EventContext& ctx) const {

  CaloGain::CaloGain gain(caloGain);

  const TileCablingService* cabling = m_tileCabling->cablingService();
  unsigned int nChn = cabling->getNChanPerCell(cell_id);

  // positive values are LAr gains
  // convert them to appropriate Tile gains
  if (gain > CaloGain::INVALIDGAIN) {

    switch (nChn) {
      case 2:
        switch (caloGain) {
          case CaloGain::LARHIGHGAIN:
            gain = CaloGain::TILEHIGHHIGH;
            break;
          case CaloGain::LARMEDIUMGAIN:
            gain = CaloGain::TILEHIGHLOW;
            break;
          case CaloGain::LARLOWGAIN:
            gain = CaloGain::TILELOWLOW;
            break;
          default:
            gain = CaloGain::INVALIDGAIN;
            break;
        }
        break;
      case 1:
        switch (caloGain) {
          case CaloGain::LARHIGHGAIN:
            gain = CaloGain::TILEONEHIGH;
            break;
          case CaloGain::LARLOWGAIN:
            gain = CaloGain::TILEONELOW;
            break;
          default:
            gain = CaloGain::INVALIDGAIN; // medium gain is invalid as well
            break;
        }
        break;
      default:
        gain = CaloGain::INVALIDGAIN;
        break;
    }

  }
  if (gain == CaloGain::INVALIDGAIN) {
    throw TileCalib::InvalidCaloGain(name() + "::getCellNoise()", gain);
  }

  unsigned int gain1 = gain & 3; // gain for first  PMT
  unsigned int gain2 = (gain >> 2) & 3; // gain for second PMT

  float noise1(0.), noise2(0.), noise12(0.), noise21(0.);
  unsigned int drawerIdx(0), channel(0), adc(0);

  try {
    if (gain1 < 2) { // valid gain
      Identifier pmt_id = cabling->getTileID()->pmt_id(cell_id, 0); // first pmt offline ID
      HWIdentifier hwid = cabling->getTileHWID()->adc_id(cabling->s2h_channel_id(pmt_id), gain1); // ADC HWID ch1,gain1
      m_tileIdTrans->getIndices(hwid, drawerIdx, channel, adc);
      noise1 = m_tileToolNoise->getNoise(drawerIdx, channel, adc, TileRawChannelUnit::MegaElectronVolts, ctx);

      if (CaloGain::LARMEDIUMGAIN == caloGain) { // swap gains to take max noise later
        hwid = cabling->getTileHWID()->adc_id(cabling->s2h_channel_id(pmt_id), 1 - gain1); // ADC HWID other gain
        m_tileIdTrans->getIndices(hwid, drawerIdx, channel, adc);
        noise12 = m_tileToolNoise->getNoise(drawerIdx, channel, adc, TileRawChannelUnit::MegaElectronVolts, ctx);
      }
    }

    if (gain2 < 2 && nChn > 1) { // valid gain and second PMT exists
      Identifier pmt_id = cabling->getTileID()->pmt_id(cell_id, 1); // second pmt offline ID
      HWIdentifier hwid = cabling->getTileHWID()->adc_id(cabling->s2h_channel_id(pmt_id), gain2); // ADC HWID ch2,gain2
      m_tileIdTrans->getIndices(hwid, drawerIdx, channel, adc);
      noise2 = m_tileToolNoise->getNoise(drawerIdx, channel, adc, TileRawChannelUnit::MegaElectronVolts, ctx);

      if (CaloGain::LARMEDIUMGAIN == caloGain) { // swap gains to take max noise later
        hwid = cabling->getTileHWID()->adc_id(cabling->s2h_channel_id(pmt_id), 1 - gain2); // ADC HWID other gain
        m_tileIdTrans->getIndices(hwid, drawerIdx, channel, adc);
        noise21 = m_tileToolNoise->getNoise(drawerIdx, channel, adc, TileRawChannelUnit::MegaElectronVolts, ctx);
      }
    }
  } catch (TileCalib::Exception& e) {
    ATH_MSG_ERROR( "Caught TileCalib::Exception: " << e.what() );
    ATH_MSG_ERROR( "CellNoiseSigma could not perform channel calibration. "
                  << "cellId=" << cell_id.getString() << " "
                  << "gain=" << gain << " " );
  }

  // calculating noise of 2 channels
  float noise = ::hypot(noise1, noise2);
  if (CaloGain::LARMEDIUMGAIN == caloGain) { // choosing maximum among 2 (high,low) combinations
    noise = std::max(noise, static_cast<float>(::hypot(noise12, noise21)));
  }

  return noise;
}
