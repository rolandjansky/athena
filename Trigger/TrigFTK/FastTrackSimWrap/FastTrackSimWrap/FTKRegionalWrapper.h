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

#include <stdint.h>

class IPixelCablingSvc; 
class ISCT_CablingSvc;
class StoreGateSvc; 
class PixelID; 
class SCT_ID;
class SCT_OnlineId;
class IdentifierHash;

class FTKRegionalWrapper : public AthAlgorithm {
public:
  FTKRegionalWrapper (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKRegionalWrapper ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle<FTK_SGHitInputI> m_hitInputTool; // input handler
  ServiceHandle<IPixelCablingSvc> m_pix_cabling_svc; 
  ServiceHandle<ISCT_CablingSvc> m_sct_cabling_svc;

  // Needed to retrieve m_pixelId in order to get the barrel_ec, phi/eta_modules etc.
  StoreGateSvc*  m_storeGate;
  StoreGateSvc*  m_detStore;
  StoreGateSvc*  m_evtStore;
  const PixelID * m_pixelId;
  const SCT_ID * m_sctId;

  // variables to manage the distribution of the hits
  bool m_IBLMode; // global FTK setup variable to handle IBL
  std::string m_pmap_path; // path of the PMAP file
  FTKPlaneMap *m_pmap; // pointer to the pmap object

  // variables to manage the region maps
  std::string m_rmap_path; // path of the region-map file
  FTKRegionMap *m_rmap; // pointer to the RMAP object
  int m_ntowers;
  int m_nplanes;
  
  bool m_SaveRawHits; // flag to allow to store FTKRawHit collections, pmap non applied
  bool m_SaveHits; // flag to allow to store FTKHit collections, pmap applied
  bool m_SavePerPlane; // flag to allow to store per-plane TTree
  
  bool m_DumpTestVectors; // flag to allow to dump test vectors

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
  TTree *m_hittree_perplane; // TTree for the hit storage  
  
  std::vector<FTKRawHit> *m_original_hits; // variables related to the FTKRawHit storage
  std::vector<FTKHit> *m_logical_hits; // variables related to the FTKHit storage
  
  std::vector<FTKRawHit> **m_original_hits_per_plane; // variables related to the FTKRawHit storage
  std::vector<FTKHit> **m_logical_hits_per_plane; // variables related to the FTKHit storage
  
  TTree *m_evtinfo; // TTree with general event information
  int m_run_number; // event's run number
  int m_event_number; // event number

  TTree *m_trackstree;
  std::vector<FTKTruthTrack> m_truth_tracks;
  // Variables created by dumpFTKTestvectors via initialize()
  std::vector<IdentifierHash> m_identifierHashList;
  std::vector<uint32_t> m_pix_rodIdlist;  
  std::vector<uint32_t> m_sct_rodIdlist; 

  bool dumpFTKTestVectors(FTKPlaneMap *pmap, FTKRegionMap *rmap);

};

#endif // FTKRegionalWrapper_h
