/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMatchingTool/TrigEgammaMatchingToolMT.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "boost/algorithm/string.hpp"
#include <typeinfo>
#include <string>

using namespace TrigCompositeUtils;
using namespace Trig;




/*
 * TrigEgammaMatchinTool API
 */


TrigEgammaMatchingToolMT::TrigEgammaMatchingToolMT( const std::string& name ): 
  asg::AsgTool( name ),
  m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{;}


//!=======================================================================

StatusCode TrigEgammaMatchingToolMT::initialize() 
{
  ATH_CHECK(tdt().retrieve());

  ATH_CHECK( m_emTauRoIKey.initialize() );
  ATH_CHECK( m_ringerKey.initialize() );


  m_keys[ "Electrons"]                = "HLT_egamma_Electrons" ;
  m_keys[ "Electrons_GSF"]            = "HLT_egamma_Electrons_GSF" ; // gsf
  m_keys[ "Electrons_LRT"]            = "HLT_egamma_Electrons_LRT" ; // lrt
  m_keys[ "Photons"]                  = "HLT_egamma_Photons" ;

  // intermediate steps
  m_keys[ "PrecisionCalo_Electron"]   = "HLT_CaloEMClusters_Electron";
  m_keys[ "PrecisionCalo_Photon"]     = "HLT_CaloEMClusters_Photon";
  m_keys[ "PrecisionCalo_LRT"]        = "HLT_CaloEMClusters_LRT" ; // lrt

  // Fast steps
  m_keys[ "FastElectrons"]            = "HLT_FastElectrons" ;
  m_keys[ "FastElectrons_LRT"]        = "HLT_FastElectrons_LRT" ; // lrt
  m_keys[ "FastPhotons"]              = "HLT_FastPhotons" ;
  m_keys[ "FastCalo"]                 = "HLT_FastCaloEMClusters" ;
  // L1
  m_keys[ "L1Calo"]                   = "LVL1EmTauRoIs" ;

  return StatusCode::SUCCESS;
}


//!=======================================================================

std::string TrigEgammaMatchingToolMT::key( std::string key) const
{
  return m_keys.at(key); 
}

//!=======================================================================

bool TrigEgammaMatchingToolMT::isPassed(const xAOD::Egamma *eg,const std::string &trigger, unsigned int condition ) const
{
    const TrigCompositeUtils::Decision *dec=nullptr;
    if( match( eg, trigger, dec ) ){
      if( dec ){
        if(xAOD::EgammaHelpers::isElectron(eg)){
          std::string key = this->key("Electrons");
          if(boost::contains(trigger,"gsf")) key=this->key("Electrons_GSF");
          if(boost::contains(trigger,"lrt")) key=this->key("Electrons_LRT");
          return ancestorPassed<xAOD::ElectronContainer>(dec, trigger,key, condition);
        }
        if(xAOD::EgammaHelpers::isPhoton(eg)){
          return ancestorPassed<xAOD::PhotonContainer>(dec, trigger, key("Photons"), condition);
        }
      }
    }
    return false;
}

//!=======================================================================

