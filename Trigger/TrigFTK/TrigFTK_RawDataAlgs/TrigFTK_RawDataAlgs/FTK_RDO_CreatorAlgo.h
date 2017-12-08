/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RDO_CreatorAlgo_h
#define FTK_RDO_CreatorAlgo_h

#include "PileUpTools/PileUpMergeSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TChain.h"

#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTKSim/FTKPMap.h"

#include "TrigFTKPool/FTKAthTrack.h" 

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include <vector>
#include <string>
#include <map>

class AtlasDetectorID;
class PixelID;
class SCT_ID;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}




/////////////////////////////////////////////////////////////////////////////
class FTK_RDO_CreatorAlgo: public AthAlgorithm {
public:
  FTK_RDO_CreatorAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTK_RDO_CreatorAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StoreGateSvc* m_StoreGate;
  StoreGateSvc* m_detStore;

  // paths with the merged tracks
  std::vector< std::string > m_ftktrack_paths_merged;
  // chain  of ftk merged tracks
  TChain *m_mergedtracks_chain;
  TTree *m_mergedtracks_tree;
  // streams with the merged roads
  FTKTrackStream *m_mergedtracks_stream;
  std::string m_mergedtracks_bname; // name to be used for the merged tracks' branch name

  // merged roads counter
  unsigned long m_mergedtracks_iev;

  std::string m_ftk_raw_trackcollection_Name;
  FTK_RawTrackContainer *m_ftk_raw_trackcollection;

  bool m_check;
  unsigned int m_NcheckFail;
  std::vector<unsigned int> m_FTK_RawTrack_checkFails;
  std::vector<unsigned int> m_FTK_RawSCT_Cluster_checkFails;
  std::vector<unsigned int> m_FTK_RawPixelCluster_checkFails;

  // map containing for each event the entry where the tracks 
  // found by the FTK (the event number is used as a key to 
  // address differences in the event sequence)
  std::map< int, Long64_t > m_trackVectorMap;


  const PixelID* m_pixelId;
  const SCT_ID*  m_sctId;

  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager*  m_SCT_Manager;

  const AtlasDetectorID* m_id_helper;


  double m_trainingBeamspotX;
  double m_trainingBeamspotY;
  double m_trainingBeamspotZ;
  double m_trainingBeamspotTiltX;
  double m_trainingBeamspotTiltY;
  bool m_reverseIBLlocx;

  // Convertor for RDO objects 
  FTK_RawTrack* SimToRaw(const FTKTrack&);
  bool check_track(const FTKTrack &track, FTK_RawTrack &rdo);
  bool checkValue(const float &v1, const float &v2, const float &res, const std::string &what); 
  bool checkInt(const unsigned int &v1, const unsigned int &v2, const std::string &what); 
  void printBits(unsigned int num, unsigned int length);
  void printTrack(const FTKTrack& track, const FTK_RawTrack *raw_track);


};

#endif // FTK_RDO_CreatorAlgo_h
