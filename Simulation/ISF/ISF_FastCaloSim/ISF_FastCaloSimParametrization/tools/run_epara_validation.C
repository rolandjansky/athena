/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1* get_cumul(TH1* hist);
void run_epara_validation();
void check();

void ATLAS_LABEL(double x,double y) ;

//Instructions:
//.x init_epara_validation.C+
//.x run_epara_validation.C


void run_epara_validation()
{
 	
 	//parametrization input files:
 	
  //string samplename="pions";
  //string samplename="photons";
  //string samplename="pions_s2864";
  //string samplename="s2865";
  //string samplename="photons0809";
  //string samplename="pionplus0809";
  string samplename="pionminus0809";
  
 	string dirname ="/afs/cern.ch/user/s/schaarsc/public/fastcalo/epara_storage";
  
  system("mkdir eparavalidation");
  system(("mkdir eparavalidation/"+samplename).c_str());
  /*
  int setbin=-1;
  cout<<"PCA bin (-1 if random)? "<<endl;
  cin>>setbin;
  */
  //Prepare the Histograms
  cout<<"Preparing validation histograms"<<endl;
  TFile* file1=TFile::Open(Form("%s/%s/firstPCA.root",dirname.c_str(),samplename.c_str()));
  TH2I* h_layer=(TH2I*)file1->Get("h_layer");
  int pcabins=h_layer->GetNbinsX();
  vector<int> layerNr;
  for(int i=1;i<=h_layer->GetNbinsY();i++)
  {
 	 if(h_layer->GetBinContent(1,i)==1) 
 	  layerNr.push_back(h_layer->GetYaxis()->GetBinCenter(i));
  }
  vector<string> layer;
  for(unsigned int l=0;l<layerNr.size();l++)
   layer.push_back(Form("layer%i",layerNr[l]));
  layer.push_back("totalE");
  
  for(unsigned int l=0;l<layer.size();l++)
   cout<<"l "<<l<<" "<<layer[l]<<endl;
  
  int nbins=100;
  TH1D* h_input[layer.size()+2];
  TH1D* h_output[layer.size()+2];
  for(unsigned int l=0;l<layerNr.size();l++)
  {
   h_input[l]=new TH1D(Form("h_input_%s",layer[l].c_str()),Form("h_input_%s",layer[l].c_str()),nbins,0,1);
   h_output[l]=new TH1D(Form("h_output_%s",layer[l].c_str()),Form("h_output_%s",layer[l].c_str()),nbins,0,1);
  }
  //Total E
  TTree* InputTree = (TTree*)file1->Get("tree_1stPCA");
  TreeReader* read_inputTree = new TreeReader();
  read_inputTree->SetTree(InputTree);

  double minE=InputTree->GetMinimum("energy_totalE");
  double maxE=InputTree->GetMaximum("energy_totalE");
  cout<<"************************"<<endl;
  cout<<"minE "<<minE<<" maxE "<<maxE<<endl;
  h_input[layerNr.size()] =new TH1D("h_input_totalE","h_input_totalE",nbins,minE,maxE);
  h_output[layerNr.size()]=new TH1D("h_output_totalE","h_output_totalE",nbins,minE,maxE);
  //Fractions:
  h_input[layer.size()]   =new TH1D("h_input_sumfractions_elmag","h_input_sumfractions_elmag",nbins,-1,2);
  h_output[layer.size()]  =new TH1D("h_output_sumfractions_elmag","h_output_sumfractions_elmag",nbins,-1,2);
  h_input[layer.size()+1] =new TH1D("h_input_sumfractions_had","h_input_sumfractions_had",nbins,-1,2);
  h_output[layer.size()+1]=new TH1D("h_output_sumfractions_had","h_output_sumfractions_had",nbins,-1,2);
  //correlation
  TH2D* h_input_layer1_layer2;
  TH2D* h_output_layer1_layer2;
  h_input_layer1_layer2=new TH2D("h_input_layer1_layer2","h_input_layer1_layer2",100,0,1,100,0,1);
  h_output_layer1_layer2=new TH2D("h_output_layer1_layer2","h_output_layer1_layer2",100,0,1,100,0,1);
  
  //Fill the Input Histograms:
  vector<int> elmag; //0-8
  for(int e=0;e<=8;e++) elmag.push_back(e);
  vector<int> had;   //9-24
  for(int h=9;h<25;h++) had.push_back(h);
  
  cout<<"Now fill input histograms"<<endl;
  for(int event=0;event<read_inputTree->GetEntries();event++)
  {
   read_inputTree->GetEntry(event);
   //if(setbin<0 || (setbin>=1 && read_inputTree->GetVariable("firstPCAbin")==setbin))
   //{
   double sum_fraction_elmag=0.0;
   double sum_fraction_had=0.0;
   double data = read_inputTree->GetVariable("energy_totalE");
   h_input[layerNr.size()]->Fill(data);
   for(unsigned int l=0;l<layerNr.size();l++)
   {
   	double data = read_inputTree->GetVariable(Form("energy_%s",layer[l].c_str()));
   	h_input[l]->Fill(data);
   	
   	int is_elmag,is_had;
   	is_elmag=is_had=0;
    
    for(int e=0;e<elmag.size();e++)
    { 	  if(elmag[e]==layerNr[l]) is_elmag=1;   }
    for(int h=0;h<had.size();h++)
    { 	  if(had[h]==layerNr[l]) is_had=1;   }
    if(is_elmag) sum_fraction_elmag+=data;
    if(is_had)   sum_fraction_had+=data;
   }
   h_input[layerNr.size()+1]->Fill(sum_fraction_elmag);
   h_input[layerNr.size()+2]->Fill(sum_fraction_had);
   h_input_layer1_layer2->Fill(read_inputTree->GetVariable("energy_layer1"),read_inputTree->GetVariable("energy_layer2"));
  } //loop over g4
  
  TH1D* h_randombin=new TH1D("h_randombin","h_randombin",pcabins,-0.5,pcabins-0.5);
  
   
  TFCSPCAEnergyParametrization etest("etest","etest");
  //TFile* file2 = TFile::Open("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/FCSParams.root");

  TFile* file2 = TFile::Open(Form("%s/%s/secondPCA.root",dirname.c_str(),samplename.c_str()));

  cout<<"pca bins before load "<<etest.n_pcabins()<<endl;
  etest.loadInputs(file2);
  cout<<"number of pca bins after load "<<etest.n_pcabins()<<" , number from firstPCA file: "<<pcabins<<endl;
  file2->Close();
  delete file2;

  //Run the loop:
  int ntoys=2000;
  TRandom3* Random=new TRandom3();
  Random->SetSeed(0);
  const TFCSTruthState* truth=new TFCSTruthState();
  const TFCSExtrapolationState* extrapol=new TFCSExtrapolationState();

  TFCSSimulationState simulstate;
  for(int i=0;i<ntoys;i++)
  {
   if(i%100==0) cout<<"Now run simulation for Toy "<<i<<endl;
   
   int randombin=0;
   double uniform=Random->Uniform(1);
   for(int n=0;n<pcabins;n++)
   {
    if(uniform>n*1.0/(double)pcabins && uniform<(n+1.)*1.0/(double)pcabins)
     randombin=n+1;
   }
   h_randombin->Fill(randombin);
         
   simulstate.set_Ebin(randombin);
   
   etest.simulate(simulstate, truth, extrapol);
   
   //fill the Histograms:
   double sum_fraction_elmag=0.0;
   double sum_fraction_had=0.0;
   
   /*
   for(int s=0;s<30;s++) //30 is a dummy number, sth big
   {
   	int is_elmag,is_had;
   	is_elmag=is_had=0;
   	for(unsigned int l=0;l<layerNr.size();l++)
    {
     if(s==layerNr[l])
     {
   	  h_output[l]->Fill(simulstate.Efrac(s));
      for(int e=0;e<elmag.size();e++)
      { 	  if(elmag[e]==layerNr[l]) is_elmag=1;   }
      for(int h=0;h<had.size();h++)
      { 	  if(had[h]==layerNr[l]) is_had=1;   }
      if(is_elmag) sum_fraction_elmag+=simulstate.Efrac(s);
      if(is_had)   sum_fraction_had+=simulstate.Efrac(s);
     }
    }
   }
   h_output[layerNr.size()]->Fill(simulstate.E());
   h_output[layerNr.size()+1]->Fill(sum_fraction_elmag);
   h_output[layerNr.size()+2]->Fill(sum_fraction_had);
   */
   
   for(unsigned int l=0;l<layerNr.size();l++)
   {
    h_output[l]->Fill(simulstate.Efrac(layerNr[l]));
   }
   h_output[layerNr.size()]->Fill(simulstate.E());
   
   //correlation plots:
   h_output_layer1_layer2->Fill(simulstate.Efrac(1),simulstate.Efrac(2));
   
   if(simulstate.Efrac(1)>0.8)
   {
   	cout<<"in toy nr "<<i<<" efrac1 "<<simulstate.Efrac(1)<<" efrac2 "<<simulstate.Efrac(2)<<endl;
   	int enter; cin>>enter;
   }
   
  } //loop over toys
  
  //save the histograms in an output file
  TFile* outfile=new TFile(Form("eparavalidation/%s/validation.root",samplename.c_str()),"RECREATE");
  cout<<"writing output to file "<<outfile->GetName()<<endl;
  for(unsigned int l=0;l<layer.size();l++)
  {
   cout<<"l "<<l<<" layer[l] "<<layer[l]<<endl;
   outfile->Add(h_output[l]);
   outfile->Add(h_input[l]);
   double min,max,rmin,rmax;
   EnergyParametrizationValidation::autozoom(h_input[l],min,max,rmin,rmax);
   TH1D* h_output_zoom=EnergyParametrizationValidation::refill(h_output[l],min,max,rmin,rmax);
   h_output_zoom->SetName(Form("h_output_zoom_%s",layer[l].c_str()));
   TH1D* h_input_zoom=EnergyParametrizationValidation::refill(h_input[l],min,max,rmin,rmax);
   h_input_zoom->SetName(Form("h_input_zoom_%s",layer[l].c_str()));
   outfile->Add(h_output_zoom);
   outfile->Add(h_input_zoom);
  }
  outfile->Add(h_input_layer1_layer2);
  outfile->Add(h_output_layer1_layer2);
  outfile->Write();
  
  /*
  
  cout<<"Now making validation plots"<<endl;
  
  vector<string> name;
  vector<string> title;
  for(unsigned int l=0;l<layer.size()-1;l++)
  {
   name.push_back(layer[l].c_str());
   title.push_back(Form("E fraction in Layer %i",layerNr[l]));
  }
  name.push_back("totalE");
  name.push_back("sumfraction_elmag");
  name.push_back("sumfraction_had");
  title.push_back("total E [MeV]");
  title.push_back("Sum of E fractions in elmag. layers");
  title.push_back("Sum of E fractions in had. layers");
  
  for(unsigned int l=0;l<layer.size()+2;l++)
  {
   int use_autozoom=1;
   int one_canvas=0;
   double min,max,rmin,rmax;
   TH1D* h_output_lin;
   TH1D* h_input_lin;
   if(use_autozoom)
   {
    EnergyParametrizationValidation::autozoom(h_input[l],min,max,rmin,rmax);
    h_output_lin=EnergyParametrizationValidation::refill(h_output[l],min,max,rmin,rmax);  h_output_lin->SetName("h_output_lin");
    h_input_lin=EnergyParametrizationValidation::refill(h_input[l],min,max,rmin,rmax); h_input_lin->SetName("h_input_lin");
   }
   else
   {
    h_output_lin=(TH1D*)h_output[l]->Clone("h_output_lin");
    h_input_lin=(TH1D*)h_input[l]->Clone("h_input_lin");
   }
   
   //linear:
   double kolmo=h_input[l]->KolmogorovTest(h_output[l]);
   double chi2=h_input[l]->Chi2Test(h_output[l],"UW");
   h_input_lin->SetMarkerSize(1.0);
   h_input_lin->SetLineWidth(1);
   h_output_lin->SetLineWidth(1);
   h_output_lin->SetFillColor(7);
   h_output_lin->Scale(h_input[l]->Integral()/h_output_lin->Integral());
   h_input_lin->GetXaxis()->SetNdivisions(504,kFALSE);
   double ymax=h_input_lin->GetBinContent(h_input_lin->GetMaximumBin());
   h_input_lin->GetYaxis()->SetRangeUser(0,ymax*1.4);
   h_input_lin->GetYaxis()->SetTitle("Linear");
   h_input_lin->GetXaxis()->SetTitle(title[l].c_str());
   
   //log:
   TH1D* h_output_log=(TH1D*)h_output_lin->Clone("h_output_log");
   TH1D* h_input_log=(TH1D*)h_input_lin->Clone("h_input_log");
   h_input_log->GetYaxis()->SetRangeUser(0.1,ymax*5.0);
   h_input_log->GetYaxis()->SetTitle("Log");
      
   //cumulative:
   TH1D* h_output_cumul=(TH1D*)get_cumul(h_output_lin); h_output_cumul->SetName("h_output_cumul");
   TH1D* h_input_cumul =(TH1D*)get_cumul(h_input_lin);  h_input_cumul->SetName("h_input_cumul");
   double sf=h_input_cumul->GetBinContent(h_input_cumul->GetNbinsX());
   h_output_cumul->Scale(1.0/sf);
   h_input_cumul->Scale(1.0/sf);
   h_input_cumul->GetYaxis()->SetRangeUser(0,1.2);
   h_input_cumul->GetYaxis()->SetTitle("Cumulative");
   
   if(one_canvas)
   {
    TCanvas* can=new TCanvas("can","can",0,0,1600,600);
    can->Divide(3,1);
    can->cd(1);
    h_input_lin->Draw("e");
    h_output_lin->Draw("histsame");
    h_input_lin->Draw("esame");
    
    TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
    leg->AddEntry(h_output_lin,"Parametrisation","f");
    leg->AddEntry(h_input_lin,"G4 Input","lpe");
    leg->Draw();
    
    can->cd(2);
    h_input_log->Draw("e");
    h_output_log->Draw("histsame");
    h_input_log->Draw("esame");
    can->cd(2)->SetLogy();

    can->cd(3);
    h_input_cumul->Draw("e");
    h_output_cumul->Draw("histsame");
    h_input_cumul->Draw("esame");
    
    can->cd(1)->RedrawAxis();
    can->cd(2)->RedrawAxis();
    can->cd(3)->RedrawAxis();
    can->Print(Form("eparavalidation/%s/%s.pdf",samplename.c_str(),name[l].c_str()));
    delete can;
   } //one_canvas
   else
   {
   	TCanvas* can_lin=new TCanvas("can_lin","can_lin",0,0,800,600);
    h_input_lin ->Draw("e");
    h_output_lin->Draw("histsame");
    h_input_lin ->Draw("esame");
    {
     TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
     leg->SetBorderSize(0);
     leg->SetFillStyle(0);
     leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
     leg->AddEntry(h_output_lin,"Parametrisation","f");
     leg->AddEntry(h_input_lin,"G4 Input","lpe");
     leg->Draw();
     TLatex* la=new TLatex(0.2,0.5,"#splitline{Photons 50 GeV}{0.2 < |#eta| < 0.25}");
     la->SetNDC();
     la->Draw();
     ATLAS_LABEL(0.2,0.9);
    }
   	can_lin->RedrawAxis();
    can_lin->Print(Form("eparavalidation/%s/%s_lin.pdf",samplename.c_str(),name[l].c_str()));

   	TCanvas* can_log=new TCanvas("can_log","can_log",0,0,800,600);
    h_input_log ->Draw("e");
    h_output_log->Draw("histsame");
    h_input_log ->Draw("esame");
    can_log->SetLogy();
    {
     TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
     leg->SetBorderSize(0);
     leg->SetFillStyle(0);
     leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
     leg->AddEntry(h_output_lin,"Parametrisation","f");
     leg->AddEntry(h_input_lin,"G4 Input","lpe");
     leg->Draw();
    }
   	can_log->RedrawAxis();
    can_log->Print(Form("eparavalidation/%s/%s_log.pdf",samplename.c_str(),name[l].c_str()));
   	
    TCanvas* can_cum=new TCanvas("can_cum","can_cum",0,0,800,600);
    h_input_cumul ->Draw("e");
    h_output_cumul->Draw("histsame");
    h_input_cumul ->Draw("esame");
    {
     TLegend* leg=new TLegend(0.65,0.82,0.99,0.93);
     leg->SetBorderSize(0);
     leg->SetFillStyle(0);
     leg->SetHeader(Form("KS: %.2f, Chi2: %.2f",kolmo,chi2));
     leg->AddEntry(h_output_lin,"Parametrisation","f");
     leg->AddEntry(h_input_lin,"G4 Input","lpe");
     leg->Draw();
    }
   	can_cum->RedrawAxis();
    //can_cum->Print(Form("eparavalidation/%s/%s_cum.pdf",samplename.c_str(),name[l].c_str()));
   	
   } //separate canvases
   
  } //for layer
  
  */
  
}


