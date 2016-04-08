/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPEfficiencyPlotBase.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"
#include "Math/QuantFuncMathCore.h"
#include "TEfficiency.h"

MuonTPEfficiencyPlotBase::MuonTPEfficiencyPlotBase(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
: PlotBase(pParent, sDir),
m_isMatched(isMatched),
m_apply_SF(ApplySF), 
m_doAsymmErrors (false)
{}

void MuonTPEfficiencyPlotBase::fill(Probe& ){
}

void MuonTPEfficiencyPlotBase::EffiDivide(MuonTPEfficiencyPlotBase* trials, MuonTPEfficiencyPlotBase* matches){
    // default: if not defined by the user try a generic approach that should always work
    
    std::vector<HistData> hd_trials = trials->retrieveBookedHistograms();
    std::vector<HistData> hd_matches = matches->retrieveBookedHistograms();
    std::vector<HistData> hd_effi = this->retrieveBookedHistograms();
    std::vector<std::pair <TGraph*,  std::string > > hd_effi_graphs = this->retrieveBookedGraphs();
    
    for (size_t i = 0; i < hd_effi.size();++i){
	EffiDivide(hd_trials[i].first, hd_matches[i].first,hd_effi[i].first);
	if (m_doAsymmErrors && hd_effi_graphs.size()>i && hd_effi_graphs[i].first){
	    TGraphAsymmErrors* hd_effi_graph = dynamic_cast <TGraphAsymmErrors*>(hd_effi_graphs[i].first);
	    if (hd_effi_graph) EffiDivide(hd_trials[i].first, hd_matches[i].first,hd_effi_graph);
	}
    }
}

void MuonTPEfficiencyPlotBase::EffiDivide(TH1* trials, TH1* matches, TH1* eff){
    
    TH1D* trials_1d = dynamic_cast<TH1D*>(trials);
    TH2D* trials_2d = dynamic_cast<TH2D*>(trials);
    if (trials_1d){
	for (int ibin = 0; ibin < trials_1d->GetNbinsX()+2;++ibin){
	    EffiDivide(ibin, trials,matches,eff);
	}
    }
    else if (trials_2d){
	for (int ibin = 0; ibin < (trials_2d->GetNbinsX()+2)*(trials_2d->GetNbinsY()+2);++ibin){
	    EffiDivide(ibin, trials,matches,eff);
	}
    }
}

void MuonTPEfficiencyPlotBase::EffiDivide (TH1* trials, TH1* matches, TGraphAsymmErrors* effi) {
    
    TH1D* trials_1d = dynamic_cast<TH1D*>(trials);
    TH1D* matches_1d = dynamic_cast<TH1D*>(matches);
    if (!trials_1d) return;       
    effi->Divide(matches_1d, trials_1d, "cp");        
}

void MuonTPEfficiencyPlotBase::EffiDivide(int ibin, TH1* trials, TH1* matches, TH1* eff){
    
    double den = trials->GetBinContent(ibin);
    double dden = trials->GetBinError(ibin);
    
    double num = matches->GetBinContent(ibin);
    double dnum = matches->GetBinError(ibin);
    
    effResult res = CalcEff (den, num, dden, dnum);
    
    eff->SetBinContent(ibin,res.first);
    //  Symmetrize the errors for the TH1* case
    eff->SetBinError(ibin,(res.second.first  + res.second.second)/2.); 
}

MuonTPEfficiencyPlotBase::effResult MuonTPEfficiencyPlotBase::CalcEff (double den,  double num,  double ,  double ) {
    
    static double gamma = 1.0 - 0.683;
    static double alfa_lo = gamma/2.0;
    static double alfa_hi = gamma/2.0;
    
    double Eff = 1;
    double dEff_up = 0;
    double dEff_down = 1;
    
    if (den > 0) {
	Eff = num / den;
	
	if ( num > 0 && den-num > 0) {
	    
	    double F_lo = ROOT::Math::fdistribution_quantile(alfa_lo, 2*num, 2*(den-num+1.));
	    double F_hi = ROOT::Math::fdistribution_quantile(1. - alfa_hi, 2*(num+1.), 2*(den-num));
	    
	    dEff_down = Eff - (num * F_lo)/(den - num + 1. + num * F_lo);
	    dEff_up = ((num + 1.) * F_hi)/((den - num) + (num+1.)*F_hi) - Eff;        
	}
    }
    
    return std::make_pair (Eff,  std::make_pair (dEff_up,  dEff_down));
}

std::string MuonTPEfficiencyPlotBase::UpdatedPlotName (const std::string & original) const{
    std::string suff = "";
    if (m_apply_SF) suff = "_AfterSF";
    return (original + suff);
}
TH1* MuonTPEfficiencyPlotBase::Book1D(const std::string & name, const std::string & labels, int nBins, float start, float end, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::Book1D(name2,labels,nBins,start,end,prependDir);

}
TH1* MuonTPEfficiencyPlotBase::Book1D(const std::string & name, TH1* refHist, const std::string & labels, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::Book1D(name2,refHist,labels,prependDir);
}
TH2* MuonTPEfficiencyPlotBase::Book2D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::Book2D(name2,labels,nBinsX,startX,endX,nBinsY,startY,endY,prependDir);
}
TH2* MuonTPEfficiencyPlotBase::Book2D(const std::string & name, TH2* refHist, const std::string & labels, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::Book2D(name2,refHist,labels,prependDir);
}
/// Book a TH3D histogram
TH3* MuonTPEfficiencyPlotBase::Book3D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, int nBinsZ, float startZ, float endZ, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::Book3D(name2,labels,nBinsX,startX,endX,nBinsY,startY,endY,nBinsZ,startZ,endZ,prependDir);
}
/// Book a TH3D histogram using refHist as reference for number of bins and axis range
TH3* MuonTPEfficiencyPlotBase::Book3D(const std::string & name, TH3* refHist, const std::string & labels, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::Book3D(name2,refHist,labels,prependDir);
}

TProfile* MuonTPEfficiencyPlotBase::BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY, float endY, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::BookTProfile(name2,labels,nBinsX,startX,endX,startY,endY,prependDir);
}
TGraphAsymmErrors* MuonTPEfficiencyPlotBase::BookGraphAsymmErrors(const std::string & name,  bool prependDir) {
    const std::string name2 = UpdatedPlotName(name);
    std::string prefix = "";
    if (prependDir) {
        prefix = m_sDirectory;
    }
    TGraphAsymmErrors* graph = new TGraphAsymmErrors();
    graph->SetName((prefix + name2).c_str());
    m_vBookedGraphs.push_back(std::make_pair(dynamic_cast<TGraph*>(graph),m_sDirectory));
    return graph;
}
std::vector<std::pair <TGraph*,  std::string> > MuonTPEfficiencyPlotBase::retrieveBookedGraphs() const{
    return m_vBookedGraphs;
}

void MuonTPEfficiencyPlotBase::BookAllAsymmErrors(void) {
    for (auto hist :  retrieveBookedHistograms()) {
        BookGraphAsymmErrors((std::string(hist.first->GetName())+"_AsymmErrors"), false);
    }
}
