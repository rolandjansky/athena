/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "../TilePaterMonTool.h"
#include "../TileFatherMonTool.h"
#include "../TileCellMonTool.h"
#include "../TileTowerMonTool.h"
#include "../TileClusterMonTool.h"
#include "../TileMuIdMonTool.h"
#include "../TileL2MonTool.h"
#include "../TileMuonFitMonTool.h"
#include "../TileDigitsMonTool.h"
#include "../TileRawChannelMonTool.h"
#include "../TileDQFragMonTool.h"
#include "../TileDQFragLWMonTool.h"
#include "../TileRODMonTool.h"
#include "../TileJetMonTool.h"
#include "../TileDigiNoiseMonTool.h"
#include "../TileTMDBDigitsMonTool.h"
#include "../TileTMDBRawChannelMonTool.h"
#include "../TileCellNoiseMonTool.h"
#include "../TileRawChannelTimeMonTool.h"
#include "../TileRawChannelNoiseMonTool.h"
#include "../TileTBBeamMonTool.h"
#include "../TileTBMonTool.h"
#include "../TileTBCellMonTool.h"
#include "../TileTBPulseMonTool.h"
#include "../TileJetMonitorAlgorithm.h"
#include "../TileDigitsFlxMonitorAlgorithm.h"
#include "../TileDQFragMonitorAlgorithm.h"
#include "../TileMBTSMonitorAlgorithm.h"
#include "../TileRawChannelFlxMonitorAlgorithm.h"
#include "../TileCellMonitorAlgorithm.h"
#include "../TileTowerMonitorAlgorithm.h"
#include "../TileClusterMonitorAlgorithm.h"
#include "../TileMuIdMonitorAlgorithm.h"
#include "../TileDigiNoiseMonitorAlgorithm.h"
#include "../TileRawChannelTimeMonitorAlgorithm.h"
#include "../TileRawChannelNoiseMonitorAlgorithm.h"
#include "../TileMuonFitMonitorAlgorithm.h"
#include "../TileRODMonitorAlgorithm.h"
#include "../TileTMDBDigitsMonitorAlgorithm.h"
#include "../TileTMDBMonitorAlgorithm.h"
#include "../TileTMDBRawChannelMonitorAlgorithm.h"
#include "../TileDigitsMonitorAlgorithm.h"

DECLARE_COMPONENT( TileFatherMonTool )
DECLARE_COMPONENT( TilePaterMonTool )
DECLARE_COMPONENT( TileCellMonTool )
DECLARE_COMPONENT( TileTowerMonTool )
DECLARE_COMPONENT( TileClusterMonTool )
DECLARE_COMPONENT( TileMuIdMonTool )
DECLARE_COMPONENT( TileL2MonTool )
DECLARE_COMPONENT( TileMuonFitMonTool )
DECLARE_COMPONENT( TileDigitsMonTool )
DECLARE_COMPONENT( TileRawChannelMonTool )
DECLARE_COMPONENT( TileDQFragMonTool )
DECLARE_COMPONENT( TileDQFragLWMonTool )
DECLARE_COMPONENT( TileRODMonTool )
DECLARE_COMPONENT( TileJetMonTool )
DECLARE_COMPONENT( TileDigiNoiseMonTool )
DECLARE_COMPONENT( TileTMDBDigitsMonTool )
DECLARE_COMPONENT( TileTMDBRawChannelMonTool )
DECLARE_COMPONENT( TileCellNoiseMonTool )
DECLARE_COMPONENT( TileRawChannelTimeMonTool )
DECLARE_COMPONENT( TileRawChannelNoiseMonTool )
DECLARE_COMPONENT( TileTBBeamMonTool )
DECLARE_COMPONENT( TileTBMonTool )
DECLARE_COMPONENT( TileTBCellMonTool )
DECLARE_COMPONENT( TileTBPulseMonTool )
DECLARE_COMPONENT( TileJetMonitorAlgorithm )
DECLARE_COMPONENT( TileDigitsFlxMonitorAlgorithm )
DECLARE_COMPONENT( TileDQFragMonitorAlgorithm )
DECLARE_COMPONENT( TileMBTSMonitorAlgorithm )
DECLARE_COMPONENT( TileRawChannelFlxMonitorAlgorithm )
DECLARE_COMPONENT( TileCellMonitorAlgorithm )
DECLARE_COMPONENT( TileTowerMonitorAlgorithm )
DECLARE_COMPONENT( TileClusterMonitorAlgorithm )
DECLARE_COMPONENT( TileMuIdMonitorAlgorithm )
DECLARE_COMPONENT( TileDigiNoiseMonitorAlgorithm )
DECLARE_COMPONENT( TileRawChannelTimeMonitorAlgorithm )
DECLARE_COMPONENT( TileRawChannelNoiseMonitorAlgorithm )
DECLARE_COMPONENT( TileMuonFitMonitorAlgorithm )
DECLARE_COMPONENT( TileRODMonitorAlgorithm )
DECLARE_COMPONENT( TileTMDBDigitsMonitorAlgorithm )
DECLARE_COMPONENT( TileTMDBMonitorAlgorithm )
DECLARE_COMPONENT( TileTMDBRawChannelMonitorAlgorithm )
DECLARE_COMPONENT( TileDigitsMonitorAlgorithm )
