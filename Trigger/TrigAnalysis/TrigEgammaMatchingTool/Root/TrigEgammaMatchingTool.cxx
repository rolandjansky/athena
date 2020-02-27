/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "boost/algorithm/string.hpp"
#include <typeinfo>
#include <string>

using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::Decision;

using namespace TrigEgammaMatchingUtils;
using namespace TrigCompositeUtils;
using namespace Trig;




/*
 * TrigEgammaMatchinTool API
 */


TrigEgammaMatchingTool::TrigEgammaMatchingTool( const std::string& name ): 
  asg::AsgTool( name ),
  m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty( "DeltaR"     , m_dR=0.07     );
  declareProperty( "L1DeltaR"   , m_dRL1=0.15   );
}



TrigEgammaMatchingTool::~TrigEgammaMatchingTool()
{
}




StatusCode TrigEgammaMatchingTool::initialize() 
{
  ATH_CHECK(tdt().retrieve());

  m_keys[ "Electron"]   = std::pair<std::string,std::string>( "", "HLT_egamma_Electrons" );
  m_keys[ "Photon"]     = std::pair<std::string,std::string>( "", "HLT_egamma_Photons" );
  m_keys[ "EFCalo"]     = std::pair<std::string,std::string>( "TrigEFCaloCalibFex", "HLT_CaloEMClusters" );
  m_keys[ "L2Electron"] = std::pair<std::string,std::string>( "", "HLT_L2Electrons" );
  m_keys[ "L2Photon"]   = std::pair<std::string,std::string>( "", "HLT_L2Photons" );
  m_keys[ "L2Calo"]     = std::pair<std::string,std::string>( "", "HLT_L2CaloEMClusters" );
  m_keys[ "L1Calo"]     = std::pair<std::string,std::string>( "", "" );



  return StatusCode::SUCCESS;
}



std::string TrigEgammaMatchingTool::key( std::string key) const
{
  if( m_keys.find(key) == m_keys.end() ){
    return "";
  }else{
    if( tdt()->getNavigationFormat()=="TriggerElement")
      return m_keys.at(key).first; 
    else
      return m_keys.at(key).second; 
  }
}


bool TrigEgammaMatchingTool::match(const xAOD::Egamma *eg,const std::string trigger, Element &obj ) const
{
  ATH_MSG_DEBUG("Match objec with trigger " << trigger);
  if(xAOD::EgammaHelpers::isElectron(eg)){
    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
    if( matchHLTElectron(el,trigger,obj) ) {
      ATH_MSG_DEBUG("Matched!");
      return true;
    }
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
    if( matchHLTPhoton(ph,trigger,obj) ) return true;
  }

  if( matchHLTCalo(eg,trigger,obj) ) return true;
  
  if(xAOD::EgammaHelpers::isElectron(eg)){
    const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
    if( matchL2Electron(el,trigger,obj) ) return true;
  
  }else if(xAOD::EgammaHelpers::isPhoton(eg)){
    const xAOD::Photon* ph =static_cast<const xAOD::Photon*> (eg);
    if( matchL2Photon(ph,trigger,obj) ) return true;
  }

  if( matchL1(eg,trigger,obj) ) return true;
  ATH_MSG_DEBUG("match() failed!");
  return false;
}


bool TrigEgammaMatchingTool::match(const xAOD::Egamma *eg,const std::string trigger ) const
{
    Element obj(trigger);
    if( match( eg, trigger, obj ) ){
      if( obj.isValid() ){
        if(xAOD::EgammaHelpers::isElectron(eg)){
          return ancestorPassed<xAOD::ElectronContainer>(obj);
        }
        if(xAOD::EgammaHelpers::isPhoton(eg)){
          return ancestorPassed<xAOD::PhotonContainer>(obj);
        }
      }
    }
    return false;
}



bool TrigEgammaMatchingTool::matchHLTPhoton(const xAOD::Photon *eg,const std::string &trigger, Element &obj ) const
{ 
  ATH_MSG_DEBUG("Match HLT Photon");
  return closestObject<xAOD::Photon>( eg, obj, trigger, key("Photon") );
}


bool TrigEgammaMatchingTool::matchHLTElectron(const xAOD::Electron *eg,const std::string &trigger, Element &obj ) const
{ 
  ATH_MSG_DEBUG("Match HLT Electron");
  return closestObject<xAOD::Electron>( eg, obj , trigger, key("Electron") );
}


bool TrigEgammaMatchingTool::matchHLTCalo(const xAOD::Egamma *eg,const std::string &trigger, Element &obj ) const
{ 
  ATH_MSG_DEBUG("Match HLT Calo");
  return closestObject<xAOD::CaloCluster>( eg, obj, trigger, key("EFCalo") );
}


bool TrigEgammaMatchingTool::matchL2Photon(const xAOD::Photon *eg,const std::string &trigger, Element &obj ) const
{
  ATH_MSG_DEBUG("Match L2 Photon");
  return closestObject<xAOD::TrigPhoton>( eg, obj, trigger, key("L2Photon") );
}
  

bool TrigEgammaMatchingTool::matchL2Electron(const xAOD::Electron *eg,const std::string &trigger, Element &obj ) const
{
  ATH_MSG_DEBUG("Match L2 Photon");
  return closestObject<xAOD::TrigElectron>( eg, obj, trigger, key("L2Electron") );
}


bool TrigEgammaMatchingTool::matchL2Calo(const xAOD::Egamma *eg,const std::string &trigger, Element &obj ) const
{
  ATH_MSG_DEBUG("Match L2 Calo");
  return closestObject<xAOD::TrigEMCluster>( eg, obj, trigger, key("L2Calo") );
}
  
  



bool TrigEgammaMatchingTool::matchL1( const xAOD::Egamma* eg, const std::string &trigger, Element &obj ) const
{
  ATH_MSG_DEBUG("Match L1 Calo");
  double deltaR=0.;
  if( tdt()->getNavigationFormat() == "TriggerElement" ){
    auto fc = (tdt()->features(trigger,TrigDefs::alsoDeactivateTEs));
    auto initRois = fc.get<TrigRoiDescriptor>();
    if ( initRois.size() < 1 ) return false;
    for (auto initRoi: initRois) {
      auto itEmTau = tdt()->ancestor<xAOD::EmTauRoI>(initRoi);
      const xAOD::EmTauRoI *l1 = itEmTau.cptr();
      if(l1 == nullptr) continue;
      deltaR = dR(eg->eta(),eg->phi(), l1->eta(),l1->phi());
      if(deltaR < m_dRL1){
        obj.setPtr(itEmTau.te());
        return true;
      }
    }
  }else{
    double deltaR=0.;                                                 
    auto initRois =  tdt()->features<TrigRoiDescriptorCollection>(trigger,TrigDefs::includeFailedDecisions,"",
                                                                  TrigDefs::allFeaturesOfType,"initialRoI");       
    if( initRois.size() < 1) return false;
    for( auto &initRoi: initRois ){               
      if( !initRoi.link.isValid() ) continue;      
      deltaR = dR(eg->eta(),eg->phi(), (*initRoi.link)->eta(),(*initRoi.link)->phi());          
      if(deltaR < m_dR){   
        obj.setPtr(initRoi.source);
        return true;                                                
      }                                                             
    }                                                               
  }                                                                    
  return false;
}


 






