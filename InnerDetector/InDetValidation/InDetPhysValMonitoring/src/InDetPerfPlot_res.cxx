/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_res.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_res.h"
#include <utility>
#include "TFitResult.h"
#include <iostream>


//anonymous namespace for utility functions at file scope
namespace {
	template<typename T> bool
	inRange(const T & variable,const T & lowerLimit, const T & upperLimit){
		return (variable >= lowerLimit) and (variable <= upperLimit);
	}
	
	 template <typename T> bool
       coerceToRange(T & variable,const T & lowerLimit, const T & upperLimit){
                bool coerced(false);
                if (variable < lowerLimit){ variable = lowerLimit; coerced=true;}
                if (variable > upperLimit){ variable = upperLimit; coerced=true;}
                return coerced;
       }
	
	// Utility routine used by projectStandardProfileY: fill one bin from a 1D slice.
	void SetProfileBin (int i, TH1* h1, TH1* pm, TH1* pw, const char* fitfun=0, double cut_tail=3.0){
	  if (h1->GetXaxis()->TestBit(TAxis::kAxisRange)) {
		// remove range if set previously
		h1->GetXaxis()->SetRange();
		h1->ResetStats();
	  }
	  double x=0.0, xe=0.0, e=0.0, ee=0.0;
	  if (h1->GetEntries() != 0.0) {
		x=  h1->GetMean();      e=  h1->GetRMS();
		xe= h1->GetMeanError(); ee= h1->GetRMSError();
		if (fitfun) {
		  TFitResultPtr r= h1->Fit (fitfun, "QS0");
		  if (r.Get() && r->Status()%1000==0) {
			x=  r->Parameter(1); e=  r->Parameter(2);
			xe= r->ParError (1); ee= r->ParError (2);
		  }
		} else if (cut_tail!=0.0) {
		  double rmin, rmax;
		  if (cut_tail<0.0) { rmin=   cut_tail*e; rmax=  -cut_tail*e; }
		  else              { rmin= x-cut_tail*e; rmax= x+cut_tail*e; }
		  // Work round ROOT 5.34.04 change that would include the under/overflow bins
		  // if the new range extends beyond the histogram edges.
		  if (rmin < h1->GetBinLowEdge(1))              rmin= h1->GetBinLowEdge(1);
		  if (rmax > h1->GetBinCenter(h1->GetNbinsX())) rmax= h1->GetBinCenter(h1->GetNbinsX());
		  h1->SetAxisRange (rmin, rmax);
		  x=  h1->GetMean();      e=  h1->GetRMS();
		  xe= h1->GetMeanError(); ee= h1->GetRMSError();
		}
	  }
	  if (pm) {
		pm->SetBinContent (i, x);
		pm->SetBinError   (i, xe);
	  }
	  if (pw) {
		pw->SetBinContent (i, e);
		pw->SetBinError   (i, ee);
	  }
	}
	
	void projectStandardProfileY (const std::vector<TH1*>& hvec, TH1* profMean, TH1* profWidth=0, int fold=0, const char* fitfun=0, double cut_tail=3.0)
	{
	  TH1 *hx= profMean ? profMean : profWidth;
	  if (!hx) return;
	  int nvec= hvec.size();
	  if (fold>nvec) fold= nvec;
	  int nb= fold>0 ? fold : nvec;
	  double xlo= hx->GetXaxis()->GetXmin(), xhi= hx->GetXaxis()->GetXmax();
	  if (profMean)  profMean ->SetBins (nb, xlo, xhi);
	  if (profWidth) profWidth->SetBins (nb, xlo, xhi);
	  double nent= 0.0;
	  for (int i=0; i<nb; i++) {
		int j1=i, j2=-1;
		if (fold>0) {
		  j1= fold+i;
		  if (j1<nvec && hvec[j1]) j2= fold-i-1;
		  else                     j1= fold-i-1;
		}
		if (!(j1>=0 && hvec[j1])) continue;
		TH1* h1= dynamic_cast<TH1*>(hvec[j1]->Clone(Form("%s_mod",hvec[j1]->GetName())));
		if (!h1) continue;
		if (  j2>=0 && hvec[j2]) h1->Add (hvec[j2]);
		SetProfileBin (i+1, h1, profMean, profWidth, fitfun, cut_tail);
		nent += h1->GetEntries();
		delete h1;
	  }
	  // restore #entries, which was modified by SetBinContent in SetProfileBin
	  if (profMean)  profMean ->SetEntries(nent);
	  if (profWidth) profWidth->SetEntries(nent);
	}
	
	
}


