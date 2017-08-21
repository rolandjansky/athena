/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************
 * Class to implement NN regression in input variables
 * <a.hasib@cern.ch>
 ****************************************************/

#include "ShowerShapeRegression.h"


#include "TH2.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TRandom1.h"
#include "TRandom3.h"
#include "TMath.h"

#include "TMVA/Tools.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TMVA/Config.h"


#include <iostream>



using namespace std;
using namespace TMVA;

ShowerShapeRegression::ShowerShapeRegression()
{
   m_calolayer      = 0;
   m_PCAbin         = 0;
   m_particle       = "";
   m_particleEnergy = 0.;
   m_etamin         = 0.;
   m_etamax         = 0.;

   m_topDir        = "";
   m_fileName      = "";
   m_outputDirName = "";
   m_NNinputName   = "";
   m_NNoutputName  = "";

   m_neurons      = 0;
   m_hiddenLayers = 0;
}


ShowerShapeRegression::~ShowerShapeRegression()
{
}


void ShowerShapeRegression::Run(std::vector<string> targetVarVec)
{
   TFile *NNinputFile  = TFile::Open(m_NNinputName.c_str());
   TFile *NNoutputFile = TFile::Open(m_NNoutputName.c_str(), "recreate");

   TRandom1 *myRandom = new TRandom1();
   myRandom->SetSeed(0);

   TRandom1 *myRandom2 = new TRandom1();
   myRandom2->SetSeed(1000);

   TH2F *hinput = (TH2F*)NNinputFile->Get("hEnergyNorm");

   int Ntoys = 5000; //hinput->GetEntries();


   for (unsigned int itarget = 0; itarget < targetVarVec.size(); itarget++)
   {
      string histoName = "hEnergyNorm"; /*h" + targetVarVec.at(itarget)*/;

      TH2F *hin = (TH2F *)NNinputFile->Get("hEnergyNorm");

      cout << "===> hin = " << hin << endl;


      cout << " ======> Run regrression training with target variable = " << (targetVarVec.at(itarget)).c_str() << " <======" << endl;

      TMVARegressionTraining(targetVarVec.at(itarget));

      std::cout << " ======> after regression training " << std::endl;

      //* Define a histogram with alpha and Ln(r) boundaries

      std::cout << " ===> 1 " << std::endl;
      int nalpha = hin->GetNbinsX() + 1;
      int nr = hin->GetNbinsY() + 1;

      std::cout << " ===> 2 " << std::endl;

      // * get the bin boundaries

      float xbinsalpha[nalpha];
      float xbinsLnr[nr];

      for (int ibin = 1; ibin < nalpha; ++ibin)
      {
         xbinsalpha[ibin - 1] = hin->GetXaxis()->GetBinLowEdge(ibin);

         if (ibin == nalpha - 1 )
            xbinsalpha[ibin] = hin->GetXaxis()->GetBinUpEdge(ibin);
      }

      for (int ibin = 0; ibin < nr; ibin++)
      {
         xbinsLnr[ibin - 1] = TMath::Log(hin->GetYaxis()->GetBinLowEdge(ibin));

         if (ibin == nr - 1)
            xbinsLnr[ibin] = TMath::Log(hin->GetXaxis()->GetBinUpEdge(ibin));
      }

      TH2F* houtLn = new TH2F("houtLn", "houtLn", nalpha - 1, xbinsalpha, nr - 1, xbinsLnr);
      //TH2F *hout = (TH2F *)hin->Clone(histoName.c_str());
      //hout->Reset();

      //* Use random numbers for tmva application

      for (int it = 0; it < Ntoys; ++it)
      {
         double random = myRandom->Uniform(0, 1);
         double random2 = myRandom2->Uniform(0, 1);

         std::cout << " ======> before regression application " << std::endl;

         std::vector<float> value = TMVARegressionApplication(random, random2, targetVarVec.at(itarget));
         std::cout << " ======> after regression application " << std::endl;


         houtLn->Fill(value.at(0), value.at(1));
         //hout->Fill(value.at(0), TMath::Exp(value.at(1)));

      }


      // // * change Ln(r) to r

      TH2F *hout = (TH2F *)hin->Clone(histoName.c_str());
      hout->Reset();


      for (int ibinx = 1; ibinx < houtLn->GetNbinsX() + 1; ibinx++)
      {
         for (int ibiny = 1; ibiny < houtLn->GetNbinsY() + 1; ibiny++)
         {
            hout->SetBinContent(ibinx, ibiny, houtLn->GetBinContent(ibinx, ibiny));
            hout->SetBinError(ibinx, ibiny, houtLn->GetBinError(ibinx, ibiny));
         }
      }


      //hout->Scale(1.0 / hout->Integral());
      // houtLn->Scale(1 / (houtLn->Integral()));
      NNoutputFile->cd();
      hout->Write();
      houtLn->Write();
   }

   NNoutputFile->Close();
}


