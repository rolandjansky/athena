/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVApplication                                                     *
 *                                                                                *
 * This exectutable provides a simple example on how to use the trained MVAs      *
 * within a C++ analysis module                                                   *
 *                                                                                *
 * ------------------------------------------------------------------------------ *
 * see also the alternative (slightly faster) way to retrieve the MVA values in   *
 * examples/TMVApplicationAlternative.cxx                                         *
 * ------------------------------------------------------------------------------ *
 **********************************************************************************/

// ---------------------------------------------------------------
// choose MVA methods to be trained + tested
Bool_t Use_Cuts            = 0;
Bool_t Use_CutsD           = 0;
Bool_t Use_CutsGA          = 0;
Bool_t Use_Likelihood      = 0;
Bool_t Use_LikelihoodD     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
Bool_t Use_LikelihoodPCA   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
Bool_t Use_PDERS           = 0;
Bool_t Use_PDERSD          = 0;
Bool_t Use_PDERSPCA        = 0;
Bool_t Use_KNN             = 0;
Bool_t Use_HMatrix         = 0;
Bool_t Use_Fisher          = 0;
Bool_t Use_FDA_GA          = 0;
Bool_t Use_FDA_MT          = 0;
Bool_t Use_MLP             = 0; // this is the recommended ANN
Bool_t Use_CFMlpANN        = 0; 
Bool_t Use_TMlpANN         = 0; 
Bool_t Use_SVM_Gauss       = 0;
Bool_t Use_SVM_Poly        = 0;
Bool_t Use_SVM_Lin         = 0;
Bool_t Use_BDT             = 1;
Bool_t Use_BDTD            = 0;
Bool_t Use_RuleFit         = 0;

// ---------------------------------------------------------------

