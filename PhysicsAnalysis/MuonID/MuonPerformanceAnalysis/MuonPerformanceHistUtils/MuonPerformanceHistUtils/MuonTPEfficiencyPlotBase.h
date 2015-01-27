/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPEfficiencyPlotBase.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef MUONTPEFFICIENCYPLOTBASE_H_
#define MUONTPEFFICIENCYPLOTBASE_H_

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/Probe.h"

// base class for the Muon Efficiency plots

class MuonTPEfficiencyPlotBase: public PlotBase {
public:
	MuonTPEfficiencyPlotBase();
    MuonTPEfficiencyPlotBase(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF = false);

    virtual void fill(Probe& probe);

    // overload Book... to get the closure test info in
    TH1* Book1D(const std::string & name, const std::string & labels, int nBins, float start, float end, bool prependDir = true);
    TH1* Book1D(const std::string & name, TH1* refHist, const std::string & labels, bool prependDir = true);
    TH2* Book2D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, bool prependDir = true);
    TH2* Book2D(const std::string & name, TH2* refHist, const std::string & labels, bool prependDir = true);
    TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY=-1, float endY=-1, bool prependDir = true);


    // overwrites the contents of this instance with the 'efficiency' computed from the input histos.
    virtual void EffiDivide (MuonTPEfficiencyPlotBase* trials, MuonTPEfficiencyPlotBase* matches);

protected:

    // these are utility methods - should work without changes for most use cases!
    std::string UpdatedPlotName (const std::string & original) const ;
    virtual void EffiDivide (TH1* trials, TH1* matches, TH1* effi);
    virtual void EffiDivide (int ibin, TH1* trials, TH1* matches, TH1* effi);

	bool m_isMatched;
	bool m_apply_SF;

};

#endif /* MUONTPEFFICIENCYPLOTBASE_H_ */