TH1* get_cumul(TH1* hist)
{
  TH1D* h_cumul=(TH1D*)hist->Clone("h_cumul");
  double sum=0;
  for(int b=1;b<=h_cumul->GetNbinsX();b++)
  {
    sum+=hist->GetBinContent(b);
    h_cumul->SetBinContent(b,sum);
  }
  return h_cumul; 
}

void ATLAS_LABEL(double x,double y) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(1);
  l.DrawLatex(x,y,"#bf{ATLAS} Simulation");
  l.DrawLatex(x,y-10,"Preliminary");
}

void check()
{
 
 //plot the TFCS1DFunctionHistogram
 
 //TFile* file=TFile::Open("/afs/cern.ch/user/s/schaarsc/public/fastcalo/epara_storage/photons/secondPCA.root");
 //TFCS1DFunctionHistogram* hf=(TFCS1DFunctionHistogram*)file->Get("bin3/layer2/TFCS1DFunctionHistogram");
 
 TFile* file=TFile::Open("/afs/cern.ch/user/s/schaarsc/public/fastcalo/athena/Simulation/ISF/ISF_FastCaloSim/ISF_FastCaloSimParametrization/tools/FCSParams.root");
 TFCS1DFunctionHistogram* hf=(TFCS1DFunctionHistogram*)file->Get("EnergyParams/pdgid_22/EN_50000/eta_0_20/bin3/layer2/TFCS1DFunctionHistogram");

 cout<<"hf "<<hf<<endl;
 
 TH1D* hist=(TH1D*)(hf->vector_to_histo());
 hist->Draw();
 
}

