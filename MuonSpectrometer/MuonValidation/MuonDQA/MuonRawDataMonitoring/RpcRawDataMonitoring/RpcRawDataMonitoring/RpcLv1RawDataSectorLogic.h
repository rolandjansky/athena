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
  
  ActiveStoreSvc* m_activeStore;
  
  StatusCode StoreTriggerType();
  int GetTriggerType() { return m_trigtype; }
  int m_trigtype;

  int m_rpc_eventstotal;
  int m_rpc_event_inarea;  
  
  bool m_lumiblockhist			;
  
  // Declare Histograms
  TH1* m_rpclv1_Hits_per_TriggerSector;
  TH1* m_rpclv1_TriggerHitsperEvent;
  TH2* m_rpclv1_TriggerHitsperEventperTriggerSector;
  TH1* m_rpclv1_Hits_per_TriggerSector_LB;
  TH2* m_rpclv1_TriggerHitsperEventperTriggerSector_LB;
  TH1* m_rpclv1_triggerBCid_inout_LB;
 
  TH2* m_rpclv1_TriggerSector_vs_Pad_triggerBCid_inout;
  TH2* m_rpclv1_TriggerSector_vs_Pad;
  TH2* m_rpclv1_TriggerSector_vs_Pad_Pt1;
  TH2* m_rpclv1_TriggerSector_vs_Pad_Pt2;
  TH2* m_rpclv1_TriggerSector_vs_Pad_Pt3;
  TH2* m_rpclv1_TriggerSector_vs_Pad_Pt4;
  TH2* m_rpclv1_TriggerSector_vs_Pad_Pt5;
  TH2* m_rpclv1_TriggerSector_vs_Pad_Pt6;
  TH2* m_rpclv1_rowinBCid_vs_TriggerSector;
  TH1* m_rpclv1_triggerBCid_inout;


  TH2* m_rpclv1_ptid_vs_Tower;
  TH2* m_rpclv1_triggerBCid_inout_vs_TriggerSector;

  TH2* m_rpclv1_triggerBCid_inout_vs_Tower;

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

  const RpcSectorLogicContainer* m_sectorLogicContainer;
 };

#endif
