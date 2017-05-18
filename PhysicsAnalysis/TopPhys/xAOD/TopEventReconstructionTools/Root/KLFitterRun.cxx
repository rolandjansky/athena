/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/KLFitterRun.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

namespace top{

  KLFitterRun::KLFitterRun(const std::string& kLeptonType,std::shared_ptr<top::TopConfig> config) :
    m_name("RECO::KLFitterRun_" + kLeptonType),
    m_myFitter(nullptr)
  {
    
    m_myFitter = std::unique_ptr<top::KLFitterTool> ( new top::KLFitterTool( m_name ) ); 
    top::check( m_myFitter->setProperty("config", config ), "Failed to setProperty of KLFitterTool" );
    top::check( m_myFitter->setProperty("LeptonType",kLeptonType), "Failed to setProperty of KLFitterTool" );
    top::check( m_myFitter->initialize() , "Failed to initialize KLFitterTool" );        
  }
  
  bool KLFitterRun::apply(const top::Event& event) const
  {   
    top::check( m_myFitter->execute(event) , "Failed to run KLFitterTool" );
    return true;
  }
  
  std::string KLFitterRun::name() const {
    return m_name;
  }  
  
}
