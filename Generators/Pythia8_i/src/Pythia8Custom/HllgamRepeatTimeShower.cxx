/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HllgamRepeatTimeShower.h"



  HllgamRepeatTimeShower::HllgamRepeatTimeShower(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_nPass(0),
  m_nVetos(0){
    declareInterface<IPythia8Custom>(this);
  }
  
  
  StatusCode HllgamRepeatTimeShower::initialize() {
    ATH_MSG_INFO( "Initialisation of " << name() << " was successful" );
    
    ATH_MSG_INFO( "********************************************************************************" );
    ATH_MSG_INFO( "********************************************************************************" );
    ATH_MSG_INFO( "***               Will run with repeated time showers of H decay             ***" );
    ATH_MSG_INFO( "***            N.B. only make sense for gamma gamma decays                   ***" );
    ATH_MSG_INFO( "********************************************************************************" );
    ATH_MSG_INFO( "********************************************************************************" );
    
    return StatusCode::SUCCESS;
  }
  
  
  StatusCode HllgamRepeatTimeShower::finalize(){
    
    ATH_MSG_INFO( "*************************************************************" );
    ATH_MSG_INFO( "N retries  " << m_nVetos );
    ATH_MSG_INFO( "N pass     " << m_nPass );
    ATH_MSG_INFO( "*************************************************************" );
    
    ATH_MSG_INFO( "Finalisation of " << name() << " was successful" );
    return StatusCode::SUCCESS;
  }
  
  double HllgamRepeatTimeShower::CrossSectionScaleFactor() const {
    unsigned long sum = m_nVetos  +  m_nPass;
    if(sum == 0 || m_nPass==0 ){
      ATH_MSG_WARNING( "Are you sure that this code has been called correctly??  " );
      return 1.;
    }
    
    return double (m_nPass)/double ( sum );
  }
  
  StatusCode HllgamRepeatTimeShower::ModifyPythiaEvent(Pythia8::Pythia& pythia) const {
    
    //Index of the photons for the Higgs decay
    std::vector<int> photons;
    
    int trials = 0;
    
    bool checkEvent = true;
    
    Pythia8::Event& event = pythia.event;
    
    while(checkEvent){
      
      int higgsIndex = -1;
      for(int ii=0; ii != event.size(); ++ii){
        // status 22 is intermediate hard process.
        // I think that is correct here because although the output
        // from Powheg is the Higgs, Pythia should still consider the
        // decay products of the Higgs to be part of the hard process
        if(event[ii].id() == 25 && abs(event[ii].status()) == 22){
          higgsIndex = ii;
          break;
        }
      }
      
      if(higgsIndex <0){
        // didn't find a Higgs - barf
        ATH_MSG_ERROR("No Higgs in event record");
        return StatusCode::FAILURE;
      }
      
      std::vector<int> daughters = event[higgsIndex].daughterList();
      while(daughters.size()==1){
        daughters=event[daughters[0]].daughterList();
      }
      
      std::vector<int> photons;
      
      for(const auto d: daughters){
        if(event[d].id() == 22){
          photons.push_back(d);
        }
      }
      
      if(photons.size() < 2){
        ATH_MSG_ERROR("Wrong number of photons from H decay. Expected >= 2, got " + std::to_string(photons.size()));
        return StatusCode::FAILURE;
      }
      
      for(const auto p: photons){
        daughters = event[p].daughterList();
        while(daughters.size() == 1){
          daughters = event[daughters[0]].daughterList();
        }
        
        if(daughters.size() < 2) continue;
        size_t nLeptons=0;
        
        for(const auto d: daughters){
          if(event[d].isLepton()) ++nLeptons;
        }
        
        if(nLeptons >= 2){
          checkEvent = false;
          break;
        }
      }
      
      if(checkEvent){
        pythia.forceTimeShower( photons[0], photons[1], 1000000);
        ++trials;
        ++m_nVetos;
        if(trials % 1000 == 0 ){
          ATH_MSG_WARNING("Has the decay been setup in Pythia correctly?  Possibly stuck in loop so far " <<  trials  << " attempts have been made");
        }
      }else{
       pythia.forceHadronLevel();
       ++m_nPass;
      }
    }
    
    return StatusCode::SUCCESS;
  }

