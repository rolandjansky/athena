/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCS2DFunction.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TFile.h"
#include "TH2.h"
#include "TMVA/DataLoader.h"

#include "TRandom1.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLatex.h"
#include "TMath.h"

using namespace std;

//=============================================
//======= TFCS2DFunction =========
//=============================================

TFCS2DFunction::TFCS2DFunction()
{
}

TFCS2DFunction::TFCS2DFunction(string inputfile)
{
  Initialize(inputfile);
}

void TFCS2DFunction::Initialize(string /*inputfile*/)
{
}

int TFCS2DFunction::fitShape(string inputfile, string weightfilename, string outfilename, string labeltitle, string labeloutput, int mycase, float calosize, bool doNNHit)
{

  //int debug=1;
  
  TRandom1* myRandom =new TRandom1(); myRandom->SetSeed(0);
  TRandom1* myRandom2=new TRandom1(); myRandom2->SetSeed(1000);
  int Ntoys1=50000;
  int Ntoys2=50000;

 //int do_iteration = 1;
 //int do_range     = 1;
 //double maxdev    = 100;
 int neurons      = 4;
 
 //double maxdev_cut=5; //5!!
  	
 tmvaregression_training(neurons,inputfile, weightfilename, outfilename, mycase,doNNHit);

 TFile* file=TFile::Open(inputfile.c_str());
 if(file) cout << "opening : " << inputfile << std::endl;
 if(!file) {
   cout<<"ERROR: Inputfile could not be opened; exiting."<<endl;
   return -1;
 }
 string sh = "";
 if(mycase==1 && !doNNHit) sh = "hLNEnergyDensity";
 if(mycase==1 && doNNHit)  sh = "hLNEnergy";
 if(mycase==2) sh = "hEnergy";
 TH2F* histo = (TH2F*)file->Get(sh.c_str());
 std::cout << "TEMP : " << histo->GetNbinsX() << std::endl;

 // Test regression at bin centres 
 if(mycase==1){

   // Test at the centres
   cout << "Testing the regression at the bin centres" << std::endl;
   TH2F* h_output=(TH2F*)histo->Clone("h_output");
   h_output->Reset();
   
   for(int i=1;i<histo->GetNbinsX()+1;i++){
     double alpha = histo->GetXaxis()->GetBinCenter(i);
     for(int j=1;j<histo->GetNbinsY()+1;j++){
       double r = histo->GetYaxis()->GetBinCenter(j);
       // boundaries for zooms
       std::vector<float> value = tmvaregression_application(alpha,r,weightfilename,mycase);
       h_output->SetBinContent(i,j,value.at(0));
     }
   }
   plotpolar(histo,h_output,labeltitle,labeloutput,"bincentre",mycase,calosize,doNNHit);
   
   // Test with random numbers 
   
   cout << "Testing the regression with " << Ntoys1 << " toys" << endl;
   TH2F* h_output2=(TH2F*)histo->Clone("h_output2");
   h_output2->Reset();
   TH2F* h_norm=(TH2F*)histo->Clone("h_norm");
   h_norm->Reset();


   // file containing the events simulated by the NN fit result!
   if(doNNHit){
     string outFile = "output/NNHit_"+labeloutput+".root";
     TFile* fsim = new TFile(outFile.c_str(),"recreate");
     TTree* Tsim = new TTree("NNSimHit","NNSimHit");
     float   fNNHitAlpha=0;
     float   fNNHitR=0;
     float   fNNHitLNEfrac=0;
     //TBranch *b0 ;
     //b0 =Tsim->Branch("NNHitAlpha", &fNNHitAlpha);
     //TBranch *b1;
     //b1 =Tsim->Branch("NNHitR", &fNNHitR);
     //TBranch *b2;
     //b2 =Tsim->Branch("NNHitLNEfrac", &fNNHitLNEfrac);
     
     gRandom = new TRandom3(0);
     TH1F* hrRandom = (TH1F*)file->Get("hr");
     if(!hrRandom) std::cout << "there is a problem with loading the histo" << std::endl;
     hrRandom->Rebin(10);
     
     for(int i=0;i<Ntoys1;i++){
       fNNHitAlpha  = TMath::Pi()/8.+ 2*myRandom->Uniform(0,1)*TMath::Pi();
       //fNNHitR      = histo->GetYaxis()->GetBinLowEdge(1)
       //+myRandom2->Uniform(0,1)*(histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY())+histo->GetYaxis()->GetBinWidth(histo->GetNbinsY())
       //				 -histo->GetYaxis()->GetBinLowEdge(0));
       fNNHitR = hrRandom->GetRandom();
       std::vector<float> value   = tmvaregression_application(fNNHitAlpha,fNNHitR,weightfilename,mycase);
       fNNHitLNEfrac=value.at(0);
       h_output2->Fill(fNNHitAlpha,fNNHitR,fNNHitLNEfrac);
       h_norm->Fill(fNNHitAlpha,fNNHitR,1.);
       Tsim->Fill();
     }
     h_output2->Divide(h_norm);
     plotpolar(histo,h_output2,labeltitle,labeloutput,"random",mycase,calosize,doNNHit);
     Tsim->Write();
     Tsim->Reset();
     fsim->Close();
   }
 }

 // Test regression with random numbers
 if(mycase==2){
   cout<<"Testing the regression with "<<Ntoys2<<" toys"<<endl;

   // Need to define a histogram with LN(r) boundaries first 
   int nalpha = histo->GetNbinsX()+1;
   int nr     = histo->GetNbinsY()+1;
   Float_t valpha[nalpha];
   Float_t vLNr[nr];
   
   for(Int_t i=1;i<nalpha;i++){
     valpha[i-1] = histo->GetXaxis()->GetBinLowEdge(i);
     if(i==nalpha-1)
       valpha[i] = histo->GetXaxis()->GetBinLowEdge(i)+histo->GetXaxis()->GetBinWidth(i);
   }
   for(Int_t j=1;j<nr;j++){
     vLNr[j-1] = TMath::Log(histo->GetYaxis()->GetBinLowEdge(j));
     if(j==1)
       vLNr[j-1] = TMath::Log(histo->GetYaxis()->GetBinLowEdge(2))-1.; 
     if(j==nr-1)
       vLNr[j] = TMath::Log(histo->GetYaxis()->GetBinLowEdge(j)+histo->GetYaxis()->GetBinWidth(j));
   }
   TH2F* h_output2=new TH2F("h_output2","h_output2",nalpha-1,valpha,nr-1,vLNr);

   for(int i=0;i<Ntoys2;i++)
     {
       double random  = myRandom->Uniform(0,1);
       double random2 = myRandom2->Uniform(0,1);
       
       std::vector<float> value   = tmvaregression_application(random,random2,weightfilename,mycase); 
       h_output2->Fill(value.at(0),value.at(1)); 	 
     }
   // Change back LN(r)->r for plotting
   TH2F* h_output3=(TH2F*)histo->Clone("h_output3");
   h_output3->Reset();
   for(Int_t j=1;j<h_output2->GetNbinsX()+1;j++){
     for(Int_t i=1;i<h_output2->GetNbinsY()+1;i++){
       h_output3->SetBinContent(j,i,h_output2->GetBinContent(j,i));
       h_output3->SetBinError(j,i,h_output2->GetBinError(j,i));
     }
   }
   plotpolar(histo,h_output3,labeltitle,labeloutput,"random",mycase,calosize,doNNHit);
 }//end mycase==2

 //int regression_success=1;
 //if(maxdev>maxdev_cut) regression_success=0;

 int status=0;
 /*if(regression_success)
   {
     cout<<"Regression successful. Weights are stored."<<endl;
     status=1;
   }
 if(!regression_success)
   {
     cout<<"Regression failed. Histogram is stored."<<endl;
     status=3;
   } //!success*/
 return status;
 
}

