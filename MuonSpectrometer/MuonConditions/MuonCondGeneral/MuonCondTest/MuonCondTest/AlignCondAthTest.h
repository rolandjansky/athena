/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//
#ifndef ALIGNCONDATHTEST
#define ALIGNCONDATHTEST

#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

class Identifier; 
class MdtIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class CscIdHelper;

namespace MuonGM
{
    class MuonDetectorManager;
    class MdtReadoutElement;
}

class AlignCondAthTest:public AthAlgorithm {
public:
  AlignCondAthTest (const std::string& name, ISvcLocator* pSvcLocator);

public:

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   const MuonGM::MuonDetectorManager*  m_MuonDetMgrDS;
   SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

   const MdtIdHelper* m_mdtIdHelper;
   const RpcIdHelper* m_rpcIdHelper;
   const TgcIdHelper* m_tgcIdHelper;
   const CscIdHelper* m_cscIdHelper;

   StatusCode checkMdtGeometry(const MuonGM::MuonDetectorManager* manager);
   StatusCode checkRpcGeometry(const MuonGM::MuonDetectorManager* manager);
   StatusCode checkTgcGeometry(const MuonGM::MuonDetectorManager* manager);
   StatusCode checkCscGeometry(const MuonGM::MuonDetectorManager* manager);
   
};

#endif

