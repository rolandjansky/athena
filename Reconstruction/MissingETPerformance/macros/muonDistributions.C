/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////
//Produces purity, efficiency, and fake rate plots for the muoncollections used in the RefMET algorithm and compares across samples.
//Also produces eta, phi, and P_T plots for each muon collection and the sum of them.
//Written by Jet Goodson, jgoodson@cern.ch
//Last modified 16 March, 2009
/////////////////////////////////////

void muonDistributions()
{
  gStyle->SetOptStat(0);
 

  //////////////////////Things to change based on file/input
  
  const int numberInputFiles = 2; //modify to number of samples (e.g., ttbar, Zmumu, J6, etc) Use 1 for no comparison.

  
  string inputFileNames[numberInputFiles] = {"../run/METPerformance.root", "../run/METoldPerformance.root"}; //names of the input files. Run them through merge.sh, only handles 1 file for each type.
  string inputSamples[numberInputFiles]= {"Zmumu","Zmumu, relaxed"}; //names of the input files as you want them to appear on the legends.
  Double_t inputCrossSections[numberInputFiles] = {1,1}; //used for scaling.
  Int_t inputColors[numberInputFiles] = {2,4}; //colors you want the graphs drawn in.
  Int_t inputLineWidth[numberInputFiles] = {3,3}; //affects visibility.
  Int_t inputLineStyle[numberInputFiles] = {1, 1}; //if one wants to make plots that are discernable in black and white.
  TFile *inputFile[numberInputFiles]; //holds the input files.

  TFile *outputFile = new TFile("muonDistributions.root", "RECREATE"); //where the resulting graphs/plots are saved.

//////////////////////////////////////


  string muonType[4] = {"MuonBoy", "Mutag", "CaloMuon", "All_Muons"}; //This shouldn't change unless new collections are added to the MET algorithm.
  string muonTypeTwo[4] = {"MuonBoy", "Mutag", "CaloMuons", "All_Muons"};
  Double_t scaleSize[numberInputFiles]; //if there are different number of entries in each fileset, this will be used to scale the bigger to the smaller.
  Int_t minEntries = 10000000000000;  //doubt one would have any datasets with much more events, but if so, make it bigger, maybe even a long.
  Int_t minLuminosity = 1000000000;
  Double_t luminosity = 0; //placeholder for integrated luminosity calc for scaling.
  TH1D *h_MET_x[numberInputFiles]; //only used for scaling.

  TH2D *h_MC_2D_truthMatched_IsolationVsPt_LogLog[4][numberInputFiles];  //Truth matched distributions are muons that were within dR < .02 of a truth muon (MC)
  TH2D *h_MC_2D_truthMatched_IsolationVsPt[4][numberInputFiles];
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPt_LogLog[4][numberInputFiles]; //truth unmatched were not dR < .02 of a truth
  TH2D *h_MC_2D_truthUnmatched_IsolationVsPt[4][numberInputFiles];
  TH2D *h_2D_jetMatched_IsolationVsPt_LogLog[4][numberInputFiles]; //With dR < .2 of a jet
  TH2D *h_2D_jetMatched_IsolationVsPt[4][numberInputFiles];
  
  
  TH2D *h_MC_2D_phiVsEta_truthMatched[4][numberInputFiles];
  TH2D *h_MC_2D_phiVsEta_truthUnmatched[4][numberInputFiles];
  TH2D *h_2D_phiVsEta_jetMatched[4][numberInputFiles];
  
  TH2D *h_2D_phiVsEta_all[4][numberInputFiles];


  TH1D *h_eta_dist_all[4][numberInputFiles]; //All reconstructed muons from the following collection, Muonboy, Mutag, CaloMuon, sum of all three
  TH1D *h_phi_dist_all[4][numberInputFiles];
  TH1D *h_Pt_dist_all[4][numberInputFiles];
  
  TH1D *h_MC_eta_dist_truthMatched[4][numberInputFiles];
  TH1D *h_MC_phi_dist_truthMatched[4][numberInputFiles];
  TH1D *h_MC_Pt_dist_truthMatched[4][numberInputFiles];
  
  TH1D *h_MC_eta_dist_truthUnmatched[4][numberInputFiles];
  TH1D *h_MC_phi_dist_truthUnmatched[4][numberInputFiles];
  TH1D *h_MC_Pt_dist_truthUnmatched[4][numberInputFiles];
  
  TH1D *h_eta_dist_jetMatched[4][numberInputFiles];
  TH1D *h_phi_dist_jetMatched[4][numberInputFiles];
  TH1D *h_Pt_dist_jetMatched[4][numberInputFiles];
  

  TH1D *h_MC_eta_dist_Truth[numberInputFiles]; //distributions of truth muons, for purity and efficiency plotting purproses. 
  TH1D *h_MC_phi_dist_Truth[numberInputFiles];
  TH1D *h_MC_Pt_dist_Truth[numberInputFiles];
  TH2D *h_MC_2D_phiVsEta_Truth[numberInputFiles];

  TH1D *h_eta_dist_Jets[numberInputFiles]; //distributions of jets for fake rate plotting purposes
  TH1D *h_phi_dist_Jets[numberInputFiles];
  TH1D *h_Pt_dist_Jets[numberInputFiles];
  TH2D *h_2D_phiVsEta_Jets[numberInputFiles];

  Int_t etaMax[4] = {0,0,0,0}; //these hold max values for each muon collection, in order to scale the histograms and not get cut off tops in the plots.
  Int_t phiMax[4] = {0,0,0,0};
  Int_t pTMax[4] = {0,0,0,0};


  string name; //holds the name of things in a string for retrieval.

  string histoNames[22] = {"/MC/TruthMatched/h_MC_2D_truthMatched_IsolationVsPt_LogLog",
			   "/MC/TruthMatched/h_MC_2D_truthMatched_IsolationVsPt",
			   "/MC/TruthUnmatched/h_MC_2D_truthUnmatched_IsolationVsPt_LogLog",
			   "/MC/TruthUnmatched/h_MC_2D_truthUnmatched_IsolationVsPt",
			   "/JetMatched/h_2D_jetMatched_IsolationVsPt_LogLog",
			   "/JetMatched/h_2D_jetMatched_IsolationVsPt",
			   "/MC/TruthMatched/h_MC_2D_phiVsEta_truthMatched",
			   "/MC/TruthUnmatched/h_MC_2D_phiVsEta_truthUnmatched",
			   "/JetMatched/h_2D_phiVsEta_jetMatched",
			   "/h_2D_phiVsEta_all",
			   "/h_eta_dist_all",
			   "/h_phi_dist_all",
			   "/h_Pt_dist_all",
			   "/MC/TruthMatched/h_MC_eta_dist_truthMatched",
			   "/MC/TruthMatched/h_MC_phi_dist_truthMatched",
			   "/MC/TruthMatched/h_MC_Pt_dist_truthMatched",
			   "/MC/TruthUnmatched/h_MC_eta_dist_truthUnmatched",
			   "/MC/TruthUnmatched/h_MC_phi_dist_truthUnmatched",
			   "/MC/TruthUnmatched/h_MC_Pt_dist_truthUnmatched",
			   "/JetMatched/h_eta_dist_jetMatched",
			   "/JetMatched/h_phi_dist_jetMatched",
			   "/JetMatched/h_Pt_dist_jetMatched"}; //names of all the histograms being retrieved.
  
  Int_t highBin = 0; //for now this assumes the same number of bins for each plot and just gets the value from a representative one. Have to make more refined if that is no longer true.
  Int_t lowBin = 0;
  Int_t nBins = 0;


  Double_t _pi = 2*acos(0); //Pi, just for convenience.

  cout << "Getting histograms." << endl; //so you know.
  Int_t k = 0;
  for(Int_t i = 0; i < numberInputFiles; ++i) //loops through each of the datasets. 
    { 
      inputFile[i] = new TFile(inputFileNames[i].c_str()); //opening input dataset.

      name = "Resolution/h_RefFinalResolution_x";
      h_MET_x[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      if(h_MET_x[i]->GetEntries() < minEntries) minEntries = h_MET_x[i]->GetEntries(); //Retrieved in order to scale the datasets, since the MET is a value for each event.
      luminosity = h_MET_x[i]->GetEntries()/inputCrossSections[i];
      if(luminosity < minLuminosity) minLuminosity = luminosity; //will scale to sample with minimum luminosity

      for(Int_t j = 0; j < 3; ++j) //looping over each muon collection.
	{
	  k = 0; //in order to iterate through the histogram names.
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
 	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i]; //changing name of histogram to differentiate between input types.
 	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	  k++;
	  
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  h_MC_2D_truthMatched_IsolationVsPt[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_2D_truthUnmatched_IsolationVsPt_LogLog[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_2D_truthUnmatched_IsolationVsPt[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_2D_jetMatched_IsolationVsPt_LogLog[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_2D_jetMatched_IsolationVsPt[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	  k++;
	  
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_2D_phiVsEta_truthMatched[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_2D_phiVsEta_truthUnmatched[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_2D_phiVsEta_jetMatched[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_2D_phiVsEta_all[j][i] = ((TH2D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	   h_eta_dist_all[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	  k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_phi_dist_all[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_Pt_dist_all[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_eta_dist_truthMatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_phi_dist_truthMatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_Pt_dist_truthMatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  h_MC_eta_dist_truthUnmatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_phi_dist_truthUnmatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_MC_Pt_dist_truthUnmatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_eta_dist_jetMatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_phi_dist_jetMatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	 

	  name = "Muons/" + muonType[j] + histoNames[k] + "_" + muonTypeTwo[j];
	  
	  h_Pt_dist_jetMatched[j][i] = ((TH1D)inputFile[i]->Get(name.c_str()));
	  name  = "Muons/" + muonType[j] + histoNames[k] + inputSamples[i];
	  h_MC_2D_truthMatched_IsolationVsPt_LogLog[j][i]->SetName(name.c_str());
	 k++;
	}//end of muon type loop.

      name = "Muons/Truth/h_MC_eta_dist_Truth";
      h_MC_eta_dist_Truth[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      name  = "Muons/Truth/h_MC_eta_dist_Truth" + inputSamples[i];
      h_MC_eta_dist_Truth[i]->SetName(name.c_str());
      name = "Muons/Truth/h_MC_phi_dist_Truth";
      h_MC_phi_dist_Truth[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      name  = "Muons/Truth/h_MC_eta_dist_Truth" + inputSamples[i];
      h_MC_phi_dist_Truth[i]->SetName(name.c_str());
      name = "Muons/Truth/h_MC_Pt_dist_Truth";
      h_MC_Pt_dist_Truth[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      name  = "Muons/Truth/h_MC_eta_dist_Truth" + inputSamples[i];
      h_MC_Pt_dist_Truth[i]->SetName(name.c_str());
      name = "Muons/Truth/h_MC_2D_phiVsEta_Truth";
      h_MC_2D_phiVsEta_Truth[i] = ((TH2D)inputFile[i]->Get(name.c_str()));
      name  = "Muons/Truth/h_MC_eta_dist_Truth" + inputSamples[i];
      h_MC_2D_phiVsEta_Truth[i]->SetName(name.c_str());



      name = "Jets/Cone4Topo/h_eta_dist_Jets";
      h_eta_dist_Jets[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      name = "Jets/Cone4Topo/h_eta_dist_Jets" + inputSamples[i];
      h_eta_dist_Jets[i]->SetName(name.c_str());
      name = "Jets/Cone4Topo/h_phi_dist_Jets";
      h_phi_dist_Jets[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      name = "Jets/Cone4Topo/h_phi_dist_Jets" + inputSamples[i];
      h_phi_dist_Jets[i]->SetName(name.c_str());
      name = "Jets/Cone4Topo/h_Pt_dist_Jets";
      h_Pt_dist_Jets[i] = ((TH1D)inputFile[i]->Get(name.c_str()));
      name = "Jets/Cone4Topo/h_Pt_dist_Jets" + inputSamples[i];
      h_Pt_dist_Jets[i]->SetName(name.c_str());
      name = "Jets/Cone4Topo/h_2D_phiVsEta_Jets";
      h_2D_phiVsEta_Jets[i] = ((TH2D)inputFile[i]->Get(name.c_str()));
      name = "Jets/Cone4Topo/h_2D_phiVsEta_Jets" + inputSamples[i];
      h_2D_phiVsEta_Jets[i]->SetName(name.c_str());



      nBins = h_eta_dist_all[0][i]->GetNbinsX(); //Getting nbins for fitting purposes.

      k = 0; //Making histograms for adding up muon collections.
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_eta_dist_all[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, -3.0, 3.0);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_phi_dist_all[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, -1*_pi, _pi);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_Pt_dist_all[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, 0.0, 200.0);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_MC_eta_dist_truthMatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins,  -3.0, 3.0);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_MC_phi_dist_truthMatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins,  -1*_pi, _pi);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_MC_Pt_dist_truthMatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, 0.0, 200.0);
      
      
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_MC_eta_dist_truthUnmatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins,  -3.0, 3.0);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_MC_phi_dist_truthUnmatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins,  -1*_pi, _pi);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_MC_Pt_dist_truthUnmatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, 0.0, 200.0);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_eta_dist_jetMatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, -3.0, 3.0);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_phi_dist_jetMatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins,  -1*_pi, _pi);
      name = histoNames[k] + "AllMuons" + inputSamples[i];
      k++;
      h_Pt_dist_jetMatched[3][i] = new TH1D(name.c_str(), name.c_str(),nBins, 0.0, 200.0); 
      
      for(int z = 0; z < 3; z++) //Adding up muon collections into the 4th histogram.
	{
	  h_eta_dist_all[3][i]->Add(h_eta_dist_all[z][i], 1.0);
	  h_phi_dist_all[3][i]->Add(h_phi_dist_all[z][i], 1.0);
	  h_Pt_dist_all[3][i]->Add(h_Pt_dist_all[z][i], 1.0);
	  
	  h_MC_eta_dist_truthMatched[3][i]->Add(h_MC_eta_dist_truthMatched[z][i], 1.0);
	  h_MC_phi_dist_truthMatched[3][i]->Add(h_MC_phi_dist_truthMatched[z][i], 1.0);
	  h_MC_Pt_dist_truthMatched[3][i]->Add(h_MC_Pt_dist_truthMatched[z][i], 1.0);
	  
	  h_MC_eta_dist_truthUnmatched[3][i]->Add(h_MC_eta_dist_truthUnmatched[z][i], 1.0);
	  h_MC_phi_dist_truthUnmatched[3][i]->Add(h_MC_phi_dist_truthUnmatched[z][i], 1.0);
	  h_MC_Pt_dist_truthUnmatched[3][i]->Add(h_MC_Pt_dist_truthUnmatched[z][i], 1.0);
	  
	  h_eta_dist_jetMatched[3][i]->Add(h_eta_dist_jetMatched[z][i], 1.0);
	  h_phi_dist_jetMatched[3][i]->Add(h_phi_dist_jetMatched[z][i], 1.0);
	  h_Pt_dist_jetMatched[3][i]->Add(h_Pt_dist_jetMatched[z][i], 1.0);

	}
      

      //inputFile[i]->Close();  //was closing this here, but it went wild every time I tried.

    }//end of input file loop.

  cout << "Histograms have been secured. Preparing for analysis!" << endl;

  outputFile->cd(); //changing to write to the outfile. 

 

 

  string loopControl = "AP"; //used to determine if graphs should be drawn "AP" or "SAME"

 
  TGraph *purityEta[numberInputFiles][4]; //graphs for holding purity, efficiency, and fake rate.
  TGraph *fakeRateEta[numberInputFiles][4];
  TGraph *efficiencyEta[numberInputFiles][4];
  
  TGraph *purityPhi[numberInputFiles][4];
  TGraph *fakeRatePhi[numberInputFiles][4];
  TGraph *efficiencyPhi[numberInputFiles][4];
  
  TGraph *purityPt[numberInputFiles][4];
  TGraph *fakeRatePt[numberInputFiles][4];
  TGraph *efficiencyPt[numberInputFiles][4];
  
  
  TCanvas *muonTypeCanvasPurityEta[4]; //tcanvases for the graphs, since one can't write a graph to a root file. 
  TCanvas *muonTypeCanvasPurityPhi[4];
  TCanvas *muonTypeCanvasPurityPt[4];

  TCanvas *muonTypeCanvasEfficiencyEta[4];
  TCanvas *muonTypeCanvasEfficiencyPhi[4];
  TCanvas *muonTypeCanvasEfficiencyPt[4];
  
  TCanvas *muonTypeCanvasFakeRateEta[4];
  TCanvas *muonTypeCanvasFakeRatePhi[4];
  TCanvas *muonTypeCanvasFakeRatePt[4];


  TLegend *legendMuonTypePurityEta[4]; //legends for each measure and muon.
  TLegend *legendMuonTypePurityPhi[4];
  TLegend *legendMuonTypePurityPt[4];

  TLegend *legendMuonTypeEfficiencyEta[4];
  TLegend *legendMuonTypeEfficiencyPhi[4];
  TLegend *legendMuonTypeEfficiencyPt[4];
  
  TLegend *legendMuonTypeFakeRateEta[4];
  TLegend *legendMuonTypeFakeRatePhi[4];
  TLegend *legendMuonTypeFakeRatePt[4];

  cout << "Making distributions." << endl;

  for(Int_t ix = 0; ix < numberInputFiles; ++ix)
    {
      scaleSize[ix] = 1.0; //calculating the scale size for the distributions,so we can compare datsets with different number of events.
      //if(h_MET_x[ix]->GetEntries() != 0) scaleSize[ix] = static_cast<Double_t>(minEntries)/static_cast<Double_t>(abs(h_MET_x[ix]->GetEntries()));
      if(h_MET_x[ix]->GetEntries() != 0) scaleSize[ix] = static_cast<Double_t>(minLuminosity)/static_cast<Double_t>(abs(h_MET_x[ix]->GetEntries()/inputCrossSections[ix]));

      for(Int_t iy = 0; iy < 4; ++iy)
	{

	  
	  
	  purityEta[ix][iy] = grapher(h_MC_eta_dist_truthMatched[iy][ix], h_MC_eta_dist_truthUnmatched[iy][ix], "#eta_{#mu}", "Purity_{#mu}", 1, inputColors[ix]);
	  efficiencyEta[ix][iy] = grapher(h_MC_eta_dist_truthMatched[iy][ix], h_MC_eta_dist_Truth[ix], "#eta_{#mu}", "#epsilon_{#mu}", 0, inputColors[ix]);
	  fakeRateEta[ix][iy] = grapher(h_eta_dist_jetMatched[iy][ix], h_eta_dist_Jets[ix], "#eta_{Jet}", "Fake Rate_{#mu}", 0, inputColors[ix]);
	  
	  purityPhi[ix][iy] = grapher(h_MC_phi_dist_truthMatched[iy][ix], h_MC_phi_dist_truthUnmatched[iy][ix], "#phi_{#mu}", "Purity_{#mu}", 1, inputColors[ix]);
	  efficiencyPhi[ix][iy] = grapher(h_MC_phi_dist_truthMatched[iy][ix], h_MC_phi_dist_Truth[ix], "#phi_{#mu}", "#epsilon_{#mu}", 0, inputColors[ix]);
	  fakeRatePhi[ix][iy] = grapher(h_phi_dist_jetMatched[iy][ix], h_phi_dist_Jets[ix], "#phi_{Jet}", "Fake Rate_{#mu}", 0, inputColors[ix]);
	 
	  purityPt[ix][iy] = grapher(h_MC_Pt_dist_truthMatched[iy][ix], h_MC_Pt_dist_truthUnmatched[iy][ix], "P_{T, #mu}", "Purity_{#mu}", 1, inputColors[ix]);
	  efficiencyPt[ix][iy] = grapher(h_MC_Pt_dist_truthMatched[iy][ix], h_MC_Pt_dist_Truth[ix], "P_{T, #mu}", "#epsilon_{#mu}", 0, inputColors[ix]);
	  fakeRatePt[ix][iy] = grapher(h_Pt_dist_jetMatched[iy][ix], h_Pt_dist_Jets[ix], "P_{T, Jet}", "Fake Rate_{#mu}", 0, inputColors[ix]);


	  loopControl = "SAME"; //will be changed only for ix == 0 loop.
	  if(ix == 0) //setting up canvases and legends.
	    {

	      loopControl = "AP";
	      name = "canvas" + muonType[iy] + "PurityEta";
	      muonTypeCanvasPurityEta[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "PurityPhi";
	      muonTypeCanvasPurityPhi[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "PurityPt";
	      muonTypeCanvasPurityPt[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "EfficiencyEta";
	      muonTypeCanvasEfficiencyEta[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "EfficiencyPhi";
	      muonTypeCanvasEfficiencyPhi[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "EfficiencyPt";
	      muonTypeCanvasEfficiencyPt[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "FakeRateEta";
	      muonTypeCanvasFakeRateEta[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "FakeRatePhi";
	      muonTypeCanvasFakeRatePhi[iy] = canvaser(name, 800, 600);
	      name = "canvas" + muonType[iy] + "FakeRatePt";
	      muonTypeCanvasFakeRatePt[iy] = canvaser(name, 800, 600);

	      name = "Samples";
	      legendMuonTypePurityEta[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      legendMuonTypePurityPhi[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      legendMuonTypePurityPt[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      
	      legendMuonTypeEfficiencyEta[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      legendMuonTypeEfficiencyPhi[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      legendMuonTypeEfficiencyPt[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      
	      legendMuonTypeFakeRateEta[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      legendMuonTypeFakeRatePhi[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      legendMuonTypeFakeRatePt[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
	      


	    }//end of ix == 0 if-statement


	  legendMuonTypePurityEta[iy]->AddEntry(purityEta[ix][iy], inputSamples[ix].c_str(), "l"); //adding entries to legends.
	  legendMuonTypePurityPhi[iy]->AddEntry(purityPhi[ix][iy], inputSamples[ix].c_str(), "l");
	  legendMuonTypePurityPt[iy]->AddEntry(purityPt[ix][iy], inputSamples[ix].c_str(), "l");

	  legendMuonTypeEfficiencyEta[iy]->AddEntry(efficiencyEta[ix][iy], inputSamples[ix].c_str(), "l");
	  legendMuonTypeEfficiencyPhi[iy]->AddEntry(efficiencyPhi[ix][iy], inputSamples[ix].c_str(), "l");
	  legendMuonTypeEfficiencyPt[iy]->AddEntry(efficiencyPt[ix][iy], inputSamples[ix].c_str(), "l");


	  legendMuonTypeFakeRateEta[iy]->AddEntry(fakeRateEta[ix][iy], inputSamples[ix].c_str(), "l");
	  legendMuonTypeFakeRatePhi[iy]->AddEntry(fakeRatePhi[ix][iy], inputSamples[ix].c_str(), "l");
	  legendMuonTypeFakeRatePt[iy]->AddEntry(fakeRatePt[ix][iy], inputSamples[ix].c_str(), "l");

	  muonTypeCanvasPurityEta[iy]->cd();
	  purityEta[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypePurityEta[iy]->Draw("SAME");
	  muonTypeCanvasPurityPhi[iy]->cd();
	  purityPhi[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypePurityPhi[iy]->Draw("SAME");
	  muonTypeCanvasPurityPt[iy]->cd();
	  purityPt[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypePurityPt[iy]->Draw("SAME");
	  
	  muonTypeCanvasEfficiencyEta[iy]->cd();
	  efficiencyEta[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypeEfficiencyEta[iy]->Draw("SAME");
	  muonTypeCanvasEfficiencyPhi[iy]->cd();
	  efficiencyPhi[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypeEfficiencyPhi[iy]->Draw("SAME");
	  muonTypeCanvasEfficiencyPt[iy]->cd();
	  efficiencyPt[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypeEfficiencyPt[iy]->Draw("SAME");
	  
	  muonTypeCanvasFakeRateEta[iy]->cd();
	  fakeRateEta[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypeFakeRateEta[iy]->Draw("SAME");
	  muonTypeCanvasFakeRatePhi[iy]->cd();
	  fakeRatePhi[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypeFakeRatePhi[iy]->Draw("SAME");
	  muonTypeCanvasFakeRatePt[iy]->cd();
	  fakeRatePt[ix][iy]->Draw(loopControl.c_str());
	  legendMuonTypeFakeRatePt[iy]->Draw("SAME");
	  
	  if(ix == numberInputFiles - 1)
	    {
	      muonTypeCanvasPurityEta[iy]->Write();	    
	      muonTypeCanvasPurityPhi[iy]->Write();	    
	      muonTypeCanvasPurityPt[iy]->Write();
	      muonTypeCanvasEfficiencyEta[iy]->Write();
	      muonTypeCanvasEfficiencyPhi[iy]->Write();
	      muonTypeCanvasEfficiencyPt[iy]->Write();
	      muonTypeCanvasFakeRateEta[iy]->Write();
	      muonTypeCanvasFakeRatePhi[iy]->Write();
	      muonTypeCanvasFakeRatePt[iy]->Write();
	      
	    }
	  
	  //Finding max value for each distribution histogram.
	  h_Pt_dist_all[iy][ix]->Scale(scaleSize[ix]); //scaling distribution size. unnecessary for purity, fake rate, and efficiency since they're ratios.
	  h_eta_dist_all[iy][ix]->Scale(scaleSize[ix]);
	  h_phi_dist_all[iy][ix]->Scale(scaleSize[ix]);

	  if(h_Pt_dist_all[iy][ix]->GetMaximum() > pTMax[iy]) pTMax[iy] = h_Pt_dist_all[iy][ix]->GetMaximum();
	  if(h_eta_dist_all[iy][ix]->GetMaximum() > etaMax[iy]) etaMax[iy] = h_eta_dist_all[iy][ix]->GetMaximum();
	  if(h_phi_dist_all[iy][ix]->GetMaximum() > phiMax[iy]) phiMax[iy] = h_phi_dist_all[iy][ix]->GetMaximum();
	 
	}//end of iy loop
    }//end of ix loop


  TLegend *pTLegend[4]; //legends and canvasses for the muon distributions.
  TLegend *etaLegend[4];
  TLegend *phiLegend[4];

  TCanvas *pTCanvas[4];
  TCanvas *etaCanvas[4];
  TCanvas *phiCanvas[4];

  for(iy = 0; iy < 4; ++iy)
    {
      name = muonTypeTwo[iy] + " P_{T}";
      pTLegend[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
      name = muonTypeTwo[iy] + " #eta";
      etaLegend[iy] = new TLegend(.6,.6,.98,.98, name.c_str());
      name = muonTypeTwo[iy] + " #phi";
      phiLegend[iy] = new TLegend(.6,.6,.98,.98, name.c_str());

      name = muonTypeTwo[iy] + "PtDistribution";
      pTCanvas[iy] = canvaser(name, 800, 600);
      name = muonTypeTwo[iy] + "EtaDistribution";
      etaCanvas[iy] = canvaser(name, 800, 600);
      name = muonTypeTwo[iy] + "PhiDistribution";
      phiCanvas[iy] = canvaser(name, 800, 600);

      for(ix = 0; ix < numberInputFiles; ++ix)
	{
	 
	  histogrammar(h_Pt_dist_all[iy][ix], inputColors[ix], "P_{T}", "#", pTMax[iy], inputLineWidth[i], inputLineStyle[i]); //sets up histograms nicely.
	  histogrammar(h_eta_dist_all[iy][ix], inputColors[ix], "#eta", "#", etaMax[iy], inputLineWidth[i], inputLineStyle[i]);
	  histogrammar(h_phi_dist_all[iy][ix], inputColors[ix], "#phi", "#", phiMax[iy], inputLineWidth[i], inputLineStyle[i]);
 

	  pTLegend[iy]->AddEntry(h_Pt_dist_all[iy][ix], inputSamples[ix].c_str(), "l");
	  etaLegend[iy]->AddEntry(h_eta_dist_all[iy][ix], inputSamples[ix].c_str(), "l");
	  phiLegend[iy]->AddEntry(h_phi_dist_all[iy][ix], inputSamples[ix].c_str(), "l");

	  pTCanvas[iy]->cd();
	  loopControl = "SAME";
	  if(ix == 0) loopControl = "";
	   
	  h_Pt_dist_all[iy][ix]->Draw(loopControl.c_str());
	  etaCanvas[iy]->cd();
	  h_eta_dist_all[iy][ix]->Draw(loopControl.c_str());
	  phiCanvas[iy]->cd();
	  h_phi_dist_all[iy][ix]->Draw(loopControl.c_str());
	   
	}//end of muon type loop

      pTCanvas[iy]->cd();
      pTLegend[iy]->Draw("SAME");
      pTCanvas[iy]->Write();
      
      etaCanvas[iy]->cd();
      etaLegend[iy]->Draw("SAME");
      etaCanvas[iy]->Write();
      
      phiCanvas[iy]->cd();
      phiLegend[iy]->Draw("SAME");
      phiCanvas[iy]->Write();
      
      
    }//end of ix loop//input files






  outputFile->Write();
  outputFile->Close();
}




double simpleDivisionError(double x, double y) //function to do error for division of type x/y, unless y is zero, then it returns zero.
{
  
  
  if(y == 0)
    {
      return 0;
    }
  else
    {
      double dx = sqrt(x);
      double dy = sqrt(y);
      double deltaX = pow(dx/y,2);
      double deltaY = pow((x*dy)/(y*y), 2);
      return sqrt(deltaX + deltaY);
    }
}

double purityError(double x, double y) //function to do the error for division of type x/(x+y), unless x+y = 0, which returns 0.
{
  
  if(x + y == 0)
    {
      return 0;
    }
  else
    {
      double dx = sqrt(x);
      double dy = sqrt(y);
      double deltaX = y*dx/pow(x+y,2);
      double deltaY = x*dy/pow(x+y,2);
      return sqrt(pow(deltaX,2) + pow(deltaY,2));
    }                
}

double conditionalDivision(double x, double y) //function to do division of type x/y, unless y is zero, then it returns zero.
{
  if(y == 0)
    {
      return 0;
    }
  else
    {
      return x/y;
    }
}

double conditionalDivisionPurity(double x, double y)//function to do the division of type x/(x+y), unless x+y = 0, which returns 0.
{
  if(x + y == 0)
    {
      return 0;
    }
  else
    {
      return x/(x +y);
    }
}


double tails(TH1D *histogramOne, TH1D *histogramTwo, int bins, int low, int high) //calculates tails of MET_true - MET_reco distributions.
{
  TF1 *fit1 = new TF1("fitted1", "gaus", -1.0*histogramOne->GetRMS(), 1.0*histogramOne->GetRMS());
  TAxis *axis1 = histogramOne->GetXaxis();
  histogramOne->Fit("fitted1","RNQ");

  TF1 *fit2 = new TF1("fitted2", "gaus", -1.0*histogramTwo->GetRMS(), 1.0*histogramTwo->GetRMS());
  histogramTwo->Fit("fitted2","RNQ");
  if(histogramOne->GetEntries() > 0)
    {
  return (histogramOne->Integral(low, axis1->FindBin(fit1->GetParameter(1) - 3.0*fit2->GetParameter(2))) + histogramOne->Integral(axis1->FindBin(fit1->GetParameter(1) + 3.0*fit2->GetParameter(2)), high))/histogramOne->GetEntries();
    }
  else if
    {
      return -1;
    }
}


double resolution(TH1D *histogram, int bin, int low, int highs) //calculates resoltuion of MET_true - MET_reco  distributions.
{
  TF1 *fit = new TF1("fitted", "gaus", -1.0*histogram->GetRMS(), 1.0*histogram->GetRMS());
  TAxis *axis = histogram->GetXaxis();
  histogram->Fit("fitted","RNQ");
  return fit->GetParameter(2);
}




TCanvas *canvaser(string name, int ex, int why) //sets up a canvas with the options I prefer.
{
  TCanvas *canvas = new TCanvas(name.c_str(), name.c_str(), 0,0,ex,why);
  canvas->SetFillColor(0);
  canvas->GetFrame()->SetFillColor(21);
  canvas->GetFrame()->SetBorderMode(-1);

  return canvas;
}

TH1 histogrammar(TH1D *histogram, int color, string exAxis, string whyAxis, int maximum, int lineWidth, int lineStyle) //sets up a histogram like I want.
{
  //int maximum = 0;
  histogram->SetLineWidth(3);
  histogram->SetLineColor(color);
  histogram->SetLineStyle(lineStyle);
  histogram->SetLineWidth(lineWidth);
  histogram->GetXaxis()->SetTitle(exAxis.c_str());
  histogram->GetYaxis()->SetTitle(whyAxis.c_str());
  histogram->GetYaxis()->SetTitleOffset(1.3);
  //maximum = histogram->GetMaximum();
  histogram->SetMaximum(1.5*maximum + 1);
  return histogram;
}


TGraph *grapher(TH1D *numerator, TH1D *denominator, string exAxis, string whyAxis, int operation, int color) //makes graphs of purity, efficiency, or fake rate from given histos
{

  Int_t binCount = numerator->GetNbinsX();
  double binWidth = numerator->GetBinWidth(binCount/2);
  double binStart = numerator->GetXaxis()->GetXmin();
  double test = denominator->GetXaxis()->GetXmin();

  Double_t iteration[10000];
  Double_t iterationError[10000];

  Double_t value[10000];
  Double_t valueError[10000];
  Int_t nonZeroCount = 0; 


  for(unsigned int i = 0; i < binCount; ++i)
    {
     
      if(operation == 0 && numerator->GetBinContent(i) != 0 && denominator->GetBinContent(i) != 0)
	{	  
	  iteration[nonZeroCount] = numerator->GetBinLowEdge(i) + 1.5*numerator->GetBinWidth(i);
	  iterationError[nonZeroCount] = .5*numerator->GetBinWidth(i);
     

 	  value[nonZeroCount] = conditionalDivision(static_cast<double>(numerator->GetBinContent(i)), static_cast<double>(denominator->GetBinContent(i)));
	  valueError[nonZeroCount] = simpleDivisionError(static_cast<double>(numerator->GetBinContent(i)), static_cast<double>(denominator->GetBinContent(i)));
	  nonZeroCount++;
	}
      else if(operation == 1 && numerator->GetBinContent(i) != 0 && denominator->GetBinContent(i) != 0)
	{
	  iteration[nonZeroCount] = numerator->GetBinLowEdge(i) + 1.5*numerator->GetBinWidth(i);
	  iterationError[nonZeroCount] = .5*numerator->GetBinWidth(i);
     

	  value[nonZeroCount] = conditionalDivisionPurity(static_cast<double>(numerator->GetBinContent(i)), static_cast<double>(denominator->GetBinContent(i)));
	  valueError[nonZeroCount] = purityError(static_cast<double>(numerator->GetBinContent(i)), static_cast<double>(denominator->GetBinContent(i)));
	  nonZeroCount++;
	}
      
    }
 
  TGraph *graph = new TGraphErrors(nonZeroCount, iteration, value, iterationError, valueError);
  graph->GetXaxis()->SetTitle(exAxis.c_str());
  graph->GetYaxis()->SetTitleOffset(1.3);
  graph->GetYaxis()->SetTitle(whyAxis.c_str());
  graph->SetMarkerStyle(7);
  graph->SetMarkerColor(color);
  graph->SetLineColor(color);
  return graph;
}

TLegend *legendary(TLegend *legend, TH1D *histogram, int binCount, int lowBin, int maxBin, char variable[100]) //automatically sets up legends for resolution histograms.
{
  char c_words[100] = "blargh";
  char c_variable[100] = "yarg";
  strcpy(c_variable, variable);
  sprintf(c_words, " #sigma = %03f", resolution(histogram, binCount, lowBin, maxBin));
  strcat(c_variable, c_words);
  legend->AddEntry(histogram, c_variable, "l");
  strcpy(c_variable, variable);
  sprintf(c_words, " Tails = %03f", tails(histogram, histogram, binCount, lowBin, maxBin));
  strcat(c_variable, c_words);
  legend->AddEntry(histogram, c_variable, "l");
  return legend;

}

