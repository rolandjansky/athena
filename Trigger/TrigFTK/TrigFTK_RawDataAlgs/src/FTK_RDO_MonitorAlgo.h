/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RDO_MonitorAlgo_h
#define FTK_RDO_MonitorAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrkTrack/TrackCollection.h" //
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrkEventPrimitives/VertexType.h"
#include <vector>
#include <string>
#include <map>

#include "TH1D.h"
#include "TProfile.h"

/////////////////////////////////////////////////////////////////////////////

class AtlasDetectorID;
class Identifier;
class PixelID;
class SCT_ID;

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}


class FTK_RDO_MonitorAlgo: public AthAlgorithm {
public:
  FTK_RDO_MonitorAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTK_RDO_MonitorAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void Hist_Init(std::vector<TH1D*> *histograms);

  void fillMaps(const FTK_RawTrackContainer* rawTracks, std::vector<std::vector<unsigned int>*>& pixList, std::vector<std::vector<unsigned int>*>& sctList);
    const std::pair<unsigned int, unsigned int> matchTrack(const Trk::Track* track, 
							   std::vector<std::vector<unsigned int>*> &pixList, 
							   std::vector<std::vector<unsigned int>*> &sctList,
							   std::map<unsigned int,std::pair<double,double>>& offlinetrackPixLocxLocy,
							   std::map<unsigned int,double>& offlinetrackSctLocx,
							   bool& uniqueMatch);

  double getSctLocX(const IdentifierHash hash, const float stripCoord, const int clusterWidth);
  const std::pair<double,double> getPixLocXlocY(const IdentifierHash hash, const float rawLocalPhiCoord, const float rawLocalEtaCoord);
  void compareTracks(const FTK_RawTrack* ftkTrack, 
		     std::map<unsigned int,std::pair<double,double>>& offlinetrackPixLocxLocy,
		     std::map<unsigned int,double>& offlinetrackSctLocx);

 private:
  
  /// Tools and services ///
  ITHistSvc*    rootHistSvc;


  /// Track collections ///


  std::string m_ftk_raw_trackcollection_Name;
  std::string m_offlineTracksName;


  ServiceHandle<IFTK_DataProviderSvc> m_DataProviderSvc;

  const PixelID* m_pixelId;
  const SCT_ID*  m_sctId;
  
  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager*  m_SCT_Manager;

  const AtlasDetectorID* m_id_helper;

  /// Histograms ///
  TH1D* h_FTK_RawTrack_n;
  TH1D* h_FTK_pt;
  TH1D* h_FTK_eta;
  TH1D* h_FTK_phi;
  TH1D* h_offline_pt;
  TH1D* h_offline_eta;
  TH1D* h_offline_phi;
  TH1D*  h_IBL_dlocX_fullRange;
  TH1D*  h_IBL_dlocY_fullRange;
  TProfile* h_missingHits;
  TH1D* h_FTK_nHitMatch;
  TH1D* h_FTK_nTrackMatch;

  std::vector<TH1D*> h_pixb_dlocX;
  std::vector<TH1D*> h_pixb_dlocY;
  std::vector<TH1D*> h_pixe_dlocX;
  std::vector<TH1D*> h_pixe_dlocY;
  std::vector<TH1D*> h_sctb_0_dlocX;
  std::vector<TH1D*> h_sctb_1_dlocX;
  std::vector<TH1D*> h_scte_0_dlocX;
  std::vector<TH1D*> h_scte_1_dlocX;

  std::vector<TH1D*> h_ftk_sctb_dlocX;
  TH1D* h_ftk_sct_clusWidth;
  TH1D* h_ftk_pix_phiClusWidth;
  TH1D* h_ftk_pix_etaClusWidth;

  double m_minPt;
  double m_maxa0;
  double m_maxz0;
  double m_mineta;
  double m_maxeta;
  double m_minphi;
  double m_maxphi;
  unsigned int m_minMatches;
  bool m_reverseIBLlocx;
};

#endif // FTK_RDO_MonitorAlgo_h
