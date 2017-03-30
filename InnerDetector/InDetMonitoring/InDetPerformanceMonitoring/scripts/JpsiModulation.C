TH2F* hdeltaM_Iteration1;
TH2F* hdeltaM_e_Iteration1; 
TCanvas c1("c_deltaM","delta;",700,500);

void JpsiModulation(int etabin=1)
{
  bool debug = false;
  //TString FileName("ZmumuValidationExampleOutput.Data.stableperiodPeriodE.newmodelv2epsilon0_3_eta_6_phi_30iterRefit2Params.root");
  //TString FileName("ZmumuValidationExampleOutput.Data.stableperiodPeriodE.ROOTerrorstest2Refit2Params.root");
  TString FileName("ZmumuValidationExampleOutput_test.root");
  //TString FileName("JPsimumu_2016_5x6.root");
  //TString FileName("ZmumuValidationExampleOutput.Data.stableperiodPeriodE.PROVAIFICSINFORMULALnewmodelv2epsilon0_3_eta_12_phi_30iter_mcbiasRefit1Params.root");
  TFile*   HistosFile;

  const char *IdHisDeltaM_Iteration1 = "Iteration1/h_dimuon_mass_vs_etaphiPos_fitresultVsPDG";
  const char *IdHisDeltaM_e_Iteration1 = "Iteration1/h_dimuon_mass_vs_etaphi_fiterror";
  
  TH2F* hdeltaM_Iteration1;
  TH2F* hdeltaM_e_Iteration1;
 
   
  printf("File  =>  %s\n",FileName.Data());
  HistosFile = new TFile(FileName.Data(),"READ");
  hdeltaM_Iteration1 = (TH2F*) gDirectory->Get(IdHisDeltaM_Iteration1);
  hdeltaM_e_Iteration1 = (TH2F*) gDirectory->Get(IdHisDeltaM_e_Iteration1);
 

  std::vector<float> v_phi;
  std::vector<float> v_deltaM;
  std::vector<float> e_deltaM;
  
  for(int phibin = 1; phibin <= hdeltaM_Iteration1->GetYaxis()->GetNbins() ; phibin++) {
    v_phi.push_back( (float) phibin);
    v_deltaM.push_back(hdeltaM_Iteration1->GetBinContent(etabin,phibin));
    e_deltaM.push_back(hdeltaM_e_Iteration1->GetBinContent(etabin,phibin));
  }
  
  //TH1F* h_deltaM_modulation = new TH1F("h_deltaM_modulation","1st iteration reconstructed mass difference; #phi; #Delta M [MeV]", v_phi.size(), 0.5, v_phi.size()+0.5);
  TH1F* h_deltaM_modulation = new TH1F("h_deltaM_modulation","1st iteration reconstructed mass difference; #phi; #Delta M [MeV]",  
				       hdeltaM_Iteration1->GetNbinsY(), hdeltaM_Iteration1->GetYaxis()->GetXmin(), hdeltaM_Iteration1->GetYaxis()->GetXmax()); 
  h_deltaM_modulation->SetStats(kFALSE);
  h_deltaM_modulation->SetMarkerStyle(20);
  TString theHistoTitle = h_deltaM_modulation->GetTitle();
  TString etaPart; etaPart.Form(" (#eta = %3.2f)", hdeltaM_e_Iteration1->GetXaxis()->GetBinCenter(etabin));
  theHistoTitle.Append(etaPart.Data());
  h_deltaM_modulation->SetTitle(theHistoTitle);

  // fill histos
  for (int i=0; i < v_phi.size(); i++) {
    if (debug == true) {
      std::cout << "  phibin: " << v_phi.at(i) 
                << "    deltaM: " << v_deltaM.at(i) 
                << " +- " << e_deltaM.at(i) 
                << std::endl;
    }

    h_deltaM_modulation->SetBinContent((int) v_phi.at(i), v_deltaM.at(i));
    h_deltaM_modulation->SetBinError((int) v_phi.at(i), e_deltaM.at(i));
  }

  double theRange = max( fabs(h_deltaM_modulation->GetMaximum()), fabs(h_deltaM_modulation->GetMinimum()));
  std::cout << " max: " << h_deltaM_modulation->GetMaximum() << std::endl;
  std::cout << " min: " << h_deltaM_modulation->GetMinimum() << std::endl;
  double number = 10;
  if (theRange < number) theRange = number;
  theRange *= 1.10;
  //theRange = 1771.79;
  std::cout << " theRange= " << theRange << std::endl;
  h_deltaM_modulation->SetMaximum( theRange);
  h_deltaM_modulation->SetMinimum(-theRange);

  // fitting function:
  TF1 *sinewave = new TF1("sinewave","[0]+[1]*sin([2]*x+[3])", h_deltaM_modulation->GetXaxis()->GetXmin(), h_deltaM_modulation->GetXaxis()->GetXmax());
  sinewave->SetParameter(0,0.5); //was 0
  sinewave->SetParameter(1,0.5);
  sinewave->SetParameter(2,2.0);
  sinewave->SetParameter(3,0.0); //was0
  
  sinewave->SetParName(0,"DeltaM_0");
  sinewave->SetParName(1,"Ampl.");
  sinewave->SetParName(2,"k");
  sinewave->SetParName(3,"Phase");

  sinewave->FixParameter(2,1.);
  sinewave->SetParLimits(3, -3.5, 3.5);
  //sinewave->SetParLimits(1,0., 100.);

  TF1 *line = new TF1("line", "[0] + [1]*x", 0.5, v_phi.size()+0.5);
  line->SetParameter(0,0.);
  line->SetParameter(1,1.);

  // draw deltaM evolution
  h_deltaM_modulation->Fit("sinewave","R");
  //h_deltaM_modulation->Fit("line","R");
  h_deltaM_modulation->DrawCopy("ep");
  //h_deltaM_modulation->DrawCopy("p");
  //TLine lineForMean;
  //lineForMean.SetLineColor(kRed);
  //lineForMean.SetLineStyle(kDashed);
  //double xlow, xhigh;
  //sinewave->GetRange(xlow, xhigh);
  //lineForMean.DrawLine(xlow, sinewave->GetParameter(0), xhigh, sinewave->GetParameter(0));

  TLine* theBaseline = new TLine();
  theBaseline->SetLineColor(kRed);
  theBaseline->SetLineStyle(kDashed);
  theBaseline->DrawLine(h_deltaM_modulation->GetXaxis()->GetXmin(), sinewave->GetParameter(0),  
			h_deltaM_modulation->GetXaxis()->GetXmax(), sinewave->GetParameter(0));

  gPad->SetGridx(1);
  gPad->SetGridy(1);
  c1.Print("JpsiModulationendcapa_eta_3_phi_12.png");
  if (debug == true) {
    std::cout << std::endl << " Modulation = " << sinewave->GetParameter(0) << " +- " << sinewave->GetParError(0) << " MeV" << std::endl << std::endl;
  }
  std::cout << "  AlphaRot = " << - (sinewave->GetParameter(1))/3096 << std::endl;
  
  //int x;
  //std::cin >> x;
  return;
}
