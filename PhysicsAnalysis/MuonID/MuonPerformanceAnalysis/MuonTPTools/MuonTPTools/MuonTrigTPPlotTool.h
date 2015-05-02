/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTrigTPPlots.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc /a.m.
 */

#ifndef MUONTRIGTPPLOTTOOL_H_
#define MUONTRIGTPPLOTTOOL_H_
#include "MuonTPTools/IMuonTPPlotTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "AsgTools/AsgTool.h"
#include <map>

class MuonTrigTPPlotTool: public MuonTPPlotTool
{
    ASG_TOOL_CLASS(MuonTrigTPPlotTool, IMuonTPPlotTool)

public:
    MuonTrigTPPlotTool(std::string name);

    virtual std::vector<MuonTPEfficiencyPlotBase*> AddPlots(std::string sDir, bool isMatched);
    virtual std::vector<MuonTPCutFlowBase*>        AddCutFlowPlots(std::string sDir);

protected:

    bool m_do_TPBasic;
    bool m_do_Valid;
    bool m_apply_SF;    
};

#endif /* MUONTRIGTPPLOTTOOL_H_ */