void ShowerShapeRegression::TMVARegressionTraining(std::string target)
{
   // * Read training and tets data
   //cout<<" NNinput file = "<<m_NNinputName.c_str()<<endl;
   TFile *NNinputFile = TFile::Open(m_NNinputName.c_str());

   if (!NNinputFile)
   {
      cout << "ERROR:: inputFile cannot be opened" << endl;
      exit(EXIT_FAILURE);
   }

   TTree *regTree = (TTree *)NNinputFile->Get("NNprob"); //will change

   TString myMethodList = "";

   // * load the TMVA libaray
   TMVA::Tools::Instance();

   // * Default MVA methods to be trained + tested
   std::map<std::string, int> Use;

   // Mutidimensional likelihood and Nearest-Neighbour methods
   Use["PDERS"]   = 0;
   Use["PDEFoam"] = 0;
   Use["KNN"]     = 0;
   //
   // Linear Discriminant Analysis
   Use["LD"] = 0;
   //
   // Function Discriminant analysis
   Use["FDA_GA"]   = 0;
   Use["FDA_MC"]   = 0;
   Use["FDA_MT"]   = 0;
   Use["FDA_GAMT"] = 0;
   //
   // Neural Network
   Use["MLP"] = 1;
   Use["DNN"] = 0;
   //
   // Support Vector Machine
   Use["SVM"] = 0;
   //
   // Boosted Decision Trees
   Use["BDT"]  = 0;
   Use["BDTG"] = 0;



   std::cout << std::endl;
   std::cout << "==> Start TMVARegression" << std::endl;

   // * Select methods
   if (myMethodList != "")
   {
      for (std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++)
      {
         it->second = 0;
      }
      std::vector<TString> mlist = gTools().SplitString(myMethodList, ',');
      for (UInt_t i = 0; i < mlist.size(); i++)
      {
         std::string regMethod(mlist[i]);
         if (Use.find(regMethod) == Use.end())
         {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++)
            {
               std::cout << it->first << " ";
            }
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }


   // * create ouput regression file
   string regRootFileName = m_topDir + m_particle + "/" + "NNreg_" + target + "_neuron" + std::to_string(m_neurons) + "_nbinsR" + std::to_string(m_nbinsR) + m_fileName + ".root";

   string RegWeightDir = "RegWeight_" + target + "_neuron" + std::to_string(m_neurons) + "_nbinsR" + std::to_string(m_nbinsR) + m_fileName;



   TFile *NNregFile = TFile::Open(regRootFileName.c_str(), "recreate");

   // * create factory object

   TMVA::Factory *factory = new TMVA::Factory("TMVARegression", NNregFile, "V:!Silent:Color:DrawProgressBar:AnalysisType=Regression");

   // * create dataloader to read the input variables

   TMVA::DataLoader *dataloader = new TMVA::DataLoader(RegWeightDir);
   cout << " reg weight dir = " << RegWeightDir.c_str() << endl;


   // define input variables that shall be used for training

   dataloader->AddVariable("alpha_prob", "AlphaProb", 'F');
   dataloader->AddVariable("r_prob", "rProb", 'F');


   dataloader->AddTarget("alpha_bin", "alpha", 'F');
   dataloader->AddTarget("r_bin", "r", 'F');

   //dataloader->AddRegressionTree(regTree, 1.0);
   dataloader->AddSignalTree(regTree, 1.0, "Train Tree");
   dataloader->AddSignalTree(regTree, 1.0, "Test Tree");

   //TCut mycut = "";

   // * Neural network (MLP)
   if (Use["MLP"])
   {
      factory->BookMethod(dataloader, TMVA::Types::kMLP, "MLP", Form("!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=20000:HiddenLayers=%i,:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=20:!UseRegulator", m_neurons));
   }


   // * Train MVAs using the set of training events
   factory->TrainAllMethods();
   // * Evaluate all MVAs using the set of test events
   factory->TestAllMethods();
   // * Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // * Save the output
   NNregFile->Close();
   std::cout << "==> Wrote root file: " << NNregFile->GetName() << std::endl;
   std::cout << "==> TMVARegression is done!" << std::endl;

   if (factory)
   {
      delete factory;
   }
   if (dataloader)
   {
      delete dataloader;
   }
}


std::vector<float> ShowerShapeRegression::TMVARegressionApplication(float xval, float yval, string target)
{
   std::vector<float> value;

   TString myMethodList = "";

   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string, int> Use;


   // --- Mutidimensional likelihood and Nearest-Neighbour methods
   Use["PDERS"]   = 0;
   Use["PDEFoam"] = 0;
   Use["KNN"]     = 0;
   //
   // --- Linear Discriminant Analysis
   Use["LD"] = 0;
   //
   // --- Function Discriminant analysis
   Use["FDA_GA"]   = 0;
   Use["FDA_MC"]   = 0;
   Use["FDA_MT"]   = 0;
   Use["FDA_GAMT"] = 0;
   //
   // --- Neural Network
   Use["MLP"] = 1;
   //
   // --- Support Vector Machine
   Use["SVM"] = 0;
   //
   // --- Boosted Decision Trees
   Use["BDT"]  = 0;
   Use["BDTG"] = 0;



   // Select methods
   if (myMethodList != "")
   {
      for (std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++)
      {
         it->second = 0;
      }
      std::vector<TString> mlist = gTools().SplitString(myMethodList, ',');
      for (UInt_t i = 0; i < mlist.size(); i++)
      {
         std::string regMethod(mlist[i]);
         if (Use.find(regMethod) == Use.end())
         {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++)
            {
               std::cout << it->first << " ";
            }
            std::cout << std::endl;
            exit(EXIT_FAILURE);
         }
         Use[regMethod] = 1;
      }
   }


   TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

   // Create a set of variables and declare them to the reader

   float alpha = xval;
   float r     = yval;

   reader->AddVariable("alpha_prob", &alpha);
   reader->AddVariable("r_prob", &r);


   // --- Book the MVA methods
   // string RegDir = m_topDir+m_particle+"/RegWeight/"+target+"_nbinsR"+std::to_string(m_nbinsR)+m_fileName;

   TString dir    = "RegWeight_" + target + "_neuron" + std::to_string(m_neurons) + "_nbinsR" + std::to_string(m_nbinsR) + m_fileName + "/weights/";
   TString prefix = "TMVARegression";

   std::cout << "===> Reg file = " << dir << std::endl;

   // Book method(s)
   for (std::map<std::string, int>::iterator it = Use.begin(); it != Use.end(); it++)
   {
      if (it->second)
      {
         TString methodName = it->first + " method";
         TString weightfile = dir + prefix + "_" + TString(it->first) + ".weights.xml";
         reader->BookMVA(methodName, weightfile);
      }
   }

   float val = (reader->EvaluateRegression("MLP method"))[0];
   float val2 = (reader->EvaluateRegression("MLP method"))[1];

   value.push_back(val);
   value.push_back(val2);


   if (reader)
   {
      delete reader;
   }


   return value;
}
