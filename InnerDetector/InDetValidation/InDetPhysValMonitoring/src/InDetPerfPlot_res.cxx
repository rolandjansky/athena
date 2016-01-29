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
   m_pullmeanPlots(NPARAMS),
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

  //  double m_Pt_logmin = Log10(m_PtMin);
  //  double m_Pt_logmax = Log10(m_PtMax);

  std::string dir = this->getDirectory();
  //  std::cout << "Histo Directory " <<  dir  << endl;

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
    
    TH2 * meanbaseHisto = Book2D(res_vs_etaName, res_vs_etaTitle, m_trackEtaBins, m_etaMin, m_etaMax, m_yBins, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
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
    TH2 * meanPtbaseHisto = Book2D(res_vs_ptName, mptTitle, m_PtBins, xMin, xMax, m_yBins, -m_paramNames.at(var).second, m_paramNames.at(var).second, PrependDirectory);
    m_mean_vs_ptbasePlots[var] = meanPtbaseHisto;
    TH1 * meanPtHisto = Book1D(resmean_vs_ptName, nptTitle, m_PtBins, xMin, xMax, PrependDirectory);
    m_mean_vs_ptPlots[var] = meanPtHisto;
    TH1 * rptHisto = Book1D(reswidth_vs_ptName, rptTitle, m_PtBins, xMin, xMax, PrependDirectory);
    m_resptPlots[var] = rptHisto;
  
    TH1 * pHisto = Book1D(pullName, pullTitle, nPullBins, allLimits.first, allLimits.second, PrependDirectory);
    m_pullPlots[var] = pHisto;
    
    TH2 * pbHisto = Book2D(pull_vs_etaName, pbTitle, m_trackEtaBins, m_etaMin, m_etaMax, m_yBins, allLimits.first, allLimits.second, PrependDirectory);
    m_pullbasePlots[var] = pbHisto;
    TH1 * pmHisto = Book1D(pullmean_vs_etaName, pmTitle, m_trackEtaBins, m_etaMin, m_etaMax, PrependDirectory);
    m_pullmeanPlots[var] = pmHisto;    
    TH1 * pwHisto = Book1D(pullwidth_vs_etaName, pwTitle, m_trackEtaBins, m_etaMin, m_etaMax, PrependDirectory);
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

    //    std::cout << "Parameter: " << varName << " " << trackParameter << " " << truthIsAvailable << " " << sigmaIsAvailable << endl;

    if(var == Z0SIN_THETA){
      float truthParameter = truthprt.auxdata< float >("z0") * std::sin(truthprt.auxdata< float >("theta"));
     
      (m_meanbasePlots[var])->Fill(eta, (trackParameter - truthParameter));
      if(log_mode == "ON"){
	(m_mean_vs_ptbasePlots[var])->Fill(log_pt, (trackParameter - truthParameter));
      }else if(log_mode == "OFF"){
	(m_mean_vs_ptbasePlots[var])->Fill(pt, (trackParameter - truthParameter));
      }
      
      if(trkprt.isAvailable<float>("z0err") and trkprt.isAvailable<float>("thetaerr")){
	float z0_sigma = (trkprt.auxdata< float >("z0err"));
	float theta_sigma = (trkprt.auxdata< float >("thetaerr"));
	float z0_sin_theta_sigma = sqrt( pow(z0_sigma * std::sin(trkprt.theta()), 2) + pow(trkprt.z0() * theta_sigma * std::cos(trkprt.theta()), 2));
	if(m_pullPlots[var]){
          (m_pullPlots[var])->Fill( (trackParameter - truthParameter) / z0_sin_theta_sigma);
          (m_pullbasePlots[var])->Fill(eta, (trackParameter - truthParameter)/z0_sin_theta_sigma);
        }
      }
    }

    if (truthIsAvailable){   //get the corresponding truth variable, only Fill if it exists
      float truthParameter = (truthprt.auxdata< float >(varName));
      if(m_meanbasePlots[var]){
	if(var == QOVERP){
	  (m_meanbasePlots[var])->Fill(eta, 1.0 - (truthParameter/trackParameter));
	  if(log_mode == "ON"){
	    (m_mean_vs_ptbasePlots[var])->Fill(log_pt, (trackParameter - truthParameter)); // ??? I'M NOT SURE ABOUT THIS, INVESTIGATE!!!
	  }else if(log_mode == "OFF"){
	    (m_mean_vs_ptbasePlots[var])->Fill(pt, 1.0 - (truthParameter/trackParameter));
	  }
	  
	  if(trkprt.isAvailable<float>("pterr")){
	    float pt_sigma = (trkprt.auxdata< float >("pterr"));  //This isn't right, I'm not sure how to get the actual error on Pt, fix later
	    float q_over_pt_sigma = truthParameter * (pt_sigma/pt);
	    if(m_pullPlots[var]){
	      (m_pullPlots[var])->Fill( (trackParameter - truthParameter) / q_over_pt_sigma);
	      (m_pullbasePlots[var])->Fill(eta, (trackParameter - truthParameter)/q_over_pt_sigma);
	    }
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
  string width_method = "iterative_convergence";      //switch for changing between the iterative convergence & gaussian fit strategies

  for (unsigned int var(0);var != NPARAMS; ++var){
    double aymin = 0;
    double aymax = 0;

    for(unsigned int j=1; j<=m_trackEtaBins; j++){
      //Create dummy histo w/ content from TH2 in relevant eta bin
      TH1D * temp = m_meanbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Big_Histo", j), j, j);
      TH1D * temp_pull = m_pullbasePlots[var]->ProjectionY(Form("%s_projy_bin%d", "Pull_Histo", j), j, j);
      double mean(0);
      double mean_error(0);
      double RMS_error(0);
      if( width_method == "iterative_convergence" ){

        double tempRes_RMS = 0;
        double newRes_RMS = 10;
        while(fabs(newRes_RMS - tempRes_RMS) > 0.01){
          tempRes_RMS = temp->GetRMS();
          aymin = (temp->GetMean()) - 5*tempRes_RMS;
          aymax = (temp->GetMean()) + 5*tempRes_RMS;
          temp->SetAxisRange(aymin, aymax);
          mean = temp->GetMean();
          newRes_RMS = temp->GetRMS();
        }
        mean_error = temp->GetMeanError();       
        RMS_error = temp->GetRMSError();      
        (m_meanPlots[var])->SetBinContent(j, mean);
        (m_meanPlots[var])->SetBinError(j, mean_error);
        (m_resoPlots[var])->SetBinContent(j, newRes_RMS);
        (m_resoPlots[var])->SetBinError(j, RMS_error);

        double tempPull_RMS = 0;
        double newPull_RMS = 10;
        while(fabs(newPull_RMS - tempPull_RMS) > 0.01){                                 
	  tempPull_RMS = temp_pull->GetRMS();
	  aymin = (temp_pull->GetMean()) - 5*tempPull_RMS;
	  aymax = (temp_pull->GetMean()) + 5*tempPull_RMS;
	  temp_pull->SetAxisRange(aymin, aymax);
	  mean = temp_pull->GetMean();
	  newPull_RMS = temp_pull->GetRMS();
        }
        mean_error = temp_pull->GetMeanError();
        RMS_error = temp_pull->GetRMSError(); 
        (m_pullmeanPlots[var])->SetBinContent(j, mean);
        (m_pullmeanPlots[var])->SetBinError(j, mean_error);	
        (m_pullwidthPlots[var])->SetBinContent(j, newPull_RMS);
        (m_pullwidthPlots[var])->SetBinError(j, RMS_error);	

        double content = m_pullwidthPlots[var]->GetBinContent(j);
        ATH_MSG_DEBUG("Pull Width: "<<newPull_RMS<<"  Histo Content: "<<content);

      }else if( width_method == "gaussian" ){
	double sigma(0);
	if(temp->GetEntries() != 0){
	  mean = temp->Fit("gaus", "QS0")->Parameter(1);
	  mean_error = temp->GetMeanError();
	  sigma = temp->Fit("gaus", "QS0")->Parameter(2);
	  RMS_error = temp->GetRMSError();
	  (m_meanPlots[var])->SetBinContent(j, mean);
	  (m_meanPlots[var])->SetBinError(j, mean_error);
	  (m_resoPlots[var])->SetBinContent(j, sigma);
	  (m_resoPlots[var])->SetBinError(j, RMS_error);
	}else{
	  (m_meanPlots[var])->SetBinContent(j, mean);
	  (m_resoPlots[var])->SetBinContent(j, sigma);
	}
	double width(0);
	if(temp_pull->GetEntries() != 0){width = temp_pull->Fit("gaus", "QS0")->Parameter(2);}
	(m_pullwidthPlots[var])->SetBinContent(j, width);
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
	while(fabs(new_RMS - temp_RMS) > 0.01){
	  temp_RMS = temp->GetRMS();
	  aymin = (temp->GetMean()) - 5*temp_RMS;
	  aymax = (temp->GetMean()) + 5*temp_RMS;
	  temp->SetAxisRange(aymin, aymax);
	  mean = temp->GetMean();
	  new_RMS = temp->GetRMS();
	}
	mean_error = temp->GetMeanError();
	RMS_error = temp->GetRMSError();
	(m_mean_vs_ptPlots[var])->SetBinContent(i, mean);
	(m_mean_vs_ptPlots[var])->SetBinError(i, mean_error);
	(m_resptPlots[var])->SetBinContent(i, new_RMS);
	(m_resptPlots[var])->SetBinError(i, RMS_error);

      }else if(width_method == "gaussian"){
	double sigma(0);
	if(temp->GetEntries() != 0){
	  mean = temp->Fit("gaus", "QS0")->Parameter(1);
	  sigma = temp->Fit("gaus", "QS0")->Parameter(2);
	  mean_error = temp->GetMeanError();
	  RMS_error = temp->GetRMSError();

	  (m_mean_vs_ptPlots[var])->SetBinContent(i, mean);
	  (m_mean_vs_ptPlots[var])->SetBinError(i, mean_error);
	  (m_resptPlots[var])->SetBinContent(i, sigma);
	  (m_resptPlots[var])->SetBinError(i, RMS_error);
	}else{
	  (m_mean_vs_ptPlots[var])->SetBinContent(i, mean);
	  (m_resptPlots[var])->SetBinContent(i, sigma);
	}
      }
    }
  }
}


std::string
InDetPerfPlot_res::formName(const unsigned int p, std::string type, const std::string dir) const{

  std::string hdir = " ";

  if (dir == "IDPerformanceMon/Tracks/SelectedGoodTracks"){hdir = "GoodTrks_";}
  else if (dir == "IDPerformanceMon/Tracks/SelectedGoodJetTracks"){hdir = "GoodJetTrks_";}
  else if (dir == "IDPerformanceMon/Tracks/SelectedGoodJetHighPtTracks"){hdir = "GoodJetHighPtTrks_";}
  else { std::cout << "Unidentified Dir = " << dir << endl;};
    
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
  std::string titleLeft[NPARAMS] = {"d_{0}: ", "z_{0}: ", "#phi: ", "#theta:", "z_{0}*sin(#theta): ", "q/p: "};
  std::string titleRight[NPARAMS] = {"","","","","",""}; 
  std::string titlePostfix("");

  //  std::cout << "Type : " << type << endl;

  if((type == "res_vs_eta") || (type == "res_vs_pt") || (type == "resmean_vs_eta") || (type == "resmean_vs_pt")){
    titleRight[0] = "(d^{rec}_{0} - d^{tru}_{0})";
    titleRight[1] = "(z^{rec}_{0} - z^{tru}_{0})";
    titleRight[2] = "(#phi^{rec} - #phi^{tru})";
    titleRight[3] = "(#theta^{rec} - #theta^{tru})";
    titleRight[4] = "(z_{0}*sin(#theta)^{rec} - z_{0}*sin(#theta)^{tru})";
    titleRight[5] = "1  - ((q/p)^{rec}/(q/p)^{tru})";

    if((type == "res_vs_eta") || (type == "res_vs_pt")){ 
      titlePrefix = "Residual "; 
    }else if((type == "resmean_vs_eta") || (type == "resmean_vs_pt")){
      titlePrefix = "Residual mean ";}

  }else if((type == "reswidth_vs_eta") || (type == "reswidth_vs_pt")){
    titleRight[0] = "#sigma(d^{rec}_{0} - d^{tru}_{0})";
    titleRight[1] = "#sigma(z^{rec}_{0} - z^{tru}_{0})";
    titleRight[2] = "#sigma(#phi^{rec} - #phi^{tru})";
    titleRight[3] = "#sigma(#theta^{rec} - #theta^{tru})";
    titleRight[4] = "#sigma(z_{0}*sin(#theta)^{rec} - z_{0}*sin(#theta)^{tru})";
    titleRight[5] = "(1 - (q/p)^{tru}/(q/p)^{rec})";
    
    titlePrefix = "Residual width ";
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
    
  //  std::cout << "varName " << titlePrefix << " | " << titleLeft[param] << " | " << titleRight[param] << endl;

  return titlePrefix + titleLeft[param] + titleRight[param] + titlePostfix;
}
