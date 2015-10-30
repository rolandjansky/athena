/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_res.cxx
 * @author Max Baugh
**/

#include "InDetPerfPlot_res.h"
#include "xAODTracking/TrackingPrimitives.h"
#include <vector>

using namespace TMath;

using std::string;
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
   m_meanbasePlots(NPARAMS),
   m_meanPlots(NPARAMS),
   m_mean_vs_ptbasePlots(NPARAMS),
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
  const bool PrependDirectory(false); //What is this?  Why is it here?
  //The original Pull script has 200 bins instead of only 20, is that important?
  //Should the pull_widerange plots be kept as well?  Or are they superfluous?
  typedef std::pair<float, float> Limits_t;
  Limits_t allLimits = Limits_t(-5.0, 5.0);
  int nPullBins(200); //number of bins specifically for the pull plots
  int m_yBins(20);  //number of y bins for the TH2 plots

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
    
    TH2 * meanbaseHisto = Book2D(mName, mTitle, m_trackEtaBins, m_etaMin, m_etaMax, m_yBins, -m_paramNames.at(var).second, m_paramNames.at(var).second);
    m_meanbasePlots[var] = meanbaseHisto;

    TProfile * meanHisto = BookTProfile(mName, mTitle, m_trackEtaBins, m_etaMin, m_etaMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
    m_meanPlots[var] = meanHisto;

    TProfile * rHisto = BookTProfile(rName, rTitle, m_trackEtaBins, m_etaMin, m_etaMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
    m_resoPlots[var] = rHisto;

    if(log_mode == "ON"){
      TH2 * meanPtbaseHisto = Book2D(mptName, mptTitle, m_PtBins, m_Pt_logmin, m_Pt_logmax, m_yBins, -m_paramNames.at(var).second, m_paramNames.at(var).second);
      TProfile * meanPt = BookTProfile(mptName, mptTitle, m_PtBins, m_Pt_logmin, m_Pt_logmax, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
      TProfile * rptHisto = BookTProfile(rptName, rptTitle, m_PtBins, m_Pt_logmin, m_Pt_logmax, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
      m_mean_vs_ptbasePlots[var] = meanPtbaseHisto;
      m_mean_vs_ptPlots[var] = meanPt;
      m_resptPlots[var] = rptHisto;
    }else if(log_mode == "OFF"){
      TH2 * meanPtbaseHisto = Book2D(mptName, mptTitle, m_PtBins, m_PtMin, m_PtMax, m_yBins, -m_paramNames.at(var).second, m_paramNames.at(var).second);
      TProfile * meanPt = BookTProfile(mptName, mptTitle, m_PtBins, m_PtMin, m_PtMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
      TProfile * rptHisto = BookTProfile(rptName, rptTitle, m_PtBins, m_PtMin, m_PtMax, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
      m_mean_vs_ptPlots[var] = meanPt;
      m_mean_vs_ptbasePlots[var] = meanPtbaseHisto;
      m_resptPlots[var] = rptHisto;
    }
  
    TH1 * pHisto = Book1D(pName, pTitle, nPullBins, allLimits.first, allLimits.second, PrependDirectory);
    m_pullPlots[var] = pHisto;
    
    TH2 * pbHisto = Book2D(pbName, pbTitle, m_trackEtaBins, m_etaMin, m_etaMax, m_yBins, allLimits.first, allLimits.second);
    //TProfile * pbHisto = BookTProfile(pbName, pbTitle, m_trackEtaBins, m_etaMin, m_etaMax, allLimits.first, allLimits.second, PrependDirectory);
    m_pullbasePlots[var] = pbHisto;
    
    TProfile * pwHisto = BookTProfile(pwName, pwTitle, m_trackEtaBins, m_etaMin, m_etaMax, 0, 3, PrependDirectory);
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
      (m_meanbasePlots[var])->Fill(eta, (trackParameter - truthParameter));
      if(log_mode == "ON"){
  (m_mean_vs_ptbasePlots[var])->Fill(log_pt, (trackParameter - truthParameter));
      }else if(log_mode == "OFF"){
  (m_mean_vs_ptbasePlots[var])->Fill(pt, (trackParameter - truthParameter));
      }
    }
    if (truthIsAvailable){   //get the corresponding truth variable, only Fill if it exists
      float truthParameter = (truthprt.auxdata< float >(varName));
      if(m_meanbasePlots[var]){
  if(var == QOVERP){
    (m_meanbasePlots[var])->Fill(eta, 1.0 - (truthParameter/trackParameter));
    if(log_mode == "ON"){
      (m_mean_vs_ptbasePlots[var])->Fill(log_pt, (trackParameter - truthParameter));
    }else if(log_mode == "OFF"){
      (m_mean_vs_ptbasePlots[var])->Fill(pt, 1.0 - (truthParameter/trackParameter));
    }
  }else{
    (m_meanbasePlots[var])->Fill(eta, (trackParameter - truthParameter));
    if(log_mode == "ON"){
      (m_mean_vs_ptbasePlots[var])->Fill(log_pt, (trackParameter - truthParameter));
    }else if(log_mode == "OFF"){
      (m_mean_vs_ptbasePlots[var])->Fill(pt, (trackParameter - truthParameter));
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
  string width_method = "gaussian";      //switch for changing between the iterative convergence & gaussian fit strategies
  double m_Pt_logmax = Log10(m_PtMax);
  double m_Pt_logmin = Log10(m_PtMin);

  double eta_step = (m_etaMax - m_etaMin)/m_trackEtaBins;
  double pt_step = (m_PtMax - m_PtMin)/m_PtBins;
  double logpt_step = (m_Pt_logmax - m_Pt_logmin)/m_PtBins;

  for (unsigned int var(0);var != NPARAMS; ++var){
    double aymin = -m_paramNames.at(var).second;  //minimum possible value for this track parameter
    double aymax = m_paramNames.at(var).second;   //maximum possible value for this track parameter
    for(unsigned int j=1; j<=m_trackEtaBins; j++){
      //Create dummy histo w/ content from TH2 in relevant eta bin
      TH1D * temp = m_meanbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", j), j, j);
      TH1D * temp_pull = m_pullbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Pull_Histo", j), j, j);
      double mean(0);
      double mean_error(0);
      double RMS_error(0);
      if( width_method == "iterative_convergence" ){
        double temp_RMS = 0;
        double new_RMS = 10;
        //int loop_count = 0;                       //useful for making sure the while-loop doesn't get out of hand
        while(fabs(new_RMS - temp_RMS) > 0.01){
          temp_RMS = temp->GetRMS();
          aymin = (temp->GetMean()) - 5*temp_RMS;
          aymax = (temp->GetMean()) + 5*temp_RMS;
          temp->SetAxisRange(aymin, aymax);
          mean = temp->GetMean();
          new_RMS = temp->GetRMS();
        }
        mean_error = temp->GetMeanError();       
        RMS_error = temp->GetRMSError();         //Is this the proper error here, or should this be scaled by sqrt(number of entries)?
        (m_meanPlots[var])->Fill((m_etaMin + (j-1)*eta_step), mean);
        (m_meanPlots[var])->SetBinError(j, mean_error);
        (m_resoPlots[var])->Fill((m_etaMin + (j-1)*eta_step), new_RMS);
        (m_resoPlots[var])->SetBinError(j, RMS_error);
        double temp_width(0);
        double new_width(10);
        while(fabs(new_width - temp_width) > 0.01){                                 
	  temp_width = temp_pull->GetRMS();
	  double pullmin = (temp_pull->GetMean()) - 5*temp_width;
	  double pullmax = (temp_pull->GetMean()) + 5*temp_width;
	  temp_pull->SetAxisRange(pullmin, pullmax);
	  new_width = temp_pull->GetRMS();
        }
        (m_pullwidthPlots[var])->Fill((m_etaMin + (j-1)*eta_step), new_width);
        double content = m_pullwidthPlots[var]->GetBinContent(j);
        ATH_MSG_DEBUG("Pull Width: "<<new_width<<"  Histo Content: "<<content);
      }else if( width_method == "gaussian" ){
	double sigma(0);
	if(temp->GetEntries() != 0){
	  mean = temp->Fit("gaus", "QS0")->Parameter(1);
	  mean_error = temp->GetMeanError();
	  sigma = temp->Fit("gaus", "QS0")->Parameter(2);
	  RMS_error = temp->GetRMSError();
	  (m_meanPlots[var])->Fill((m_etaMin + (j-1)*eta_step), mean);
	  (m_meanPlots[var])->SetBinError(j, mean_error);
	  (m_resoPlots[var])->Fill((m_etaMin + (j-1)*eta_step), sigma);
	  (m_resoPlots[var])->SetBinError(j, RMS_error);
	}else{
	  (m_meanPlots[var])->Fill((m_etaMin + (j-1)*eta_step), mean);
	  (m_resoPlots[var])->Fill((m_etaMin + (j-1)*eta_step), sigma);
	}
	double width(0);
	if(temp_pull->GetEntries() != 0){
	  width = temp_pull->Fit("gaus", "QS0")->Parameter(2);
	  (m_pullwidthPlots[var])->Fill((m_etaMin + (j-1)*eta_step), width);
	}else{
	  (m_pullwidthPlots[var])->Fill((m_etaMin + (j-1)*eta_step), width);
	}
      }
    }
    for (unsigned int i=1; i<=m_PtBins; i++){
      TH1D * temp = m_mean_vs_ptbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", i), i , i);
      double mean(0);
      double mean_error(0);
      double RMS_error(0);
      if(width_method == "iterative_convergence"){
	double temp_RMS = 0;
	double new_RMS = 10;
	int loop_count = 0;
	while(abs(new_RMS - temp_RMS) > 0.01){
	  loop_count += 1;
	  temp_RMS = temp->GetRMS();
	  aymin = (temp->GetMean()) - 5*temp_RMS;
	  aymax = (temp->GetMean()) + 5*temp_RMS;
	  temp->SetAxisRange(aymin, aymax);
	  mean = temp->GetMean();
	  new_RMS = temp->GetRMS();
	}
	mean_error = temp->GetMeanError();
	RMS_error = temp->GetRMSError();
	if(log_mode == "ON"){ 
	  (m_mean_vs_ptPlots[var])->Fill((m_Pt_logmin + (i-1)*logpt_step), mean);
	  (m_resptPlots[var])->Fill((m_Pt_logmin + (i-1)*logpt_step), new_RMS);
	}else if(log_mode == "OFF"){        
	  (m_mean_vs_ptPlots[var])->Fill((m_PtMin + (i-1)*pt_step), mean);
	  (m_resptPlots[var])->Fill((m_PtMin + (i-1)*pt_step), new_RMS);
	}
	(m_mean_vs_ptPlots[var])->SetBinError(i, mean_error);
	(m_resptPlots[var])->SetBinError(i, RMS_error);
      }else if(width_method == "gaussian"){
	double sigma(0);
	if(temp->GetEntries() != 0){
	  mean = temp->Fit("gaus", "QS0")->Parameter(1);
	  sigma = temp->Fit("gaus", "QS0")->Parameter(2);
	  mean_error = temp->GetMeanError();
	  RMS_error = temp->GetRMSError();
	  if(log_mode == "ON"){
	    (m_mean_vs_ptPlots[var])->Fill((m_Pt_logmin + (i-1)*logpt_step), mean);
	    (m_resptPlots[var])->Fill((m_Pt_logmin + (i-1)*logpt_step), sigma);
	  }else if(log_mode == "OFF"){
	    (m_mean_vs_ptPlots[var])->Fill((m_PtMin + (i-1)*pt_step), mean);
	    (m_resptPlots[var])->Fill((m_PtMin + (i-1)*pt_step), sigma);
	  }
	  (m_mean_vs_ptPlots[var])->SetBinError(i, mean_error);
	  (m_resptPlots[var])->SetBinError(i, RMS_error);
	}else{
	  if(log_mode == "ON"){
	    (m_mean_vs_ptPlots[var])->Fill((m_Pt_logmin + (i-1)*logpt_step), mean);
	    (m_resptPlots[var])->Fill((m_Pt_logmin + (i-1)*logpt_step), sigma);
	  }else if(log_mode == "OFF"){
	    (m_mean_vs_ptPlots[var])->Fill((m_PtMin + (i-1)*pt_step), mean);
	    (m_resptPlots[var])->Fill((m_PtMin + (i-1)*pt_step), sigma);
	  }
	}
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
