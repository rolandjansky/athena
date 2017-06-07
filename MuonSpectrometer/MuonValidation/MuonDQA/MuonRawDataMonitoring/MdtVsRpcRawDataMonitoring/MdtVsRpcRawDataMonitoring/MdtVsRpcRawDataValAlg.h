/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h" 
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h" 
 
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"


class TFile;
class RpcIdHelper;
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
  virtual StatusCode procHistograms();
  StatusCode finalize();
 	
 private:
    
  // Private function to add the clusters to the ntuple
  StatusCode addClusters(std::string clusterContainerName);  
   
  StoreGateSvc* m_eventStore;  
  ActiveStoreSvc* m_activeStore;

  //mdt stuff
  const MdtIdHelper* m_mdtIdHelper;
  //m_chambersIdmdt;
  int m_mdt_eventstotal;

  int m_sector;
  int m_side;
  std::vector<std::string> m_layer_name_list       ;
  std::vector<std::string> m_layerSector_name_list ;   
  int m_rpc_eventstotal;
  std::vector<Identifier>* m_padsId;
  
  const MuonGM::MuonDetectorManager* m_muonMgr;
  const RpcIdHelper* m_rpcIdHelper; 
  const IRPCcablingSvc* m_cabling;
   
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

  //Declare Properties  
  
  std::string m_chamberName;
  std::string m_StationSize;
  std::string m_key_rpc; 
  int m_StationEta;
  int m_StationPhi;
  int m_lastEvent;
  int m_cosmicStation;
 
 
  TH1*    m_MdtRpcZdiff             ; 
 
  TH2*    m_MdtNHitsvsRpcNHits      ;
  
  std::string m_key_mdt;
};

#endif
   
