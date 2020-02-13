/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEDIGITSFLXALGORITHM_H
#define TILEMONITORING_TILEDIGITSFLXALGORITHM_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"


// Atlas includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileHWID;

class TileDigitsFlxMonitorAlgorithm : public AthMonitorAlgorithm {
  public:
    TileDigitsFlxMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TileDigitsFlxMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  private:

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Tile digits container"};

    const TileHWID* m_tileHWID;

    Gaudi::Property<int> m_firstSample{this, "FirstSample", 0, "First sample to put into histogram"};
    Gaudi::Property<int> m_lastSample{this, "LastSample", 16, "Last sample to put into histogram"}; 

};
#endif // TILEMONITORING_TILEDIGITSFLXALGORITHM_H
