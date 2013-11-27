/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArGeoH62004Algs/LArTBH6TriggerTimeTool.h"
//#include "LArGeoH62004Algs/LArTBH6CellCorrTool.h"
//#include "LArGeoH62004Algs/TBClusterMaker.h"

DECLARE_TOOL_FACTORY( LArTBH6TriggerTimeTool );
//DECLARE_TOOL_FACTORY( LArTBH6CellCorrTool );
//DECLARE_TOOL_FACTORY( TBClusterMaker );

DECLARE_FACTORY_ENTRIES(LArG4TBH6Tools) {
   DECLARE_ALGORITHM(LArTBH6BeamInfo);
//   DECLARE_ALGORITHM(CBNT_TBTrackInfo);
   DECLARE_TOOL( LArTBH6TriggerTimeTool );
//   DECLARE_TOOL( LArTBH6CellCorrTool );
//   DECLARE_TOOL( TBClusterMaker );
}
