/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_res.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_res.h"
#include "xAODTracking/TrackingPrimitives.h"
#include <vector>
#include <iostream>

using namespace TMath;

using std::string;
using std::cout;
using std::endl;
using std::sqrt;
using std::pair;
using std::make_pair;

InDetPerfPlot_res::InDetPerfPlot_res(InDetPlotBase* pParent, const std::string & sDir)  :InDetPlotBase(pParent, sDir), 
   m_trackEtaBins(20),
   m_etaMin(-2.5),
   m_etaMax(2.5),
   m_PtBins(20),
   m_PtMin(400),
   m_PtMax(10000),
   log_mode("ON"),
   m_meanPlots(NPARAMS),
   m_mean_vs_ptPlots(NPARAMS),
   m_resoPlots(NPARAMS),
   m_resptPlots(NPARAMS),
   m_pullPlots(NPARAMS),
   m_pullbasePlots(NPARAMS),
   m_pullwidthPlots(NPARAMS),
   m_paramNames{{"d0",1.0}, {"z0",1.5},{"phi",0.03}, {"theta",0.1},{"z0*sin(theta)",1.5},{"qOverP",0.4}}
   {
//
}


void InDetPerfPlot_res::initializePlots() {
  const bool dontPrependDirectory(false); //What is this?  Why is it here?
  //The original Pull script has 200 bins instead of only 20, is that important?
  //Should the pull_widerange plots be kept as well?  Or are they superfluous?
  typedef std::pair<float, float> Limits_t;
  Limits_t allLimits = Limits_t(-5.0, 5.0);
  int nPullBins(200); //number of bins specifically for the pull plots

  double m_Pt_logmin = Log10(m_PtMin);
  double m_Pt_logmax = Log10(m_PtMax);

  for (unsigned int var(0); var != NPARAMS; ++var){
    std::string mName = formName(var, "measure");
    std::string mTitle = formTitle(var, "measure");
    std::string rName = formName(var,"reseta");
    std::string rTitle = formTitle(var, "reseta");    
    std::string mptName = formName(var, "meas_pt");
    std::string mptTitle = formTitle(var, "meas_pt");
    std::string rptName = formName(var, "respt");
    std::string rptTitle = formTitle(var, "respt");
    std::string pName = formName(var, "pull");
    std::string pTitle = formTitle(var, "pull");
    std::string pbName = formName(var, "pullbase");
    std::string pbTitle = formTitle(var, "pullbase");
    std::string pwName = formName(var, "width");
    std::string pwTitle = formTitle(var, "width");
    
    TProfile * meanHisto = BookTProfile(mName, mTitle, m_trackEtaBins, m_etaMin, m_etaMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, dontPrependDirectory);
    m_meanPlots[var] = meanHisto;

    TProfile * rHisto = BookTProfile(rName, rTitle, m_trackEtaBins, m_etaMin, m_etaMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, dontPrependDirectory);
    m_resoPlots[var] = rHisto; 

    if(log_mode == "ON"){
      TProfile * meanPtHisto = BookTProfile(mptName, mptTitle, m_PtBins, m_Pt_logmin, m_Pt_logmax, -m_paramNames.at(var).second, m_paramNames.at(var).second, dontPrependDirectory);
      m_mean_vs_ptPlots[var] = meanPtHisto;

      TProfile * rptHisto = BookTProfile(rptName, rptTitle, m_PtBins, m_Pt_logmin, m_Pt_logmax, -m_paramNames.at(var).second, m_paramNames.at(var).second, dontPrependDirectory);
      m_resptPlots[var] = rptHisto;

    }else if(log_mode == "OFF"){
      TProfile * meanPtHisto = BookTProfile(mptName, mptTitle, m_PtBins, m_PtMin, m_PtMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, dontPrependDirectory);
      m_mean_vs_ptPlots[var] = meanPtHisto;
      
      TProfile * rptHisto = BookTProfile(rptName, rptTitle, m_PtBins, m_PtMin, m_PtMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, dontPrependDirectory);
      m_resptPlots[var] = rptHisto;
    }
  
    TH1 * pHisto = Book1D(pName, pTitle, nPullBins, allLimits.first, allLimits.second, dontPrependDirectory);
    m_pullPlots[var] = pHisto;
    
    TProfile * pbHisto = BookTProfile(pbName, pbTitle, m_trackEtaBins, m_etaMin, m_etaMax, allLimits.first, allLimits.second, dontPrependDirectory);
    m_pullbasePlots[var] = pbHisto;
    
    TProfile * pwHisto = BookTProfile(pwName, pwTitle, m_trackEtaBins, m_etaMin, m_etaMax, 0, 1, dontPrependDirectory);
    m_pullwidthPlots[var] = pwHisto;    
  }
}

