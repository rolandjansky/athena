/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCHAMBERIDSELECTOR_H
#define MUONCHAMBERIDSELECTOR_H
 
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>
#include <sstream>
#include <vector>

class MuonChamberIDSelector : public AthAlgorithm  {

 public:
  MuonChamberIDSelector(const std::string &name,ISvcLocator *pSvcLocator); //!< Athena algorithm constructor
  virtual ~MuonChamberIDSelector()=default;
  // Basic algorithm methods:
  virtual StatusCode initialize();   //!< Algorithm initialization: retrieves StoreGate/DetectorStore/MuonIdHelpers/MuonPrepDataContainers
  virtual StatusCode execute();      //!< Retrieves and records containers, performs selection
 
  StatusCode ChamberperformSelection() ;     
  StatusCode selectMDT(); 
  StatusCode selectRPC(); //!< selects the RPCcollections
  StatusCode selectTGC(){return StatusCode::SUCCESS;} //!< selects the TGCcollections
  StatusCode selectCSC(){return StatusCode::SUCCESS;} //!< selects the CSCcollections
 
 private: 
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  std::vector<Identifier>* m_mdtchambersId;
  std::vector<Identifier>* m_rpcchambersId;  
};

#endif  
