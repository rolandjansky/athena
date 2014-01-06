/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCHitsValAlg_H
#define RPCHitsValAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/MsgStream.h"

#include "TTree.h"

#include "MuonHitTest/ValAlgInterface.h"
#include "MuonIdHelpers/RpcIdHelper.h"
//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include <string>
#include <sstream>   
#include <vector>
#include <map>

class RPCSimHit; 
class Identifier;
class TTree; 
class TFile;
class RpcIdHelper;

// pre-declarations
namespace MuonGM {
  class RpcReadoutElement;
  class MuonDetectorManager;
}

template <class ConcreteAlgorithm> class AlgFactory;

class RPCHitsValAlg: public Algorithm, private ValAlgInterface {

  friend class AlgFactory<RPCHitsValAlg>;

 public:
  RPCHitsValAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~RPCHitsValAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();


 private:


  TTree* rpctree;/** Pointer to the NTuple tree
		     The tree that will contain our "AANtuple" with one entry per hit*/ 

  /**Simple method for accessing root histo service for booking*/
  ITHistSvc*     ToolRootHistSvc();


  int event_counter;
  bool m_DoRPCTest;

  bool m_writeNtuple;                  /**jobOption: write data to AANtuple*/
  bool m_writeChambers;

  std::string m_ntupleTreeName;        /** jobOption: Ntuple tree name*/
  std::string m_ntupleFileName;        /** jobOption: Ntuple file name*/   
  std::string m_ntupleDirName;         /** jobOption: Ntuple directory name*/


  /**The MC truth key */
  std::string m_key; 	

  std::vector<std::string> hardware_name_list;
  bool histo_flag;
  std::string m_chamberName;
  std::string m_StationSize;
  int m_StationEta;
  int m_StationPhi;
  int m_LastEvent;
  std::map<std::string,int> hitsperchamber_map;


  /**Pointer On MuonDetectorManager */
  const MuonGM::RpcReadoutElement* descriptor;
  const MuonGM::MuonDetectorManager*    m_pMuonMgr;

  /**Pointers On Helpers */
  const RpcIdHelper*      m_pRpcIdHelper;
  	
  MsgStream log;
  StoreGateSvc*              m_sgSvc;
  bool isVerbose() {return (outputLevel() <= MSG::VERBOSE);}

  /**Athena-Aware Ntuple variable declaration, explanation in RPCHitsValAlg.cxx*/
  int counter_ValHitNumber ; 
  int m_Validation_RPC_Type;
  int m_Validation_RPC_NumberOfHits;
  int m_Validation_RPC_EventNumber;
  int m_Validation_RPC_RunNumber;
  double m_Validation_RPC_HitX;
  double m_Validation_RPC_HitY;
  double m_Validation_RPC_HitZ;
  double m_Validation_RPC_HitR;
//  char m_Validation_RPC_StationName[3]; // string in as char array
  int m_Validation_RPC_StationName;
  int m_Validation_RPC_StationEta;
  int m_Validation_RPC_StationPhi;
  int m_Validation_RPC_DoubletR;
  int m_Validation_RPC_DoubletZ;
  int m_Validation_RPC_DoubletPhi;
  int m_Validation_RPC_MeasuresPhi;
  int m_Validation_RPC_Strip;
  int m_Validation_RPC_DoubletGasGap;



	 
 protected:
 
  ITHistSvc*     m_rootsvc;


};  

#endif
