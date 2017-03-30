//
#include "PlotEvolution2.h"
//
fit_results PlotEvolution2(int etabin=1, int phibin=1, TFile *HistosFile  = new TFile("/afs/cern.ch/work/o/oestrada/public/alignment2/Jpsi_20.7.3.8/20.7.3.8/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/scripts/ZmumuValidationExampleOutput.Data.stableperiodPeriodE.epsilonm002_3_eta_1_phiRefit2Params.root"))
{
  bool debug = true;
  //TString FileName("ZmumuValidationExample_epsilon_000_MC_ID.root");
  //TString FileName("ZmumuValidationExample_epsilon_000_RD.root");
  //TString FileName("Jpsi_epsilon_000_RD.root");
  //TString FileName();
  //TFile   HistosFile(FileName.Data());
  //TFile HistosFile;
 
  std::vector<float> v_iter;
  std::vector<float> v_deltaM;
  std::vector<float> e_deltaM;
  std::vector<float> v_epsilon;
  std::vector<float> e_epsilon;
  std::vector<float> v_chi2;

  fit_results thisfit;

  // loop on folders
  bool goodfolder = true;
  int  iternumber = 1;
  TString foldername, histoname;
  while (goodfolder) {
      foldername.Clear(); foldername.Append("Iteration"); foldername += iternumber;
      TDirectory* iterfolder = HistosFile->GetDirectory(foldername.Data());
      if (iterfolder != NULL) {
	if (debug) cout << " --- " << foldername.Data() << endl;
	histoname.Clear(); histoname.Append(foldername.Data()); histoname.Append("/h_dimuon_mass_vs_etaphi_fitresultVsPDG");
	if (debug) cout << "   -> " << histoname.Data() << endl;
	TH2F* h_deltaM = (TH2F*) HistosFile->Get(histoname.Data());

	histoname.Clear(); histoname.Append(foldername.Data()); histoname.Append("/h_dimuon_mass_vs_etaphi_fiterror");
	if (debug) std::cout << "   -> " << histoname.Data() << std::endl;
	TH2F* h_deltaM_e = (TH2F*) HistosFile->Get(histoname.Data());

	histoname.Clear(); histoname.Append(foldername.Data()); histoname.Append("/h_radialdistortion_vs_etaphi_Integrated");
	TH2F* h_epsilon = (TH2F*) HistosFile->Get(histoname.Data());

	histoname.Clear(); histoname.Append(foldername.Data()); histoname.Append("/h_radialdistortion");
        //histoname.Clear(); histoname.Append(foldername.Data()); histoname.Append("/h_radialdistortion_vs_etaphi_Integrated");
	TH2F* h_epsilon_e = (TH2F*) HistosFile->Get(histoname.Data());

        histoname.Clear(); histoname.Append(foldername.Data()); histoname.Append("/h_dimuon_mass_vs_etaphi_Chi2overNdf");
	TH2F* h_chi2 = (TH2F*) HistosFile->Get(histoname.Data());


	v_iter.push_back((float) iternumber);
	v_deltaM.push_back(h_deltaM->GetBinContent(etabin,phibin));
	e_deltaM.push_back(h_deltaM_e->GetBinContent(etabin,phibin));

	v_epsilon.push_back(h_epsilon->GetBinContent(etabin,phibin));
	//e_epsilon.push_back(h_epsilon_e->GetRMS()/sqrt(h_epsilon_e->GetEntries()));
	e_epsilon.push_back(h_epsilon->GetBinError(etabin,phibin));
        
        v_chi2.push_back(h_chi2->GetBinContent(etabin,phibin));

      }
      else {
	goodfolder = false;
      }
      iternumber++;
  }

  TH1F* h_deltaM_evol = new TH1F("h_deltaM_evol","#DeltaM evolution; Iteration; #Delta M [MeV]", v_iter.size(), 0.5, v_iter.size()+0.5);
  h_deltaM_evol->SetStats(kFALSE);
  double mass_range = 20;
  h_deltaM_evol->SetMaximum( mass_range);
  h_deltaM_evol->SetMinimum(-mass_range);
  h_deltaM_evol->SetMarkerStyle(20);

  TH1F* h_epsilon_evol = new TH1F("h_epsilon_evol","#epsilon evolution; Iteration; #epsilon", v_iter.size(), 0.5, v_iter.size()+0.5);
  h_epsilon_evol->SetStats(kFALSE);
  double epsilon_range = 0.002;
  h_epsilon_evol->SetMaximum( epsilon_range);
  h_epsilon_evol->SetMinimum(-epsilon_range);
  h_epsilon_evol->SetMarkerStyle(20);

  // fill histos
  for (int i=0; i < v_iter.size(); i++) {
    if (debug) {
      std::cout << "  iter: " << v_iter.at(i) 
		<< "    deltaM: " << v_deltaM.at(i) 
		<< " +- " << e_deltaM.at(i) 
		<< "    epsilon: " << v_epsilon.at(i)
		<< " +- " << e_epsilon.at(i)
                << "    chi2/ndf: " << v_chi2.at(i)
		<< std::endl;
    }
    if (v_chi2.at(i) < 4.0) {
    h_deltaM_evol->SetBinContent((int) v_iter.at(i), v_deltaM.at(i));
    h_deltaM_evol->SetBinError((int) v_iter.at(i), e_deltaM.at(i));
    h_epsilon_evol->SetBinContent((int) v_iter.at(i), v_epsilon.at(i));
    h_epsilon_evol->SetBinError((int) v_iter.at(i), e_epsilon.at(i)); 
    }
      
  }

  // fitting function:
  TF1 *sinewave = new TF1("sinewave","[0]+[1]*sin([2]*x+[3])", 14.5, v_iter.size()+0.5);
  sinewave->SetParameter(0,0.);
  sinewave->SetParameter(1,1.);
  sinewave->SetParameter(2,1.);
  sinewave->SetParameter(3,0.);
  
  /*TF1 *sinewave = new TF1("sinewave", "[0]+[1]*x", 14.5, v_iter.size()+0.5);
  sinewave->SetParameter(0,0.);
  sinewave->SetParameter(1,0.);  */


  // draw deltaM evolution
  TCanvas c1("c_deltaM","delta;",700,500);
  h_deltaM_evol->Fit("sinewave","R");
  chi2overndf =  sinewave->GetChisquare() / sinewave->GetNDF();
  if (chi2overndf > 4.0) {
   std::cout << " **** WARNING ****: Big Chi2 / Ndf: " << chi2overndf << "   chi2: " << sinewave->GetChisquare() << "  ndf: " << sinewave->GetNDF() << std::endl;
  }
  h_deltaM_evol->DrawCopy("p");
  TLine lineForMean;
  lineForMean.SetLineColor(kRed);
  lineForMean.SetLineStyle(kDashed);
  double xlow, xhigh;
  sinewave->GetRange(xlow, xhigh);
  lineForMean.DrawLine(xlow, sinewave->GetParameter(0), xhigh, sinewave->GetParameter(0));
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  //c1.Print("DeltaMEvol.png");
  if (debug) std::cout << std::endl << " deltaM = " << sinewave->GetParameter(0) << " +- " << sinewave->GetParError(0) << " MeV" << std::endl << std::endl;
  
  thisfit.deltaM_val = sinewave->GetParameter(0);
  thisfit.deltaM_err = sinewave->GetParError(0);


 
  TCanvas c2("c_epsilon","epsilon",700,500);
  h_epsilon_evol->Fit("sinewave","R");
  chi2overndf_e =  sinewave->GetChisquare() / sinewave->GetNDF();
  if (chi2overndf_e > 4.0) {
   std::cout << " **** WARNING ****: Big Chi2 / Ndf: " << chi2overndf << "   chi2: " << sinewave->GetChisquare() << "  ndf: " << sinewave->GetNDF() << std::endl;
  }
  h_epsilon_evol->DrawCopy("p");
  lineForMean.DrawLine(xlow, sinewave->GetParameter(0), xhigh, sinewave->GetParameter(0));
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  //c2.Print("EpsilonEvol.png");
  if (debug) std::cout << std::endl << " epsilon = " << sinewave->GetParameter(0) << " +- " << sinewave->GetParError(0) << std::endl << std::endl;

  thisfit.epsilon_val = sinewave->GetParameter(0);
  thisfit.epsilon_err = sinewave->GetParError(0);
  //thisfit.epsilon_err = sinewave->GetParameter(1)/2;
 
  int x;
  cin >> x;

  h_deltaM_evol->Delete();
  h_epsilon_evol->Delete();
  return thisfit;
}
