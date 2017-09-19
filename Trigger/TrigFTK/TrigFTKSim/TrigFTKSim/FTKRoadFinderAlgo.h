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
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKLogging.h"

#include <string>
#include <vector>
#include <ostream>

/////////////////////////////////////////////////////////////////////////////
class FTKRoadFinderAlgo: public AthAlgorithm, FTKLogger {
public:
  FTKRoadFinderAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKRoadFinderAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 protected:
  // to forward messages from FTKLogging to athena
  virtual void PostMessage(void);


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
  bool m_fixEndcapL0; //fix for endcap L0 in clustering

  bool m_ITkMode; // Use ITk geometry
  
  double m_ss_offset_fraction;

  int m_PixelClusteringMode; // clustering mode: 0 left edge, 1 center (also use ToT and correct for different lenght of pixels)
  bool m_Ibl3DRealistic; //If true, use HWlike clustering in IBL 3D mod
  int m_DuplicateGanged; // duplicate ganged pixels to remove efficiency loss
  int m_GangedPatternRecognition;  // pattern recognition to remove duplicates
  int m_SctClustering;       // SCT clustering: 0 disabled, 1 enabled
  bool m_read_clusters; //Read Clusters from FTKIP File? Default is false
  bool m_clustering; //Perform clustering? Default is true

  bool m_SCTTrkMode;
  std::string m_scttrk_tracklist;
  std::string m_scttrk_roadlist;
  std::string m_scttrk_sectormap_path;
  int m_scttrk_nlayers;
  int m_scttrk_nsubs;
  int m_scttrk_lastlayer;

  // set the use of the TSP DB banks
  bool m_useTSPBank;
  bool m_useCompressedBank;
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

  // decide splitting strategy for AM patterns with DC bits
  // default=0: no splitting
  // >0       : split AM patterns and optimize DC usage
  int m_setAMSplit;

  //max number of AM patterns after split, default=-1: no limits
  int m_maxAMAfterSplit; 

  //minimum threshold for DVol/DNPatt for split; default=0
  int m_minDVolOverDNPatt;  


 // store the variable if the TSP bank cache has to be saved
  bool m_doMakeCache;
  // path of the output file
  std::string m_CachePath;

  // save all found roads if >0
  int m_SaveAllRoads;

  // store all ss not just in roads
  bool m_StoreAllSS; 

  // read FTKHits directly from file
  bool m_read_FTKhits_directly;

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

  std::string m_modulelut_path; // LUT to map global ID in local IDs for HWModeSS=2
  std::string m_modulelut2nd_path; // LUT to map global ID in local IDs for HWMo

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

  bool m_useMinimalAMIN; // flag to propagate to the FTK_AMBank to set the match method

  int m_SectorAsPatterns; // flag to propagate the use of a list of sectors as pattern bank

  int m_DCMatchMethod; // flag to propagate to the FTKTSPMap classes

  bool m_AutoDisable; // possibility to avoid internal algorithm execution in particular confitions

  int m_firstEventFTK; // first event to run over

};

#endif // FTKRoadFinderAlgo_h
