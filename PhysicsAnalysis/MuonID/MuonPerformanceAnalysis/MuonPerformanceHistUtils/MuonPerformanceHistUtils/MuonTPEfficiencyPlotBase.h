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
#include "TGraphAsymmErrors.h"
#include "TH3.h"

// base class for the Muon Efficiency plots

class MuonTPEfficiencyPlotBase: public PlotBase {
public:
    
    ///  use this to store the efficiency result for a single calculation. Storage scheme: < Eff,  < Err_up,  Err_down> >
    typedef std::pair < double,  std::pair <double,  double> > effResult; 
    
	MuonTPEfficiencyPlotBase();
    MuonTPEfficiencyPlotBase(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF = false);

    virtual void fill(Probe& probe);

    // overload Book... to get the closure test info in
    TH1* Book1D(const std::string & name, const std::string & labels, int nBins, float start, float end, bool prependDir = true);
    TH1* Book1D(const std::string & name, TH1* refHist, const std::string & labels, bool prependDir = true);
    TH2* Book2D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, bool prependDir = true);
    TH2* Book2D(const std::string & name, TH2* refHist, const std::string & labels, bool prependDir = true);
    TH3* Book3D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, int nBinsZ, float startZ, float endZ, bool prependDir = true);
    TH3* Book3D(const std::string & name, TH3* refHist, const std::string & labels, bool prependDir = true);
    TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY=-1, float endY=-1, bool prependDir = true);

    ///  Add some methods not provided by the TrkValHistUtils package - TGraphAsymmErrors for efficiencies
    TGraphAsymmErrors* BookGraphAsymmErrors(const std::string & name,  bool prependDir = true);

    // overwrites the contents of this instance with the 'efficiency' computed from the input histos.
    virtual void EffiDivide (MuonTPEfficiencyPlotBase* trials, MuonTPEfficiencyPlotBase* matches);
    ///Retrieve all booked graphs
    std::vector<std::pair <TGraph*,  std::string> > retrieveBookedGraphs() const;
    
    ///  create a TGraphAsymmErrors equivalent to every defined plot
    void BookAllAsymmErrors(void);
    
    void SetDoAsymmErrors (bool in) { m_doAsymmErrors = in;}

protected:

    // these are utility methods - should work without changes for most use cases!
    std::string UpdatedPlotName (const std::string & original) const ;
    virtual void EffiDivide (TH1* trials, TH1* matches, TH1* effi);
    virtual void EffiDivide (TH1* trials, TH1* matches, TGraphAsymmErrors* effi);
    virtual void EffiDivide (int ibin, TH1* trials, TH1* matches, TH1* effi);
    effResult CalcEff (double trials,  double matches,  double err_trials,  double err_matches);

    std::vector<std::pair <TGraph*,  std::string> >  m_vBookedGraphs;
    bool m_isMatched;
    bool m_apply_SF;
    bool m_doAsymmErrors;

};

#endif /* MUONTPEFFICIENCYPLOTBASE_H_ */
