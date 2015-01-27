/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPCutFlowPlotBase.h
 *
 *  Created on: Nov 11, 2014
 *      Author: goblirsc
 */

#ifndef MUONTPCUTFLOWPLOTBASE_H_
#define MUONTPCUTFLOWPLOTBASE_H_


#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/Probe.h"

// base class for the Muon Efficiency plots

class MuonTPCutFlowBase: public PlotBase {
public:
    MuonTPCutFlowBase();
    MuonTPCutFlowBase(PlotBase* pParent, std::string sDir);

    virtual void fill(std::string bin, double weight);
    virtual TH1* AddCutFlow (std::string name, std::vector<std::string> stages);

protected:
    // cache the histo for each bin label - avoid rechecking each time
    std::map <std::string, std::vector<TH1*> > m_hist_for_binlabel;

};

#endif /* MUONTPCUTFLOWPLOTBASE_H_ */
