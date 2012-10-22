/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


std::vector<std::string> hVector;
std::vector<double> binPt, binEta, binPhi;

void SetBinning();
void MuonPerformancePlotsPtEtaPhi(TDirectory* testdir, TCanvas* MyC1, std::string collname, std::string colldir, std::string PtEtaPhi);
void MuonPerformancePlotsOneShot(TDirectory* testdir, TCanvas* MyC1, std::string collname, std::string colldir, std::string OneShot);
void MuonPerformancePlotsOneShotSame(TDirectory* testdir, TCanvas* MyC1, std::string collname, std::string colldir, std::string OneShot, double min, double max, bool save);
void MuonPerformancePlotsFromTree(TDirectory* testdir, TCanvas* MyC1, std::string collname);
void MuonPerformancePlotsTreeVar(TTree* tree, TTree* ref_tree, TVirtualPad* padC1, TH1F* hobs, std::string collname, std::string xlabel, std::string ylabel, bool normalize);
void Stylist(TCanvas *c, TH1F* h, std::string type);
void getAxisRangeForObs(TDirectory* testdir, std::string colldir, std::string obs, double& min, double& max);

//=====================================================================
//
// Analysis steering by collection name
//
//=====================================================================

void MuonRecValidator_RTT_perfplots(const char* filename)
{
  cout << "\n --> MuonRecValidator_RTT_perfplots Compiled OK \n" << endl; 
  const char* filename;
  OpenFile(filename);
  SetBinning();
  
  // reference plots
  MuonPerformancePlots(filename, "Trigger_Level_1");
  MuonPerformancePlots(filename, "Trigger_Level_2_MS");
  MuonPerformancePlots(filename, "Trigger_Level_3_MS");  
  MuonPerformancePlots(filename, "Trigger_Level_2_CB");
  MuonPerformancePlots(filename, "Trigger_Level_3_CB");
  MuonPerformancePlots(filename, "Trigger_OverAll_MS");
  MuonPerformancePlots(filename, "Trigger_OverAll_CB");
   
  MuonPerformancePlots(filename, "TrackParticleCandidate");
  MuonPerformancePlots(filename, "MooreTrackParticles");
  MuonPerformancePlots(filename, "MuonboyMuonSpectroOnlyTrackParticles");

  MuonPerformancePlots(filename, "StacoMuonCollection");
  MuonPerformancePlots(filename, "MuidMuonCollection");

  // comparisons plots
  MuonPerformanceComparisonPlots(filename, "Efficiency_Pt_h");
  MuonPerformanceComparisonPlots(filename, "Efficiency_Eta_h");
  MuonPerformanceComparisonPlots(filename, "Efficiency_Phi_h");

  MuonPerformanceComparisonPlots(filename, "FakeRate_Pt_h");
  MuonPerformanceComparisonPlots(filename, "FakeRate_Eta_h");
  MuonPerformanceComparisonPlots(filename, "FakeRate_Phi_h");

  MuonPerformanceComparisonPlots(filename, "Resolution_Mean_Pt_h");
  MuonPerformanceComparisonPlots(filename, "Resolution_Mean_Eta_h");

  MuonPerformanceComparisonPlots(filename, "Resolution_Width_Pt_h");
  MuonPerformanceComparisonPlots(filename, "Resolution_Width_Eta_h");

  std::cout << "Start writing HTML files" << std::endl;
  writeHTMLFiles();
  std::cout << "End writing HTML files" << std::endl;

  std::cout << "Packing HTML files in tar archive" << std::endl;
  archiveHTMLFiles();

  if(castorConn) {
    std::cout << "Copying to Castor HTML tar archive" << std::endl;
    std::cout << "Archive can be downloaded from : /castor/cern.ch/user/m/mbellomo/MuonRecValidator" << std::endl;
    sendToCastorHTMLFiles();
  }  
}

//=====================================================================
//
//  HTML Files
//
//=====================================================================
void archiveHTMLFiles()
{
  std::string cmd = "tar -czvf "+reference_release+"_"+test_release+".tar.gz "+reference_release+"_"+test_release;
  gSystem->Exec(cmd.c_str());
}

void sendToCastorHTMLFiles()
{
  std::string cmd = "rfcp "+reference_release+"_"+test_release+".tar.gz /castor/cern.ch/user/m/mbellomo/MuonRecValidator/.";
  gSystem->Exec(cmd.c_str());
}

