/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALIGNCONDATHTEST
#define ALIGNCONDATHTEST

#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <fstream>

class AlignCondAthTest:public AthAlgorithm {
public:
  AlignCondAthTest (const std::string& name, ISvcLocator* pSvcLocator);

public:
   StatusCode initialize();
   StatusCode execute();

private:
   const MuonGM::MuonDetectorManager* m_MuonDetMgrDS; // nominal MuonDetectorManager (no alignment) from the DetectorStore (needed in this test alg to compare against the ConditionsObject)
   SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

   ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

   bool m_alinePrint;
   bool m_blinePrint;
   bool m_mdtPrint;
   bool m_rpcPrint;
   bool m_tgcPrint;
   bool m_cscPrint;

   StatusCode checkALines(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout);
   StatusCode checkBLines(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout);
   StatusCode checkMdtGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout);
   StatusCode checkRpcGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout);
   StatusCode checkTgcGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout);
   StatusCode checkCscGeometry(const MuonGM::MuonDetectorManager* manager, std::ofstream* fout);
   
};

#endif

