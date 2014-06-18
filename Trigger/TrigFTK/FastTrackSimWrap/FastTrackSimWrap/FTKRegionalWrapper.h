/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKRegionalWrapper_h
#define FTKRegionalWrapper_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKPMap.h"

#include "TFile.h"
#include "TTree.h"

#include <string>

class FTKRegionalWrapper : public AthAlgorithm {
public:
  FTKRegionalWrapper (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKRegionalWrapper ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle<FTK_SGHitInputI> m_hitInputTool; // input handler

  // variables to manage the distribution of the hits
  bool m_IBLMode; // global FTK setup variable to handle IBL
  std::string m_pmap_path; // path of the PMAP file
  FTKPlaneMap *m_pmap; // pointer to the pmap object

  // variables to manage the region maps
  std::string m_rmap_path; // path of the region-map file
  FTKRegionMap *m_rmap; // pointer to the RMAP object
  int m_ntowers;

  bool m_SaveRawHits; // flag to allow to store FTKRawHit collections, pmap non applied
  bool m_SaveHits; // flag to allow to store FTKHit collections, pmap applied

  /* the following flags were copied from TrigFTKSim/FTKDataInput.h on 11/02/14 check they are still up to date. Thanks, Guido*/
  bool m_Clustering; // flag to enable the clustering
  bool m_SaveClusterContent; // flag to allow the debug of the cluster content
  bool m_DiagClustering; // true if diagonal clustering is used
  bool m_SctClustering; // true if SCT clustering is performed
  unsigned int m_PixelClusteringMode; // 1 means ToT information is used
                                      //   && 400/600um pixels are accounted for
                                      // 0 is default used up to 2013
  bool m_DuplicateGanged;
  bool m_GangedPatternRecognition;

  // variables related to the output fiels
  std::string m_outpath; // output path
  TFile *m_outfile; // ROOT file descriptor
  
  TTree *m_hittree; // TTree for the hit storage
  std::vector<FTKRawHit> *m_original_hits; // variables related to the FTKRawHit storage
  std::vector<FTKHit> *m_logical_hits; // variables related to the FTKHit storage

  TTree *m_evtinfo; // TTree with general event information
  int m_run_number; // event's run number
  int m_event_number; // event number

  TTree *m_trackstree;
  std::vector<FTKTruthTrack> m_truth_tracks;
};

#endif // FTKRegionalWrapper_h
