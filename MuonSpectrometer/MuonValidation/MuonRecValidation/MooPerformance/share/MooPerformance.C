//////////////////////////////////////////////////////////////
// Moore Validation/Performance Plots
//
// MooPerformance.C
//
// useage: to run in batch mode type
//         root -q -b 'MooPerformance.C("title of your choice")'
//
// Nektarios Ch. Benekos     Thomas Moore, Stephane Willocq 
// CERN/ATC                  Univ of Massachusetts, Amherst
//
// Feb, 2005
// Vivek Jain, SUNY Albany, Jan 2006:
// Added code to distinguish between real and fake muons
//////////////////////////////////////////////////////////////

#include "MooPerformanceMacros.C"
#include <vector.h>

void MooPerformance(char* genTitle)
{
  gROOT->LoadMacro( "MooVecVecFloat.h+" );
  gROOT->Reset();
  gROOT->SetStyle("Plain");

  gStyle->SetPadBottomMargin(0.20); // def = 0.1
  gStyle->SetPadTopMargin(0.15); // def = 0.1
  gStyle->SetPadLeftMargin(0.15); // def = 0.1, mine = 15
  gStyle->SetPadRightMargin(0.04); // def = 0.1
  gStyle->SetNdivisions(508,"X");
  gStyle->SetNdivisions(506,"Y");
  gStyle->SetTitleH(0.15); // title height (% of pad height?)
  gStyle->SetTitleW(0.30); // title width (% of pad width?)
  gStyle->SetTitleX(0.15); // title top left corner X position (% of pad width?)
  gStyle->SetTitleY(1);    // title top left corner Y position (% of pad width?)

  gStyle->SetHistLineWidth(2.0);
  gStyle->SetFuncColor(kRed);
  gStyle->SetFuncWidth(1.0);
  gStyle->SetPalette(1);
  gStyle->SetOptFit(kTRUE);

  gStyle->SetHistMinimumZero();


  // Process Files-- Either change the file names below or
  // create links with these names.


  printf("genTitle = %s\n", genTitle);

  TChain theTree("CollectionTree");
  theTree.Add("ntuple.root");
  TDirectory* dirTree = new TDirectory("dirTree",genTitle);
  dirTree->cd();
  printf("Chain name is %s\n",theTree.GetName());
  printf("Directory title is %s\n",dirTree->GetTitle());
  //theTree.Process("MooHists.C+");
  theTree.Process("MooHists.C");
  gDirectory->pwd();
  gROOT->cd();

  ///////////////////////////////
  // make pT dependent graphs
  ///////////////////////////////

   //  makepTGraphs("pTRes", "1/p_{T} Resolution");
   //  makepTGraphs("effVsPhi1Bin", "Efficiency");

  ////////////////////////////
  // Start plotting
  ////////////////////////////

  TCanvas *c1 = new TCanvas("c1","Moore performance",10,10,800,600);

  /////////
  // MOORE
  /////////

  // low-level hit info 
  c1->Clear();
  TPostScript psFile("MooreHits.ps",112);

  gStyle->SetOptStat(111110);
  allpTPage("rpcPhiStatName","");
  allpTPage("tgcPhiStatName","");
  allpTPage("cscEtaStatName","");
  allpTPage("cscPhiStatName","");
  allpTPage("mdtStatName","");
  allpTPage("detType","");
  allpTPage("rpcPhiHitRes","");
  allpTPage("tgcPhiHitRes","");
  allpTPage("cscEtaHitRes","");
  allpTPage("cscPhiHitRes","");
  allpTPage("mdtHitRes","");
  allpTPage("rpcPhiHitSig","");
  allpTPage("tgcPhiHitSig","");
  allpTPage("cscEtaHitSig","");
  allpTPage("cscEtaHitSig2","");
  allpTPage("cscPhiHitSig","");
  allpTPage("cscPhiHitSig2","");
  allpTPage("mdtHitSig","");
  allpTPage("nAllHits","");
  gStyle->SetOptStat(0);
  allpTPage2D("nScatXY","");
  allpTPage2D("nScatRZ","");
  allpTPage2D("x0ScatXY","");
  allpTPage2D("x0ScatRZ","");
  psFile.Close();  

  c1->Clear();
  TPostScript psFile("MooreQual.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("nRecTrks","Mo");
  allpTPage("nRecTrks1","Mo");
  allpTPage("chi2","Mo");
  allpTPage("chi2Brl","Mo");
  allpTPage("chi2ECMdt","Mo");
  allpTPage("chi2ECCsc","Mo");
  allpTPage("chi2Prob","Mo");
  allpTPage("chi2ProbBrl","Mo");
  allpTPage("chi2ProbECMdt","Mo");
  allpTPage("chi2ProbECCsc","Mo");
  allpTPage("nPhiHits","");
  allpTPage("nMdtHits","");
  allpTPage("precHitsLayer","");
  allpTPage("precHitsLayerEta1","");
  allpTPage("precHitsLayerEta2","");
  allpTPage("precHitsLayerEta3","");
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPadGridY(kTRUE);
  allpTPage2D("nMdtHitsEtaPhi_real","");
  allpTPage("nMdtHitsVsEta_real","");
  allpTPage2D("nPhiHitsEtaPhi_real","");
  allpTPage("nPhiHitsVsEta_real","");
  allpTPage("nPrecEtaHitsVsEta_real","");
  allpTPage("nTrigEtaHitsVsEta_real","");
  allpTPage2D("nStationsEtaPhi_real","");
  gStyle->SetOptStat(0);
  allpTPage("nStationsVsEta_real","");
  gStyle->SetPadGridY(kFALSE);
  // some comparison of hits/chi-sq for real/fake muons
  gStyle->SetOptStat(111110);
  allpTPage("NumHitsPhi_real","Mo");
  allpTPage("NumHitsPhi_fake","Mo");
  allpTPage("NumHitsMdt_real","Mo");
  allpTPage("NumHitsMdt_fake","Mo");
  allpTPage("statimo_real","Mo");
  allpTPage("statimo_fake","Mo");
  allpTPage("ProbChiSq_real","Mo");
  allpTPage("ProbChiSq_fake","Mo");
  allpTPage("Heta_real","Mo");
  allpTPage("eta_fake","Mo");
  allpTPage("Hphi_real","Mo");
  allpTPage("phi_fake","Mo");
  allpTPage("pT_real","Mo");
  allpTPage("pT_fake","Mo");
  allpTPage("pT1_real","Mo");
  allpTPage("pT1_fake","Mo");
  allpTPage("pT2_real","Mo");
  allpTPage("pT2_fake","Mo");
  gStyle->SetOptStat(kFALSE);
  allpTPage2D("pTVsEta_real","Mo");
  allpTPage2D("pT1VsEta_real","Mo");
  allpTPage2D("pTVsEta_fake","Mo");
  allpTPage2D("pT1VsEta_fake","Mo");
  //
  psFile.Close();

  c1->Clear();
  TPostScript psFile("MooreEff.ps",112);
  gStyle->SetOptStat(0);
  gStyle->SetPadGridY(kTRUE);
  allpTPage("effVsEta","Mo");
  allpTPage2D("effVsEtaPhi","Mo");
  allpTPage("effVsPhiBrl","Mo");
  allpTPage("effVsPhiECMdt","Mo");
  allpTPage("effVsPhiECCsc","Mo");
  // Now look at some fake rates
  allpTPage("effVsEta_fakeEvt","Mo");
  allpTPage("effVsEta_fake1Evt","Mo");
  allpTPage2D("effVsEtaPhi_fake","Mo");
  gStyle->SetPadGridY(kFALSE);
  psFile.Close();

  c1->Clear();
  TPostScript psFile("MooreResolution.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("pTRes","Mo");
  allpTPage("a0Res","Mo");
  allpTPage("z0Res","Mo");
  allpTPage("phiRes","Mo");
  allpTPage("cotThetaRes","Mo");
  allpTPage("pTPull","Mo");
  allpTPage("a0Pull","Mo");
  allpTPage("z0Pull","Mo");
  allpTPage("phiPull","Mo");
  allpTPage("cotThetaPull","Mo");
  allpTPage("pTResBrl","Mo");
  allpTPage("pTResECMdt","Mo");
  allpTPage("pTResECCsc","Mo");
  allpTPage("a0ResBrl","Mo");
  allpTPage("a0ResECMdt","Mo");
  allpTPage("a0ResECCsc","Mo");
  allpTPage("z0ResBrl","Mo");
  allpTPage("z0ResECMdt","Mo");
  allpTPage("z0ResECCsc","Mo");
  allpTPage("phiResBrl","Mo");
  allpTPage("phiResECMdt","Mo");
  allpTPage("phiResECCsc","Mo");
  allpTPage("cotThetaResBrl","Mo");
  allpTPage("cotThetaResECMdt","Mo");
  allpTPage("cotThetaResECCsc","Mo");
  allpTPage("pTPullBrl","Mo");
  allpTPage("pTPullECMdt","Mo");
  allpTPage("pTPullECCsc","Mo");
  allpTPage("a0PullBrl","Mo");
  allpTPage("a0PullECMdt","Mo");
  allpTPage("a0PullECCsc","Mo");
  allpTPage("z0PullBrl","Mo");
  allpTPage("z0PullECMdt","Mo");
  allpTPage("z0PullECCsc","Mo");
  allpTPage("phiPullBrl","Mo");
  allpTPage("phiPullECMdt","Mo");
  allpTPage("phiPullECCsc","Mo");
  allpTPage("cotThetaPullBrl","Mo");
  allpTPage("cotThetaPullECMdt","Mo");
  allpTPage("cotThetaPullECCsc","Mo");
  // Chi2 of each variable as a function of eta
  gStyle->SetOptStat(kFALSE);
  allpTPage2D("chi2_pT","Mo");
  allpTPage2D("chi2_a0","Mo");
  allpTPage2D("chi2_z0","Mo");
  allpTPage2D("chi2_phi","Mo");
  allpTPage2D("chi2_cth","Mo");
  //
  allpTPage("pTResWidthVsEta","Mo");
  allpTPage("pTPullWidthVsEta","Mo");
  allpTPage("a0PullWidthVsEta","Mo");
  allpTPage("z0PullWidthVsEta","Mo");
  allpTPage("phiResWidthVsEta","Mo");
  allpTPage("phiPullWidthVsEta","Mo");
  allpTPage("cotThetaPullWidthVsEta","Mo");
  allpTPage("pTPullMeanVsEta","Mo");
  allpTPage("a0PullMeanVsEta","Mo");
  allpTPage("z0PullMeanVsEta","Mo");
  allpTPage("phiResMeanVsEta","Mo");
  allpTPage("phiPullMeanVsEta","Mo");
  allpTPage("cotThetaPullMeanVsEta","Mo");
  psFile.Close();

  //////////////////
  // Inner Detector
  //////////////////

  c1->Clear();
  TPostScript psFile("InnerDetQual.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("nRecTrks","Id");
  gStyle->SetOptStat(kFALSE);
  allpTPage("chi2","Id");
  allpTPage("chi2Brl","Id");
  allpTPage("chi2ECMdt","Id");
  allpTPage("chi2ECCsc","Id");
  allpTPage("chi2Prob","Id");
  psFile.Close();

  c1->Clear();
  TPostScript psFile("InnerDetEff.ps",112);
  gStyle->SetPadGridY(kTRUE);
  allpTPage("effVsEta","Id");
  allpTPage("effVsPhiBrl","Id");
  allpTPage("effVsPhiECMdt","Id");
  allpTPage("effVsPhiECCsc","Id");
  gStyle->SetPadGridY(kFALSE);
  psFile.Close();

  c1->Clear();
  TPostScript psFile("InnerDetResolution.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("pTRes","Id");
  allpTPage("a0Res","Id");
  allpTPage("z0Res","Id");
  allpTPage("phiRes","Id");
  allpTPage("cotThetaRes","Id");
  allpTPage("pTPull","Id");
  allpTPage("a0Pull","Id");
  allpTPage("z0Pull","Id");
  allpTPage("phiPull","Id");
  allpTPage("cotThetaPull","Id");
  allpTPage("pTResBrl","Id");
  allpTPage("pTResECMdt","Id");
  allpTPage("pTResECCsc","Id");
  allpTPage("a0ResBrl","Id");
  allpTPage("a0ResECMdt","Id");
  allpTPage("a0ResECCsc","Id");
  allpTPage("z0ResBrl","Id");
  allpTPage("z0ResECMdt","Id");
  allpTPage("z0ResECCsc","Id");
  allpTPage("phiResBrl","Id");
  allpTPage("phiResECMdt","Id");
  allpTPage("phiResECCsc","Id");
  allpTPage("cotThetaResBrl","Id");
  allpTPage("cotThetaResECMdt","Id");
  allpTPage("cotThetaResECCsc","Id");
  allpTPage("pTPullBrl","Id");
  allpTPage("pTPullECMdt","Id");
  allpTPage("pTPullECCsc","Id");
  allpTPage("a0PullBrl","Id");
  allpTPage("a0PullECMdt","Id");
  allpTPage("a0PullECCsc","Id");
  allpTPage("z0PullBrl","Id");
  allpTPage("z0PullECMdt","Id");
  allpTPage("z0PullECCsc","Id");
  allpTPage("phiPullBrl","Id");
  allpTPage("phiPullECMdt","Id");
  allpTPage("phiPullECCsc","Id");
  allpTPage("cotThetaPullBrl","Id");
  allpTPage("cotThetaPullECMdt","Id");
  allpTPage("cotThetaPullECCsc","Id");
  allpTPage("pTPullWidthVsEta","Id");
  allpTPage("a0PullWidthVsEta","Id");
  allpTPage("z0PullWidthVsEta","Id");
  allpTPage("phiPullWidthVsEta","Id");
  allpTPage("cotThetaPullWidthVsEta","Id");
  allpTPage("pTPullMeanVsEta","Id");
  allpTPage("a0PullMeanVsEta","Id");
  allpTPage("z0PullMeanVsEta","Id");
  allpTPage("phiPullMeanVsEta","Id");
  allpTPage("cotThetaPullMeanVsEta","Id");
  gStyle->SetOptStat(kFALSE);
  psFile.Close();

  ////////////////////
  // MuId Standalone
  ////////////////////

  c1->Clear();
  TPostScript psFile("StandaloneQual.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("nRecTrks","Mu");
  allpTPage("chi2","Mu");
  allpTPage("chi2Brl","Mu");
  allpTPage("chi2ECMdt","Mu");
  allpTPage("chi2ECCsc","Mu");
  //  allpTPage("chi2Prob","Mu");
  allpTPage("Hphi_real","Mu");
  allpTPage("phi_fake","Mu");
  gStyle->SetOptStat(kFALSE);
  allpTPage2D("pTVsEta_real","Mu");
  allpTPage2D("pT1VsEta_real","Mu");
  allpTPage2D("pTVsEta_fake","Mu");
  allpTPage2D("pT1VsEta_fake","Mu");
  psFile.Close();

  c1->Clear();
  TPostScript psFile("StandaloneEff.ps",112);
  gStyle->SetPadGridY(kTRUE);
  allpTPage("effVsEta","Mu");
  allpTPage("effVsEtaPhi","Mu");
  allpTPage("effVsPhiBrl","Mu");
  allpTPage("effVsPhiECMdt","Mu");
  allpTPage("effVsPhiECCsc","Mu");
  // Now look at some fake rates
  allpTPage("effVsEta_fakeEvt","Mu");
  allpTPage("effVsEta_fake1Evt","Mu");
  allpTPage2D("effVsEtaPhi_fake","Mu");
  gStyle->SetPadGridY(kFALSE);

  psFile.Close();

  c1->Clear();
  TPostScript psFile("StandaloneResolution.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("pTRes","Mu");
  allpTPage("a0Res","Mu");
  allpTPage("z0Res","Mu");
  allpTPage("phiRes","Mu");
  allpTPage("cotThetaRes","Mu");
  allpTPage("pTPull","Mu");
  allpTPage("a0Pull","Mu");
  allpTPage("z0Pull","Mu");
  allpTPage("phiPull","Mu");
  allpTPage("cotThetaPull","Mu");
  allpTPage("pTResBrl","Mu");
  allpTPage("pTResECMdt","Mu");
  allpTPage("pTResECCsc","Mu");
  allpTPage("a0ResBrl","Mu");
  allpTPage("a0ResECMdt","Mu");
  allpTPage("a0ResECCsc","Mu");
  allpTPage("z0ResBrl","Mu");
  allpTPage("z0ResECMdt","Mu");
  allpTPage("z0ResECCsc","Mu");
  allpTPage("phiResBrl","Mu");
  allpTPage("phiResECMdt","Mu");
  allpTPage("phiResECCsc","Mu");
  allpTPage("cotThetaResBrl","Mu");
  allpTPage("cotThetaResECMdt","Mu");
  allpTPage("cotThetaResECCsc","Mu");
  allpTPage("pTPullBrl","Mu");
  allpTPage("pTPullECMdt","Mu");
  allpTPage("pTPullECCsc","Mu");
  allpTPage("a0PullBrl","Mu");
  allpTPage("a0PullECMdt","Mu");
  allpTPage("a0PullECCsc","Mu");
  allpTPage("z0PullBrl","Mu");
  allpTPage("z0PullECMdt","Mu");
  allpTPage("z0PullECCsc","Mu");
  allpTPage("phiPullBrl","Mu");
  allpTPage("phiPullECMdt","Mu");
  allpTPage("phiPullECCsc","Mu");
  allpTPage("cotThetaPullBrl","Mu");
  allpTPage("cotThetaPullECMdt","Mu");
  allpTPage("cotThetaPullECCsc","Mu");
  gStyle->SetOptStat(kFALSE);
  allpTPage("pTPullWidthVsEta","Mu");
  allpTPage("a0PullWidthVsEta","Mu");
  allpTPage("z0PullWidthVsEta","Mu");
  allpTPage("phiPullWidthVsEta","Mu");
  allpTPage("cotThetaPullWidthVsEta","Mu");
  allpTPage("pTPullMeanVsEta","Mu");
  allpTPage("a0PullMeanVsEta","Mu");
  allpTPage("z0PullMeanVsEta","Mu");
  allpTPage("phiPullMeanVsEta","Mu");
  allpTPage("cotThetaPullMeanVsEta","Mu");
  psFile.Close();

  //////////////////
  // MuId Combined
  //////////////////

  c1->Clear();
  TPostScript psFile("CombinedQual.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("nRecTrks","Cb");
  allpTPage("chi2","Cb");
  allpTPage("chi2Brl","Cb");
  allpTPage("chi2ECMdt","Cb");
  allpTPage("chi2ECCsc","Cb");
  allpTPage("chi2Prob","Cb");
  allpTPage("Hphi_real","Cb");
  allpTPage("phi_fake","Cb");
  gStyle->SetOptStat(kFALSE);
  allpTPage2D("pTVsEta_real","Cb");
  allpTPage2D("pT1VsEta_real","Cb");
  allpTPage2D("pTVsEta_fake","Cb");
  allpTPage2D("pT1VsEta_fake","Cb");
  psFile.Close();

  c1->Clear();
  TPostScript psFile("CombinedEff.ps",112);
  gStyle->SetPadGridY(kTRUE);
  allpTPage("effVsEta","Cb");
  allpTPage("effVsEtaPhi","Cb");
  allpTPage("effVsPhiBrl","Cb");
  allpTPage("effVsPhiECMdt","Cb");
  allpTPage("effVsPhiECCsc","Cb");
  // Now look at some fake rates
  allpTPage("effVsEta_fakeEvt","Cb");
  allpTPage("effVsEta_fake1Evt","Cb");
  allpTPage2D("effVsEtaPhi_fake","Cb");
  gStyle->SetPadGridY(kFALSE);
  psFile.Close();

  c1->Clear();
  TPostScript psFile("CombinedResolution.ps",112);
  gStyle->SetOptStat(111110);
  allpTPage("pTRes","Cb");
  allpTPage("a0Res","Cb");
  allpTPage("z0Res","Cb");
  allpTPage("phiRes","Cb");
  allpTPage("cotThetaRes","Cb");
  allpTPage("pTPull","Cb");
  allpTPage("a0Pull","Cb");
  allpTPage("z0Pull","Cb");
  allpTPage("phiPull","Cb");
  allpTPage("cotThetaPull","Cb");
  allpTPage("pTResBrl","Cb");
  allpTPage("pTResECMdt","Cb");
  allpTPage("pTResECCsc","Cb");
  allpTPage("a0ResBrl","Cb");
  allpTPage("a0ResECMdt","Cb");
  allpTPage("a0ResECCsc","Cb");
  allpTPage("z0ResBrl","Cb");
  allpTPage("z0ResECMdt","Cb");
  allpTPage("z0ResECCsc","Cb");
  allpTPage("phiResBrl","Cb");
  allpTPage("phiResECMdt","Cb");
  allpTPage("phiResECCsc","Cb");
  allpTPage("cotThetaResBrl","Cb");
  allpTPage("cotThetaResECMdt","Cb");
  allpTPage("cotThetaResECCsc","Cb");
  allpTPage("pTPullBrl","Cb");
  allpTPage("pTPullECMdt","Cb");
  allpTPage("pTPullECCsc","Cb");
  allpTPage("a0PullBrl","Cb");
  allpTPage("a0PullECMdt","Cb");
  allpTPage("a0PullECCsc","Cb");
  allpTPage("z0PullBrl","Cb");
  allpTPage("z0PullECMdt","Cb");
  allpTPage("z0PullECCsc","Cb");
  allpTPage("phiPullBrl","Cb");
  allpTPage("phiPullECMdt","Cb");
  allpTPage("phiPullECCsc","Cb");
  allpTPage("cotThetaPullBrl","Cb");
  allpTPage("cotThetaPullECMdt","Cb");
  allpTPage("cotThetaPullECCsc","Cb");
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPadGridY(kTRUE);
  allpTPage("pTPullWidthVsEta","Cb");
  allpTPage("a0PullWidthVsEta","Cb");
  allpTPage("z0PullWidthVsEta","Cb");
  allpTPage("phiPullWidthVsEta","Cb");
  allpTPage("cotThetaPullWidthVsEta","Cb");
  allpTPage("pTPullMeanVsEta","Cb");
  allpTPage("a0PullMeanVsEta","Cb");
  allpTPage("z0PullMeanVsEta","Cb");
  allpTPage("phiPullMeanVsEta","Cb");
  allpTPage("cotThetaPullMeanVsEta","Cb");
  gStyle->SetPadGridY(kFALSE);
  psFile.Close();

  /////////////////////////////////////////////////////////////////////
  // Comparisons of Moore/Inner Detector/MuId Standalone/MuId Combined
  // 2x2 plots for a particular pT
  //////////////////////////////////////////////////////////////////////

  //TIter next(gROOT->GetListOfFiles());
  //TFile* f;
  //while ((f = (TFile*)next())) {

  //f->cd();

  TDirectory* currentDir = gDirectory;
  TIter objIter(gDirectory->GetList());
  TObject* obj;
  while (obj = objIter() ) {

    if (obj->InheritsFrom("TDirectory")) {

      ((TDirectory*)obj)->cd();
      char psFileName[256];
      sprintf(psFileName,"ComparisonPlots_%s.ps",((TDirectory*)obj)->GetTitle());
      TPostScript psFile(psFileName,112);
      gStyle->SetOptStat(111110);
      compRecoPage("pTRes");
      compRecoPage("a0Res");
      compRecoPage("z0Res");
      compRecoPage("phiRes");
      compRecoPage("cotThetaRes");
      compRecoPage("pTPull");
      compRecoPage("a0Pull");
      compRecoPage("z0Pull");
      compRecoPage("phiPull");
      compRecoPage("cotThetaPull");
      gStyle->SetOptStat(kFALSE);
      compRecoPage2D("effVsEtaPhi");
      psFile.Close();
    }
    currentDir->cd();
  }

  ///////////////////////////////////////////////////////
  // Overlay the 4 reconstruction stages at all pT values
  ///////////////////////////////////////////////////////

  c1->Clear();
  TPostScript psFile("OverlayPlots.ps",112);
  gStyle->SetPadGridY(kTRUE);
  allpTPage("effVsEta","All","BL");
  allpTPage("effVsPhiBrl","All","BL");
  allpTPage("effVsPhiECMdt","All","BL");
  allpTPage("effVsPhiECCsc","All","BL");
  allpTPage("pTResWidthVsEta","All","TL");
  allpTPage("pTPullWidthVsEta","All","TL");
  allpTPage("pTResWidthVsBrlPhi","All","TL");
  allpTPage("pTPullWidthVsBrlPhi","All","TL");
  allpTPage("pTResWidthVsECMdtPhi","All","TL");
  allpTPage("pTPullWidthVsECMdtPhi","All","TL");
  allpTPage("pTResWidthVsECCscPhi","All","TL");
  allpTPage("pTPullWidthVsECCscPhi","All","TL");
  allpTPage("pTResVsBrlPhiReg","All","TL");
  allpTPage("pTPullVsBrlPhiReg","All","TL");
  allpTPage("pTResVsECMdt1PhiReg","All","TL");
  allpTPage("pTPullVsECMdt1PhiReg","All","TL");
  allpTPage("pTResVsECMdt2PhiReg","All","TL");
  allpTPage("pTPullVsECMdt2PhiReg","All","TL");
  allpTPage("pTResVsECCscPhiReg","All","TL");
  allpTPage("pTPullVsECCscPhiReg","All","TL");
  gStyle->SetPadGridY(kFALSE);
  //  makepTPage();
  psFile.Close();

  c1->Clear();
  makeSummaryPages();

}
