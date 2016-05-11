/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaEFElectronSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaEFElectronSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEFElectronSelectorTool::
TrigEgammaEFElectronSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("ElectronOnlPPSelector"      , m_electronOnlIsEMTool  );
  declareProperty("ElectronOnlLHSelector"      , m_electronOnlLHTool    );
  declareProperty("ElectronTrigOnlLHSelector", m_electronTrigOnlLHTool);
  declareProperty("TrackIsolationTool", m_trackIsolationTool);

  m_PidToolMap["Tight"] = 0;
  m_PidToolMap["Medium"]= 1;
  m_PidToolMap["Loose"] = 2;
  m_PidToolMap["VLoose"]= 3;

}
//**********************************************************************
StatusCode TrigEgammaEFElectronSelectorTool::initialize() {

  if (m_electronOnlIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_electronOnlLHTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }

  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaEFElectronSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::emulation(const xAOD::IParticleContainer *container, bool &pass, const std::string &trigger)
{
  //apply pids here
  //check if is etcut, perf, etc
  pass=false;

  if(!container){
    ATH_MSG_WARNING("Can not emulate EF electron step. Container is null");
    return false;
  }

  const xAOD::ElectronContainer *elContainer = static_cast<const xAOD::ElectronContainer *>(container);
  ATH_MSG_DEBUG("ElectronContainer size: "<<elContainer->size());
  std::string trigItem = trigger;
  trigItem.erase(0, 4); //Removes HLT_ prefix from name
  bool isL1 = false;
  bool perf = false;
  bool etcut = false;
  bool isolation = false;  
  bool idperf = false;
  bool hltcalo = false;
  float etthr = 0;
  float l1thr = 0;
  std::string type = "";
  std::string l1type = "";
  std::string pidname = "";
  if (boost::contains(trigItem,"iloose"))  isolation = true;
  if (boost::contains(trigItem,"idperf"))  idperf = true;
  if (boost::contains(trigItem,"HTLcalo")) hltcalo = true;
  parseTriggerName(trigItem,"Loose",isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger 
  if (boost::contains(pidname,"1")) return false; // Not emulating Run1
  bool found = false;
  for(const auto& el : *elContainer){
    //emulate electron cuts
    if (idperf) {
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr) continue;
      pass=true;
    }
    else if (etcut) {
      ATH_MSG_DEBUG("Doing etcut");
      ATH_MSG_DEBUG("ET = " << el->caloCluster()->et()/1.e3 << " ET threshold = " << etthr);
      if (el->caloCluster()->et()/1.e3 < etthr)  continue;
      pass=true;
      ATH_MSG_DEBUG("Track pattern " << el->trackParticle()->patternRecoInfo());
    }
    else if (isolation || hltcalo) {
      ATH_MSG_DEBUG("Doing isolation");
      if (!ApplyElectronPid(el,pidname)) continue;
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr) continue;
      float val=0.;
      ATH_MSG_DEBUG("Isolation");
      el->isolationValue(val,xAOD::Iso::ptcone20);
      if ((val/el->trackParticle()->pt()) > 0.1)  continue;
      pass=true;
    }
    else {
      if (!ApplyElectronPid(el,pidname)) continue;
      ATH_MSG_DEBUG("Apply Pid " << pidname);
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr)  continue;
      pass=true;
    }

    if(!found && pass){
      pass = true;  
      found = true;
    }

    dressDecision(el, trigger , pass );
  }//loop over electrons

  return true;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool=false;
  bool passSel=false;
  //eg->passSelection(passSel,pidname);
  if (pidname == "Tight") {
    passTool = m_electronOnlIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium") {
    passTool = m_electronOnlIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose") {
    passTool = m_electronOnlIsEMTool[2]->accept(eg);
  }
  else if (pidname == "VLoose") {
    passTool = m_electronOnlIsEMTool[3]->accept(eg);
  }
  else if (pidname == "LHTight") {
    passTool = m_electronOnlLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium") {
    passTool = m_electronOnlLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose") {
    passTool = m_electronOnlLHTool[2]->accept(eg);
  }
  else if (pidname == "LHVLoose") {
    passTool = m_electronOnlLHTool[3]->accept(eg);
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

bool TrigEgammaEFElectronSelectorTool::ApplyOnlElectronPid(const xAOD::Electron *eg, const std::string pidname) {
  ATH_MSG_DEBUG("Apply *Online* Pid " << pidname);
  if (pidname == "LHTight"){
    return m_electronTrigOnlLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium"){
    return m_electronTrigOnlLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose"){
    return m_electronTrigOnlLHTool[2]->accept(eg);
  }
  else if (pidname == "LHVLoose") {
    return m_electronTrigOnlLHTool[3]->accept(eg);
  }
  else ATH_MSG_DEBUG("No Pid tool, continue without PID");
  return false;
}
bool TrigEgammaEFElectronSelectorTool::emulatePidSelection(const HLT::TriggerElement *te, const std::string pidname) {
  bool passedPid = false;
  const auto* cont  = m_trigdec->ancestor<xAOD::ElectronContainer>(te).cptr();
  if (cont) {
    for (const auto &el : *cont) {
      passedPid = ApplyOnlElectronPid(el, pidname);
      if (passedPid == true) break;
    }
  } 
  else {
    ATH_MSG_DEBUG("Electron container is NULL");
  }
  return passedPid;
}

//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::emulateIsolation(const HLT::TriggerElement *te) {
    bool isIsolated = false;
    float ptcone20_new = -99;
    float ptvarcone20_new = -99;
    float ptcone20_old = -99;
    float caloClusterEt=0.0;
    float ratio_new=0.0;
    float ratio_old=0.0;
    if (te == NULL) {
        ATH_MSG_INFO("emulateIsolation() \t Trigger Element is NULL");
        return isIsolated;
    }
    const auto* trackParticles  = m_trigdec->ancestor<xAOD::TrackParticleContainer>(te).cptr();
    const auto* cont  = m_trigdec->ancestor<xAOD::ElectronContainer>(te).cptr();
    if (trackParticles == NULL) {
        ATH_MSG_INFO("emulateIsolation() \t Track particle container is NULL");
        return isIsolated;
    }
    if (cont == NULL) {
        ATH_MSG_INFO("emulateIsolation() \t Electron container is NULL");
        return isIsolated;
    }
    for (const auto &el : *cont) {
        std::vector<xAOD::Iso::IsolationType> isoTypes;
        isoTypes.push_back(xAOD::Iso::ptcone20);
        xAOD::TrackCorrection corrlist;
        corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
        xAOD::Vertex *vx = 0;
        const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(el, 0);
        el->isolationValue(ptcone20_old, xAOD::Iso::ptcone20);
        xAOD::TrackIsolation trackIsolation;
        // Adding leading trk vertex
        xAOD::Vertex leadTrkVtx;
        leadTrkVtx.makePrivateStore();
        float leadTrkpt=0.0;
        const xAOD::TrackParticle *leadTrk = NULL;
        for (const xAOD::TrackParticle *trk: *trackParticles) {
            if(trk->pt() > leadTrkpt) {
                leadTrkpt = trk->pt();
                leadTrk = trk;
            }
        }
        // set z vertex position to leading track and shift by vz
        if(leadTrk!=NULL) {
            leadTrkVtx.setZ(leadTrk->z0()+leadTrk->vz());
        }
        else {
            ATH_MSG_INFO("emulateIsolation() \t Lead track pointer null");
        }
        vx = &leadTrkVtx;
        bool testIsol = m_trackIsolationTool->trackIsolation(trackIsolation, *el, isoTypes, corrlist, vx, &tracksToExclude, trackParticles);
        if (!testIsol) {
            ATH_MSG_INFO("emulateIsolation() \t Track Isolation failed, testIsol is False");
        }
        ptcone20_new = trackIsolation.ptcones[0];
        ptvarcone20_new = trackIsolation.ptvarcones_10GeVDivPt[0];
        caloClusterEt = el->caloCluster()->et();
        ratio_old = ptcone20_old/caloClusterEt;
        // ratio_new = ptcone20_new/caloClusterEt;
        // For testing ptvarcone and ivarloose only - need to be fixed later
        ratio_new = ptvarcone20_new/caloClusterEt;
        // =======================================
        ATH_MSG_INFO("emulateIsolation() \t ptcone20 (old)    = " << ptcone20_old);
        ATH_MSG_INFO("emulateIsolation() \t ptcone20 (new)    = " << ptcone20_new);
        ATH_MSG_INFO("emulateIsolation() \t ptvarcone20 (new) = " << ptvarcone20_new);
        ATH_MSG_INFO("emulateIsolation() \t CaloCluster Et    = " << caloClusterEt);
        ATH_MSG_INFO("emulateIsolation() \t ratio (old)       = " << ratio_old);
        ATH_MSG_INFO("emulateIsolation() \t ratio (new)       = " << ratio_new);
        if (ratio_new > 0.1) {
            continue; 
        }
        else {
            isIsolated = true;
        }
    }
    ATH_MSG_INFO("emulateIsolation() \t isIsolation = " << isIsolated);
    return isIsolated;
}
//!==========================================================================
//
//