void writeHTMLFiles()
{
  std::cout << "- writing main ..." << std::endl;

  std::string foutname, fMoore, fMuonboy, fStaco,fMuid, fID, fTrigger, fComparison;

  foutname = "MuonRecValidatorMain_"+reference_release+"_"+test_release+".html";
  fMoore = "MuonRecValidator_Moore_"+reference_release+"_"+test_release+".html";
  fMuid = "MuonRecValidator_Muid_"+reference_release+"_"+test_release+".html";
  fMuonboy = "MuonRecValidator_Muonboy_"+reference_release+"_"+test_release+".html";
  fStaco = "MuonRecValidator_Staco_"+reference_release+"_"+test_release+".html";
  fID = "MuonRecValidator_ID_"+reference_release+"_"+test_release+".html";
  fTrigger = "MuonRecValidator_Trigger_"+reference_release+"_"+test_release+".html";
  fComparison = "MuonRecValidator_Comparison_"+reference_release+"_"+test_release+".html";
 
  FILE *fout = fopen(foutname.c_str(),"w+");
  fprintf(fout, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\" width=\"450\" height=\"300\">\n");
  fprintf(fout, "<html>\n");
  fprintf(fout, "<head>\n");
  fprintf(fout, "<title>MuonRecValidator validation plots</title>\n");
  fprintf(fout, "</head>\n");
  fprintf(fout, "<body>\n");
  fprintf(fout, "<h1>MuonRecValidator</h1>\n");
  fprintf(fout, "<p> Reference release is %s </p>\n", reference_release.c_str());
  fprintf(fout, "<p> Reference dataset is %s </p>\n", reference_file.c_str());
  fprintf(fout, "<p> Test release is      %s </p>\n", test_release.c_str());
  fprintf(fout, "<p> Test dataset is      %s </p>\n", test_file.c_str());
  fprintf(fout, "<p></p>\n");
  fprintf(fout, "<p><a href=\"%s\"> Moore</a></p>\n", fMoore.c_str());
  fprintf(fout, "<p><a href=\"%s\"> Muid</a></p>\n", fMuid.c_str());
  fprintf(fout, "<p><a href=\"%s\"> Muonboy</a></p>\n", fMuonboy.c_str());
  fprintf(fout, "<p><a href=\"%s\"> Staco</a></p>\n", fStaco.c_str());
  fprintf(fout, "<p><a href=\"%s\"> ID</a></p>\n", fID.c_str());
  fprintf(fout, "<p><a href=\"%s\"> Trigger</a></p>\n", fTrigger.c_str());
  fprintf(fout, "<p><a href=\"%s\"> Comparison</a></p>\n", fComparison.c_str());
 
  std::cout << "Computing integrated trigger efficiencies" << std::endl;
  std::cout << "***** Test dataset " << test_release << std::endl;
  fprintf(fout, "<p></p>\n");
  fprintf(fout, "<p><h3>Summary tables</h3></p>\n");
  fprintf(fout, "<p></p>\n");
  
  fprintf(fout, "<TABLE BORDER=1 RULES=ROWS FRAME=BOX CELLPADDING=3 CELLSPACING=1>\n");
  fprintf(fout, "<TR BGCOLOR=\"#99CCGG\"> <TD>Integrated efficiencies</TD>      <TD> %s </TD>      <TD> %s </TD> </TR>\n", 
	  test_release.c_str(), reference_release.c_str());

  fprintf(fout, "<TR> <TH COLSPAN=3 BGCOLOR=\"#99CCFF\">Trigger</TH> </TR>\n");

  MuonPerformanceIntegralEff(fout, "Trigger_Level_1");
  MuonPerformanceIntegralEff(fout, "Trigger_Level_2_MS");
  MuonPerformanceIntegralEff(fout, "Trigger_Level_2_CB");
  MuonPerformanceIntegralEff(fout, "Trigger_Level_3_MS");
  MuonPerformanceIntegralEff(fout, "Trigger_Level_3_CB");
  MuonPerformanceIntegralEff(fout, "Trigger_OverAll_MS");
  MuonPerformanceIntegralEff(fout, "Trigger_OverAll_CB");

  fprintf(fout, "<TR> <TH COLSPAN=3 BGCOLOR=\"#99CCFF\">Offline</TH> </TR>\n");

  MuonPerformanceIntegralEff(fout, "MooreTrackParticles");
  MuonPerformanceIntegralEff(fout, "MuidMuonCollection");
  MuonPerformanceIntegralEff(fout, "MuonboyMuonSpectroOnlyTrackParticles");
  MuonPerformanceIntegralEff(fout, "StacoMuonCollection");
  MuonPerformanceIntegralEff(fout, "TrackParticleCandidate");
 
  fprintf(fout, "</TABLE>\n");
  fprintf(fout, "</body>\n");
  fprintf(fout, "</html>\n");
  fclose(fout);

  writeCollHTMLFile(fMoore);
  writeCollHTMLFile(fMuid);
  writeCollHTMLFile(fMuonboy);
  writeCollHTMLFile(fStaco);
  writeCollHTMLFile(fID);
  writeCollHTMLFile(fTrigger);
  writeCollHTMLFile(fComparison);

  std::cout << "- populating htmlstuff directory with gif's and html's ..." << std::endl;
  
  std::string cmd = "mkdir -p "+reference_release+"_"+test_release;
  gSystem->Exec(cmd.c_str());
  cmd = "for f in `ls *.eps | awk -F . '{print $1}'` ; do convert ${f}.eps "+reference_release+"_"+test_release+"/${f}.gif ; done";
  gSystem->Exec(cmd.c_str());
  cmd = "mv *.html "+reference_release+"_"+test_release;
  gSystem->Exec(cmd.c_str());
}

void writeCollHTMLFile(std::string fColl)
{
  std::cout << "- writing " << fColl << " ..." << std::endl;

  std::vector<std::string> collnames;
  if(strstr(fColl.c_str(),"Moore"))
    collnames.push_back("MooreTrackParticles");
  else if(strstr(fColl.c_str(),"Muid"))
    collnames.push_back("MuidMuonCollection");
  else if(strstr(fColl.c_str(),"Muonboy"))
    collnames.push_back("MuonboyMuonSpectroOnlyTrackParticles");
  else if(strstr(fColl.c_str(),"Staco"))
    collnames.push_back("StacoMuonCollection");
  else if(strstr(fColl.c_str(),"ID"))
    collnames.push_back("TrackParticleCandidate");
  else if(strstr(fColl.c_str(),"Trigger")) {
     collnames.push_back("Trigger_Level_1");
     collnames.push_back("Trigger_Level_2_MS");
     collnames.push_back("Trigger_Level_2_CB");
     collnames.push_back("Trigger_Level_3_MS");
     collnames.push_back("Trigger_Level_3_CB");
  }
  else if(strstr(fColl.c_str(),"Comparison"))
    collnames.push_back("Comparison");

  // write out file
  FILE *fout = fopen(fColl.c_str(),"w+");
  fprintf(fout,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\" width=\"450\" height=\"300\">\n");
  fprintf(fout,"<html>\n");
  fprintf(fout,"<head>\n");
  fprintf(fout,"<title>%s</title>\n", fColl.c_str());
  fprintf(fout,"</head>\n");
  fprintf(fout,"<body>\n");
  fprintf(fout,"<h1>%s</h1>\n", fColl.c_str());
  
  for(int i=0; i<(int)collnames.size(); i++) {     
    for(int j=0; j<(int)hVector.size(); j++) {
      if(strstr(hVector[j].c_str(),collnames[i].c_str())) {
	if(collnames[i] == "Trigger_Level_2_MS") {
	  if(strstr(hVector[j].c_str(), "Eta") ) {
	    fprintf(fout,"<img src=\"%s\" width=\"450\" height=\"300\">\n", hVector[j].c_str());
	  }
	}else
	  fprintf(fout,"<img src=\"%s\" width=\"450\" height=\"300\">\n", hVector[j].c_str());
      }
    }
  }
  
  fprintf(fout, "</body>\n");
  fprintf(fout, "</html>\n");
  fclose(fout);
}

