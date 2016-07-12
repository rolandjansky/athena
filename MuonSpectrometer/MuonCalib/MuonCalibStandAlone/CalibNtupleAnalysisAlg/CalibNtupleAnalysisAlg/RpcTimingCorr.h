/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_RpcTimingCorr_H
#define MuonCalib_RpcTimingCorr_H


//Gaudi
#include "GaudiKernel/ServiceHandle.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/CalibSegmentPreparationTool.h"

//root
#include "TFile.h"
#include "TTree.h"

class RegionSelectionSvc;
class StoreGateSvc;
class RpcIdHelper;

namespace MuonCalib {
class IIdToFixedIdTool;
class RPCClusterBuilder;
class RPCCluster;

class RpcTimingCorr : public AlgTool, virtual public CalibSegmentPreparationTool {
 public:
//=========================constructor==========================================
  RpcTimingCorr(const std::string &t, const std::string &n, const IInterface *p);
  inline ~RpcTimingCorr() {}
//=========================public member functions==============================
  //initialize and finalize
  StatusCode initialize(void);
  StatusCode finalize(void);
  //load event
  void prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments);
 private:
//=========================private data=========================================
  bool m_strict_nhits_cut;
  bool m_do_debug_ntuple;
  //! pointer to region selection service
  RegionSelectionSvc *p_reg_sel_svc;
  StoreGateSvc *m_detStore;         // pointer to the detector store
  const RpcIdHelper *m_RpcIdHelper; // pointer to the RPC ID helper
  const MuonGM::MuonDetectorManager *m_detMgr; // pointer to the muon
		
  std::string m_detector_store; // name of the detector store
  std::string m_RPC_ID_helper;  // name of the RPC ID helper
  std::string m_offset_file;
  const MuonCalib::IIdToFixedIdTool *m_id_tool;
  RPCClusterBuilder *m_cluster_builder;
  //!debug ntuple
  TFile *m_debug_file;
  TTree *m_debug_tree;
  Int_t m_n_strips;
  Int_t m_station[100], m_eta[100], m_phi[100], m_double_r[100], m_double_z[100], m_double_phi[100], m_gas_gap[100], m_meas_phi[100], m_strip[100];
  Int_t m_mdt_station, m_mdt_eta, m_mdt_phi;
  Double_t m_t_corr, m_t_strip[100];
  char m_mdt_station_str[4];
  char m_station_str[100][4];
  void fill_debug_rpc(const RPCCluster *cluster);
};

} //namespace MuonCalib
#endif