void InDetPerfPlot_res::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  float eta = trkprt.eta();
  float pt = trkprt.pt();

  double log_pt = Log10(trkprt.pt());

  float trkParticleParams[NPARAMS];
  trkParticleParams[D0] = trkprt.d0();
  trkParticleParams[Z0] = trkprt.z0();
  trkParticleParams[PHI] = trkprt.phi0();
  trkParticleParams[THETA] = trkprt.theta();
  trkParticleParams[Z0SIN_THETA] = trkprt.z0() * std::sin(trkprt.theta()); //This should be it
  trkParticleParams[QOVERP] = trkprt.qOverP();
   
  for (unsigned int var(0);var!=NPARAMS;++var){
    const string  varName = m_paramNames.at(var).first;
    const string  errName = varName + std::string("err");
    float trackParameter = trkParticleParams[var];                      //needed for all of them
    bool truthIsAvailable = truthprt.isAvailable<float>(varName);
    bool sigmaIsAvailable = trkprt.isAvailable<float>(errName);

    if(var == Z0SIN_THETA){
      float truthParameter = truthprt.auxdata< float >("z0") * std::sin(truthprt.auxdata< float >("theta"));
      (m_meanPlots[var])->Fill(eta, (trackParameter - truthParameter));
      if(log_mode == "ON"){
	(m_mean_vs_ptPlots[var])->Fill(log_pt, (trackParameter - truthParameter));
      }else if(log_mode == "OFF"){
	(m_mean_vs_ptPlots[var])->Fill(pt, (trackParameter - truthParameter));
      }
    }
    if (truthIsAvailable){   //get the corresponding truth variable, only Fill if it exists
      float truthParameter = (truthprt.auxdata< float >(varName));
      if(m_meanPlots[var]){
	if(var == QOVERP){
	  (m_meanPlots[var])->Fill(eta, 1.0 - (truthParameter/trackParameter));
	  if(log_mode == "ON"){
	    (m_mean_vs_ptPlots[var])->Fill(log_pt, (trackParameter - truthParameter));
	  }else if(log_mode == "OFF"){
	    (m_mean_vs_ptPlots[var])->Fill(pt, 1.0 - (truthParameter/trackParameter));
	  }
	}else{
	  (m_meanPlots[var])->Fill(eta, (trackParameter - truthParameter));
	  if(log_mode == "ON"){
	    (m_mean_vs_ptPlots[var])->Fill(log_pt, (trackParameter - truthParameter));
	  }else if(log_mode == "OFF"){
	    (m_mean_vs_ptPlots[var])->Fill(pt, (truthParameter - trackParameter));
	  }
	}
      }
      if(sigmaIsAvailable){
	float sigma = (trkprt.auxdata< float >(errName));
	if(m_pullPlots[var]){
	 (m_pullPlots[var])->Fill( (trackParameter - truthParameter) / sigma);
	 (m_pullbasePlots[var])->Fill(eta, (trackParameter - truthParameter)/sigma);
	}
      }
    }
  }
  
}


void InDetPerfPlot_res::finalizePlots(){
  double m_Pt_logmax = Log10(m_PtMax);
  double m_Pt_logmin = Log10(m_PtMin);

  for (unsigned int var(0);var != NPARAMS; ++var){
    for (unsigned int i=0; i!=m_trackEtaBins; i++){
      double step = (m_etaMax - m_etaMin)/m_trackEtaBins;
      double initial = m_meanPlots[var]->GetBinError(i);
      double sqrt_num_entries = sqrt(m_meanPlots[var]->GetBinEntries(i));
      double RMS = initial * sqrt_num_entries;
      (m_resoPlots[var])->Fill((m_etaMin + i*step), RMS);
      if(m_pullPlots[var]){
	double width = m_pullbasePlots[var]->GetBinError(i);
	(m_pullwidthPlots[var])->Fill((m_etaMin + i*step), width);
      }
    }

    for (unsigned int i=0; i!=m_PtBins; i++){
      if(log_mode == "ON"){
	double incr = (m_Pt_logmax - m_Pt_logmin)/m_PtBins;
	double initial = m_mean_vs_ptPlots[var]->GetBinError(i);
	double sqrt_num_entries = sqrt(m_mean_vs_ptPlots[var]->GetBinEntries(i));
	double sigma = initial * sqrt_num_entries; 
	(m_resptPlots[var])->Fill((m_Pt_logmin + i*incr), sigma);
      }else if(log_mode == "OFF"){
	double incr = (m_PtMax - m_PtMin)/m_PtBins;  
	double initial = m_mean_vs_ptPlots[var]->GetBinError(i);
	double sqrt_num_entries = sqrt(m_mean_vs_ptPlots[var]->GetBinEntries(i));
	double sigma = initial * sqrt_num_entries;
	(m_resptPlots[var])->Fill((m_PtMin + i*incr), sigma);
      }
    }
  }  
}


