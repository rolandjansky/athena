/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_TRIGT1CALOCALIBCONDITIONSDICT_H
#define TRIGT1CALOCALIBCONDITIONS_TRIGT1CALOCALIBCONDITIONSDICT_H

#include "TrigT1CaloCalibConditions/L1CaloPedestalContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanResultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanRunInfoContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannel.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalib.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibV1Container.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibV1.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsV1.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsV1Container.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaults.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannels.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowers.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategyContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategy.h"
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunParsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunPars.h"
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfig.h"

#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParameters.h"




struct TrigT1CaloCalibConditions_DUMMY_Instantiation {
  // we need to instantiate templated objects to get them into the dict
  std::map<unsigned int, L1CaloRampData> L1CaloRampDataMap;
  std::pair<L1CaloRampStep, L1CaloRampStep> L1CaloRampDataPoint;
  std::vector<std::pair<L1CaloRampStep, L1CaloRampStep> > L1CaloRampDataVector;
  std::pair<const unsigned int, L1CaloRampData> L1CaloRampDataMapValueType;
  std::map<unsigned int, L1CaloRampData>::const_iterator L1CaloRampDataMapConstIterator;
  L1CaloEnergyScanResultsContainer::const_iterator L1CaloEnergyScanResultsMapConstInterator;
  std::pair<const unsigned int, L1CaloEnergyScanResults> L1CaloEnergyScanResultsMapValueType;
  std::map<unsigned int, L1CaloPprDisabledChannel> L1CaloPprDisabledChannelMap;
  std::map<unsigned int, L1CaloPprDisabledChannel>::const_iterator L1CaloPprDisabledChannelMapConstIterator;

  L1CaloPprChanCalibContainer::const_iterator L1CaloPprChanCalibContainerCI;
  L1CaloPprChanCalibV1Container::const_iterator L1CaloPprChanCalibV1ContainerCI;
  L1CaloPprChanDefaultsV1Container::const_iterator L1CaloPprChanDefaultsV1ContainerCI;
  L1CaloPprChanDefaultsContainer::const_iterator L1CaloPprChanDefaultsContainerCI;
  L1CaloPpmDeadChannelsContainer::const_iterator L1CaloPpmDeadChannelsContainerCI;
  L1CaloDisabledTowersContainer::const_iterator L1CaloDisabledTowersContainerCI;
  L1CaloPprChanStrategyContainer::const_iterator L1CaloPprChanStrategyContainerConstInterator;
  L1CaloDerivedRunParsContainer::const_iterator L1CaloDerivedRunParsContainerConstInterator;
  L1CaloReadoutConfigContainer::const_iterator L1CaloReadoutConfigContainerConstInterator;
  L1CaloRunParametersContainer::const_iterator L1CaloRunParametersContainerConstInterator;

};

#endif
