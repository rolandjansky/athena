/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPPlots.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/DiMuonTPPlotTool.h"
#include "MuonPerformanceHistUtils/ZmumuBasicTPEfficiencyPlots.h"
#include "MuonPerformanceHistUtils/ZmumuTagProbeDileptonPlots.h"
#include "MuonPerformanceHistUtils/ZmumuFineEtaPhiEfficiencyPlots.h"
#include "MuonPerformanceHistUtils/DiLeptonTPEventCutFlowPlots.h"
#include "MuonPerformanceHistUtils/ZmumuDetRegionEfficiencyPlots.h"
#include "MuonPerformanceHistUtils/JPsiTagProbeDileptonPlots.h"
#include "MuonPerformanceHistUtils/JPsiBasicTPEfficiencyPlots.h"
#include "MuonPerformanceHistUtils/MuonTrigTPEfficiencyPlots.h"

DiMuonTPPlotTool::DiMuonTPPlotTool(std::string name)
  :  MuonTPPlotTool(name){
  declareProperty("doTrkValidPlots",m_do_Valid=false);
  declareProperty("doZmumuKinematicPlots",m_do_TPBasic_Zmm=false);
  declareProperty("doJPsiKinematicPlots",m_do_TPBasic_JPsi=false);
  declareProperty("doZmumuDileptonPlots",m_do_DiLepton_Zmm=false);
  declareProperty("doJPsiDileptonPlots",m_do_DiLepton_JPsi=false);
  declareProperty("doFineEtaPhiPlots",m_do_TPFineEtaPhi=false);
  declareProperty("doDetectorRegionPlots",m_do_DetRegions=false);
  declareProperty("doTrigValidPlots",m_do_TriggerPlots=false);
  declareProperty("ApplyScaleFactors",m_apply_SF=false);
}

std::vector<MuonTPEfficiencyPlotBase*> DiMuonTPPlotTool::AddPlots(std::string sDir, bool isMatched){

  std::vector<MuonTPEfficiencyPlotBase*> out (0);
  if (m_do_Valid) out.push_back( new MuonTPEfficiencyPlots(0, sDir, isMatched));
  if (m_do_TPBasic_Zmm) out.push_back( new ZmumuBasicTPEfficiencyPlots(0, sDir+"/basic_", isMatched,m_apply_SF));
  if (m_do_TPBasic_JPsi) out.push_back( new JPsiBasicTPEfficiencyPlots(0, sDir+"/basic_", isMatched,m_apply_SF));
  if (m_do_TPFineEtaPhi) out.push_back( new ZmumuFineEtaPhiEfficiencyPlots(0, sDir+"/", isMatched,m_apply_SF));
  if (m_do_DiLepton_Zmm) out.push_back( new ZmumuTagProbeDileptonPlots(0, sDir+"/", isMatched,m_apply_SF));
  if (m_do_DiLepton_JPsi) out.push_back( new JPsiTagProbeDileptonPlots(0, sDir+"/", isMatched,m_apply_SF));
  if (m_do_DetRegions) out.push_back(new ZmumuDetRegionEfficiencyPlots(0,sDir+"/",isMatched,m_apply_SF));
  if (m_do_TriggerPlots) out.push_back(new MuonTrigTPEfficiencyPlots(0,sDir+"/",isMatched,m_apply_SF));
  return out;

}

std::vector<MuonTPCutFlowBase*> DiMuonTPPlotTool::AddCutFlowPlots(std::string sDir){

  std::vector<MuonTPCutFlowBase*> out (0);
  out.push_back (new DiLeptonTPEventCutFlowPlots(0,sDir));
  return out;

}
