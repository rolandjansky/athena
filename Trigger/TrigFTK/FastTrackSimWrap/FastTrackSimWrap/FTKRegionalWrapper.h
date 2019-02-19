/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKRegionalWrapper_h
#define FTKRegionalWrapper_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "TrigFTKToolInterfaces/ITrigFTKClusterConverterTool.h"
#include "TrigFTKTrackConverter/TrigFTKClusterConverterTool.h"

#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKPMap.h"

#include "TFile.h"
#include "TTree.h"

#include <string>

#include <stdint.h>

class IPixelCablingSvc; 
class StoreGateSvc; 
class PixelID; 
class AtlasDetector;
class SCT_ID;
class SCT_OnlineId;
class IdentifierHash;
class ITrigFTKClusterConverterTool;

namespace InDetDD {
  class PixelDetectorManager;
}

class FTKRegionalWrapper : public AthAlgorithm {
public:
  FTKRegionalWrapper (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKRegionalWrapper ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode initOutputFile();

private:
  ToolHandle<FTK_SGHitInputI> m_hitInputTool; // input handler
  ToolHandle<ITrigFTKClusterConverterTool>  m_clusterConverterTool; /** Tool to convert FTKHits to IDClusters */

  ServiceHandle<IPixelCablingSvc> m_pix_cabling_svc; 
  ToolHandle<ISCT_CablingTool> m_sct_cablingToolInc; // This class accesses SCT cabling during initialization.

  // Needed to retrieve m_pixelId in order to get the barrel_ec, phi/eta_modules etc.


  StoreGateSvc*  m_storeGate;
  StoreGateSvc*  m_detStore;
  StoreGateSvc*  m_evtStore;
  const PixelID * m_pixelId;
  const SCT_ID * m_sctId;
  const AtlasDetectorID* m_idHelper;
  const InDetDD::PixelDetectorManager*  m_PIX_mgr;

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  // variables to manage the distribution of the hits
  int m_IBLMode; //  global FTK setup variable to handle IBL
  bool m_fixEndcapL0; //fix for endcap L0 in clustering
  bool m_ITkMode; //  global FTK setup variable to toggle ITk geometry
  std::string m_pmap_path; //  path of the PMAP file
  FTKPlaneMap *m_pmap; //  pointer to the pmap object

  // variables to manage the region maps
  std::string m_rmap_path; //  path of the region-map file
  FTKRegionMap *m_rmap; //  pointer to the RMAP object
  int m_ntowers;
  int m_nplanes;
 
  bool m_SaveRawHits; //  flag to allow to store FTKRawHit collections, pmap non applied
  bool m_SaveHits; //  flag to allow to store FTKHit collections, pmap applied
  bool m_SavePerPlane; //  flag to allow to store per-plane TTree
  
  bool m_DumpTestVectors; //  flag to allow to dump test vectors

  bool m_EmulateDF; /** Flag to allow output FTK_IP to only include modules 
			which come froms specific RodIDs.  Specify Rod Id
		    with the  m_pix_rodIdlist,m_sct_rodIdlist variables*/

  /* the following flags were copied from TrigFTKSim/FTKDataInput.h on 11/02/14 check they are still up to date. Thanks, Guido*/
  bool m_Clustering; //  flag to enable the clustering
  bool m_SaveClusterContent; //  flag to allow the debug of the cluster content
  bool m_DiagClustering; //  true if diagonal clustering is used
  bool m_SctClustering; //  true if SCT clustering is performed
  unsigned int m_PixelClusteringMode; /*  1 means ToT information is used
                                          && 400/600um pixels are accounted for
					  0 is default used up to 2013 */
  bool m_Ibl3DRealistic; //If true, use HWlike clustering in IBL 3D mod
  bool m_DuplicateGanged;
  bool m_GangedPatternRecognition;
  bool m_WriteClustersToESD;  //write the clusters to the ESD

  bool m_getOffline;
  std::string m_offlineName;

  // variables related to the output fiels
  std::string m_outpath; // output path
  TFile *m_outfile; // ROOT file descriptor
  
  TTree *m_hittree; // TTree for the hit storage
  TTree *m_hittree_perplane; // TTree for the hit storage  
  
  std::vector<FTKRawHit> *m_original_hits; // variables related to the FTKRawHit storage
  std::vector<FTKHit> *m_logical_hits; // variables related to the FTKHit storage
  
  std::vector<FTKRawHit> **m_original_hits_per_plane; // variables related to the FTKRawHit storage
  std::vector<FTKHit> **m_logical_hits_per_plane; // variables related to the FTKHit storage
  
  TTree *m_evtinfo; /** TTree with general event information */
  int m_run_number; /** event's run number */
  int m_event_number; /** event number */
  float m_averageInteractionsPerCrossing;
  float m_actualInteractionsPerCrossing;
  int m_LB;
  int m_BCID;
  unsigned int m_extendedLevel1ID;
  unsigned int m_level1TriggerType;
  std::vector<unsigned int> m_level1TriggerInfo;

  TTree *m_trackstree;
  std::vector<FTKTruthTrack> m_truth_tracks;
  // Variables created by dumpFTKTestvectors via initialize()
  std::vector<IdentifierHash> m_identifierHashList;

  std::vector<uint32_t> m_pix_rodIdlist;  /** List of RodIDs to be used to emulate DF output*/
  std::vector<uint32_t> m_sct_rodIdlist;  /** List of RodIDs to be used to emulate DF output*/

  std::vector<std::string> m_spix_rodIdlist;  /** List of RodIDs to be used to emulate DF output*/
  std::vector<std::string> m_ssct_rodIdlist;  /** List of RodIDs to be used to emulate DF output*/

  bool dumpFTKTestVectors(FTKPlaneMap *pmap, FTKRegionMap *rmap);


  //Added for cluster conversion
  std::string m_FTKPxlClu_CollName;  /** default name for the FTK pixel ID Cluster container */
  InDet::PixelClusterContainer *m_FTKPxlCluContainer;   /**  FTK pixel ID Cluster container */
  std::string m_FTKSCTClu_CollName;  /** default name for the FTK sct ID Cluster container */
  InDet::SCT_ClusterContainer *m_FTKSCTCluContainer;   /**  FTK pixel ID Cluster container */


  PRD_MultiTruthCollection* m_ftkPixelTruth; /** FTK Pixel ID Truth collection */
  PRD_MultiTruthCollection* m_ftkSctTruth; /** FTK SCT ID Truth collection */
  const McEventCollection*  m_mcEventCollection; /** Truth collection */

  std::string m_ftkPixelTruthName;  /** name of FTK Pixel ID Truth collection */
  std::string m_ftkSctTruthName; /** name of FTK SCT ID Truth collection */
  std::string m_mcTruthName;  /** Truth collection name */

  std::vector<int> m_L1ID_to_save; // vector of L1ID to save, if empty save all

  //offline clusters
  std::vector<float>   *m_offline_locX;
  std::vector<float>   *m_offline_locY;
  std::vector<int>     *m_offline_isPixel;
  std::vector<int>     *m_offline_isBarrel;
  std::vector<int>     *m_offline_layer;
  std::vector<int>     *m_offline_resAssociatedTrack;
  std::vector<int>     *m_offline_clustID;
  std::vector<int>     *m_offline_trackNumber;

  //offline tracks
  std::vector<float>   *m_offline_pt;
  std::vector<float>   *m_offline_eta;
  std::vector<float>   *m_offline_phi;


  TTree *m_offline_cluster_tree;

};

#endif // FTKRegionalWrapper_h
