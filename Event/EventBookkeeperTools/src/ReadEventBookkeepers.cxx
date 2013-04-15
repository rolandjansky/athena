/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Creation: David Cote, September 2008. <david.cote@cern.ch>
// Authors: Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
#include "EventBookkeeperTools/ReadEventBookkeepers.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <vector>

ReadEventBookkeepers::ReadEventBookkeepers(const std::string& name, ISvcLocator* pSvcLocator) :
  // *****************************************************
  Algorithm(name, pSvcLocator),
  m_cutflowSvc("CutFlowSvc/CutFlowSvc",name),
  m_storeGate(0), m_metaStore(0)
{
  declareProperty("EventBookkeeperCollectionName",m_EvtBookCollName = "EventBookkeepers");
  declareProperty("StreamName",m_InputStreamName = "InputStreamName");
  declareProperty("counter",m_counter = 0);
  declareProperty("cutfSvc", m_cutflowSvc);

  m_counter=0;
}

ReadEventBookkeepers::~ReadEventBookkeepers() {

}

StatusCode ReadEventBookkeepers::initialize(){
  //*******************************************************
  MsgStream log(msgSvc(), name());
  
  outputFile = new TFile("CutFlow.root","recreate");
  auxTree = new TTree("Bookkeep","Bookkeep");
  auxTree->Branch("filter_name", &m_name,"m_name/C");
  auxTree->Branch("filter_entries", &m_entries,"m_entries/I");

                                                                                
  // Locate the StoreGateSvc and initialize our local ptr
  // ****************************************************
  // Get the ICutFlowSvc
  StatusCode sc = m_cutflowSvc.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Cannot get ICutFlowSvc interface." << endreq;
    return(sc);
  }  
  
  sc = service("StoreGateSvc", m_storeGate);
  if (!sc.isSuccess() || 0 == m_storeGate) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return StatusCode::FAILURE;
  } 
  
  sc = service("StoreGateSvc/InputMetaDataStore", m_metaStore);
  //sc = service("StoreGateSvc/MetaDataStore", m_metaStore);
  if (!sc.isSuccess() || 0 == m_metaStore) {
    log << MSG::ERROR << "Could not find MetaDataStore" << endreq;
    return StatusCode::FAILURE;
  } 
  
  return StatusCode::SUCCESS;
}


