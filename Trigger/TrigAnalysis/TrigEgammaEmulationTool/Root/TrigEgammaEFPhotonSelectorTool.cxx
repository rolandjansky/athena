/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEFPhotonSelectorTool::
TrigEgammaEFPhotonSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("PhotonOnlPPSelector"        , m_photonOnlIsEMTool    );

  m_PidToolMap["Tight"] = 0;
  m_PidToolMap["Medium"]= 1;
  m_PidToolMap["Loose"] = 2;

}
//**********************************************************************
StatusCode TrigEgammaEFPhotonSelectorTool::initialize() {

  if (m_photonOnlIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }

  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaEFPhotonSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFPhotonSelectorTool::emulation( const xAOD::IParticleContainer *container, bool &pass, const std::string &trigger)
{
  //apply pids here
  //check if is etcut, perf, etc
  pass=false;
  if(!container){
    ATH_MSG_WARNING("Can not emulate EF photon step. Container is null");
    return false;
  }
  std::string trigItem = trigger;
  trigItem.erase(0, 4); //Removes HLT_ prefix from name
  bool isL1 = false;
  bool perf = false;
  bool etcut = false;
  //bool isolation = false;  
  //bool idperf = false;
  //bool hltcalo = false;
  float etthr = 0;
  float l1thr = 0;
  std::string type = "";
  std::string l1type = "";
  std::string pidname = "";
  //if (boost::contains(trigItem,"iloose"))  isolation = true;
  //if (boost::contains(trigItem,"idperf"))  idperf = true;
  //if (boost::contains(trigItem,"HTLcalo")) hltcalo = true;
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger 
  if (boost::contains(pidname,"1")) return false; // Not emulating Run1

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
  if (pidname == "Tight") {
    passTool = m_photonOnlIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium") {
    passTool = m_photonOnlIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose") {
    passTool = m_photonOnlIsEMTool[2]->accept(eg);
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
