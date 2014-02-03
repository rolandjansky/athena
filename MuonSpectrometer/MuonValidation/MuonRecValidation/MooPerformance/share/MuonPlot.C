{
  #include "IterativeGaussFit.C"    
  // Get input file and make tree
  TFile f("ntuple.root");
  TTree* muTree = (TTree*)f.Get("MYTREE");
    
  gStyle->SetOptFit(1);
    
  // Declare variables/vectors to be filled
  int eventNum(0); int runNum(0); int lbNum(0);
  std::vector<long>   *RegionInMS(0);
  std::vector<double> *fieldIntegralMS(0), *X0InMS(0);
  std::vector<double> *scatcurvature(0), *scatneighbour(0);
  std::vector<double> *innerMatchChi2(0), *outerMatchChi2(0), *mombalance(0);
  std::vector<double> *z0ID(0), *z0CB(0), *z0MS(0), *z0SA(0), *z0MGx(0),  *z0MG(0), *z0g(0);
  std::vector<double> *covZ0ID(0), *covZ0CB(0), *covZ0MS(0), *covZ0SA(0), *covZ0MGx(0),  *covZ0MG(0);
  std::vector<double> *d0ID(0), *d0CB(0), *d0MS(0), *d0SA(0), *d0MGx(0),  *d0MG(0), *d0g(0);
  std::vector<double> *covD0ID(0), *covD0CB(0), *covD0MS(0), *covD0SA(0), *covD0MGx(0),  *covD0MG(0);
  std::vector<double> *thetaID(0), *thetaCB(0), *thetaMS(0), *thetaSA(0), *thetaMGx(0),  *thetaMG(0), *thetag(0), *etag(0);
  std::vector<double> *covThetaID(0), *covThetaCB(0), *covThetaMS(0), *covThetaSA(0), *covThetaMGx(0),  *covThetaMG(0);
  std::vector<double> *phiID(0), *phiCB(0), *phiMS(0), *phiSA(0), *phiMGx(0),  *phiMG(0), *phig(0);
  std::vector<double> *covPhiID(0), *covPhiCB(0), *covPhiMS(0), *covPhiSA(0), *covPhiMGx(0),  *covPhiMG(0);
  std::vector<double> *chi2ID(0), *chi2CB(0), *chi2MS(0), *chi2SA(0), *chi2MGx(0),  *chi2MG(0);
  std::vector<double> *dofID(0), *dofCB(0), *dofMS(0), *dofSA(0), *dofMGx(0),  *dofMG(0);
  std::vector<double> *qOverPID(0), *qOverPCB(0), *qOverPMS(0), *qOverPSA(0), *qOverPMGx(0),  *qOverPMG(0), *qOverPg(0);
  std::vector<double> *cov45ID(0), *cov45CB(0), *cov45MS(0), *cov45SA(0), *cov45MGx(0),  *cov45MG(0);
  std::vector<double> *covQOverPID(0), *covQOverPCB(0), *covQOverPMS(0), *covQOverPSA(0), *covQOverPMGx(0),  *covQOverPMG(0);
    
  // Declare histograms
  // Set keys
  std::string keys[6];
  keys[0] = "combined muons";
  keys[1] = "inner detector muons";
  keys[2] = "CombinedMuonFit muons";
  keys[3] = "standalone muons";
  keys[4] = "muGirl muons";
  keys[5] = "muGirl extrapolated muons";

  std::string abbreviations[6];
  abbreviations[1] = "ID";
  abbreviations[2] = "MS";
  abbreviations[3] = "SA";
  abbreviations[5] = "MGx";
  abbreviations[0] = "CB";
  abbreviations[4] = "MG";
  
  std::string regions[7];
  regions[0] = "Barrel: 'air core + full field'";
  regions[1] = "Barrel: 'low field or dense material'";
  regions[2] = "Endcap: 'air core + full field'";
  regions[3] = "Endcap: 'low field or dense material'";
  regions[4] = "CSC short: 'air core + full field'";
  regions[5] = "CSC short: 'low field or dense material'";
  regions[6] = "";

  // Histogram creation:
  TH1F* hregion	= new TH1F("MS region", "region", 10, -0.5, 9.5);
  TH1F* hBdlByRegion[7];
  for (int i=0; i<7; ++i)
  {
      hBdlByRegion[i]	= new TH1F(("field integral in MS"+regions[i]).c_str(), regions[i].c_str(), 200, 0., 10.);
  }
  TH1F* hX0ByRegion[7];
  for (int i=0; i<7; ++i)
  {
      hX0ByRegion[i]	= new TH1F(("X0 in MS"+regions[i]).c_str(), regions[i].c_str(), 200, 0., 100.);
  }
  // match quality and discriminators
  TH1F* hinnerMatch		= new TH1F("inner match", "inner match", 50, -0.1, 9.9);
  TH1F* houterMatch		= new TH1F("outer match", "outer match", 50, -0.1, 9.9);
  TH1F* hBdl			= new TH1F("fieldIntegral", "field integral", 44, -0.5, 10.5);
  TH1F* hmomentumBalance	= new TH1F("momentum balance", "momentum balance", 200, -10., 10.);
  TH1F* hscatCurvature		= new TH1F("scattering curvature", "scattering curvature", 40, -10., 10.);
  TH1F* hscatNeighbour		= new TH1F("scattering neighbour", "scattering neighbour", 40, -10., 10.);
  
  // qOverP and pT
  TH1F* hqOverP[6];
  for (int i=0; i<6; ++i)
      hqOverP[i] = new TH1F(("qOverP resolution"+abbreviations[i]).c_str(), keys[i].c_str(), 240, -0.6, 0.6);
  TH1F* hPt[6];
  for (int i=0; i<6; ++i)
      hPt[i] = new TH1F(("Pt resolution"+abbreviations[i]).c_str(), keys[i].c_str(), 250, -0.25, 0.25);
  
  // momentum pull
  TH1F* hqOverPPull[6]; TH1F* hPtPull[6];
  for (int i=0; i<6; ++i)
  {
      hqOverPPull[i] = new TH1F(("Pull qOverP"+abbreviations[i]).c_str(), keys[i].c_str(), 200, -5.0, 5.0);
      hPtPull[i] = new TH1F(("Pull Pt"+abbreviations[i]).c_str(), keys[i].c_str(), 200, -5.0, 5.0);
  }
  
  // by MS region
  TH1F* hqOverPCBResByRegion[7];
  for (int i=0; i<7; ++i)
      hqOverPCBResByRegion[i] = new TH1F(("CB qOverP Resolution"+regions[i]).c_str(), regions[i].c_str(), 200, -0.25, 0.25);
  TH1F* hqOverPCBPullByRegion[7];
  for (int i=0; i<7; ++i)
      hqOverPCBPullByRegion[i] = new TH1F(("CB qOverP Pull"+regions[i]).c_str(), regions[i].c_str(), 200, -5.0, 5.0);
  TH1F* hqOverPSAResByRegion[7];
  for (int i=0; i<7; ++i)
      hqOverPSAResByRegion[i] = new TH1F(("SA qOverP Resolution"+regions[i]).c_str(), regions[i].c_str(), 200, -0.5, 0.5);
  TH1F* hqOverPSAPullByRegion[7];
  for (int i=0; i<7; ++i)
      hqOverPSAPullByRegion[i] = new TH1F(("SA qOverP Pull"+regions[i]).c_str(), regions[i].c_str(), 200, -5.0, 5.0);
  
  TH1F* hPtCBResByRegion[7];
  for (int i=0; i<7; ++i)
      hPtCBResByRegion[i] = new TH1F(("CB Pt Resolution"+regions[i]).c_str(), regions[i].c_str(), 300, -0.25, 0.25);
  TH1F* hPtCBPullByRegion[7];
  for (int i=0; i<7; ++i)
      hPtCBPullByRegion[i] = new TH1F(("CB Pt Pull"+regions[i]).c_str(), regions[i].c_str(), 100, -5.0, 5.0);
  TH1F* hPtSAResByRegion[7];
  for (int i=0; i<7; ++i)
      hPtSAResByRegion[i] = new TH1F(("SA Pt Resolution"+regions[i]).c_str(), regions[i].c_str(), 250, -0.5, 0.5);
  TH1F* hPtSAPullByRegion[7];
  for (int i=0; i<7; ++i)
      hPtSAPullByRegion[i] = new TH1F(("SA Pt Pull"+regions[i]).c_str(), regions[i].c_str(), 100, -5.0, 5.0);
  TH1F* hPtMGPullByRegion[7];
  for (int i=0; i<7; ++i)
      hPtMGPullByRegion[i] = new TH1F(("MG Pt Pull"+regions[i]).c_str(), regions[i].c_str(), 100, -5.0, 5.0);
  
  // D0 resolution and pull
  TH1F* hd0[6]; TH1F* hd0Low[6]; TH1F* hd0High[6]; TH1F* hd0Pull[6];
  for (int i=0; i<6; ++i)
  {
      hd0Pull[i] 	= new TH1F(("Pull D0"+abbreviations[i]).c_str(),		keys[i].c_str(), 200, -5.0, 5.0);
      if (i==3 || i==5)
      {
	  hd0[i]	= new TH1F(("D0 resolution"+abbreviations[i]).c_str(),		keys[i].c_str(), 400, -10.0, 10.0);
	  hd0Low[i]	= new TH1F(("D0 resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -20.0, 20.0);
	  hd0High[i]	= new TH1F(("D0 resolution (high)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -20.0, 20.0);
      }
      else
      {
	  hd0[i]	= new TH1F(("D0 resolution"+abbreviations[i]).c_str(),		keys[i].c_str(), 400, -0.1, 0.1);
	  hd0Low[i]	= new TH1F(("D0 resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -0.5, 0.5);
	  hd0High[i]	= new TH1F(("D0 resolution (high)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -0.5, 0.5);
      }
  }
  // resol and pull by region
  TH1F* hd0SAResByRegion[7]; TH1F* hd0LowSAResByRegion[7]; TH1F* hd0HighSAResByRegion[7]; TH1F* hd0SAPullByRegion[7];
  for (int i=0; i<7; ++i)
  {
      hd0SAResByRegion[i]	= new TH1F(("SA D0 Resolution"+regions[i]).c_str(),		regions[i].c_str(), 200, -10.0, 10.0);
      hd0LowSAResByRegion[i]	= new TH1F(("SA D0 Resolution (low)"+regions[i]).c_str(),	regions[i].c_str(), 200, -10.0, 10.0);
      hd0HighSAResByRegion[i]	= new TH1F(("SA D0 Resolution (high)"+regions[i]).c_str(),	regions[i].c_str(), 200, -10.0, 10.0);
      hd0SAPullByRegion[i]	= new TH1F(("SA D0 Pull"+regions[i]).c_str(),			regions[i].c_str(), 200, -5.0, 5.0);
  }
  
  // Z0 resolution and pull
  TH1F* hz0[6]; TH1F* hz0Low[6]; TH1F* hz0High[6]; TH1F* hz0Pull[6];
  for (int i=0; i<6; ++i)
  {
      if (i==3 || i==5)
      {
	  hz0[i]	= new TH1F(("Z0 resolution"+abbreviations[i]).c_str(),		keys[i].c_str(), 400, -1000.0, 1000.0);
	  hz0Low[i]	= new TH1F(("Z0 resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -1000.0, 1000.0);
	  hz0High[i]	= new TH1F(("Z0 resolution (high)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -1000.0, 1000.0);
      }
      else
      {
	  hz0[i]	= new TH1F(("Z0 resolution"+abbreviations[i]).c_str(),		keys[i].c_str(), 200, -0.5, 0.5);
	  hz0Low[i]	= new TH1F(("Z0 resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 200, -0.5, 0.5);
	  hz0High[i]	= new TH1F(("Z0 resolution (high)"+abbreviations[i]).c_str(),	keys[i].c_str(), 200, -0.5, 0.5);
      }
      hz0Pull[i]	= new TH1F(("Pull Z0"+abbreviations[i]).c_str(),		keys[i].c_str(), 200, -5.0, 5.0);
  }
  // resol and pull by region
  TH1F* hz0SAResByRegion[7]; TH1F* hz0LowSAResByRegion[7]; TH1F* hz0HighSAResByRegion[7]; TH1F* hz0SAPullByRegion[7];
  for (int i=0; i<7; ++i)
  {
      hz0SAResByRegion[i]	= new TH1F(("SA Z0 Resolution"+regions[i]).c_str(),		regions[i].c_str(), 400, -1000.0, 1000.0);
      hz0LowSAResByRegion[i]	= new TH1F(("SA Z0 Resolution (low)"+regions[i]).c_str(),	regions[i].c_str(), 400, -1000.0, 1000.0);
      hz0HighSAResByRegion[i]	= new TH1F(("SA Z0 Resolution (high)"+regions[i]).c_str(),	regions[i].c_str(), 400, -1000.0, 1000.0);
      hz0SAPullByRegion[i]	= new TH1F(("SA Z0 Pull"+regions[i]).c_str(),			regions[i].c_str(), 200, -5.0, 5.0);
  }

  // Phi resolution and pull
  TH1F* hphi[6]; TH1F* hphiLow[6]; TH1F* hphiHigh[6]; TH1F* hphiPull[6];
  for (int i=0; i<6; ++i)
  {
      if (i==3 || i==5)
      {
	  hphi[i]	= new TH1F(("Phi resolution"+abbreviations[i]).c_str(),		keys[i].c_str(), 400, -0.05, 0.05);
	  hphiLow[i]	= new TH1F(("Phi resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -0.20, 0.20);
	  hphiHigh[i]	= new TH1F(("Phi resolution (high)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -0.02, 0.02);
      }
      else
      {
	  hphi[i]	= new TH1F(("Phi resolution"+abbreviations[i]).c_str(),		keys[i].c_str(), 300, -0.003, 0.003);
	  hphiLow[i]	= new TH1F(("Phi resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 300, -0.003, 0.003);
	  hphiHigh[i]	= new TH1F(("Phi resolution (high)"+abbreviations[i]).c_str(),	keys[i].c_str(), 300, -0.003, 0.003);
      }
      hphiPull[i]	= new TH1F(("Pull Phi"+abbreviations[i]).c_str(),		keys[i].c_str(), 200, -5.0, 5.0);
  }
  // resol and pull by region
  TH1F* hphiSAResByRegion[7]; TH1F* hphiLowSAResByRegion[7]; TH1F* hphiHighSAResByRegion[7]; TH1F* hphiSAPullByRegion[7];
  for (int i=0; i<7; ++i)
  {
      hphiSAResByRegion[i]	= new TH1F(("SA Phi Resolution"+regions[i]).c_str(),		regions[i].c_str(), 400, -0.05, 0.05);
      hphiLowSAResByRegion[i]	= new TH1F(("SA Phi Resolution (low)"+regions[i]).c_str(),	regions[i].c_str(), 400, -0.20, 0.20);
      hphiHighSAResByRegion[i]	= new TH1F(("SA Phi Resolution (high)"+regions[i]).c_str(),	regions[i].c_str(), 400, -0.02, 0.02);
      hphiSAPullByRegion[i]	= new TH1F(("SA Phi Pull"+regions[i]).c_str(),			regions[i].c_str(), 200, -5.0, 5.0);
  }
  
  // Theta resolution and pull
  TH1F* htheta[6]; TH1F* hthetaLow[6]; TH1F* hthetaHigh[6]; TH1F* hthetaPull[6];
  for (int i=0; i<6; ++i)
  {
      if (i==3 || i==5)
      {
	  htheta[i]	= new TH1F(("Theta resolution"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -0.04, 0.04);
	  hthetaLow[i]	= new TH1F(("Theta resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 400, -0.20, 0.20);
	  hthetaHigh[i]	= new TH1F(("Theta resolution (high)"+abbreviations[i]).c_str(),keys[i].c_str(), 400, -0.01, 0.01);
      }
      else
      {
	  htheta[i]	= new TH1F(("Theta resolution"+abbreviations[i]).c_str(),	keys[i].c_str(), 300, -0.003, 0.003);
	  hthetaLow[i]	= new TH1F(("Theta resolution (low)"+abbreviations[i]).c_str(),	keys[i].c_str(), 300, -0.003, 0.003);
	  hthetaHigh[i]	= new TH1F(("Theta resolution (high)"+abbreviations[i]).c_str(),keys[i].c_str(), 300, -0.003, 0.003);
      }
      hthetaPull[i]	= new TH1F(("Pull Theta"+abbreviations[i]).c_str(),		keys[i].c_str(), 200, -5.0, 5.0);
  }
  // resol and pull by region
  TH1F* hthetaSAResByRegion[7]; TH1F* hthetaLowSAResByRegion[7]; TH1F* hthetaHighSAResByRegion[7]; TH1F* hthetaSAPullByRegion[7]; TH1F* hthetaCBPullByRegion[7];
  for (int i=0; i<7; ++i)
  {
      hthetaSAResByRegion[i]	= new TH1F(("SA Theta Resolution"+regions[i]).c_str(),		regions[i].c_str(), 400, -0.04, 0.04);
      hthetaLowSAResByRegion[i]	= new TH1F(("SA Theta Resolution (low)"+regions[i]).c_str(),	regions[i].c_str(), 400, -0.20, 0.20);
      hthetaHighSAResByRegion[i]= new TH1F(("SA Theta Resolution (high)"+regions[i]).c_str(),	regions[i].c_str(), 400, -0.01, 0.01);
      hthetaSAPullByRegion[i]	= new TH1F(("SA Theta Pull"+regions[i]).c_str(),		regions[i].c_str(), 200, -5.0, 5.0);
      hthetaCBPullByRegion[i]	= new TH1F(("CB Theta Pull"+regions[i]).c_str(),		regions[i].c_str(), 200, -5.0, 5.0);
  }

  // Chi2 per d.o.f.
  TH1F* hchi2dof[6];
  for (int i=0; i<6; ++i)
      hchi2dof[i] = new TH1F(("chi2dof"+abbreviations[i]).c_str(), keys[i].c_str(), 200, 0.0, 5.0);

  // MG-CB
  TH1F* hqOverPMGoverCB	= new TH1F("hqOverPMGoverCB","qOverP MG/CB", 200, 0.9, 1.1);
  TH1F* hD0MGminusCB	= new TH1F("hD0MGminusCB","D0 MG-CB", 200, -0.01, 0.01);
  TH1F* hZ0MGminusCB	= new TH1F("hZ0MGminusCB","Z0 MG-CB", 200, -0.05, 0.05);
  TH1F* hPhiMGminusCB	= new TH1F("hPhiMGminusCB","Phi MG-CB", 200, -0.0002, 0.0002);
  TH1F* hThetaMGminusCB	= new TH1F("hThetaMGminusCB","Theta MG-CB", 200, -0.0002, 0.0002);

  // CMF-CB
  TH1F* hqOverPCMFoverCB= new TH1F("hqOverPCMFoverCB","qOverP CMF/CB", 200, 0.9, 1.1);
  TH1F* hD0CMFminusCB	= new TH1F("hD0CMFminusCB","D0_CMF-D0_CB", 200, -0.01, 0.01);
  TH1F* hZ0CMFminusCB	= new TH1F("hZ0CMFminusCB","Z0_CMF-Z0_CB", 200, -0.05, 0.05);
  TH1F* hPhiCMFminusCB	= new TH1F("hPhiCMFminusCB","Phi_CMF-Phi_CB", 200, -0.0002, 0.0002);
  TH1F* hThetaCMFminusCB= new TH1F("hThetaCMFminusCB","Theta_CMF-Theta_CB", 200, -0.0002, 0.0002);

  // ID-CB
  TH1F* hD0IDminusCB	= new TH1F("hD0IDminusCB","D0_ID-D0_CB", 150, -0.03, 0.03);
  TH1F* hZ0IDminusCB	= new TH1F("hZ0IDminusCB","Z0_ID-Z0_CB", 150, -0.3, 0.3);
  TH1F* hPhiIDminusCB	= new TH1F("hPhiIDminusCB","Phi_ID-Phi_CB", 200, -0.001, 0.001);
  TH1F* hThetaIDminusCB	= new TH1F("hThetaIDminusCB","Theta_ID-Theta_CB", 200, -0.001, 0.001);

  // ID-CMF
  TH1F* hD0IDminusCMF	= new TH1F("hD0IDminusCMF","D0_ID-D0_CMF", 150, -0.03, 0.03);
  TH1F* hZ0IDminusCMF	= new TH1F("hZ0IDminusCMF","Z0_ID-Z0_CMF", 150, -0.3, 0.3);
  TH1F* hPhiIDminusCMF	= new TH1F("hPhiIDminusCMF","Phi_ID-Phi_CMF", 200, -0.001, 0.001);
  TH1F* hThetaIDminusCMF = new TH1F("hThetaIDminusCMF","Theta_ID-Theta_CMF", 200, -0.001, 0.001);

  // Set branch addresses
  muTree->SetBranchAddress("fieldIntegralMS",&fieldIntegralMS);
  muTree->SetBranchAddress("RegionInMS",&RegionInMS);
  muTree->SetBranchAddress("X0InMS", &X0InMS);
  muTree->SetBranchAddress("innerMatchChi2", &innerMatchChi2);
  muTree->SetBranchAddress("outerMatchChi2", &outerMatchChi2);
  muTree->SetBranchAddress("mombalance", &mombalance);
  muTree->SetBranchAddress("scatcurvature", &scatcurvature);
  muTree->SetBranchAddress("scatneighbour", &scatneighbour);
  
  // D0
  muTree->SetBranchAddress("a0ID",&d0ID);
  muTree->SetBranchAddress("a0MS",&d0MS);
  muTree->SetBranchAddress("a0SA",&d0SA);    
  muTree->SetBranchAddress("a0MGx",&d0MGx);
  muTree->SetBranchAddress("a0CB",&d0CB);
  muTree->SetBranchAddress("a0MG",&d0MG);
  muTree->SetBranchAddress("a0g",&d0g);
  // D0 covariance
  muTree->SetBranchAddress("cov11ID",&covD0ID);
  muTree->SetBranchAddress("cov11MS",&covD0MS);
  muTree->SetBranchAddress("cov11SA",&covD0SA);    
  muTree->SetBranchAddress("cov11MGx",&covD0MGx);
  muTree->SetBranchAddress("cov11CB",&covD0CB);
  muTree->SetBranchAddress("cov11MG",&covD0MG);
     
  // Z0
  muTree->SetBranchAddress("z0ID",&z0ID);
  muTree->SetBranchAddress("z0MS",&z0MS);
  muTree->SetBranchAddress("z0SA",&z0SA);    
  muTree->SetBranchAddress("z0MGx",&z0MGx);
  muTree->SetBranchAddress("z0CB",&z0CB);
  muTree->SetBranchAddress("z0MG",&z0MG);
  muTree->SetBranchAddress("z0g",&z0g);
  // Z0 covariance
  muTree->SetBranchAddress("cov22ID",&covZ0ID);
  muTree->SetBranchAddress("cov22MS",&covZ0MS);
  muTree->SetBranchAddress("cov22SA",&covZ0SA);    
  muTree->SetBranchAddress("cov22MGx",&covZ0MGx);
  muTree->SetBranchAddress("cov22CB",&covZ0CB);
  muTree->SetBranchAddress("cov22MG",&covZ0MG);
   
  // Phi
  muTree->SetBranchAddress("phiID",&phiID);
  muTree->SetBranchAddress("phiMS",&phiMS);
  muTree->SetBranchAddress("phiSA",&phiSA);    
  muTree->SetBranchAddress("phiMGx",&phiMGx);
  muTree->SetBranchAddress("phiCB",&phiCB);
  muTree->SetBranchAddress("phiMG",&phiMG);
  muTree->SetBranchAddress("phig",&phig);
  // Phi convariance
  muTree->SetBranchAddress("cov33ID",&covPhiID);
  muTree->SetBranchAddress("cov33MS",&covPhiMS);
  muTree->SetBranchAddress("cov33SA",&covPhiSA);    
  muTree->SetBranchAddress("cov33MGx",&covPhiMGx);
  muTree->SetBranchAddress("cov33CB",&covPhiCB);
  muTree->SetBranchAddress("cov33MG",&covPhiMG);
        
  // Theta
  muTree->SetBranchAddress("thetaID",&thetaID);
  muTree->SetBranchAddress("thetaMS",&thetaMS);
  muTree->SetBranchAddress("thetaSA",&thetaSA);    
  muTree->SetBranchAddress("thetaMGx",&thetaMGx);
  muTree->SetBranchAddress("thetaCB",&thetaCB);
  muTree->SetBranchAddress("thetaMG",&thetaMG);
  muTree->SetBranchAddress("thetag",&thetag);
  muTree->SetBranchAddress("etag",&etag);
  // Theta convariance
  muTree->SetBranchAddress("cov44ID",&covThetaID);
  muTree->SetBranchAddress("cov44MS",&covThetaMS);
  muTree->SetBranchAddress("cov44SA",&covThetaSA);    
  muTree->SetBranchAddress("cov44MGx",&covThetaMGx);
  muTree->SetBranchAddress("cov44CB",&covThetaCB);
  muTree->SetBranchAddress("cov44MG",&covThetaMG);
        
  // chi2
  muTree->SetBranchAddress("chi2ID",&chi2ID);
  muTree->SetBranchAddress("chi2MS",&chi2MS);
  muTree->SetBranchAddress("chi2SA",&chi2SA);    
  muTree->SetBranchAddress("chi2MGx",&chi2MGx);
  muTree->SetBranchAddress("chi2CB",&chi2CB);
  muTree->SetBranchAddress("chi2MG",&chi2MG);
        
  // nDOF
  muTree->SetBranchAddress("dofID",&dofID);
  muTree->SetBranchAddress("dofMS",&dofMS);
  muTree->SetBranchAddress("dofSA",&dofSA);    
  muTree->SetBranchAddress("dofMGx",&dofMGx);
  muTree->SetBranchAddress("dofCB",&dofCB);
  muTree->SetBranchAddress("dofMG",&dofMG);  
        
  // qOverP
  muTree->SetBranchAddress("qOverPID",&qOverPID);
  muTree->SetBranchAddress("qOverPMS",&qOverPMS);
  muTree->SetBranchAddress("qOverPSA",&qOverPSA);    
  muTree->SetBranchAddress("qOverPMGx",&qOverPMGx);
  muTree->SetBranchAddress("qOverPCB",&qOverPCB);
  muTree->SetBranchAddress("qOverPMG",&qOverPMG);
  muTree->SetBranchAddress("qOverPg",&qOverPg);
  // qOverP convariance and correlation with theta
  muTree->SetBranchAddress("cov45ID",&cov45ID);
  muTree->SetBranchAddress("cov45MS",&cov45MS);
  muTree->SetBranchAddress("cov45SA",&cov45SA);    
  muTree->SetBranchAddress("cov45MGx",&cov45MGx);
  muTree->SetBranchAddress("cov45CB",&cov45CB);
  muTree->SetBranchAddress("cov45MG",&cov45MG);
  muTree->SetBranchAddress("cov55ID",&covQOverPID);
  muTree->SetBranchAddress("cov55MS",&covQOverPMS);
  muTree->SetBranchAddress("cov55SA",&covQOverPSA);    
  muTree->SetBranchAddress("cov55MGx",&covQOverPMGx);
  muTree->SetBranchAddress("cov55CB",&covQOverPCB);
  muTree->SetBranchAddress("cov55MG",&covQOverPMG);
              
  // Loop over events
  // cout << "start of loop" << endl;
  int nEvents		= (int)muTree->GetEntries();
  int numberAnyPt	= 0;
  int numberLowPt	= 0;
  int numberHighPt	= 0;
  for (int i = 0; i < nEvents; ++i)
  {
      muTree->GetEntry(i);

      // skip if not truth matched
      if ((*z0g)[0] == 99999.)		continue;
      // cout << "event " << i << "  eta " << (*etag)[0] << "   phi " << (*phig)[0] << endl;
      
      // Loop over muons
      int nMuons = z0CB->size();
      for (int m = 0; m < nMuons; ++m)
      {
	  // skip if not truth matched
	  if ((*z0g)[m] > 9999.)	continue;
	  double charge		= 1.0;
	  if ((*qOverPg)[m] < 0.) charge = -1.0;
	  double eta		= fabs((*etag)[m]);
	  double phi		= (*phig)[m];	// modulo 8
	  double qOverP		= (*qOverPg)[m];
	  double sinTheta	= sin((*thetag)[m]);
	  double pt		= fabs(sinTheta/qOverP);
	  bool highPt		= false;
	  ++numberAnyPt;
	  if (pt > 99000.)
	  {
	      ++numberHighPt;
	      highPt	= true;
	  }
	  bool lowPt		= false;
	  if (pt < 22000.)
	  {
	      ++numberLowPt;
	      lowPt	= true;
	  }
	  
	  int region		= (*RegionInMS)[m];
	  // skip CSC long chambers as OB1 in MC12
	  if (region == 4)		continue;
	  
	  // reassign region according to material thickness rather than long/short
	  bool aircore = true;
	  if ((*X0InMS)[m] > 5.0) aircore = false;
	  if (region < 2)
	  {
	      if (aircore && (*fieldIntegralMS)[m] > 1.8)
	      {
		  region = 0;
	      }
	      else
	      {
		  region = 1;
	      }
	  }
	  else if (region < 4)
	  {
	      if (aircore && (*fieldIntegralMS)[m] > 2.0)
	      {
		  region = 2;
	      }
	      else
	      {
		  region = 3;
	      }
	  }
	  else
	  {
	      if (aircore && (*fieldIntegralMS)[m] > 4.0)
	      {
		  region = 4;
	      }
	      else
	      {
		  region = 5;
	      }
	  }    
	
	  // cout << "event " << i << " pt " << pt << "  eta " << eta << "   phi " << phi << endl; 
	
	  bool OKID	= ((*z0ID)[m] < 9999.);
	  bool OKMS	= ((*z0MS)[m] < 9999.);
	  bool OKSA	= ((*z0SA)[m] < 9999.);
	  bool OKMGx	= ((*z0MGx)[m] < 9999.);
	  bool OKCB	= ((*z0CB)[m] < 9999.);
	  bool OKMG	= ((*z0MG)[m] < 9999.);
	  if (OKID)
	  {
	      // cout << " ID " << endl;n
	      // D0 resolution and pull
	      double deltaD0		= (*d0ID)[m]-(*d0g)[m];
	      hd0[1]			->Fill( deltaD0 );
	      hd0Low[1]			->Fill( deltaD0 );
	      hd0High[1]		->Fill( deltaD0 );
	      hd0Pull[1]		->Fill( deltaD0/sqrt((*covD0ID)[m]) );
	      // Z0 resolution and pull
	      double deltaZ0		= (*z0ID)[m]-(*z0g)[m];
	      hz0[1]			->Fill( deltaZ0 );
	      hz0Low[1]			->Fill( deltaZ0 );
	      hz0High[1]		->Fill( deltaZ0 );
	      hz0Pull[1]		->Fill( deltaZ0/sqrt((*covZ0ID)[m]) );
	      // Phi resolution and pull
	      double deltaPhi		= (*phiID)[m]-(*phig)[m];
	      hphi[1]			->Fill( deltaPhi );
	      hphiLow[1]		->Fill( deltaPhi );
	      hphiHigh[1]		->Fill( deltaPhi );
	      hphiPull[1]		->Fill( deltaPhi/sqrt((*covPhiID)[m]) );
	      // Theta resolution and pull
	      double deltaTheta		= (*thetaID)[m]-(*thetag)[m];
	      htheta[1]			->Fill( deltaTheta );
	      hthetaLow[1]		->Fill( deltaTheta );
	      hthetaHigh[1]		->Fill( deltaTheta );
	      hthetaPull[1]		->Fill( deltaTheta/sqrt((*covThetaID)[m]) );
	      // chi2/nDOF
	      if ((*dofID)[m] > 0) hchi2dof[1]->Fill( (*chi2ID)[m]/(*dofID)[m] );
	      // qOverP and pt resolutions
	      double cosThetai	= cos((*thetaID)[m]);
	      double sinThetai	= sin((*thetaID)[m]);
	      double qOverPi	= (*qOverPID)[m];
	      double pi		= fabs(1./qOverPi);
	      double pti	= pi*sinThetai;
	      if (charge*(*qOverPID)[m] > 0.)
	      {
		  hqOverP[1]->Fill( qOverPi/qOverP -1. );
		  hPt[1]->Fill( pti/pt -1. );
	      }
	      // and pulls
	      if ((*covQOverPID)[m] > 0.)
	      {
		  double wtqOverP	= 1./sqrt((*covQOverPID)[m]);
		  double drv4		= pi*cosThetai;
		  double drv5		= -sinThetai/(qOverP*qOverP);
		  double wtPt		= 1./sqrt(
		      drv4*drv4*(*covThetaID)[m] +
		      2.*drv4*drv5*(*cov45ID)[m] +
		      drv5*drv5*(*covQOverPID)[m]);
		  hqOverPPull[1]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hPtPull[1]->Fill( wtPt*(pti-pt) );
	      }
	  }
	
	  if (OKMS)
	  {
	      // cout << " CMF " << endl;
	      // D0 resolution and pull
	      double deltaD0		= (*d0MS)[m]-(*d0g)[m];
	      hd0[2]			->Fill( deltaD0 );
	      hd0Low[2]			->Fill( deltaD0 );
	      hd0High[2]		->Fill( deltaD0 );
	      hd0Pull[2]		->Fill( deltaD0/sqrt((*covD0MS)[m]) );
	      // Z0 resolution and pull
	      double deltaZ0		= (*z0MS)[m]-(*z0g)[m];
	      hz0[2]			->Fill( deltaZ0 );
	      hz0Low[2]			->Fill( deltaZ0 );
	      hz0High[2]		->Fill( deltaZ0 );
	      hz0Pull[2]		->Fill( deltaZ0/sqrt((*covZ0MS)[m]) );
	      // Phi resolution and pull
	      double deltaPhi		= (*phiMS)[m]-(*phig)[m];
	      hphi[2]			->Fill( deltaPhi );
	      hphiLow[2]		->Fill( deltaPhi );
	      hphiHigh[2]		->Fill( deltaPhi );
	      hphiPull[2]		->Fill( deltaPhi/sqrt((*covPhiMS)[m]) );
	      // Theta resolution and pull
	      double deltaTheta		= (*thetaMS)[m]-(*thetag)[m];
	      htheta[2]			->Fill( deltaTheta );
	      hthetaLow[2]		->Fill( deltaTheta );
	      hthetaHigh[2]		->Fill( deltaTheta );
	      hthetaPull[2]		->Fill( deltaTheta/sqrt((*covThetaMS)[m]) );
	      // chi2/nDOF
	      if ((*dofMS)[m] > 0) hchi2dof[2]->Fill( (*chi2MS)[m]/(*dofMS)[m] );

	      // qOverP and pt resolutions
	      double cosThetai	= cos((*thetaMS)[m]);
	      double sinThetai	= sin((*thetaMS)[m]);
	      double qOverPi	= (*qOverPMS)[m];
	      double pi		= fabs(1./qOverPi);
	      double pti	= pi*sinThetai;
	      if (charge*(*qOverPMS)[m] > 0.)
	      {
		  hqOverP[2]->Fill( qOverPi/qOverP -1. );
		  hPt[2]->Fill( pti/pt -1. );
	      }
	      // and pulls
	      if ((*covQOverPMS)[m] > 0.)
	      {
		  double wtqOverP	= 1./sqrt((*covQOverPMS)[m]);
		  double drv4	= pi*cosThetai;
		  double drv5	= -sinThetai/(qOverP*qOverP);
		  double wtPt	= 1./sqrt(
		      drv4*drv4*(*covThetaMS)[m] +
		      2.*drv4*drv5*(*cov45MS)[m] +
		      drv5*drv5*(*covQOverPMS)[m]);
		  hqOverPPull[2]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hPtPull[2]->Fill( wtPt*(pti-pt) );
	      }
	  }
	
	  if (OKSA)
	  {
	      //cout << " SA " << endl;
	      // D0 resolution and pull
	      double deltaD0			= (*d0SA)[m]-(*d0g)[m];
	      double d0Pull			= deltaD0/sqrt((*covD0SA)[m]);
	      hd0[3]				->Fill( deltaD0 );
	      hd0Low[3]				->Fill( deltaD0 );
	      hd0High[3]			->Fill( deltaD0 );
	      hd0SAResByRegion[region]		->Fill(	deltaD0 );
	      hd0LowSAResByRegion[region]	->Fill(	deltaD0 );
	      hd0HighSAResByRegion[region]	->Fill(	deltaD0 );
	      hd0Pull[3]			->Fill( d0Pull );
	      hd0SAPullByRegion[region]		->Fill( d0Pull );
	      // Z0 resolution and pull
	      double deltaZ0			= (*z0SA)[m]-(*z0g)[m];
	      double z0Pull			= deltaZ0/sqrt((*covZ0SA)[m]);
	      hz0[3]				->Fill( deltaZ0 );
	      hz0Low[3]				->Fill( deltaZ0 );
	      hz0High[3]			->Fill( deltaZ0 );
	      hz0SAResByRegion[region]		->Fill(	deltaZ0 );
	      hz0LowSAResByRegion[region]	->Fill(	deltaZ0 );
	      hz0HighSAResByRegion[region]	->Fill(	deltaZ0 );
	      hz0Pull[3]			->Fill( z0Pull );
	      hz0SAPullByRegion[region]		->Fill( z0Pull );
	      // Phi resolution and pull
	      double deltaPhi			= (*phiSA)[m]-(*phig)[m];
	      double phiPull			= deltaPhi/sqrt((*covPhiSA)[m]);
	      hphi[3]				->Fill( deltaPhi );
	      hphiLow[3]			->Fill( deltaPhi );
	      hphiHigh[3]			->Fill( deltaPhi );
	      hphiSAResByRegion[region]		->Fill(	deltaPhi );
	      hphiLowSAResByRegion[region]	->Fill(	deltaPhi );
	      hphiHighSAResByRegion[region]	->Fill(	deltaPhi );
	      hphiPull[3]			->Fill( phiPull );
	      hphiSAPullByRegion[region]	->Fill( phiPull );
	      // Theta resolution and pull
	      double deltaTheta			= (*thetaSA)[m]-(*thetag)[m];
	      double thetaPull			= deltaTheta/sqrt((*covThetaSA)[m]);
	      htheta[3]				->Fill( deltaTheta );
	      hthetaLow[3]			->Fill( deltaTheta );
	      hthetaHigh[3]			->Fill( deltaTheta );
	      hthetaPull[3]			->Fill( deltaTheta/sqrt((*covThetaSA)[m]) );
	      hthetaSAResByRegion[region]	->Fill(	deltaTheta );
	      hthetaLowSAResByRegion[region]	->Fill(	deltaTheta );
	      hthetaHighSAResByRegion[region]	->Fill(	deltaTheta );
	      hthetaPull[3]			->Fill( thetaPull );
	      hthetaSAPullByRegion[region]	->Fill( thetaPull );
	      
	      // chi2/nDOF
	      if ((*dofSA)[m] > 0) hchi2dof[3]->Fill( (*chi2SA)[m]/(*dofSA)[m] );
	    
	      // qOverP and pt resolutions
	      double cosThetai	= cos((*thetaSA)[m]);
	      double sinThetai	= sin((*thetaSA)[m]);
	      double qOverPi	= (*qOverPSA)[m];
	      double pi		= fabs(1./qOverPi);
	      double pti	= pi*sinThetai;
	      if (charge*qOverPi > 0.)
	      {
		  hqOverP[3]->Fill( qOverPi/qOverP -1. );
		  hqOverPSAResByRegion[region]->Fill( qOverPi/qOverP -1. );
		  hPt[3]->Fill( pti/pt -1. );
		  hPtSAResByRegion[region]->Fill( pti/pt -1. );
	      }
	      // and pulls
	      if (qOverP != 0. && (*covQOverPSA)[m] > 0.)
	      {
		  double wtqOverP	= 1./sqrt((*covQOverPSA)[m]);
		  double drv4		= pi*cosThetai;
		  double drv5		= -sinThetai/(qOverP*qOverP);
		  double wtPt		= 1./sqrt(
		      drv4*drv4*(*covThetaSA)[m] +
		      2.*drv4*drv5*(*cov45SA)[m] +
		      drv5*drv5*(*covQOverPSA)[m]);
		  hqOverPPull[3]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hqOverPSAPullByRegion[region]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hPtPull[3]->Fill( wtPt*(pti-pt) );
		  hPtSAPullByRegion[region]->Fill( wtPt*(pti-pt) );
	      }
	  }
	
	  if (OKMGx)
	  {
	      // cout << " MGx " << endl;
	      // D0 resolution and pull
	      double deltaD0		= (*d0MGx)[m]-(*d0g)[m];
	      hd0[5]			->Fill( deltaD0 );
	      hd0Low[5]			->Fill( deltaD0 );
	      hd0High[5]		->Fill( deltaD0 );
	      hd0Pull[5]		->Fill( deltaD0/sqrt((*covD0MGx)[m]) );
	      // Z0 resolution and pull
	      double deltaZ0		= (*z0MGx)[m]-(*z0g)[m];
	      hz0[5]			->Fill( deltaZ0 );
	      hz0Low[5]			->Fill( deltaZ0 );
	      hz0High[5]		->Fill( deltaZ0 );
	      hz0Pull[5]		->Fill( deltaZ0/sqrt((*covZ0MGx)[m]) );
	      // Phi resolution and pull
	      double deltaPhi		= (*phiMGx)[m]-(*phig)[m];
	      hphi[5]			->Fill( deltaPhi );
	      hphiLow[5]		->Fill( deltaPhi );
	      hphiHigh[5]		->Fill( deltaPhi );
	      hphiPull[5]		->Fill( deltaPhi/sqrt((*covPhiMGx)[m]) );
	      // Theta resolution and pull
	      double deltaTheta		= (*thetaMGx)[m]-(*thetag)[m];
	      htheta[5]			->Fill( deltaTheta );
	      hthetaLow[5]		->Fill( deltaTheta );
	      hthetaHigh[5]		->Fill( deltaTheta );
	      hthetaPull[5]		->Fill( deltaTheta/sqrt((*covThetaMGx)[m]) );
	      // chi2/nDOF
	      if ((*dofMGx)[m] > 0) hchi2dof[5]->Fill( (*chi2MGx)[m]/(*dofMGx)[m] );
	    	    
	      // qOverP and pt resolutions
	      double cosThetai	= cos((*thetaMGx)[m]);
	      double sinThetai	= sin((*thetaMGx)[m]);
	      double qOverPi	= (*qOverPMGx)[m];
	      double pi		= fabs(1./qOverPi);
	      double pti	= pi*sinThetai;
	      if (charge*qOverPi > 0.)
	      {
		  hqOverP[5]->Fill( qOverPi/qOverP -1. );
		  hPt[5]->Fill( pti/pt -1. );
	      }
	      // and pulls
	      if ((*covQOverPMGx)[m] > 0.)
	      {
		  double wtqOverP	= 1./sqrt((*covQOverPMGx)[m]);
		  double drv4		= pi*cosThetai;
		  double drv5		= -sinThetai/(qOverP*qOverP);
		  double wtPt		= 1./sqrt(
		      drv4*drv4*(*covThetaMGx)[m] +
		      2.*drv4*drv5*(*cov45MGx)[m] +
		      drv5*drv5*(*covQOverPMGx)[m]);
		  hqOverPPull[5]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hPtPull[5]->Fill( wtPt*(pti-pt) );
	      }
	  }
	
	  if (OKCB)
	  {
	      // cout << " CB " << endl;
	      // int r	= (*RegionInMS)[m];
	      // std::cout << " CB field integral" << (*fieldIntegralMS)[m] << std::endl;
	      // std::cout << " CB region " << r  << ", " << region << "   X0 " << (*X0InMS)[m] << std::endl;
	      hregion->Fill( static_cast<double>(region) );
	      hBdl->Fill( (*fieldIntegralMS)[m] );
	      hBdlByRegion[region]->Fill( (*fieldIntegralMS)[m] );
	      hX0ByRegion[region]->Fill( (*X0InMS)[m] );
	      hinnerMatch->Fill( (*innerMatchChi2)[m]/5.0 );
	      houterMatch->Fill( (*outerMatchChi2)[m]/4.0 );
	      hmomentumBalance->Fill( (*mombalance)[m] );
	      hscatCurvature->Fill( (*scatcurvature)[m] );
	      hscatNeighbour->Fill( (*scatneighbour)[m] );

	      // D0 resolution
	      double deltaD0		= (*d0CB)[m]-(*d0g)[m];
	      hd0[0]			->Fill( deltaD0 );
	      hd0Low[0]			->Fill( deltaD0 );
	      hd0High[0]		->Fill( deltaD0 );
	      hd0Pull[0]		->Fill( deltaD0/sqrt((*covD0CB)[m]) );
	      // Z0 resolution and pull
	      double deltaZ0		= (*z0CB)[m]-(*z0g)[m];
	      hz0[0]			->Fill( deltaZ0 );
	      hz0Low[0]			->Fill( deltaZ0 );
	      hz0High[0]		->Fill( deltaZ0 );
	      hz0Pull[0]		->Fill( deltaZ0/sqrt((*covZ0CB)[m]) );
	      // Phi resolution
	      double deltaPhi		= (*phiCB)[m]-(*phig)[m];
	      hphi[0]			->Fill( deltaPhi );
	      hphiLow[0]	       	->Fill( deltaPhi );
	      hphiHigh[0]		->Fill( deltaPhi );
	      hphiPull[0]		->Fill( deltaPhi/sqrt((*covPhiCB)[m]) );
	      // Theta resolution and pull
	      double deltaTheta		= (*thetaCB)[m]-(*thetag)[m];
	      double thetaPull		= deltaTheta/sqrt((*covThetaCB)[m]);
	      htheta[0]			->Fill( deltaTheta );
	      hthetaLow[0]	       	->Fill( deltaTheta );
	      hthetaHigh[0]		->Fill( deltaTheta );
	      hthetaPull[0]		->Fill( thetaPull );
	      hthetaCBPullByRegion[region]->Fill( thetaPull );
	      // chi2/nDOF
	      if ((*dofCB)[m] > 0) hchi2dof[0]->Fill( (*chi2CB)[m]/(*dofCB)[m] );
	    	    
	      // qOverP and pt resolutions
	      double cosThetai	= cos((*thetaCB)[m]);
	      double sinThetai	= sin((*thetaCB)[m]);
	      double qOverPi	= (*qOverPCB)[m];
	      double pi		= fabs(1./qOverPi);
	      double pti	= pi*sinThetai;
	      if (charge*qOverPi > 0.)
	      {
		  hqOverP[0]->Fill( qOverPi/qOverP -1. );
		  hqOverPCBResByRegion[region]->Fill( qOverPi/qOverP -1. );
		  hPt[0]->Fill( pti/pt -1. );
		  hPtCBResByRegion[region]->Fill( pti/pt -1. );
	      }
	      // and pulls
	      if ((*covQOverPCB)[m] > 0.)
	      {
		  double wtqOverP	= 1./sqrt((*covQOverPCB)[m]);
		  double drv4		= pi*cosThetai;
		  double drv5		= -sinThetai/(qOverP*qOverP);
		  double wtPt		= 1./sqrt(
		      drv4*drv4*(*covThetaCB)[m] +
		      2.*drv4*drv5*(*cov45CB)[m] +
		      drv5*drv5*(*covQOverPCB)[m]);
		  hqOverPPull[0]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hqOverPCBPullByRegion[region]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hPtPull[0]->Fill( wtPt*(pti-pt) );
		  hPtCBPullByRegion[region]->Fill( wtPt*(pti-pt) );
	      }
	  }
  	
	  if (OKMG)
	  {
	      // cout << " MG " << endl;

	      // D0 resolution and pull
	      double deltaD0		= (*d0MG)[m]-(*d0g)[m];
	      hd0[4]			->Fill( deltaD0 );
	      hd0Low[4]			->Fill( deltaD0 );
	      hd0High[4]		->Fill( deltaD0 );
	      hd0Pull[4]		->Fill( deltaD0/sqrt((*covD0MG)[m]) );
	      // Z0 resolution and pull
	      double deltaZ0		= (*z0MG)[m]-(*z0g)[m];
	      hz0[4]			->Fill( deltaZ0 );
	      hz0Low[4]			->Fill( deltaZ0 );
	      hz0High[4]		->Fill( deltaZ0 );
	      hz0Pull[4]		->Fill( deltaZ0/sqrt((*covZ0MG)[m]) );
	      // Phi resolution and pull
	      double deltaPhi		= (*phiMG)[m]-(*phig)[m];
	      hphi[4]			->Fill( deltaPhi );
	      hphiLow[4]		->Fill( deltaPhi );
	      hphiHigh[4]		->Fill( deltaPhi );
	      hphiPull[4]		->Fill( deltaPhi/sqrt((*covPhiMG)[m]) );
	      // Theta resolution and pull
	      double deltaTheta		= (*thetaMG)[m]-(*thetag)[m];
	      htheta[4]			->Fill( deltaTheta );
	      hthetaLow[4]		->Fill( deltaTheta );
	      hthetaHigh[4]		->Fill( deltaTheta );
	      hthetaPull[4]		->Fill( deltaTheta/sqrt((*covThetaMG)[m]) );
	      // chi2/nDOF
	      if ((*dofMG)[m] > 0) hchi2dof[4]->Fill( (*chi2MG)[m]/(*dofMG)[m] );
	    
	      // qOverP and pt resolutions
	      double cosThetai	= cos((*thetaMG)[m]);
	      double sinThetai	= sin((*thetaMG)[m]);
	      double qOverPi	= (*qOverPMG)[m];
	      double pi		= fabs(1./qOverPi);
	      double pti	= pi*sinThetai;
	      if (charge*qOverPi > 0.)
	      {
		  hqOverP[4]->Fill( qOverPi/qOverP -1. );
		  hPt[4]->Fill( pti/pt -1. );
	      }
	      // and pulls
	      if ((*covQOverPMG)[m] > 0.)
	      {
		  double wtqOverP	= 1./sqrt((*covQOverPMG)[m]);
		  double drv4		= pi*cosThetai;
		  double drv5		= -sinThetai/(qOverP*qOverP);
		  double wtPt		= 1./sqrt(
		      drv4*drv4*(*covThetaMG)[m] +
		      2.*drv4*drv5*(*cov45MG)[m] +
		      drv5*drv5*(*covQOverPMG)[m]);
		  hqOverPPull[4]->Fill( wtqOverP*(qOverPi-qOverP) );
		  hPtPull[4]->Fill( wtPt*(pti-pt) );
		  hPtMGPullByRegion[region]->Fill( wtPt*(pti-pt) );
	      }
	  }
		
	  if (OKCB && OKID)
	  {
	      // ID-CB
	      hD0IDminusCB->Fill( (*d0ID)[m]-(*d0CB)[m]);
	      hZ0IDminusCB->Fill( (*z0ID)[m]-(*z0CB)[m]);
	      hPhiIDminusCB->Fill( (*phiID)[m]-(*phiCB)[m]);
	      hThetaIDminusCB->Fill( (*thetaID)[m]-(*thetaCB)[m]);
	  }
	  if (OKMS && OKID)
	  {
	      // ID-CMF
	      hD0IDminusCMF->Fill( (*d0ID)[m]-(*d0MS)[m]);
	      hZ0IDminusCMF->Fill( (*z0ID)[m]-(*z0MS)[m]);
	      hPhiIDminusCMF->Fill( (*phiID)[m]-(*phiMS)[m]);
	      hThetaIDminusCMF->Fill( (*thetaID)[m]-(*thetaMS)[m]);
	  }
	  if (OKCB && OKMG)
	  {
	      // MG-CB
	      hqOverPMGoverCB->Fill( (*qOverPMG)[m]/(*qOverPCB)[m]);
	      hD0MGminusCB->Fill( (*d0MG)[m]-(*d0CB)[m]);
	      hZ0MGminusCB->Fill( (*z0MG)[m]-(*z0CB)[m]);
	      hPhiMGminusCB->Fill( (*phiMG)[m]-(*phiCB)[m]);
	      hThetaMGminusCB->Fill( (*thetaMG)[m]-(*thetaCB)[m]);
	  }
	  if (OKCB && OKMS)
	  {
	      // CMF-CB
	      hqOverPCMFoverCB->Fill( (*qOverPMS)[m]/(*qOverPCB)[m]);
	      hD0CMFminusCB->Fill( (*d0MS)[m]-(*d0CB)[m]);
	      hZ0CMFminusCB->Fill( (*z0MS)[m]-(*z0CB)[m]);
	      hPhiCMFminusCB->Fill( (*phiMS)[m]-(*phiCB)[m]);
	      hThetaCMFminusCB->Fill( (*thetaMS)[m]-(*thetaCB)[m]);
	  }
	  
      } // end of loop over muons
      
  } // end of loop over events

  // print gif files without fit

  // region in MS
  TCanvas *cRegion = new TCanvas("region","region in MS",150,10,990,660);
  cRegion->cd(0);
  hregion->Draw();
  cRegion->Print("./region.gif");

  // MS field integral
  TCanvas *cBdl = new TCanvas("fieldIntegral","field integral in MS",150,10,990,660);
  cBdl->Divide(2,3);
  for (Int_t i=0; i<6; ++i) { cBdl->cd(i+1); hBdlByRegion[i]->Draw(); }
  cBdl->Print("./MS_Bdl.gif");
  // MS material
  TCanvas *cX0 = new TCanvas("X0","X0 in MS",150,10,990,660);
  cX0->Divide(2,3);
  for (Int_t i=0; i<6; ++i) { cX0->cd(i+1); hX0ByRegion[i]->Draw(); }
  cX0->Print("./MS_material.gif");
  // ID-MS match quality
  TCanvas *cmatch = new TCanvas("match","quality",150,10,990,660);    
  cmatch->Divide(2,3);
  cmatch->cd(1); hinnerMatch->Draw();
  cmatch->cd(2); houterMatch->Draw();
  cmatch->cd(3); hBdl->Draw();
  cmatch->cd(4); hmomentumBalance->Draw();
  cmatch->cd(5); hscatCurvature->Draw();
  cmatch->cd(6); hscatNeighbour->Draw();
  cmatch->Print("./match_quality.gif");

  // Chi2/dof
  TCanvas *theCanvas1 = new TCanvas("chi2_dof","chi2 per d.o.f.",150,10,990,660);    
  theCanvas1->Divide(2,3);
  for (Int_t i=0; i<6; ++i) {theCanvas1->cd(i+1); hchi2dof[i]->Draw();} 
  theCanvas1->Print("./fit_chi2.gif");
  
  // Do the fits
  int fitInt		= 0;
  double mu		= 0.;
  double mu_err		= 0.;
  double sigma		= 0.;
  double sigma_err	= 0.;

  fitInt	= IterativeGaussFit(hD0IDminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit d0 ID-CB    sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hZ0IDminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit Z0 ID-CB    sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hPhiIDminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit phi ID-CB   sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hThetaIDminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit theta ID-CB sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hD0IDminusCMF, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit d0 ID-CMF   sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hZ0IDminusCMF, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit Z0 ID-CMF   sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hPhiIDminusCMF, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit phi ID-CMF  sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hThetaIDminusCMF, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit theta ID-CMF sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hD0MGminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit d0 MG-CB    sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hZ0MGminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit Z0 MG-CB    sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hPhiMGminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit phi MG-CB   sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hThetaMGminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit theta MG-CB sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hD0CMFminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit d0 CMF-CB    sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hZ0CMFminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit Z0 CMF-CB    sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hPhiCMFminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit phi CMF-CB   sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hThetaCMFminusCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit theta CMF-CB sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hqOverPMGoverCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit qOverP MG/CB sigma " << sigma << std::endl;
  fitInt	= IterativeGaussFit(hqOverPCMFoverCB, mu, mu_err, sigma, sigma_err, 1);
  std::cout << "iterative gauss fit qOverP CMF/CB sigma " << sigma << std::endl;
  
  // bin according to Pt
  std::cout << "   numberAnyPt " << numberAnyPt  << "   numberLowPt " << numberLowPt
	    << "   numberHighPt " << numberHighPt << std::endl;
  bool highPtBins	= false;
  bool lowPtBins	= false;
  if (2*numberHighPt > numberAnyPt)
  {
      highPtBins	= true;
      std::cout << " chosen high Pt binning" << std::endl;
  }
  else if (2*numberLowPt > numberAnyPt)
  {
      lowPtBins	= true;
      std::cout << " chosen low Pt binning" << std::endl;
  }
  
  // gauss fits to resolution and pull    
  for (int i = 0; i < 6; ++i)
  {
      std::cout << " gauss fits " << i << std::endl;

      // resolutions - start with narrow gauss fit, broad gauss as back-up
      if (! lowPtBins)
      {
	  fitInt			= IterativeGaussFit(hqOverP[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hqOverP[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit qOverP Res     sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hqOverPCBResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hqOverPCBResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit qOverP CB Res  sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hqOverPSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hqOverPSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit qOverP SA Res  sigma " << sigma << std::endl;
      }
      if (! highPtBins)
      {
	  fitInt			= IterativeGaussFit(hPt[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPt[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt Res         sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hPtCBResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPtCBResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt CB Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hPtSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPtSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt SA Res      sigma " << sigma << std::endl;
      }
      if (lowPtBins)
      {	  
	  fitInt			= IterativeGaussFit(hd0Low[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hd0Low[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit d0             sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hd0LowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hd0LowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit d0 SA Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hz0Low[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hz0Low[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit z0             sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hz0LowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hz0LowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit z0 SA Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hphiLow[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hphiLow[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit phi            sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hphiLowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hphiLowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit phi SA Res     sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hthetaLow[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaLow[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit theta          sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hthetaLowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaLowSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit theta SA Res   sigma " << sigma << std::endl;
      }
      else if (highPtBins)
      {	  
	  fitInt			= IterativeGaussFit(hd0High[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hd0High[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit d0             sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hd0HighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hd0HighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit d0 SA Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hz0High[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hz0High[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit z0             sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hz0HighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hz0HighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit z0 SA Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hphiHigh[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hphiHigh[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit phi            sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hphiHighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hphiHighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit phi SA Res     sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hthetaHigh[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaHigh[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit theta          sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hthetaHighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaHighSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit theta SA Res   sigma " << sigma << std::endl;
      }
      else
      {	  
	  fitInt			= IterativeGaussFit(hd0[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hd0[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit d0             sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hd0SAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hd0SAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit d0 SA Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hz0[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hz0[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit z0             sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hz0SAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hz0SAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit z0 SA Res      sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hphi[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hphi[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit phi            sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hphiSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hphiSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit phi SA Res     sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(htheta[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(htheta[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit theta          sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hthetaSAResByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaSAResByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit theta SA Res   sigma " << sigma << std::endl;
      }
      
      // fit pulls to 1.5 or 2.5 sigma ?
      fitInt			= IterativeGaussFit(hd0Pull[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hd0Pull[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit d0 pull        sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hd0SAPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hd0SAPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit d0 SA Pull     sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hz0Pull[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hz0Pull[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit z0 pull        sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hz0SAPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hz0SAPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit z0 SA Pull     sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hphiPull[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hphiPull[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit phi pull       sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hphiSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hphiSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit phi SA Pull    sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hthetaPull[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaPull[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit theta pull     sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hthetaSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit theta SA Pull  sigma " << sigma << std::endl;
      fitInt			= IterativeGaussFit(hthetaCBPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
      if (fitInt == 1) fitInt	= IterativeGaussFit(hthetaCBPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
      if (! fitInt) std::cout << "iterative gauss fit theta CB Pull  sigma " << sigma << std::endl;
      if (! lowPtBins)
      {
	  fitInt			= IterativeGaussFit(hqOverPPull[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hqOverPPull[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit qOverP Pull    sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hqOverPCBPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hqOverPCBPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit qOverP CB Pull sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hqOverPSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 1);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hqOverPSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit qOverP SA Pull sigma " << sigma << std::endl;
      }
      // asymm fit to low Pt pulls should highlight Landau tail
      if (! highPtBins)
      {
	  fitInt			= IterativeGaussFit(hPtPull[i], mu, mu_err, sigma, sigma_err, 3);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPtPull[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt Pull        sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hPtCBPullByRegion[i], mu, mu_err, sigma, sigma_err, 3);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPtCBPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt CB Pull     sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hPtSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 3);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPtSAPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt SA Pull     sigma " << sigma << std::endl;
	  fitInt			= IterativeGaussFit(hPtMGPullByRegion[i], mu, mu_err, sigma, sigma_err, 3);
	  if (fitInt == 1) fitInt	= IterativeGaussFit(hPtMGPullByRegion[i], mu, mu_err, sigma, sigma_err, 2);
	  if (! fitInt) std::cout << "iterative gauss fit Pt MG Pull     sigma " << sigma << std::endl;
      }
  }

  // Draw histograms
  if (! lowPtBins)
  {      
      // qOverP resolution and pull
      TCanvas *cpResolution = new TCanvas("pResolution","qOverP resolution",150,10,990,660);
      cpResolution->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cpResolution->cd(i+1); hqOverP[i]->Draw();}
      cpResolution->Print("./qOverP_res.gif");
      TCanvas *cqOverPPull	= new TCanvas("qOverP_Pull","qOverP pull",150,10,990,660);
      cqOverPPull->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cqOverPPull->cd(i+1); hqOverPPull[i]->Draw();} 
      cqOverPPull->Print("./qOverP_pull.gif");
  
      // qOverP by region
      TCanvas *cqOverPCBbyRegion = new TCanvas("qOverP_CB_Resolution by region","CB qOverP resolution by region",150,10,990,660);    
      cqOverPCBbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cqOverPCBbyRegion->cd(i+1); hqOverPCBResByRegion[i]->Draw();} 
      cqOverPCBbyRegion->Print("./qOverP_CB_resByRegion.gif");
      TCanvas *cqOverPCBPullbyRegion = new TCanvas("qOverP_CBpull by region","CB qOverP pull by region",150,10,990,660);    
      cqOverPCBPullbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cqOverPCBPullbyRegion->cd(i+1); hqOverPCBPullByRegion[i]->Draw();} 
      cqOverPCBPullbyRegion->Print("./qOverP_CB_pullByRegion.gif");
      TCanvas *cqOverPSAbyRegion = new TCanvas("qOverP_SAresolution by region","SA qOverP resolution by region",150,10,990,660);    
      cqOverPSAbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cqOverPSAbyRegion->cd(i+1); hqOverPSAResByRegion[i]->Draw();} 
      cqOverPSAbyRegion->Print("./qOverP_SA_resByRegion.gif");
      TCanvas *cqOverPSAPullbyRegion = new TCanvas("qOverP_SApull by region","SA qOverP pull by region",150,10,990,660);    
      cqOverPSAPullbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cqOverPSAPullbyRegion->cd(i+1); hqOverPSAPullByRegion[i]->Draw();} 
      cqOverPSAPullbyRegion->Print("./qOverP_SA_pullByRegion.gif");
  }
  
  if (! highPtBins)
  {
      // Pt resolution and pull
      TCanvas *cptResolution = new TCanvas("ptResolution","Pt resolution",150,10,990,660);
      cptResolution->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cptResolution->cd(i+1); hPt[i]->Draw();}
      cptResolution->Print("./Pt_res.gif");
      TCanvas *cptpull	= new TCanvas("pt_Pull","Pt pull",150,10,990,660);
      cptpull->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cptpull->cd(i+1); hPtPull[i]->Draw();} 
      cptpull->Print("./Pt_pull.gif");

      // pt by region
      TCanvas *cptCBbyRegion = new TCanvas("ptResolution by region","CB pt resolution by region",150,10,990,660);    
      cptCBbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cptCBbyRegion->cd(i+1); hPtCBResByRegion[i]->Draw();} 
      cptCBbyRegion->Print("./Pt_CB_resByRegion.gif");
      TCanvas *cPtCBPullbyRegion = new TCanvas("Pt_CBpull by region","CB Pt pull by region",150,10,990,660);    
      cPtCBPullbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cPtCBPullbyRegion->cd(i+1); hPtCBPullByRegion[i]->Draw();} 
      cPtCBPullbyRegion->Print("./Pt_CB_pullByRegion.gif");
      TCanvas *cPtSAbyRegion = new TCanvas("Pt_SAresolution by region","SA Pt resolution by region",150,10,990,660);    
      cPtSAbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cPtSAbyRegion->cd(i+1); hPtSAResByRegion[i]->Draw();} 
      cPtSAbyRegion->Print("./Pt_SA_resByRegion.gif");
      TCanvas *cPtSAPullbyRegion = new TCanvas("Pt_SApull by region","SA Pt pull by region",150,10,990,660);    
      cPtSAPullbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cPtSAPullbyRegion->cd(i+1); hPtSAPullByRegion[i]->Draw();}
      cPtSAPullbyRegion->Print("./Pt_SA_pullByRegion.gif");
      TCanvas *cPtMGPullbyRegion = new TCanvas("Pt_MGpull by region","MG Pt pull by region",150,10,990,660);    
      cPtMGPullbyRegion->Divide(2,3);
      for (Int_t i=0; i<6; ++i) {cPtMGPullbyRegion->cd(i+1); hPtMGPullByRegion[i]->Draw();}
      cPtMGPullbyRegion->Print("./Pt_MG_pullByRegion.gif");
  }

  // D0
  TCanvas *cD0 = new TCanvas("D0","D0",150,10,990,660);    
  cD0->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cD0->cd(i+1); hd0Low[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cD0->cd(i+1); hd0High[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cD0->cd(i+1); hd0[i]->Draw();}
  }
  cD0->Print("./D0_res.gif");
  // D0 pull
  TCanvas *cD0Pull = new TCanvas("D0Pull","D0 Pull",150,10,990,660);    
  cD0Pull->Divide(2,3); 
  for (Int_t i=0; i<6; ++i) {cD0Pull->cd(i+1); hd0Pull[i]->Draw();} 
  cD0Pull->Print("./D0_pull.gif");
  // D0 by region
  TCanvas *cd0SAbyRegion = new TCanvas("D0_SAresolution by region","SA D0 resolution by region",150,10,990,660);    
  cd0SAbyRegion->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cd0SAbyRegion->cd(i+1); hd0LowSAResByRegion[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cd0SAbyRegion->cd(i+1); hd0HighSAResByRegion[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cd0SAbyRegion->cd(i+1); hd0SAResByRegion[i]->Draw();}
  }
  cd0SAbyRegion->Print("./D0_SA_resByRegion.gif");
  TCanvas *cd0SAPullbyRegion = new TCanvas("D0_SApull by region","SA D0 pull by region",150,10,990,660);    
  cd0SAPullbyRegion->Divide(2,3);
  for (Int_t i=0; i<6; ++i) {cd0SAPullbyRegion->cd(i+1); hd0SAPullByRegion[i]->Draw();}
  cd0SAPullbyRegion->Print("./D0_SA_pullByRegion.gif");
  
  // Z0
  TCanvas *cZ0 = new TCanvas("Z0","Z0",150,10,990,660);    
  cZ0->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cZ0->cd(i+1); hz0Low[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cZ0->cd(i+1); hz0High[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cZ0->cd(i+1); hz0[i]->Draw();}
  }
  cZ0->Print("./Z0_res.gif");
  // Z0 pull
  TCanvas *cZ0Pull = new TCanvas("Z0Pull","Z0 Pull",150,10,990,660);    
  cZ0Pull->Divide(2,3); 
  for (Int_t i=0; i<6; ++i) {cZ0Pull->cd(i+1); hz0Pull[i]->Draw();} 
  cZ0Pull->Print("./Z0_pull.gif");
	   
  // by region
  TCanvas *cz0SAbyRegion = new TCanvas("Z0_SAresolution by region","SA Z0 resolution by region",150,10,990,660);    
  cz0SAbyRegion->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cz0SAbyRegion->cd(i+1); hz0LowSAResByRegion[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cz0SAbyRegion->cd(i+1); hz0HighSAResByRegion[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cz0SAbyRegion->cd(i+1); hz0SAResByRegion[i]->Draw();}
  }
  cz0SAbyRegion->Print("./Z0_SA_resByRegion.gif");
  TCanvas *cz0SAPullbyRegion = new TCanvas("Z0_SApull by region","SA Z0 pull by region",150,10,990,660);    
  cz0SAPullbyRegion->Divide(2,3);
  for (Int_t i=0; i<6; ++i) {cz0SAPullbyRegion->cd(i+1); hz0SAPullByRegion[i]->Draw();}
  cz0SAPullbyRegion->Print("./Z0_SA_pullByRegion.gif");
  
  // Phi
  TCanvas *cPhi = new TCanvas("Phi","Phi",150,10,990,660);    
  cPhi->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cPhi->cd(i+1); hphiLow[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cPhi->cd(i+1); hphiHigh[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cPhi->cd(i+1); hphi[i]->Draw();}
  }
  cPhi->Print("./Phi_res.gif");
  // Phi pull
  TCanvas *cPhiPull = new TCanvas("PhiPull","Phi Pull",150,10,990,660);    
  cPhiPull->Divide(2,3); 
  for (Int_t i=0; i<6; ++i) {cPhiPull->cd(i+1); hphiPull[i]->Draw();} 
  cPhiPull->Print("./Phi_pull.gif");
  TCanvas *cphiSAbyRegion = new TCanvas("Phi_SAresolution by region","SA Phi resolution by region",150,10,990,660);    
  cphiSAbyRegion->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cphiSAbyRegion->cd(i+1); hphiLowSAResByRegion[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cphiSAbyRegion->cd(i+1); hphiHighSAResByRegion[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cphiSAbyRegion->cd(i+1); hphiSAResByRegion[i]->Draw();}
  }
  cphiSAbyRegion->Print("./Phi_SA_resByRegion.gif");
  TCanvas *cphiSAPullbyRegion = new TCanvas("Phi_SApull by region","SA Phi pull by region",150,10,990,660);    
  cphiSAPullbyRegion->Divide(2,3);
  for (Int_t i=0; i<6; ++i) {cphiSAPullbyRegion->cd(i+1); hphiSAPullByRegion[i]->Draw();}
  cphiSAPullbyRegion->Print("./Phi_SA_pullByRegion.gif");

  // Theta
  TCanvas *cTheta = new TCanvas("Theta","Theta",150,10,990,660);    
  cTheta->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cTheta->cd(i+1); hthetaLow[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cTheta->cd(i+1); hthetaHigh[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cTheta->cd(i+1); htheta[i]->Draw();}
  }
  cTheta->Print("./Theta_res.gif");
  // Theta pull
  TCanvas *cThetaPull = new TCanvas("ThetaPull","Theta Pull",150,10,990,660);    
  cThetaPull->Divide(2,3); 
  for (Int_t i=0; i<6; ++i) {cThetaPull->cd(i+1); hthetaPull[i]->Draw();}    
  cThetaPull->Print("./Theta_pull.gif");
  TCanvas *cthetaSAbyRegion = new TCanvas("Theta_SAresolution by region","SA Theta resolution by region",150,10,990,660);    
  cthetaSAbyRegion->Divide(2,3);
  if (lowPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cthetaSAbyRegion->cd(i+1); hthetaLowSAResByRegion[i]->Draw();}
  }
  else if (highPtBins)
  {
      for (Int_t i=0; i<6; ++i) {cthetaSAbyRegion->cd(i+1); hthetaHighSAResByRegion[i]->Draw();}
  }
  else
  {
      for (Int_t i=0; i<6; ++i) {cthetaSAbyRegion->cd(i+1); hthetaSAResByRegion[i]->Draw();}
  }
  cthetaSAbyRegion->Print("./Theta_SA_resByRegion.gif");
  TCanvas *cthetaSAPullbyRegion = new TCanvas("Theta_SApull by region","SA Theta pull by region",150,10,990,660);    
  cthetaSAPullbyRegion->Divide(2,3);
  for (Int_t i=0; i<6; ++i) {cthetaSAPullbyRegion->cd(i+1); hthetaSAPullByRegion[i]->Draw();}
  cthetaSAPullbyRegion->Print("./Theta_SA_pullByRegion.gif");
  TCanvas *cthetaCBPullbyRegion = new TCanvas("Theta_CBpull by region","CB Theta pull by region",150,10,990,660);    
  cthetaCBPullbyRegion->Divide(2,3);
  for (Int_t i=0; i<6; ++i) {cthetaCBPullbyRegion->cd(i+1); hthetaCBPullByRegion[i]->Draw();}
  cthetaCBPullbyRegion->Print("./Theta_CB_pullByRegion.gif");
  
  // ID-CB
  TCanvas *theCanvasID = new TCanvas("ID-CB","ID-CB",150,10,990,660);    
  theCanvasID->Divide(2,2);
  theCanvasID->cd(1); hD0IDminusCB->Draw();
  theCanvasID->cd(2); hZ0IDminusCB->Draw();
  theCanvasID->cd(3); hPhiIDminusCB->Draw();
  theCanvasID->cd(4); hThetaIDminusCB->Draw();
  theCanvasID->Print("./ID-CB.gif");
  // ID-CMF
  TCanvas *theCanvasIF = new TCanvas("ID-CMF","ID-CMF",150,10,990,660);    
  theCanvasIF->Divide(2,2);
  theCanvasIF->cd(1); hD0IDminusCMF->Draw();
  theCanvasIF->cd(2); hZ0IDminusCMF->Draw();
  theCanvasIF->cd(3); hPhiIDminusCMF->Draw();
  theCanvasIF->cd(4); hThetaIDminusCMF->Draw();
  theCanvasIF->Print("./ID-CMF.gif");
  // MG-CB
  TCanvas *theCanvasMG = new TCanvas("MG-CB","MG-CB",150,10,990,660);    
  theCanvasMG->Divide(2,2);
  theCanvasMG->cd(1); hD0MGminusCB->Draw();
  theCanvasMG->cd(2); hZ0MGminusCB->Draw();
  theCanvasMG->cd(3); hPhiMGminusCB->Draw();
  theCanvasMG->cd(4); hThetaMGminusCB->Draw();
  theCanvasMG->Print("./MG-CB.gif");
  // CMF-CB
  TCanvas *theCanvasCMF = new TCanvas("CMF-CB","CMF-CB",150,10,990,660);    
  theCanvasCMF->Divide(2,2);
  theCanvasCMF->cd(1); hD0CMFminusCB->Draw();
  theCanvasCMF->cd(2); hZ0CMFminusCB->Draw();
  theCanvasCMF->cd(3); hPhiCMFminusCB->Draw();
  theCanvasCMF->cd(4); hThetaCMFminusCB->Draw();
  theCanvasCMF->Print("./CMF-CB.gif");
  // qOverP ratio
  TCanvas *theCanvasQoverP = new TCanvas("qOverPMGoverCB","qOverP MG/CB",150,10,990,660); 
  theCanvasQoverP->Divide(2,1);
  theCanvasQoverP->cd(1); hqOverPMGoverCB->Draw();
  theCanvasQoverP->cd(2); hqOverPCMFoverCB->Draw();
  theCanvasQoverP->Print("./qOverP_ratio.gif");
  
  delete muTree;
  // f->close();
  return 0;
  
}