InDetPerfPlot_res::InDetPerfPlot_res(PlotBase* pParent, const std::string & sDir)
   :PlotBase(pParent, sDir), 
   m_trackEtaBins(20),
   m_resPlots(NPARAMS, std::vector<TH1*>(m_trackEtaBins)),
   m_paramNames{"d0","z0", "phi", "theta","z0st","qOverP"},
   m_projectionNames{"mean", "res"}
   {
//
}


void 
InDetPerfPlot_res::initializePlots() {
	//never prepend the directory path to the histo variable names
	const bool prependDirectory(false);
	//Initialize the projections which are the final result:

	const float lowerEtaLimit(-2.5),upperEtaLimit(2.5);
	for (unsigned int param(0);param!=NPARAMS;++param){
		std::string hName0 = formProjectionName(MEAN, param);
		std::string hTitle0 = formProjectionTitle(MEAN, param);
		std::string hName1 = formProjectionName(RES, param);
		std::string hTitle1 = formProjectionTitle(RES, param);
		TH1 * pmeanHisto = Book1D(hName0,hTitle0,m_trackEtaBins, lowerEtaLimit, upperEtaLimit, prependDirectory);
		TH1 * presHisto = Book1D(hName1,hTitle1,m_trackEtaBins, lowerEtaLimit, upperEtaLimit, prependDirectory);
		m_projectedPlots.push_back(std::make_pair(pmeanHisto,presHisto));
	}
	//Initialize the data source histograms which will be used: 
    //number of bins for each parameter
    //upper and lower histogram limits, from existing RTT code
	typedef std::pair<float, float> Limits_t;
	unsigned int nBins[NPARAMS]={2000,200,3000, 2000, 200, 200};
	Limits_t limits[NPARAMS];
	limits[D0] = Limits_t(-1.0,1.0);
	limits[Z0] = Limits_t(-1.5,1.5);
	limits[PHI] = Limits_t(-0.03,0.03);
	limits[THETA] = Limits_t(-0.1,0.1);
	limits[Z0SIN_THETA] = Limits_t(-1.5,1.5);
	limits[QOVER_PT] = Limits_t(-0.4,0.4);
	//
	for (unsigned int i(0);i!=m_trackEtaBins;++i) {
		for (unsigned int var(0);var!=NPARAMS;++var){
			std::string hName = formName(var,i);
			TH1 * psourceHisto = Book1D(hName,hName,nBins[var], limits[var].first, limits[var].second, prependDirectory);
			//we already initialised the vector in the initialiser list, so don't push_back
    		m_resPlots[var][i]=psourceHisto;
    	}
    }
}

