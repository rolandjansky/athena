/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKDumpCondAlgo_h
#define FTKDumpCondAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"

//#include "TrigFTKSim/FTK_SGHitInput.h"
//#include "TrigFTKSim/FTK_SGRoadOutput.h"
//#include "TrigFTKSim/FTK_RoadMarketTool.h"
#include "TrigFTKSim/FTKDetectorTool.h" 

//#include "TrigFTKSim/DumpCond.h"
//#include "TrigFTKSim/FTK_AMBank.h"
//#include "TrigFTKSim/tsp/FTKTSPBank.h"

#include <string>

/////////////////////////////////////////////////////////////////////////////
class FTKDumpCondAlgo: public AthAlgorithm {
public:
  FTKDumpCondAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKDumpCondAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  ToolHandle<FTKDetectorToolI> m_detectorTool; // detector status handler

  // options
  bool m_DumpBadModules;
  bool m_DumpModuleIDMap;
  bool m_DumpGlobalToLocalMap;

  bool m_DumpIDMap;

  bool m_DumpModulePositions;

  int m_IBLMode;
  bool m_ITkMode;
  bool m_fixEndcapL0; //fix for endcap L0 in clustering

};

#endif // FTKDumpCondAlgo_h
