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
#include <utility>
#include "TH1D.h"
#include "TH2D.h"

using namespace TMath;

InDetPerfPlot_res::InDetPerfPlot_res(InDetPlotBase* pParent, const std::string & sDir)  :InDetPlotBase(pParent, sDir), 
   m_trackEtaBins(20),
   m_etaMin(-2.5),
   m_etaMax(2.5),
   //m_truthEtaCut
   
   m_PtBins(10),
   m_PtMin(0.316227766),
   m_PtMax(100.0),
   //m_mean{},
   m_d0_vs_eta_bin0{},
   log_mode("ON"),
   m_meanbasePlots(NPARAMS,nullptr),
   m_meanPlots(NPARAMS,nullptr),
   m_mean_vs_ptbasePlots(NPARAMS, nullptr),
   m_mean_vs_ptPlots(NPARAMS, nullptr),
   m_resoPlots(NPARAMS,nullptr),
   m_resptPlots(NPARAMS, nullptr),
   m_pullPlots(NPARAMS, nullptr),
   m_pullbasePlots(NPARAMS, nullptr),
   m_pullmeanPlots(NPARAMS, nullptr),
   m_pullwidthPlots(NPARAMS, nullptr),
   m_paramNames{{"d0",{1.0, 2000}}, {"z0",{1.5, 200}},{"phi",{0.03, 3000}}, {"theta",{0.1, 2000}},{"z0*sin(theta)",{1.5, 200}},{"qopt",{0.4,200}}}
   {
//
}


void InDetPerfPlot_res::initializePlots() {
  const bool PrependDirectory(false);  
  typedef std::pair<float, float> Limits_t;
  Limits_t allLimits = Limits_t(-5.0, 5.0); //4-4-16 normally +/- 5, set to +/- 20 for testing
  int nPullBins(200); //number of bins specifically for the pull plots

  std::string dir = this->getDirectory();
  for (unsigned int var(0); var != NPARAMS; ++var){

    std::string res_vs_etaName = formName(var,"res_vs_eta",dir);
    std::string res_vs_etaTitle = formTitle(var, "res_vs_eta");
    std::string resmean_vs_etaName = formName(var, "resmean_vs_eta",dir);
    std::string resmean_vs_etaTitle = formTitle(var, "resmean_vs_eta");
    std::string reswidth_vs_etaName = formName(var,"reswidth_vs_eta",dir);
    std::string reswidth_vs_etaTitle = formTitle(var, "reswidth_vs_eta");
    
    std::string res_vs_ptName = formName(var, "res_vs_pt",dir);
    std::string mptTitle = formTitle(var, "res_vs_pt");
    std::string resmean_vs_ptName = formName(var, "resmean_vs_pt",dir);
    std::string nptTitle = formTitle(var, "resmean_vs_pt");
    std::string reswidth_vs_ptName = formName(var,"reswidth_vs_pt",dir);
    std::string rptTitle = formTitle(var, "reswidth_vs_pt");
    
    std::string pullName = formName(var, "pull",dir);
    std::string pullTitle = formTitle(var, "pull");
    std::string pull_vs_etaName = formName(var, "pull_vs_eta",dir);
    std::string pbTitle = formTitle(var, "pull_vs_eta");
    std::string pullmean_vs_etaName = formName(var, "pullmean_vs_eta",dir);
    std::string pmTitle = formTitle(var, "pullmean_vs_eta");
    std::string pullwidth_vs_etaName = formName(var, "pullwidth_vs_eta",dir);
    std::string pwTitle = formTitle(var, "pullwidth_vs_eta");
    
    TH2 * meanbaseHisto = Book2D(res_vs_etaName, res_vs_etaTitle, m_trackEtaBins, m_etaMin, m_etaMax, m_paramNames.at(var).second.second, -m_paramNames.at(var).second.first, m_paramNames.at(var).second.first, PrependDirectory);
    m_meanbasePlots[var] = meanbaseHisto;
    TH1 * meanHisto = Book1D(resmean_vs_etaName, resmean_vs_etaTitle, m_trackEtaBins, m_etaMin, m_etaMax, PrependDirectory);
    m_meanPlots[var] = meanHisto;
    TH1 * rHisto = Book1D(reswidth_vs_etaName, reswidth_vs_etaTitle, m_trackEtaBins, m_etaMin, m_etaMax, PrependDirectory);
    m_resoPlots[var] = rHisto;

    double xMin(0),xMax(0);
    if(log_mode == "ON"){
      xMin = Log10(m_PtMin);
      xMax = Log10(m_PtMax);
    } else if(log_mode == "OFF"){
      xMin = m_PtMin;
      xMax = m_PtMax;
    }
    TH2 * meanPtbaseHisto = Book2D(res_vs_ptName, mptTitle, m_PtBins, xMin, xMax, m_paramNames.at(var).second.second, -m_paramNames.at(var).second.first, m_paramNames.at(var).second.first, PrependDirectory);
    m_mean_vs_ptbasePlots[var] = meanPtbaseHisto;
    TH1 * meanPtHisto = Book1D(resmean_vs_ptName, nptTitle, m_PtBins, xMin, xMax, PrependDirectory);
    m_mean_vs_ptPlots[var] = meanPtHisto;
    TH1 * rptHisto = Book1D(reswidth_vs_ptName, rptTitle, m_PtBins, xMin, xMax, PrependDirectory);
    m_resptPlots[var] = rptHisto;
  
    TH1 * pHisto = Book1D(pullName, pullTitle, nPullBins, allLimits.first, allLimits.second, PrependDirectory);
    m_pullPlots[var] = pHisto;
    
    TH2 * pbHisto = Book2D(pull_vs_etaName, pbTitle, m_trackEtaBins, m_etaMin, m_etaMax, nPullBins, allLimits.first, allLimits.second, PrependDirectory);
    m_pullbasePlots[var] = pbHisto;
    TH1 * pmHisto = Book1D(pullmean_vs_etaName, pmTitle, m_trackEtaBins, m_etaMin, m_etaMax, PrependDirectory);
    m_pullmeanPlots[var] = pmHisto;    
    TH1 * pwHisto = Book1D(pullwidth_vs_etaName, pwTitle, m_trackEtaBins, m_etaMin, m_etaMax, PrependDirectory);
    m_pullwidthPlots[var] = pwHisto;    
  }
}

