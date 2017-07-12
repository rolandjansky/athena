/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcRawDataMonitoring
// Author:   N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce)
// Sept. 2007
//
// DESCRIPTION:
// Subject: RPCLV1-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RpcLv1RawDataValAlg_H
#define RpcLv1RawDataValAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

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

#include "RpcRawDataMonitoring/RpcGlobalUtilities.h"

#include <sstream>
#include <string.h>
#include <vector>
#include <map>
 
class TFile;
class RpcIdHelper;
template <class ConcreteAlgorithm> class AlgFactory;
/////////////////////////////////////////////////////////////////////////////

class RpcLv1RawDataValAlg: public ManagedMonitorToolBase {
 
 public:

  RpcLv1RawDataValAlg ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~RpcLv1RawDataValAlg();
  StatusCode initialize(); 
 
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  StatusCode finalize();

 private:
    
  // Private function to add the clusters to the ntuple
  // StatusCode addClusters(std::string clusterContainerName);  
  
  // Functions declaration
  
  //Function for histogram booking and parameterd for fitting
  StatusCode  bookRPCLV1cmatimevschHistograms(std::string m_sectorlogic_name, std::string m_tower_name, std::string m_cma_name);    
  StatusCode bookRPCLV1TriggerRoadHistograms(std::string m_sectorlogic_name, std::string m_tower_name, std::string m_cma_name, std::string m_thr_name);
  StatusCode bookRPCLV1ProfilesHistograms(int m_i_sector, std::string m_sectorlogic_name, std::string m_cma_name,  int m_i_ijk, std::string m_ijk_name) ;  

   
  MuonDQAHistMap m_stationHists;


  StoreGateSvc* m_eventStore;
  
  ActiveStoreSvc* m_activeStore;
  
  StatusCode StoreTriggerType();
  int GetTriggerType() { return m_trigtype; }
  int m_trigtype;
  
  int m_sector;
  int m_side;

  std::vector<std::string> m_sectorlogicTowerCma_name_list   ;
  std::vector<std::string> m_sectorlogicTowerCma_name_list2  ;
  std::vector<std::string> m_profile_list                    ;
  
  bool m_doClusters         ;
  bool m_checkCabling       ;
  bool m_rpclv1file         ;  
  bool m_rpclv1hist         ; 
  bool m_rpclv1prof         ;
  int  m_rpclv1reducenbins  ;
     
  const MuonGM::MuonDetectorManager* m_muonMgr;
  const RpcIdHelper* m_rpcIdHelper;
  
  const IRPCcablingSvc* m_cabling;
    
  //Declare Properties
  
  // cool
  void  bookRPCCoolHistograms(std::vector<std::string>::const_iterator &m_iter, int, int, std::string m_layer);
  bool m_doCoolDB		       ;
  
  std::string m_chamberName;
  std::string m_StationSize;
  //std::string m_key_rpc; 
  int m_StationEta;
  int m_StationPhi;
  int m_lastEvent;
  int m_cosmicStation;
};


#endif 
