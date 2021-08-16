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

    // retrieve all hypo chains
    ATH_CHECK( m_hypoTools.retrieve() );


    ATH_CHECK( m_ringerTools.retrieve() );
    ATH_CHECK( m_egammaPhotonCBTools.retrieve() );
    ATH_CHECK( m_egammaElectronDNNTools.retrieve() );
    ATH_CHECK( m_egammaElectronCBTools.retrieve() );
    ATH_CHECK( m_egammaElectronLHTools.retrieve() );    
    ATH_CHECK( m_ringerTools.retrieve() );
    

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

  for ( auto& tool : m_hypoTools )
  {
    if( tool->chain() == trigger )
    {
      Trig::TrigData input(trigger);
      if(!match(roi, input)) return acceptData;
      if(input.isValid())
        return tool->emulate(input);
    }

  }// Loop over all hypo tool chains

  return acceptData;
}


//**********************************************************************

bool TrigEgammaEmulationToolMT::match( const TrigCompositeUtils::Decision *roi, 
                                       Trig::TrigData &output,
                                       unsigned int condition) const
{
  output.clear();

  if(!roi) {
    ATH_MSG_DEBUG("no Roi!");
    return false;
  }


  // Link all selectors with this trig object to easy acesss throut of the chain
  // Avoid multiple selectors
  output.egammaElectronCBTools=m_egammaElectronCBTools;
  output.egammaElectronDNNTools=m_egammaElectronDNNTools;
  output.egammaElectronLHTools=m_egammaElectronLHTools;
  output.egammaPhotonCBTools=m_egammaPhotonCBTools;
  output.ringerTools=m_ringerTools;


  if(output.signature == "electron"){
    for (auto& trigger : m_electronTrigList){

      if(boost::contains(trigger,"gsf")) continue;
      if(boost::contains(trigger,"lrt")) continue;
      ATH_MSG_DEBUG("Matching with " << trigger );


      auto vec_el_linkInfo = match()->getFeatures<xAOD::ElectronContainer>(roi,trigger,condition);

      if( !vec_el_linkInfo.empty() ){

        output.clear();

        // Step 5
        {
          for(auto&featLinkInfo : vec_el_linkInfo)
          {
            if(!featLinkInfo.isValid()) continue;
            auto el = *featLinkInfo.link;
            output.electrons.push_back(el);
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
          // get rings from container access
          output.rings = match()->getRingsFeature(roi);
          if(output.rings){
            output.emCluster = output.rings->emCluster();
          }
        }



        // Step 0
        {
          // L1Calo (step 0)
          output.l1 = match()->getL1Feature(roi);
          auto featLinkInfo = match()->getFeature<TrigRoiDescriptorCollection>(roi,trigger, condition);
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
      if(output.isValid())
        return true;
      
    }// Loop over triggers

  }else if (output.signature == "photon"){

    for (auto& trigger : m_photonTrigList){

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
          // get rings from container access
          output.rings = match()->getRingsFeature(roi);

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
  }


  return false;
}


//**********************************************************************



TrigData::TrigData( std::string name )
{
    this->signature = "electron"; // default
    if(boost::contains(name,"HLT_e")) this->signature = "electron";
    else if(boost::contains(name,"HLT_g")) this->signature = "photon";
}


void TrigData::clear()
{
  this->electrons.clear(); 
  this->trig_electrons.clear(); 
  this->clusters.clear(); 
  this->photons.clear();
  this->roi = nullptr;
  this->l1 = nullptr;
  this->emCluster = nullptr;
  this->rings = nullptr;
  this->trig_photon = nullptr;
}


bool TrigData::isValid() const
{
  bool passed = (this->roi && this->l1 && this->emCluster && this->rings);
  if (this->signature == "photon"){
    return (this->trig_photon && !this->photons.empty() && passed);
  }else if (this->signature == "electron"){
    return (!this->trig_electrons.empty() && !this->electrons.empty() && passed);
  }else{
    return false;
  }
}


bool TrigData::isPassed( const xAOD::Photon *ph , std::string pidname ) const
{
  if (pidname=="tight"){
    return (bool)this->egammaPhotonCBTools[0]->accept(ph);
  }else if (pidname=="medium"){
    return (bool)this->egammaPhotonCBTools[1]->accept(ph);
  }else if (pidname=="loose"){
    return (bool)this->egammaPhotonCBTools[2]->accept(ph);
  }else{
    return true;
  }
}

bool TrigData::isPassed( const xAOD::Electron *el , float avgmu, std::string pidname ) const
{
  if (pidname=="lhtight"){
    return (bool)this->egammaElectronLHTools[0]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="lhmedium"){
    return (bool)this->egammaElectronLHTools[1]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="lhloose"){
    return (bool)this->egammaElectronLHTools[2]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="lhvloose"){
    return (bool)this->egammaElectronLHTools[3]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="dnntight"){
    return (bool)this->egammaElectronDNNTools[0]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="dnnmedium"){
    return (bool)this->egammaElectronDNNTools[1]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="dnnloose"){
    return (bool)this->egammaElectronDNNTools[2]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="dnnvloose"){
    return (bool)this->egammaElectronDNNTools[3]->accept(Gaudi::Hive::currentContext(),el,avgmu );
  }else if (pidname=="tight"){
    return (bool)this->egammaElectronCBTools[0]->accept(Gaudi::Hive::currentContext(),el );
  }else if (pidname=="medium"){
    return (bool)this->egammaElectronCBTools[1]->accept(Gaudi::Hive::currentContext(),el );
  }else if (pidname=="loose"){
    return (bool)this->egammaElectronCBTools[2]->accept(Gaudi::Hive::currentContext(),el );
  }else{
    return true;
  }
}


bool TrigData::isPassed( const xAOD::TrigRingerRings *cl , float avgmu, std::string pidname ) const
{
  if (pidname=="tight"){
    float output = this->ringerTools[0]->predict(cl);
    return (bool)this->ringerTools[0]->accept(cl, output, avgmu);
  }else if (pidname=="medium"){
    float output = this->ringerTools[1]->predict(cl);
    return (bool)this->ringerTools[1]->accept(cl, output, avgmu);
  }else if (pidname=="loose"){    
    float output = this->ringerTools[2]->predict(cl);
    return (bool)this->ringerTools[2]->accept(cl, output, avgmu);
  }else if (pidname=="vloose"){
    float output = this->ringerTools[3]->predict(cl);
    return (bool)this->ringerTools[3]->accept(cl, output, avgmu);
  }else{
    return true;
  }
}



