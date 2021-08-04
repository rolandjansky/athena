/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWPRDVALALG_H
#define NSWPRDVALALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "EDM_object.h"

#include "MMSimHitVariables.h"
#include "MMSDOVariables.h"
#include "MMDigitVariables.h"
#include "MMRDOVariables.h"
#include "MMPRDVariables.h"
#include "MDTSimHitVariables.h"
#include "MDTSDOVariables.h"
#include "MDTDigitVariables.h"
#include "RPCSimHitVariables.h"
#include "RPCSDOVariables.h"
#include "RPCDigitVariables.h"
#include "CSCSimHitVariables.h"
#include "CSCDigitVariables.h"
#include "CSCRDOVariables.h"
#include "CSCPRDVariables.h"
#include "TGCSimHitVariables.h"
#include "TGCSDOVariables.h"
#include "TGCDigitVariables.h"
#include "TGCRDOVariables.h"
#include "TGCPRDVariables.h"
#include "sTGCSimHitVariables.h"
#include "sTGCSDOVariables.h"
#include "sTGCDigitVariables.h"
#include "sTGCRDOVariables.h"
#include "sTGCPRDVariables.h"
#include "MuEntryVariables.h"
#include "TruthVariables.h"

#include "TTree.h"

#include <vector>
#include <memory>

class ITHistSvc;

class NSWPRDValAlg:public AthAlgorithm
{
 public:
  NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

  // Matching algorithm
  StatusCode NSWMatchingAlg();  // First set up which object should be matched, given the input used to fill the NSW Ntuple
  StatusCode NSWMatchingAlg (EDM_object data0, EDM_object data1); // This part of the matching algortihm does the actual comparison given two EDM obects
  StatusCode setDataAdress (EDM_object &oData, TString branch_name); // This function couples the branch of the NSW validation Ntuple with the EDM object. 

 private:
  std::unique_ptr<TruthVariables>         m_TruthVar;
  std::unique_ptr<MuEntryVariables>       m_MuEntryVar;
  std::unique_ptr<sTGCSimHitVariables>    m_sTgcSimHitVar;
  std::unique_ptr<sTGCRDOVariables>       m_sTgcRdoVar;
  std::unique_ptr<sTGCSDOVariables>       m_sTgcSdoVar;
  std::unique_ptr<sTGCSDOVariables>       m_sTgcFastSdoVar;
  std::unique_ptr<sTGCDigitVariables>     m_sTgcDigitVar;
  std::unique_ptr<sTGCPRDVariables>       m_sTgcPrdVar;
  std::unique_ptr<MMSimHitVariables>      m_MmSimHitVar;
  std::unique_ptr<MMSDOVariables>         m_MmSdoVar;
  std::unique_ptr<MMSDOVariables>         m_MmFastSdoVar;
  std::unique_ptr<MMDigitVariables>       m_MmDigitVar;
  std::unique_ptr<MMRDOVariables>         m_MmRdoVar;
  std::unique_ptr<MMPRDVariables>         m_MmPrdVar;
  std::unique_ptr<CSCSimHitVariables>     m_CSCSimHitVar;
  std::unique_ptr<CSCDigitVariables>      m_CscDigitVar;
  std::unique_ptr<CSCRDOVariables>        m_CSCRDOVar;
  std::unique_ptr<CSCPRDVariables>        m_CSCPRDVar;
  std::unique_ptr<MDTSimHitVariables>     m_MDTSimHitVar;
  std::unique_ptr<MdtSDOVariables>        m_MDTSDOVar;
  std::unique_ptr<MdtDigitVariables>      m_MDTDigitVar;
  std::unique_ptr<RPCSimHitVariables>     m_RPCSimHitVar;
  std::unique_ptr<RpcSDOVariables>        m_RPCSDOVar;
  std::unique_ptr<RpcDigitVariables>      m_RPCDigitVar;
  std::unique_ptr<TGCSimHitVariables>     m_TGCSimHitVar;
  std::unique_ptr<TgcSDOVariables>        m_TGCSDOVar;
  std::unique_ptr<TGCDigitVariables>      m_TgcDigitVar;
  std::unique_ptr<TGCRDOVariables>        m_TgcRdoVar;
  std::unique_ptr<TGCPRDVariables>        m_TGCPRDVar;

  TTree* m_tree; // still needed in NSWMatchingAlg during finalize