void TMVApplication( TString myMethodList = "" ) {
  cout << endl;
  cout << "==> start TMVApplication" << endl;

  if (myMethodList != "") {
      Use_CutsGA = Use_CutsD = Use_Cuts
      = Use_LikelihoodPCA = Use_LikelihoodD = Use_Likelihood
      = Use_PDERSPCA = Use_PDERSD = Use_PDERS 
      = Use_KNN
      = Use_MLP = Use_CFMlpANN = Use_TMlpANN
      = Use_HMatrix = Use_Fisher = Use_BDTD = Use_BDT = Use_RuleFit 
      = Use_SVM_Gauss = Use_SVM_Poly = Use_SVM_Lin
      = Use_FDA_GA = Use_FDA_MT
         = 0;

    TList* mlist = TMVA::Tools::ParseFormatLine( myMethodList, " :," );

//     if (mlist->FindObject( "Cuts"          ) != 0) Use_Cuts          = 1; 
//     if (mlist->FindObject( "CutsD"         ) != 0) Use_CutsD         = 1; 
//     if (mlist->FindObject( "CutsGA"        ) != 0) Use_CutsGA        = 1; 
    if (mlist->FindObject( "Likelihood"    ) != 0) Use_Likelihood    = 1; 
    if (mlist->FindObject( "LikelihoodD"   ) != 0) Use_LikelihoodD   = 1; 
    if (mlist->FindObject( "LikelihoodPCA" ) != 0) Use_LikelihoodPCA = 1; 
    if (mlist->FindObject( "PDERS"         ) != 0) Use_PDERS         = 1; 
    if (mlist->FindObject( "PDERSD"        ) != 0) Use_PDERSD        = 1; 
    if (mlist->FindObject( "PDERSPCA"      ) != 0) Use_PDERSPCA      = 1; 
    if (mlist->FindObject( "KNN"           ) != 0) Use_KNN           = 1; 
    if (mlist->FindObject( "HMatrix"       ) != 0) Use_HMatrix       = 1; 
    if (mlist->FindObject( "Fisher"        ) != 0) Use_Fisher        = 1; 
    if (mlist->FindObject( "MLP"           ) != 0) Use_MLP           = 1; 
    if (mlist->FindObject( "CFMlpANN"      ) != 0) Use_CFMlpANN      = 1; 
    if (mlist->FindObject( "TMlpANN"       ) != 0) Use_TMlpANN       = 1; 
    if (mlist->FindObject( "BDTD"          ) != 0) Use_BDTD          = 1; 
    if (mlist->FindObject( "BDT"           ) != 0) Use_BDT           = 1; 
    if (mlist->FindObject( "RuleFit"       ) != 0) Use_RuleFit       = 1; 
    if (mlist->FindObject( "SVM_Gauss"     ) != 0) Use_SVM_Gauss     = 1; 
    if (mlist->FindObject( "SVM_Poly"      ) != 0) Use_SVM_Poly      = 1; 
    if (mlist->FindObject( "SVM_Lin"       ) != 0) Use_SVM_Lin       = 1; 
    if (mlist->FindObject( "FDA_MT"        ) != 0) Use_FDA_MT        = 1; 
    if (mlist->FindObject( "FDA_GA"        ) != 0) Use_FDA_GA        = 1; 

    delete mlist;
  }

// enter the number of different types of events and the # of background weight files used
  Int_t const ntype = 8;
  Int_t const nbgndw = 7;

// give the desired names and cut values for the backgrounds  
  string suffix[7] = {("HZ130-ZZ"), ("HZ250-tt"), ("HZ250-WW"), ("HZ130-tn"), ("HZ250-lt"), ("HZ250-ZW"), ("HZ250-Zi")}; 
  float cutValue[7] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};  // 
  float cutProduct = -1.0;
 
  Int_t seen[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  Float_t weight[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  //   Float_t preselected[8] = {18799., 13650., 5911., 5852., 65., 1220., 9191., 104.};  // preselection for 120 GeV 28may
  //    Float_t preselected[8] = {2674., 13650., 5911., 5852., 65., 1220., 9191., 104.};  //  preselection for 130 GeV
  //  Float_t preselected[8] = { 2972, 13650., 5911., 5852., 65., 1220., 9191., 104.};  //  preselection for 140 GeV
  float mvaCuts[nbgndw], mvaCutsD[nbgndw], mvaLk[nbgndw], mvaLkD[nbgndw], mvaLkPCA[nbgndw], mvaPD[nbgndw], mvaPDD[nbgndw];
  float mvaPDP[nbgndw], mvaPDPCA[nbgndw], mvaHm[nbgndw], mvaFi[nbgndw], mvaNn[nbgndw], ;
  float mvaNn[nbgndw], mvaNnC[nbgndw], mvaNnT[nbgndw], mvaBdt[nbgndw], mvaBdtD[nbgndw], mvaRf[nbgndw];

// create a set of variables and declare them to the reader
// - the variable names must corresponds in name and type to 
// those given in the weight file(s) that you use
 
// read the TMVA weights of several different types of background 

  Int_t njet, nel, nmu, nlep, lepID;
  Float_t ejet1, ejet2, ejet3, ptlep1, ptlep2, cone1, cone2, massZ;
  Float_t cosll2d, cosll3d, mT, MET, cosMETptlep, cosJetMET, cosphi;

  TMVA::Reader *reader[nbgndw];

  for (int i=0; i<nbgndw; i++) {


// create the TMVA_Reader objects
//
    reader[i]= new TMVA::Reader();    
  
    reader[i]->AddVariable( "njet", &njet);
    reader[i]->AddVariable( "ejet1", &ejet1);
    reader[i]->AddVariable( "ejet2", &ejet2);
    reader[i]->AddVariable( "ejet3", &ejet3);
    reader[i]->AddVariable( "ptlep1", &ptlep1);
    reader[i]->AddVariable( "ptlep2", &ptlep2);
    reader[i]->AddVariable( "cone1", &cone1);
    reader[i]->AddVariable( "cone2", &cone2);
    reader[i]->AddVariable( "massZ", &massZ);
    reader[i]->AddVariable( "cosll2d", &cosll2d);
    reader[i]->AddVariable( "cosll3d", &cosll3d);
    reader[i]->AddVariable( "mT", &mT);
    reader[i]->AddVariable( "MET", &MET);
    reader[i]->AddVariable( "cosMETptlep", &cosMETptlep);
    reader[i]->AddVariable( "cosJetMET", &cosJetMET);
    reader[i]->AddVariable( "cosphi", &cosphi);

//// book the MVA methods

    string dir    = "weights/";
//   string prefix = "MVAnalysis";

//     if (Use_Cuts)          reader[i]->BookMVA( "Cuts method",          dir + suffix[i] + "_Cuts.weights.txt" );
//     if (Use_CutsD)         reader[i]->BookMVA( "CutsD method",         dir + suffix[i] + "_CutsD.weights.txt" );
//     if (Use_CutsGA)        reader[i]->BookMVA( "CutsGA method",        dir + suffix[i] + "_CutsGA.weights.txt" );
    if (Use_Likelihood)    reader[i]->BookMVA( "Likelihood method",    dir + suffix[i] + "_Likelihood.weights.txt" );
    if (Use_LikelihoodD)   reader[i]->BookMVA( "LikelihoodD method",   dir + suffix[i] + "_LikelihoodD.weights.txt" );
    if (Use_LikelihoodPCA) reader[i]->BookMVA( "LikelihoodPCA method", dir + suffix[i] + "_LikelihoodPCA.weights.txt" );
    if (Use_PDERS)         reader[i]->BookMVA( "PDERS method",         dir + suffix[i] + "_PDERS.weights.txt" );
    if (Use_PDERSD)        reader[i]->BookMVA( "PDERSD method",        dir + suffix[i] + "_PDERSD.weights.txt" );
    if (Use_PDERSPCA)      reader[i]->BookMVA( "PDERSPCA method",      dir + suffix[i] + "_PDERSPCA.weights.txt" );
    if (Use_KNN)           reader[i]->BookMVA( "KNN method",           dir + suffix[i] + "_KNN.weights.txt" );
    if (Use_HMatrix)       reader[i]->BookMVA( "HMatrix method",       dir + suffix[i] + "_HMatrix.weights.txt" );
    if (Use_Fisher)        reader[i]->BookMVA( "Fisher method",        dir + suffix[i] + "_Fisher.weights.txt" );
    if (Use_MLP)           reader[i]->BookMVA( "MLP method",           dir + suffix[i] + "_MLP.weights.txt" );
    if (Use_CFMlpANN)      reader[i]->BookMVA( "CFMlpANN method",      dir + suffix[i] + "_CFMlpANN.weights.txt" );
    if (Use_TMlpANN)       reader[i]->BookMVA( "TMlpANN method",       dir + suffix[i] + "_TMlpANN.weights.txt" );
    if (Use_BDT)           reader[i]->BookMVA( "BDT method",           dir + suffix[i] + "_BDT.weights.txt" );
    if (Use_BDTD)          reader[i]->BookMVA( "BDTD method",          dir + suffix[i] + "_BDTD.weights.txt" );
    if (Use_RuleFit)       reader[i]->BookMVA( "RuleFit method",       dir + suffix[i] + "_RuleFitTMVA.weights.txt" );
    if (Use_SVM_Gauss)     reader[i]->BookMVA( "SVM_Gauss method",     dir + suffix[i] + "_SVM_Gauss.weights.txt" );
    if (Use_SVM_Poly)      reader[i]->BookMVA( "SVM_Poly method",      dir + suffix[i] + "_SVM_Poly.weights.txt" );
    if (Use_SVM_Lin)       reader[i]->BookMVA( "SVM_Lin method",       dir + suffix[i] + "_SVM_Lin.weights.txt" );
    if (Use_FDA_MT)        reader[i]->BookMVA( "FDA_MT method",        dir + suffix[i] + "_FDA_MT.weights.txt" );
    if (Use_FDA_GA)        reader[i]->BookMVA( "FDA_GA method",        dir + suffix[i] + "_FDA_GA.weights.txt" );

// end loop over the different types of background weight files
     }
// book output histograms
  UInt_t nbin = 100;
  TH1F *histLk, *histLkD, *histLkPCA, *histPD, *histPDD, *histPDPCA, *histKNN, *histHm, *histFi;
  TH1F *histNn, *histNnC, *histNnT, *histBdt, *histBdtD, *histRf;
  TH1F *histSVMG, *histSVMP, *histSVML;
  TH1F *histFDAMT, *histFDAGA;

  if (Use_Likelihood)    histLk    = new TH1F( "MVA_Likelihood",    "MVA_Likelihood",    nbin,  0, 1 );
  if (Use_LikelihoodD)   histLkD   = new TH1F( "MVA_LikelihoodD",   "MVA_LikelihoodD",   nbin,  0.000001, 0.9999 );
  if (Use_LikelihoodPCA) histLkPCA = new TH1F( "MVA_LikelihoodPCA", "MVA_LikelihoodPCA", nbin,  0, 1 );
  if (Use_PDERS)         histPD    = new TH1F( "MVA_PDERS",         "MVA_PDERS",         nbin,  0, 1 );
  if (Use_PDERSD)        histPDD   = new TH1F( "MVA_PDERSD",        "MVA_PDERSD",        nbin,  0, 1 );
  if (Use_PDERSPCA)      histPDPCA = new TH1F( "MVA_PDERSPCA",      "MVA_PDERSPCA",      nbin,  0, 1 );
  if (Use_KNN)           histKNN   = new TH1F( "MVA_KNN",           "MVA_KNN",           nbin,  0, 1 );
  if (Use_HMatrix)       histHm    = new TH1F( "MVA_HMatrix",       "MVA_HMatrix",       nbin, -0.95, 1.55 );
  if (Use_Fisher)        histFi    = new TH1F( "MVA_Fisher",        "MVA_Fisher",        nbin, -4, 4 );
  if (Use_MLP)           histNn    = new TH1F( "MVA_MLP",           "MVA_MLP",           nbin, -0.25, 1.5 );
  if (Use_CFMlpANN)      histNnC   = new TH1F( "MVA_CFMlpANN",      "MVA_CFMlpANN",      nbin,  0, 1 );
  if (Use_TMlpANN)       histNnT   = new TH1F( "MVA_TMlpANN",       "MVA_TMlpANN",       nbin, -1.3, 1.3 );
  if (Use_BDT)           histBdt   = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.8, 0.8 );
  if (Use_BDTD)          histBdtD  = new TH1F( "MVA_BDTD",          "MVA_BDTD",          nbin, -0.4, 0.6 );
  if (Use_RuleFit)       histRf    = new TH1F( "MVA_RuleFitTMVA",   "MVA_RuleFitTMVA",   nbin, -2.0, 2.0 );
  if (Use_SVM_Gauss)     histSVMG  = new TH1F( "MVA_SVM_Gauss",     "MVA_SVM_Gauss",     nbin, 0.0, 1.0 );
  if (Use_SVM_Poly)      histSVMP  = new TH1F( "MVA_SVM_Poly",      "MVA_SVM_Poly",      nbin, 0.0, 1.0 );
  if (Use_SVM_Lin)       histSVML  = new TH1F( "MVA_SVM_Lin",       "MVA_SVM_Lin",       nbin, 0.0, 1.0 );
  if (Use_FDA_MT)        histFDAMT = new TH1F( "MVA_FDA_MT",        "MVA_FDA_MT",        nbin, -2.0, 3.0 );
  if (Use_FDA_GA)        histFDAGA = new TH1F( "MVA_FDA_GA",        "MVA_FDA_GA",        nbin, -2.0, 3.0 );

// book examsple histogram for probability (the other methods are done similarly)
  TH1F *probHistFi, *rarityHistFi;
  if (Use_Fisher) {
    probHistFi   = new TH1F( "PROBA_MVA_Fisher",  "PROBA_MVA_Fisher",  nbin, 0, 1 );
    rarityHistFi = new TH1F( "RARITY_MVA_Fisher", "RARITY_MVA_Fisher", nbin, 0, 1 );
  }

// open a new ROOT output file
  TFile final("/data/TMVA/final/final.root", "recreate");

// create a new tree to save additional info

  Float_t bdtZZ = 0.0;
  Float_t bdttt = 0.0;
  Float_t bdtWW = 0.0;
  Float_t bdttn = 0.0;
  Float_t bdtlt = 0.0;
  Float_t bdtZW = 0.0;
  Float_t bdtZi = 0.0;

  Int_t runnum;
  Int_t evtnum, type;
  Float_t w;

  TTree finalTree ("finalTree", "TMVA weights");
  finalTree.Branch("runnum",&runnum,"runnum/I");	         
  finalTree.Branch("evtnum",&evtnum,"evtnum/I");	         
  finalTree.Branch("type",&type,"type/I");	         
  finalTree.Branch("bdtZZ",&bdtZZ,"bdtZZ/F");	         
  finalTree.Branch("bdttt",&bdttt,"bdttt/F");	         
  finalTree.Branch("bdtWW",&bdtWW,"bdtWW/F");	         
  finalTree.Branch("bdttn",&bdttn,"bdttn/F");	         
  finalTree.Branch("bdtlt",&bdtlt,"bdtlt/F");	         
  finalTree.Branch("bdtZW",&bdtZW,"bdtZW/F");	         
  finalTree.Branch("bdtZi",&bdtZi,"bdtZi/F");	         
  finalTree.Branch("mT",&mT,"mT/F");	         
  finalTree.Branch("massZ",&massZ,"massZ/F");	         
  finalTree.Branch("cosll2d",&cosll2d,"cosll2d/F");	         
  finalTree.Branch("cosll3d",&cosll3d,"cosll3d/F");	         
  finalTree.Branch("cosMETptlep",&cosMETptlep,"cosMETptlep/F");	         
  finalTree.Branch("cosphi",&cosphi,"cosphi/F");	         
  finalTree.Branch("MET",&MET,"MET/F");	         
  finalTree.Branch("w",&w,"w/F");	         

// declare all these trees to be friends
//   finalTree.AddFriend("TreeS",fname);
//   finalTree.AddFriend("TreeB",fname);

 // book output histograms
   UInt_t nbin = 100;
	TH1F *histLk, *histLkD, *histLkPCA, *histPD, *histPDD, *histPDPCA, *histHm, *histFi;
   TH1F *histNn, *histNnC, *histNnT, *histBdt, *histBdtD, *histRf;
   if (Use_Likelihood)    histLk    = new TH1F( "MVA_Likelihood",    "MVA_Likelihood",    nbin,  0, 1 );
   if (Use_LikelihoodD)   histLkD   = new TH1F( "MVA_LikelihoodD",   "MVA_LikelihoodD",   nbin,  0.000001, 0.9999 );
   if (Use_LikelihoodPCA) histLkPCA = new TH1F( "MVA_LikelihoodPCA", "MVA_LikelihoodPCA", nbin,  0, 1 );
   if (Use_PDERS)         histPD    = new TH1F( "MVA_PDERS",         "MVA_PDERS",         nbin,  0, 1 );
   if (Use_PDERSD)        histPDD   = new TH1F( "MVA_PDERSD",        "MVA_PDERSD",        nbin,  0, 1 );
   if (Use_PDERSPCA)      histPDPCA = new TH1F( "MVA_PDERSPCA",      "MVA_PDERSPCA",      nbin,  0, 1 );
   if (Use_HMatrix)       histHm    = new TH1F( "MVA_HMatrix",       "MVA_HMatrix",       nbin, -0.95, 1.55 );
   if (Use_Fisher)        histFi    = new TH1F( "MVA_Fisher",        "MVA_Fisher",        nbin, -4, 4 );
   if (Use_MLP)           histNn    = new TH1F( "MVA_MLP",           "MVA_MLP",           nbin, -0.25, 1.5 );
   if (Use_CFMlpANN)      histNnC   = new TH1F( "MVA_CFMlpANN",      "MVA_CFMlpANN",      nbin,  0, 1 );
   if (Use_TMlpANN)       histNnT   = new TH1F( "MVA_TMlpANN",       "MVA_TMlpANN",       nbin, -1, 1 );
   if (Use_BDT)           histBdt   = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.4, 0.6 );
   if (Use_BDTD)          histBdtD  = new TH1F( "MVA_BDTD",          "MVA_BDTD",          nbin, -0.4, 0.6 );
   if (Use_RuleFit)       histRf    = new TH1F( "MVA_RuleFit",       "MVA_RuleFit",       nbin, -1.3, 1.3 );

   // 
   //
   // prepare the tree
   // - here the variable names have to corresponds to your tree
   // - you can use the same variables as above which is slightly faster,
   //   but of course you can use different ones and copy the values inside the event loop
   //

