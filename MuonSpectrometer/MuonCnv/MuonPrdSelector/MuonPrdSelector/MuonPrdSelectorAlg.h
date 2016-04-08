/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonPrdSelectorAlg.h
//   Header file for class MuonPrdSelectorAlg
///////////////////////////////////////////////////////////////////

#ifndef MUONPRDSELECTORALG_H
#define MUONPRDSELECTORALG_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
//#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrdSelector/MuonIdCutTool.h"
#include <utility>
#include <vector>
#include <string>



class StoreGateSvc;

class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

/*
   class MuonPrdSelectorAlg
   class to cut out hits from Muon PRDCollections
   cuts, based on Identifier, are defined by MuonIdCutTool
   base functionality of class authored by Zdenko.van.Kesteren@cern.ch

   author: Andrew Meade ameade@physics.umass.edu

 */

namespace MuonGM {
  class MuonDetectorManager;
}

class MuonPrdSelectorAlg : public AthAlgorithm  {
 public:
  MuonPrdSelectorAlg(const std::string &name,ISvcLocator *pSvcLocator); //!< Athena algorithm constructor

  // Basic algorithm methods:
  virtual StatusCode initialize();   //!< Algorithm initialization: retrieves StoreGate/DetectorStore/MuonIdHelpers/MuonPrepDataContainers
  virtual StatusCode execute();      //!< Retrieves and records containers, performs selection
  virtual StatusCode finalize();     //!< Does nothing

 private:
  void print();                      //!< method for DEBUG purposes: prints the content of input and output MuonPrepDataContainer
  
  StatusCode retrieveContainers() ;  //!< retrieves input MuonPrepDataContainers from StoreGate
  StatusCode recordContainers() ;    //!< records output MuonPrepDataContainers to StoreGate
  StatusCode performSelection() ;    //!< checks if each hit in each collection passes a set of cuts
                                     //!< these hits are written to new collections
  
  StatusCode selectMDTs() ; //!< selects the hits from MDTcollections
  StatusCode selectRPCs() ; //!< selects the hits from RPCcollections
  StatusCode selectTGCs() ; //!< selects the hits from TGCcollections
  StatusCode selectCSCs() ; //!< selects the hits from CSCcollections


  const MuonGM::MuonDetectorManager*  m_detMgr;

  const MdtIdHelper*  m_mdtIdHelper;
  const CscIdHelper*  m_cscIdHelper;
  const RpcIdHelper*  m_rpcIdHelper;
  const TgcIdHelper*  m_tgcIdHelper;

  ToolHandle<IMuonIdCutTool>             m_muonIdCutTool;  //defines the cuts

  std::string         m_inputContainer_mdt; //container names
  std::string         m_inputContainer_rpc;
  std::string         m_inputContainer_tgc;
  std::string         m_inputContainer_csc;
  
  std::string         m_outputContainer_mdt;
  std::string         m_outputContainer_rpc;
  std::string         m_outputContainer_tgc;
  std::string         m_outputContainer_csc;
  
  const Muon::MdtPrepDataContainer* m_mdtPRDs_in ;
  const Muon::RpcPrepDataContainer* m_rpcPRDs_in ;
  const Muon::TgcPrepDataContainer* m_tgcPRDs_in ;
  const Muon::CscStripPrepDataContainer* m_cscPRDs_in ;

  Muon::MdtPrepDataContainer* m_mdtPRDs_out ;
  Muon::RpcPrepDataContainer* m_rpcPRDs_out ;
  Muon::TgcPrepDataContainer* m_tgcPRDs_out ;
  Muon::CscStripPrepDataContainer* m_cscPRDs_out ;
};


#endif //MUONPRDSELECTORALG_H
