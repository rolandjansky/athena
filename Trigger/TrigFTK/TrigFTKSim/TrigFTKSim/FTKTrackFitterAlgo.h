/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTrackFitterAlgo_h
#define FTKTrackFitterAlgo_h


#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigFTKSim/FTK_RoadMarketTool.h"
#include "TrigFTKSim/FTK_SGTrackOutput.h"

#include "TrigFTKSim/FTKSector711DB.h"
#include "TrigFTKSim/ftktypes.h"
#include "TrigFTKSim/TrackFitter.h"
#include "TrigFTKSim/TrackFitter711.h"

#include <string>
#include <vector>
#include <ostream>

/////////////////////////////////////////////////////////////////////////////
class FTKTrackFitterAlgo: public AthAlgorithm {
public:
  FTKTrackFitterAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKTrackFitterAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  TrackFitter * m_tfpobj; // instance of the TrackFitter object

  ToolHandle<FTK_RoadMarketToolI> m_roadMarketTool; // road input handler
  ToolHandle<FTK_SGTrackOutputI> m_trackOutputTool; // track output handler
  ToolHandle<FTK_SGTrackOutputI> m_trackOutputTool_pre_hw; // track output handler

  // Second stage fitter flag
  bool m_SecondStageFit;

  // enable the use of IBL
  int m_IBLMode;

  bool m_fixEndcapL0; //fix for endcap L0 in clustering


  // use ITk geometry
  bool m_ITkMode;
  
  // Number of regions/banks
  int m_nbanks;
  int m_nsubregions;
  int m_verbosity;
  
  // for set parameter
  int m_chi2cut;
  int m_chi2cut_maj;
  int m_chi2cut_vetmaj;
  int m_chi2dofcut;

  bool m_doAuxFW;
 
  int m_HitWarrior;
  int m_HitWarrior_first;
  int m_KeepRejected; 
  int m_FitRemoved;
  int m_DoMajority;
  int m_OnePerRoad;
  int m_HWNDiff;
  int m_MaxNcomb;
  int m_MaxNhitsPerPlane;
  int m_MaxTrkout;
  int m_NoRecoveryNHits;
  
  bool m_SCTTrkMode;

  std::string m_loadHWConf_path;

  // second stage fit specific properties
  int m_SSF_TFMode;
  bool m_SSF_multiconn;
  int m_SSF_maxnconn;
  bool m_SSF_allow_extramiss;
  int m_SSF_check_TR_by;
  float m_SSF_TR_min_eta;
  float m_SSF_TR_max_eta;

  bool m_save_1stStageTrks;

  //output
  bool m_doTrackFile;
  bool m_addRoads;
  std::string m_trackfilepath;
  std::string m_trackfilename;

  // Plane map pointer, to be set during the initialization
  FTKPlaneMap *m_pmap;
  std::string m_pmap_path;
  FTKPlaneMap *m_pmap_complete;
  std::string m_pmapcomplete_path;
  FTKPlaneMap *m_pmap_unused;
  std::string m_pmapunused_path;
  std::string m_modulelut2nd_path; // LUT to map global ID in local IDs for HWModeSS=2          

  std::vector<std::string> m_fitconstantspath;
  std::vector<std::string> m_fit711constantspath;
  std::vector<std::string> m_sectorpath;
  std::vector<int> m_bankregion;
  std::vector<int> m_banksubregion;

  FTKRegionMap *m_rmap;
  FTKRegionMap *m_rmap_unused;
  FTKSSMap *m_ssmap;
  FTKSSMap *m_ssmap_unused;

  std::string m_rmap_path;
  std::string m_ssmap_path;
  std::string m_ssmapunused_path;

  bool m_AutoDisable;
  bool m_PrintSSBConstants;  
  double m_dTIBL; // dT (in K) for IBL compared to reference. For use in correcting for IBL bowing in x(phi) direction
};

#endif // FTKTrackFitterAlgo_h
