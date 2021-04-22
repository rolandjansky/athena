/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */

#include "boost/algorithm/string.hpp"
#include "TrigEgammaEmulationToolMT/TrigEgammaEmulationToolMT.h"

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
    //ATHCHECK( m_ringerTools.retrieve() );



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

asg::AcceptData TrigEgammaEmulationToolMT::emulate(const TrigCompositeUtils::Decision *dec,
                                                   std::string trigger) const
{
  asg::AcceptData acceptData (&m_accept);

  for ( auto& tool : m_hypoTools ){

    if( tool->getChain() == trigger ){

      if( tool->getSignature() == "electron" ){
        auto input = match(dec, m_trigElectronList, "electron");
        return tool->emulate(input);
      }else if(tool->getSignature() == "photon"){
        auto input = match(dec, m_trigPhotonList, "photon");
        return tool->emulate(input);
      }

    }

  }// Loop over all hypo tool chains

  return acceptData;
}


//**********************************************************************


Trig::TrigData TrigEgammaEmulationToolMT::match( const TrigCompositeUtils::Decision *dec ,
                                                 const std::vector<std::string> &triggerList,
                                                 std::string trigType ) const
{

  Trig::TrigData output{};

  for (auto& trigger : triggerList){

    // L1 RoI
    if( !output.roi ){
      auto feat = match()->getFeature<TrigRoiDescriptorCollection>(dec,trigger);
      if(feat.link.isValid()){
        output.roi = *feat.link;
      }
    }

    // L1Calo
    if( !output.l1 ){
      output.l1 = match()->getL1Feature(dec);
    }

    // Fast calo electron/photon
    if( !output.emCluster ){
      auto feat = match()->getFeature<xAOD::TrigEMClusterContainer>(dec,trigger);
      if(feat.link.isValid()){
        output.emCluster = *feat.link;
      }
    }

    if( !output.cluster ){
      auto vec_feat = match()->getFeatures<xAOD::CaloClusterContainer>(dec,trigger);
      if( !vec_feat.empty() ){
        if( vec_feat.front().link.isValid()){
          output.cluster = *vec_feat.front().link;
        }
      }
    }

    if (trigType == "electron"){
      if( output.electrons.empty() ){
        auto vec_feat = match()->getFeatures<xAOD::ElectronContainer>(dec,trigger);
        for ( auto& feat : vec_feat ){
          if(!feat.link.isValid()) continue;
          output.electrons.push_back(*feat.link);
        }
      }
    }else if(trigType == "photon"){
      if( output.photons.empty() ){
        auto vec_feat = match()->getFeatures<xAOD::PhotonContainer>(dec,trigger);
        for ( auto& feat : vec_feat ){
          if(!feat.link.isValid()) continue;
          output.photons.push_back(*feat.link);
        }      
      }
    }


    if (trigType == "electron"){
      if ( output.roi && output.l1 && output.emCluster && output.cluster && !output.electrons.empty()){
        break;// stop the loop!
      }
    }else if (trigType == "photon"){
      if ( output.roi && output.l1 && output.emCluster && output.cluster && !output.photons.empty()){
        break;// stop the loop!
      }
    }
  }

  // This is a HACK to avoid multiple instances
  // Decorate the output with all ASG tools
  output.electronLHTools=m_electronLHTools;
  output.photonIsEMTools=m_photonIsEMTools;
  output.ringerTools=m_ringerTools;

  return output;
}