//=====================================================================
//
//  OpenFile
//
//=====================================================================
void OpenFile(const char* filename) {

  //open file
  TFile *file = new TFile(filename);
    
  // Select the file
  file = new TFile(filename);
  if (file->IsZombie()) {
    cout << "No file for iteration " << "!\n" << endl;
    cout << "Finished OK! \n" << endl;
  }
  else { 
    tree = (TTree*)file->Get("CollectionTree");     
    int events = tree->GetEntries();
    cout << " - Events: " << events << endl;     
  }
}

//=====================================================================
//
//  MuonPerformanceComparisonPlots
//
//=====================================================================
void MuonPerformanceComparisonPlots(const char* filename, std::string obs)
{  
 
  //open file
  TFile *file = new TFile(filename);
  TDirectory* testdir = gDirectory;

  std::string colldir="MuonSpectrometerPerformance/"; 
  const char* colldir_char=colldir.c_str ();  
  cout << "**********************************************************" << endl;
  cout << "**   MuonRecValidationComparisonPlots Directory : " << colldir_char << "   **" << endl;
  cout << "**********************************************************" << endl;   

  TCanvas *MyC1 = new TCanvas("c","MuonRecValidator",1000,700);
  gPad->SetBottomMargin(0.14);
  gPad->SetTopMargin(0.17);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.1);

  double min, max;
  getAxisRangeForObs(testdir, colldir, obs, min, max);

  MuonPerformancePlotsOneShotSame(testdir, MyC1, "TrackParticleCandidate", colldir, obs, min, max, false);
  MuonPerformancePlotsOneShotSame(testdir, MyC1, "MooreTrackParticles", colldir, obs, min, max, false);
  MuonPerformancePlotsOneShotSame(testdir, MyC1, "MuonboyMuonSpectroOnlyTrackParticles", colldir, obs, min, max, false);
  MuonPerformancePlotsOneShotSame(testdir, MyC1, "StacoMuonCollection", colldir, obs, min, max, false);
  MuonPerformancePlotsOneShotSame(testdir, MyC1, "MuidMuonCollection", colldir, obs, min, max, true);

}

void getAxisRangeForObs(TDirectory* testdir, std::string colldir, std::string obs, double& min, double& max)
{
  std::vector<string> collname;
  collname.push_back("TrackParticleCandidate");
  collname.push_back("MooreTrackParticles");
  collname.push_back("MuonboyMuonSpectroOnlyTrackParticles");
  collname.push_back("StacoMuonCollection");
  collname.push_back("MuidMuonCollection");

  for(int i=0; i<(int)collname.size(); i++) {
  
    std::string histoname = colldir;
    histoname += collname[i]+"/"+collname[i]+"_"+obs;
  
    // plot over test 
    TH1F* histo = (TH1F*)testdir->Get(histoname.c_str());

    // get maximum 
    double maxEntry = histo->GetBinContent(histo->GetMaximumBin());
    if(histo->GetBinContent(histo->GetMaximumBin()) > maxEntry)
      maxEntry = histo->GetBinContent(histo->GetMaximumBin());
    // get minimum
    double minEntry = histo->GetBinContent(histo->GetMinimumBin());
    if(histo->GetBinContent(histo->GetMinimumBin()) > minEntry)
      minEntry = histo->GetBinContent(histo->GetMinimumBin());
  
    double low  = minEntry-fabs(.2*minEntry);
    double high = maxEntry+fabs(.2*maxEntry);

    if(!i || min>low)
      min = low;

    if(!i || max<high)
      max = high;
  }
}

//=====================================================================
//
//  MuonPerformancePlots
//
//=====================================================================
void MuonPerformancePlots(const char* filename, std::string collname)
{  
 
  //open file
  TFile *file = new TFile(filename);
  TDirectory* testdir = gDirectory;

  //give Collection:
  const char* collname_char=collname.c_str ();   
  cout << "**********************************************************" << endl;
  cout << "**   MuonRecValidationPlots for : " << collname_char << "   **" << endl;
  cout << "**********************************************************" << endl;   

  std::string colldir="MuonSpectrometerPerformance/"+collname; 
  const char* colldir_char=colldir.c_str ();  
  cout << "**********************************************************" << endl;
  cout << "**   MuonRecValidationPlots Directory : " << colldir_char << "   **" << endl;
  cout << "**********************************************************" << endl;   

  std::string outpsname="MuonRecValidationPlots"+collname+".ps";  
  const char* outpsname_char=outpsname.c_str ();   
  cout << "**********************************************************" << endl;
  cout << "**   Output postscript file : " << outpsname_char << "   **" << endl;
  cout << "**********************************************************" << endl;   

  TTree *muonval=(TTree*) gDirectory->Get(colldir_char);
  muonval->UseCurrentStyle(); 
  muonval->ls();
  muonval->Print();

  TCanvas *MyC1 = new TCanvas("c","MuonRecValidator",1000,700);
  gPad->SetBottomMargin(0.14);
  gPad->SetTopMargin(0.17);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.1);

  // tree observables
  if(!strstr(collname_char,"Trigger"))
    MuonPerformancePlotsFromTree(testdir, MyC1, collname);

  MyC1->Clear();
  MyC1->SetLogy(0);

  // pt eta phi histograms 
  MuonPerformancePlotsPtEtaPhi(testdir, MyC1, collname, colldir, "Efficiency");
  
  if(!strstr(collname_char,"Trigger")) {
    MuonPerformancePlotsPtEtaPhi(testdir, MyC1, collname, colldir, "FakeRate");
    //MuonPerformancePlotsPtEtaPhi(testdir, MyC1, collname, colldir, "Triggered_Efficiency");
    //MuonPerformancePlotsPtEtaPhi(testdir, MyC1, collname, colldir, "NotTriggered_Efficiency");
    MuonPerformancePlotsPtEtaPhi(testdir, MyC1, collname, colldir, "Resolution_Width");
    MuonPerformancePlotsPtEtaPhi(testdir, MyC1, collname, colldir, "Resolution_Mean");
  }
    
  // single one shot histograms
  if(collname != "TrackParticleCandidate" && !strstr(collname_char,"Trigger")) {
  
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "MetricDistance_EndCap");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "MetricDistance_Barrel");
    MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "pT_Resolution");
    MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "pT_Resolution_Eta_EndCap");
    MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "pT_Resolution_Eta_Barrel");
    MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "Eta_Resolution");
    MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "Phi_Resolution");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "Status");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "RealDOF");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "FakeDOF");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "ImpactParameter_Z0_Resolution");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "ImpactParameter_D0_Resolution");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "Pull_Phi_Resolution");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "Pull_Eta_Resolution");
    //MuonPerformancePlotsOneShot(testdir, MyC1, collname, colldir, "Pull_Pt_Resolution");
    
  }
}

