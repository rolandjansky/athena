/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "IPileupValidationTools.h"
#include "StoreGate/StoreGateSvc.h"

#include "TTree.h"

class StoreGateSvc;
class ITHistSvc;
class TTree;

namespace PileupRTT
{

class PileupSimpleEventInfo : public AthAlgorithm {
public:

  PileupSimpleEventInfo(const std::string &name, ISvcLocator *pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  //virtual StatusCode CBNT_clear();

private:

  //std::string m_eventInfo;

  //sequential event number
  long m_iEvent ;
  
  // ntuple variables;
  unsigned int    m_runNumber;
  unsigned int    m_eventNumber;
  unsigned int    m_eventTime;
  unsigned int    m_lumiBlock;
  unsigned int    m_bCID;
  unsigned int    m_lVL1ID;
  long    m_iEventNumber;
  double  m_eventWeight;
  unsigned int    m_statusElement;
  unsigned int    m_lvl1TriggerType;
  std::vector<unsigned int>* m_lvl1TriggerInfo;
  std::vector<unsigned int>* m_lvl2TriggerInfo;
  std::vector<unsigned int>* m_evtFilterInfo;
  std::vector<std::string>* m_streamTagName;
  std::vector<std::string>* m_streamTagType;

  TTree* m_theTree;
  ITHistSvc * m_thistSvc; 
  
};
}