StatusCode ReadEventBookkeepers::execute() {
  MsgStream log(msgSvc(), name());
  
  const DataHandle<EventBookkeeperCollection> coll;
  StatusCode sc = m_metaStore->retrieve(coll, m_EvtBookCollName);
  
  if(m_counter==0){


    if (!sc.isSuccess()) {
      log<<MSG::WARNING <<" There is NO EventBookkeeper collection in this FILE, "<<endreq;       
    }else{
      
      std::vector<EventBookkeeper*> m_vector;
      
      int counnter;
      counnter = GetCycle();
      if(counnter==0){
      
	SetCycle();	
	std::string inputName = m_InputStreamName; 

	// Each accepted loop entry counts in the tree as en event
	// So there is a 1-1 relation for the filter name and NAcceptedEvents
	std::ofstream myfile;
	myfile.open ("CutFlow.txt"); 
	
	// First loop through all the elements to find the biggest cycle
	int biggestCycle =0; 
	int counterA = 0;
	EventBookkeeperCollection::const_iterator itro=coll->begin(), itroEnd = coll->end();
	for(; itro != itroEnd; itro++){
	  int nCycle = 9;
	  nCycle = (*itro)->getCycle();
	  if(nCycle>biggestCycle) biggestCycle = nCycle;
	  
	  if(counterA==0 && nCycle==1){	    
	    myfile<<"Initial Stream : "<<(*itro)->getInputStream()<<  "\n";
	    counterA++;
	  }
	}
	
	counterA = 0;
	EventBookkeeperCollection::const_iterator itroo=coll->begin(), itrooEnd = coll->end();
	for(; itroo != itrooEnd; itroo++){
	  if((*itroo)->getName()=="AllEvents" && counterA==0){
	    log<<MSG::DEBUG << "Starting from "<<(*itroo)->getName()<<", description "<<(*itroo)->getDescription()<<endreq;
	    log<<MSG::DEBUG << "--> Events "<<(*itroo)->getNAcceptedEvents()<<endreq;
	    log<<MSG::DEBUG << "------------------------------------------------------------------------------------"<<endreq;

	    myfile << " Total number of events processed : "<<(*itroo)->getNAcceptedEvents()<<  "\n";
	    myfile << " Number of skim cycles executed : "<<biggestCycle<<  "\n";
	    myfile << "-------------------------------------------- \n";
	    myfile<<"\n";
	    counterA++;	    
	  }
	}
	
	// Second loop to find all EventBookkeeper objects that have the same name as inputFile 
	// and subsequent search for their parents/grandparents and respective selection efficiency
	
	std::string cond_name = inputName;
	int cond_cycle = biggestCycle;
	std::string tmp_name = "";
	int tmp_cycle = 9; 

	for (int iter =(int)biggestCycle ; iter >=0; iter --){
	  
	  EventBookkeeperCollection::const_iterator itrw=coll->begin(), itrwEnd = coll->end();
	  int cond = -1;
	  for(; itrw != itrwEnd; itrw++){
	    cond = -1;      
	    cond = (*itrw)->getName().find(cond_name);
	    int cycle = (*itrw)->getCycle(); 
	    std::string logic = (*itrw)->getLogic();	    
	    	    
	    if( cycle == cond_cycle && cond>0 && logic=="Accept"){ 
	    
	      // found the streams in the last cycle that have a stream name compatible with some filters
	      std::string inputstream=(*itrw)->getInputStream();
	      std::string name=(*itrw)->getName();	
	      uint64_t nEvt= (*itrw)->getNAcceptedEvents();
	      log<<MSG::DEBUG << " FOUND A FILTER "<<name << " from a file "<<cond_name<<" with "<<nEvt<<"   accepted Events "<<endreq;
	      log<<MSG::DEBUG << "       ------->  cycle "<<(*itrw)->getCycle()<<" and cond_cycle is "<<cond_cycle<<endreq;
	      log<<MSG::DEBUG << "       -------> Input Stream"<<inputstream <<" and OutputStream "<<(*itrw)->getOutputStream()<<" and logic "<<(*itrw)->getLogic()<<endreq;
	      // Now for this stream re-loop through all the streams and find one with cycle-1 which corresponds to ParentStream
	      tmp_name = inputstream;
	      tmp_cycle = (*itrw)->getCycle() - 1; 
	      EventBookkeeper *tmpBook = new EventBookkeeper();
	      tmpBook->setCycle((*itrw)->getCycle());
	      tmpBook->setOutputStream((*itrw)->getOutputStream());
	      tmpBook->setName((*itrw)->getName());
	      tmpBook->setNAcceptedEvents((*itrw)->getNAcceptedEvents());
	      tmpBook->setNWeightedAcceptedEvents((*itrw)->getNWeightedAcceptedEvents());
	      tmpBook->setDescription((*itrw)->getDescription());
	      tmpBook->setInputStream((*itrw)->getInputStream());

	      m_vector.push_back(tmpBook);
	    }
	  }
	  cond_name = tmp_name;
	  cond_cycle = tmp_cycle;
	}	

	log<<MSG::DEBUG << "----------------------------------------" <<endreq;
	log<<MSG::DEBUG << "  vector size "<<m_vector.size() << endreq;     

	// The information is in the vector
	// Loop the vector and print in screen and txt file the filters
	// Build histo and fill tree and put them in root file
								
	int counterB=0;
	counterA=0;		
	for(int j=1; j< (int)biggestCycle+1;j++){
	  counterB=0;
	  //	  counterA=0;
	  //  if(counterA==0){
	  myfile<<"------------------------------------------------------\n";
	  myfile <<"-- CYCLE : "<<j<<"\n";
	  myfile<<"------------------------------------------------------\n";
	  // }
	  // counterA++;
	  for(int i =0;i<(int)m_vector.size();i++){
	 
	    if((int)m_vector[i]->getCycle()==j ){
	      
	      if(counterB==0){
		myfile <<"-- OutputStream : "<<m_vector[i]->getOutputStream()<<"\n";	 
		myfile <<"-- Filters :"<<"\n";
		std::string name=m_vector[i]->getName();
		uint64_t nEvt= m_vector[i]->getNAcceptedEvents();
		counterB++;
		val=nEvt;
		m_entries=val;
		for(int a=0;a<(int)name.size();a++){
		  m_name[a] = name[a];
		}
		auxTree->Fill();
	      }
	      //myfile << "---- "<<m_vector[i]->getName()<<" with description "<<m_vector[i]->getDescription()<<" that passes "<<m_vector[i]->getNAcceptedEvents()<<" events.\n";
	      myfile << "---- "<<m_vector[i]->getName()<<" that passes "<<m_vector[i]->getNAcceptedEvents()<<" events.\n";
	      myfile << "---- Cuts :"<<"\n";
	      myfile<<"------------------------------------------------------\n";
	    }	    	    	    
	  }
	}
	
	myfile.close();
      }     

      //Test of DumpCutFlowToTTree
      TTree* aTree=m_cutflowSvc->dumpCutFlowToTTree();
      outputFile->Write();
      delete aTree;
    }
    m_counter++;  
  }   
  return (StatusCode::SUCCESS);
}

//*****************************************************
StatusCode ReadEventBookkeepers::finalize() {
  return StatusCode::SUCCESS;
}
 

