/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"

#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TFile.h"

#include "TRandom1.h"

//=============================================
//======= TFCS1DFunction =========
//=============================================

TFCS1DFunction::TFCS1DFunction()
{
}

TFCS1DFunction::TFCS1DFunction(TH1* hist)
{
  Initialize(hist);
}

void TFCS1DFunction::Initialize(TH1* /*hist*/)
{
}

int TFCS1DFunction::testHisto(TH1* hist, std::string weightfilename, float &rangeval, float &startval, std::string outfilename, int skip_regression,
    int neurons_start, int neurons_end, double cut_maxdev, int ntoys)
{

  //int debug=1;

  //transform the histogram
  TH1* h_transf=transform(hist, rangeval, startval); h_transf->SetName("h_transf");

  //Turn the histogram into a tree:
  std::vector<double> contents;
  std::vector<double> centers;
  for(int b=1;b<=h_transf->GetNbinsX();b++)
  {
    contents.push_back(h_transf->GetBinContent(b));
    centers.push_back(h_transf->GetBinCenter(b));
  }

  TTree* tree=new TTree("tree","tree");
  Float_t x,y;
  tree->Branch("x",&x,"x/F");
  tree->Branch("y",&y,"y/F");

  for(unsigned int i=0;i<centers.size();i++)
  {
    y=(Float_t)(contents[i]);   //xvals are the BinContents
    x=(Float_t)(centers[i]);    //yvals are the BinCenters

    tree->Fill();
  }

  double range_low=get_range_low(h_transf);

  TRandom1* myRandom=new TRandom1(); myRandom->SetSeed(0);

  //int do_iteration=1;
  int do_range=1;
  double maxdev=1000;
  int neurons=neurons_start;
  std::vector<TH1*> histos;
  if(skip_regression) std::cout<<"skipping regression"<<std::endl;
  while(!skip_regression && maxdev>cut_maxdev && neurons<=neurons_end)
  {

    tmvaregression_training(neurons, tree, weightfilename, outfilename);

    std::cout<<"Testing the regression with "<<ntoys<<" toys"<<std::endl;
    TH1* h_output=(TH1*)h_transf->Clone("h_output");
    h_output->Reset();
    for(int i=0;i<ntoys;i++)
    {
      double random=myRandom->Uniform(1);
      if(do_range && random<range_low) random=range_low;
      double value =tmvaregression_application(random,weightfilename);
      h_output->Fill(value);    
    }

    TH1* h_cumul=get_cumul(h_output);
    h_cumul->SetName(Form("h_cumul_neurons%i",neurons));
    histos.push_back(h_cumul);

    maxdev=get_maxdev(h_transf,h_cumul);
    std::cout<<"---> Neurons="<<neurons<<" MAXDEV="<<maxdev<<"%"<<std::endl;
    neurons++;
    //if(!do_iteration) break;
  }

  //TH1* histclone=(TH1*)hist->Clone("histclone");

  /* 
     TFile* out_iteration=new TFile(Form("output/iteration_%s.root",label.c_str()),"RECREATE");
     for(int h=0;h<histos.size();h++)
     {
     out_iteration->Add(histos[h]);
     }
     out_iteration->Add(histclone);
     out_iteration->Write();
     out_iteration->Close();
     */

  int regression_success=1;
  if(maxdev>cut_maxdev) regression_success=0;

  int status=0;
  if(regression_success)
  {
    std::cout<<"Regression successful. Weights are stored."<<std::endl;
    if(rangeval<0)  status=1;
    if(rangeval>=0) status=2;
  }

  if(!regression_success)
  {
    std::cout<<"Regression failed. Histogram is stored."<<std::endl;
    status=3;
  } //!success

  return status;

}

double TFCS1DFunction::rnd_to_fct(double /*rnd*/)
{

  return 0;
}


double TFCS1DFunction::get_range_low(TH1* hist)
{
  double range_low=0.0;
  int bin_start=-1;
  for(int b=1;b<=hist->GetNbinsX();b++)
  {
    if(hist->GetBinContent(b)>0 && bin_start<0)
    {
      bin_start=b;
      range_low=hist->GetBinContent(b);
      b=hist->GetNbinsX()+1;
    }
  }
  return range_low;
}

