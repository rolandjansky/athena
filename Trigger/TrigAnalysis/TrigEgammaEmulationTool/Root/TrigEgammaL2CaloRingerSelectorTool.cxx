/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaL2CaloRingerSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloRingerSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/lexical_cast.hpp>
#include <math.h>

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2CaloRingerSelectorTool::
TrigEgammaL2CaloRingerSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname),
      m_selectorTool()
{
  declareProperty("CalibPathConstants", m_calibPath_constants=""              );
  declareProperty("CalibPathThresholds", m_calibPath_thresholds=""            );

  m_output    = 0;
  m_useLumiVar=false;
  m_useEtaVar=false;

}
//**********************************************************************
StatusCode TrigEgammaL2CaloRingerSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }
  m_selectorTool.setConstantsCalibPath( m_calibPath_constants ); 
  m_selectorTool.setThresholdsCalibPath( m_calibPath_thresholds ); 

  if(!m_selectorTool.initialize())
    return StatusCode::FAILURE;
  
 
  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2CaloRingerSelectorTool::finalize() {
  

  ATH_MSG_DEBUG("TrigL2CaloRingerHypo finalization completed successfully.");
  return StatusCode::SUCCESS;
}

//!==========================================================================

bool TrigEgammaL2CaloRingerSelectorTool::emulation(const xAOD::TrigEMCluster* emCluster, bool &pass, const Trig::Info &info)
{
  pass = false;
  m_output = -999;
  emCluster->auxdecor<float>("TrigEgammaL2CaloRingerSelectorTool_rnnOutput")=m_output;
  
  if(!emCluster){
    ATH_MSG_DEBUG("emCluster is nullptr");
    return false;
  }

  //retrieve rings
  const xAOD::TrigRingerRings *ringer = getTrigCaloRings(emCluster);
  
  if(!ringer){
    ATH_MSG_DEBUG("Can not found TrigRingerRings object matched with the current TrigEMCluster.");
    return false;
  }//protection

  setEtThr((info.thrHLT-3));


  ///It's ready to select the correct eta/et bin
  float eta = std::fabs(emCluster->eta());
  if(eta>2.50) eta=2.50;///fix for events out of the ranger
  float et  = emCluster->et()*1e-3; ///in GeV

  ATH_MSG_DEBUG("Et = " << et << " <  EtCut = "<< m_etCut);
  if(et < m_etCut){
    ATH_MSG_DEBUG("Event reproved by Et threshold. Et = " << et << ", EtCut = "<< m_etCut);
    return true;
  }
  auto avgmu = getOnlAverageMu();
  
  const std::vector<float> rings = ringer->rings();
  std::vector<float> refRings(rings.size());
  refRings.assign(rings.begin(), rings.end());
  

  m_output = m_selectorTool.calculate( refRings, et, eta, avgmu );
  pass = m_selectorTool.accept(m_output, et,eta,avgmu);
  
  emCluster->auxdecor<float>("TrigEgammaL2CaloRingerSelectorTool_rnnOutput")=m_output;


  ATH_MSG_DEBUG("Et = " << et << " GeV, |eta| = " << eta << " and rnnoutput = " << m_output );
  return true;
}
//!===========================================================================