//    TFile *input(0);
//    cout << "--- accessing data file: " << fname << endl;
//    input = TFile::Open( fname );

//    TTree* theTree = (TTree*)input->Get("TreeS");

//   TTree* theTree;
//    for (int choice=0; choice<2; choice++) {
//     if (choice == 0)  theTree = (TTree*)input->Get("TreeS");
//     if (choice == 1)  theTree = (TTree*)input->Get("TreeB");

   int nfchain=0;
   TChain* theTree = 0;
   for (int choice=0; choice<2; choice++) {
     //    if (theTree) delete theTree;
     if (choice == 0)  theTree = new TChain("TreeS");
     if (choice == 1)  theTree = new TChain("TreeB");

     nfchain += theTree->Add("/data/TMVA/preselection-HZ250-10jun.root");
     nfchain += theTree->Add("/data/TMVA/preselection-ZZ.root");
     nfchain += theTree->Add("/data/TMVA/preselection-tt.root");
     nfchain += theTree->Add("/data/TMVA/preselection-WW.root");
     nfchain += theTree->Add("/data/TMVA/preselection-tn.root");
     nfchain += theTree->Add("/data/TMVA/preselection-lt.root");
     nfchain += theTree->Add("/data/TMVA/preselection-ZW.root");
     nfchain += theTree->Add("/data/TMVA/preselection-Zi.root");

    theTree->SetBranchAddress( "njet", &njet );
    theTree->SetBranchAddress( "ejet1", &ejet1 );
    theTree->SetBranchAddress( "ejet2", &ejet2 );
    theTree->SetBranchAddress( "ejet3", &ejet3 );
    theTree->SetBranchAddress( "ptlep1", &ptlep1 );
    theTree->SetBranchAddress( "ptlep2", &ptlep2 );
    theTree->SetBranchAddress( "cone1", &cone1 );
    theTree->SetBranchAddress( "cone2", &cone2 );
    theTree->SetBranchAddress( "nel", &nel );
    theTree->SetBranchAddress( "nmu", &nmu );
    theTree->SetBranchAddress( "nlep", &nlep );
    theTree->SetBranchAddress( "lepID", &lepID );
    theTree->SetBranchAddress( "massZ", &massZ );
    theTree->SetBranchAddress( "cosll2d", &cosll2d );
    theTree->SetBranchAddress( "cosll3d", &cosll3d );
    theTree->SetBranchAddress( "mT", &mT );
    theTree->SetBranchAddress( "MET", &MET );
    theTree->SetBranchAddress( "cosMETptlep", &cosMETptlep);
    theTree->SetBranchAddress( "cosJetMET", &cosJetMET);
    theTree->SetBranchAddress( "cosphi", &cosphi );
    theTree->SetBranchAddress( "runnum", &runnum );
    theTree->SetBranchAddress( "evtnum", &evtnum );
    theTree->SetBranchAddress( "type", &type );
    theTree->SetBranchAddress( "w", &w );

// efficiency calculator for cut method
    int    nSelCuts = 0;
    float effS     = 0.7;

    cout << "--- processing: " << theTree->GetEntries() << " events" << endl; 

    TStopwatch sw;
    sw.Start();

// loop over all events
    int counter = 0;
    cout << "Found " << theTree->GetEntries() << " for choice = " << choice << endl;
    for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {

      theTree->GetEntry(ievt);

// store the weight for each event type
      weight[type] = w;
      //      cout << "weight: " << w <<endl;
      
      if (ievt == 0) cout << "--- ... processing the first event " << endl;
      if (ievt%1000 == 0) cout << "--- ... processing event: " << ievt << endl;
      if (ievt==theTree->GetEntries()-1) cout << "--- ... processed last event: " << ievt << endl;

// only retain half the events, namely the even events to avoid reusing the same events as for training
      seen[type]++;
      if ((seen[type] % 2) == 0) {
	counter++;    //

// now extract the value of tmva for each background type for this event
	for (int i=0; i<nbgndw; i++) {

// 
// return the MVAs and fill to histograms
// 
// 	  if (Use_Cuts) {
// // Cuts is a special case: give the desired signal efficienciy
// 	    Bool_t passed = reader[i]->EvaluateMVA( "Cuts method", effS );
// 	    if (passed) nSelCuts++;
// 	  }
// 	  if (Use_CutsD) {
// // Cuts is a special case: give the desired signal efficienciy
// 	    Bool_t passed = reader[i]->EvaluateMVA( "CutsD method", effS );
// 	    if (passed) nSelCutsD++;
// 	  }
// 	  if (Use_CutsGA) {
// // Cuts is a special case: give the desired signal efficienciy
// 	    Bool_t passed = reader[i]->EvaluateMVA( "CutsGA method", effS );
// 	    if (passed) nSelCutsGA++;
// 	  }

	  if (Use_Likelihood   )   histLk    ->Fill( reader[i]->EvaluateMVA( "Likelihood method"    ) );
	  if (Use_LikelihoodD  )   histLkD   ->Fill( reader[i]->EvaluateMVA( "LikelihoodD method"   ) );
	  if (Use_LikelihoodPCA)   histLkPCA ->Fill( reader[i]->EvaluateMVA( "LikelihoodPCA method" ) );
	  if (Use_PDERS        )   histPD    ->Fill( reader[i]->EvaluateMVA( "PDERS method"         ) );
	  if (Use_PDERSD       )   histPDD   ->Fill( reader[i]->EvaluateMVA( "PDERSD method"        ) );
	  if (Use_PDERSPCA     )   histPDPCA ->Fill( reader[i]->EvaluateMVA( "PDERSPCA method"      ) );
	  if (Use_KNN          )   histKNN   ->Fill( reader[i]->EvaluateMVA( "KNN method"           ) );
	  if (Use_HMatrix      )   histHm    ->Fill( reader[i]->EvaluateMVA( "HMatrix method"       ) );
	  if (Use_Fisher       )   histFi    ->Fill( reader[i]->EvaluateMVA( "Fisher method"        ) );
	  if (Use_MLP          )   histNn    ->Fill( reader[i]->EvaluateMVA( "MLP method"           ) );
	  if (Use_CFMlpANN     )   histNnC   ->Fill( reader[i]->EvaluateMVA( "CFMlpANN method"      ) );
	  if (Use_TMlpANN      )   histNnT   ->Fill( reader[i]->EvaluateMVA( "TMlpANN method"       ) );
	  if (Use_BDT          )   histBdt   ->Fill( reader[i]->EvaluateMVA( "BDT method"           ) );
	  if (Use_BDTD         )   histBdtD  ->Fill( reader[i]->EvaluateMVA( "BDTD method"          ) );
	  if (Use_RuleFit      )   histRf    ->Fill( reader[i]->EvaluateMVA( "RuleFit method"       ) );
	  if (Use_SVM_Gauss    )   histSVMG  ->Fill( reader[i]->EvaluateMVA( "SVM_Gauss method"     ) );
	  if (Use_SVM_Poly     )   histSVMP  ->Fill( reader[i]->EvaluateMVA( "SVM_Poly method"      ) );
	  if (Use_SVM_Lin      )   histSVML  ->Fill( reader[i]->EvaluateMVA( "SVM_Lin method"       ) );
	  if (Use_FDA_MT       )   histFDAMT ->Fill( reader[i]->EvaluateMVA( "FDA_MT method"        ) );
	  if (Use_FDA_GA       )   histFDAGA ->Fill( reader[i]->EvaluateMVA( "FDA_GA method"        ) );
	  
      // retrieve probability instead of MVA output
	  if (Use_Fisher       )   {
	    probHistFi  ->Fill( reader[i]->GetProba ( "Fisher method" ) );
	    rarityHistFi->Fill( reader[i]->GetRarity( "Fisher method" ) );
	  }

// 	  if (Use_Cuts         )   mvaCuts[i] = reader[i]->EvaluateMVA( "Cuts method"          );
// 	  if (Use_CutsD        )   mvaCutsD[i]= reader[i]->EvaluateMVA( "CutsD method"         );
// 	  if (Use_Likelihood   )   mvaLk[i]   = reader[i]->EvaluateMVA( "Likelihood method"    );
// 	  if (Use_LikelihoodD  )   mvaLkD[i]  = reader[i]->EvaluateMVA( "LikelihoodD method"   );
// 	  if (Use_LikelihoodPCA)   mvaLkPCA[i]= reader[i]->EvaluateMVA( "LikelihoodPCA method" );
// 	  if (Use_PDERS        )   mvaPD[i]   = reader[i]->EvaluateMVA( "PDERS method"         );
// 	  if (Use_PDERSD       )   mvaPDD[i]  = reader[i]->EvaluateMVA( "PDERSD method"        );
// 	  if (Use_PDERSPCA     )   mvaPDPCA[i]= reader[i]->EvaluateMVA( "PDERSPCA method"      );
// 	  if (Use_HMatrix      )   mvaHm[i]   = reader[i]->EvaluateMVA( "HMatrix method"       );
// 	  if (Use_Fisher       )   mvaFi[i]   = reader[i]->EvaluateMVA( "Fisher method"        );
// 	  if (Use_MLP          )   mvaNn[i]   = reader[i]->EvaluateMVA( "MLP method"           );
// 	  if (Use_CFMlpANN     )   mvaNnC[i]  = reader[i]->EvaluateMVA( "CFMlpANN method"      );
// 	  if (Use_TMlpANN      )   mvaNnT[i]  = reader[i]->EvaluateMVA( "TMlpANN method"       );
	  if (Use_BDT          )   mvaBdt[i]  = reader[i]->EvaluateMVA( "BDT method"           );        
// 	  if (Use_BDTD         )   mvaBdtD[i] = reader[i]->EvaluateMVA( "BDTD method"          );        
// 	  if (Use_RuleFit      )   mvaRf [i]  = reader[i]->EvaluateMVA( "RuleFit method"       );        

	  if (i == 0) bdtZZ = mvaBdt[i];
	  if (i == 1) bdttt = mvaBdt[i];
	  if (i == 2) bdtWW = mvaBdt[i];
	  if (i == 3) bdttn = mvaBdt[i];
	  if (i == 4) bdtlt = mvaBdt[i];
	  if (i == 5) bdtZW = mvaBdt[i];
	  if (i == 6) bdtZi = mvaBdt[i];
// 
// end loop on background types to extract each separate BDT value
//  
	}
// if using the product of the output variables, apply the cut here:
//	cout << type << "\t" << bdtZZ << "\t" << bdttt << "\t" << bdtWW << "\t" << w << endl;
	finalTree.Fill();  
 
// end loop on reading only the second half of events
      }
// end loop on reading all events
    } 
    sw.Stop();

// get elapsed time
//     if (Use_Cuts)   cout << "--- efficiency for Cuts method  : " << float(nSelCuts)/theTree->GetEntries()
//                         << " (for a required signal efficiency of " << effS << ")" << endl;
//     if (Use_CutsD)  cout << "--- efficiency for CutsD method : " << float(nSelCutsD)/theTree->GetEntries()
//                         << " (for a required signal efficiency of " << effS << ")" << endl;
//     if (Use_CutsGA) cout << "--- efficiency for CutsGA method: " << float(nSelCutsGA)/theTree->GetEntries()
//                         << " (for a required signal efficiency of " << effS << ")" << endl;
    cout << "--- end of event loop" << endl;
  
//end loop on choice of reading signal or backgrounds
   }