void 
InDetPerfPlot_res::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
	const int lowerEtaLimit(0),upperEtaLimit(19);
	//
	float m_track_truth_eta = truthprt.eta();
	int ieta = int(m_track_truth_eta*4.0 + 10.0);
	// the following happens quite often; its annoying to see these messages (TODO: use msgSvc)
	if (not inRange(ieta,lowerEtaLimit,upperEtaLimit)) {
		//std::cout << "Res plot: ieta is out of range, value="<<ieta <<std::endl;
		return;
	}
	
	//get the track particle parameters needed into a simple array
	float trkParticleParams[NPARAMS];
	trkParticleParams[D0] = trkprt.d0();
	trkParticleParams[Z0] = trkprt.z0();
	trkParticleParams[PHI] = trkprt.phi0();
	trkParticleParams[THETA] = trkprt.theta();
	//checked the following, at line 2519 of IDStandardPerformance.cxx
	// https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetValidation/InDetPerformanceRTT/tags/InDetPerformanceRTT-01-00-05/src
	trkParticleParams[Z0SIN_THETA] = trkprt.z0() * std::sin(trkprt.theta()); //! CHECK!
	trkParticleParams[QOVER_PT] = trkprt.qOverP();
	//
	//const float unfilledValue(0.0); //should be NaN?
	for (unsigned int var(0);var!=NPARAMS;++var){
		const std::string & varName = m_paramNames[var];
		const float trackParameter = trkParticleParams[var];
		const bool truthIsAvailable = truthprt.isAvailable<float>(varName);
		// get the corresponding truth variable, dont fill if doesn't exist
		if (truthIsAvailable and m_resPlots[var][ieta]){
		  const float truthParameter = (truthprt.auxdata< float >(varName));
			(m_resPlots[var][ieta])->Fill( trackParameter - truthParameter);
		}
	}
}

void InDetPerfPlot_res::finalizePlots(){
		int fold= 0;
		projectStandardProfileY (m_resPlots[D0],    m_projectedPlots[D0].first,    m_projectedPlots[D0].second,    fold, 0, -3.0);
		projectStandardProfileY (m_resPlots[Z0],    m_projectedPlots[Z0].first,    m_projectedPlots[Z0].second,    fold, 0, -3.0);
		projectStandardProfileY (m_resPlots[Z0SIN_THETA],  m_projectedPlots[Z0SIN_THETA].first,  m_projectedPlots[Z0SIN_THETA].second,  fold, 0, -3.0);
		projectStandardProfileY (m_resPlots[PHI],  m_projectedPlots[PHI].first,    m_projectedPlots[PHI].second,   fold, 0, -3.0);
		projectStandardProfileY (m_resPlots[QOVER_PT],  m_projectedPlots[QOVER_PT].first,    m_projectedPlots[QOVER_PT].second,  fold, 0, -3.0);
		projectStandardProfileY (m_resPlots[THETA], m_projectedPlots[THETA].first,    m_projectedPlots[THETA].second, fold, 0, -3.0);
}

std::string
InDetPerfPlot_res::formName(const unsigned int p, const unsigned int eta) const {
	return m_paramNames[p] + std::string("res_bin")+std::to_string(eta);
}

std::string
InDetPerfPlot_res::formProjectionName(const unsigned int projection, const unsigned int param) const {
	return m_projectionNames[projection] + std::string("_vs_eta_")+m_paramNames[param];
}

std::string
InDetPerfPlot_res::formProjectionTitle(const unsigned int projection, const unsigned int param) const {
	//should use formatting/regex, but it's late and I'll get it wrong
	static std::string titleComponents[NPARAMS][2] = {
		{"d_{0}", "d^{rec}_{0}-d^{tru}_{0}"},
		{"z_{0}", "z^{rec}_{0}-z^{tru}_{0}"},
		{"#phi", "#phi^{rec}-#phi^{tru}"},
		{"#theta","#theta^{rec}-#theta^{tru}"},
		{"z_{0}*sin(#theta)", "z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru}"},
		{"q/p_{T}", "q/p^{rec}-q/p^{tru}"}
	};
	static std::string projectionTitles[2] = { "Track Measurement Bias: ","Track Resolution: "};
	static std::string sigma[2] = {"#sigma(",""};
	static std::string closure[2] = {"",")"};
	return projectionTitles[projection] + titleComponents[param][0] + std::string("versus #eta;#eta;") + sigma[projection] + titleComponents[param][1]+ closure[projection];
}
