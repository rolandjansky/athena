/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RDO_ReaderAlgo_h
#define FTK_RDO_ReaderAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrkTrack/TrackCollection.h" //
#include "TrkToolInterfaces/IUpdator.h"
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
#include "TTree.h"

/////////////////////////////////////////////////////////////////////////////
class AtlasDetectorID;
class Identifier;
class PixelID;
class SCT_ID;

namespace Trk {
  class IResidualPullCalculator;
}  

class FTK_RDO_ReaderAlgo: public AthAlgorithm {
public:
  FTK_RDO_ReaderAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTK_RDO_ReaderAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void Hist_Init(std::vector<TH1D*> *histograms);
  void Tree_Init();
  void Fill_Raw_Tracks();
  void Fill_Converted_Tracks();
  void Fill_Refit_Tracks();
  void Fill_Offline_Tracks();
  void Fill_Raw_Vertices_fast(unsigned int track_requirement);
  void Fill_Refit_Vertices_fast(unsigned int track_requirement);
  void Fill_Converted_Vertices(unsigned int track_requirement);
  void Fill_Refit_Vertices(unsigned int track_requirement);
  void Fill_Offline_Vertices(unsigned int track_requirement);
  void Fill_Truth_Vtx();
  void Fill_Clusters(TrackCollection *trackCollection,std::vector<float> *x_residual,std::vector<float> *y_residual,std::vector<float> *x_loc,std::vector<float> *y_loc,std::vector<bool> *is_Pixel,std::vector<bool> *is_Barrel,std::vector<bool> *is_SCT,std::vector<int> *Layer,std::vector<int> *resAssociatedTrack,std::vector<int> *clustID);
  void Fill_Clusters(const xAOD::TrackParticleContainer *trackCollection,std::vector<float> *x_residual,std::vector<float> *y_residual,std::vector<float> *x_loc,std::vector<float> *y_loc,std::vector<bool> *is_Pixel,std::vector<bool> *is_Barrel,std::vector<bool> *is_SCT,std::vector<int> *Layer,std::vector<int> *resAssociatedTrack,std::vector<int> *clustID);

private:

  std::string strVertexType( const Trk::VertexType vxtype);
  std::string strVertexType( const xAOD::VxType::VertexType vxtype);
  

  /// Tools and services ///
  ITHistSvc*    rootHistSvc;
  StoreGateSvc* m_StoreGate;


  /// Track collections ///

  bool m_getRawTracks;

  std::string m_verticesKey;
  std::string m_ftk_raw_trackcollection_Name;
  FTK_RawTrackContainer *m_ftk_raw_trackcollection;

  bool m_getTracks;
  //  std::string m_TrackCollectionName;
  bool m_getRefitTracks; 
  bool m_getOfflineTracks; 
  //  std::string m_refitTrackCollectionName;
  bool m_getTrackParticles; 
  //  std::string m_TrackParticleCollectionName;
  bool m_getRefitTrackParticles; 
  //  std::string m_refitTrackParticleCollectionName;
  bool m_getRawVertex_Fast;
  bool m_getConvertedVertex;
  bool m_getRefitVertex_Fast;
  bool m_getRefitVertex;
  //  std::string m_VertexContainerName;
  //  std::string m_refitVertexContainerName;
  bool m_getOfflineVertex;
  bool m_getTruthVertex;
  bool m_getClusters;
  bool m_getOfflineClusters;

  const AtlasDetectorID* m_idHelper;
  const PixelID* m_pixelId;  
  const SCT_ID* m_sctId;  
  const InDetDD::SiDetectorManager*  m_PIX_mgr;
  const InDetDD::SiDetectorManager*  m_SCT_mgr;

  ToolHandle<Trk::IResidualPullCalculator> m_residualCalc;
  //IFTK_DataProviderSvc* m_DataProviderSvc;
  ServiceHandle<IFTK_DataProviderSvc> m_DataProviderSvc;
  ToolHandle<Trk::IUpdator> m_iUpdator;

