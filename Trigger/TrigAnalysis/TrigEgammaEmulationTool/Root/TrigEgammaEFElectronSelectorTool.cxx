/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


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
#include <boost/dynamic_bitset.hpp>
#include "PATCore/AcceptData.h"
using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEFElectronSelectorTool::
TrigEgammaEFElectronSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  //declareProperty("TrackIsolationTool"         , m_trackIsolationTool   );
  declareProperty("ElectronOnlPPSelector"      , m_electronOnlIsEMTool          );
  declareProperty("ElectronOnlLHSelector"      , m_electronOnlLHTool            );
  declareProperty("ElectronTrigOnlLHSelector"  , m_electronTrigOnlLHTool        );
  declareProperty("IsolationSelector"          , m_isolationTool                );
  declareProperty("LikelihoodInfo"             , m_lhinfo = ""                  );
}
//**********************************************************************
StatusCode TrigEgammaEFElectronSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }
  if (m_electronOnlIsEMTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("retrieve failed for tools");
    return StatusCode::FAILURE;
  }
  if (m_electronOnlLHTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("retrieve failed for tools");
    return StatusCode::FAILURE;
  }
  if (m_isolationTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("retrieve failed for tools");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaEFElectronSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::emulation(const xAOD::IParticleContainer *container, bool &pass, const Trig::Info &info)
{
  pass=false;

  // checking when LH trigger, must be check if the lhinfo match with the trigger name
  if(info.lhinfo != m_lhinfo){
    ATH_MSG_DEBUG("This selector does not  match with the likelihood information [ " << info.lhinfo << " ]");
    return false;
  }
  if(!container){
    ATH_MSG_WARNING("Can not emulate EF electron step. Container is null");
    return false;
  }

  const xAOD::ElectronContainer *elContainer = static_cast<const xAOD::ElectronContainer *>(container);
  ATH_MSG_DEBUG("ElectronContainer size: "<<elContainer->size());

  if (boost::contains(info.pidname,"1")) return false; // Not emulating Run1

  boost::dynamic_bitset<> bitAccept(elContainer->size());
  unsigned bit = 0;
  float etthr  = info.thrHLT;
  auto avgmu = getOnlAverageMu();

  for(const auto& el : *elContainer){
    bit++;
    ATH_MSG_INFO("Size = " << el->trackParticle()->numberOfParameters());
    //auto maxParameters = el->trackParticle()->numberOfParameters();

    //emulate electron cuts
    if (info.idperf  || info.etcut || info.perf ) {
      //ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      //if (el->caloCluster()->et()/1.e3 < etthr) continue;
      bitAccept.set(bit-1,true);
    }
    else if (info.isolation || info.hltcalo) {
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr) continue;
      if (!ApplyElectronPid(el,info.pidname,avgmu)) continue;
      ATH_MSG_DEBUG("Isolation");
      // Apply isolation
      if(!ApplyIsolation(el,info))  continue;
      bitAccept.set(bit-1,true);
    }
    else {
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et());
      if (el->caloCluster()->et()/1.e3 < etthr)  continue;
      //ATH_MSG_INFO("Original LHValue: " << el->likelihoodValue("LHCaloValue"));
      if (!ApplyElectronPid(el,info.pidname,avgmu)) continue;
      ATH_MSG_DEBUG("Apply Pid " << info.pidname);
      bitAccept.set(bit-1,true);
    }
    dressDecision(el, info.trigName , pass );
  }//loop over electrons

  if(bitAccept.count() >0)  pass=true;
  return true;
}
//!==========================================================================
bool TrigEgammaEFElectronSelectorTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname, 
                                                        double avgmu) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool=false;
  bool passSel=false;
  eg->passSelection(passSel,pidname);
  //If Alg becomes Reentrant this needs to change
  const EventContext ctx = Gaudi::Hive::currentContext();

  if (pidname == "Tight") {
    passTool = (bool)m_electronOnlIsEMTool[0]->accept(ctx,eg);
  }
  else if (pidname == "Medium") {
    passTool = (bool)m_electronOnlIsEMTool[1]->accept(ctx,eg);
  }
  else if (pidname == "Loose") {
    passTool = (bool)m_electronOnlIsEMTool[2]->accept(ctx,eg);
  }
  else if (pidname == "VLoose") {
    passTool = (bool)m_electronOnlIsEMTool[3]->accept(ctx,eg);
  }
  else if (pidname == "LHTight") {
    passTool = (bool)m_electronOnlLHTool[0]->accept(ctx,eg,avgmu);
  }// Tight
  else if (pidname == "LHMedium") {
    passTool = (bool)m_electronOnlLHTool[1]->accept(ctx,eg,avgmu);
  }// Medium
  else if (pidname == "LHLoose") {
    passTool = (bool)m_electronOnlLHTool[2]->accept(ctx,eg,avgmu);
  }// Loose
  else if (pidname == "LHVLoose") {
    passTool = (bool)m_electronOnlLHTool[3]->accept(ctx,eg,avgmu);
  }// VeryLoose
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
bool TrigEgammaEFElectronSelectorTool::ApplyIsolation(const xAOD::Electron *el, const Trig::Info &info)
{
  bool status = false;
  bool isIsolated = false;
  ATH_MSG_DEBUG( "In TrigEgammaEFElectronSelectorTool::ApplyIsolation, trigName: " << info.trigName);

  if(boost::contains(info.trigName,"ivartight")){
    status = m_isolationTool[0]->emulation( el, isIsolated, info );
  }else if(boost::contains(info.trigName,"ivarmedium")){
    status =  m_isolationTool[1]->emulation( el, isIsolated, info );
  }else if(boost::contains(info.trigName,"ivarloose")){
    status =  m_isolationTool[2]->emulation( el, isIsolated, info );
  }else if(boost::contains(info.trigName,"iloose")){
    status =  m_isolationTool[3]->emulation( el, isIsolated, info );
  }else if(boost::contains(info.trigName,"icalotight")){
    status =  m_isolationTool[4]->emulation( el, isIsolated, info );
  }else if(boost::contains(info.trigName,"icalomedium")){
    status =  m_isolationTool[5]->emulation( el, isIsolated, info );
  }else if(boost::contains(info.trigName,"icaloloose")){
    status =  m_isolationTool[6]->emulation( el, isIsolated, info );
  }else{
    ATH_MSG_DEBUG("No Isolation tool, continue without ISO");
    return true;
  }

  if (!status)
    ATH_MSG_WARNING("Problem to emulate the isolation selector.");

  return isIsolated;
}
//!==========================================================================

