/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOL_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOL_H

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPadContainer.h"

#include <string>
#include <set>

#define maxOfflineHash 600 // actually 593

//class IROBDataProviderSvc;


namespace MuonGM
{
  class MuonDetectorManager;
}

class IRPCcablingSvc;
class RpcIdHelper;
class RpcPadIdHash;
class IdentifierHash;
class RpcPad;
class RpcPadContainer;
class IRPCConditionsSvc;


namespace Muon {


class IMuonRawDataProviderTool;
class IRPC_RDO_Decoder;
    
    
    
/////////////////////////////////////////////////////////////////////////////

class RpcRdoToPrepDataTool : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool {
public:

  RpcRdoToPrepDataTool( const std::string&, const std::string&, const IInterface* );

  // to allow access to the IMuonRdoToPrepDataTool interface
  StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

  // setup/teardown functions, similar like those for Algorithm/Service
  StatusCode initialize();
  StatusCode finalize();

  // decoding method 
  StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect );
  StatusCode decode( const std::vector<uint32_t>& robIds );

  // debugging 
  void printInputRdo();
  void printPrepData();
  void printCoinData();

  // to resolve possible conflicts with IProperty::interfaceID()
  static const InterfaceID& interfaceID() { return IMuonRdoToPrepDataTool::interfaceID(); }


private:
  StatusCode processPad(const RpcPad *rdoColl, bool& processingetaview, bool& processingphiview, int& nPrepRawData, 
                        std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect, IdContext& rpcContext);
  StatusCode manageOutputContainers(bool& firstTimeInTheEvent);
  void processTriggerHitHypothesis(RpcCoinMatrix::const_iterator itD, RpcCoinMatrix::const_iterator itD_end, bool highptpad, // these are inputs 
                                   bool& triggerHit,
                                   unsigned short& threshold,
                                   unsigned short& overlap,
                                   bool& toSkip);


private:
  //double m_factor;

  // configurable options
  //int   m_rpcOffset;                    //!< Identifier hash offset
  //int   m_print_prepData;               //!< if 1 write a summary at the collections found
  float m_etaphi_coincidenceTime;       //!< time for phi*eta coincidence 
  float m_overlap_timeTolerance;        //!< tolerance of the timing calibration 
  bool  m_processingData;               //!< data or MC 
  bool  m_producePRDfromTriggerWords;   //!< if 1 store as prd the trigger hits 
  bool  m_solvePhiAmbiguities;          //!< toggle on/off the removal of phi ambiguities 
  bool  m_doingSecondLoopAmbigColls;    //!< true if running a second loop over ambiguous collections in RoI-based mode
  bool  m_reduceCablingOverlap;         //!< toggle on/off the overlap removal
  float m_timeShift;                    //!< any global time shift ?!
  bool m_useBStoRdoTool;                //!< toggle on/off the decoding of RPC BS into RDO for EF
  bool m_decodeData;                    //!< toggle on/off the decoding of RPC RDO into RpcPerpData
  bool m_RPCInfoFromDb;                 //!< correct time prd from cool db
  // end of configurable options 

  /// Muon Detector Descriptor
  const MuonGM::MuonDetectorManager * m_muonMgr;
  
  /// RPC identifier helper
  const RpcIdHelper*  m_rpcHelper;
  // RpcPadIdHash helper 
  //RpcPadIdHash* m_padHashIdHelper;
 
  /// RpcPrepData containers
  SG::WriteHandle<Muon::RpcPrepDataContainer> m_rpcPrepDataContainer;
  /// RpcCoinData containers
  SG::WriteHandle<Muon::RpcCoinDataContainer> m_rpcCoinDataContainer;
  SG::ReadHandle<RpcPadContainer>             m_rdoContainer;

  /// RPC cabling Svc
  const IRPCcablingSvc *m_rpcCabling;

//   // Rob Data Provider handle 
//   ServiceHandle<IROBDataProviderSvc>          m_robDataProvider;
  // handle tp the RPC_RawDataProviderTool
  ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool; 
  ToolHandle<Muon::IRPC_RDO_Decoder>         m_rpcRdoDecoderTool; 

  ServiceHandle<IRPCConditionsSvc> m_rSummarySvc;
  
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
