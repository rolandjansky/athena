/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MonitoringFile_IDEnhancedPrimaryVertex.cxx,v 1.1 2009-04-03 08:48:00 ponyisi Exp $
// **********************************************************************

#include "DataQualityUtils/MonitoringFile.h"

// #include <cmath>
#include <vector>

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TProfile.h>
#include <TMath.h>
#include <TString.h>

namespace dqutils {

// define helper methods here rather then in this central MonitoringFile.h "beast"
void plotResolution(const TString& coordinate, const TString& versus);
void plotEfficiency();
double error_func( float  x, const Double_t *par);
double scaleFactorFitFcn(double *x, double *par);
std::vector<float> stableGaussianFit(TH1 * histo);

void MonitoringFile::pv_PrimaryVertexMonitoring_calcResoAndEfficiency( const std::string & inFilename, bool /* isIncremental */ )
{
//  std::cout << "\n";
//  std::cout << "Running Inner-Detector primary vertexing monitoring analysis\n";
//   std::cout << "\nWarning messages from fitting and histogram updating follow:\n\n";

  TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
  if (f == 0 || ! f->IsOpen()) {
      //std::cerr << "MonitoringFile::PrimaryVertexMonitoring_calcResoAndEfficiency(): "
//	      << "Input file not opened\n";
    delete f;
    return;
  }
  if(f->GetSize()<1000.) {
//    std::cerr << "MonitoringFile::PrimaryVertexMonitoring_calcResoAndEfficiency(): "
//	      << "MonitoringFile empty\n";
    delete f;
    return; 
  }

  bool dirExists = false;
  TString run_dir;
  TIter next_run ( f->GetListOfKeys() );
  TKey* key_run(0);
  while ((key_run = dynamic_cast<TKey*>( next_run() )) !=0 ) {
    TObject* obj_run = key_run->ReadObj();
    TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
    if (tdir_run !=0 ) {
      std::string tdir_run_name( tdir_run->GetName() );
      if (tdir_run_name.find("run")!= std::string::npos ) {
        run_dir=tdir_run_name;

        dirExists = f->GetDirectory(run_dir+"/InDetGlobal/PrimaryVertex");
        if (dirExists)
        {
//          std::cout << run_dir << "/InDetGlobal/PrimaryVertex exists. Creating and saving plots:" << std::endl;
          f->cd(run_dir+"/InDetGlobal/PrimaryVertex");
        }
      }
    }
    else
    {
	delete obj_run;
    }
  }

  if (!dirExists)
  {
      //std::cerr << "Either enhanced vertex info is not there or monitoring file was produced outside T0 context. Trying dir: InDetGlobal/PrimaryVertex\n";
    dirExists = f->GetDirectory("InDetGlobal/PrimaryVertex");
    if (dirExists) f->cd("InDetGlobal/PrimaryVertex");
  }

  if (dirExists)
  {
    // create histos
    plotResolution("X", "Ntrk");
    plotResolution("Y", "Ntrk");
    plotResolution("Z", "Ntrk");

    plotResolution("X", "SumPt2");
    plotResolution("Y", "SumPt2");
    plotResolution("Z", "SumPt2");

    plotEfficiency();
  }

  f->Close();
  delete f;
//  std::cout << "\n";
//  std::cout << "Finish Inner-Detector primary vertex monitoring analysis"<<std::endl;
  return;
}

void
plotResolution(const TString& coordinate="Z", const TString& versus="Ntrk")
{
//  cout << "Creating and writing histos for resolution " << coordinate << " versus " << versus << endl;

  TH2F* h_Vrt_pullVsSomething_split(0);
  TH2F* h_Vrt_err_vs_Something(0);
//   TH2F* h_Vrt_Tag_err_vs_Something(0);
  TString xAxisLabel("");
  if (versus == "Ntrk")
  {
    h_Vrt_pullVsSomething_split = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"pullVsNtrkAverage_split");
    h_Vrt_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"err_vs_ntrk");
//     h_Vrt_Tag_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_Tag_"+coordinate+"err_vs_ntrk");
    xAxisLabel = "Number of fitted tracks";
  } else if (versus == "SumPt2")
  {
    h_Vrt_pullVsSomething_split = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"pullVsPt2Average_split");
    h_Vrt_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_"+coordinate+"err_vs_pt2");
