/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/MuonConditionsTestAlg.h"
#include "MuonCondSvc/MuonHierarchy.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"


MuonConditionsTestAlg::MuonConditionsTestAlg(
                         const std::string& name,
                         ISvcLocator* pSvcLocator ) :
                         AthAlgorithm( name, pSvcLocator ),
			 m_pSummarySvc("MDTCondSummarySvc", name),
			 m_rSummarySvc("RPCCondSummarySvc", name),
			 m_tSummarySvc("TGCCondSummarySvc", name),
			 m_vSummarySvc("CSCCondSummarySvc", name){
			
}

MuonConditionsTestAlg::~MuonConditionsTestAlg()
{
  msg(MSG::INFO) << "Calling destructor" << endreq;
}

//Initialize
StatusCode
MuonConditionsTestAlg::initialize(){
 
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endreq;
  sc = m_pSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endreq;
  }
  sc = m_rSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endreq;
  }

  sc = m_tSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endreq;
  }

  sc = m_vSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endreq;
  }


  return sc;
}

//Execute
StatusCode
MuonConditionsTestAlg::execute(){
  StatusCode sc(StatusCode::SUCCESS);

  msg(MSG::INFO)<< "Calling execute" << endreq;
  msg(MSG::INFO)<<"Dummy call for the MDT STATUS"<<endreq;
  msg(MSG::INFO)<<"THE CHAMBER SWITCHED OFF ARE: "<<endreq;
  m_pSummarySvc->deadStations();
  unsigned int size =m_pSummarySvc->deadStations().size();
  msg(MSG::INFO) << "writing from Algo SERVICE \n" <<size <<endreq;
  for(unsigned int k=0;k<size;k++){
    std::string chamber = (m_pSummarySvc->deadStations()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" <<chamber <<endreq;
  } 

  std::cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
  if (m_pSummarySvc->isGoodChamber(Identifier(1699348480))) msg(MSG::VERBOSE) << "The chamber is good\n" <<endreq;
  else  msg(MSG::INFO) << "The chamber is not good\n" <<endreq; 

 if (m_pSummarySvc->isGoodChamber(Identifier(1834287104))) msg(MSG::VERBOSE) << "The chamber is good\n" <<endreq;
  else  msg(MSG::INFO) << "The chamber is not good\n" <<endreq; 


  unsigned int size_id =m_pSummarySvc->deadStationsId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_id <<endreq;
  for(unsigned int k=0;k<size_id;k++){
    Identifier Id = (m_pSummarySvc->deadStationsId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" << Id<<endreq;
  } 
std::cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;	

  unsigned int size_id2 =m_pSummarySvc->deadMultiLayersId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_id2 <<endreq;
  for(unsigned int k=0;k<size_id2;k++){
    Identifier Id = (m_pSummarySvc->deadMultiLayersId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE MULTILAYERS \n" << Id<<endreq;
  }



  std::cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
  m_pSummarySvc->deadTubes();
  unsigned int size2 =m_pSummarySvc->deadTubes().size();
  msg(MSG::INFO) << "writing from Algo SERVICE TUBES\n" <<size2 <<endreq;
  std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
  for(unsigned int k=0;k<size2;k++){
    std::string chamber = (m_pSummarySvc->deadTubes()[k]);
    msg(MSG::VERBOSE) << "writing from Algo SERVICE DEAD TUBES \n" <<chamber <<endreq;
  } 

  m_pSummarySvc->deadTubesId();
  unsigned int size2tube =m_pSummarySvc->deadTubesId().size();
  msg(MSG::VERBOSE) << "writing from Algo SERVICE TUBES\n" <<size2tube <<endreq;
  std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
  

  for(unsigned int k=0;k<size2tube;k++){
    Identifier Id = (m_pSummarySvc->deadTubesId()[k]);
    msg(MSG::VERBOSE) << "writing from Algo SERVICE TUBES \n" << Id<<endreq;
  }


  msg(MSG::INFO)<<"Dummy call for the RPC STATUS"<<endreq;
  msg(MSG::INFO)<<"THE Panels efficiencies ARE: "<<endreq;
  m_rSummarySvc->EffPanelId();
 unsigned int size_rpc =m_rSummarySvc->EffPanelId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_rpc <<endreq;
  for(unsigned int k=0;k<size_rpc;k++){
    Identifier Id = (m_rSummarySvc->EffPanelId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" << Id<<endreq;
  }

//   m_simSummarySvc->RPC_EfficiencyMap();
//   msg(MSG::INFO) << "writing from Algo SERVICE Identifier Map NEWWWWW\n" <<endreq;
//   //int size_map = m_rSummarySvc->RPC_EfficiencyMap().size(); 
//     msg(MSG::VERBOSE) << "*************** MAp size is\n" << size<< endreq; 
//    std::map<Identifier, double>::iterator iter; 
   
  //double RPC_eff = (RPC_EfficiencyMap->second); 
  //double RPC_id = (RPC_EfficiencyMap->first);
  std::cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;


 unsigned int size_tgc =m_tSummarySvc->deadStationsId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_tgc <<endreq;
  for(unsigned int k=0;k<size_tgc;k++){
    Identifier Id = (m_tSummarySvc->deadStationsId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" << Id<<endreq;
  }
std::cout<< "*****************CSC *****************************" << std::endl;


 unsigned int size_csc =m_vSummarySvc->deadStationsId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_csc <<endreq;
  for(unsigned int k=0;k<size_csc;k++){
    Identifier Id = (m_vSummarySvc->deadStationsId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER CSC *******\n" << Id<<endreq;
  }

 unsigned int size_csc2 =m_vSummarySvc->deadWireLayersId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_csc2 <<endreq;
  for(unsigned int k=0;k<size_csc2;k++){
    Identifier Id = (m_vSummarySvc->deadWireLayersId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE WIRE Layers CSC *******\n" << Id<<endreq;
  }

 unsigned int size_csc4 =m_vSummarySvc->deadStations().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_csc4 <<endreq;
  for(unsigned int k=0;k<size_csc4;k++){
    std::string  chamber= (m_vSummarySvc->deadStations()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER CSC *******\n" << chamber<<endreq;
  }

 unsigned int size_csc5 =m_vSummarySvc->deadWireLayers().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_csc5 <<endreq;
  for(unsigned int k=0;k<size_csc5;k++){
    std::string wire = (m_vSummarySvc->deadWireLayers()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE WIRE Layers CSC *******\n" << wire<<endreq;
  }






  return sc;
}


//Finalize
StatusCode
MuonConditionsTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endreq;
  return sc;
}