void InDetPerfPlot_res::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  //float eta = trkprt.eta();
  //float pt = trkprt.pt(); //unused
  //double log_pt = Log10(pt) - 3.0;  //To convert from MeV to GeV, subtract 3; unused

  double truth_eta = truthprt.eta();  //eta of the truthParticle
  double truth_pt = truthprt.pt();    //pt of the truthParticle
  double log_trupt = Log10(truth_pt) - 3.0;  

  float truth_charge = 1;

  double trkParticleParams[NPARAMS];
  trkParticleParams[D0] = trkprt.d0();
  trkParticleParams[Z0] = trkprt.z0();
  trkParticleParams[PHI] = trkprt.phi0();
  trkParticleParams[THETA] = trkprt.theta();
  trkParticleParams[Z0SIN_THETA] = trkprt.z0() * std::sin(trkprt.theta()); 
  //trkParticleParams[QOVERP] = trkprt.qOverP();
  trkParticleParams[QOPT] = (trkprt.qOverP())*1000./sin(trkprt.theta()); //This switches it to the "qOverPt" PRTT uses

  if(trkParticleParams[PHI] < 0) trkParticleParams[PHI] += 2*CLHEP::pi;  //Add in the 2*pi correction for negative phi, as in PRTT

  for (unsigned int var(0);var!=NPARAMS;++var){
    const std::string  varName = m_paramNames.at(var).first;
    const std::string  errName = varName + std::string("err");
    float trackParameter = trkParticleParams[var];                      //needed for all of them
    bool truthIsAvailable = truthprt.isAvailable<float>(varName);
    if(varName == "qopt") truthIsAvailable = truthprt.isAvailable<float>("qOverP");                      //need to get q/pt to actually fill    
    bool sigmaIsAvailable = trkprt.isAvailable<float>(errName);
    if(varName == "z0*sin(theta)") {
      truthIsAvailable = (truthprt.isAvailable<float>("z0") and truthprt.isAvailable<float>("theta"));
      sigmaIsAvailable = (trkprt.isAvailable<float>("z0err") and trkprt.isAvailable<float>("thetaerr"));
    }
    if (truthIsAvailable){   //get the corresponding truth variable, only Fill if it exists
      float truthParameter = 0;
      float deviation(0);
      if(m_meanbasePlots[var]){
	if(var == QOPT){
	  truthParameter = (truthprt.auxdata< float >("qOverP"))*1000./sin(truthprt.auxdata< float >("theta"));
	  if(truthParameter < 0) truth_charge = -1;
	  deviation = (trackParameter - truthParameter)*fabs(truth_pt/1000.)*truth_charge;  //Used to match PRTT version
	}else if(var == Z0SIN_THETA){
	  truthParameter = truthprt.auxdata< float >("z0") * std::sin(truthprt.auxdata< float >("theta"));
	  deviation = trackParameter - truthParameter;
	}else{
	  truthParameter = (truthprt.auxdata< float >(varName));
	  if((var == PHI) and (truthParameter < 0)) truthParameter += 2*CLHEP::pi; //add in 2*pi correction for negative phi, as in PRTT
	  deviation = trackParameter - truthParameter;
	}
	(m_meanbasePlots[var])->Fill(truth_eta, deviation);
	if(log_mode == "ON"){                                                    
	  (m_mean_vs_ptbasePlots[var])->Fill(log_trupt, deviation);                      
	}else if(log_mode == "OFF"){                                                                                 
	  (m_mean_vs_ptbasePlots[var])->Fill(truth_pt, deviation);                                        
	}             
      }
      if(sigmaIsAvailable){
	float sigma(0);
	if(var == Z0SIN_THETA){
	  float z0_sigma = (trkprt.auxdata< float >("z0err"));
	  float theta_sigma = (trkprt.auxdata< float >("thetaerr"));
	  const float theta=trkprt.theta(); 
	  const float cosTheta{std::cos(theta)}; const float sinTheta{std::sin(theta)};
	  const float z0{trkprt.z0()};
	  //is this right? looks asymmetric. It follows from the standard error propagation formula for uncorrelated variables.
	  const float sigmaSq= ((z0_sigma * sinTheta) * (z0_sigma * sinTheta)) + ( (z0*theta_sigma*cosTheta) * (z0*theta_sigma*cosTheta) );
	  sigma =std::sqrt(sigmaSq);
	}else{
	  sigma = (trkprt.auxdata< float >(errName));
	}
	if(m_pullPlots[var]){
	  float pull(0);
	  if( var == QOPT){
	    pull = ((trackParameter - truthParameter)*truth_charge/sigma); //used to match PRTT version
	  }else{
	    pull = ((trackParameter - truthParameter)/sigma);
	  }
	  (m_pullPlots[var])->Fill(pull);
	  (m_pullbasePlots[var])->Fill(truth_eta, pull);  
	}
      }
    }//REAL END OF IF(TRUTHISAVAILABLE) STATEMENT
  }
}