  bool m_fillHists;
  bool m_fillTree;

  /// Histograms ///
  TH1D* h_FTK_RawTrack_n;
  TH1D* h_FTK_RawTrack_phi;
  TH1D* h_FTK_RawTrack_d0;
  TH1D* h_FTK_RawTrack_z0;
  TH1D* h_FTK_RawTrack_invPt;
  TH1D* h_FTK_RawTrack_cot;
  TH1D* h_FTK_RawTrack_eta;
  TH1D* h_FTK_RawTrack_nPix;
  TH1D* h_FTK_RawTrack_nSCT;

  TH1D* h_Track_n;
  TH1D* h_Track_phi;
  TH1D* h_Track_d0;
  TH1D* h_Track_z0;
  TH1D* h_Track_invPt;
  TH1D* h_Track_cot;
  TH1D* h_Track_eta;
  TH1D* h_Track_nPix;
  TH1D* h_Track_nSCT;

  TH1D* h_refitTrack_n;
  TH1D* h_refitTrack_phi;
  TH1D* h_refitTrack_d0;
  TH1D* h_refitTrack_z0;
  TH1D* h_refitTrack_invPt;
  TH1D* h_refitTrack_cot;
  TH1D* h_refitTrack_eta;
  TH1D* h_refitTrack_nPix;
  TH1D* h_refitTrack_nSCT;

  TTree *Tree_ftk;

  std::vector<float> fastAlg_FTKraw_vertex_x_position;
  std::vector<float> fastAlg_FTKraw_vertex_y_position;
  std::vector<float> fastAlg_FTKraw_vertex_z_position;
  std::vector<float> fastAlg_FTKraw_vertex_x_error;
  std::vector<float> fastAlg_FTKraw_vertex_y_error;
  std::vector<float> fastAlg_FTKraw_vertex_z_error;
  std::vector<int>   fastAlg_FTKraw_vertex_nTrack;
  std::vector<int>   fastAlg_FTKraw_vertex_ndf;
  std::vector<float> fastAlg_FTKraw_vertex_chi2;
  std::vector<float> fastAlg_FTKraw_vertex_chi2_over_ndf;

  std::vector<float> raw_track_theta;
  std::vector<float> raw_track_eta;
  std::vector<float> raw_track_phi0;
  std::vector<float> raw_track_d0;
  std::vector<float> raw_track_z0;
  std::vector<float> raw_track_invPt;
  std::vector<float> raw_track_Pt;

  std::vector<float> fastAlg_FTKraw_vertex_associated_track_theta;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_eta;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_phi0;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_d0;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_z0;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_invPt;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_Pt;
  std::vector<float> fastAlg_FTKraw_vertex_associated_track_cot;
  std::vector<int>   fastAlg_FTKraw_vertex_associated_track_nVerts;
  std::vector<int>   fastAlg_FTKraw_vertex_associated_track_VtxNumber;

  std::vector<int>   fastAlg_FTKraw_vertex_number;
  
  Float_t fastAlg_FTKraw_vertex_sumPt;
  Float_t fastAlg_FTKraw_vertex_sumPt2;
  Int_t   fastAlg_FTKraw_vertex_sumPt2_vtxNumber;
  //
  std::vector<float> fastAlg_FTKrefit_vertex_x_position;
  std::vector<float> fastAlg_FTKrefit_vertex_y_position;
  std::vector<float> fastAlg_FTKrefit_vertex_z_position;
  std::vector<float> fastAlg_FTKrefit_vertex_x_error;
  std::vector<float> fastAlg_FTKrefit_vertex_y_error;
  std::vector<float> fastAlg_FTKrefit_vertex_z_error;
  std::vector<int>   fastAlg_FTKrefit_vertex_nTrack;
  std::vector<int>   fastAlg_FTKrefit_vertex_ndf;
  std::vector<float> fastAlg_FTKrefit_vertex_chi2;
  std::vector<float> fastAlg_FTKrefit_vertex_chi2_over_ndf;

  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_theta;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_eta;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_phi0;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_d0;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_z0;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_invPt;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_Pt;
  std::vector<float> fastAlg_FTKrefit_vertex_associated_track_cot;
  std::vector<int>   fastAlg_FTKrefit_vertex_associated_track_nVerts;
  std::vector<int>   fastAlg_FTKrefit_vertex_associated_track_VtxNumber;

