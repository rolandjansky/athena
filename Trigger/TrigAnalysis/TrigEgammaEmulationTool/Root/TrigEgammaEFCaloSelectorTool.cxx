/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaEFCaloSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaEFCaloSelectorTool.h"
#include "PATCore/AcceptData.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaEFCaloSelectorTool::
TrigEgammaEFCaloSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  declareProperty("ElectronCaloPPSelector"  , m_electronCaloIsEMTool  );
  declareProperty("ElectronCaloLHSelector"  , m_electronCaloLHTool    ); 
  //declareProperty("ShowerBuilderTool"       , m_showerBuilder, "Handle of instance of EMShowerBuilder");
  //declareProperty("FourMomBuilderTool"      , m_fourMomBuilder, "Handle of instance of EMFourBuilder" );
  
  m_PidToolMap["Tight"] = 0;
  m_PidToolMap["Medium"]= 1;
  m_PidToolMap["Loose"] = 2;
  m_PidToolMap["VLoose"]= 3;
}
//**********************************************************************
StatusCode TrigEgammaEFCaloSelectorTool::initialize() {
  
  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

  if (m_electronCaloIsEMTool.retrieve().isFailure() ) {
    ATH_MSG_WARNING("retrieve failed for tools");
  }
  if (m_electronCaloLHTool.retrieve().isFailure() ) {
    ATH_MSG_WARNING("retrieve failed for tools");
  } 
  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaEFCaloSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFCaloSelectorTool::emulation(const xAOD::IParticleContainer *container, bool &pass, const Trig::Info &info)
{
  pass=false;
  if(!container){
    ATH_MSG_WARNING("Can not emulate EF Calo step. Container is null");
    return false;
  }
 
  const xAOD::ElectronContainer *elContainer = static_cast<const xAOD::ElectronContainer *>(container);
  ATH_MSG_DEBUG("ElectronContainer size: "<<elContainer->size());
 
  boost::dynamic_bitset<> bitAccept(elContainer->size());
  unsigned bit=0;
  float etthr = info.thrHLT;
  auto avgmu = getOnlAverageMu();
  for(const auto el : *elContainer) {
    bit++;
    // Now creating the photon objects (dummy EDM for Calo selection at EF)
    if (info.ringer) {
      ATH_MSG_DEBUG(" Etcut trigger cut " << etthr);
      if (el->caloCluster()->et()*1.e-3 < etthr)  continue;
      bitAccept.set(bit-1,true); //at least one electron
    }else if(info.perf || info.etcut){
      pass=true;
      return true;
    }else {// pidname or idperf
      if (el->caloCluster()->et()*1.e-3 < etthr) continue;
      //ATH_MSG_INFO("Original LHValue: " << el->likelihoodValue("LHCaloValue"));
      //calo cuts for photon and electron using the same asgTool
      if(info.applyEFCaloPid){
        if (!ApplyCaloPid(el, info.pidname, avgmu)) continue;
        ATH_MSG_DEBUG("Apply Pid " << info.pidname);
      }
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et()*1.e-3);
      bitAccept.set(bit-1,true); //at least one electron
    }
    if(bitAccept.count() > 0)  pass=true; 
    dressDecision(el, info.trigName , pass );
  }//loop over online electrons
  
  return true;
}
//!==========================================================================
bool TrigEgammaEFCaloSelectorTool::ApplyCaloPid(const xAOD::Egamma *eg, const std::string pidname, double avg_mu) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  bool passTool=false;
  bool passSel=false;
  //float lhValue=0.0;
  //eg->passSelection(passSel,pidname);
  
  const EventContext ctx = Gaudi::Hive::currentContext();

  if (pidname == "Tight") {
    passTool = (bool) m_electronCaloIsEMTool[0]->accept(ctx, eg);
  }
  else if (pidname == "Medium") {
    passTool = (bool) m_electronCaloIsEMTool[1]->accept(ctx, eg);
  }
  else if (pidname == "Loose") {
    passTool = (bool) m_electronCaloIsEMTool[2]->accept(ctx, eg);
  }
  else if (pidname == "VLoose") {
    passTool = (bool) m_electronCaloIsEMTool[3]->accept(ctx, eg);
  }
  else if (pidname == "LHTight") {
    passTool = (bool) m_electronCaloLHTool[0]->accept(ctx, eg, avg_mu);
    //lhValue = m_electronCaloLHTool[0]->getTResult().getResult(0);
  }
  else if (pidname == "LHMedium") {
    passTool = (bool) m_electronCaloLHTool[1]->accept(ctx, eg, avg_mu);
    //lhValue = m_electronCaloLHTool[1]->getTResult().getResult(0);
  }
  else if (pidname == "LHLoose") {
    passTool = (bool) m_electronCaloLHTool[2]->accept(ctx, eg, avg_mu);
    //lhValue = m_electronCaloLHTool[2]->getTResult().getResult(0);
  }
  else if (pidname == "LHVLoose") {
    passTool = (bool) m_electronCaloLHTool[3]->accept(ctx, eg, avg_mu);
    //lhValue = m_electronCaloLHTool[3]->getTResult().getResult(0);
  }
  else {
    ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return true;
  }
    
 // ATH_MSG_INFO("Reproduced LHValue: " << lhValue);

  if (passTool != passSel) {
    ATH_MSG_DEBUG("Apply Pid ERROR in Tool and eg->passSelection is " << passSel << " and ApplyElectronCaloPid is " << passTool);
  }
  return passTool;
}
//!==========================================================================
