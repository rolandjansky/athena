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
{
}



TrigEgammaMatchingToolMT::~TrigEgammaMatchingToolMT()
{
}




StatusCode TrigEgammaMatchingToolMT::initialize() 
{
  ATH_CHECK(tdt().retrieve());

  ATH_CHECK( m_emTauRoIKey.initialize() );
  m_keys[ "Electron"]   = "HLT_egamma_Electrons" ;
  m_keys[ "Photon"]     = "HLT_egamma_Photons" ;
  m_keys[ "Track"]      = "";
  m_keys[ "EFCalo"]     = "HLT_CaloEMClusters" ;
  m_keys[ "L2Electron"] = "HLT_FastElectrons" ;
  m_keys[ "L2Photon"]   = "HLT_FastPhotons" ;
  m_keys[ "L2Calo"]     = "HLT_FastCaloEMClusters" ;
  m_keys[ "L1Calo"]     = "LVL1EmTauRoIs" ;

  return StatusCode::SUCCESS;
}



std::string TrigEgammaMatchingToolMT::key( std::string key) const
{
  return m_keys.at(key); 
}


bool TrigEgammaMatchingToolMT::match(const xAOD::Egamma *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{

  ATH_MSG_DEBUG("Match decec with trigger " << trigger);
  if(xAOD::EgammaHelpers::isElectron(eg)){
    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
    if( matchHLTElectron(el,trigger,dec) ) {
      ATH_MSG_DEBUG("Matched!");
      return true;
    }
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
    if( matchHLTPhoton(ph,trigger,dec) ) return true;
  }

  if( matchHLTCalo(eg,trigger,dec) ) return true;
  
  if(xAOD::EgammaHelpers::isElectron(eg)){
    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
    if( matchL2Electron(el,trigger,dec) ) return true;
  
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
    if( matchL2Photon(ph,trigger,dec) ) return true;
  }
  
  if( matchL2Calo(eg,trigger,dec) ) return true;

  if( matchL1(eg,trigger,dec) ) return true;
  ATH_MSG_DEBUG("match() failed!");
  return false;
}


bool TrigEgammaMatchingToolMT::match(const xAOD::Egamma *eg,const std::string &trigger ) const
{
    const TrigCompositeUtils::Decision *dec=nullptr;
    if( match( eg, trigger, dec ) ){
      if( dec ){
        if(xAOD::EgammaHelpers::isElectron(eg)){
          return ancestorPassed<xAOD::ElectronContainer>(dec, trigger,key("Electron"));
        }
        if(xAOD::EgammaHelpers::isPhoton(eg)){
          return ancestorPassed<xAOD::PhotonContainer>(dec, trigger, key("Photon"));
        }
      }
    }
    return false;
}



bool TrigEgammaMatchingToolMT::matchHLTPhoton(const xAOD::Photon *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{ 
  ATH_MSG_DEBUG("Match HLT Photon");
  return closestObject<xAOD::PhotonContainer>( eg, dec, trigger, key("Photon") );
}


bool TrigEgammaMatchingToolMT::matchHLTElectron(const xAOD::Electron *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{ 
  ATH_MSG_DEBUG("Match HLT Electron");
  return closestObject<xAOD::ElectronContainer>( eg, dec , trigger, key("Electron") );
}


bool TrigEgammaMatchingToolMT::matchHLTCalo(const xAOD::Egamma *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{ 
  ATH_MSG_DEBUG("Match HLT Calo");
  return closestObject<xAOD::CaloClusterContainer>( eg, dec, trigger, key("EFCalo") );
}


bool TrigEgammaMatchingToolMT::matchL2Photon(const xAOD::Photon *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{
  ATH_MSG_DEBUG("Match L2 Photon");
  return closestObject<xAOD::TrigPhotonContainer>( eg, dec, trigger, key("L2Photon") );
}
  

bool TrigEgammaMatchingToolMT::matchL2Electron(const xAOD::Electron *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{
  ATH_MSG_DEBUG("Match L2 Photon");
  return closestObject<xAOD::TrigElectronContainer>( eg, dec, trigger, key("L2Electron") );
}


bool TrigEgammaMatchingToolMT::matchL2Calo(const xAOD::Egamma *eg,const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{
  ATH_MSG_DEBUG("Match L2 Calo");
  return closestObject<xAOD::TrigEMClusterContainer>( eg, dec, trigger, key("L2Calo") );
}
  
  
bool TrigEgammaMatchingToolMT::matchL1( const xAOD::Egamma* eg, const std::string &trigger, const TrigCompositeUtils::Decision *&dec ) const
{
  ATH_MSG_DEBUG("Match L1 Calo");
  double deltaR=0.;
  auto initRois =  tdt()->features<TrigRoiDescriptorCollection>(trigger,TrigDefs::includeFailedDecisions,"",
                                                                TrigDefs::allFeaturesOfType,"initialRoI");       
  
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


 






