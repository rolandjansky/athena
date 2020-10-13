/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILEBCHPRBS_H
#define TILECALIBBLOBOBJS_TILEBCHPRBS_H

/** 
    @brief Static class providing TileCal bad channel problems.
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details The different problems are encoded as enums. The enum
    assigned to a problem should never be changed (otherwise readback
    of old conditions data is compromised)!
*/

#include <map>
#include <string>

class TileBchPrbs
{
 public:

  enum Prb{

    Invalid                    = 0,
    
    //=============================
    //=== ADC
    //=============================
    //---serious problems
    GeneralMaskAdc             = 1000,
    AdcDead                    = 1001,
    VeryLargeHfNoise           = 1004,
    NoData                     = 1005,
    WrongDspConfig             = 1006,
    SevereStuckBit             = 1007,
    SevereDataCorruption       = 1008,
    //--- impact on data quality
    StuckBit                   = 1002,
    DataCorruption             = 1003,
    LargeHfNoise               = 1100,
    CorrelatedNoise            = 1101,
    LargeLfNoise               = 1102,
    NoCis                      = 1103,
    BadCis                     = 1104,
    IgnoredByDQV               = 1105,

    //=============================
    //=== Channel
    //=============================
    //--- serious problems
    GeneralMaskChannel         = 2000,
    NoPmt                      = 2001,
    NoHV                       = 2002,
    WrongHV                    = 2003,

    //--- impact on data quality
    NoLaser                    = 2100,
    BadLaser                   = 2101,
    NoCesium                   = 2102,
    BadCesium                  = 2103,
    NoTiming                   = 2104,
    BadTiming                  = 2105,
    Emergency                  = 2106,
    HVReadoutPb                = 2107,
    BrokenClearFibre           = 2108,
    IgnoreCs                   = 2109,
    UnstableCs                 = 2110,
    WrongBCID                  = 2111,
    TimingDmuBcOffset          = 2112,
    BurntIntegrator            = 2113,
    
    //============================================
    //=== Level-1 Trigger (all channel) - online
    //============================================
    TrigGeneralMask            = 3000,
    TrigNoGain                 = 3001,
    TrigHalfGain               = 3002,
    TrigNoisy                  = 3003,

    //================================
    //=== Other ONLINE (all channel)
    //================================
    IgnoredInDsp               = 4000,
    IgnoredInHlt               = 4001,
    DisableForL1               = 4002,
    OnlineBadTiming            = 4105,
    OnlineWrongBCID            = 4111,
    OnlineTimingDmuBcOffset    = 4112,

    //================================
    //=== Other ONLINE ADC
    //================================
    OnlineGeneralMaskAdc       = 5000

 };


  /** @brief Get description of problem */
  static std::string getDescription(const Prb& prb);
  
 private:
  /** @brief Initializes the problem description map*/
  static std::map<Prb,std::string> initPrbDesc();
};

#endif
