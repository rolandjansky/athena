/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorPhotonAlgorithm.h"


using namespace Trig;


TrigEgammaMonitorPhotonAlgorithm::TrigEgammaMonitorPhotonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  TrigEgammaMonitorAnalysisAlgorithm( name, pSvcLocator )
{}

TrigEgammaMonitorPhotonAlgorithm::~TrigEgammaMonitorPhotonAlgorithm()
{}



StatusCode TrigEgammaMonitorPhotonAlgorithm::initialize() 
{
  
  ATH_CHECK(TrigEgammaMonitorAnalysisAlgorithm::initialize());
  ATH_CHECK(m_offPhotonKey.initialize());


  for(const auto& trigName:m_trigInputList)
  {
    if(getTrigInfoMap().count(trigName) != 0){
      ATH_MSG_WARNING("Trigger already booked, removing from trigger list " << trigName);
    }else {
      m_trigList.push_back(trigName);
      setTrigInfo(trigName);
    }
  }


 
  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaMonitorPhotonAlgorithm::fillHistograms( const EventContext& ctx ) const  
{
    ATH_MSG_DEBUG("Executing TrigEgammaMonitorPhotonAlgorithm");


    if(tdt()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS; 
    }
    
    
    ATH_MSG_DEBUG("Chains for Analysis " << m_trigList);

    for(const auto& trigger : m_trigList){
        
        const TrigInfo info = getTrigInfo(trigger);
        
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger << " " << info.trigName);
          
        std::vector< std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs;
    
        if ( executeNavigation( ctx, info.trigName,info.trigThrHLT,pairObjs).isFailure() ) 
        {
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }


        fillDistributions( pairObjs, info );
        fillEfficiencies( pairObjs, info );
        fillResolutions( pairObjs, info );


        ATH_MSG_DEBUG("End Chain Analysis ============================= " << trigger);
    } // End loop over trigger list
    
    
    return StatusCode::SUCCESS;
}





StatusCode TrigEgammaMonitorPhotonAlgorithm::executeNavigation( const EventContext& ctx, std::string trigItem, float etthr, 
                                                       std::vector<std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> &pairObjs) 
  const
{
  ATH_MSG_DEBUG("Apply navigation selection");


  SG::ReadHandle<xAOD::PhotonContainer> offPhotons(m_offPhotonKey, ctx);

  if(!offPhotons.isValid())
  {
    ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
	  return StatusCode::FAILURE;
  }
 

  const std::string decor="is"+m_photonPid;

  for(const auto& eg : *offPhotons ){
      const TrigCompositeUtils::Decision *dec=nullptr; 
      if(!eg->caloCluster()){
          ATH_MSG_DEBUG("No caloCluster");
          continue;
      } 
      if( !(getCluster_et(eg) > (etthr-5.)*Gaudi::Units::GeV)) continue; //Take 2GeV above threshold
      if(!eg->passSelection(m_photonPid)) continue;
      if(m_doUnconverted){
          if (eg->vertex()){
              ATH_MSG_DEBUG("Removing converted photons, continuing...");
              continue;
          }
      }
      xAOD::Photon *ph = new xAOD::Photon(*eg);
      ph->auxdecor<bool>(decor)=static_cast<bool>(true);
      match()->match(ph, trigItem, dec, TrigDefs::includeFailedDecisions);
      //match()->match(ph, trigItem, dec);
      std::pair< const xAOD::Photon*, const TrigCompositeUtils::Decision * > pair(ph,dec);
      pairObjs.push_back(pair);

  }

  ATH_MSG_DEBUG("BaseToolMT::Photon TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}