//=====================================================================
//
//  MuonPerformancePlotsFromTree
//
//=====================================================================

void MuonPerformancePlotsFromTree(TDirectory* testdir, TCanvas* MyC1, std::string collname)
{
  TTree* tree = (TTree*)testdir->Get("CollectionTree");
  TFile* tmpfile;
  if(!castorConn) 
    tmpfile = new TFile(reference_file.c_str());
  else 
    tmpfile = TFile::Open(reference_file.c_str());
  TTree* ref_tree = (TTree*)tmpfile->Get("CollectionTree");
  std::string var;
  TVirtualPad* padC1;

  ///////////////////////////////////////////// 2x2 canvas 1 /////////////////////////////////////////////

  MyC1->Clear();
  MyC1->Divide(2,2);
  MyC1->SetTitle(collname.c_str());

  // charge
  MyC1->cd(1);
  padC1 = MyC1->GetPad(1);
  padC1->SetLogy(0);
  var = collname+"_m_charge";
  TH1F* charge = new TH1F("charge","charge",3,-1.5,1.5);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, charge, collname, "Muon charge", "normalized to unity", true);

  // Phi
  MyC1->cd(2);
  padC1 = MyC1->GetPad(2);
  padC1->SetLogy(0);
  var = collname+"_m_fPhi";
  TH1F* Phi = new TH1F("fPhi","fPhi",(int)binPhi.size()-1, &(binPhi[0]));
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, Phi, collname, "#phi", "normalized to unity", true);
  
  // Eta
  MyC1->cd(3);
  padC1 = MyC1->GetPad(3);
  padC1->SetLogy(0);
  var = collname+"_m_fEta";
  TH1F* Eta = new TH1F("fEta","fEta",(int)binEta.size()-1, &(binEta[0]));
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, Eta, collname, "#eta", "normalized to unity", true);
  
  // Pt
  MyC1->cd(4);
  padC1 = MyC1->GetPad(4);
  padC1->SetLogy(0);
  var = collname+"_m_fPt";
  TH1F* Pt = new TH1F("fPt","fPt",150, 0., 150000.);
  Pt->SetAxisRange(0., 0.3, "Y");
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, Pt, collname, "p_{T} [MeV]", "normalized to unity", true);

  std::string idname = "TreeVar_2x2_1";
  std::string outname =  collname+"_"+idname+"_"+test_release+plotname_extension;
  MyC1->SaveAs(outname.c_str());
  std::string vecname = collname+"_"+idname+"_"+test_release+".gif";
  hVector.push_back(vecname); 

  ///////////////////////////////////////////// 2x2 canvas 2 /////////////////////////////////////////////

  MyC1->Clear();
  MyC1->Divide(2,2);
  MyC1->SetTitle(collname.c_str());

  // number of hits
  MyC1->cd(1);
  padC1 = MyC1->GetPad(1);
  padC1->SetLogy(0);
  var = collname+"_m_numberOfHits";
  TH1F* nHits = new TH1F("numberOfHits","numberOfHits", 60, 0., 60.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, nHits, collname, "Number of MDT hits", "entries", true);

  // deltaPhi
  MyC1->cd(2);
  padC1 = MyC1->GetPad(2);
  padC1->SetLogy(1);
  TH1F* deltaPhi = new TH1F("fDeltaPhi","fDeltaPhi",100,0.,1.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, deltaPhi, collname, "#Delta#phi", "normalized to unity", true);

  // deltaEta
  MyC1->cd(3);
  padC1 = MyC1->GetPad(3);
  padC1->SetLogy(1);
  TH1F* deltaEta = new TH1F("fDeltaEta","fDeltaEta",100,0.,1.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, deltaEta, collname, "#Delta#eta", "normalized to unity", true);

  // deltaPt
  MyC1->cd(4);
  padC1 = MyC1->GetPad(4);
  padC1->SetLogy(1);
  TH1F* deltaPt = new TH1F("fDeltaPt","fDeltaPt",150,0.,150000.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, deltaPt, collname, "#Delta p_{T} [MeV]", "normalized to unity", true);

  std::string idname = "TreeVar_2x2_2";
  std::string outname =  collname+"_"+idname+"_"+test_release+plotname_extension;
  MyC1->SaveAs(outname.c_str());
  std::string vecname = collname+"_"+idname+"_"+test_release+".gif";
  hVector.push_back(vecname); 

  ///////////////////////////////////////////// 2x2 canvas 3 /////////////////////////////////////////////
 
  MyC1->Clear();
  MyC1->Divide(2,2);
  MyC1->SetTitle(collname.c_str());

  // chi2
  MyC1->cd(1);
  padC1 = MyC1->GetPad(1);
  padC1->SetLogy(0);
  TH1F* chi2 = new TH1F("chi2","chi2",150,0.,150.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, chi2, collname, "#Chi^{2}", "normalized to unity", true); 
  
  // chi2 over DOF
  MyC1->cd(2);
  padC1 = MyC1->GetPad(2);
  padC1->SetLogy(1);
  TH1F* chi2oDOF = new TH1F("chi2oDOF","chi2oDOF",100,0.,100.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, chi2oDOF, collname, "#Chi^{2}/DOF", "normalized to unity", true);

  std::string idname = "TreeVar_2x2_3";
  std::string outname =  collname+"_"+idname+"_"+test_release+plotname_extension;
  MyC1->SaveAs(outname.c_str());
  std::string vecname = collname+"_"+idname+"_"+test_release+".gif";
  hVector.push_back(vecname); 

  ///////////////////////////////////////////// 2x2 canvas 4 /////////////////////////////////////////////

  MyC1->Clear();
  MyC1->Divide(2,2);
  MyC1->SetTitle(collname.c_str());
 
  // Z0
  MyC1->cd(1);
  padC1 = MyC1->GetPad(1);
  padC1->SetLogy(1);
  TH1F* Z0 = new TH1F("fZ0","fZ0", 100, -8000., 8000.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, Z0, collname, "z_{0} [mm]", "normalized to unity", true);

  // D0
  MyC1->cd(2);
  padC1 = MyC1->GetPad(2);
  padC1->SetLogy(1);
  TH1F* R0 = new TH1F("fR0","fR0", 200, -200., 200.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, R0, collname, "d_{0} [mm]", "normalized to unity", true);

  // et iso
  MyC1->cd(3);
  padC1 = MyC1->GetPad(3);
  padC1->SetLogy(1);
  TH1F* et_iso = new TH1F("et_iso","et_iso",50,0.,100000.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, et_iso, collname, "E_{T} isolation [MeV]", "normalized to unity", true);

  // energy loss
  MyC1->cd(4);
  padC1 = MyC1->GetPad(4);
  padC1->SetLogy(1);
  TH1F* energyLoss = new TH1F("energyLoss","energyLoss", 50, 0., 100000.);
  MuonPerformancePlotsTreeVar(tree, ref_tree, padC1, energyLoss, collname, "E loss [MeV]", "normalized to unity", true);

  std::string idname = "TreeVar_2x2_4";
  std::string outname =  collname+"_"+idname+"_"+test_release+plotname_extension;
  MyC1->SaveAs(outname.c_str());
  std::string vecname = collname+"_"+idname+"_"+test_release+".gif";
  hVector.push_back(vecname); 
}

