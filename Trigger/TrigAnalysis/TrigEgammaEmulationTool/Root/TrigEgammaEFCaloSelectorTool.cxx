/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  m_PidToolMap["Tight"] = 0;
  m_PidToolMap["Medium"]= 1;
  m_PidToolMap["Loose"] = 2;
  m_PidToolMap["VLoose"]= 3;
}
//**********************************************************************
StatusCode TrigEgammaEFCaloSelectorTool::initialize() {

  if (m_electronCaloIsEMTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  }
  if (m_electronCaloLHTool.retrieve().isFailure() ) {
    std::cout << "retrieve failed for tools" << std::endl;
  } 
  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaEFCaloSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaEFCaloSelectorTool::emulation(const xAOD::IParticleContainer *container, bool &pass, const TrigInfo &info)
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
  for(const auto el : *elContainer) {
    bit++;
    // Now creating the photon objects (dummy EDM for Calo selection at EF)
    if (info.etcut || info.ringer) {
      ATH_MSG_DEBUG(" Etcut trigger cut " << etthr);
      if (el->caloCluster()->et()*1.e-3 < etthr)  continue;
      bitAccept.set(bit-1,true); //at least one electron
    }else if(info.perf){
      pass=true;
      return true;
    }else {// pidname or idperf
      //calo cuts for photon and electron using the same asgTool
      if (!ApplyElectronCaloPid(el, info.pidname)) continue;
      ATH_MSG_DEBUG("Apply Pid " << info.pidname);
      ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << el->caloCluster()->et()*1.e-3);
      if (el->caloCluster()->et()*1.e-3 < etthr) continue;
      bitAccept.set(bit-1,true); //at least one electron
    }
    
    if(bitAccept.count() > 0)  pass=true; 
    dressDecision(el, info.trigName , pass );
  }//loop over online electrons
  return true;
}
//!==========================================================================
bool TrigEgammaEFCaloSelectorTool::ApplyElectronCaloPid(const xAOD::Electron *eg, const std::string pidname) {
  ATH_MSG_DEBUG("Apply Pid " << pidname);
 
  bool passTool=false;
  bool passSel=false;
  //eg->passSelection(passSel,pidname);
  if (pidname == "Tight") {
    passTool = m_electronCaloIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium") {
    passTool = m_electronCaloIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose") {
    passTool = m_electronCaloIsEMTool[2]->accept(eg);
  }
  else if (pidname == "VLoose") {
    passTool = m_electronCaloIsEMTool[3]->accept(eg);
  }
  else if (pidname == "LHTight") {
    passTool = m_electronCaloLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium") {
    passTool = m_electronCaloLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose") {
    passTool = m_electronCaloLHTool[2]->accept(eg);
  }
  else if (pidname == "LHVLoose") {
    passTool = m_electronCaloLHTool[3]->accept(eg);
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
