/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/MuonConditionsHistoSummary.h"
#include "MuonCondSvc/MuonHierarchy.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"


MuonConditionsHistoSummary::MuonConditionsHistoSummary(
                         const std::string& name,
                         ISvcLocator* pSvcLocator ) :
                         AthAlgorithm( name, pSvcLocator ),
			 m_pSummarySvc("MDTCondSummarySvc", name),
			 m_rSummarySvc("RPCCondSummarySvc", name),
			 m_tSummarySvc("TGCCondSummarySvc", name),
			 m_thistSvc(NULL),
			 m_t_deadstations(NULL),
			 m_t_deadmultilayer(NULL),
			 m_t_deadtube(NULL),
			 m_t_panelid(NULL),
			 m_t_stripid(NULL) {
			
}

MuonConditionsHistoSummary::~MuonConditionsHistoSummary()
{
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

//Initialize
StatusCode
MuonConditionsHistoSummary::initialize(){
 
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endmsg;
  sc = m_pSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endmsg;
  }
  sc = m_rSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endmsg;
  }

  sc = m_tSummarySvc.retrieve();

  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endmsg;
  }
sc = service("THistSvc", m_thistSvc);
    if(StatusCode::SUCCESS not_eq sc)
    {
       msg(MSG::ERROR)
             << "Unable to retrieve pointer to THistSvc"
             << endmsg;
        return sc;
    }

    
  
 //!mdt stuff  
m_t_deadmultilayer= new TTree("t_deadmultilayer"," dead multilayers");
 m_t_deadtube= new TTree("t_deadtube"," dead tubes ");
m_t_deadstations= new TTree("t_deadstations"," Dead Stations ");

m_t_deadstations->Branch("deadstat",&m_deadstat);
m_t_deadstations->Branch("deadstat_id",&m_deadstat_id);
m_t_deadmultilayer->Branch("deadmultilayer",&m_deadmultilayer);
m_t_deadmultilayer->Branch("deadmultilayer_id",&m_deadmultilayer_id);
m_t_deadtube->Branch("deadtube",&m_deadtube);
m_t_deadtube->Branch("deadtube_id",&m_deadtube_id);

 

  m_t_panelid= new TTree("t_panelid"," eff panel id ");
  m_t_stripid= new TTree("t_stripid","eff strip id  "); 

//!rpc stuff stuff 
 sc = m_thistSvc->regTree("/file1/t_deadmultilayer", m_t_deadmultilayer); 
sc = m_thistSvc->regTree("/file1/t_deadtube", m_t_deadtube);
sc = m_thistSvc->regTree("/file1/t_deadstations", m_t_deadstations);
sc = m_thistSvc->regTree("/file1/t_panelid", m_t_panelid);
sc = m_thistSvc->regTree("/file1/t_stripid", m_t_stripid);




  if(StatusCode::SUCCESS not_eq sc) {
      msg(MSG::ERROR)<< "Could not register m_h_MdtDeadChambers "<< endmsg;
    return sc;
  }

  return sc;
}

//Execute
StatusCode
MuonConditionsHistoSummary::execute(){
  StatusCode sc(StatusCode::SUCCESS);


  msg(MSG::INFO)<< "Calling execute" << endmsg;
  msg(MSG::INFO)<<"Dummy call for the MDT STATUS"<<endmsg;  
  msg(MSG::INFO)<<"THE CHAMBER SWITCHED OFF ARE: "<<endmsg;
   
 
  
  if(m_pSummarySvc->deadStations().size()==m_pSummarySvc->deadStationsId().size()){msg(MSG::INFO) << "id normalcomparionssuccessful"  <<endmsg;
 
  for(unsigned int k=0;k<m_pSummarySvc->deadStations().size();k++){
     
    m_deadstat = (m_pSummarySvc->deadStations()[k]);
    m_deadstat_id = (m_pSummarySvc->deadStationsId()[k]).get_compact();           
    m_t_deadstations->Fill();
  
  } 
  }
    if(m_pSummarySvc->deadMultiLayers().size()==m_pSummarySvc->deadMultiLayersId().size()){msg(MSG::INFO) << "id normalcomparionssuccessful"  <<endmsg;
  for(unsigned int k=0;k<m_pSummarySvc->deadMultiLayers().size();k++){
     m_deadmultilayer= (m_pSummarySvc->deadMultiLayers()[k]);
    m_deadmultilayer_id=(m_pSummarySvc->deadMultiLayersId()[k]).get_compact();           
    m_t_deadmultilayer->Fill();
  } 
  }
     if(m_pSummarySvc->deadTubes().size()==m_pSummarySvc->deadTubesId().size()){msg(MSG::INFO) << "id normalcomparionssuccessful"  <<endmsg;
  for(unsigned int k=0;k<m_pSummarySvc->deadTubes().size();k++){
     
     m_deadtube= (m_pSummarySvc->deadTubes()[k]);
    m_deadtube_id=(m_pSummarySvc->deadTubesId()[k]).get_compact();           
    m_t_deadtube->Fill();
  } 
  }
  
  


  


  msg(MSG::INFO)<<"Dummy call for the RPC STATUS"<<endmsg;
  msg(MSG::INFO)<<"THE Panels efficiencies ARE: "<<endmsg;
 
 
  for(unsigned int k=0;k<m_rSummarySvc->EffPanelId().size();k++){
    m_panelid= (m_rSummarySvc->EffPanelId()[k]).get_compact();
    m_t_panelid->Fill();
  }
   for(unsigned int k=0;k<m_rSummarySvc->EffStripId().size();k++){
    m_stripid= (m_rSummarySvc->EffStripId()[k]).get_compact(); 
      m_t_stripid->Fill();
  }
  
std::cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;

/*
 int size_tgc =m_tSummarySvc->deadStationsId().size();
  msg(MSG::INFO) << "writing from Algo SERVICE Identifier\n" <<size_tgc <<endmsg;
  for(unsigned int k=0;k<size_tgc;k++){
    Identifier Id = (m_tSummarySvc->deadStationsId()[k]);
    msg(MSG::INFO) << "writing from Algo SERVICE CHAMBER \n" << Id<<endmsg;
  }
std::cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4" << std::endl;
*/
  return sc;
}


//Finalize
StatusCode
MuonConditionsHistoSummary::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endmsg;
  return sc;
}
