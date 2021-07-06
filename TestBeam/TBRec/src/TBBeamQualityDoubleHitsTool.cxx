/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


#include "TBBeamQualityDoubleHitsTool.h"

#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBScintillator.h"

#include <string>
#include <ios>
#include <algorithm>

TBBeamQualityDoubleHitsTool::TBBeamQualityDoubleHitsTool(const std::string& type,
							 const std::string& name,
							 const IInterface* parent)
  : TBBeamQualityTool(type,name,parent),
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

StatusCode TBBeamQualityDoubleHitsTool::accept(const std::vector<std::string>& particles)
{//accept
  MsgStream log(msgSvc(),name());
    
  log << MSG::DEBUG
      << "TBBeamQualityDoubleHits: Started Accept"
      << endmsg;

  if (m_h6cuts==true) {// h6cuts  
  
    // specifying correct cuts
    // wide/narrow beam selection to be added
    
    // Hadrons
    if (particles[0]=="pi+" || particles[0]=="pi-" || particles[0]=="p+" ) {
      m_ScintCut1ADC= 270;
      m_ScintCut2ADC = 265;
    }
    
    // Leptons
    if (particles[0]=="e+" || particles[0]=="e-" || particles[0]=="mu+" || particles[0]=="mu-" ) {
      m_ScintCut1ADC= 280;
      m_ScintCut2ADC = 275;
    }
    
  }// h6cuts

  log <<MSG::DEBUG
      << "m_ScintCut1ADC: "<< m_ScintCut1ADC
      << "m_ScintCut2ADC: "<< m_ScintCut2ADC 
      <<endmsg;
  
  TBScintillatorCont * scintCont = nullptr;
  if (evtStore()->retrieve(scintCont, m_SGScintkey).isFailure()) {
    log << MSG::DEBUG
        << "Can't Retrieve "
        << m_SGScintkey
        <<" from SG"
        << endmsg;
  }
  else {
      
    m_Scint1ADC = 0;
    m_Scint2ADC = 0;    

    for (const TBScintillator* scint : *scintCont) {
      std::string detect = scint->getDetectorName();
      if (detect==m_scint_names[m_ScintCut1]){
        m_Scint1ADC = scint->getSignal();
      }
      if (detect==m_scint_names[m_ScintCut2]){
        m_Scint2ADC = scint->getSignal();
      }
    }
  }

  if (m_Scint1ADC>m_ScintCut1ADC && m_Scint2ADC>m_ScintCut2ADC) {
    return StatusCode::FAILURE;
  }
  else {
    return StatusCode::SUCCESS;  
  }
    
}//accept
