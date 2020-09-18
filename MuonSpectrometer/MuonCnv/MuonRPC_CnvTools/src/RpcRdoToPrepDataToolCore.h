/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLCORE_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLCORE_H

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "MuonCondData/RpcCondDbData.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "RPC_CondCabling/RpcCablingCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <string>
#include <set>

#define maxOfflineHash 600 // actually 593

namespace Muon {

/////////////////////////////////////////////////////////////////////////////

class RpcRdoToPrepDataToolCore : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool {
public:

  RpcRdoToPrepDataToolCore( const std::string&, const std::string&, const IInterface* );

  // to allow access to the IMuonRdoToPrepDataTool interface
  StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  // setup/teardown functions, similar like those for Algorithm/Service
  StatusCode initialize();

  // decoding method 
  StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect );
  StatusCode decode( const std::vector<uint32_t>& robIds );

  // debugging 
  void printInputRdo();
  void printPrepData();
  void printCoinData();

  // to resolve possible conflicts with IProperty::interfaceID()
  static const InterfaceID& interfaceID() { return IMuonRdoToPrepDataTool::interfaceID(); }


protected:
  StatusCode processPad(const RpcPad *rdoColl, bool& processingetaview, bool& processingphiview, int& nPrepRawData, 
                        std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect, IdContext& rpcContext);

  // Overridden by subclasses to handle legacy and MT cases
  virtual StatusCode manageOutputContainers(bool& firstTimeInTheEvent);

  void processTriggerHitHypothesis(RpcCoinMatrix::const_iterator itD, RpcCoinMatrix::const_iterator itD_end, bool highptpad, // these are inputs 
                                   bool& triggerHit,
                                   unsigned short& threshold,
                                   unsigned short& overlap,
                                   bool& toSkip);

  float m_etaphi_coincidenceTime;       //!< time for phi*eta coincidence 
  float m_overlap_timeTolerance;        //!< tolerance of the timing calibration 
  bool  m_producePRDfromTriggerWords;   //!< if 1 store as prd the trigger hits 
  bool  m_solvePhiAmbiguities;          //!< toggle on/off the removal of phi ambiguities 
  bool  m_doingSecondLoopAmbigColls;    //!< true if running a second loop over ambiguous collections in RoI-based mode
  bool  m_reduceCablingOverlap;         //!< toggle on/off the overlap removal
  float m_timeShift;                    //!< any global time shift ?!
  bool m_decodeData;                    //!< toggle on/off the decoding of RPC RDO into RpcPerpData
  bool m_RPCInfoFromDb;                 //!< correct time prd from cool db
  // end of configurable options 

  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muDetMgrKey {this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"}; 
  
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
  /// RpcPrepData containers
  Muon::RpcPrepDataContainer* m_rpcPrepDataContainer;
  SG::WriteHandleKey<Muon::RpcPrepDataContainer> m_rpcPrepDataContainerKey;
  /// RpcCoinData containers
  Muon::RpcCoinDataContainer* m_rpcCoinDataContainer;
  SG::WriteHandleKey<Muon::RpcCoinDataContainer> m_rpcCoinDataContainerKey;
  
  SG::ReadHandleKey<RpcPadContainer>             m_rdoContainerKey;

  // Rob Data Provider handle 
  ToolHandle<Muon::IRPC_RDO_Decoder> m_rpcRdoDecoderTool{this,"RdoDecoderTool","Muon::RpcRDO_Decoder"}; 

  SG::ReadCondHandleKey<RpcCondDbData> m_readKey{this, "ReadKey", "RpcCondDbData", "Key of RpcCondDbData"};
  SG::ReadCondHandleKey<RpcCablingCondData> m_rpcReadKey{this, "RpcCablingKey", "RpcCablingCondData", "Key of RpcCablingCondData"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};

  //keepTrackOfFullEventDecoding
  bool m_fullEventDone;
  
  //the set of already requested and decoded offline (PrepRawData) collections
  std::set<IdentifierHash> m_decodedOfflineHashIds;
  
  //the set of already requested and decoded ROBs
  std::set<uint32_t> m_decodedRobIds;
  
  //the set of unrequested collections with phi hits stored with ambiguityFlag > 1
  std::set<IdentifierHash> m_ambiguousCollections;
    
};
}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
