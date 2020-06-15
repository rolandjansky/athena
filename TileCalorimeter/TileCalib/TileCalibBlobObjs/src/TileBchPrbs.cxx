/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TileCalibBlobObjs/TileBchPrbs.h"

//________________________________________________________________
std::map<TileBchPrbs::Prb,std::string> TileBchPrbs::m_prbNames;

//
//________________________________________________________________
std::string 
TileBchPrbs::getDescription(const Prb& prb)
{
  if(!m_prbNames.size()) { initPrbDesc(); }
  std::map<Prb,std::string>::const_iterator iMap = m_prbNames.find(prb);
  std::string desc("<no description available>");
  if(iMap!=m_prbNames.end()){ desc=iMap->second; }
  return desc;
}

//
//________________________________________________________________
void 
TileBchPrbs::initPrbDesc()
{
  m_prbNames[TileBchPrbs::Invalid                ] = "Invalid";

  //=== adc
  m_prbNames[TileBchPrbs::GeneralMaskAdc         ] = "ADC masked (unspecified)";
  m_prbNames[TileBchPrbs::AdcDead                ] = "ADC dead";
  m_prbNames[TileBchPrbs::StuckBit               ] = "Stuck bit";
  m_prbNames[TileBchPrbs::SevereStuckBit         ] = "Severe stuck bit";
  m_prbNames[TileBchPrbs::DataCorruption         ] = "Data corruption";
  m_prbNames[TileBchPrbs::SevereDataCorruption   ] = "Severe data corruption";
  m_prbNames[TileBchPrbs::VeryLargeHfNoise       ] = "Very large HF noise";
  m_prbNames[TileBchPrbs::NoData                 ] = "No data";
  m_prbNames[TileBchPrbs::WrongDspConfig         ] = "Wrong DSP configuration";
  m_prbNames[TileBchPrbs::LargeHfNoise           ] = "Large HF noise";
  m_prbNames[TileBchPrbs::CorrelatedNoise        ] = "Correlated noise";
  m_prbNames[TileBchPrbs::LargeLfNoise           ] = "Large LF noise";
  m_prbNames[TileBchPrbs::NoCis                  ] = "No CIS calibration";
  m_prbNames[TileBchPrbs::BadCis                 ] = "Bad CIS calibration";
  m_prbNames[TileBchPrbs::IgnoredByDQV           ] = "Ignored by DQV";

  //=== channel
  m_prbNames[TileBchPrbs::GeneralMaskChannel     ] = "Channel masked (unspecified)";
  m_prbNames[TileBchPrbs::NoPmt                  ] = "No PMT connected";
  m_prbNames[TileBchPrbs::NoHV                   ] = "No HV";
  m_prbNames[TileBchPrbs::WrongHV                ] = "Wrong HV";
  m_prbNames[TileBchPrbs::NoLaser                ] = "No laser calibration";
  m_prbNames[TileBchPrbs::BadLaser               ] = "Bad laser calibration";
  m_prbNames[TileBchPrbs::NoCesium               ] = "No cesium calibration";
  m_prbNames[TileBchPrbs::BadCesium              ] = "Bad cesium calibration";
  m_prbNames[TileBchPrbs::NoTiming               ] = "No timing set in dskew";
  m_prbNames[TileBchPrbs::BadTiming              ] = "Bad timing";
  m_prbNames[TileBchPrbs::Emergency              ] = "Module in emergency mode";
  m_prbNames[TileBchPrbs::HVReadoutPb            ] = "HV readout problem";
  m_prbNames[TileBchPrbs::BrokenClearFibre       ] = "Broken clear fibre";
  m_prbNames[TileBchPrbs::IgnoreCs               ] = "Ignore cesium";
  m_prbNames[TileBchPrbs::UnstableCs             ] = "Unstable cesium";
  m_prbNames[TileBchPrbs::WrongBCID              ] = "Wrong BCID";
  m_prbNames[TileBchPrbs::TimingDmuBcOffset      ] = "Timing DMU BC offset";
  m_prbNames[TileBchPrbs::BurntIntegrator        ] = "Burnt Integrator";

  //=== DSP
  m_prbNames[TileBchPrbs::IgnoredInDsp           ] = "Ignored in DSP";
  m_prbNames[TileBchPrbs::IgnoredInHlt           ] = "Ignored in HLT";
  
  //=== Trigger
  m_prbNames[TileBchPrbs::TrigGeneralMask        ] = "Channel masked for LV1 (unspecified)";
  m_prbNames[TileBchPrbs::TrigNoGain             ] = "LV1 channel no gain";
  m_prbNames[TileBchPrbs::TrigHalfGain           ] = "LV1 channel half gain";
  m_prbNames[TileBchPrbs::TrigNoisy              ] = "LV1 channel noisy";
  m_prbNames[TileBchPrbs::DisableForL1           ] = "Channel disabled for LV1";

  //=== online channel
  m_prbNames[TileBchPrbs::OnlineBadTiming        ] = "Online bad timing";
  m_prbNames[TileBchPrbs::OnlineTimingDmuBcOffset] = "Online timing DMU BC offset";

  //=== online adc
  m_prbNames[TileBchPrbs::OnlineGeneralMaskAdc   ] = "Online ADC masked (unspecified)";
  m_prbNames[TileBchPrbs::OnlineWrongBCID        ] = "Online wrong BCID";
}
