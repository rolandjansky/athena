/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuBasicTPEfficiencyPlots.cxx
 *
 *  Created on: Sep 2, 2014
 *      Author: goblirsc
 */

#include "MuonPerformanceHistUtils/DiLeptonTPEventCutFlowPlots.h"

DiLeptonTPEventCutFlowPlots::DiLeptonTPEventCutFlowPlots(PlotBase* pParent, std::string sDir)
  : MuonTPCutFlowBase (pParent, sDir) {}

void DiLeptonTPEventCutFlowPlots::initializePlots()
{
  std::vector<std::string> main_cf_steps;
  main_cf_steps.push_back("Processed");
  main_cf_steps.push_back("GRL");
  main_cf_steps.push_back("Trigger");
  main_cf_steps.push_back("HaveTag");
  main_cf_steps.push_back("HaveProbe");
  m_tp_main_selection = AddCutFlow("MainSelection",main_cf_steps);
  for (auto step: main_cf_steps) {
    m_tp_main_selection->Fill(step.c_str(), 0);           //  fill once to avoid merging issues
  }

  std::vector<std::string> tag_cf_steps;
  tag_cf_steps.push_back("TagCandidates");
  tag_cf_steps.push_back("TagQuality");
  tag_cf_steps.push_back("TagPt");
  tag_cf_steps.push_back("TagEta");
  tag_cf_steps.push_back("TagTrigger");
  tag_cf_steps.push_back("TagIP");
  tag_cf_steps.push_back("TagIsolation");
  m_tp_tag_selection = AddCutFlow("TagSelection",tag_cf_steps);
  for (auto step: tag_cf_steps) {
    m_tp_tag_selection->Fill(step.c_str(), 0);            //  fill once to avoid merging issues
  }

  std::vector<std::string> probe_cf_steps;
  probe_cf_steps.push_back("ProbeCandidates");
  probe_cf_steps.push_back("ProbeInvMass");
  probe_cf_steps.push_back("ProbeKinematics");
  probe_cf_steps.push_back("ProbeQuality");
  probe_cf_steps.push_back("ProbeIsolation");
  probe_cf_steps.push_back("ProbeIP");
  probe_cf_steps.push_back("ProbeDeltaPhi");
  probe_cf_steps.push_back("ProbeDeltaEta");
  probe_cf_steps.push_back("ProbeDR_exTP");

  m_tp_probe_selection = AddCutFlow("ProbeSelection",probe_cf_steps);
  for (auto step: probe_cf_steps) {
    m_tp_probe_selection->Fill(step.c_str(), 0);          //  fill once to avoid merging issues
  }
}