//     h_Vrt_Tag_err_vs_Something      = (TH2F*)gDirectory->Get("Vrt_Tag_"+coordinate+"err_vs_pt2");
    xAxisLabel = "#sqrt{#sum p_{T}^{2}} [GeV]";
  } else return;

  //if (h_Vrt_pullVsSomething_split == 0) std::cout << "h_Vrt_pullVsSomething_split has zero pointer!" << std::endl;
  //if (h_Vrt_err_vs_Something == 0) std::cout << "h_Vrt_err_vs_Something has zero pointer!" << std::endl;
  if (h_Vrt_pullVsSomething_split == 0 or h_Vrt_err_vs_Something == 0) return;

  int n_bins   = h_Vrt_pullVsSomething_split->GetNbinsX();
  std::vector<float> rms_z;
  std::vector<float> rms_z_er;
  std::vector<float> sigma_z;
  std::vector<float> sigma_z_er;
  std::vector<float> bins_z_nt;
  std::vector<float> bins_z_nt_er;

// root starts counting the bins at 1, i.e. bin 1 holds NTrk = 0. or sqrt(sumpt2) = 0. - 0.25. GeV
//   std::cout << "n bins: " << n_bins << "\tTotal entries: " << h_Vrt_pullVsSomething_split->GetEntries() << std::endl;
//   TH1D * nTrksPerVertex = h_Vrt_pullVsSomething_split->ProjectionX("projectionNTrks_"+coordinate+"_"+versus);

//   TH1D * profileZFull = h_Vrt_pullVsSomething_split->ProjectionY("projectionPullsFull_"+coordinate+"_"+versus);

  Int_t startBin = 0;
  TH1D *profileZ = 0;
  const Int_t minEntriesForKFactorBin = 1000;
  for(int bin_count = 1; bin_count < n_bins+1; bin_count++)
  {
    //Fixed binning
//     TH1D *profileZ = h_Vrt_pullVsSomething_split->ProjectionY("projectionPulls", bin_count, bin_count,"e");
//     Double_t binCenter = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinCenter(bin_count);
//     Double_t binWidth = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinWidth(bin_count)/2.;

    //Variable binning
    TH1D *profileZTmp = h_Vrt_pullVsSomething_split->ProjectionY("projectionPulls", bin_count, bin_count,"e");
    //cout << "Bin: " << bin_count << ", Entries: " << profileZTmp->GetEntries() << endl;
    if (profileZ == 0) {
      startBin = bin_count;
      profileZ = (TH1D*) profileZTmp->Clone("projectionPulls_Integrated");
      //cout << "StartBin = " << startBin << endl;
    } else {
      profileZ->Add(profileZTmp);
    }
    delete profileZTmp;
    profileZTmp = 0;
    if ((profileZ->GetEntries() < minEntriesForKFactorBin) && (bin_count < n_bins)) //not enough entries, not last bin
      continue;

    Double_t lowEdge = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinLowEdge(startBin);
    Double_t highEdge = h_Vrt_pullVsSomething_split->GetXaxis()->GetBinLowEdge(bin_count) + h_Vrt_pullVsSomething_split->GetXaxis()->GetBinWidth(bin_count);
    Double_t binCenter = (lowEdge + highEdge) / 2;
    Double_t binWidth = (highEdge - lowEdge) / 2; //half of the bin width
    //cout << "Bin done: " << binCenter << " +/- " << binWidth << ", Entries: " << profileZ->GetEntries() << endl;
    // variable binning end

    bins_z_nt.push_back(binCenter);
    bins_z_nt_er.push_back(binWidth); // dummy error of binwidth for now

    rms_z.push_back(profileZ->GetRMS());
    rms_z_er.push_back(profileZ->GetRMSError());

    //making a gaussian fit if there is anough entries
    if(profileZ->GetEntries() > 100.)
    {
      std::vector<float> fit_res = stableGaussianFit(profileZ);
      sigma_z.push_back(fit_res[0]);
      sigma_z_er.push_back(fit_res[1]);
    }else{
      sigma_z.push_back(0.);
      sigma_z_er.push_back(0.);
    }//end of good number of bins selection

    delete profileZ; // must keep this to delete the projection from memory (next one has same name!)
    profileZ = 0;
  }//end of loop over all the ntrk bins

  TGraphErrors * krms_z_vs_ntrk = new TGraphErrors(bins_z_nt.size(), &(bins_z_nt[0]),&(rms_z[0]),&(bins_z_nt_er[0]), &(rms_z_er[0]) );
  krms_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" scale factor from RMS");
  krms_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  krms_z_vs_ntrk->SetTitle("scaleFactor"+coordinate+"_RMS");
  krms_z_vs_ntrk->SetName("scaleFactor"+coordinate+"_"+versus+"_RMS");

  TGraphErrors * kgs_z_vs_ntrk = new TGraphErrors(bins_z_nt.size(), &(bins_z_nt[0]),&(sigma_z[0]),&(bins_z_nt_er[0]), &(sigma_z_er[0]) );
  kgs_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" scale factor from gauss fit");
  kgs_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  kgs_z_vs_ntrk->SetTitle("scaleFactor"+coordinate+"_Fit");
  kgs_z_vs_ntrk->SetName("scaleFactor_"+coordinate+"_"+versus+"_Fit");

