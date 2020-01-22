/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonChamberIDSelector.h
///////////////////////////////////////////////////////////////////

#ifndef MUONCHAMBERIDSELECTOR_H
#define MUONCHAMBERIDSELECTOR_H
 
//Base class
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <sstream>
#include <vector>

#include "MuonIdHelpers/MuonIdHelperTool.h"
 
class Identifier;

class MuonChamberIDSelector : public AthAlgorithm  {

 public:
  MuonChamberIDSelector(const std::string &name,ISvcLocator *pSvcLocator); //!< Athena algorithm constructor
  virtual ~MuonChamberIDSelector();
  // Basic algorithm methods:
  virtual StatusCode initialize();   //!< Algorithm initialization: retrieves StoreGate/DetectorStore/MuonIdHelpers/MuonPrepDataContainers
  virtual StatusCode execute();      //!< Retrieves and records containers, performs selection
  virtual StatusCode finalize();     //!< Does nothing

  /** vector of identifiers of mdt chambers */
  //  std::vector<Identifier>& mdtchambersId() const;
  
 
  StatusCode ChamberperformSelection() ;     
  //   StatusCode selectMDT(std::vector<Identifier>& mdtchambersId) ; 
  StatusCode selectMDT() ; 
  StatusCode selectRPC() ; //!< selects the RPCcollections
  StatusCode selectTGC() ; //!< selects the TGCcollections
  StatusCode selectCSC() ; //!< selects the CSCcollections
 
  //std::vector<Identifier> m_mdtchambersId;
 private: 

  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
   
  std::vector<Identifier>* m_mdtchambersId;
  std::vector<Identifier>* m_rpcchambersId;  

};
 
//inline std::vector<Identifier>& MuonChamberIDSelector::mdtchambersId() const {
//     return m_mdtchambersId;
//  }
 
#endif  
