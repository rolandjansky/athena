/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>
using namespace RooFit;
string muonColl = "StacoMuonCollection";

void getHistos(string fname="MuonPhysMon.root",string dname="") 
{  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);

  TFile* f=new TFile(fname.c_str(),"READ");
  
  //Get the run number - it's the top-folder's name in the root file!
  TIter dirItr(gDirectory->GetListOfKeys());
  TKey *key=0;

  vector<TH1F*> h_mass_trigs;
  vector<string> trigs;
   
  while (key=(TKey*) dirItr()) {
    TDirectory *dir= (TDirectory*)key->ReadObj();
    string root=dir->GetName();

    if (dname!="" && root!=dname) {
      cout<<"Skipping directory: " << root << endl;continue;
    }
    else if ( root.find("run") < root.length() ) {
      root = root + "/"; cout<<root<<endl;
    }
    else root = "";

    string pathOverview = root+"Muon/MuonPhysicsMonitoring/Jpsimumu/Overview";
    TDirectory* dirOverview = f->GetDirectory(pathOverview.c_str());
    TIter trigItr(dirOverview->GetListOfKeys());
    TKey *trigKey=0;
    while( trigKey=(TKey*)trigItr()) {
      TDirectory* trigDir = (TDirectory*)trigKey->ReadObj();
      TIter trig2Itr(trigDir->GetListOfKeys());
      TKey *trig2Key=0;
      while( trig2Key=(TKey*)trig2Itr()) {
	TDirectory* trig3Dir = (TDirectory*)trig2Key->ReadObj();
	string alg = trig3Dir->GetName();
	if (alg.find("Muid")<alg.length()) continue;
	TString algTrig( (char*)strstr( trig3Dir->GetName(), "Collection" ) );
	algTrig.Remove(0,10);
	if (algTrig=="") algTrig="NoTrig";
	string salgTrig=algTrig;
	trigs.push_back(salgTrig);
	h_mass_trigs.push_back(new TH1F());
	h_mass_trigs.back() = (TH1F*) trig3Dir->Get("m_Jpsi_Mass")->Clone();
      }
    }

    /////////////////////////////////////////////////////////////////
  }
  
  const int N=h_mass_trigs.size();
  vector<int> nJpsi(N);
  vector<int> nJpsiErr(N);
  vector<double> mass(N);
  vector<double> massErr(N);

  TCanvas *c = new TCanvas();
  if (N<5) c->Divide(2,2);
  else if (N<10) c->Divide(3,3);
  else if (N<17) c->Divide(4,4);
  for (unsigned int i=0; i< h_mass_trigs.size(); i++) {
    c->cd(i+1);
    TH1F *hJpsiMass = h_mass_trigs[i]->Clone();
    fitHistogram(hJpsiMass,nJpsi[i],mass[i],nJpsiErr[i],massErr[i]);    
  }
  
  TCanvas *c1 = new TCanvas();TLegend *leg = new TLegend(0.38,0.85,0.95,1); 
  for (unsigned int i=0; i< h_mass_trigs.size(); i++) {
    h_mass_trigs[i]->SetLineColor(i*2+2);
    //h_mass_trigs[i]->SetFillColor(i*2+2);
    char txt[100];
    sprintf(txt,"%s: %i",trigs[i].c_str(),nJpsi[i]);
    leg->AddEntry(h_mass_trigs[i],txt,"lp");
    if (i==0) {
      h_mass_trigs[i]->GetYaxis()->SetRangeUser(0.,h_mass_trigs[i]->GetMaximum()*1.1);
      h_mass_trigs[i]->Draw();
    }
    else h_mass_trigs[i]->Draw("same");
  }
  leg->Draw();

   TCanvas *c2 = new TCanvas();
   TH1F *h_mass_per_trigger = new TH1F("h_mass_per_trigger","Reconstructed J/#psi mass per trigger",1,0,1);
   for (unsigned int i=0; i< h_mass_trigs.size(); i++) {
     h_mass_per_trigger->Fill( trigs[i].c_str(),mass[i]);
     h_mass_per_trigger->SetBinError(i+1,massErr[i]);
   }  
   h_mass_per_trigger->Draw("e");

   TCanvas *c3 = new TCanvas();
   TH1F *h_nJpsi_per_trigger = new TH1F("h_nJpsi_per_trigger","Reconstructed J/#psi events per trigger",1,0,1);
   for (unsigned int i=0; i< h_mass_trigs.size(); i++) {
     h_nJpsi_per_trigger->Fill( trigs[i].c_str(), nJpsi[i]);
     h_nJpsi_per_trigger->SetBinError( i+1, nJpsiErr[i]);
   }  
   h_nJpsi_per_trigger->Draw("e");
}





void fitHistogram(TH1F* hJpsiMass,int &nJpsi,  double &mass, int &nJpsiErr,double &massErr)
{
  RooMsgService::instance().setSilentMode(true); 
  RooRealVar m("m","mass",2000,4000,"MeV");
  //Data
  RooDataHist data("data","data",m,hJpsiMass);
  //Parameters
  //RooRealVar b("b","slope b",-0.00005,-0.0001,0.0001,"MeV^-1");
  RooRealVar nsig("nsig","signal fraction",150., 1.,2000.);
  RooRealVar nbkg("nbkg","background fraction",500.,1.,40000.);
  RooRealVar mean("mean","JPsi mass mean",3100,2800,3300,"MeV");
  RooRealVar sigma("sigma","JPsi mass sigma",120,10,300,"MeV");
  //model
  RooGaussian sig("sig","signal",m,mean,sigma);  
  RooPolynomial bkg("bkg","background",m);
  RooAddPdf model("model","model",RooArgList(sig,bkg),RooArgList(nsig,nbkg));
  model.fitTo(data,PrintEvalErrors(-1));
  RooPlot* frame1 = m.frame();
  frame1->SetTitle("J/#psi Mass");
  data.plotOn(frame1);
  model.plotOn(frame1);
  model.paramOn(frame1,Parameters(RooArgList(nsig,mean,sigma)),Layout(0.6,0.95,0.93),Format("NEU")); 
  //Parameters printing in console///
  RooArgSet* params = model.getVariables();
  //params->Print("v");
  frame1->Draw();
  
  nJpsi = nsig.getVal();
  nJpsiErr = nsig.getError();
  mass = mean.getVal();
  massErr = mean.getError();
  
  return;
}
