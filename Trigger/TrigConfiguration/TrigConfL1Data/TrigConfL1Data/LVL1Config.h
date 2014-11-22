/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_LVL1Config
#define TrigConf_LVL1Config
/*
  LVL1Config.h
*/
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/Muctpi.h"

namespace TrigConf {

  class LVL1Config {
  public:
    LVL1Config();
    ~LVL1Config();

    inline Menu& menu();
    inline ThresholdConfig& thresholdConfig();
    inline Muctpi& muctpi();

  protected:
    Menu m_menu;
    ThresholdConfig m_thresholdConfig;
    Muctpi m_muctpi;
  };

  Menu& LVL1Config::menu() { return m_menu; }
  ThresholdConfig& LVL1Config::thresholdConfig() { return m_thresholdConfig; }
  Muctpi& LVL1Config::muctpi() { return m_muctpi; }

}

#endif
