/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQuality.cxx                        #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modified: Feb 7/2005                         #
//#####################################################
                                 

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TBEvent/TBEventInfo.h"

#include "TBRec/TBBeamQualityTool.h"
#include "TBRec/TBBeamQuality.h"


TBBeamQuality::TBBeamQuality(const std::string& name,  
			     ISvcLocator* pService)
  : Algorithm(name,pService)
{ //properties
  declareProperty("BQParticles", m_bqparticle=std::vector<std::string>(1,"noselect"));
  declareProperty("BQToolNames", m_bqtools);

  m_storegate = 0;
} //properties

TBBeamQuality::~TBBeamQuality()
{ }

/*--------------- INITIALIZE ---------------*/

StatusCode TBBeamQuality::initialize()
{//init
  // message service
  MsgStream log(messageService(),name());
  log<<MSG::DEBUG
     <<"Initialize"
     <<endreq;
  
  // assigning StoreGate
  StatusCode sc = service("StoreGateSvc", m_storegate);
  if( sc.isFailure() ) { 
      log << MSG::ERROR 
	  << "Can't assign StoreGateSvc" 
	  << endreq;
      return sc;
  } 

  sc = m_bqtools.retrieve();  
  if (sc.isFailure() ) {
	  log<< MSG::ERROR << "failed to pick up tools  " << endreq; 
	  return sc;
  }

  // check tools
  if ( m_bqtools.size() == 0 ) {
      log << MSG::ERROR
	  << "TBBeamQuality: No Tools found"
	  <<endreq;
      return StatusCode::FAILURE;
  } else {
     for(unsigned int i = 0; i < m_bqtools.size(); ++i) {
        m_bqacceptCounter[m_bqtools[i]] = 0;
        m_bqrejectCounter[m_bqtools[i]] = 0;    
        m_bqtotalCounter[m_bqtools[i]] =0;
     }
  }
  
  // print out list of tools
  log << MSG::INFO <<" "<< endreq;
  log << MSG::INFO <<" TBBeamQuality: List of Tools:"
      <<endreq;
  log << MSG::INFO 
      <<"-----------------------------------"<<endreq;
  
  unsigned int toolCtr = 0;
  ToolHandleArray<TBBeamQualityTool>::iterator first_tool = m_bqtools.begin();
  ToolHandleArray<TBBeamQualityTool>::iterator last_tool = m_bqtools.end();
  for ( ;first_tool != last_tool; first_tool++)
    { 
      toolCtr ++;
      log<< MSG::INFO 
	 <<" Tool Name: "
	 <<(*first_tool)->name()
	 <<" Tool Type: "
	 <<(*first_tool)->type()
	 <<endreq;
    }
  return StatusCode::SUCCESS;
}//init

/*---------------EXECUTE---------------*/
StatusCode TBBeamQuality::execute()
{ // execute
  // message service
  MsgStream log(messageService(),name());
  log<<MSG::DEBUG
     <<"Execute"
     <<endreq;
  
  if(m_bqparticle[0]=="noselect") { //obtain particle type
    // determining particel type
    const TBEventInfo* eventInfo;
    StatusCode sc = StoreGate::pointer()->retrieve(eventInfo);
    if (sc.isFailure())
    {
      log << MSG::ERROR << "TBBeamQuality Algo Tool: Could not retrieve event info" << endreq;
    }
    std::string particle=eventInfo->getBeamParticle();
    m_bqparticle=std::vector<std::string>(1,particle);
    log<<MSG::INFO<<"Beam Quality selected default particle: "<<m_bqparticle[0]<<endreq;
  }
 
  // invoke tools
  ToolHandleArray<TBBeamQualityTool>::iterator first_Tool = m_bqtools.begin();
  ToolHandleArray<TBBeamQualityTool>::iterator last_Tool = m_bqtools.end();
  
  // setting event to success
  bool successFlag = true;
  while ( successFlag && first_Tool != last_Tool )
    {
      m_bqtotalCounter[*first_Tool]++;
      successFlag = (*first_Tool)->accept(m_bqparticle) == StatusCode::SUCCESS;
      if ( successFlag )
	{
	  m_bqacceptCounter[*first_Tool]++;
	}
      else
	{
	  m_bqrejectCounter[*first_Tool]++;
	}
      first_Tool++;
    }
  
  setFilterPassed(successFlag);
  return StatusCode::SUCCESS;
} //execute

/*---------------FINALIZE---------------*/
StatusCode TBBeamQuality::finalize()
{ //finalize
  // message service
  MsgStream log(messageService(),name());
  
  // print summary
  ToolHandleArray<TBBeamQualityTool>::iterator first_Tool = m_bqtools.begin();
  ToolHandleArray<TBBeamQualityTool>::iterator last_Tool = m_bqtools.end();
  
  log<< MSG::INFO
     << "************ Beam Quality Tool ************"
     <<endreq;
  for (; first_Tool != last_Tool; first_Tool++ )
    {
      log << MSG ::INFO
	  <<(*first_Tool)->name() <<" : "
	  << "(Total/Accept/Rejected)"
	  << " ("
	  << m_bqtotalCounter[*first_Tool]  << "/"
	  << m_bqacceptCounter[*first_Tool] << "/"
	  << m_bqrejectCounter[*first_Tool] 
	  << ")"
	  << endreq;
    } 
  return StatusCode::SUCCESS;
} //finalize