// approximating the graph with 2nd order polynomial.
  float maxFitRange(100.);
  float minFitRange(2.);
  if (versus == "SumPt2") {
    minFitRange = 0.5;
    maxFitRange = 20.;
  }
  TF1 *kgs_z_ntrk_fit;
  const Double_t *kgs_z_ntrk_fit_er;
  int fitResKFactorMethod = 2; // set by hand for now
  if (fitResKFactorMethod == 1) {
    //Fit with a pol2
    kgs_z_vs_ntrk->Fit("pol2","Q","",minFitRange,maxFitRange);
    kgs_z_vs_ntrk->GetFunction("pol2")->SetLineColor(kRed);
    kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol2");
    kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors();
  } else if (fitResKFactorMethod == 2) {
    //Fit with a pol1
    kgs_z_vs_ntrk->Fit("pol1","Q","",minFitRange,maxFitRange);
    kgs_z_vs_ntrk->GetFunction("pol1")->SetLineColor(kRed);
    kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol1");
    kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors();
  } else if (fitResKFactorMethod == 3) {
    TF1 *kgsFitFcn = new TF1("kgsFitFcn", scaleFactorFitFcn, minFitRange, maxFitRange,3);
    kgsFitFcn->SetParameter(0, minFitRange);
    kgsFitFcn->SetParameter(1, 1.0);
    kgsFitFcn->SetParameter(2, 1.0);
    for (int ifit=0; ifit < 1; ifit++) //initial estimation of best parameters
      kgs_z_vs_ntrk->Fit(kgsFitFcn, "Q");
    kgs_z_vs_ntrk->Fit(kgsFitFcn, "Q"); //perform actual fit
    kgs_z_ntrk_fit = kgsFitFcn;
    kgs_z_ntrk_fit_er = kgsFitFcn->GetParErrors();
/*    cout << "ScaleFactor fit for " << coordinate << " vs " << versus << " (method " << fitResKFactorMethod << ")= "
        << kgsFitFcn->GetParameter(0) << " +/- " << kgsFitFcn->GetParError(0) << " "
        << kgsFitFcn->GetParameter(1) << " +/- " << kgsFitFcn->GetParError(1) << " "
        << kgsFitFcn->GetParameter(2) << " +/- " << kgsFitFcn->GetParError(2) << endl; */
  } else if (fitResKFactorMethod == 4) {
    //constant fit
    kgs_z_vs_ntrk->Fit("pol0","Q","",minFitRange,maxFitRange);
    kgs_z_vs_ntrk->GetFunction("pol0")->SetLineColor(kRed);
    kgs_z_ntrk_fit = kgs_z_vs_ntrk->GetFunction("pol0");
    kgs_z_ntrk_fit_er = kgs_z_ntrk_fit->GetParErrors();
/*    cout << "ScaleFactor fit for " << coordinate << " vs " << versus << " (method " << fitResKFactorMethod << ")= "
      << kgs_z_ntrk_fit->GetParameter(0) << " +/- " << kgs_z_ntrk_fit->GetParError(0) << endl; */
  }

