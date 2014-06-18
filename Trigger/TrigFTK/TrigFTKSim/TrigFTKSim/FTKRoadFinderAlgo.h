/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKRoadFinderAlgo_h
#define FTKRoadFinderAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/FTK_SGRoadOutput.h"
#include "TrigFTKSim/FTK_RoadMarketTool.h"

#include "TrigFTKSim/RoadFinder.h"
#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"

#include <string>
#include <vector>
#include <ostream>

/////////////////////////////////////////////////////////////////////////////
class FTKRoadFinderAlgo: public AthAlgorithm {
public:
  FTKRoadFinderAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKRoadFinderAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  RoadFinder m_rfobj; // instance of the RoadFinder object

  ToolHandle<FTK_SGHitInputI> m_hitInputTool; // input handler
  ToolHandle<FTK_SGRoadOutputI> m_roadOutputTool; // road output handler
  ToolHandle<FTK_RoadMarketToolI> m_roadMarketTool; // road output handler

  // Number of regions/banks
  int m_nbanks;
  int m_nsubregions;

  int m_verbosity;
  bool m_BarrelOnly;
  bool m_EnableFTKSim;
  int m_MaxMissingPlanes;
  int m_RoadWarrior;
  int m_KeepRemoved;

  int m_MaxMissingSCTPairs; // increase the number of missing SCT modules, enable the transition region fix
  bool m_RestrictSctPairModule; // restrict SCT pairs modules in the transition region
  bool m_RestrictSctPairLayer; // restrict SCT layers in the transition region

  int m_IBLMode; // Enable the use of IBL
  
  double m_ss_offset_fraction;

  int m_PixelClusteringMode; // clustering mode: 0 left edge, 1 center (also use ToT and correct for different lenght of pixels)
  int m_SctClustering;       // SCT clustering: 0 disabled, 1 enabled

  bool m_SCTTrkMode;
  std::string m_scttrk_tracklist;
  std::string m_scttrk_roadlist;
  std::string m_scttrk_sectormap_path;
  int m_scttrk_nlayers;
  int m_scttrk_nsubs;
  int m_scttrk_lastlayer;

  // set the use of the TSP DB banks
  bool m_useTSPBank;
  // if the TSP bank is used it is possible to ask a specific level
  int m_BankLevel;
  // set if the TSP simulation is performed, 0 not used
  int m_doTSPSim;
  // minimum TSP coverage, if >1 the way the AM bank is built change
  int m_minTSPCoverage;
  /* set a variable to decide if the number of patterns has to be
     done at TSP level (0) or at AM level. When the AM level is used
     the pattern bank can stop when the number is reached (1) or untill
     the number is not exceeded (2) */
  int m_setAMSize;
 // store the variable if the TSP bank cache has to be saved
  bool m_doMakeCache;
  // path of the output file
  std::string m_CachePath;

  // save all found roads if >0
  int m_SaveAllRoads;

  // Plane map pointer, to be set during the initialization
  FTKPlaneMap *m_pmap;
  FTKPlaneMap *m_pmap_unused;
  // region map used in this session
  FTKRegionMap *m_rmap;
  FTKRegionMap *m_rmap_unused;
  int m_CUR_REGION;
  // Super-strip map, this object transform an hit in a SS index
  FTKSSMap *m_ssmap;
  FTKSSMap *m_ssmap_unused;
  FTKSSMap *m_ssmap_tsp;
  // require presence of first layer (pixel B-layer)
  bool m_require_first;
  // lists containing output of 8L run
  std::string scttrk_tracklist;
  std::string scttrk_roadlist;
  // 4L,8L -> 11L lookup map
  FTKSectorMap *m_scttrk_sectormap;
  
  // HW-like SS encoding falg
  unsigned int m_HWModeSS;

  std::string m_pmap_path;
  std::string m_pmapunused_path;
  std::string m_rmap_path;
  std::string m_rmapunused_path;
  std::string m_ssmap_path;
  std::string m_ssmapunused_path;
  std::string m_ssmaptsp_path;
  std::string m_badmap_path;
  std::string m_badmap_path2;

  bool m_CachedBank;
  std::vector<std::string> m_patternbankpath;
  std::vector<int> m_bankregion;
  std::vector<int> m_banksubreg;
  std::vector<int> m_bankpatterns;

  bool m_InputFromWrapper;
  bool m_RegionalWrapper;
  std::vector<std::string> m_wrapperpaths;

  bool m_doroadfile;
  std::string m_roadfilepath;
  std::string m_roadfilesdir;
  bool m_roadmarket;
  bool m_saveroads;

  bool m_AutoDisable; // possibility to avoid internal algorithm execution in particular confitions
};

#endif // FTKRoadFinderAlgo_h
