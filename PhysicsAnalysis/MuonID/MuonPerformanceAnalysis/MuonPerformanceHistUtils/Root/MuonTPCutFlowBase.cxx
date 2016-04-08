/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPCutFlowPlotBase.cxx
 *
 *  Created on: Nov 11, 2014
 *      Author: goblirsc
 */

#include "../MuonPerformanceHistUtils/MuonTPCutFlowBase.h"


MuonTPCutFlowBase::MuonTPCutFlowBase(PlotBase* pParent, std::string sDir)
      : PlotBase(pParent, sDir)
    {}

void MuonTPCutFlowBase::fill(std::string bin , double w){
    auto tofill = m_hist_for_binlabel.find(bin);
    if (tofill != m_hist_for_binlabel.end()){
        for (auto hist: tofill->second){
            hist->Fill(bin.c_str(),w);
        }
    }
}

TH1* MuonTPCutFlowBase::AddCutFlow (std::string name, std::vector<std::string> stages){

    TH1D hist = TH1D(name.c_str(),name.c_str(),stages.size(),0.5,stages.size()+0.5);
    for (size_t i = 1; i < stages.size()+1; ++i){
        hist.GetXaxis()->SetBinLabel(i, stages[i-1].c_str());
    }
    TH1* out = Book1D(name,&hist,"Cut Flow; Cut Flow Stage; a.u.");
    for (auto stage:stages){
        if (m_hist_for_binlabel.find(stage) != m_hist_for_binlabel.end()){
            m_hist_for_binlabel[stage] = std::vector<TH1*>(0);
        }
        m_hist_for_binlabel[stage].push_back(out);
    }
    return out;
}
