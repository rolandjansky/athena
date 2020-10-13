/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TileCalibBlobObjs/TileBchPrbs.h"

//
//________________________________________________________________
std::string 
TileBchPrbs::getDescription(const Prb& prb)
{
  static const std::map<Prb,std::string> prbNames = initPrbDesc();
  std::map<Prb,std::string>::const_iterator iMap = prbNames.find(prb);
  std::string desc("<no description available>");
  if(iMap!=prbNames.end()){ desc=iMap->second; }
  return desc;
}

//
//________________________________________________________________
std::map<TileBchPrbs::Prb,std::string>
TileBchPrbs::initPrbDesc()
{
  std::map<Prb,std::string> prbNames;
  prbNames[TileBchPrbs::Invalid                ] = "Invalid";

  //=== adc
  prbNames[TileBchPrbs::GeneralMaskAdc         ] = "ADC masked (unspecified)";
  prbNames[TileBchPrbs::AdcDead                ] = "ADC dead";
  prbNames[TileBchPrbs::StuckBit               ] = "Stuck bit";
  prbNames[TileBchPrbs::SevereStuckBit         ] = "Severe stuck bit";
  prbNames[TileBchPrbs::DataCorruption         ] = "Data corruption";
  prbNames[TileBchPrbs::SevereDataCorruption   ] = "Severe data corruption";
  prbNames[TileBchPrbs::VeryLargeHfNoise       ] = "Very large HF noise";
  prbNames[TileBchPrbs::NoData                 ] = "No data";
  prbNames[TileBchPrbs::WrongDspConfig         ] = "Wrong DSP configuration";
  prbNames[TileBchPrbs::LargeHfNoise           ] = "Large HF noise";
  prbNames[TileBchPrbs::CorrelatedNoise        ] = "Correlated noise";
  prbNames[TileBchPrbs::LargeLfNoise           ] = "Large LF noise";
  prbNames[TileBchPrbs::NoCis                  ] = "No CIS calibration";
  prbNames[TileBchPrbs::BadCis                 ] = "Bad CIS calibration";
  prbNames[TileBchPrbs::IgnoredByDQV           ] = "Ignored by DQV";

  //=== channel
  prbNames[TileBchPrbs::GeneralMaskChannel     ] = "Channel masked (unspecified)";
  prbNames[TileBchPrbs::NoPmt                  ] = "No PMT connected";
  prbNames[TileBchPrbs::NoHV                   ] = "No HV";
  prbNames[TileBchPrbs::WrongHV                ] = "Wrong HV";
  prbNames[TileBchPrbs::NoLaser                ] = "No laser calibration";
  prbNames[TileBchPrbs::BadLaser               ] = "Bad laser calibration";
  prbNames[TileBchPrbs::NoCesium               ] = "No cesium calibration";
  prbNames[TileBchPrbs::BadCesium              ] = "Bad cesium calibration";
  prbNames[TileBchPrbs::NoTiming               ] = "No timing set in dskew";
  prbNames[TileBchPrbs::BadTiming              ] = "Bad timing";
  prbNames[TileBchPrbs::Emergency              ] = "Module in emergency mode";
  prbNames[TileBchPrbs::HVReadoutPb            ] = "HV readout problem";
  prbNames[TileBchPrbs::BrokenClearFibre       ] = "Broken clear fibre";
  prbNames[TileBchPrbs::IgnoreCs               ] = "Ignore cesium";
  prbNames[TileBchPrbs::UnstableCs             ] = "Unstable cesium";
  prbNames[TileBchPrbs::WrongBCID              ] = "Wrong BCID";
  prbNames[TileBchPrbs::TimingDmuBcOffset      ] = "Timing DMU BC offset";
  prbNames[TileBchPrbs::BurntIntegrator        ] = "Burnt Integrator";

  //=== DSP
  prbNames[TileBchPrbs::IgnoredInDsp           ] = "Ignored in DSP";
  prbNames[TileBchPrbs::IgnoredInHlt           ] = "Ignored in HLT";
  
  //=== Trigger
  prbNames[TileBchPrbs::TrigGeneralMask        ] = "Channel masked for LV1 (unspecified)";
  prbNames[TileBchPrbs::TrigNoGain             ] = "LV1 channel no gain";
  prbNames[TileBchPrbs::TrigHalfGain           ] = "LV1 channel half gain";
  prbNames[TileBchPrbs::TrigNoisy              ] = "LV1 channel noisy";
  prbNames[TileBchPrbs::DisableForL1           ] = "Channel disabled for LV1";

  //=== online channel
  prbNames[TileBchPrbs::OnlineBadTiming        ] = "Online bad timing";
  prbNames[TileBchPrbs::OnlineTimingDmuBcOffset] = "Online timing DMU BC offset";
  prbNames[TileBchPrbs::OnlineWrongBCID        ] = "Online wrong BCID";

  //=== online adc
  prbNames[TileBchPrbs::OnlineGeneralMaskAdc   ] = "Online ADC masked (unspecified)";

  return prbNames;
}
