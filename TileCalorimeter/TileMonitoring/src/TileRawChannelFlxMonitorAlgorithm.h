/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILERAWCHANNELFLXALGORITHM_H
#define TILEMONITORING_TILERAWCHANNELFLXALGORITHM_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"


// Atlas includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileHWID;

class TileRawChannelFlxMonitorAlgorithm : public AthMonitorAlgorithm {
  public:
    TileRawChannelFlxMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TileRawChannelFlxMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  private:

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Tile digits container"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKeyFlx{this,
        "TileRawChannelContainerFlx", "TileRawChannelOpt2", "Tile raw channel container"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKeyLegacy{this,
        "TileRawChannelContainerLegacy", "TileRawChannelFit", "Tile raw channel container"};

    const TileHWID* m_tileHWID;

    Gaudi::Property<int> m_firstSample{this, "FirstSample", 0, "First sample to put into histogram"};
    Gaudi::Property<int> m_lastSample{this, "LastSample", 16, "Last sample to put into histogram"}; 

    Gaudi::Property<std::vector<int>> m_fragIDsToCompare{this,
    "TileFragIDsToCompare", {0x203, 0x403}, "Tile Frag IDs of modules to compare."};


};
#endif // TILEMONITORING_TILERAWCHANNELFLXALGORITHM_H