  std::vector<int>   fastAlg_FTKrefit_vertex_number;
  
  Float_t fastAlg_FTKrefit_vertex_sumPt;
  Float_t fastAlg_FTKrefit_vertex_sumPt2;
  Int_t   fastAlg_FTKrefit_vertex_sumPt2_vtxNumber;


  //                                         
  std::vector<float> offlineAlg_FTKconverted_vertex_x_position;
  std::vector<float> offlineAlg_FTKconverted_vertex_y_position;
  std::vector<float> offlineAlg_FTKconverted_vertex_z_position;
  std::vector<float> offlineAlg_FTKconverted_vertex_x_error;
  std::vector<float> offlineAlg_FTKconverted_vertex_y_error;
  std::vector<float> offlineAlg_FTKconverted_vertex_z_error;
  std::vector<int>   offlineAlg_FTKconverted_vertex_nTrack;
  std::vector<int>   offlineAlg_FTKconverted_vertex_ndf;
  std::vector<float> offlineAlg_FTKconverted_vertex_chi2;
  std::vector<float> offlineAlg_FTKconverted_vertex_chi2_over_ndf;

  std::vector<float> converted_track_theta;
  std::vector<float> converted_track_eta;
  std::vector<float> converted_track_phi0;
  std::vector<float> converted_track_d0;
  std::vector<float> converted_track_z0;
  std::vector<float> converted_track_invPt;
  std::vector<float> converted_track_Pt;

  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_theta;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_eta;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_phi0;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_d0;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_z0;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_invPt;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_Pt;
  std::vector<float> offlineAlg_FTKconverted_vertex_associated_track_cot;
  std::vector<int>   offlineAlg_FTKconverted_vertex_associated_track_nVerts;
  std::vector<int>   offlineAlg_FTKconverted_vertex_associated_track_VtxNumber;

  std::vector<int>   offlineAlg_FTKconverted_vertex_number;

  Float_t offlineAlg_FTKconverted_vertex_sumPt;
  Float_t offlineAlg_FTKconverted_vertex_sumPt2;
  Int_t   offlineAlg_FTKconverted_vertex_sumPt2_vtxNumber;


  //                                                                                              
  std::vector<float> offlineAlg_FTKrefit_vertex_x_position;
  std::vector<float> offlineAlg_FTKrefit_vertex_y_position;
  std::vector<float> offlineAlg_FTKrefit_vertex_z_position;
  std::vector<float> offlineAlg_FTKrefit_vertex_x_error;
  std::vector<float> offlineAlg_FTKrefit_vertex_y_error;
  std::vector<float> offlineAlg_FTKrefit_vertex_z_error;
  std::vector<int>   offlineAlg_FTKrefit_vertex_nTrack;
  std::vector<int>   offlineAlg_FTKrefit_vertex_ndf;
  std::vector<float> offlineAlg_FTKrefit_vertex_chi2;
  std::vector<float> offlineAlg_FTKrefit_vertex_chi2_over_ndf;


  std::vector<float> refit_track_theta;
  std::vector<float> refit_track_eta;
  std::vector<float> refit_track_phi0;
  std::vector<float> refit_track_d0;
  std::vector<float> refit_track_z0;
  std::vector<float> refit_track_invPt;
  std::vector<float> refit_track_Pt;


  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_theta;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_eta;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_phi0;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_d0;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_z0;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_invPt;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_Pt;
  std::vector<float> offlineAlg_FTKrefit_vertex_associated_track_cot;
  std::vector<int>   offlineAlg_FTKrefit_vertex_associated_track_nVerts;
  std::vector<int>   offlineAlg_FTKrefit_vertex_associated_track_VtxNumber;

