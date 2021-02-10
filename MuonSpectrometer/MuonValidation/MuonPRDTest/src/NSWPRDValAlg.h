/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWPRDVALALG_H
#define NSWPRDVALALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "EDM_object.h"

#include <vector>

class MDTSimHitVariables;
class RPCSimHitVariables;
class RPCDigitVariables;
class CSCSimHitVariables;
class TGCSimHitVariables;
class MMDigitVariables;
class MMSimHitVariables;
class MMSDOVariables;
class MMRDOVariables;
class MMPRDVariables;

class sTGCDigitVariables;
class sTGCSimHitVariables;
class sTGCSDOVariables;
class sTGCRDOVariables;
class sTGCPRDVariables;

class CSCDigitVariables;

class MuEntryVariables;
class TruthVariables;

class ITHistSvc;
class TTree;

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
  TruthVariables*         m_TruthVar;
  MuEntryVariables*       m_MuEntryVar;
  sTGCSimHitVariables*    m_sTgcSimHitVar;
  sTGCRDOVariables*       m_sTgcRdoVar;
  sTGCSDOVariables*       m_sTgcSdoVar;
  sTGCSDOVariables*       m_sTgcFastSdoVar;
  sTGCDigitVariables*     m_sTgcDigitVar;
  sTGCPRDVariables*       m_sTgcPrdVar;
  MMSimHitVariables*      m_MmSimHitVar;
  MMSDOVariables*         m_MmSdoVar;
  MMSDOVariables*         m_MmFastSdoVar;
  MMDigitVariables*       m_MmDigitVar;
  MMRDOVariables*         m_MmRdoVar;
  MMPRDVariables*         m_MmPrdVar;
  CSCDigitVariables*      m_CscDigitVar;
  MDTSimHitVariables*     m_MDTSimHitVar;
  RPCSimHitVariables*     m_RPCSimHitVar;
  RPCDigitVariables*      m_RPCDigitVar;
  CSCSimHitVariables*     m_CSCSimHitVar;
  TGCSimHitVariables*     m_TGCSimHitVar;

  ITHistSvc *m_thistSvc;
  TTree *m_tree;

  // MuonDetectorManager from the Detector Store (to be used only at initialize)
  const MuonGM::MuonDetectorManager* m_muonDetMgrDS;

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  BooleanProperty  m_isData;             // if false use MuonDetectorManager from detector store everywhere
  BooleanProperty  m_doTruth;            // switch on the output of the MC truth
  BooleanProperty  m_doMuEntry;          // switch on the output of the Muon Entry Layer
  BooleanProperty  m_doSTGCHit;          // switch on the output of the Small TGC data
  BooleanProperty  m_doSTGCFastDigit;    // switch on the output of the Small TGC fast digitization
  BooleanProperty  m_doSTGCDigit;        // swicth on the output of the Small TGC digit
  BooleanProperty  m_doSTGCRDO;          // switch on the output of the Small TGC RDO
  BooleanProperty  m_doSTGCPRD;          // swicth on the output of the Small TGC prepdata
  BooleanProperty  m_doMMHit;            // switch on the output of the MicroMegas data
  BooleanProperty  m_doMMFastDigit;      // switch on the output of the MicroMegas fast digitization
  BooleanProperty  m_doMMDigit;          // switch on the output of the MicroMegas digitization
  BooleanProperty  m_doMMRDO;            // switch on the output of the MicroMegas RDO
  BooleanProperty  m_doMMPRD;            // switch on the output of the MicroMegas prepdata
  BooleanProperty  m_doCSCDigit;         // switch on the output of the MicroMegas digitization
  BooleanProperty  m_doMDTHit;
  BooleanProperty  m_doRPCHit;           // switch on the output of the RPC data   
  BooleanProperty  m_doRPCDigit;         // switch on the output of the RPC digitization 
  BooleanProperty  m_doCSCHit;
  BooleanProperty  m_doTGCHit;

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
  std::string m_CSC_DigitContainerName;
  std::string m_MDT_SimContainerName;
  std::string m_RPC_DigitContainerName;
  std::string m_RPC_SimContainerName;
  std::string m_CSC_SimContainerName;
  std::string m_TGC_SimContainerName;

  // Matching algorithm
  BooleanProperty m_doNSWMatching;
  BooleanProperty m_doNSWMatchingMuon;
  uint m_maxStripDiff;
  bool  m_noMatchWarning;
};

#endif // NSWPRDVALALG_H