// plotting the fit error of the unconstrained primary vertex and correcting them
  int nbins_z_err_ntrk = h_Vrt_err_vs_Something->GetNbinsX();

  std::vector<float> av_err_z;
  std::vector<float> av_err_z_er;
//   std::vector<float> av_err_tag_z;
//   std::vector<float> av_err_tag_z_er;
  std::vector<float> err_bins_z_nt;
  std::vector<float> err_bins_z_nt_er;
  std::vector<float> res_z;
  std::vector<float> res_z_er;
//   std::vector<float> res_tag_z;
//   std::vector<float> res_tag_z_er;

  for(int bin_count = 1; bin_count <= nbins_z_err_ntrk; ++bin_count)
  {
    err_bins_z_nt.push_back(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count));
    err_bins_z_nt_er.push_back(h_Vrt_err_vs_Something->GetXaxis()->GetBinWidth(bin_count)/2.);

    TH1D * profileY = h_Vrt_err_vs_Something->ProjectionY("projectionErrors",bin_count,bin_count,"e");
//     TH1D * profileYTag(0);
//     if (h_Vrt_Tag_err_vs_Something)
//       profileYTag = h_Vrt_Tag_err_vs_Something->ProjectionY("projectionErrorsTag",bin_count,bin_count,"e");

    float mean       = profileY->GetMean();
    float mean_error = profileY->GetMeanError();
//     float meanTag(0);
//     float mean_errorTag(0);
//     if (profileYTag) {
//       meanTag = profileYTag->GetMean();
//       mean_errorTag = profileYTag->GetMeanError();
//     }
    delete profileY;
//     delete profileYTag;

    av_err_z.push_back(mean);
    av_err_z_er.push_back(mean_error);
//     av_err_tag_z.push_back(meanTag);
//     av_err_tag_z_er.push_back(mean_errorTag);

  //estimating the approximate k-factor and the error value
    double pr_er  =  0.0;
    float val(0.);
    if (fitResKFactorMethod == 1) {
      pr_er = error_func( bin_count, kgs_z_ntrk_fit_er );
    } else if (fitResKFactorMethod == 2) {
      val = h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count);
      pr_er = TMath::Power(kgs_z_ntrk_fit_er[1]*val, 2) + TMath::Power(kgs_z_ntrk_fit_er[0], 2);
      pr_er = TMath::Sqrt(pr_er);
//       cout << "val = " << val << ", pr_er = " << pr_er << ", p0er = " << kgs_z_ntrk_fit_er[0] << ", p1er = "<< kgs_z_ntrk_fit_er[1] << endl;
    } else if (fitResKFactorMethod == 3) {
      val = h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count);
      //approximately the error on the plateau
      pr_er = kgs_z_ntrk_fit_er[2];
    } else if (fitResKFactorMethod == 4) {
      pr_er = kgs_z_ntrk_fit_er[0];
    }

    res_z.push_back(mean * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)));
    res_z_er.push_back(TMath::Sqrt(TMath::Power(mean_error * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)),2) + TMath::Power( pr_er * mean ,2)));
