/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

/**********************************************************************
 * AsgTool: TrigEgammaEFPhotonSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaEFPhotonSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include "PATCore/AcceptData.h"
using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEFPhotonSelectorTool::
TrigEgammaEFPhotonSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("PhotonOnlPPSelector"        , m_photonOnlIsEMTool    );
  declareProperty("IsolationSelector"          , m_isolationTool        );

  m_PidToolMap["Tight"] = 0;
  m_PidToolMap["Medium"]= 1;
  m_PidToolMap["Loose"] = 2;

}
//**********************************************************************
StatusCode TrigEgammaEFPhotonSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

 
  if (m_photonOnlIsEMTool.retrieve().isFailure() ) {
    ATH_MSG_WARNING("retrieve failed for tools");
  }

  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaEFPhotonSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFPhotonSelectorTool::emulation( const xAOD::IParticleContainer *container, bool &pass, const Trig::Info &info)
{
  //apply pids here
  //check if is etcut, perf, etc
  pass=false;
  if(!container){
    ATH_MSG_WARNING("Can not emulate EF photon step. Container is null");
    return false;
  }
  if (boost::contains(info.pidname,"1")) return false; // Not emulating Run1

  /*
     if(!found && pass){
      pass = true;  
      found = true;
    }

    dressDecision( "TrigEgammaEmulationTool_"+trigger , pass );
  */
  return true;
}
//!==========================================================================
bool TrigEgammaEFPhotonSelectorTool::ApplyPhotonPid(const xAOD::Photon *eg, const std::string pidname) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool=false;
  bool passSel=false;
  eg->passSelection(passSel,pidname);
  //If Alg becomes Reentrant this needs to change
  const EventContext ctx = Gaudi::Hive::currentContext();
  
  if (pidname == "Tight") {
    passTool = (bool)m_photonOnlIsEMTool[0]->accept(ctx,eg);
  }
  else if (pidname == "Medium") {
    passTool = (bool)m_photonOnlIsEMTool[1]->accept(ctx,eg);
  }
  else if (pidname == "Loose") {
    passTool = (bool)m_photonOnlIsEMTool[2]->accept(ctx,eg);
  }
  else {
    ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return true;
  }

  if (passTool != passSel) {
    ATH_MSG_DEBUG("Apply Pid ERROR in Tool and eg->passSelection is " << passSel << " and ApplyElectronCaloPid is " << passTool);
  }

  return passTool;
}
//!==========================================================================
bool TrigEgammaEFPhotonSelectorTool::ApplyIsolation(const xAOD::Photon *ph, const Trig::Info &info)
{
  bool status = false;
  bool isIsolated = false;

  if(boost::contains(info.trigName,"icalotight")){
    status = m_isolationTool[0]->emulation( ph, isIsolated, info );
  }else if(boost::contains(info.trigName,"icalomedium")){
    status =  m_isolationTool[1]->emulation( ph, isIsolated, info );
  }else if(boost::contains(info.trigName,"icaloloose")){
    status =  m_isolationTool[2]->emulation( ph, isIsolated, info );
  }else if(boost::contains(info.trigName,"icalovloose")){
    status =  m_isolationTool[3]->emulation( ph, isIsolated, info );
  }else{
    ATH_MSG_DEBUG("No Isolation tool, continue without ISO");
    return true;
  }

  if (!status)
    ATH_MSG_WARNING("Problem to emulate the isolation selector.");


  return isIsolated;
}