void TFCS1DFunction::tmvaregression_training(int neurons, TTree *regTree, std::string weightfile, std::string outfilename)
{
  using namespace TMVA;

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
  TString dirname=Form("%s/",weightfile.c_str());
  (TMVA::gConfig().GetIONames()).fWeightFileDir = dirname;
  	
  TMVA::DataLoader *dl=new TMVA::DataLoader("dl");

  if(Use["MLP"])
    //factory->BookMethod( TMVA::Types::kMLP, "MLP", Form("!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=20000:HiddenLayers=%i:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator",neurons) );
    factory->BookMethod(dl, TMVA::Types::kMLP, "MLP", Form("!H:!V:VerbosityLevel=Info:NeuronType=tanh:NCycles=20000:HiddenLayers=%i:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator",neurons)); 
    	
  dl->AddVariable( "y", "y", 'F' );
  dl->AddTarget( "x" );
  //TFile *input(0);
  Double_t regWeight  = 1.0;   

  dl->AddRegressionTree( regTree, regWeight );
  TCut mycut = "";
  dl->PrepareTrainingAndTestTree( mycut,"nTrain_Regression=0:nTest_Regression=0:SplitMode=Random:NormMode=NumEvents:!V" );


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


TH1* TFCS1DFunction::transform(TH1* h_input, float &rangeval, float& startval)
{

  bool do_transform=false;
  float xmin=h_input->GetXaxis()->GetXmin();
  float xmax=h_input->GetXaxis()->GetXmax();
  if(xmin<0 || xmax>1) do_transform=true;

  TH1D* h_out;

  if(do_transform)
  {
    int nbins=h_input->GetNbinsX();
    double min=0;
    double max=1;
    h_out=new TH1D("h_out","h_out",nbins,min,max);

    for(int b=1;b<=nbins;b++)
      h_out->SetBinContent(b,h_input->GetBinContent(b));

    //store the inital range
    rangeval=xmax-xmin;
    startval=xmin;
  }
  if(!do_transform)
  {
    rangeval=-1;
    h_out=(TH1D*)h_input->Clone("h_out");
  }
  return h_out;

}


double TFCS1DFunction::get_maxdev(TH1* h_input1, TH1* h_approx1)
{

  TH1D* h_input =(TH1D*)h_input1->Clone("h_input");
  TH1D* h_approx=(TH1D*)h_approx1->Clone("h_approx");

  double maxdev=0.0;

  //normalize the histos to the same area:
  double integral_input=h_input->Integral();
  double integral_approx=0.0;
  for(int b=1;b<=h_input->GetNbinsX();b++)
    integral_approx+=h_approx->GetBinContent(h_approx->FindBin(h_input->GetBinCenter(b)));
  h_approx->Scale(integral_input/integral_approx);

  double ymax=h_approx->GetBinContent(h_approx->GetNbinsX())-h_approx->GetBinContent(h_approx->GetMinimumBin());
  for(int i=1;i<=h_input->GetNbinsX();i++)
  {
    double val=fabs(h_approx->GetBinContent(h_approx->FindBin(h_input->GetBinCenter(i)))-h_input->GetBinContent(i))/ymax;
    if(val>maxdev) maxdev=val;
  }

  delete h_input;
  delete h_approx;

  return maxdev*100.0;

}


double TFCS1DFunction::tmvaregression_application(double uniform, std::string weightfile)
{

  using namespace TMVA;

  TString myMethodList = "" ;
  TMVA::Tools::Instance();

  std::map<std::string,int> Use;

  // --- Mutidimensional likelihood and Nearest-Neighbour methods
  Use["PDERS"]           = 0;   Use["PDEFoam"]         = 0;   Use["KNN"]            = 0;
  Use["LD"]             = 0;   Use["FDA_GA"]          = 0;   Use["FDA_MC"]          = 0;
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
        return 0;
      }
      Use[regMethod] = 1;
    }
  }

  // --------------------------------------------------------------------------------------------------

  TMVA::Reader *reader = new TMVA::Reader( "!Color:Silent");    

  Float_t y=uniform;
  reader->AddVariable( "y", &y );

  TString dir    = Form("%s/",weightfile.c_str());
  TString prefix = "TMVARegression";

  // Book method(s)
  for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
  {
    if (it->second)
    {
      TString methodName = it->first + " method";
      TString weightfile = dir + prefix + "_" + TString(it->first) + ".weights.xml";
      reader->BookMVA( methodName, weightfile ); 
    }
  }

  Float_t val = (reader->EvaluateRegression("MLP method"))[0];
  delete reader;
  return val;

}


TH1* TFCS1DFunction::get_cumul(TH1* hist)
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


//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCS1DFunction)