//     res_tag_z.push_back(meanTag * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)));
//     res_tag_z_er.push_back(TMath::Sqrt(TMath::Power(mean_errorTag * kgs_z_ntrk_fit->Eval(h_Vrt_err_vs_Something->GetXaxis()->GetBinCenter(bin_count)),2) + TMath::Power( pr_er * mean ,2)));
  }
  TGraphErrors * res_z_vs_ntrk = new TGraphErrors(err_bins_z_nt.size(), &(err_bins_z_nt[0]),&(res_z[0]),&(err_bins_z_nt_er[0]), &(res_z_er[0]) );
  res_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" Vertex Resolution [mm]");
  res_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
  res_z_vs_ntrk->SetTitle(coordinate+" Vertex Resolution");
  res_z_vs_ntrk->SetName("resolution_"+coordinate+"_"+versus);

//   TGraphErrors * res_tag_z_vs_ntrk = new TGraphErrors(err_bins_z_nt.size(), &(err_bins_z_nt[0]),&(res_tag_z[0]),&(err_bins_z_nt_er[0]), &(res_tag_z_er[0]) );
//   res_tag_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" Tagged Vertex Resolution [mm]");
//   res_tag_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
//   res_tag_z_vs_ntrk->SetTitle(coordinate+" Tagged Vertex Resolution");
//   res_tag_z_vs_ntrk->SetName("resolution_tag_"+coordinate+"_"+versus);

//   TGraphErrors * mean_err_z_vs_ntrk = new TGraphErrors(err_bins_z_nt.size(), &(err_bins_z_nt[0]),&(av_err_z[0]),&(err_bins_z_nt_er[0]), &(av_err_z_er[0]) );
//   mean_err_z_vs_ntrk->GetYaxis()->SetTitle(coordinate+" mean vertex error [mm]");
//   mean_err_z_vs_ntrk->GetXaxis()->SetTitle(xAxisLabel);
//   mean_err_z_vs_ntrk->SetTitle(coordinate+" Mean Vertex Error");
//   mean_err_z_vs_ntrk->SetName("resolution_"+coordinate+"_"+versus+"_Uncorrected"); //not corrected with k-factor

  ///////////////
  // Writing out
  ///////////////
  if (versus == "Ntrk") res_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,100.);
  else res_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,20.);
  res_z_vs_ntrk->GetYaxis()->SetRangeUser(0.0025,1.);
  res_z_vs_ntrk->Write("", TObject::kOverwrite);
  delete res_z_vs_ntrk;

  if (versus == "Ntrk") krms_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,100.);
  else krms_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,20.);
  krms_z_vs_ntrk->GetYaxis()->SetRangeUser(0.5,1.3);
  krms_z_vs_ntrk->Write("", TObject::kOverwrite);
  delete krms_z_vs_ntrk;

  if (versus == "Ntrk") kgs_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,100.);
  else kgs_z_vs_ntrk->GetXaxis()->SetRangeUser(0.,20.);
  kgs_z_vs_ntrk->GetYaxis()->SetRangeUser(0.5,1.3);
  kgs_z_vs_ntrk->Write("", TObject::kOverwrite);
  delete kgs_z_vs_ntrk;

//   nTrksPerVertex->GetYaxis()->SetTitle("Entries");
//   nTrksPerVertex->Draw();
//   nTrksPerVertex->Write("", TObject::kOverwrite);

  return;
}