void InDetPerfPlot_res::Refinement(TH1D * temp, std::string width , int var, int j, const std::vector<TH1*>& tvec, const std::vector<TH1*>& rvec){
  if(temp->GetXaxis()->TestBit(TAxis::kAxisRange)){
    //remove range if set previously
    temp->GetXaxis()->SetRange();
    temp->ResetStats();
  }
  double  mean(0.0), mean_error(0.0), prim_RMS(0.0), sec_RMS(0.0), RMS_error(0.0);
  if(temp->GetEntries() != 0.0){
    mean = temp->GetMean();
    prim_RMS = temp->GetRMS();
    mean_error = temp->GetMeanError();
    RMS_error = temp->GetRMSError();
    if (width == "iterative_convergence"){
      sec_RMS = prim_RMS + 1.0;
      while(fabs(sec_RMS - prim_RMS) > 0.001){
	prim_RMS = temp->GetRMS();
	double aymin = -3.0*prim_RMS;
	double aymax = 3.0*prim_RMS;
	if(aymin < temp->GetBinLowEdge(1))                aymin = temp->GetBinLowEdge(1);
	if(aymax > temp->GetBinCenter(temp->GetNbinsX())) aymax = temp->GetBinCenter(temp->GetNbinsX());
	temp->SetAxisRange(aymin, aymax);
	mean = temp->GetMean();
	sec_RMS = temp->GetRMS();
      }
      mean_error = temp->GetMeanError();
      RMS_error = temp->GetRMSError();
    }else if(width == "gaussian"){
      TFitResultPtr r = temp->Fit("gaus", "QS0");
      if(r.Get() and r->Status()%1000 == 0){
	mean = r->Parameter(1);
	mean_error = r->ParError(1);
	sec_RMS = r->Parameter(2);
	RMS_error = r->ParError(2);
      }
    }else if(width == "fusion"){
      sec_RMS = prim_RMS + 1.0;
      double aymin = temp->GetBinLowEdge(1);
      double aymax = temp->GetBinCenter(temp->GetNbinsX());
      while((fabs(sec_RMS - prim_RMS) > 0.001) ){
	prim_RMS = temp->GetRMS();		
	aymin = -3.0*prim_RMS;
	aymax = 3.0*prim_RMS;
	if(aymin < temp->GetBinLowEdge(1))                aymin = temp->GetBinLowEdge(1);
	if(aymax > temp->GetBinCenter(temp->GetNbinsX())) aymax = temp->GetBinCenter(temp->GetNbinsX());
	temp->SetAxisRange(aymin, aymax);
	mean = temp->GetMean();
	sec_RMS = temp->GetRMS();
      }
      TFitResultPtr r = temp->Fit("gaus", "QS0", "", aymin, aymax);
      if(r.Get() and r->Status()%1000 == 0){
	mean = r->Parameter(1);
	mean_error = r->ParError(1);
	sec_RMS = r->Parameter(2);
	RMS_error = r->ParError(2);
      }
    }
  }
  (tvec[var])->SetBinContent(j, mean);
  (tvec[var])->SetBinError(j, mean_error);
  (rvec[var])->SetBinContent(j, sec_RMS);
  (rvec[var])->SetBinError(j, RMS_error);  
}

