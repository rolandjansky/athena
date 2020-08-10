/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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



StatusCode TrigEgammaMonitorElectronAlgorithm::fillHistograms( const EventContext& ctx ) const  
{

    ATH_MSG_DEBUG("Executing TrigEgammaMonitorElectronAlgorithm");

    if(tdt()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS; 
    }
    

    ATH_MSG_DEBUG("Chains for Analysis " << m_trigList);

    for(const auto& trigger : m_trigList){
        
        const TrigInfo info = getTrigInfo(trigger);
        
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger << " " << info.trigName);
 

        std::vector< std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision*>> pairObjs;
        if ( executeNavigation( ctx, info.trigName,info.trigThrHLT,info.trigPidType, pairObjs).isFailure() ) 
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







StatusCode TrigEgammaMonitorElectronAlgorithm::executeNavigation( const EventContext& ctx, 
                                                             std::string trigItem,
                                                             float etthr,
                                                             std::string pidname,
                                                             std::vector<std::pair<const xAOD::Egamma*, const TrigCompositeUtils::Decision* >> &pairObjs) const
{

  ATH_MSG_DEBUG("Apply navigation selection "); 

  SG::ReadHandle<xAOD::ElectronContainer> offElectrons(m_offElectronKey, ctx);

  if(!offElectrons.isValid())
  {
    ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
	  return StatusCode::FAILURE;
  }


  const std::string decor="is"+pidname;
  for(const auto& eg : *offElectrons ){

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
        if(!ApplyElectronPid(eg,pidname)){
	        ATH_MSG_DEBUG("Fails ElectronID "<< pidname);
	        continue;
	      }
	      ATH_MSG_DEBUG("Passes ElectronID "<< pidname);
      }

      if (m_forceProbeIsolation) {///default is false
        if (!isIsolated(eg, m_offProbeIsolation))  continue;///default is Loose
      }

      if(m_forceVetoVeryLoose){///default is false
        bool veto = (bool)this->m_electronLHVLooseTool->accept(eg);
        if(veto)  continue;
      }

      xAOD::Electron *el = new xAOD::Electron(*eg);
      el->auxdecor<bool>(decor)=static_cast<bool>(true);

      match()->match(el, trigItem, dec);
      std::pair< const xAOD::Electron*, const TrigCompositeUtils::Decision * > pair(el,dec);
      pairObjs.push_back(pair);

  }

  ATH_MSG_DEBUG("BaseToolMT::Electron TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}




