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
#include "CSCSDOVariables.h"
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

class NSWPRDValAlg: public AthAlgorithm
{
 public:
  NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

  // Matching algorithm
  StatusCode NSWMatchingAlg();  // First set up which object should be matched, given the input used to fill the NSW Ntuple
  StatusCode NSWMatchingAlg (EDM_object data0, EDM_object data1); // This part of the matching algortihm does the actual comparison given two EDM obects
  StatusCode setDataAdress (EDM_object &oData, const TString& branch_name); // This function couples the branch of the NSW validation Ntuple with the EDM object. 

 private:

  std::vector<std::unique_ptr<ValAlgVariables>> m_testers;
  
  TTree* m_tree{nullptr}; // still needed in NSWMatchingAlg during finalize

  // MuonDetectorManager from the Detector Store (to be used only at initialize)
  const MuonGM::MuonDetectorManager* m_muonDetMgrDS;
  const ITGCcablingSvc* m_tgcCabling;
  ITHistSvc* m_thistSvc;

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};
 
   ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
   PublicToolHandle<Muon::ICSC_RDO_Decoder> m_csc_decoder{this, "CscRDODecoder", "Muon::CscRDO_Decoder/CSC_RDODecoder"};
   Gaudi::Property<bool>  m_isData{this, "isData", false};             // if false use MuonDetectorManager from detector store everywhere
   Gaudi::Property<bool>  m_doTruth{this, "doTruth", false};            // switch on the output of the MC truth
   Gaudi::Property<bool>  m_doMuEntry{this, "doMuEntry", false};          // switch on the output of the Muon Entry Layer
   Gaudi::Property<bool>  m_doSTGCHit{this, "doSTGCHit", false};          // switch on the output of the Small TGC simulated hits
   Gaudi::Property<bool>  m_doSTGCFastDigit{this, "doSTGCFastDigit", false};    // switch on the output of the Small TGC fast digitization
   Gaudi::Property<bool>  m_doSTGCDigit{this, "doSTGCDigit", false};        // swicth on the output of the Small TGC digit
   Gaudi::Property<bool>  m_doSTGCRDO{this, "doSTGCRDO", false};          // switch on the output of the Small TGC RDO
   Gaudi::Property<bool>  m_doSTGCPRD{this, "doSTGCPRD", false};          // swicth on the output of the Small TGC prepdata
   Gaudi::Property<bool>  m_doMMHit{this, "doMMHit", false};            // switch on the output of the MicroMegas simulated hits
   Gaudi::Property<bool>  m_doMMFastDigit{this, "doMMFastDigit", false};      // switch on the output of the MicroMegas fast digitization
   Gaudi::Property<bool>  m_doMMDigit{this, "doMMDigit", false};          // switch on the output of the MicroMegas digitization
   Gaudi::Property<bool>  m_doMMRDO{this, "doMMRDO", false};            // switch on the output of the MicroMegas RDO
   Gaudi::Property<bool>  m_doMMPRD{this, "doMMPRD", false};            // switch on the output of the MicroMegas prepdata
   Gaudi::Property<bool>  m_doCSCHit{this, "doCSCHit", false};           // switch on the output of the CSC simulated hits
   Gaudi::Property<bool>  m_doCSCSDO{this, "doCSCSDO", false};           // switch on the output of the CSC SDO
   Gaudi::Property<bool>  m_doCSCDigit{this, "doCSCDigit", false};         // switch on the output of the CSC digitization
   Gaudi::Property<bool>  m_doCSCRDO{this, "doCSCRDO", false};           // switch on the output of the CSC RDO
   Gaudi::Property<bool>  m_doCSCPRD{this, "doCSCPRD", false};           // switch on the output of the CSC prepdata
   Gaudi::Property<bool>  m_doMDTHit{this, "doMDTHit", false};           // switch on the output of the MDT simulated hits
   Gaudi::Property<bool>  m_doMDTSDO{this, "doMDTSDO", false};           // switch on the output of the MDT SDO
   Gaudi::Property<bool>  m_doMDTDigit{this, "doMDTDigit", false};         // switch on the output of the MDT digitization
   Gaudi::Property<bool>  m_doRPCHit{this, "doRPCHit", false};           // switch on the output of the RPC simulated hits
   Gaudi::Property<bool>  m_doRPCSDO{this, "doRPCSDO", false};           // switch on the output of the RPC SDO
   Gaudi::Property<bool>  m_doRPCDigit{this, "doRPCDigit", false};         // switch on the output of the RPC digitization
   Gaudi::Property<bool>  m_doTGCHit{this, "doTGCHit", false};           // switch on the output of the TGC simulated hits
   Gaudi::Property<bool>  m_doTGCSDO{this, "doTGCSDO", false};           // switch on the output of the TGC SDO
   Gaudi::Property<bool>  m_doTGCDigit{this, "doTGCDigit", false};         // switch on the output of the TGC digitization
   Gaudi::Property<bool>  m_doTGCRDO{this, "doTGCRDO", false};           // switch on the output of the TGC RDO
   Gaudi::Property<bool>  m_doTGCPRD{this, "doTGCPRD", false};           // switch on the output of the TGC prepdata

  unsigned int m_runNumber{0};
  unsigned int m_eventNumber{0};

  Gaudi::Property<std::string> m_Truth_ContainerName{this, "Truth_ContainerName", "TruthEvent"};
  Gaudi::Property<std::string> m_MuEntry_ContainerName{this, "MuonEntryLayer_ContainerName", "MuonEntryLayer"};
  
  Gaudi::Property<std::string> m_NSWsTGC_ContainerName{this, "NSWsTGC_ContainerName", "sTGCSensitiveDetector" };
  Gaudi::Property<std::string> m_NSWsTGC_SDOContainerName{this, "NSWsTGC_SDOContainerName","sTGC_SDO" };
  Gaudi::Property<std::string> m_NSWsTGC_DigitContainerName{this,"NSWsTGC_DigitContainerName", "sTGC_DIGITS"};
  Gaudi::Property<std::string> m_NSWsTGC_RDOContainerName{this, "NSWsTGC_RDOContainerName", "sTGCRDO" };
  Gaudi::Property<std::string> m_NSWsTGC_PRDContainerName{this, "NSWsTGC_PRDContainerName","STGC_Measurements" };

  Gaudi::Property<std::string> m_NSWMM_ContainerName{this, "NSWMM_ContainerName", "MicromegasSensitiveDetector" };
  Gaudi::Property<std::string> m_NSWMM_SDOContainerName{this, "NSWMM_SDOContainerName", "MM_SDO"};
  Gaudi::Property<std::string> m_NSWMM_DigitContainerName{this, "NSWMM_DigitContainerName","MM_DIGITS"} ;
  Gaudi::Property<std::string> m_NSWMM_RDOContainerName{this, "NSWMM_RDOContainerName", "MMRDO"};
  Gaudi::Property<std::string> m_NSWMM_PRDContainerName{this, "NSWMM_PRDContainerName", "MM_Measurements"};

  Gaudi::Property<std::string> m_CSC_SimContainerName{this,"CSC_SimContainerName", "CSC_Hits"};
  Gaudi::Property<std::string> m_CSC_SDOContainerName{this,"CSC_SDOContainerName",  "CSCSDO"};
  Gaudi::Property<std::string> m_CSC_DigitContainerName{this, "CSC_DigitContainerName", "CSC_DIGITS"};
  Gaudi::Property<std::string> m_CSC_RDOContainerName{this,"CSC_RDOContainerName",  "CSCRDO"};
  Gaudi::Property<std::string> m_CSC_PRDContainerName{this,"CSC_PRDContainerName", "CSC_Clusters"  };

  Gaudi::Property<std::string> m_MDT_SimContainerName{this, "MDT_SimContainerName", "MDT_Hits" };
  Gaudi::Property<std::string> m_MDT_SDOContainerName{this, "MDT_SDOContainerName", "MDT_SDO" };
  Gaudi::Property<std::string> m_MDT_DigitContainerName{this, "MDT_DigitContainerName", "MDT_DIGITS"};
 
  Gaudi::Property<std::string> m_RPC_SimContainerName{this, "RPC_SimContainerName", "RPC_Hits"};
  Gaudi::Property<std::string> m_RPC_SDOContainerName{this, "RPC_SDOContainerName", "RPC_SDO"};
  Gaudi::Property<std::string> m_RPC_DigitContainerName{this ,"RPC_DigitContainerName", "RPC_DIGITS"} ;
 
  Gaudi::Property<std::string> m_TGC_SimContainerName{this, "TGC_SimContainerName", "TGC_Hits"};
  Gaudi::Property<std::string> m_TGC_SDOContainerName{this, "TGC_SDOContainerName", "TGC_SDO" };
  Gaudi::Property<std::string> m_TGC_DigitContainerName{this, "TGC_DigitContainerName","TGC_DIGITS"  };
  Gaudi::Property<std::string> m_TGC_RDOContainerName{this,"TGC_RDOContainerName", "TGCRDO" };
  Gaudi::Property<std::string> m_TGC_PRDContainerName{this, "TGC_PRDContainerName","TGC_Measurements" };

  // Matching algorithm
 
  Gaudi::Property<bool> m_doNSWMatching{this, "doNSWMatchingAlg", true};
  Gaudi::Property<bool> m_doNSWMatchingMuon{this, "doNSWMatchingMuonOnly", false};
  Gaudi::Property<uint> m_maxStripDiff{this, "setMaxStripDistance", 3};
  // this property is temporarely added to be able to deactivate the "No match found!" warning when running on the grid
  Gaudi::Property<bool>  m_noMatchWarning{this, "suppressNoMatchWarning", false};
};

#endif // NSWPRDVALALG_H
