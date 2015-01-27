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

MuonTPEfficiencyPlotBase::MuonTPEfficiencyPlotBase(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF)
	  : PlotBase(pParent, sDir),
	    m_isMatched(isMatched),
	    m_apply_SF(ApplySF)
	{}
void MuonTPEfficiencyPlotBase::fill(Probe& ){

}

void MuonTPEfficiencyPlotBase::EffiDivide(MuonTPEfficiencyPlotBase* trials, MuonTPEfficiencyPlotBase* matches){
	// default: if not defined by the user try a generic approach that should always work

	std::vector<HistData> hd_trials = trials->retrieveBookedHistograms();
	std::vector<HistData> hd_matches = matches->retrieveBookedHistograms();
	std::vector<HistData> hd_effi = this->retrieveBookedHistograms();

	for (size_t i = 0; i < hd_effi.size();++i){
		EffiDivide(hd_trials[i].first, hd_matches[i].first,hd_effi[i].first);
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

	// do nothing in case of TProfiles
}

void MuonTPEfficiencyPlotBase::EffiDivide(int ibin, TH1* trials, TH1* matches, TH1* eff){

	double den = trials->GetBinContent(ibin);
	double dden = trials->GetBinError(ibin);

	double num = matches->GetBinContent(ibin);
	double dnum = matches->GetBinError(ibin);

	double misses = den - num;
	double dmisses = sqrt(dden*dden-dnum*dnum);

	double effi = den != 0 ? num/den : 0;
	double deffi = den !=0 ? sqrt((dnum*dnum*misses*misses+dmisses*dmisses*num*num)/(den*den*den*den)) : 1;

	eff->SetBinContent(ibin,effi);
    eff->SetBinError(ibin,deffi);
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
TProfile* MuonTPEfficiencyPlotBase::BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY, float endY, bool prependDir){

    const std::string name2 = UpdatedPlotName(name);
    return PlotBase::BookTProfile(name2,labels,nBinsX,startX,endX,startY,endY,prependDir);
}