  std::vector<int>   offlineAlg_FTKrefit_vertex_number;

  Float_t offlineAlg_FTKrefit_vertex_sumPt;
  Float_t offlineAlg_FTKrefit_vertex_sumPt2;
  Int_t   offlineAlg_FTKrefit_vertex_sumPt2_vtxNumber;


  ////                                                                                            

  std::vector<float> offline_track_theta;
  std::vector<float> offline_track_eta;
  std::vector<float> offline_track_phi0;
  std::vector<float> offline_track_d0;
  std::vector<float> offline_track_z0;
  std::vector<float> offline_track_invPt;
  std::vector<float> offline_track_Pt;


  std::vector<float> offlineAlg_offlineTracks_vertex_x_position;
  std::vector<float> offlineAlg_offlineTracks_vertex_y_position;
  std::vector<float> offlineAlg_offlineTracks_vertex_z_position;
  std::vector<float> offlineAlg_offlineTracks_vertex_x_error;
  std::vector<float> offlineAlg_offlineTracks_vertex_y_error;
  std::vector<float> offlineAlg_offlineTracks_vertex_z_error;
  std::vector<int>   offlineAlg_offlineTracks_vertex_nTrack;
  std::vector<int>   offlineAlg_offlineTracks_vertex_ndf;
  std::vector<float> offlineAlg_offlineTracks_vertex_chi2;
  std::vector<float> offlineAlg_offlineTracks_vertex_chi2_over_ndf;

  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_theta;
  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_eta;
  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_phi0;
  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_d0;
  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_z0;
  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_invPt;
  std::vector<float> offlineAlg_offlineTracks_vertex_associated_track_Pt;
  std::vector<int>   offlineAlg_offlineTracks_vertex_associated_track_nVerts;
  std::vector<int>   offlineAlg_offlineTracks_vertex_associated_track_VtxNumber;

  std::vector<int>   isRawFastHS;
  std::vector<int>   isRefitFastHS;
  std::vector<int>   isConvertedOfflineHS;
  std::vector<int>   isRefitOfflineHS;
  std::vector<int>   isOfflineOfflineHS;

  std::vector<int>   offlineAlg_offlineTracks_vertex_number;

  Float_t offlineAlg_offlineTracks_vertex_sumPt;
  Float_t offlineAlg_offlineTracks_vertex_sumPt2;
  Int_t   offlineAlg_offlineTracks_vertex_sumPt2_vtxNumber;


  Double_t truth_x0;
  Double_t truth_y0;
  Double_t truth_z0;

  Int_t RunNumber;
  Int_t EventNumber;
  Int_t LumiBlock;
  Int_t BCID;
  Int_t averageInteractionsPerCrossing;
  Int_t actualInteractionsPerCrossing;
  Int_t extendedLevel1ID;
  Int_t level1TriggerType;
  std::vector<unsigned int> level1TriggerInfo;


  std::vector<float>   *refit_x_residual;
  std::vector<float>   *refit_y_residual;
  std::vector<float>   *refit_locX;
  std::vector<float>   *refit_locY;
  std::vector<bool>   *refit_isPixel;
  std::vector<bool>   *refit_isBarrel;
  std::vector<bool>   *refit_isSCT;
  std::vector<int>     *refit_layer;
  std::vector<int>     *refit_resAssociatedTrack;
  std::vector<int>     *refit_clustID;


  std::vector<float>   *offline_x_residual;
  std::vector<float>   *offline_y_residual;
  std::vector<float>   *offline_locX;
  std::vector<float>   *offline_locY;
  std::vector<bool>   *offline_isPixel;
  std::vector<bool>   *offline_isBarrel;
  std::vector<bool>   *offline_isSCT;
  std::vector<int>     *offline_layer;
  std::vector<int>     *offline_resAssociatedTrack;
  std::vector<int>     *offline_clustID;

};

#endif // FTK_RDO_ReaderAlgo_h
