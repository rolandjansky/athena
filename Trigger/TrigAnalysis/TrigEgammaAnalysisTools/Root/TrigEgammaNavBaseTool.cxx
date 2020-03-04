/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPBaseTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 *      Joao Victor Pinto <joao.victor.da.fonseca.pinto@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Inherits from TrigEgammaAnalysisBaseTool.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "PATCore/AcceptData.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************


using namespace Trig;
using namespace TrigConf;
using namespace xAOD;
using namespace boost;


TrigEgammaNavBaseTool::TrigEgammaNavBaseTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname)
{

  declareProperty("TriggerList"               , m_trigInputList               );
  declareProperty("BasePath"                  , m_dir                         );
  declareProperty("Analysis"                  , m_anatype="Analysis"          );

  /*! Algortihm configuratio */  
  declareProperty("PhotonPid"                 , m_photonPid = "Tight"         );
  declareProperty("doUnconverted"             , m_doUnconverted=false         );
  declareProperty("OfflineProbeIsolation"     , m_offProbeIsolation="Loose"   );
  declareProperty("ForceProbeIsolation"       , m_forceProbeIsolation=false   );
  declareProperty("ForcePidSelection"         , m_forcePidSelection=true      );
  declareProperty("ForceEtThreshold"          , m_forceEtThr=true             ); 
  declareProperty("ForceVetoVeryLoose"        , m_forceVetoVeryLoose=false    ); 
  declareProperty("RemoveCrack"               , m_rmCrack=true                ); 

}



StatusCode TrigEgammaNavBaseTool::childFinalize() 
{
    ATH_MSG_VERBOSE( "TrigEgammaNavBaseTool: child Finalize tool " << name() );
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavBaseTool::childInitialize() 
{
  ATH_MSG_VERBOSE( "TrigEgammaNavBaseTool: child Initialize tool " << name() );
  ATH_CHECK(m_offElectronKey.initialize());
  ATH_CHECK(m_offPhotonKey.initialize());
 
  return StatusCode::SUCCESS;
}




StatusCode TrigEgammaNavBaseTool::executeNavigation( const EventContext& ctx,  const TrigInfo info, 
                                                     std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &pairObjs )  const
{
  ATH_MSG_DEBUG("Apply navigation selection");
  if(info.trigType == "electron") return executeElectronNavigation( ctx, info.trigName,info.trigThrHLT,info.trigPidType, pairObjs);
  else if(info.trigType=="photon") return executePhotonNavigation( ctx, info.trigName,info.trigThrHLT, pairObjs);
  return StatusCode::SUCCESS;
}




bool TrigEgammaNavBaseTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname)  const 
{
  try{
	  ATH_MSG_DEBUG("Applying Electron PID with pidname =  " << pidname);
	  bool accept=false;
	  if (pidname == "Tight"){
	    accept = (bool) m_electronIsEMTool[0]->accept(eg);
	  }
	  else if (pidname == "Medium"){
	    accept = (bool) m_electronIsEMTool[1]->accept(eg);
	    return static_cast<bool>(accept);
	  }
	  else if (pidname == "Loose"){
	    accept = (bool) m_electronIsEMTool[2]->accept(eg);
	  }
	  else if (pidname == "LHTight"){
	    accept = (bool) m_electronLHTool[0]->accept(eg);
	  }
	  else if (pidname == "LHMedium"){
	    accept = (bool) m_electronLHTool[1]->accept(eg);
	  }
	  else if (pidname == "LHLoose"){
	    accept = (bool) m_electronLHTool[2]->accept(eg);
	  }
	  else {
	    ATH_MSG_DEBUG("No Pid tool, continue without PID");
	  }
	  ATH_MSG_DEBUG("ElectronLHTool for " << pidname << " = " <<  (int) accept );
	  return accept;
  }catch (...) {
    ATH_MSG_WARNING("Failed to apply pid " << pidname);
	  return false;
  }
}


StatusCode TrigEgammaNavBaseTool::executeElectronNavigation( const EventContext& ctx, 
                                                             std::string trigItem,
                                                             float etthr,
                                                             std::string pidname,
                                                             std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &pairObjs) const
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

      TrigEgammaMatchingUtils::Element obj( trigItem ); 

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

      match()->match(el, trigItem, obj);
      std::pair< const xAOD::Electron*, const TrigEgammaMatchingUtils::Element > pair(el,obj);
      pairObjs.push_back(pair);

  }

  ATH_MSG_DEBUG("BaseTool::Electron TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}



StatusCode TrigEgammaNavBaseTool::executePhotonNavigation( const EventContext& ctx, std::string trigItem, float etthr, 
                                                           std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &pairObjs) const
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
      TrigEgammaMatchingUtils::Element td( trigItem ); 
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
      match()->match(ph, trigItem, td);
      std::pair< const xAOD::Photon*, const TrigEgammaMatchingUtils::Element > pair(ph,td);
      pairObjs.push_back(pair);

  }

  ATH_MSG_DEBUG("BaseTool::Photon TEs " << pairObjs.size() << " found.");
  return StatusCode::SUCCESS;
}



