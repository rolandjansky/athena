/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibBlobObjs/TileBchDecoder.h"
#include "TileCalibBlobObjs/Exception.h"

//
//________________________________________________________________
TileBchDecoder::TileBchDecoder(BitPatVer bitPatVer)
{
  if(     bitPatVer==TileBchDecoder::BitPat_ofl01){ init_BitPat_ofl01(); }
  else if(bitPatVer==TileBchDecoder::BitPat_onl01){ init_BitPat_onl01(); }
  else{
    throw TileCalib::InvalidBitPattern("TileBchDecoder::Ctor", bitPatVer);
  }
}

//
//________________________________________________________________
TileBchStatus
TileBchDecoder::decode(uint32_t status_channel, uint32_t status_adc) const 
{
  TileBchStatus status;
  for(unsigned int i = 0; i < 32; ++i){
    //=== decode channel problems
    if(status_channel & 0x00000001){ status += m_bitToPrbChn[i]; }
    //=== decode adc problems
    if(status_adc     & 0x00000001){ status += m_bitToPrbAdc[i]; }
    //=== shift the status words by one bit
    status_channel >>= 1;
    status_adc     >>= 1;
  }
  return status;
}

//
//________________________________________________________________
TileBchWords
TileBchDecoder::encode(const TileBchStatus& status) const 
{
  TileBchWords words;
  const std::set<TileBchPrbs::Prb>& prbs = status.getPrbs();
  for(std::set<TileBchPrbs::Prb>::const_iterator iPrb = prbs.begin();
      iPrb!=prbs.end(); ++iPrb)
    {
      std::map<TileBchPrbs::Prb,std::pair<unsigned int,unsigned int> >::const_iterator 
	iMap = m_prbToBit.find(*iPrb);
      if(iMap == m_prbToBit.end()){
	//=== throw an exception if we can not encode the problem
	throw TileCalib::InvalidBchProblem("TileBchDecoder::encode", *iPrb);
      }
      words[iMap->second.first] |= (1 << iMap->second.second);
    }
  return words;
}

//
//________________________________________________________________
void 
TileBchDecoder::init_BitPat_ofl01()
{
  //=== assign decoder to channel problems
  m_bitToPrbChn.clear();
  m_bitToPrbChn.resize(32,TileBchPrbs::Invalid);
  m_bitToPrbChn[ 0] = TileBchPrbs::GeneralMaskChannel;
  m_bitToPrbChn[ 1] = TileBchPrbs::NoPmt;
  m_bitToPrbChn[ 2] = TileBchPrbs::NoHV;
  m_bitToPrbChn[ 3] = TileBchPrbs::WrongHV;
  m_bitToPrbChn[ 4] = TileBchPrbs::NoLaser;
  m_bitToPrbChn[ 5] = TileBchPrbs::BadLaser;
  m_bitToPrbChn[ 6] = TileBchPrbs::NoCesium;
  m_bitToPrbChn[ 7] = TileBchPrbs::BadCesium;
  m_bitToPrbChn[ 8] = TileBchPrbs::NoTiming;
  m_bitToPrbChn[ 9] = TileBchPrbs::BadTiming;
  m_bitToPrbChn[10] = TileBchPrbs::TrigGeneralMask;
  m_bitToPrbChn[11] = TileBchPrbs::TrigNoGain;
  m_bitToPrbChn[12] = TileBchPrbs::TrigHalfGain;
  m_bitToPrbChn[13] = TileBchPrbs::TrigNoisy;
  m_bitToPrbChn[14] = TileBchPrbs::Emergency;
  m_bitToPrbChn[15] = TileBchPrbs::HVReadoutPb;
  m_bitToPrbChn[16] = TileBchPrbs::BrokenClearFibre;
  m_bitToPrbChn[17] = TileBchPrbs::IgnoreCs;
  m_bitToPrbChn[18] = TileBchPrbs::UnstableCs;
  m_bitToPrbChn[19] = TileBchPrbs::WrongBCID;
  m_bitToPrbChn[20] = TileBchPrbs::TimingDmuBcOffset;
  m_bitToPrbChn[21] = TileBchPrbs::BurntIntegrator;
  
  //=== assign decoder to adc problems
  m_bitToPrbAdc.clear();
  m_bitToPrbAdc.resize(32,TileBchPrbs::Invalid);
  m_bitToPrbAdc[ 0] = TileBchPrbs::GeneralMaskAdc;
  m_bitToPrbAdc[ 1] = TileBchPrbs::AdcDead;
  m_bitToPrbAdc[ 2] = TileBchPrbs::StuckBit;
  m_bitToPrbAdc[ 3] = TileBchPrbs::DataCorruption;
  m_bitToPrbAdc[ 4] = TileBchPrbs::VeryLargeHfNoise;
  m_bitToPrbAdc[ 5] = TileBchPrbs::NoData;
  m_bitToPrbAdc[ 6] = TileBchPrbs::WrongDspConfig;
  m_bitToPrbAdc[ 7] = TileBchPrbs::LargeHfNoise;
  m_bitToPrbAdc[ 8] = TileBchPrbs::CorrelatedNoise;
  m_bitToPrbAdc[ 9] = TileBchPrbs::LargeLfNoise;
  m_bitToPrbAdc[10] = TileBchPrbs::NoCis;
  m_bitToPrbAdc[11] = TileBchPrbs::BadCis;
  m_bitToPrbAdc[12] = TileBchPrbs::SevereStuckBit;
  m_bitToPrbAdc[13] = TileBchPrbs::SevereDataCorruption;
  m_bitToPrbAdc[14] = TileBchPrbs::IgnoredByDQV;

  //=== initialize problem to word/bit map
  initPrbToBit();
}

