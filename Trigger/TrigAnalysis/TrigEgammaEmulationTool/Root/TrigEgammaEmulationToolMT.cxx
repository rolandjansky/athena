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

    ATH_CHECK( m_electronLHTools.retrieve() );
    ATH_CHECK( m_photonIsEMTools.retrieve() );
    ATH_CHECK( m_ringerTools.retrieve() );
    ATH_CHECK(m_hypoTools.retrieve());

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
    if( tool->getChain() == trigger )
    {
      Trig::TrigData input{};

      if(tool->getSignature()=="electron"){
        matchElectron(roi,input);
      }else if(tool->getSignature()=="photon"){
        matchPhoton(roi,input);
      }

      if(input.isValid){
        input.electronLHTools=m_electronLHTools;
        input.photonIsEMTools=m_photonIsEMTools;
        input.ringerTools=m_ringerTools;
        return tool->emulate(input);
      }
    }
  }// Loop over all hypo tool chains

  return acceptData;
}


//**********************************************************************


bool TrigEgammaEmulationToolMT::matchElectron( const TrigCompositeUtils::Decision *roi ,
                                               Trig::TrigData &output ) const
{
  unsigned int condition=TrigDefs::includeFailedDecisions;
  output.isValid = false;

  if(!roi) return false;

  for (auto& trigger : m_trigElectronList){

    auto vec_el_linkInfo = match()->getFeatures<xAOD::ElectronContainer>(roi,trigger,condition);

    if( !vec_el_linkInfo.empty() ){

      // clean all vectors before fill it
      output.electrons.clear();
      output.trigElectrons.clear();
      output.clusters.clear();
      output.roi = nullptr;
      output.l1 = nullptr;
      output.emCluster = nullptr;
      output.rings = nullptr;
      output.isValid = false;

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
          output.trigElectrons.push_back(*featLinkInfo.link);
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
      
      if( output.roi && 
          output.l1 && 
          output.emCluster && 
          output.rings && 
          !output.trigElectrons.empty() &&
          !output.clusters.empty() && 
          !output.electrons.empty())
      {
        output.isValid = true;
      }


      ATH_MSG_DEBUG( "L1 RoI TDET  = " << (output.roi?"Yes":"No")); 
      ATH_MSG_DEBUG( "L1 RoI EmTau = " << (output.l1?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Cluster   = " << (output.emCluster?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Rings     = " << (output.rings?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Electrons = " << (output.trigElectrons.size())); 
      ATH_MSG_DEBUG( "HLT Cluster  = " << output.clusters.size()); 
      ATH_MSG_DEBUG( "HLT el       = " << output.electrons.size()); 
    }// has electron

    // stop the trigger loop since we have all features inside
    if(output.isValid) return true;

  }// Loop over triggers


  return false;
}

//**********************************************************************


bool TrigEgammaEmulationToolMT::matchPhoton( const TrigCompositeUtils::Decision *roi ,
                                               Trig::TrigData &output ) const
{
  unsigned int condition=TrigDefs::includeFailedDecisions;
  output.isValid = false;

  if(!roi) return false;

  for (auto& trigger : m_trigPhotonList){

    auto vec_ph_linkInfo = match()->getFeatures<xAOD::PhotonContainer>(roi,trigger,condition);

    if( !vec_ph_linkInfo.empty() ){

      // clean all vectors before fill it
      output.photons.clear();
      output.clusters.clear();
      output.roi = nullptr;
      output.l1 = nullptr;
      output.emCluster = nullptr;
      output.rings = nullptr;
      output.trigPhoton = nullptr;
      output.isValid = false;
      
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
      
      if( output.roi && 
          output.l1 && 
          output.emCluster && 
          output.rings && 
          output.trigPhoton &&
          !output.clusters.empty() && 
          !output.photons.empty())
      {
        output.isValid = true;
      }

      ATH_MSG_DEBUG( "L1 RoI TDET  = " << (output.roi?"Yes":"No")); 
      ATH_MSG_DEBUG( "L1 RoI EmTau = " << (output.l1?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Cluster   = " << (output.emCluster?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Rings     = " << (output.rings?"Yes":"No")); 
      ATH_MSG_DEBUG( "L2 Photon    = " << (output.trigPhoton?"Yes":"No")); 
      ATH_MSG_DEBUG( "HLT Cluster  = " << output.clusters.size()); 
      ATH_MSG_DEBUG( "HLT ph       = " << output.photons.size()); 
    }// has electron

    // stop the trigger loop since we have all features inside
    if(output.isValid) return true;

  }// Loop over triggers

  return false;
}
