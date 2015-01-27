/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuBasicTPEfficiencyPlots.cxx
 *
 *  Created on: Sep 2, 2014
 *      Author: goblirsc
 */

#include "MuonPerformanceHistUtils/ZmumuTPEventCutFlowPlots.h"

ZmumuTPEventCutFlowPlots::ZmumuTPEventCutFlowPlots(PlotBase* pParent, std::string sDir)
  : MuonTPCutFlowBase (pParent, sDir) {

    std::vector<std::string> main_cf_steps;
    main_cf_steps.push_back("Processed");
    main_cf_steps.push_back("HaveTag");
    main_cf_steps.push_back("HaveProbe");
    tp_main_selection = AddCutFlow("MainSelection",main_cf_steps);


    std::vector<std::string> tag_cf_steps;
    tag_cf_steps.push_back("TagCandidates");
    tag_cf_steps.push_back("TagQuality");
    tag_cf_steps.push_back("TagPt");
    tag_cf_steps.push_back("TagEta");
    tag_cf_steps.push_back("TagTrigger");
    tag_cf_steps.push_back("TagIsolation");
    tp_tag_selection = AddCutFlow("TagSelection",tag_cf_steps);

    std::vector<std::string> probe_cf_steps;
    probe_cf_steps.push_back("ProbeCandidates");
    probe_cf_steps.push_back("ProbeKinematics");
    probe_cf_steps.push_back("ProbeQuality");
    probe_cf_steps.push_back("ProbeIsolation");
    probe_cf_steps.push_back("ProbeInvMass");
    probe_cf_steps.push_back("ProbeDeltaPhi");
    tp_probe_selection = AddCutFlow("ProbeSelection",probe_cf_steps);
    
}