//=====================================================================
//
//  MuonPerformancePlotsFromTreeVar
//
//=====================================================================

void MuonPerformancePlotsTreeVar(TTree* tree, TTree* ref_tree, TVirtualPad* padC1, TH1F* hobs, std::string collname, std::string xlabel, std::string ylabel, bool normalize)
{
  std::cout << "plotting tree var " << hobs->GetName() << std::endl;

  padC1->SetLeftMargin(0.17);
  padC1->SetRightMargin(0.12);
  padC1->SetTopMargin(0.17);
  padC1->SetBottomMargin(0.1);

  hobs->Sumw2();
  std::string var = hobs->GetTitle();
  std::string plotvar = collname+"_m_"+var+">>"+hobs->GetName();

  tree->Draw(plotvar.c_str());
 
  TH1F* hobs_ref = (TH1F*)hobs->Clone();
  hobs_ref->Reset();
  std::string hobs_ref_name = hobs->GetName();
  hobs_ref_name = hobs_ref_name+"_ref";
  hobs_ref->SetName(hobs_ref_name.c_str());
  plotvar = collname+"_m_"+var+">>"+hobs_ref->GetName();
  ref_tree->Draw(plotvar.c_str());

  // normalize
  if(normalize) {
    double integ = hobs_ref->Integral(); 
    if(integ) {
      double norm = 1/integ;
      hobs_ref->Scale(norm);
    }
    integ = hobs->Integral(); 
    if(integ) {
      double norm = 1/integ;
      hobs->Scale(norm);
    }
  }

  // get maximum 
  double maxEntry = hobs_ref->GetBinContent(hobs_ref->GetMaximumBin());
  if(hobs->GetBinContent(hobs->GetMaximumBin()) > maxEntry)
    maxEntry = hobs->GetBinContent(hobs->GetMaximumBin());
  // get minimum
  double minEntry = hobs_ref->GetBinContent(hobs_ref->GetMinimumBin());
  if(hobs->GetBinContent(hobs->GetMinimumBin()) > minEntry)
    minEntry = hobs->GetBinContent(hobs->GetMinimumBin());

  double low  = minEntry-fabs(.4*minEntry);
  double high = maxEntry+fabs(.4*maxEntry);

  // adjust y-axis range
  if(high > 0 || !padC1->GetLogy())
    hobs_ref->SetMaximum(high);
  if(low > 0 || !padC1->GetLogy())
    hobs_ref->SetMinimum(low);

  hobs_ref->SetLineColor(2);
  hobs_ref->SetStats(0);

  hobs_ref->GetXaxis()->SetTitle(xlabel.c_str());
  hobs_ref->GetXaxis()->SetTitleColor(1);
  hobs_ref->GetYaxis()->SetTitle(ylabel.c_str());
  hobs_ref->GetYaxis()->SetTitleColor(1);
  
  hobs_ref->Draw("hist");
  hobs->SetMarkerStyle(20);
  hobs->SetMarkerColor(4);
  hobs->SetLineColor(4);
  hobs->Draw("samePE");

  TLegend *leg = new TLegend(0.78, 0.85, 1., 1.);
  leg->AddEntry(hobs_ref, reference_release.c_str(), "L");
  leg->AddEntry(hobs, test_release.c_str(), "P");
  leg->SetBorderSize(0);
  leg->SetLineColor(1);
  leg->SetLineWidth(0.5);
  leg->SetFillColor(0);
  leg->Draw();

  TPaveText *pt = new TPaveText(0.012, 0.85, 0.77, 1.,"NDC");
  pt->SetFillColor(0);
  pt->SetBorderSize(0);
  pt->SetLineColor(1);
  pt->SetTextColor(9);
  pt->SetTextSize(0.065);
  text = pt->AddText(collname.c_str());
  pt->Draw();

}

