/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRDSELECTORALG_H
#define MUONPRDSELECTORALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrdSelector/IMuonIdCutTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <utility>
#include <vector>
#include <string>

/*
   class MuonPrdSelectorAlg
   class to cut out hits from Muon PRDCollections
   cuts, based on Identifier, are defined by MuonIdCutTool
   base functionality of class authored by Zdenko.van.Kesteren@cern.ch

   author: Andrew Meade ameade@physics.umass.edu

 */

class MuonPrdSelectorAlg : public AthAlgorithm  {
 public:
  MuonPrdSelectorAlg(const std::string &name,ISvcLocator *pSvcLocator); //!< Athena algorithm constructor

  // Basic algorithm methods:
  virtual StatusCode initialize();   //!< Algorithm initialization: retrieves StoreGate/DetectorStore/MuonIdHelpers/MuonPrepDataContainers
  virtual StatusCode execute();      //!< Retrieves and records containers, performs selection

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

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  ToolHandle<IMuonIdCutTool> m_muonIdCutTool{this,"MuonIdCutTool","MuonIdCutTool/MuonIdCutTool","defines the cuts"};

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