//
//________________________________________________________________
void 
TileBchDecoder::init_BitPat_onl01()
{
  //=== assign decoder to channel problems
  m_bitToPrbChn.clear();
  m_bitToPrbChn.resize(32,TileBchPrbs::Invalid);
  m_bitToPrbChn[ 0] = TileBchPrbs::IgnoredInDsp;
  m_bitToPrbChn[ 1] = TileBchPrbs::IgnoredInHlt;
  m_bitToPrbChn[ 2] = TileBchPrbs::DisableForL1;
  m_bitToPrbChn[ 3] = TileBchPrbs::TrigGeneralMask;
  m_bitToPrbChn[ 4] = TileBchPrbs::TrigNoGain;
  m_bitToPrbChn[ 5] = TileBchPrbs::TrigHalfGain;
  m_bitToPrbChn[ 6] = TileBchPrbs::TrigNoisy;
  m_bitToPrbChn[ 7] = TileBchPrbs::OnlineBadTiming;
  m_bitToPrbChn[ 8] = TileBchPrbs::OnlineWrongBCID;
  m_bitToPrbChn[ 9] = TileBchPrbs::OnlineTimingDmuBcOffset;
  
  //=== assign decoder to adc problems
  m_bitToPrbAdc.clear();
  m_bitToPrbAdc.resize(32,TileBchPrbs::Invalid);
  m_bitToPrbAdc[ 0] = TileBchPrbs::OnlineGeneralMaskAdc;

  //=== initialize problem to word/bit map
  initPrbToBit();
}

//
//________________________________________________________________
void 
TileBchDecoder::initPrbToBit()
{
  m_prbToBit.clear();
  //=== init channel problems
  for(unsigned int i = 0; i < 32; ++i){
    TileBchPrbs::Prb prb = m_bitToPrbChn[i];
    if(prb==TileBchPrbs::Invalid) continue;
    //=== check for duplicates
    if(m_prbToBit.find(prb) != m_prbToBit.end()){
      throw TileCalib::InvalidBitPattern("TileBchDecoder::initPrbToBit(Chn)",i);
    }
    m_prbToBit[prb] = std::make_pair(0,i);
  }

  //=== init adc problems
  for(unsigned int i = 0; i < 32; ++i){
    TileBchPrbs::Prb prb = m_bitToPrbAdc[i];
    if(prb==TileBchPrbs::Invalid) continue;
    //=== check for duplicates
    if(m_prbToBit.find(prb)!=m_prbToBit.end()){
      throw TileCalib::InvalidBitPattern("TileBchDecoder::initPrbToBit(Adc)",i);
    }
    m_prbToBit[prb] = std::make_pair(1, i);
  }
}

//
//________________________________________________________________
void 
TileBchDecoder::printBitAssignment(std::ostream& stm) const
{
  stm << "Current bit to problem assignment" << std::endl;
  stm << "---------------------------------" << std::endl;
  stm << "Channel:"                          << std::endl;
  for(unsigned int i = 0; i < 32; ++i){
    TileBchPrbs::Prb prb = m_bitToPrbChn[i];
    std::string desc = TileBchPrbs::getDescription(prb);
    stm << "bit "<<i<<"\t:\t" << desc << std::endl;
  }
  stm << "Adc:"                              << std::endl;
  for(unsigned int i = 0; i < 32; ++i){
    TileBchPrbs::Prb prb = m_bitToPrbAdc[i];
    std::string desc = TileBchPrbs::getDescription(prb);
    stm << "bit "<<i<<"\t:\t" << desc << std::endl;
  }
}
