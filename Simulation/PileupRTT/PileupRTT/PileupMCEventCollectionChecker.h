/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>
#include <list>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "McParticleEvent/TruthParticle.h"
//#include "McParticleEvent/TruthParticleContainer.h"

#include "GaudiKernel/MsgStream.h"          
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"

#include "StoreGate/StoreGateSvc.h"            
#include "StoreGate/DataHandle.h"
/*
#include "AthenaKernel/DefaultKey.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
*/

//////////////////////////////////////////////////////////////

class ITHistSvc;
class TTree;

namespace PileupRTT
{

class PileupMCEventCollectionChecker : public AthAlgorithm {
 public:
  
  PileupMCEventCollectionChecker (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  
  // string for MCEventCollection key
  std::string m_mcTruthInfo;

  // string for PileUpEventInfo key
  std::string m_pileUpEventInfo;

  // string to hold outputfile name of MC Info
  std::string m_outfile;

  // flag for writing MC Info to text file
  bool m_dumpMCInfo;

  int m_nExecute;
  int m_nCollections;
  double m_nCollectionsWrong;
  int m_nSubEvents;
  int m_nNonSeparatorGenEvents;
  int m_nSubEventsTotal;
  int m_nNonSeparatorGenEventsTotal;

  std::vector<int>* m_MCEventCollectionNo;
  std::vector<int>* m_GenEventNo;
  std::vector<int>* m_BugTest;
  std::vector<int>* m_NNonSeparatorGenEvents;
  std::vector<int>* m_NSubEvents;
  std::vector<int>* m_SubEventTest;
  std::vector<int>* m_BCID;

  TTree* m_theTree;
  ITHistSvc * m_thistSvc; 

};
}