void InDetPerfPlot_res::finalizePlots(){
  //switch for changing between iterative convergence & gaussian fit methods
  //iterative convergence with 3*RMS works better than 5*RMS
  std::string width_method = "iterative_convergence"; //4-13-16 normally iterative_convergence, switch to gaussian for testing         
  std::string pull_width_method = "gaussian";

  for (unsigned int var(0);var != NPARAMS; ++var){
    
    for(unsigned int j=1; j<=m_trackEtaBins; j++){
      //Create dummy histo w/ content from TH2 in relevant eta bin
      TH1D * temp = m_meanbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", j), j, j);
      TH1D * temp_pull = m_pullbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Pull_Histo", j), j, j);
      Refinement(temp, width_method, var, j, m_meanPlots, m_resoPlots);
      Refinement(temp_pull, pull_width_method, var, j, m_pullmeanPlots, m_pullwidthPlots); 
    }

    for (unsigned int i=1; i<=m_PtBins; i++){
      TH1D * temp = m_mean_vs_ptbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", i), i , i);
      Refinement(temp, width_method, var, i, m_mean_vs_ptPlots, m_resptPlots);
    }
  }
}


std::string
InDetPerfPlot_res::formName(const unsigned int p, std::string type, const std::string dir) const{

  std::string hdir = " ";

  if (dir == "IDPerformanceMon/Tracks/SelectedGoodTracks"){hdir = "GoodTrks_";}
  else if (dir == "IDPerformanceMon/Tracks/SelectedGoodJetTracks"){hdir = "GoodJetTrks_";}
  else if (dir == "IDPerformanceMon/Tracks/SelectedGoodJetHighPtTracks"){hdir = "GoodJetHighPtTrks_";}
  else { ATH_MSG_INFO( "Unidentified Dir = " << dir );}
    
  // to be removed when postprocessHistos script is ready for extended histo names
  hdir = "";

  if (type == "res_vs_eta"){
    return hdir+std::string("res_")+m_paramNames.at(p).first +std::string("_vs_eta");
  }else if(type == "resmean_vs_eta"){
    return hdir+std::string("resmean_")+m_paramNames.at(p).first +std::string("_vs_eta");
  }else if(type == "reswidth_vs_eta"){
    return hdir+std::string("reswidth_")+m_paramNames.at(p).first +std::string("_vs_eta");
  }else if((type == "res_vs_pt") and (log_mode == "OFF")){
    return hdir+std::string("res_")+m_paramNames.at(p).first +std::string("_vs_pt");
  }else if((type == "res_vs_pt") and (log_mode == "ON")){
    return hdir+std::string("res_")+m_paramNames.at(p).first +std::string("_vs_logpt");
  }else if((type == "resmean_vs_pt") and (log_mode == "OFF")){
    return hdir+std::string("resmean_")+m_paramNames.at(p).first +std::string("_vs_pt");
  }else if((type == "resmean_vs_pt") and (log_mode == "ON")){
    return hdir+std::string("resmean_")+m_paramNames.at(p).first +std::string("_vs_logpt");
  }else if((type == "reswidth_vs_pt") and (log_mode == "OFF")){
    return hdir+std::string("reswidth_")+m_paramNames.at(p).first +std::string("_vs_pt");
  }else if((type == "reswidth_vs_pt") and (log_mode == "ON")){
    return hdir+std::string("reswidth_")+m_paramNames.at(p).first +std::string("_vs_logpt");
  }else if(type == "pull"){
    return hdir+std::string("pull_")+m_paramNames.at(p).first;
  }else if(type == "pull_vs_eta"){
    return hdir+std::string("pull_")+m_paramNames.at(p).first +std::string("_vs_eta");
  }else if(type == "pullmean_vs_eta"){
    return hdir+std::string("pullmean_")+m_paramNames.at(p).first +std::string("_vs_eta");
  }else if(type == "pullwidth_vs_eta"){
    return hdir+std::string("pullwidth_")+m_paramNames.at(p).first +std::string("_vs_eta");
  }
  return "";
}


