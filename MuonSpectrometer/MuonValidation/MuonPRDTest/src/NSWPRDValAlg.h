/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWPRDVALALG_H
#define NSWPRDVALALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>

class MMDigitVariables;
class MMSimHitVariables;
class MMFastDigitVariables;

class sTGCDigitVariables;
class sTGCSimHitVariables;
class sTGCFastDigitVariables;

class CSCDigitVariables;

class MuEntryVariables;
class TruthVariables;

class ITHistSvc;
class TTree;
class MmIdHelper;
class sTgcIdHelper;
class CscIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

class NSWPRDValAlg:public AthAlgorithm
{
 public:
  NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  TruthVariables*         m_TruthVar;
  MuEntryVariables*       m_MuEntryVar;
  sTGCSimHitVariables*    m_sTgcSimHitVar;
  sTGCFastDigitVariables* m_sTgcFastDigitVar;
  sTGCDigitVariables*     m_sTgcDigitVar;
  MMSimHitVariables*      m_MmSimHitVar;
  MMFastDigitVariables*   m_MmFastDigitVar;
  MMDigitVariables*       m_MmDigitVar;
  CSCDigitVariables*      m_CscDigitVar;

  ITHistSvc *m_thistSvc;
  TTree *m_tree;

  const MuonGM::MuonDetectorManager* m_detManager;
  const MmIdHelper*   m_MmIdHelper;
  const sTgcIdHelper* m_sTgcIdHelper;
  const CscIdHelper*  m_CscIdHelper;

  BooleanProperty  m_doTruth;            // switch on the output of the MC truth
  BooleanProperty  m_doMuEntry;          // switch on the output of the Muon Entry Layer
  BooleanProperty  m_doSTGCHit;          // switch on the output of the Small TGC data
  BooleanProperty  m_doSTGCFastDigit;    // switch on the output of the Small TGC fast digitization
  BooleanProperty  m_doSTGCDigit;        // swicth on the output of the Small TGC digit
  BooleanProperty  m_doMMHit;            // switch on the output of the MicroMegas data
  BooleanProperty  m_doMMFastDigit;      // switch on the output of the MicroMegas fast digitization
  BooleanProperty  m_doMMDigit;          // switch on the output of the MicroMegas digitization
  BooleanProperty  m_doCSCDigit;          // switch on the output of the MicroMegas digitization

  unsigned int m_runNumber;
  unsigned int m_eventNumber;

  std::string m_Truth_ContainerName;
  std::string m_MuEntry_ContainerName;
  std::string m_NSWsTGC_ContainerName;
  std::string m_NSWsTGC_FastDigitContainerName;
  std::string m_NSWsTGC_DigitContainerName;
  std::string m_NSWMM_ContainerName;
  std::string m_NSWMM_FastDigitContainerName;
  std::string m_NSWMM_DigitContainerName;
  std::string m_CSC_DigitContainerName;
};

#endif // NSWHITSDUMPALG_H
