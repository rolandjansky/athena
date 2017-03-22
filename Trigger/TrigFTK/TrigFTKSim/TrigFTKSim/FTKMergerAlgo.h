/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKMergerAlgo_h
#define FTKMergerAlgo_h

#include "PileUpTools/PileUpMergeSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TChain.h"

#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKPMap.h"

#include "TrigFTKPool/FTKAthTrack.h" 


#include <vector>
#include <string>
#include <map>


namespace Trk { class ITrackParticleCreatorTool; }
//namespace Trk { class TrackParticleCreatorTool; }

namespace Rec {
  class  TrackParticle;
  class TrackParticleContainer;
}

class ITrigFTKTrackConverter;
class ITrigFTKUncertiantyTool;

/* class VxContainer ; */

/////////////////////////////////////////////////////////////////////////////
class FTKMergerAlgo: public AthAlgorithm {
public:
  FTKMergerAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKMergerAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StoreGateSvc* m_StoreGate;
  StoreGateSvc* m_detStore;

  PileUpMergeSvc *m_mergeSvc; // Pile up service, used to merge sub-regions output

  // path for the plane-map information
  std::string m_pmap_path;
  FTKPlaneMap *m_pmap;

  bool m_useStandalone; // use input from standalone version instead of Athena
  bool m_singleProces; // assume the merger runs in a single process, after the trackfitter
  bool m_doMerging; // if "true" input is expected from regions/sub-regions and has to be merged

  bool m_MergeRoads; // if true also the roads will be merged, if the road merging is required but the roads are not found an error will be raised
  bool m_MergeRoadsDetailed; // if true roads will be merged and detailed info will be saved. if true, this overrides m_MergeRoads == false

  unsigned int m_nregions; // number of regions
  unsigned int m_nsubregions; // number of sub-regions (in each region)
  unsigned int m_neventsUnmergedFiles;// number of events in unmerged files 
  unsigned int m_neventsMerged;// number of events already unmerged 

  //
  //  Merged output
  //
  TTree *m_outputTree;
  TFile *m_outputFile;
  
  //HW parameter 
  int m_HW_level; // hit-warrior level
  unsigned int m_HW_ndiff; // maximum number of different points
  int m_NCoords; // NCoords
  float *m_HW_dev; //HW
  std::string m_HW_path; // HW configuration path
  int m_keep_rejected; // >0 keep rejected roads (1 HW rej, 2 bad quality)

  int m_SCTCoordOffset; // offset for the SCT pixel coordinates

  // vectors with information for the merging
  // paths with the merged tracks
  std::vector< std::string > m_ftktrack_tomerge_paths;
  int m_ftktrack_tomerge_firstregion;
  int m_ftktrack_tomerge_firstsubreg;
  std::string m_ftrtrack_tomerge_bfname;
  std::string m_ftkroad_tomerge_bfname;
  int m_MergeRegion;
  unsigned int m_nStreamsToMerge;
  TTree ***m_ftktrack_tomerge_tree;  
  TFile ***m_ftktrack_tomerge_file;
  TBranch ***m_ftktrack_tomerge_branch;
  FTKTrackStream ***m_ftktrack_tomerge_stream;  
  std::string m_ftktrack_mergeoutput;
  std::string m_ftktrack_mergeInputPath;
  std::string m_ftktrack_mergeFileRoot;

  TBranch ***m_ftkroad_tomerge_branch;
  FTKTrackStream ***m_ftkroad_tomerge_stream;
  std::string m_ftkroad_mergeoutput;
  std::string m_ftkroad_mergeInputPath;
  std::string m_ftkroad_mergeFileRoot;

  //
  //  Require all inputs files to be present
  //    if missing one:
  //       - fail if true
  //       - print warning continue if false
  bool m_forceAllInput;

  //
  //   Use naming as done on the grid 
  //
  bool m_doGrid;

  // paths with the merged tracks
  std::vector< std::string > m_ftktrack_paths_merged;
  // chain  of ftk merged tracks
  TChain *m_mergedtracks_chain;
  TTree *m_mergedtracks_tree;
  // streams with the merged tracks;
  FTKTrackStream *m_mergedtracks_stream;
  FTKRoadStream ***m_srbanks; // old roads
  FTKRoadStream **m_banks; // new roads
  FTKTrackStream **m_merged_bank;


  std::string m_mergedtracks_bname; // name to be used for the merged tracks' branch name

  // merged roads counter
  unsigned long m_mergedtracks_iev;
  StatusCode convertMergedTracks();

  StatusCode mergeSGContent();

  StatusCode mergePoolFiles();

