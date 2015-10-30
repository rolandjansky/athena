/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcLv1RawDataSectorLogic
// Authors:  A. Vest (TU Dresden), C. Rudolph (TU Dresden) 
// February 2010
//
// DESCRIPTION:
// Subject: RPCLV1-->Sector Logic Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RpcLv1RawDataSectorLogic_H
#define RpcLv1RawDataSectorLogic_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "DataModel/DataLink.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonGeoModel/MYSQL.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

#include <sstream>
#include <string.h>
#include <vector>
#include <map>
 
class TFile;
class RpcIdHelper;
template <class ConcreteAlgorithm> class AlgFactory;
/////////////////////////////////////////////////////////////////////////////

class RpcLv1RawDataSectorLogic: public ManagedMonitorToolBase {
 
 public:

  RpcLv1RawDataSectorLogic ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~RpcLv1RawDataSectorLogic();
  StatusCode initialize(); 
  StatusCode finalize();   

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

 private:
    
  // Private functions and members and histograms
  
  StoreGateSvc*   m_eventStore;
  ActiveStoreSvc* m_activeStore;
  
  StatusCode StoreTriggerType();
  int GetTriggerType() { return trigtype; }
  int trigtype;

  mutable MsgStream m_log;     // Gaudi message stream
  bool m_debuglevel;           // control debug messages  
 
  int  m_sector;
  int  m_side;
  int rpc_eventstotal;
  int rpc_event_inarea;  
  
  bool m_lumiblockhist			;
  
  std::vector<Identifier>* m_rpcchambersId;
  std::vector<std::string> sectorlogic_list;
  std::vector<std::string> sectorlogicTowerCma_name_list;
  std::vector<std::string> sectorlogicTowerCma_name_list2;
  std::vector<std::string> profile_list;
  
  
  std::vector<Identifier>* m_padsId;
  std::map<std::string,int> hitsperchamber_map;

  bool in_area;
  bool histo_flag;  

  // Declare Histograms
  TH1* rpclv1_Hits_per_TriggerSector;
  TH1* rpclv1_TriggerHitsperEvent;
  TH2* rpclv1_TriggerHitsperEventperTriggerSector;
  TH1* rpclv1_Hits_per_TriggerSector_LB;
  TH2* rpclv1_TriggerHitsperEventperTriggerSector_LB;
  TH1* rpclv1_triggerBCid_inout_LB;
 
  TH2* rpclv1_TriggerSector_vs_Pad_triggerBCid_inout;
  TH2* rpclv1_TriggerSector_vs_Pad;
  TH2* rpclv1_TriggerSector_vs_Pad_Pt1;
  TH2* rpclv1_TriggerSector_vs_Pad_Pt2;
  TH2* rpclv1_TriggerSector_vs_Pad_Pt3;
  TH2* rpclv1_TriggerSector_vs_Pad_Pt4;
  TH2* rpclv1_TriggerSector_vs_Pad_Pt5;
  TH2* rpclv1_TriggerSector_vs_Pad_Pt6;
  TH2* rpclv1_rowinBCid_vs_TriggerSector;
  TH2* rpclv1_triggerBCid_vs_TriggerSector;
  TH1* rpclv1_triggerBCid_inout;


  TH2* rpclv1_ptid_vs_Tower;
  TH2* rpclv1_triggerBCid_vs_tower;
  TH2* rpclv1_triggerBCid_inout_vs_TriggerSector;

  TH2* rpclv1_Pad_vs_SL_old;
  TH2* rpclv1cmalogicstat_old;
  TH2* rpclv1ROI_LowPt_old;
  TH2* rpclv1ROI_HighPt_old;
  TH2* rpclv1_TriggerLy_vs_SL_old;
  TH2* rpclv1_TriggerCond_vs_SL_old;
  TH2* rpclv1_triggerBCid_inout_vs_Tower;

  // Global Variables declaration
	    
  //string variables
  
  std::string sectorlogic_name;
  std::string tower_name;
  std::string cma_name;
  std::string thr_name;
  std::string ijk_name;
  std::string cma_name_p;
  std::string ijk_name_p;
  char sectorlogic_name_char[1000];
  char tower_name_char[1000];
  char cma_name_char[1000];
  char thr_name_char[1000];
  char ijk_name_char[1000];
  char cma_name_p_char[1000];
  char ijk_name_p_char[1000];
  

  // Sector Logic variable declaration

  bool b_isInput;
  bool b_isInput1;
  int i_rowinBcid;
  int i_rowinBcid1;
  int i_padid;
  int i_padid1;
  int i_ptid;
  int i_roi;
  int i_roi1;
  int i_outerPlane;
  int i_overlapPhi;
  int i_overlapEta;
  int i_triggerBcid;
  int i_triggerBcid1;
  int i_sectorid;
  int sli;
  int m_nTriggerHits;
  int m_nTriggerHits_out;
  int m_nTriggerHitsperSector;
  int m_Diff_triggerBCid;
  int m_in_triggerBCid;
  int m_out_triggerBCid;
  int m_in_rowinBCid;
  int m_out_rowinBCid;
  int m_Tower_out;
  int m_Tower_in;
  int m_in_sectorid;

  const RpcSectorLogicContainer* sectorLogicContainer;
 
  std::string cm_ch_label   ;
  std::string pad_ch_label  ;
   
  
};

#endif
