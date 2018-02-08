/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigFTKSim/FTKRoadFinderAlgo.h"
#include "TrigFTKSim/FTKDumpCondAlgo.h"
#include "TrigFTKSim/FTKTrackFitterAlgo.h"
#include "TrigFTKSim/FTKMergerAlgo.h"
#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/FTK_SGRoadOutput.h"
#include "TrigFTKSim/FTK_SGTrackOutput.h"
#include "TrigFTKSim/FTK_RoadMarketTool.h"
#include "TrigFTKSim/FTKDetectorTool.h"

DECLARE_ALGORITHM_FACTORY( FTKRoadFinderAlgo )
DECLARE_ALGORITHM_FACTORY( FTKDumpCondAlgo )
DECLARE_ALGORITHM_FACTORY( FTKTrackFitterAlgo )
DECLARE_TOOL_FACTORY( FTK_SGHitInput )
DECLARE_TOOL_FACTORY( FTK_SGRoadOutput )
DECLARE_TOOL_FACTORY( FTK_SGTrackOutput )
DECLARE_TOOL_FACTORY( FTK_RoadMarketTool )
DECLARE_TOOL_FACTORY( FTKDetectorTool )
DECLARE_ALGORITHM_FACTORY( FTKMergerAlgo )

DECLARE_FACTORY_ENTRIES(TrigFTKSim) {
  DECLARE_ALGORITHM( FTKRoadFinderAlgo );
  DECLARE_ALGORITHM( FTKDumpCondAlgo );
  DECLARE_ALGORITHM( FTKTrackFitterAlgo );
  DECLARE_TOOL( FTK_SGHitInput );
  DECLARE_TOOL( FTK_SGRoadOutput );
  DECLARE_TOOL( FTK_SGTrackOutput );
  DECLARE_TOOL( FTK_RoadMarketTool );
  DECLARE_TOOL( FTKDetectorTool );
  DECLARE_ALGORITHM( FTKMergerAlgo );
}
