/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MdtVsRpcRawDataMonitoring
// Author:   N. Benekos(Illinois) - G. Chiodini(INFN-Lecce)
// Sept. 2007
//
// DESCRIPTION:
// Subject: MDTvsRPC-->Offline Muon Data Quality
///////////////////////////////////////////////////////////////////////////

#ifndef MdtVsRpcRawDataValAlg_H
#define MdtVsRpcRawDataValAlg_H

#include <sstream>
#include <string.h>
#include <vector>
#include <map>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

class TH1;
class TH2;

template <class ConcreteAlgorithm> class AlgFactory;

/////////////////////////////////////////////////////////////////////////////

class MdtVsRpcRawDataValAlg: public ManagedMonitorToolBase
{
 public:
 
  MdtVsRpcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent ); 
 
  virtual ~MdtVsRpcRawDataValAlg();
  StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
 	
 private:
    
  // Private function to add the clusters to the ntuple
  StatusCode addClusters(std::string clusterContainerName);  
   
  //m_chambersIdmdt;
  int m_mdt_eventstotal;

  int m_sector;
  int m_side;
  std::vector<std::string> m_layer_name_list       ;
  std::vector<std::string> m_layerSector_name_list ;   
  int m_rpc_eventstotal;
  std::vector<Identifier>* m_padsId;
  
  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
   
  void bookMDTvsRPCHistograms(std::string m_hardware_name, std::string m_layer_name, int binz, int binminz, int binmaxz, int binx, int binminx, int binmaxx );
  void bookMDTvsRPCsectorHistograms(std::string m_sector_name, std::string m_layer_name, float stripzmin, float stripzmax,float wirezmin, float wirezmax );

  MuonDQAHistMap m_stationHists;

  bool m_doClusters;
  std::string m_clusterContainerName;
  bool m_checkCabling;
  bool m_mdtvsrpcchamberhist; 
  bool m_mdtvsrpcsectorhist; 
 
  int  m_mdtvsrpcreducerpcnbins    ;  
  int  m_mdtvsrpcreducemdtnbins    ;   
  int  m_mdtvsrpcreducemdttdcnbins ; 

  bool m_doMdtvsRpcESD ;

  std::string m_chamberName;
  std::string m_StationSize;
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc{this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};
  SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_key_mdt{this,"MdtPrepDataContainer","MDT_DriftCircles","MDT PRDs"};
  int m_StationEta;
  int m_StationPhi;
  int m_lastEvent;
  int m_cosmicStation;
 
  TH1*    m_MdtRpcZdiff             ; 
  TH2*    m_MdtNHitsvsRpcNHits      ;
};

#endif
   