std::string
InDetPerfPlot_res::formTitle(const unsigned int param, std::string type) const{

  std::string titlePrefix("");
  std::string titleLeft[NPARAMS] = {"d_{0}: ", "z_{0}: ", "#phi: ", "#theta: ", "z_{0}*sin(#theta): ", "q/pt: "};
  std::string titleRight[NPARAMS] = {"","","","","",""}; 
  std::string titlePostfix("");


  if((type == "res_vs_eta") || (type == "res_vs_pt") || (type == "resmean_vs_eta") || (type == "resmean_vs_pt")){
    titleRight[0] = "(d^{rec}_{0} - d^{tru}_{0})";
    titleRight[1] = "(z^{rec}_{0} - z^{tru}_{0})";
    titleRight[2] = "(#phi^{rec} - #phi^{tru})";
    titleRight[3] = "(#theta^{rec} - #theta^{tru})";
    titleRight[4] = "(z_{0}*sin(#theta)^{rec} - z_{0}*sin(#theta)^{tru})";
    titleRight[5] = "((q/p)^{rec} - (q/p)^{tru})";

    if((type == "res_vs_eta") || (type == "res_vs_pt")){ 
      titlePrefix = "Resolution "; 
    }else if((type == "resmean_vs_eta") || (type == "resmean_vs_pt")){
      titlePrefix = "Resolution mean ";}

  }else if((type == "reswidth_vs_eta") || (type == "reswidth_vs_pt")){
    titleRight[0] = "#sigma(d^{rec}_{0} - d^{tru}_{0})";
    titleRight[1] = "#sigma(z^{rec}_{0} - z^{tru}_{0})";
    titleRight[2] = "#sigma(#phi^{rec} - #phi^{tru})";
    titleRight[3] = "#sigma(#theta^{rec} - #theta^{tru})";
    titleRight[4] = "#sigma(z_{0}*sin(#theta)^{rec} - z_{0}*sin(#theta)^{tru})";
    titleRight[5] = "#sigma((q/p)^{rec} - (q/p)^{tru})";
    
    titlePrefix = "Resolution width ";
  }else if((type == "pull") || (type == "pull_vs_eta") || (type == "pullwidth_vs_eta") || (type == "pullmean_vs_eta")){
    titleRight[0] = "(d^{rec}_{0} - d^{tru}_{0})/#sigma_{d_{0}}";
    titleRight[1] = "(z^{rec}_{0} - z^{tru}_{0})/#sigma_{z_{0}}";
    titleRight[2] = "(#phi^{rec} - #phi^{tru})/#sigma_{#phi}";
    titleRight[3] = "(#theta^{rec} - #theta^{tru})/#sigma_{#theta}";
    titleRight[4] = "(z^{rec}_{0}*sin(#theta^{rec}) - z^{tru}_{0}*sin(#theta^{tru})/#sigma_{z_{0}*sin(#theta)}"; //???
    titleRight[5] = "((q/p)^{rec} - (q/p)^{tru})/#sigma_{q/p}";

    if((type == "pull_vs_eta") || (type == "pull")){ 
      titlePrefix = "Pull ";
    } else if (type == "pullmean_vs_eta"){
      titlePrefix = "Pull mean ";
    } else if (type == "pullwidth_vs_eta"){
      titlePrefix = "Pull width ";
    }
  }     

  if((type == "res_vs_eta") || (type == "resmean_vs_eta") || (type == "reswidth_vs_eta") || (type == "pull_vs_eta") || (type == "pullwidth_vs_eta")) titlePostfix = " vs #eta";
  if(log_mode == "ON"){
    if((type == "res_vs_pt") || (type == "resmean_vs_pt") || (type == "reswidth_vs_pt")) titlePostfix = " vs log(p_{T})";} else {
    if((type == "res_vs_pt") || (type == "resmean_vs_pt") || (type == "reswidth_vs_pt")) titlePostfix = " vs p_{T}";}
    

  return titlePrefix + titleLeft[param] + titleRight[param] + titlePostfix;
}