//=====================================================================
//
//  MuonPerformancePlotsOneShot
//
//=====================================================================

void MuonPerformancePlotsOneShot(TDirectory* testdir, TCanvas* MyC1, std::string collname, std::string colldir, std::string OneShot) {

  std::string histoname;
  MyC1->SetLogy(0);
  MyC1->cd();
  histoname = colldir;
  histoname += "/"+collname+"_"+OneShot;

  // plot reference
  TFile* tmpfile;
  if(!castorConn) 
    tmpfile = new TFile(reference_file.c_str());
  else 
    tmpfile = TFile::Open(reference_file.c_str());
  
  TH1F *href = (TH1F*)tmpfile->Get(histoname.c_str());

  if(strstr(OneShot.c_str(),"Resolution")) {
    MyC1->SetLogy(1);
    if(strstr(OneShot.c_str(),"Mean"))
      href->GetYaxis()->SetTitle("Resolution mean");
    if(strstr(OneShot.c_str(),"Width"))
      href->GetYaxis()->SetTitle("Resolution Width");
  }
  
  href->SetLineColor(2);  
  href->SetFillColor(0);
  href->SetStats(0);
  href->Draw("hist");

  // plot over test 
  TH1F* histo = (TH1F*)testdir->Get(histoname.c_str());
  histo->SetMarkerColor(4);
  histo->Draw("samePE");

  TLegend *leg = new TLegend(0.02, 0.87, 0.2, 1.0);
  leg->AddEntry(href, reference_release.c_str(), "L");
  leg->AddEntry(histo, test_release.c_str(), "P");
  leg->SetLineColor(1);
  leg->SetFillColor(0);
  leg->Draw("same");

  TPaveText *pt = new TPaveText(0.3, 0.87, 0.8, 0.97,"NDC");
  pt->SetFillColor(0);
  pt->SetLineColor(0);
  pt->SetTextColor(9);
  text = pt->AddText(collname.c_str());
  pt->Draw("same");

  histoname = collname+"_"+OneShot+"_"+test_release+plotname_extension;
  MyC1->SaveAs(histoname.c_str());
  std::string vecname = collname+"_"+OneShot+"_"+test_release+".gif";
  hVector.push_back(vecname);
}

void MuonPerformancePlotsOneShotSame(TDirectory* testdir, TCanvas* MyC1, std::string collname, std::string colldir, std::string OneShot, double min, double max, bool save) {

  std::string histoname;

  MyC1->cd();
  histoname = colldir;
  histoname += collname+"/"+collname+"_"+OneShot;

  int line_col, marker_col, marker_style;
  double x1, x2, y1, y2;
  std::string alg;

  if(collname == "TrackParticleCandidate") {
    line_col = 8;
    marker_col = line_col;
    marker_style = 22;
    x1 = 0.21;
    y1 = 0.85;
    alg = "ID";
  }else if(collname == "MooreTrackParticles") {
    line_col = 38;
    marker_col = line_col;
    marker_style = 24;
    x1 = 0.41;
    y1 = 0.85;
    alg = "Moore";
  }else if(collname == "MuonboyMuonSpectroOnlyTrackParticles") {
    line_col = 45;
    marker_col = line_col;
    marker_style = 25;
    x1 = 0.61.;
    y1 = 0.85;
    alg = "MuonBoy";
  }else if(collname == "MuidMuonCollection") {
    line_col = 4;
    marker_col = line_col;
    marker_style = 21;
    x1 = 0.41;
    y1 = 0.92;
    alg = "Muid";
  }else if(collname == "StacoMuonCollection") {
    line_col = 2;
    marker_col = line_col;
    marker_style = 20;
    x1 = 0.61;
    y1 = 0.92;
    alg = "STACO";
  }
  x2 = x1 + 0.2;
  y2 = y1 + 0.06;
  
  /*
  // plot reference
  TFile* tmpfile;
  if(!castorConn) 
  tmpfile = new TFile(reference_file.c_str());
  else 
  tmpfile = TFile::Open(reference_file.c_str());
  TH1F *href = (TH1F*)tmpfile->Get(histoname.c_str());
  href->SetLineColor(line_col);  
  href->SetFillColor(0);
  href->SetStats(0);
  if(collname == "TrackParticleCandidate")
  href->DrawCopy("hist");
  else
  href->DrawCopy("samehist");
  */
  
  // plot over test 
  TH1F* histo = (TH1F*)testdir->Get(histoname.c_str());
  histo->SetMarkerColor(marker_col);
  histo->SetLineColor(line_col);
  histo->SetMarkerStyle(marker_style);
  //Stylist(MyC1, histo, OneShot);
  
  if(strstr(OneShot.c_str(),"Resolution")) {
    if(strstr(OneShot.c_str(),"Mean"))
      histo->GetYaxis()->SetTitle("Resolution mean");
    if(strstr(OneShot.c_str(),"Width"))
      histo->GetYaxis()->SetTitle("Resolution Width");
  }

  if(max>0 || !MyC1->GetLogy())
    histo->SetMaximum(max);
  if(min>0 || !MyC1->GetLogy())
    histo->SetMinimum(min);

  if(collname == "TrackParticleCandidate")
    histo->DrawCopy("PE");
  else
    histo->DrawCopy("samePE");
  
  TLegend *leg = new TLegend(x1, y1, x2, y2,"","NDC");
  leg->AddEntry(histo, alg.c_str(), "P");
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->GetEntry()->SetTextColor(line_col);
  leg->GetEntry()->SetTextSize(0.05);
  leg->Draw("same");

  if(save) {
    
    TPaveText *pt = new TPaveText(0.02, 0.9, 0.2, 0.96, "NDC");
    pt->AddText(test_release.c_str());
    pt->SetLineColor(0);
    pt->SetFillColor(5);
    pt->Draw();
    
    histoname = "Comparison_"+OneShot+"_"+test_release+plotname_extension;        
    MyC1->SaveAs(histoname.c_str()); 
    std::string vecname = "Comparison_"+OneShot+"_"+test_release+".gif";
    hVector.push_back(vecname);
  }
}

