/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */

#include "boost/algorithm/string.hpp"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"

using namespace Trig;



//**********************************************************************

TrigEgammaEmulationToolMT::TrigEgammaEmulationToolMT( const std::string& myname )
    : AsgTool(myname),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingToolMT/TrigEgammaMatchingToolMT")
{
  declareProperty( "MatchTool" , m_matchTool );
}


//**********************************************************************
StatusCode TrigEgammaEmulationToolMT::initialize() 
{
    ATH_CHECK(m_matchTool.retrieve());

    //Enable expert methods
    ATH_CHECK(m_trigdec.retrieve());
    m_trigdec->ExperimentalAndExpertMethods()->enable();

    // retrieve all hypo chains
    ATH_CHECK( m_hypoTools.retrieve() );
    
    ATH_CHECK( m_egammaElectronDNNTools.retrieve()  );
    ATH_CHECK( m_egammaElectronCBTools.retrieve()   );
    ATH_CHECK( m_egammaElectronLHTools.retrieve()   );        
    ATH_CHECK( m_egammaPhotonCBTools.retrieve()     );      
    ATH_CHECK( m_ringerTools.retrieve()             );

    //add cuts into TAccept
    m_accept.addCut("L1Calo"  , "Trigger L1Calo step"     );
    m_accept.addCut("L2Calo"  , "Trigger L2Calo step"     );
    m_accept.addCut("L2"      , "Trigger L2Electron step" );
    m_accept.addCut("EFCalo"  , "Trigger EFCalo step"     );
    m_accept.addCut("EFTrack" , "Trigger EFTrack step"    );
    m_accept.addCut("HLT"     , "Trigger HLT decision"    );

   
    return StatusCode::SUCCESS;
}

//**********************************************************************

asg::AcceptData TrigEgammaEmulationToolMT::emulate(const TrigCompositeUtils::Decision *roi,
                                                   std::string trigger) const
{
  asg::AcceptData acceptData (&m_accept);

  for ( auto& tool : m_hypoTools ){

    if( tool->chain() == trigger )
    {
      Trig::TrigData input(tool->signature());
      match(roi, input);

      if(input.isValid()){
        input.egammaElectronDNNTools  = m_egammaElectronDNNTools;
        input.egammaElectronCBTools   = m_egammaElectronCBTools;
        input.egammaElectronLHTools   = m_egammaElectronLHTools;        
        input.egammaPhotonCBTools     = m_egammaPhotonCBTools;      
        input.ringerTools             = m_ringerTools;
        return tool->emulate(input);
      }
      
    }
  }// Loop over all hypo tool chains

  return acceptData;
}


//**********************************************************************

bool TrigEgammaEmulationToolMT::match( const TrigCompositeUtils::Decision *roi, Trig::TrigData &output) const
{
  if(output.signature == "electron"){
    return match_electron(roi, output, m_trigElectronList);
  }else if (output.signature == "photon"){
    return match_photon(roi, output, m_trigPhotonList);
  }else{
    return false;
  }
}


//**********************************************************************