  StatusCode initStandaloneTracks();
  StatusCode mergeStandaloneTracks();
  StatusCode finalizeStandaloneTracks();
  StatusCode finalizeCopyTruthTree();

  //remove track function
  std::list<FTKAthTrack>::iterator removeTrack(std::list<FTKAthTrack>&, std::list<FTKAthTrack>::iterator, FTKAthTrack&, const FTKAthTrack&,bool isnew=false);

  // merging functions for the standalong tracks 
  int m_nfits_rej;
  void merge_tracks(FTKTrackStream *&, FTKTrackStream ***, int ireg);
  std::list<FTKTrack>::iterator removeTrack(std::list<FTKTrack>&, std::list<FTKTrack>::iterator, FTKTrack&, const FTKTrack&,bool isnew=false);

   StatusCode merge_roads(FTKRoadStream *&,FTKRoadStream **, unsigned int, unsigned int);

  //
  // Format the input file names
  //
  std::string GetPathName(unsigned int ireg, unsigned int isub);

  // disable the event synchronization and other checks
  bool m_force_merge;




  // collection of FTK tracks for the LVL2 use
  std::string m_out_indettrack_Name;
  TrigInDetTrackCollection *m_out_indettrack;

  // collection of FTK clusters
  std::string m_FTKPxlClu_CollName; // name of the collection
  InDet::PixelClusterContainer *m_FTKPxlCluContainer; // pixel container object
  std::string m_FTKSCTClu_CollName; // name of the collection
  InDet::SCT_ClusterContainer *m_FTKSCTCluContainer; // SCT container object
  AtlasDetectorID* m_idHelper;
  const InDetDD::SiDetectorManager*     m_PIX_mgr;
  const InDetDD::SiDetectorManager*     m_SCT_mgr;
  const PixelID *m_pixel_id;
  const SCT_ID *m_sct_id;

   std::vector<int> zerovec;


  // Trk::Track
  std::string m_out_trktrack_Name;
  TrackCollection *m_out_trktrack;


  std::string m_out_ftktrackconv_Name;
  TrackCollection *m_out_ftktrackconv;

  // TrackParticle type container control for the refitted tracks
  std::string m_out_ftktrackparticleconv_Name;
  Rec::TrackParticleContainer *m_out_ftktrackparticleconv;

  // TrigInDetTrack collection for the refitted tracks
  std::string m_out_indettrackconv_Name;
  TrigInDetTrackCollection *m_out_indettrackconv;

  ToolHandle< ITrigFTKTrackConverter >  m_trackConverterTool;

  //
  //  Tool to get the FTK uncertainties
  //
  ToolHandle< ITrigFTKUncertiantyTool > m_uncertiantyTool;

  
  //
  //  Tool to make the track summaries
  //
  ToolHandle<Trk::ITrackSummaryTool>    m_trackSumTool;


  //collection of FTK Track Particle 
  //ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
  //  Trk::ITrackParticleCreatorTool*  m_particleCreatorTool;
  //Rec::TrackParticle * aParticle;

  std::string                  m_out_trackPC_Name;
  Rec::TrackParticleContainer*      m_out_trackPC;
  const VxContainer*                m_primcontainer;
  std::string m_vxCandidatesPrimaryName;

  std::vector< std::string > m_truthFileNames; // if not blank, vector of paths of files with truth info in it, to store in output
  std::string m_truthTrackTreeName; // if not = "", name of truth track tree, to store in output
  std::string m_evtinfoTreeName; // if not = "", name of evtinfo tree, to store in output
  std::string m_offlineTreeName; // if not = "", name of offline tree, to store in output
  int m_saveTruthTree;
  std::string                  m_out_convTrackPC_Name;
  Rec::TrackParticleContainer*      m_out_convTrackPC;

  // map containing for each event the entry where the tracks 
  // found by the FTK (the event number is used as a key to 
  // address differences in the event sequence)
  std::map< int, Long64_t > m_trackVectorMap;

  //
  //  Convert from Trk::Track to TrackParticle
  //
  Rec::TrackParticle* createTrackParticle(const Trk::Track* ,const Trk::VxCandidate* ,Trk::TrackParticleOrigin, unsigned int bitmask=0);
  Rec::TrackParticle* createTrackParticle(const Trk::Track* ,const Trk::VxCandidate* ,Trk::TrackParticleOrigin, const Trk::TrackSummary*);

  //
  //  Helpers for the covariance matrix
  //
  double getSigmaQoverP(double invpt, double sigmaTwoInvPt, double eta,  double sigmaEta);
  double getSigmaTheta (double eta,   double sigmaEta);

};

#endif // FTKMergerAlgo_h
