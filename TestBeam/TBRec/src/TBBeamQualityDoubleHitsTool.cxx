/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityDoubleHitsTool.cxx          #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modified: Feb 7/2005                         #
//#####################################################


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/AlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Property.h"

#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBScintillator.h"
#include "TBRec/TBBeamQualityDoubleHitsTool.h"

#include <string>
#include <ios>
#include <algorithm>

TBBeamQualityDoubleHitsTool::TBBeamQualityDoubleHitsTool(const std::string& name,
							 const std::string& type,
							 const IInterface* parent)
  : TBBeamQualityTool(name,type,parent),
    m_StoreGate(nullptr),
    m_Scint1ADC(0),
    m_Scint2ADC(0)
{

  // enter cut parameters
  // H6 default:
  declareInterface<TBBeamQualityTool>(this);
  declareProperty("Scintnames",m_scint_names);
  declareProperty("ScintCut1",m_ScintCut1=1);
  declareProperty("ScintCut2",m_ScintCut2=2);
  declareProperty("ScintCut1ADC",m_ScintCut1ADC);
  declareProperty("ScintCut2ADC",m_ScintCut2ADC);
  declareProperty("h6cuts",m_h6cuts=true);
} 

TBBeamQualityDoubleHitsTool::~TBBeamQualityDoubleHitsTool()
{ }

/*---------------INITIALIZE---------------*/
StatusCode TBBeamQualityDoubleHitsTool::initializeTool()
{ //init
  MsgStream log(msgSvc(),name());
  
  
  return StatusCode::SUCCESS;
}//init

StatusCode TBBeamQualityDoubleHitsTool::accept(std::vector<std::string> m_particles)
{//accept
  MsgStream log(msgSvc(),name());
    
  log << MSG::DEBUG
      << "TBBeamQualityDoubleHits: Started Accept"
      << endreq;

  if (m_h6cuts==true) {// h6cuts  
  
    // specifying correct cuts
    // wide/narrow beam selection to be added
    
    // Hadrons
    if (m_particles[0]=="pi+" || m_particles[0]=="pi-" || m_particles[0]=="p+" ) {
      m_ScintCut1ADC= 270;
      m_ScintCut2ADC = 265;
    }
    
    // Leptons
    if (m_particles[0]=="e+" || m_particles[0]=="e-" || m_particles[0]=="mu+" || m_particles[0]=="mu-" ) {
      m_ScintCut1ADC= 280;
      m_ScintCut2ADC = 275;
    }
    
  }// h6cuts

  log <<MSG::DEBUG
      << "m_ScintCut1ADC: "<< m_ScintCut1ADC
      << "m_ScintCut2ADC: "<< m_ScintCut2ADC 
      <<endreq;
  
  // checking out StoreGateSvc
  StatusCode sc = service("StoreGateSvc",m_StoreGate);
  if ( sc.isFailure() )
    {
      log << MSG::ERROR
	  << "Cannot alllocate StoreGate service!"
	  << endreq;
    }
  
  TBScintillatorCont * scint;
  
  sc = m_StoreGate->retrieve(scint, m_SGScintkey);
  
    if (sc.isFailure()){
      log << MSG::DEBUG
	  << "Can't Retrieve "
	  << m_SGScintkey
	  <<" from SG"
	  << endreq;
    }else { //else
      
      m_Scint1ADC = 0;
      m_Scint2ADC = 0;    
      
      TBScintillatorCont::const_iterator it_scint   = scint->begin();
      TBScintillatorCont::const_iterator last_scint   = scint->end();
      
      
    for(;it_scint!=last_scint;it_scint++) {
      
      const TBScintillator * scint = (*it_scint);
      
      std::string detect = scint->getDetectorName();
      
      if (detect==m_scint_names[m_ScintCut1]){
	m_Scint1ADC = scint->getSignal();
      }
      if (detect==m_scint_names[m_ScintCut2]){
	m_Scint2ADC = scint->getSignal();
      }
    }
    
    } //else   
    
    
    if (m_Scint1ADC>m_ScintCut1ADC && m_Scint2ADC>m_ScintCut2ADC) {
      return StatusCode::FAILURE;
    }
    else {
      return StatusCode::SUCCESS;  
    }
    
}//accept