// Reconstruction efficiency
void plotEfficiency()
{
//  cout << "Creating and writing histos for efficiency" << endl;

  // get histos but return if any histo is not there in input
  TH1F *h_Vrt_split_tag_ntrk = (TH1F*)gDirectory->Get("Vrt_split_tag_ntrk");                       if (h_Vrt_split_tag_ntrk == 0) return;
  TH1F *h_Vrt_split_probe_ntrk = (TH1F*)gDirectory->Get("Vrt_split_probe_ntrk");                   if (h_Vrt_split_probe_ntrk == 0) return;
  TH1F *h_Vrt_split_matched_tag_ntrk = (TH1F*)gDirectory->Get("Vrt_split_matched_tag_ntrk");       if (h_Vrt_split_matched_tag_ntrk == 0) return;
  TH1F *h_Vrt_split_matched_probe_ntrk = (TH1F*)gDirectory->Get("Vrt_split_matched_probe_ntrk");   if (h_Vrt_split_matched_probe_ntrk == 0) return;
  TH1F *h_Vrt_split_dist_tag = (TH1F*)gDirectory->Get("Vrt_split_dist_tag");                       if (h_Vrt_split_dist_tag == 0) return;
  TH1F *h_Vrt_split_dist_probe = (TH1F*)gDirectory->Get("Vrt_split_dist_probe");                   if (h_Vrt_split_dist_probe == 0) return;

  // Use BayesDivide routing of TGraphAsymmErrors
  TGraphAsymmErrors *g_Vrt_rec_eff_m1_split_vs_ntrk = new TGraphAsymmErrors();
  g_Vrt_rec_eff_m1_split_vs_ntrk->BayesDivide(h_Vrt_split_probe_ntrk, h_Vrt_split_tag_ntrk);
  g_Vrt_rec_eff_m1_split_vs_ntrk->SetName("g_RecEff_M1");

  TGraphAsymmErrors *g_Vrt_sel_eff_m1_split_vs_ntrk = new TGraphAsymmErrors();
  g_Vrt_sel_eff_m1_split_vs_ntrk->BayesDivide(h_Vrt_split_matched_probe_ntrk, h_Vrt_split_matched_tag_ntrk);
  g_Vrt_sel_eff_m1_split_vs_ntrk->SetName("g_SelEff_M1");
  
  // formatting and writing out
  g_Vrt_rec_eff_m1_split_vs_ntrk->GetHistogram()->GetXaxis()->SetTitle("Number of tracks");
  g_Vrt_rec_eff_m1_split_vs_ntrk->GetHistogram()->GetYaxis()->SetTitle("Reconstruction efficiency");
  g_Vrt_rec_eff_m1_split_vs_ntrk->SetMarkerStyle(20);
  g_Vrt_rec_eff_m1_split_vs_ntrk->Write("", TObject::kOverwrite);
  delete g_Vrt_rec_eff_m1_split_vs_ntrk;

  g_Vrt_sel_eff_m1_split_vs_ntrk->GetHistogram()->GetXaxis()->SetTitle("Number of tracks");
  g_Vrt_sel_eff_m1_split_vs_ntrk->GetHistogram()->GetYaxis()->SetTitle("Selection Efficiency");
  g_Vrt_sel_eff_m1_split_vs_ntrk->SetMarkerStyle(20);
  g_Vrt_sel_eff_m1_split_vs_ntrk->Write("", TObject::kOverwrite);
  delete g_Vrt_sel_eff_m1_split_vs_ntrk;

  return;
}

std::vector<float> stableGaussianFit(TH1 * histo)
{
  std::vector<float> results;
  if(histo)
  {
    double sigmas  = 2.;

    histo->Fit("gaus","Q0","",-2,2);
    TF1* func = histo->GetFunction("gaus");
    double actualSigma=func->GetParameter(2);
    double actualSigmaErr=func->GetParError(2);

    for (int u=0;u<10;u++)
    {
      histo->Fit("gaus","Q0","",-sigmas*actualSigma,sigmas*actualSigma);
      func = histo->GetFunction("gaus");
      actualSigma    = func->GetParameter(2);
      actualSigmaErr = func->GetParError(2);
    }//end of the fitting loop

    results.push_back(actualSigma);
    results.push_back(actualSigmaErr);
  }else{
    results.push_back(0.);
    results.push_back(0.);
  }//end of protection against an empty histogram

  return results;
}//end of stableGaussian Fit function

double error_func( float  x, const Double_t *par)
{
//calculating the square of the propagated error on the fit values
  return  ( TMath::Power(par[0],2) +  x * TMath::Power(par[1],2) +   TMath::Power(x * par[2],2)) ;
}

double scaleFactorFitFcn(double *x, double *par)
{
  // Truncated gaus-smeared step function
  // par 0: mean of turn-on (and truncation point)
  // par 1: slope of turn-on
  // par 2: plateau
  return (x[0]>=par[0])*TMath::Erf(par[1]*x[0] - par[0])*par[2];
}

}
