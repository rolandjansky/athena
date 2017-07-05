/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/PseudoTopRecoRun.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

namespace top{

  PseudoTopRecoRun::PseudoTopRecoRun(const std::string& kLeptonType, std::shared_ptr<top::TopConfig> config) :
    m_name("RECO::PseudoTopRecoRun_" + kLeptonType),
    m_PseudoTopReco(nullptr)
  {
    
    m_PseudoTopReco = std::unique_ptr<top::PseudoTopReco> ( new top::PseudoTopReco( m_name ) ); 
    top::check( m_PseudoTopReco->setProperty("config", config ), "Failed to setProperty of PseudoTopReco" );
    top::check( m_PseudoTopReco->setProperty("LeptonType", kLeptonType), "Failed to setProperty of PseudoTopReco" );
    //top::check( m_PseudoTopReco->setProperty("RecoType",   kRecoType), "Failed to setProperty of PseudoTopReco" );
    top::check( m_PseudoTopReco->initialize() , "Failed to initialize PseudoTopRecoTool" );     
   
  }
  
  bool PseudoTopRecoRun::apply(const top::Event& event) const
  {   
    top::check( m_PseudoTopReco->execute(event) , "Failed to run PseudoTopReco" );
    return true;
  }

  bool PseudoTopRecoRun::applyParticleLevel(const top::ParticleLevelEvent& plEvent) const
  {
    top::check( m_PseudoTopReco->execute(plEvent) , "Failed to run PseudoTopReco" );
    return true;
  }
  
  std::string PseudoTopRecoRun::name() const {
    return m_name;
  }  
  
}