bool TrigEgammaMatchingToolMT::match(const xAOD::Egamma *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{

  ATH_MSG_DEBUG("Match decec with trigger " << trigger);
  if(xAOD::EgammaHelpers::isElectron(eg)){
    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
    if( matchHLTElectron(el,trigger,dec, condition) ) {
      ATH_MSG_DEBUG("Matched!");
      return true;
    }
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
    if( matchHLTPhoton(ph,trigger,dec, condition) ) return true;
  }

  if( matchHLTCalo(eg,trigger,dec) ) return true;
  
  if(xAOD::EgammaHelpers::isElectron(eg)){
    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
    if( matchL2Electron(el,trigger,dec, condition) ) return true;
  
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
    if( matchL2Photon(ph,trigger,dec, condition) ) return true;
  }
  
  if( matchL2Calo(eg,trigger,dec, condition) ) return true;

  if( matchL1(eg,trigger,dec, condition) ) return true;
  ATH_MSG_DEBUG("match() failed!");
  return false;
}


//!=======================================================================

bool TrigEgammaMatchingToolMT::matchHLTPhoton(const xAOD::Photon *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{ 
  ATH_MSG_DEBUG("Match HLT Photon");
  return closestObject<xAOD::PhotonContainer>( eg, dec, trigger, key("Photons"), condition );
}

//!=======================================================================

bool TrigEgammaMatchingToolMT::matchHLTElectron(const xAOD::Electron *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{ 
  if (boost::contains(trigger,"gsf")){
      ATH_MSG_DEBUG("Matched HLT Electron GSF");
      return closestObject<xAOD::ElectronContainer>( eg, dec , trigger, key("Electrons_GSF"), condition );
    }else if(boost::contains(trigger,"lrt")){
      ATH_MSG_DEBUG("Matched HLT Electron LRT");
      return closestObject<xAOD::ElectronContainer>( eg, dec , trigger, key("Electrons_LRT"), condition );
    }else {
      ATH_MSG_DEBUG("Matched HLT Electron");
      return closestObject<xAOD::ElectronContainer>( eg, dec , trigger, key("Electrons"), condition );     
    }
}

//!=======================================================================

bool TrigEgammaMatchingToolMT::matchHLTCalo(const xAOD::Egamma *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{ 
  ATH_MSG_DEBUG("Match HLT PrecisionCalo");
  if(boost::contains(trigger,"lrt")){
    ATH_MSG_DEBUG("Matched HLT PrecisionCalo LRT");
    return closestObject<xAOD::CaloClusterContainer>( eg, dec, trigger, key("PrecisionCalo_LRT"), condition );
  }else if(xAOD::EgammaHelpers::isElectron(eg)){
    ATH_MSG_DEBUG("Matched HLT PrecisionCalo Electron");
    return closestObject<xAOD::CaloClusterContainer>( eg, dec, trigger, key("PrecisionCalo_Electron"), condition );
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    ATH_MSG_DEBUG("Matched HLT PrecisionCalo Photon");
    return closestObject<xAOD::CaloClusterContainer>( eg, dec, trigger, key("PrecisionCalo_Photon"), condition );
  }else{
    ATH_MSG_DEBUG("Match HLT PrecisionCalo failed!");
    return false;
  }
}

//!=======================================================================

bool TrigEgammaMatchingToolMT::matchL2Photon(const xAOD::Photon *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{
  ATH_MSG_DEBUG("Match L2 Photon");
  return closestObject<xAOD::TrigPhotonContainer>( eg, dec, trigger, key("FastPhotons"), condition );
}
  
//!=======================================================================

bool TrigEgammaMatchingToolMT::matchL2Electron(const xAOD::Electron *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{
  ATH_MSG_DEBUG("Match L2 Electron");
  if(boost::contains(trigger,"lrt")){
    return closestObject<xAOD::TrigElectronContainer>( eg, dec, trigger, key("FastElectrons_LRT"), condition );
  }else{
    return closestObject<xAOD::TrigElectronContainer>( eg, dec, trigger, key("FastElectrons"), condition );
  }
}

//!=======================================================================

bool TrigEgammaMatchingToolMT::matchL2Calo(const xAOD::Egamma *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{
  ATH_MSG_DEBUG("Match L2 Calo");
  return closestObject<xAOD::TrigEMClusterContainer>( eg, dec, trigger, key("FastCalo") , condition);
}
  
//!=======================================================================

bool TrigEgammaMatchingToolMT::matchL1( const xAOD::Egamma* eg, const std::string &trigger, const TrigCompositeUtils::Decision *&dec, unsigned int condition ) const
{
  ATH_MSG_DEBUG("Match L1 Calo");
  double deltaR=0.;
  auto initRois =  tdt()->features<TrigRoiDescriptorCollection>(trigger,condition,"",TrigDefs::allFeaturesOfType,"initialRoI");       
  
  if( initRois.size() < 1) return false;
  for( auto &initRoi: initRois ){               
    if( !initRoi.link.isValid() ) continue;      
    deltaR = dR(eg->eta(),eg->phi(), (*initRoi.link)->eta(),(*initRoi.link)->phi());          
    if(deltaR < (float)m_dRL1){   
      dec=initRoi.source;
      return true;                                                
    }                                                             
  }                                                                                                                            
  return false;
}

//!=======================================================================
 
const xAOD::EmTauRoI* TrigEgammaMatchingToolMT::getL1Feature( const TrigCompositeUtils::Decision *dec ) const
{
  if( !dec )  return nullptr;
  auto initRoi = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(dec, "initialRoI"); 
  if( !initRoi.isValid() ) return nullptr;

  SG::ReadHandle<xAOD::EmTauRoIContainer> l1_cont( m_emTauRoIKey, Gaudi::Hive::currentContext());
  if( !l1_cont.isValid() ) return nullptr;

  for( const auto *l1 : *l1_cont ){
    if ( l1->roiType() != xAOD::EmTauRoI::RoIType::EMRoIWord) continue;
    if((*initRoi.link)->roiWord()==l1->roiWord()){
      return l1;
    }
  }
  return nullptr;
} 

//!=======================================================================

const xAOD::TrigRingerRings* TrigEgammaMatchingToolMT::getRingsFeature( const TrigCompositeUtils::Decision *dec ) const
{
  if( !dec )  return nullptr;
  auto initRoi = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(dec, "initialRoI"); 
  if( !initRoi.isValid() ) return nullptr;
  
  SG::ReadHandle<xAOD::TrigRingerRingsContainer> rg_cont( m_ringerKey, Gaudi::Hive::currentContext() );

  if( !rg_cont.isValid() ) return nullptr;

  for( const auto *rg : *rg_cont ){
    const xAOD::TrigEMCluster *cl = rg->emCluster();
    if(!cl) continue;
    if(cl->RoIword()==(*initRoi.link)->roiWord())
      return rg;
  }
  return nullptr;
} 









