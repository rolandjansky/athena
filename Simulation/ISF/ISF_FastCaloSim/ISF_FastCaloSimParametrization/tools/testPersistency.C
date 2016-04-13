/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  if(1==0)
  {
    // Needs athena environment setup and ISF_FastCaloSimParametrization package compiled.
    // Uses the root interface library created in the compilation of ISF_FastCaloSimParametrization
    gSystem->Load("libISF_FastCaloSimParametrizationLib.so");
  }
  else
  {
    gSystem->AddIncludePath(" -I.. ");

    gROOT->LoadMacro("../Root/myMsgLogger.cxx+");
    gROOT->LoadMacro("../ISF_FastCaloSimParametrization/MeanAndRMS.h+");
/*
    gROOT->LoadMacro("../Root/TFCSExtrapolationState.cxx+");
    gROOT->LoadMacro("../Root/TFCSTruthState.cxx+");
    gROOT->LoadMacro("../Root/TFCSSimulationState.cxx+");
    gROOT->LoadMacro("../Root/TFCSParametrizationBase.cxx+");
    gROOT->LoadMacro("../Root/TFCSParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSEnergyParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSPCAEnergyParametrization.cxx+");
//    gROOT->LoadMacro("../Root/TFCS1DFunctionHistogram.cxx+");
  //  gROOT->LoadMacro("../Root/TFCS1DFunctionRegression.cxx+");
    gROOT->LoadMacro("../Root/TFCS1DFunction.cxx+");
    gROOT->LoadMacro("../Root/TFCSFunction.cxx+");
    gROOT->LoadMacro("../Root/TFCSLateralShapeParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSNNLateralShapeParametrization.cxx+");
    gROOT->LoadMacro("../Root/TFCSSimpleLateralShapeParametrization.cxx+");
*/
    gROOT->LoadMacro("../Root/TFCS1DFunction.cxx+");
    gROOT->LoadMacro("../Root/TFCS1DFunctionRegression.cxx+");
    gROOT->LoadMacro("../Root/TFCS1DFunctionHistogram.cxx+");
    gROOT->LoadMacro("../Root/TFCSFunction.cxx+");
  }
  
  //test the TFCSFunction:
  
  //that one works well:
  /*
  TFile *input=TFile::Open("PCA2_bin0_pions_new_2dbin_10bins.root");
  TH1D* hist=(TH1D*)input->Get("h_cumulative_total")); hist->SetName("hist");
  
  //that one crashes:
  //TFile *input=TFile::Open("PCA2_bin8_pions_calo_10bins.root");
  //TH1D* hist=(TH1D*)input->Get("h_cumulative_PCA_3")); hist->SetName("hist");
  
  TFCS1DFunction* fct=TFCSFunction::Create(hist);
  cout<<fct<<endl;
    
  //save fct
  TFile* output=new TFile("test.root","RECREATE");
  output->Add(fct);
  output->Write();
  */
  
  /*
  TFile* file=TFile::Open("output/secondPCA_pions_nbins1st5_nbins2nd2.root");
  vector<int> layerNr; 
  TH2I* h_layer=(TH2I*)file->Get("h_layer");
  for(int i=1;i<=h_layer->GetNbinsY();i++)
  {
 	 if(h_layer->GetBinContent(1,i)==1) 
 	  layerNr.push_back(h_layer->GetYaxis()->GetBinCenter(i));
  }
  vector<string> layer;
  for(unsigned int l=0;l<layerNr.size();l++)
   layer.push_back(Form("layer%i",layerNr[l]));
  layer.push_back("totalE");
  int pcabins=h_layer->GetNbinsX();
  //get a random PCA bin:
  TRandom* Random=new TRandom3();
  Random->SetSeed(0);
  double uniform=Random->Uniform(1);
  int randombin=0;
  for(int n=0;n<nbins;n++)
  {
   if(uniform>n*1.0/(double)pcabins && uniform<(n+1.)*1.0/(double)pcabins)
   	randombin=n;
  }
  stringstream ss;
  ss << randombin;
  string randombinstr = ss.str();
  //for this random bin, store the info of the cumulatives:
  TIter next(file->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next()))
  {
   TDirectory* bindir=(TDirectory*)key->ReadObj();
   bindir->cd();
   string binname="bin"+randombinstr;
   if(bindir->GetName()==binname)
   {
    cdbin->cd();
    for(unsigned int l=0;l<layer.size();l++)
    {
     TH1D* hist=(TH1D*)file->Get(Form("h_cumul_%s",layer[l].c_str())); hist->SetName("hist");
     TFCS1DFunction* fct=TFCSFunction::Create(hist);
    }
   }
  }
  
  //perform the simulation:  
  TFCSPCAEnergyParametrization* etest=new TFCSPCAEnergyParametrization("etest","etest");
  etest->loadInputs(file);
  const TFCSTruthState* truth=new TFCSTruthState();
  const TFCSExtrapolationState* extrapol=new TFCSExtrapolationState();
  TFCSSimulationState simulstate;
  simulstate.set_Ebin(randombin);
  etest->simulate(simulstate, truth, extrapol);
  */
   
}