std::string
InDetPerfPlot_res::formName(const unsigned int p, std::string type) const{
  if(type == "measure"){
    return std::string("Track Measurement Bias: ")+m_paramNames.at(p).first +std::string(" vs eta");
  }else if((type == "meas_pt") and (log_mode == "OFF")){
    return std::string("Track Measurement Bias: ")+m_paramNames.at(p).first +std::string(" vs pt");
  }else if((type == "meas_pt") and (log_mode == "ON")){
    return std::string("Track Measurement Bias: ")+m_paramNames.at(p).first +std::string(" vs log_10(pt)");
  }else if(type == "reseta"){
    return std::string("Track Resolution: ")+m_paramNames.at(p).first +std::string(" vs eta");
  }else if((type == "respt") and (log_mode == "OFF")){
    return std::string("Track Resolution: ")+m_paramNames.at(p).first +std::string(" vs pt");
  }else if((type == "respt") and (log_mode == "ON")){
    return std::string("Track Resolution: ")+m_paramNames.at(p).first +std::string(" vs log_10(pt)");
  }else if(type == "pull"){
    return std::string("Pull Distribution of ")+m_paramNames.at(p).first;
  }else if(type == "pullbase"){
    return std::string("Pull Distribution of ")+m_paramNames.at(p).first +std::string(" vs eta");
  }else if(type == "width"){
    return std::string("Pull Width of ")+m_paramNames.at(p).first +std::string(" vs eta");
  }
  return "";
}


std::string
InDetPerfPlot_res::formTitle(const unsigned int param, std::string type) const{

  static std::string titlePrefix("");
  static std::string titleLeft[NPARAMS] = {"d_{0}: ", "z_{0}: ", "#phi: ", "#theta:", "z_{0}*sin(#theta): ", "q/p: "};
  static std::string titleRight[NPARAMS] = {"","","","","",""}; 

  if((type == "measure") || (type == "meas_pt")){
    static std::string titleRight[NPARAMS] = {
      "(d^{rec}_{0} - d^{tru}_{0})",
      "(z^{rec}_{0} - z^{tru}_{0})",
      "(#phi^{rec} - #phi^{tru})",
      "(#theta^{rec} - #theta^{tru})",
      "(z_{0}*sin(#theta)^{rec} - z_{0}*sin(#theta)^{tru})",
      "1  - ((q/p)^{rec}/(q/p)^{tru})",
    };
    static std::string titlePrefix("Track Measurement Bias of ");
  }else if((type == "reseta") || (type == "respt")){
    static std::string titleRight[NPARAMS] = {
      "#sigma(d^{rec}_{0} - d^{tru}_{0})",
      "#sigma(z^{rec}_{0} - z^{tru}_{0})",
      "#sigma(#phi^{rec} - #phi^{tru})",
      "#sigma(#theta^{rec} - #theta^{tru})",
      "#sigma(z_{0}*sin(#theta)^{rec} - z_{0}*sin(#theta)^{tru})",
      "(1 - (q/p)^{tru}/(q/p)^{rec})",
    };
    static std::string titlePrefix("Track Resolution of ");
  }else if((type == "pull") || (type == "width") || (type == "pullbase")){
    static std::string titleRight[NPARAMS] = {
      "(d^{rec}_{0} - d^{tru}_{0})/#sigma_{d_{0}}",
      "(z^{rec}_{0} - z^{tru}_{0})/#sigma_{z_{0}}",
      "(#phi^{rec} - #phi^{tru})/#sigma_{#phi}",
      "(#theta^{rec} - #theta^{tru})/#sigma_{#theta}",
      "useless junk",
     "((q/p)^{rec} - (q/p)^{tru})/#sigma_{q/p}"
    };
    static std::string titlePrefix("Pull Distribution of ");
  }
  return titlePrefix + titleLeft[param] + titleRight[param];
}