//=====================================================================
//
//  MuonPerformancePlotsPtEtaPhi
//
//=====================================================================

void MuonPerformancePlotsPtEtaPhi(TDirectory* testdir, TCanvas* MyC1, std::string collname, std::string colldir, std::string PtEtaPhi) {

  std::string histoname;

  ///////////////////////////////////////////////////////////// pt

  MyC1->cd();
  MyC1->SetLogy(0);

  histoname = colldir;
  histoname += "/"+collname+"_"+PtEtaPhi+"_Pt_h";

  // plot reference
  TFile* tmpfile;
  if(!castorConn) 
    tmpfile = new TFile(reference_file.c_str());
  else 
    tmpfile = TFile::Open(reference_file.c_str());

  TH1F *href = (TH1F*)tmpfile->Get(histoname.c_str());

  std::string ylabel;
  if(strstr(PtEtaPhi.c_str(),"Resolution")) {
    if(strstr(PtEtaPhi.c_str(),"Mean"))
      ylabel = "Resolution mean";
    else if(strstr(PtEtaPhi.c_str(),"Width"))
      ylabel = "Resolution width";
    else
      ylabel = href->GetYaxis()->GetTitle();
  }else
    ylabel = href->GetYaxis()->GetTitle();
  
  href->SetLineColor(2);
  href->SetFillColor(0);
  href->SetStats(0);
  Stylist(MyC1, href, PtEtaPhi+"_Pt");
  href->GetXaxis()->SetTitle("p_{T} [MeV]");
  href->GetYaxis()->SetTitle(ylabel.c_str());
  href->Draw("hist");
  
  // plot over test
  TH1F* histo = (TH1F*)testdir->Get(histoname.c_str());
  histo->SetMarkerColor(4);
  histo->Draw("samePE");

  TLegend *leg = new TLegend(0.02, 0.87, 0.2, 1.0);
  leg->AddEntry(href, reference_release.c_str(), "L");
  leg->AddEntry(histo, test_release.c_str(), "P");
  leg->SetLineColor(1);
  leg->SetFillColor(0);
  leg->Draw("same");

  TPaveText *pt = new TPaveText(0.3, 0.87, 0.8, 0.97,"NDC");
  pt->SetFillColor(0);
  pt->SetLineColor(0);
  pt->SetTextColor(9);
  text = pt->AddText(collname.c_str());
  pt->Draw("same");

  histoname = collname+"_"+PtEtaPhi+"_Pt_"+test_release+plotname_extension;
  MyC1->SaveAs(histoname.c_str()); 
  std::string vecname = collname+"_"+PtEtaPhi+"_Pt_"+test_release+".gif";
  hVector.push_back(vecname);

  ///////////////////////////////////////////////////////////// eta

  MyC1->cd();
  histoname = colldir;
  histoname += "/"+collname+"_"+PtEtaPhi+"_Eta_h";
  
  // plot reference
  href = (TH1F*)tmpfile->Get(histoname.c_str());
  href->SetLineColor(2);
  href->SetFillColor(0);
  href->SetStats(0);
  Stylist(MyC1, href, PtEtaPhi+"_Eta");
  href->GetXaxis()->SetTitle("#eta");
  href->GetYaxis()->SetTitle(ylabel.c_str());
  href->Draw("hist");
  
  // plot over test
  TH1F* histo = (TH1F*)testdir->Get(histoname.c_str());
  histo->SetMarkerColor(4);
  histo->GetXaxis()->SetTitle("#eta");
  histo->Draw("samePE");

  TLegend *leg = new TLegend(0.02, 0.87, 0.2, 1.0);
  leg->AddEntry(href, reference_release.c_str(), "L");
  leg->AddEntry(histo, test_release.c_str(), "P");
  leg->SetLineColor(1);
  leg->SetFillColor(0);
  leg->Draw("same");

  TPaveText *pt = new TPaveText(0.3, 0.87, 0.8, 0.97,"NDC");
  pt->SetFillColor(0);
  pt->SetLineColor(0);
  pt->SetTextColor(9);
  text = pt->AddText(collname.c_str());
  pt->Draw("same");

  histoname = collname+"_"+PtEtaPhi+"_Eta_"+test_release+plotname_extension;
  MyC1->SaveAs(histoname.c_str());
  std::string vecname = collname+"_"+PtEtaPhi+"_Eta_"+test_release+".gif";
  hVector.push_back(vecname);

  ///////////////////////////////////////////////////////////// phi

  MyC1->cd();
  histoname = colldir;
  histoname += "/"+collname+"_"+PtEtaPhi+"_Phi_h";
  
  // plot reference
  href = (TH1F*)tmpfile->Get(histoname.c_str());
  href->SetLineColor(2);
  href->SetFillColor(0);
  href->SetStats(0);
  Stylist(MyC1, href, PtEtaPhi+"_Phi");
  href->GetXaxis()->SetTitle("#phi");
  href->GetYaxis()->SetTitle(ylabel.c_str());
  href->Draw("hist");
  
  // plot over test
  TH1F* histo = (TH1F*)testdir->Get(histoname.c_str());
  histo->SetMarkerColor(4);
  histo->Draw("samePE");

  TLegend *leg = new TLegend(0.02, 0.87, 0.2, 1.0);
  leg->AddEntry(href, reference_release.c_str(), "L");
  leg->AddEntry(histo, test_release.c_str(), "P");
  leg->SetLineColor(1);
  leg->SetFillColor(0);
  leg->Draw("same");

  TPaveText *pt = new TPaveText(0.3, 0.87, 0.8, 0.97,"NDC");
  pt->SetFillColor(0);
  pt->SetLineColor(0);
  pt->SetTextColor(9);
  text = pt->AddText(collname.c_str());
  pt->Draw("same");

  histoname = collname+"_"+PtEtaPhi+"_Phi_"+test_release+plotname_extension;
  MyC1->SaveAs(histoname.c_str());
  std::string vecname = collname+"_"+PtEtaPhi+"_Phi_"+test_release+".gif";
  hVector.push_back(vecname);

}

