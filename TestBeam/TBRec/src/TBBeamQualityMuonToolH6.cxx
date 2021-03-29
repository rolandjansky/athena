/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityMuonToolH6.cxx              #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Modofied: Nov 18/2004                        #
//##################################################### 

#include "TBBeamQualityMuonToolH6.h"

// TRIGGERS
#include "TBEvent/TBTriggerPatternUnit.h"


#include <string>
#include <ios>
#include <algorithm>

TBBeamQualityMuonToolH6::TBBeamQualityMuonToolH6(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : TBBeamQualityTool(type,name,parent),
    m_mu1(false),
    m_mu2(false),
    m_mu3(false),
    m_mu4(false),
    m_successflag(false)
{ 
  declareInterface<TBBeamQualityTool>(this); 
}

TBBeamQualityMuonToolH6::~TBBeamQualityMuonToolH6()
{ }

StatusCode TBBeamQualityMuonToolH6::initializeTool()
{
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG
      << "initialize"
      << endmsg;
  
  // setting triggflag
  
  if(m_triggflag.size()==0){ //trig
    
    m_triggflag.push_back(0x00000001); // Entry 0:  S1 accept
    m_triggflag.push_back(0x00000002); // Entry 1:  S2 accept
    m_triggflag.push_back(0x00000004); // Enrty 2:  S3 accept
    m_triggflag.push_back(0x00000008); // Entry 3:  B2 accept
    m_triggflag.push_back(0x00000010); // Entry 4:  VetoLeft
    m_triggflag.push_back(0x00000020); // Entry 5:  VetoRight
    m_triggflag.push_back(0x00000040); // Entry 6:  VetoDown
    m_triggflag.push_back(0x00000080); // Entry 7:  VetoUp
    m_triggflag.push_back(0x00000100); // Entry 8:  Mu 1 Left
    m_triggflag.push_back(0x00000200); // Entry 9:  Mu 2 Left
    m_triggflag.push_back(0x00000400); // Entry 10: Mu 2 Left
    m_triggflag.push_back(0x00000800); // Entry 11: Early PileUp
    m_triggflag.push_back(0x00001000); // Enrty 12: Late PileUp
    m_triggflag.push_back(0x00002000); // Entry 13: In Spill Tag
    m_triggflag.push_back(0x00004000); // Entry 14: Off Spill Tag
    m_triggflag.push_back(0x00008000); // Entry 15: Cedar 6 0f 8 segments
    m_triggflag.push_back(0x00010000); // Entry 16: Mu 2 Right
    m_triggflag.push_back(0x00020000); // Entry 17: Hole Counter
    m_triggflag.push_back(0x00040000); // Entry 18: W1*W2 coincidence
    m_triggflag.push_back(0x00080000); // Entry 19: B1 accept
    m_triggflag.push_back(0x00100000); // Entry 20: Scalar Reset Tag
    m_triggflag.push_back(0x08000000); // Entry 21: Cedar 7 of 8 segments
    m_triggflag.push_back(0x10000000); // Entry 21: Cedar 8 of 8 segments                                                                                                         
  } //trig
  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityMuonToolH6::accept(const std::vector<std::string>& particles)
{//accept
  
  MsgStream log(msgSvc(),name());
  
  // Trigger Pattern Pointer
  TBTriggerPatternUnit * triggpat_object;
  
  ATH_CHECK( evtStore()->retrieve(triggpat_object, m_SGTrigkey) );

  unsigned int word =triggpat_object->getTriggerWord();
    
  m_mu1=false;
  m_mu2=false;
  m_mu3=false;
  m_mu4=false;
    
  // muon triggers are:
  // Entry 8:  Mu 1 Left
  // Entry 9:  Mu 2 Left
  // Entry 10: Mu 2 Left
  // Entry 16: Mu 2 Right
    
  if ((word & m_triggflag[8])!=0) {
    m_mu1=true;
  }
  if ((word & m_triggflag[9])!=0) {
    m_mu2=true;
  }
  if ((word & m_triggflag[10])!=0) {
    m_mu3=true;
  }
  if ((word & m_triggflag[16])!=0) {
    m_mu4=true;
  }
  
  //checking for muon coincidence:
  
  // rejecting event for all particles except for muons
  
  //muons
  log<<MSG::DEBUG
     <<particles[0]
     <<endmsg;
  
  if(particles[0]=="mu+" || particles[0]=="mu-"){
    m_successflag=false;
    
    if (((m_mu1==true)&(m_mu3==true))||((m_mu1==true)&(m_mu4==true))||((m_mu2==true)&(m_mu3==true))||((m_mu2==true)&(m_mu4==true))) {
      m_successflag=true;
    }
  }else{
    //all particles except muons
    m_successflag=true;
    
    if (((m_mu1==true)&(m_mu3==true))||((m_mu1==true)&(m_mu4==true))||((m_mu2==true)&(m_mu3==true))||((m_mu2==true)&(m_mu4==true))) {
      m_successflag=false;
    }
  }
  if(m_successflag==true){
    return StatusCode::SUCCESS; 
  }else{    
    return StatusCode::FAILURE;
  }
}//accept