//
// write histograms
//
   finalTree.Print();
   //   finalTree.Close();
   final.Write();
   //   final.Close();

   TFile *target  = new TFile( "TMVApp.root","RECREATE" );
   if (Use_Likelihood   )   histLk    ->Write();
   if (Use_LikelihoodD  )   histLkD   ->Write();
   if (Use_LikelihoodPCA)   histLkPCA ->Write();
   if (Use_PDERS        )   histPD    ->Write();
   if (Use_PDERSD       )   histPDD   ->Write();
   if (Use_PDERSPCA     )   histPDPCA ->Write();
   if (Use_KNN          )   histKNN   ->Write();
   if (Use_HMatrix      )   histHm    ->Write();
   if (Use_Fisher       )   histFi    ->Write();
   if (Use_MLP          )   histNn    ->Write();
   if (Use_CFMlpANN     )   histNnC   ->Write();
   if (Use_TMlpANN      )   histNnT   ->Write();
   if (Use_BDT          )   histBdt   ->Write();
   if (Use_BDTD         )   histBdtD  ->Write();
   if (Use_RuleFit      )   histRf    ->Write();
   if (Use_SVM_Gauss    )   histSVMG  ->Write();
   if (Use_SVM_Poly     )   histSVMP  ->Write();
   if (Use_SVM_Lin      )   histSVML  ->Write();
   if (Use_FDA_MT       )   histFDAMT ->Write();
   if (Use_FDA_GA       )   histFDAGA ->Write();

   if (Use_Fisher       ) { probHistFi->Write(); rarityHistFi->Write(); }
   target->Close();

   cout << "--- created root file: \"TMVApp.root\" containing the MVA output histograms" << endl;

   for (int j=0; j<nbgndw; j++) {delete reader[j];}
   cout << "==> TMVApplication is done!" << endl << endl;
} 