void TFCS2DFunction::tmvaregression_training(int neurons, string inputfile, string weightfile, string outfilename, int mycase, bool doNNHit)
{
 using namespace TMVA;

 TFile* file=TFile::Open(inputfile.c_str());
 if(!file) {
   cout<<"ERROR: Inputfile could not be opened; exiting."<<endl;
   return;
 }
 string stree = "";
 if(mycase==1) 
   stree = "NNfit";
 if(mycase==2) 
   stree = "NNinv";

 TTree* regTree = (TTree*)file->Get(stree.c_str());

 TString myMethodList = "" ;
 TMVA::Tools::Instance();
 std::map<std::string,int> Use;
 
 Use["PDERS"] = 0;  Use["PDEFoam"] = 0; Use["KNN"] = 0;  Use["LD"]  = 0; Use["FDA_GA"] = 0; Use["FDA_MC"] = 0;
 Use["FDA_MT"] = 0; Use["FDA_GAMT"] = 0; Use["MLP"] = 1; Use["SVM"] = 0; Use["BDT"] = 0; Use["BDTG"] = 0;
 
 std::cout << std::endl; std::cout << "==> Start TMVARegression with "<<neurons<<" Neurons "<<std::endl;
 
 if(myMethodList != "")
 {
  for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;
  std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
  for (UInt_t i=0; i<mlist.size(); i++)
  {
   std::string regMethod(mlist[i]);
   if (Use.find(regMethod) == Use.end())
   {
    std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
    for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
    std::cout << std::endl;
    return;
   }
   Use[regMethod] = 1;
  }
 }
 
// TString outfileName("TMVAReg.root");
 TFile* outputFile = TFile::Open( outfilename.c_str(), "RECREATE" );
 TMVA::Factory *factory = new TMVA::Factory( "TMVARegression", outputFile, "!V:!Silent:Color:DrawProgressBar" );
 	
 TMVA::DataLoader *dl=new TMVA::DataLoader("dl");
 	
 TString dirname=Form("%s/",weightfile.c_str());

 if(Use["MLP"])
  factory->BookMethod(dl, TMVA::Types::kMLP, "MLP", Form("!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=20000:HiddenLayers=%i:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator",neurons) ); 

 (TMVA::gConfig().GetIONames()).fWeightFileDir = dirname;
 if(mycase==1)
 {
  dl->AddVariable("alpha","alpha",'F');
  dl->AddVariable("r","r",'F');
  if(!doNNHit)
   dl->AddTarget("energydensityLN");
  if(doNNHit)
  dl->AddTarget("energyLN");
 }
 if(mycase==2)
 {
  dl->AddVariable("probalpha","probalpha",'F');
  dl->AddVariable("probr","probr",'F');
  dl->AddTarget("bina");
  dl->AddTarget("binrLN");
 }

 dl->AddSignalTree( regTree, 1.0, "Training" );
 dl->AddSignalTree( regTree, 1.0, "Test" );

 // Train MVAs using the set of training events
 factory->TrainAllMethods();
 
 // ---- Evaluate all MVAs using the set of test events
 factory->TestAllMethods();
 
 // ----- Evaluate and compare performance of all configured MVAs
 factory->EvaluateAllMethods();
 
 // Save the output
 outputFile->Close();
 
 std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
 std::cout << "==> TMVARegression is done!" << std::endl;      
  
 delete factory;  
 
}