  // MuonDetectorManager from the Detector Store (to be used only at initialize)
  const MuonGM::MuonDetectorManager* m_muonDetMgrDS;
  const ITGCcablingSvc* m_tgcCabling;
  ITHistSvc* m_thistSvc;

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};
      
  Gaudi::Property<bool> m_retrieveTgcCabling{this,"RetrieveTgcCabling",false};

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  BooleanProperty  m_isData;             // if false use MuonDetectorManager from detector store everywhere
  BooleanProperty  m_doTruth;            // switch on the output of the MC truth
  BooleanProperty  m_doMuEntry;          // switch on the output of the Muon Entry Layer
  BooleanProperty  m_doSTGCHit;          // switch on the output of the Small TGC simulated hits
  BooleanProperty  m_doSTGCFastDigit;    // switch on the output of the Small TGC fast digitization
  BooleanProperty  m_doSTGCDigit;        // swicth on the output of the Small TGC digit
  BooleanProperty  m_doSTGCRDO;          // switch on the output of the Small TGC RDO
  BooleanProperty  m_doSTGCPRD;          // swicth on the output of the Small TGC prepdata
  BooleanProperty  m_doMMHit;            // switch on the output of the MicroMegas simulated hits
  BooleanProperty  m_doMMFastDigit;      // switch on the output of the MicroMegas fast digitization
  BooleanProperty  m_doMMDigit;          // switch on the output of the MicroMegas digitization
  BooleanProperty  m_doMMRDO;            // switch on the output of the MicroMegas RDO
  BooleanProperty  m_doMMPRD;            // switch on the output of the MicroMegas prepdata
  BooleanProperty  m_doCSCHit;           // switch on the output of the CSC simulated hits
  BooleanProperty  m_doCSCDigit;         // switch on the output of the CSC digitization
  BooleanProperty  m_doCSCRDO;           // switch on the output of the CSC RDO
  BooleanProperty  m_doCSCPRD;           // switch on the output of the CSC prepdata
  BooleanProperty  m_doMDTHit;           // switch on the output of the MDT simulated hits
  BooleanProperty  m_doMDTSDO;           // switch on the output of the MDT SDO
  BooleanProperty  m_doMDTDigit;         // switch on the output of the MDT digitization
  BooleanProperty  m_doRPCHit;           // switch on the output of the RPC simulated hits
  BooleanProperty  m_doRPCSDO;           // switch on the output of the RPC SDO
  BooleanProperty  m_doRPCDigit;         // switch on the output of the RPC digitization
  BooleanProperty  m_doTGCHit;           // switch on the output of the TGC simulated hits
  BooleanProperty  m_doTGCSDO;           // switch on the output of the TGC SDO
  BooleanProperty  m_doTGCDigit;         // switch on the output of the TGC digitization
  BooleanProperty  m_doTGCRDO;           // switch on the output of the TGC RDO
  BooleanProperty  m_doTGCPRD;           // switch on the output of the TGC prepdata

  unsigned int m_runNumber;
  unsigned int m_eventNumber;

  std::string m_Truth_ContainerName;
  std::string m_MuEntry_ContainerName;
  std::string m_NSWsTGC_ContainerName;
  std::string m_NSWsTGC_SDOContainerName;
  std::string m_NSWsTGC_DigitContainerName;
  std::string m_NSWsTGC_RDOContainerName;
  std::string m_NSWsTGC_PRDContainerName;
  std::string m_NSWMM_ContainerName;
  std::string m_NSWMM_SDOContainerName;
  std::string m_NSWMM_DigitContainerName;
  std::string m_NSWMM_RDOContainerName;
  std::string m_NSWMM_PRDContainerName;
  std::string m_CSC_SimContainerName;
  std::string m_CSC_DigitContainerName;
  std::string m_CSC_RDOContainerName;
  std::string m_CSC_PRDContainerName;
  std::string m_MDT_SimContainerName;
  std::string m_MDT_SDOContainerName;
  std::string m_MDT_DigitContainerName;
  std::string m_RPC_SimContainerName;
  std::string m_RPC_SDOContainerName;
  std::string m_RPC_DigitContainerName;
  std::string m_TGC_SimContainerName;
  std::string m_TGC_SDOContainerName;
  std::string m_TGC_DigitContainerName;
  std::string m_TGC_RDOContainerName;
  std::string m_TGC_PRDContainerName;

  // Matching algorithm
  BooleanProperty m_doNSWMatching;
  BooleanProperty m_doNSWMatchingMuon;
  uint m_maxStripDiff;
  bool  m_noMatchWarning;
};

#endif // NSWPRDVALALG_H