void MuonPerformanceIntegralEff(FILE* fout, std::string collname)
{
  Double_t eff_test, err_test, eff_ref, err_ref;
  std::string hname;
  TFile *file;
  // test 
  file = new TFile(test_file.c_str());
  hname = "MuonSpectrometerPerformance/"+collname+"/"+collname+"_Efficiency_Overall";  
  TH1F *heff = (TH1F*)file->Get(hname.c_str());
  eff_test = heff->GetBinContent(2);
  hname = "MuonSpectrometerPerformance/"+collname+"/"+collname+"_EfficiencyUncertainty_Overall";  
  heff = (TH1F*)file->Get(hname.c_str());
  err_test = heff->GetBinContent(2);
  // reference
  if(!castorConn) 
    file = new TFile(reference_file.c_str());
  else 
    file = TFile::Open(reference_file.c_str());
  hname = "MuonSpectrometerPerformance/"+collname+"/"+collname+"_Efficiency_Overall";  
  heff = (TH1F*)file->Get(hname.c_str());
  eff_ref = heff->GetBinContent(2);
  hname = "MuonSpectrometerPerformance/"+collname+"/"+collname+"_EfficiencyUncertainty_Overall";  
  heff = (TH1F*)file->Get(hname.c_str());
  err_ref = heff->GetBinContent(2);

  printf("%s - \t\t\t %s = %.4f +- %.4f *** %s = %.4f %.4f\n", 
	 collname.c_str(), test_release.c_str(), eff_test, err_test, reference_release.c_str(), eff_ref, err_ref);
  fprintf(fout, "<TR> <TD> %s </TD>      <TD> %.4f +- %.4f </TD>      <TD> %.4f +- %.4f </TD> </TR>\n", 
	  collname.c_str(), eff_test, err_test, eff_ref, err_ref);
}

//=====================================================================
//
//  Stylist
//
//=====================================================================

void Stylist(TCanvas *c, TH1F* h, std::string type)
{
  if(strstr(type.c_str(),"Efficiency_Pt")) {
    h->SetAxisRange(0., 1.05, "Y");
  }
  else if(strstr(type.c_str(),"Efficiency_Eta") || strstr(type.c_str(),"Efficiency_Phi")) {
    h->SetAxisRange(0.4, 1.05, "Y");
  }
  else if(strstr(type.c_str(),"FakeRate_Pt")) {
    h->SetAxisRange(0., 0.5, "Y");
  }
  else if(strstr(type.c_str(),"FakeRate_Eta") || strstr(type.c_str(),"FakeRate_Phi")) {
    h->SetAxisRange(0., 0.1, "Y");
  }
  else if(strstr(type.c_str(),"Resolution_Width_Pt")) {
    h->SetAxisRange(0., 0.1, "Y");
  }
  else if(strstr(type.c_str(),"Resolution_Width_Eta") || strstr(type.c_str(),"Resolution_Width_Phi")) {
    h->SetAxisRange(0., 0.1, "Y");
  }
  else if(strstr(type.c_str(),"Resolution_Mean_Pt")) {
    h->SetAxisRange(-0.1, 0.1, "Y");
  }
  else if(strstr(type.c_str(),"Resolution_Mean_Eta") || strstr(type.c_str(),"Resolution_Mean_Phi")) {
    h->SetAxisRange(-0.1, 0.1, "Y");
  }
}

//=====================================================================
//
//  Binning
//
//=====================================================================

void SetBinning()
{  
  binEta.push_back(-2.50);
  binEta.push_back(-2.00);
  binEta.push_back(-1.60);
  binEta.push_back(-1.30);
  binEta.push_back(-1.05);
  binEta.push_back(-0.90);
  binEta.push_back(-0.75);
  binEta.push_back(-0.60);
  binEta.push_back(-0.45);
  binEta.push_back(-0.25);
  binEta.push_back(0.0);
  binEta.push_back(0.25);
  binEta.push_back(0.45);
  binEta.push_back(0.60);
  binEta.push_back(0.75);
  binEta.push_back(0.90);
  binEta.push_back(1.30);
  binEta.push_back(1.60);
  binEta.push_back(2.00);
  binEta.push_back(2.50);

  binPhi.push_back(-180.0 * 0.0174533);
  binPhi.push_back(-165.0 * 0.0174533);
  binPhi.push_back(-150.0 * 0.0174533);
  binPhi.push_back(-120.0 * 0.0174533);
  binPhi.push_back(-105.0  * 0.0174533);
  binPhi.push_back(-75.0  * 0.0174533);
  binPhi.push_back(-60.0  * 0.0174533);
  binPhi.push_back(-30.0  * 0.0174533);
  binPhi.push_back(-15.0 * 0.0174533);
  binPhi.push_back(15.0  * 0.0174533);
  binPhi.push_back(30.0  * 0.0174533);
  binPhi.push_back(60.0  * 0.0174533);
  binPhi.push_back(75.0  * 0.0174533);
  binPhi.push_back(105.0 * 0.0174533);
  binPhi.push_back(120.0 * 0.0174533);
  binPhi.push_back(150.0 * 0.0174533);
  binPhi.push_back(165.0 * 0.0174533);
  binPhi.push_back(180.0 * 0.0174533);

}
