/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorElectronAlgorithm.h"

using namespace Trig;


TrigEgammaMonitorElectronAlgorithm::TrigEgammaMonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  TrigEgammaMonitorAnalysisAlgorithm( name, pSvcLocator )
{}

TrigEgammaMonitorElectronAlgorithm::~TrigEgammaMonitorElectronAlgorithm()
{}



StatusCode TrigEgammaMonitorElectronAlgorithm::initialize() 
{
  ATH_CHECK(TrigEgammaMonitorAnalysisAlgorithm::initialize());

  ATH_CHECK(m_offElectronKey.initialize());

  
  for(auto& trigName : m_trigInputList)
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



StatusCode TrigEgammaMonitorElectronAlgorithm::fillHistograms( const EventContext& ctx ) const  
{

    ATH_MSG_DEBUG("Executing TrigEgammaMonitorElectronAlgorithm");

    if(isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS; 
    }
    

    ATH_MSG_DEBUG("Chains for Analysis " << m_trigList);

    for(const auto& trigger : m_trigList){
        
        const TrigInfo info = getTrigInfo(trigger);
        
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger << " " << info.trigger);
 
        std::vector< std::pair<std::shared_ptr<const xAOD::Egamma>, const TrigCompositeUtils::Decision*>> pairObjs;
        if ( executeNavigation( ctx, info.trigger,info.etthr,info.pidname, pairObjs).isFailure() ) 
        {
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }

        std::vector< std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*>> pairObjsRaw;
        pairObjsRaw.reserve(pairObjs.size());
        for (const auto& itr : pairObjs) {
          pairObjsRaw.emplace_back(itr.first.get(), itr.second);
        }


        fillDistributions( pairObjsRaw, info );
        fillEfficiencies( pairObjsRaw, info );
        fillResolutions( pairObjsRaw, info );

        ATH_MSG_DEBUG("End Chain Analysis ============================= " << trigger);
    } // End loop over trigger list
 

    return StatusCode::SUCCESS;
}







StatusCode TrigEgammaMonitorElectronAlgorithm::executeNavigation( const EventContext& ctx, 
                                                             const std::string& trigItem,
                                                             float etthr,
                                                             const std::string& pidName,
                                                             std::vector<std::pair<std::shared_ptr<const xAOD::Egamma>, const TrigCompositeUtils::Decision* >> &pairObjs) const
{

  ATH_MSG_DEBUG("Apply navigation selection "); 

  SG::ReadHandle<xAOD::ElectronContainer> offElectrons(m_offElectronKey, ctx);

  if(!offElectrons.isValid())
  {
    ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
	  return StatusCode::FAILURE;
  }


  const std::string decor="is"+pidName;
  for(const auto *const eg : *offElectrons ){

      const TrigCompositeUtils::Decision *dec=nullptr; 

      if(!eg->trackParticle()){
          ATH_MSG_DEBUG("No track Particle");
          continue;
      }
      if(!eg->caloCluster()){
          ATH_MSG_DEBUG("No caloCluster");
          continue;
      }

      if(m_forceEtThr){///default is true
        if( !( getEt(eg)  > (etthr-5.)*1.e3) ) continue;
      }

      if(m_rmCrack){///default is true
        if ( (fabs(eg->eta())>1.37 && fabs(eg->eta())<1.52) || fabs(eg->eta())>2.47 )  continue; 
      }

      if(m_forcePidSelection){///default is true
        if(!ApplyElectronPid(eg,pidName)){
	        ATH_MSG_DEBUG("Fails ElectronID "<< pidName);
	        continue;
	      }
	      ATH_MSG_DEBUG("Passes ElectronID "<< pidName);
      }

      if (m_forceProbeIsolation) {///default is false
        if (!isIsolated(eg, m_offProbeIsolation))  continue;///default is Loose
      }

      if(m_forceVetoVeryLoose){///default is false
        bool veto = ApplyElectronPid(eg,"lhvloose"); 
        if(veto)  continue;
      }

      const auto el = std::make_shared<const xAOD::Electron>(*eg);
      el->auxdecor<bool>(decor)=static_cast<bool>(true);

      match()->match(el.get(), trigItem, dec, TrigDefs::includeFailedDecisions);
      //match()->match(el, trigItem, dec);
      pairObjs.emplace_back(el, dec);

  }

  ATH_MSG_DEBUG("BaseToolMT::Electron TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}