vector<float> TFCS2DFunction::tmvaregression_application(Float_t x, Float_t y, string weightfile, int mycase)
{
 
 using namespace TMVA;

 vector<float> vval ;
 
 TString myMethodList = "" ;
 TMVA::Tools::Instance();
 
 std::map<std::string,int> Use;
 
 // --- Mutidimensional likelihood and Nearest-Neighbour methods
 Use["PDERS"]           = 0;   Use["PDEFoam"]         = 0;   Use["KNN"]            = 0;
 Use["LD"]		          = 0;   Use["FDA_GA"]          = 0;   Use["FDA_MC"]          = 0;
 Use["FDA_MT"]          = 0;   Use["FDA_GAMT"]        = 0;   Use["MLP"]             = 1; 
 Use["SVM"]             = 0;   Use["BDT"]             = 0;   Use["BDTG"]            = 0;
 // ---------------------------------------------------------------
 
 // Select methods (don't look at this code - not of interest)
 if(myMethodList != "")
 {
  for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;
  std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
  for (UInt_t i=0; i<mlist.size(); i++)
  {
   std::string regMethod(mlist[i]);
   if (Use.find(regMethod) == Use.end())
   {
    std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
    for(std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
    std::cout << std::endl;
    return vval;
   }
   Use[regMethod] = 1;
  }
 }
 
 // --------------------------------------------------------------------------------------------------
 
 TMVA::Reader *reader = new TMVA::Reader( "!Color:Silent");    
 

 //Float_t r=uniform;
 Float_t r = y; 
 Float_t alpha = x ; 
 Float_t probr = y ; 
 Float_t probalpha = x ; 

 if(mycase==1){
   reader->AddVariable( "alpha", &alpha );
   reader->AddVariable( "r", &r );
 }
 if(mycase==2){
   reader->AddVariable( "probalpha", &probalpha );
   reader->AddVariable( "probr", &probr );
 }

 TString dir    = Form("%s/",weightfile.c_str());
 TString prefix = "TMVARegression";
 
 // Book method(s)
 for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
 {
  if (it->second)
  {
   TString methodName = it->first + " method";
   TString weightfile2 = dir + prefix + "_" + TString(it->first) + ".weights.xml";
   reader->BookMVA( methodName, weightfile2 ); 
  }
 }
 
 Float_t val  = (reader->EvaluateRegression("MLP method"))[0];
 Float_t val2 = 0 ; 
 if(mycase==2) 
   val2 = (reader->EvaluateRegression("MLP method"))[1];

 vval.push_back(val);
 if(mycase==2)
   vval.push_back(val2);

 delete reader;
 return vval;
 
}

void TFCS2DFunction::plotpolar(TH2F* href, TH2F* hout, string labeltitle, string labeloutput, string method, int mycase, float calosize, bool doNNHit){

  gStyle->SetOptStat(0);
  
  Double_t stops[5] = {0.00, 0.24, 0.61, 0.84, 1.00};
  Double_t red[5]   = {0.00, 0.00, 0.87, 1.00, 0.51};
  Double_t green[5] = {0.00, 0.81, 1.00, 0.20, 0.00};
  Double_t blue[5]  = {0.51, 1.00, 0.12, 0.00, 0.00};
  
  UInt_t npoints = 5;
  Int_t ncontours=20;
  TColor::CreateGradientColorTable(npoints, stops, red, green, blue, ncontours);

  // LOOP ON THE 3 ZOOMS
  for(int i=1;i<4;i++){

    float zoom = 1000 ; 
    string szoom = "";

    if(i==1){
      zoom  = href->GetYaxis()->GetBinLowEdge(href->GetNbinsY())+href->GetYaxis()->GetBinWidth(href->GetNbinsY()); 
      szoom = "zoom0";
    }
    if(i==2){
      int nzoom1=int((href->GetNbinsY())/3.*2.)-1;
      zoom  = href->GetYaxis()->GetBinLowEdge(nzoom1); 
      szoom = "zoom1" ; 
    }
    if(i==3){
      int nzoom2 = int((href->GetNbinsY())/3.)-1;
      zoom  = href->GetYaxis()->GetBinLowEdge(nzoom2);
      szoom = "zoom2" ; 
    }
    
    std::cout << "NOW DOING FOR ZOOM = " << zoom << std::endl;
    Float_t myr[3];
    myr[0]=-zoom;
    myr[1]=0;
    myr[2]= zoom;
    TH2F* hdummy = new TH2F("hdummy","hdummy",2,myr,2,myr);
    hdummy->GetYaxis()->SetRangeUser(-float(zoom),float(zoom));
    hdummy->GetXaxis()->SetRangeUser(-float(zoom),float(zoom));
    hdummy->GetXaxis()->SetTitle("x [mm]");
    hdummy->GetYaxis()->SetTitle("y [mm]");

    href->SetContour(ncontours);
    //if(mycase==2) href->Scale(1./href->Integral());
    href->GetYaxis()->SetRangeUser(-float(zoom),float(zoom));
    href->GetZaxis()->SetLabelSize(0.03);
    if(mycase==1 && !doNNHit) href->GetZaxis()->SetTitle("log(E/mm^{2})");
    if(mycase==1 && doNNHit) href->GetZaxis()->SetTitle("log(E)");
    if(mycase==2) href->GetZaxis()->SetTitle("norm. E [MeV]");
    href->GetZaxis()->SetTitleSize(0.04);
    href->GetZaxis()->SetTitleOffset(1.);

    hout->SetContour(ncontours);
    //if(mycase==2) hout->Scale(1./hout->Integral());
    hout->GetYaxis()->SetRangeUser(-float(zoom),float(zoom));
    hout->GetZaxis()->SetLabelSize(0.03);
    if(mycase==1 && !doNNHit) hout->GetZaxis()->SetTitle("log(E/mm^{2})");
    if(mycase==1 && doNNHit) hout->GetZaxis()->SetTitle("log(E)");
    if(mycase==2) hout->GetZaxis()->SetTitle("norm. E [MeV]");
    hout->GetZaxis()->SetTitleSize(0.04);
    hout->GetZaxis()->SetTitleOffset(1.);

    // ratio plot
    TH2F* hratio = (TH2F*) hout->Clone("hratio");
    hratio->Divide(href);

    TLatex l;
    TLatex l2;
    TLatex l3;
    TLatex l4;

    TCanvas* c1 = new TCanvas("c1","c1",10,10,900,800);
    c1->SetRightMargin(0.13);
    hdummy->Draw("COLZ");
    href->Draw("COLZ POL SAME");
    l.DrawLatex(-1.3*zoom,-1.3*zoom,"ATLAS");
    l2.DrawLatex(-0.9*zoom,-1.3*zoom,"Simulation Preliminary");
    l3.DrawLatex(-zoom,1.02*zoom,(labeltitle+", NN input").c_str());    
    l4.DrawLatex(-0.95*zoom,0.85*zoom,szoom.c_str());
    if(!doNNHit) c1->SaveAs(("output/NNinput_"+labeloutput+"_"+szoom+"_"+method+".pdf").c_str());
    if( doNNHit) c1->SaveAs(("output/NNinput_"+labeloutput+"_"+szoom+"_"+method+"_NNHit.pdf").c_str());

    TCanvas* c2 =new TCanvas("c2","c2",10,10,900,800);
    c2->SetRightMargin(0.13);
    hdummy->Draw("COLZ");
    float myZmax = href->GetMaximum();
    float myZmin = href->GetMinimum();
    hout->SetMaximum(myZmax);
    hout->SetMinimum(myZmin);
    hout->Draw("COLZ POL SAME");
    l.DrawLatex(-1.3*zoom,-1.3*zoom,"ATLAS");
    l2.DrawLatex(-0.9*zoom,-1.3*zoom,"Simulation Preliminary");
    l3.DrawLatex(-zoom,1.02*zoom,(labeltitle+", NN output").c_str());
    l4.DrawLatex(-0.95*zoom,0.85*zoom,szoom.c_str());
    if(!doNNHit) c2->SaveAs(("output/NNoutput_"+labeloutput+"_"+szoom+"_"+method+".pdf").c_str());
    if( doNNHit) c2->SaveAs(("output/NNoutput_"+labeloutput+"_"+szoom+"_"+method+"_NNHit.pdf").c_str());

    TCanvas* c3 =new TCanvas("c3","c3",10,10,900,800);
    c3->SetRightMargin(0.13);
    //c3->SetLogz();
    hdummy->Draw("COLZ");
    hratio->SetMinimum(0);
    hratio->SetMaximum(3);
    hratio->Draw("COLZ POL SAME");
    l.DrawLatex(-1.3*zoom,-1.3*zoom,"ATLAS");
    l2.DrawLatex(-0.9*zoom,-1.3*zoom,"Simulation Preliminary");
    l3.DrawLatex(-zoom,1.02*zoom,(labeltitle+", ratio").c_str());
    l4.DrawLatex(-0.95*zoom,0.85*zoom,szoom.c_str());
    if(!doNNHit) c3->SaveAs(("output/NNratio_"+labeloutput+"_"+szoom+"_"+method+".pdf").c_str());
    if( doNNHit) c3->SaveAs(("output/NNratio_"+labeloutput+"_"+szoom+"_"+method+"_NNHit.pdf").c_str());

    // projections
    if(i==1){ // only for zoom0 case

      int nR = href->GetNbinsY()+1;
      Float_t* vR = new Float_t[nR];
      for(Int_t i=1;i<hratio->GetNbinsY()+1;i++){
	vR[i-1]=hratio->GetYaxis()->GetBinLowEdge(i);
	if(i==hratio->GetNbinsY()){
	  vR[i] = hratio->GetYaxis()->GetBinLowEdge(i)+hratio->GetYaxis()->GetBinWidth(i);
	}
      }
      TH1D* hratio1Dr     = new TH1D("hratio1Dr","hratio1Dr",nR-1,vR);
      
      for(Int_t j=1;j<hratio->GetNbinsY()+1;j++){
	float myrcumul = 0 ; 
	for(Int_t i=1;i<hratio->GetNbinsX()+1;i++){
	  myrcumul += hratio->GetBinContent(i,j);
	}
	hratio1Dr->SetBinContent(j,myrcumul/hratio->GetNbinsX());
      }
      
      int nA = href->GetNbinsX()+1;
      Float_t* vA = new Float_t[nA];
      for(Int_t i=1;i<hratio->GetNbinsX()+1;i++){
	vA[i-1]=hratio->GetXaxis()->GetBinLowEdge(i);
	if(i==hratio->GetNbinsX()){
	  vA[i] = hratio->GetXaxis()->GetBinLowEdge(i)+hratio->GetXaxis()->GetBinWidth(i);
	}
      }
      TH1D* hratio1Dalpha  = new TH1D("hratio1Dalpha","hratio1Dalpha",nA-1,vA);
      for(Int_t i=1;i<hratio->GetNbinsX()+1;i++){
	float myalphacumul = 0 ;
	for(Int_t j=1;j<hratio->GetNbinsY()+1;j++){
	  myalphacumul += hratio->GetBinContent(i,j);
	}
	hratio1Dalpha->SetBinContent(i,myalphacumul/hratio->GetNbinsY());
      }
      
      TCanvas* c4 =new TCanvas("c4","c4",10,10,900,800);
      c4->SetRightMargin(0.13);
      hratio1Dalpha->GetXaxis()->SetTitle("#alpha");
      hratio1Dalpha->GetYaxis()->SetTitle("NN output / NN input");
      hratio1Dalpha->GetYaxis()->SetRangeUser(0,2);
      hratio1Dalpha->Draw();
      l.DrawLatex(0,-0.3,"ATLAS Simulation Preliminary");
      l3.DrawLatex(0,2.03,(labeltitle+", ratio").c_str());
      l4.DrawLatex(2,1.85,szoom.c_str());
      if(!doNNHit) c4->SaveAs(("output/NNratioAlpha_"+labeloutput+"_"+szoom+"_"+method+".pdf").c_str());
      if( doNNHit) c4->SaveAs(("output/NNratioAlpha_"+labeloutput+"_"+szoom+"_"+method+"_NNHit.pdf").c_str());

      TCanvas* c5 =new TCanvas("c5","c5",10,10,900,800);
      c5->SetRightMargin(0.13);
      c5->SetLogx();
      hratio1Dr->GetXaxis()->SetTitle("r [mm]");
      hratio1Dr->GetYaxis()->SetTitle("NN output / NN input");
      hratio1Dr->GetXaxis()->SetRangeUser(-float(zoom),float(zoom));
      hratio1Dr->GetYaxis()->SetRangeUser(0,2);
      hratio1Dr->Draw();
      l.DrawLatex(0,-0.3,"ATLAS Simulation Preliminary");
      l3.DrawLatex(0,2.03,(labeltitle+", ratio").c_str());
      l4.DrawLatex(2,1.85,szoom.c_str());
      if(!doNNHit) c5->SaveAs(("output/NNratioR_"+labeloutput+"_"+szoom+"_"+method+".pdf").c_str());
      if( doNNHit) c5->SaveAs(("output/NNratioR_"+labeloutput+"_"+szoom+"_"+method+"_NNHit.pdf").c_str());

      // rebin r with respect to the calorimeter geometry
      float newRboundary = 0 ; 
      float newbincontent = 0 ; 
      std::vector<float> vnewRboundary;
      std::vector<float> vnewbincontent;
      int n = 1 ;
      int nmergedbins=0;
      vnewRboundary.push_back(hratio1Dr->GetBinLowEdge(1));
      for(Int_t i=1;i<hratio1Dr->GetNbinsX()+1;i++){
	newRboundary     = hratio1Dr->GetBinLowEdge(i)+hratio1Dr->GetBinWidth(i);
	newbincontent   += hratio1Dr->GetBinContent(i);
	nmergedbins++;
	if(newRboundary>n*calosize){
	  vnewbincontent.push_back(newbincontent/nmergedbins);
	  vnewRboundary.push_back(newRboundary);
	  n++;
	  nmergedbins=0;
	  newbincontent=0;
	}
      }
      int nnewRboundary = vnewRboundary.size();
      Float_t vnewRboundary1[nnewRboundary];
      for(unsigned int j=0;j<vnewRboundary.size();j++){
	vnewRboundary1[j] = vnewRboundary.at(j);
      }
      TH1F* hratio1DrBIN = new TH1F("hratio1DrBIN","hratio1DrBIN",nnewRboundary-1,vnewRboundary1);
      
      for(unsigned int k=0;k<vnewbincontent.size();k++){
	hratio1DrBIN->SetBinContent(k+1,vnewbincontent.at(k));
      }
      std::ostringstream scalosize;
      scalosize << calosize ; 
      
      if(calosize==-1000)
	std::cout << "There is a problem with determining the size of the calo cell! " << std::endl;
      TCanvas* c6 =new TCanvas("c6","c6",10,10,900,800);
      c6->SetRightMargin(0.13);
      hratio1DrBIN->GetXaxis()->SetRangeUser(-float(zoom),float(zoom));
      hratio1DrBIN->GetXaxis()->SetTitle("r [mm]");
      hratio1DrBIN->GetYaxis()->SetTitle("NN output / NN input");
      hratio1DrBIN->GetXaxis()->SetRangeUser(-float(zoom),float(zoom));
      hratio1DrBIN->GetYaxis()->SetRangeUser(0,2);
      hratio1DrBIN->Draw();
      c6->SetLogx();
      l.DrawLatex(-0,-0.3,"ATLAS Simulation Preliminary");
      l3.DrawLatex(0,2.03,(labeltitle+", ratio").c_str());
      string scalo = szoom+" ,rebin of "+scalosize.str()+" mm";
      l4.DrawLatex(calosize,1.85,scalo.c_str());
      if(!doNNHit) c6->SaveAs(("output/NNratioR_rebincalo_"+labeloutput+"_"+szoom+"_"+method+".pdf").c_str());
      if( doNNHit) c6->SaveAs(("output/NNratioR_rebincalo_"+labeloutput+"_"+szoom+"_"+method+"_NNHit.pdf").c_str());

      delete hratio1DrBIN;
      delete hratio1Dr;
      delete hratio1Dalpha;
      delete c4;
      delete c5;
      delete c6;
    }

    delete hdummy;
    delete c1;
    delete c2;
    delete c3;
  }

}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS2DFunction)

