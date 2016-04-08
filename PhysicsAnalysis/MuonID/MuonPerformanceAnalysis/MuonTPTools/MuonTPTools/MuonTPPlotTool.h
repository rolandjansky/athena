/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPPlots.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef MUONTPPLOTTOOL_H_
#define MUONTPPLOTTOOL_H_
#include "MuonTPTools/IMuonTPPlotTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "AsgTools/AsgTool.h"
#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include <map>

// base class for the plotting tools.
// takes care of most technical aspects
// you typically only need to overload AddPlots, in order to add the plots you like
// define the plot classes to add in the HistUtils package

class MuonTPPlotTool:
        public asg::AsgTool,
		virtual public IMuonTPPlotTool {
// 		ASG_TOOL_CLASS(MuonTPPlotTool, IMuonTPPlotTool)

public:
        MuonTPPlotTool(std::string name);

        virtual StatusCode RegisterPlots (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools);

        virtual std::vector<MuonTPEfficiencyPlotBase*> AddPlots(std::string sDir, bool isMatched);
        virtual std::vector<MuonTPCutFlowBase*> AddCutFlowPlots(std::string sDir);
            // fill the histos
        virtual void fill(Probe& probe, ToolHandle <IMuonTPSelectionTool> & tp_tool, ToolHandle <IMuonTPEfficiencyTool> & eff_tool);
        virtual void fillCutFlow (std::string stage, double weight, ToolHandle <IMuonTPSelectionTool> & tp_tool);

            /// retrieve booked histograms
        std::vector<HistData> retrieveBookedHistograms();
        std::vector<std::pair <TGraph*,  std::string> > retrieveBookedGraphs();

        virtual void CalcEff(void);


        virtual ~MuonTPPlotTool();
protected:


        std::string m_efficiencyFlag;
        std::map<std::string,std::vector<MuonTPEfficiencyPlotBase*> > m_probeTPEffPlots;
        std::map<std::string,std::vector<MuonTPEfficiencyPlotBase*> > m_matchTPEffPlots;
        std::map<std::string,std::vector<MuonTPEfficiencyPlotBase*> > m_effTPEffPlots;
        std::map<std::string,std::vector<MuonTPCutFlowBase*> > m_TPcutFlowPlots;

        bool m_only_A_side;
        bool m_only_C_side;
        double m_probe_abseta_min;
        double m_probe_abseta_max;
        bool m_doAsymmErrors;
        bool m_doEffPlots;
        bool m_doProbeMatchPlots;
};

#endif /* MUONTPPLOTTOOL_H_ */
