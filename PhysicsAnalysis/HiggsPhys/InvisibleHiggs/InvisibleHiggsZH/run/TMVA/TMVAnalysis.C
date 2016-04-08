/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @(#)root/tmva $Id: TMVAnalysis.C,v 1.1.1.1 2008-06-18 09:03:36 pgagnon Exp $
/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAnalysis                                                        *
 *                                                                                *
 * This macro provides examples for the training and testing of all the           *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l TMVAnalysis.C\(\"Fisher,Likelihood\"\)                              *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 **********************************************************************************/

#include <iostream>

#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
// requires links
// #include "TMVA/Factory.h"
// #include "TMVA/Tools.h"
// #include "TMVA/Config.h"

#include "TMVAGui.C"
   
// ---------------------------------------------------------------
// choose MVA methods to be trained + tested
Bool_t Use_Cuts            = 0;
Bool_t Use_CutsD           = 0;
Bool_t Use_CutsGA          = 0; // was 1
// ---
Bool_t Use_Likelihood      = 0; // was 1;
Bool_t Use_LikelihoodD     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
Bool_t Use_LikelihoodPCA   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
Bool_t Use_LikelihoodKDE   = 0;
Bool_t Use_LikelihoodMIX   = 0;
// ---
Bool_t Use_PDERS           = 0; // was 1;
Bool_t Use_PDERSD          = 0;
Bool_t Use_PDERSPCA        = 0;
Bool_t Use_KNN             = 0; // was 1;
// ---
Bool_t Use_HMatrix         = 0; // was 1;
Bool_t Use_Fisher          = 0; // was 1;
// ---
Bool_t Use_FDA_GA          = 0;
Bool_t Use_FDA_MC          = 0;
Bool_t Use_FDA_SA          = 0;
Bool_t Use_FDA_MT          = 0; // was 1;
Bool_t Use_FDA_GAMT        = 0;
Bool_t Use_FDA_MCMT        = 0;
// ---
Bool_t Use_MLP             = 0; // was 1; // this is the recommended ANN
Bool_t Use_CFMlpANN        = 0; 
Bool_t Use_TMlpANN         = 0; 
// ---
Bool_t Use_BDT             = 1;
Bool_t Use_BDTD            = 0;
// ---
Bool_t Use_RuleFitTMVA     = 0;
Bool_t Use_RuleFitJF       = 0;
// ---
Bool_t Use_SVM_Gauss       = 0; // was 1;
Bool_t Use_SVM_Poly        = 0;
Bool_t Use_SVM_Lin         = 0;
// ---------------------------------------------------------------

// read input data file with ascii format (otherwise ROOT) ?
Bool_t ReadDataFromAsciiIFormat = kFALSE;

void TMVAnalysis( TString myMethodList = "" ) 
{
   // explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the 
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAnalysis.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   TList* mlist = TMVA::Tools::ParseFormatLine( myMethodList, " :," );

   if (mlist->GetSize()>0) {
      Use_CutsGA = Use_CutsD = Use_Cuts
         = Use_LikelihoodKDE = Use_LikelihoodMIX = Use_LikelihoodPCA = Use_LikelihoodD = Use_Likelihood
         = Use_PDERSPCA = Use_PDERSD = Use_PDERS 
         = Use_KNN
         = Use_MLP = Use_CFMlpANN = Use_TMlpANN
         = Use_HMatrix = Use_Fisher = Use_BDTD = Use_BDT
         = Use_RuleFitTMVA = Use_RuleFitJF
         = Use_SVM_Gauss = Use_SVM_Poly = Use_SVM_Lin 
         = Use_FDA_GA = Use_FDA_MC = Use_FDA_SA = Use_FDA_MT = Use_FDA_GAMT = Use_FDA_MCMT 
         = 0;

      if (mlist->FindObject( "Cuts"          ) != 0) Use_Cuts          = 1; 
      if (mlist->FindObject( "CutsD"         ) != 0) Use_CutsD         = 1; 
      if (mlist->FindObject( "CutsGA"        ) != 0) Use_CutsGA        = 1; 
      if (mlist->FindObject( "Likelihood"    ) != 0) Use_Likelihood    = 1; 
      if (mlist->FindObject( "LikelihoodD"   ) != 0) Use_LikelihoodD   = 1; 
      if (mlist->FindObject( "LikelihoodPCA" ) != 0) Use_LikelihoodPCA = 1; 
      if (mlist->FindObject( "LikelihoodKDE" ) != 0) Use_LikelihoodKDE = 1; 
      if (mlist->FindObject( "LikelihoodMIX" ) != 0) Use_LikelihoodMIX = 1; 
      if (mlist->FindObject( "PDERSPCA"      ) != 0) Use_PDERSPCA      = 1; 
      if (mlist->FindObject( "PDERSD"        ) != 0) Use_PDERSD        = 1; 
      if (mlist->FindObject( "PDERS"         ) != 0) Use_PDERS         = 1; 
      if (mlist->FindObject( "KNN"           ) != 0) Use_KNN           = 1; 
      if (mlist->FindObject( "HMatrix"       ) != 0) Use_HMatrix       = 1; 
      if (mlist->FindObject( "Fisher"        ) != 0) Use_Fisher        = 1; 
      if (mlist->FindObject( "MLP"           ) != 0) Use_MLP           = 1; 
      if (mlist->FindObject( "CFMlpANN"      ) != 0) Use_CFMlpANN      = 1; 
      if (mlist->FindObject( "TMlpANN"       ) != 0) Use_TMlpANN       = 1; 
      if (mlist->FindObject( "BDTD"          ) != 0) Use_BDTD          = 1; 
      if (mlist->FindObject( "BDT"           ) != 0) Use_BDT           = 1; 
      if (mlist->FindObject( "RuleFitJF"     ) != 0) Use_RuleFitJF     = 1; 
      if (mlist->FindObject( "RuleFitTMVA"   ) != 0) Use_RuleFitTMVA   = 1; 
      if (mlist->FindObject( "SVM_Gauss"     ) != 0) Use_SVM_Gauss     = 1; 
      if (mlist->FindObject( "SVM_Poly"      ) != 0) Use_SVM_Poly      = 1; 
      if (mlist->FindObject( "SVM_Lin"       ) != 0) Use_SVM_Lin       = 1; 
      if (mlist->FindObject( "FDA_MC"        ) != 0) Use_FDA_MC        = 1; 
      if (mlist->FindObject( "FDA_GA"        ) != 0) Use_FDA_GA        = 1; 
      if (mlist->FindObject( "FDA_SA"        ) != 0) Use_FDA_SA        = 1; 
      if (mlist->FindObject( "FDA_MT"        ) != 0) Use_FDA_MT        = 1; 
      if (mlist->FindObject( "FDA_GAMT"      ) != 0) Use_FDA_GAMT      = 1; 
      if (mlist->FindObject( "FDA_MCMT"      ) != 0) Use_FDA_MCMT      = 1; 

      delete mlist;
   }
  
   std::cout << "Start Test TMVAnalysis" << std::endl
             << "======================" << std::endl
             << std::endl;
   std::cout << "Testing all standard methods may take about 10 minutes of running..." << std::endl;

   // Create a new root output file.
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory will
   // then run the performance analysis for you.
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/ 
   //
   // The second argument is the output file for the training results
   TMVA::Factory *factory = new TMVA::Factory( "TMVAnalysis", outputFile, Form("!V:%sColor", gROOT->IsBatch()?"!":"") );

   // if you wish to modify default settings 
   // (please check "src/Config.h" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

      // load the signal and background event samples from ROOT trees
//      TFile *input(0);
//       TString fname = "/data/TMVA/preselection-HZ140-all-Zincl.root";
//       //TString fname = "~/TMVA/examples/data/normalisation.root";
//       input = TFile::Open( fname );
//       if (!input) {
//          std::cout << "ERROR: could not open data file" << std::endl;
//          exit(1);
//       }
//       TTree *signal     = (TTree*)input->Get("TreeS");
//       TTree *background = (TTree*)input->Get("TreeB");

      int nfchains=0;
      int nfchainb=0;
      TChain* signal = new TChain("TreeS");
      TChain* background = new TChain("TreeB");
      nfchains += signal->Add("/data/TMVA/preselection-HZ250-10jun.root");

      //            nfchainb += background->Add("/data/TMVA/preselection-tt.root");
      //              nfchainb += background->Add("/data/TMVA/preselection-WW.root");
      //          nfchainb += background->Add("/data/TMVA/preselection-lt.root");
      //        nfchainb += background->Add("/data/TMVA/preselection-ZW.root");
       nfchainb += background->Add("/data/TMVA/preselection-Zi.root");

      // global event weights (see below for setting event-wise weights)
      Float_t w; 
      Float_t signalWeight;
      Float_t backgroundWeight;

      signal->SetBranchAddress("w",&w);
      signal->GetEntry(0,1);
      signalWeight = w;

      background->SetBranchAddress("w",&w);
      background->GetEntry(0,1);
      backgroundWeight = w;

//       cout << " " << endl;
//       cout << " " << endl;
//       cout << "***************************************************************************************" << endl;
//       cout << "Signal weight is: " << signalWeight << " and background weight is: " << backgroundWeight << " for file: " << fname << endl;
//       cout << "***************************************************************************************" << endl;
//       cout << " " << endl;
      cout << " " << endl;
      factory->AddSignalTree    ( signal,     signalWeight );
      factory->AddBackgroundTree( background, backgroundWeight );
    
   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]
      factory->AddVariable("njet", 'I');
      factory->AddVariable("ejet1", 'F');
      factory->AddVariable("ejet2", 'F');
      factory->AddVariable("ejet3", 'F');
      factory->AddVariable("ptlep1", 'F');
      factory->AddVariable("ptlep2", 'F');
      factory->AddVariable("cone1", 'F');
      factory->AddVariable("cone2", 'F');
//       factory->AddVariable("nel", 'I');
//       factory->AddVariable("nmu", 'I');
//       factory->AddVariable("nlep", 'I');
//       factory->AddVariable("lepID", 'I');
      factory->AddVariable("massZ", 'F');
      factory->AddVariable("cosll2d", 'F');
      factory->AddVariable("cosll3d", 'F');
      factory->AddVariable("mT", 'F');
      factory->AddVariable("MET", 'F');
      factory->AddVariable("cosMETptlep", 'F');
      factory->AddVariable("cosJetMET", 'F');
      factory->AddVariable("cosphi", 'F');

//PG    factory->AddVariable("var1+var2", 'F');
//    factory->AddVariable("var1-var2", 'F');
//    factory->AddVariable("var3", 'F');
//    factory->AddVariable("var4", 'F');

   // This would set individual event weights (the variables defined in the 
   // expression need to exist in the original TTree)
   factory->SetWeightExpression("w");
   cout << "weight: " << w << endl;

   // Apply additional cuts on the signal and background sample. 
   TCut mycut = ""; // for example: TCut mycut = "abs(var1)<0.5 && abs(var2-0.5)<1";

   // tell the factory to use all remaining events in the trees after training for testing:
   //PG   factory->PrepareTrainingAndTestTree( mycut, "NSigTrain=3000:NBkgTrain=3000:SplitMode=Random:NormMode=NumEvents:!V" );  

   factory->PrepareTrainingAndTestTree( mycut, "NSigTrain=0:NBkgTrain=0:SplitMode=Alternate:NormMode=None" );  

// For systematic studies, switch to Random mode instead of Alternate
   //   factory->PrepareTrainingAndTestTree( mycut, "NSigTrain=1357:NBkgTrain=2926:SplitMode=Alternate:NormMode=None" );  //WW
   //      factory->PrepareTrainingAndTestTree( mycut, "NSigTrain=1357:NBkgTrain=2956:SplitMode=Alternate:NormMode=None" );  //tt
//    factory->PrepareTrainingAndTestTree( mycut, "NSigTrain=0:NBkgTrain=0:SplitMode=Random:NormMode=None" );  
//    cout << " &&&&&&&&&&&&&&&& Switched to Random instead of Alternate mode for systematic studies  &&&&&&&&&&&&&&&&&&&&&&&&" << endl;
   // If no numbers of events are given, half of the events in the tree are used for training, and 
   // the other half for testing:
   //   factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );  
   // To also specify the number of testing events, use:
   //   factory->PrepareTrainingAndTestTree( mycut, 
   //                                        "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );  

   // ---- Book MVA methods
   //
   // please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc.
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable

   // Cut optimisation
   if (Use_Cuts) 
     factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
                          "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   if (Use_CutsD) 
     factory->BookMethod( TMVA::Types::kCuts, "CutsD", 
                          "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

   if (Use_CutsGA)
     factory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                         "!H:!V:FitMethod=GA:EffSel:Steps=30:Cycles=3:PopSize=100:SC_steps=10:SC_rate=5:SC_factor=0.95:VarProp=FSmart" );
   
   // Likelihood
   if (Use_Likelihood) 
      factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood", 
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=100:NSmoothBkg[0]=10:NSmoothBkg[1]=100:NSmooth=10:NAvEvtPerBin=50" ); 

   // test the decorrelated likelihood
   if (Use_LikelihoodD) 
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD", 
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=100:NSmoothBkg[0]=10:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate" ); 

   if (Use_LikelihoodPCA) 
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA", 
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=100:NSmoothBkg[0]=10:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" ); 
 
   // test the new kernel density estimator
   if (Use_LikelihoodKDE) 
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE", 
                           "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // test the mixed splines and kernel density estimator (depending on which variable)
   if (Use_LikelihoodMIX) 
      factory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX", 
                           "!H:!V:!TransformOutput:PDFInterpol[0]=KDE:PDFInterpol[1]=KDE:PDFInterpol[2]=Spline2:PDFInterpol[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" ); 

   // PDE - RS method
   if (Use_PDERS)
      factory->BookMethod( TMVA::Types::kPDERS, "PDERS", 
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:InitialScale=0.99" );
   
   if (Use_PDERSD) 
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSD", 
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:InitialScale=0.99:VarTransform=Decorrelate" );

   if (Use_PDERSPCA) 
      factory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA", 
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:InitialScale=0.99:VarTransform=PCA" );
  
   // K-Nearest Neighbour classifier (KNN)
   if (Use_KNN)
      factory->BookMethod( TMVA::Types::kKNN, "KNN", 
                           "nkNN=40:TreeOptDepth=6:ScaleFrac=0.8:!UseKernel:!Trim" );  

   // H-Matrix (chi2-squared) method
   if (Use_HMatrix)
      factory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V" ); 

   // Fisher discriminant
   if (Use_Fisher)
      factory->BookMethod( TMVA::Types::kFisher, "Fisher", 
                           "H:!V:!Normalise:CreateMVAPdfs:Fisher:NbinsMVAPdf=50:NsmoothMVAPdf=1" );    

   // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit or GA
   if (Use_FDA_MC) 
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                          "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );
   
   if (Use_FDA_GA) 
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=100:Cycles=3:Steps=20:Trim=True:SaveBestGen=0" );

   if (Use_FDA_SA) 
      factory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=50000:TemperatureGradient=0.7:InitialTemperature=2000000:MinTemperature=500:Eps=1e-04:NFunLoops=5:NEps=4" );

   if (Use_FDA_MT) 
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

   if (Use_FDA_GAMT) 
      factory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

   if (Use_FDA_MCMT) 
      factory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

   // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
   if (Use_MLP)
      factory->BookMethod( TMVA::Types::kMLP, "MLP", "Normalise:H:!V:NCycles=200:HiddenLayers=N+1,N:TestRate=5" );

   // CF(Clermont-Ferrand)ANN
   if (Use_CFMlpANN)
      factory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=500:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...  
  
   // Tmlp(Root)ANN
   if (Use_TMlpANN)
      factory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...
  
   // Support Vector Machines using three different Kernel types (Gauss, polynomial and linear)
   if (Use_SVM_Gauss)
      factory->BookMethod( TMVA::Types::kSVM, "SVM_Gauss", "Sigma=2:C=1:Tol=0.001:Kernel=Gauss" );

   if (Use_SVM_Poly)
      factory->BookMethod( TMVA::Types::kSVM, "SVM_Poly", "Order=4:Theta=1:C=0.1:Tol=0.001:Kernel=Polynomial" );

   if (Use_SVM_Lin)
      factory->BookMethod( TMVA::Types::kSVM, "SVM_Lin", "!H:!V:Kernel=Linear:C=1:Tol=0.001" );  

   // Boosted Decision Trees (second one with decorrelation)
   if (Use_BDT)
      factory->BookMethod( TMVA::Types::kBDT, "BDT", 
                           "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5" );
   if (Use_BDTD)
      factory->BookMethod( TMVA::Types::kBDT, "BDTD", 
                           "!H:!V:NTrees=400:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=CostComplexity:PruneStrength=4.5:VarTransform=Decorrelate" );

   // RuleFit -- TMVA implementation of Friedman's method
   if (Use_RuleFitTMVA)
      factory->BookMethod( TMVA::Types::kRuleFit, "RuleFitTMVA",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

   // Friedman's RuleFit method, implementation by J. Friedman
   if (Use_RuleFitJF)
      factory->BookMethod( TMVA::Types::kRuleFit, "RuleFitJF",
                           "!V:RuleFitModule=RFFriedman:Model=ModRuleLinear:GDStep=0.01:GDNSteps=10000:GDErrScale=1.1:RFNendnodes=4" );

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();    

   // --------------------------------------------------------------
   
   // Save the output
   outputFile->Close();

   std::cout << "==> wrote root file TMVA.root" << std::endl;
   std::cout << "==> TMVAnalysis is done!" << std::endl;      

   // Clean up
   delete factory;

   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()) TMVAGui( outfileName );
}
