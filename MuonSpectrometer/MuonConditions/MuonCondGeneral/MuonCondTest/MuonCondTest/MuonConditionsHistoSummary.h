/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonConditionsHistoSummary_H
#define MuonConditionsHistoSummary_H 
//STL
#include <string>

//Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
//Athena
#include "TH1.h"
#include "TTree.h"
 
//Athena

#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"

//Forward declarations
class ISvcLocator;
class StatusCode;

class MuonConditionsHistoSummary : public AthAlgorithm {
 public:
  MuonConditionsHistoSummary(const std::string &name,ISvcLocator *pSvcLocator) ;
 ~MuonConditionsHistoSummary();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  ServiceHandle<IMDTConditionsSvc> m_pSummarySvc;
  ServiceHandle<IRPCConditionsSvc> m_rSummarySvc;
  ServiceHandle<ITGCConditionsSvc> m_tSummarySvc; 
  
  ITHistSvc* m_thistSvc;
//MDT stuff 
std::string deadmultilayer;
std::string deadtube;
std::string deadstat;


 std::string deadstat_id;
 std::string deadmultilayer_id;
 std::string deadtube_id;
 
  
 TTree * m_t_deadstations;
  TTree * m_t_deadmultilayer;
  TTree * m_t_deadtube;

 //RPC stuff 
 std::string panelid ;
 std::string stripid ;
 
  TTree * m_t_panelid;
  TTree * m_t_stripid;
  
  
  
  
 
 

}; //end of class

#endif