bool TrigEgammaEmulationToolMT::match_electron( const TrigCompositeUtils::Decision *roi ,
                                                Trig::TrigData &output, 
                                                std::vector<std::string> trigList ) const
{
  unsigned int condition=TrigDefs::includeFailedDecisions;
  output.clear();

  if(!roi) return false;

  for (auto& trigger : trigList){

    auto vec_el_linkInfo = match()->getFeatures<xAOD::ElectronContainer>(roi,trigger,condition);

    if( !vec_el_linkInfo.empty() ){

      output.clear();

      // Step 5
      {
        for(auto&featLinkInfo : vec_el_linkInfo){
          if(!featLinkInfo.isValid()) continue;
          output.electrons.push_back(*featLinkInfo.link);
        }
      }


      // Step 3
      {
        auto vec_feat = match()->getFeatures<xAOD::CaloClusterContainer>(roi,trigger,condition);
        for(auto& featLinkInfo : vec_feat){
          if(!featLinkInfo.isValid()) continue;
          output.clusters.push_back(*featLinkInfo.link);
        }
      }

      // Step 2
      {
        auto vec_feat = match()->getFeatures<xAOD::TrigElectronContainer>(roi, trigger, condition);
        for ( auto& featLinkInfo : vec_feat ){
          if(!featLinkInfo.isValid()) continue;
          output.trig_electrons.push_back(*featLinkInfo.link);
        }    
      }

      // Step 1
      {
        auto featLinkInfo = match()->getFeature<xAOD::TrigEMClusterContainer>(roi,trigger);
        if(featLinkInfo.isValid()){
          output.emCluster = *featLinkInfo.link;
        }
      }

      // Step 0
      {
        // L1Calo (step 0)
        output.l1 = match()->getL1Feature(roi);
        auto featLinkInfo = match()->getFeature<TrigRoiDescriptorCollection>(roi,trigger);
        if(featLinkInfo.isValid()){
          output.roi = *featLinkInfo.link;
        }
      }
      


      ATH_MSG_DEBUG( "L1 RoI TDET  = " << (output.roi?"Yes":"No")); 
      ATH_MSG_DEBUG( "L1 RoI EmTau = " << (output.l1?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Cluster   = " << (output.emCluster?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Rings     = " << (output.rings?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Electrons = " << (output.trig_electrons.size())); 
      ATH_MSG_DEBUG( "HLT Cluster  = " << output.clusters.size()); 
      ATH_MSG_DEBUG( "HLT el       = " << output.electrons.size()); 
    }// has electron

    // stop the trigger loop since we have all features inside
    if(output.isValid()) return true;

  }// Loop over triggers


  return false;
}

//**********************************************************************


bool TrigEgammaEmulationToolMT::match_photon( const TrigCompositeUtils::Decision *roi ,
                                              Trig::TrigData &output,
                                              std::vector<std::string> trigList ) const
{
  unsigned int condition=TrigDefs::includeFailedDecisions;
  output.clear();

  if(!roi) return false;

  for (auto& trigger : trigList){

    auto vec_ph_linkInfo = match()->getFeatures<xAOD::PhotonContainer>(roi,trigger,condition);

    if( !vec_ph_linkInfo.empty() ){

      // clean all vectors before fill it
      output.clear();
      
      // Step 5
      {
        for(auto&featLinkInfo : vec_ph_linkInfo){
          if(!featLinkInfo.isValid()) continue;
          output.photons.push_back(*featLinkInfo.link);
        }
      }

      // Step 3
      {
        auto vec_feat = match()->getFeatures<xAOD::CaloClusterContainer>(roi,trigger,condition);
        for(auto& featLinkInfo : vec_feat){
          if(!featLinkInfo.isValid()) continue;
          output.clusters.push_back(*featLinkInfo.link);
        }
      }

      // Step 1
      {
        auto featLinkInfo = match()->getFeature<xAOD::TrigEMClusterContainer>(roi,trigger);
        if(featLinkInfo.isValid()){
          output.emCluster = *featLinkInfo.link;
        }
      }

      // Step 0
      {
        // L1Calo (step 0)
        output.l1 = match()->getL1Feature(roi);
        auto featLinkInfo = match()->getFeature<TrigRoiDescriptorCollection>(roi,trigger);
        if(featLinkInfo.isValid()){
          output.roi = *featLinkInfo.link;
        }
      }
  
      ATH_MSG_DEBUG( "L1 RoI TDET  = " << (output.roi?"Yes":"No")); 
      ATH_MSG_DEBUG( "L1 RoI EmTau = " << (output.l1?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Cluster   = " << (output.emCluster?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Rings     = " << (output.rings?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Photon    = " << (output.trig_photon?"Yes":"No")); 
      ATH_MSG_DEBUG( "HLT Cluster  = " << output.clusters.size()); 
      ATH_MSG_DEBUG( "HLT ph       = " << output.photons.size()); 
    }// has electron

    // stop the trigger loop since we have all features inside
    if(output.isValid()) return true;

  }// Loop over triggers

  return false;
}
