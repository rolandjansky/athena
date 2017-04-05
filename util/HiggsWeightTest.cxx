// $Id$
#include <TFile.h>
#include <TError.h>
#include <TTree.h>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

// ASG include(s):
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include "TruthWeightTools/TruthWeightTool.h"
#include "TruthWeightTools/HiggsWeightTool.h"
#include <TRandom3.h>
#include <TH1F.h>

typedef TString Str;
typedef std::vector<Str> StrV;
typedef std::vector<TH1F*> HistV;
typedef std::vector<double> NumV;
void fatal(TString msg) { printf("\nFATAL:\n\n  %s\n\n",msg.Data()); abort(); }
void fillHistos(HistV &hw, double x, NumV ws) {
  if (hw.size()<ws.size()) fatal(Form("Bad input to fillHistos %lu hs, %lu ws",hw.size(),ws.size()));
  for (size_t i=0;i<ws.size();++i) hw[i]->Fill(x,ws[i]);
}
HistV makeHistos(int N, Str prefix, int Nbins, double min, double max, Str tit="") {
  HistV hv; for (int i=1;i<=N;++i) hv.push_back(new TH1F(prefix+Form("%i",i),tit,Nbins,min,max)); return hv;
}
HistV makeHistos(StrV names, int Nbins, double min, double max, Str tit="") {
  HistV hv; for (Str n:names) hv.push_back(new TH1F(n,tit,Nbins,min,max)); return hv;
}

int main( int argc, char* argv[] ) {

   // Get the application's name:
   const char* APP_NAME = argv[ 0 ];

   std::vector<TString> files;
   bool forceNNLOPS=false;
   bool forceVBF=false;
   bool forceVH=false;
   double weightMax=100.0;
   TString ofn("higgsWeightTest_histo.root");
   for (int i=1;i<argc;++i) {
     TString arg(argv[i]);
     if      (arg=="--forceNNLOPS") forceNNLOPS=true;
     else if (arg=="--forceVBF"   ) forceVBF=true;
     else if (arg=="--forceVH"    ) forceVH=true;
     else if (arg=="--output") ofn=argv[++i];
     else if (arg=="--weightCutOff") weightMax=atof(argv[++i]);
     else if (arg.Contains(".root")) files.push_back(arg);
     else throw std::runtime_error(TString("Cannot intepret argument: "+arg).Data());
   }

   if (files.size()==0) {
     ::Error( APP_NAME, "Usage: %s <xAOD file1> [xAOD file2]...", APP_NAME );
     return 1;
   }

   // output file
   TFile *of = new TFile(ofn,"RECREATE");
   TTree *tree = new TTree("TruthTree","");
   float wnom=0, yH=0, pTH=0, wnlo=0; int nj, fineSTXS;
   tree->Branch("weight",&wnom,"weight/F");
   tree->Branch("weightNLO",&wnlo,"weightNLO/F");
   tree->Branch("y_H",&yH,"y_H/F");
   tree->Branch("pT_H",&pTH,"pT_H/F");
   tree->Branch("Njets30",&nj,"Njets30/I");
   tree->Branch("STXS",&fineSTXS,"STXS/I");
   Str ptTit = ";#it{p}_{T,#it{H}} [GeV]";
   int Nbins=25; double min=0, max=250;

   TH1F *h_wnom1 = new TH1F("w_nom1","Nominal weight",100,0,200);
   TH1F *h_wnom2 = new TH1F("w_nom2","Nominal weight",100,0,20);

   TH1F *h_pTH = new TH1F("pTH",ptTit,Nbins,min,max);
   HistV h_pTH_pdf4lhc = makeHistos(30,"pTH_pdf4lhc",Nbins,min,max,ptTit);
   HistV h_pTH_nnpdf30 = makeHistos(100,"pTH_nnpdf30",Nbins,min,max,ptTit);
   HistV h_pTH_aS = makeHistos({"pTH_aSup","pTH_aSdn"},Nbins,min,max,ptTit);
   // various QCD variations
   HistV h_pTH_qcd         = makeHistos(8,"pTH_qcd",Nbins,min,max,ptTit); // Default Powheg QCD variations (NLO)
   HistV h_pTH_nnlops_qcd  = makeHistos(26,"pTH_nnlops_qcd",Nbins,min,max,ptTit); // NNLOPS internal QCD vars
   HistV h_pTH_nnlops_qcd2 = makeHistos(2,"pTH_nnlo_qcd",Nbins,min,max,ptTit); // NNLO and Powheg vars for NNLOPS
   HistV h_pTH_wg1qcd      = makeHistos(8,"pTH_wg1qcd",Nbins,min,max,ptTit); // WG1 propsed scheme for ggF
   HistV h_pTH_qcd_stxs    = makeHistos(9,"pTH_qcd_stxs",Nbins,min,max,ptTit); // WG1 propsed scheme for ggF

   Nbins=10; min=-0.5; max=9.5; Str tit=";#it{N}_{jets}";
   TH1F *h_Njets = new TH1F("Njets30",tit,Nbins,min,max);
   HistV h_Njets_pdf4lhc = makeHistos(30,"Njets30_pdf4lhc",Nbins,min,max,tit);
   HistV h_Njets_nnpdf30 = makeHistos(100,"Njets30_nnpdf30",Nbins,min,max,tit);
   HistV h_Njets_aS = makeHistos({"Njets30_aSup","Njets30_aSdn"},Nbins,min,max,tit);
   // various QCD variations
   HistV h_Njets_qcd         = makeHistos(8,"Njets30_qcd",Nbins,min,max,tit); // Default Powheg QCD variations (NLO)
   HistV h_Njets_nnlops_qcd  = makeHistos(26,"Njets30_nnlops_qcd",Nbins,min,max,tit); // NNLOPS internal QCD vars
   HistV h_Njets_nnlops_qcd2 = makeHistos(2,"Njets30_nnlo_qcd",Nbins,min,max,tit); // NNLO and Powheg vars for NNLOPS
   HistV h_Njets_wg1qcd      = makeHistos(8,"Njets30_wg1qcd",Nbins,min,max,tit); // WG1 propsed scheme for ggF

   Nbins=52; min=1; max=53; tit=";STXS fine index";
   TH1F *h_STXS = new TH1F("STXS",tit,Nbins,min,max);
   HistV h_STXS_pdf4lhc = makeHistos(30,"STXS_pdf4lhc",Nbins,min,max,tit);
   HistV h_STXS_nnpdf30 = makeHistos(100,"STXS_nnpdf30",Nbins,min,max,tit);
   HistV h_STXS_aS = makeHistos({"STXS_aSup","STXS_aSdn"},Nbins,min,max,tit);
   // various QCD variations
   HistV h_STXS_qcd         = makeHistos(8,"STXS_qcd",Nbins,min,max,tit); // Default Powheg QCD variations (NLO)
   HistV h_STXS_nnlops_qcd  = makeHistos(26,"STXS_nnlops_qcd",Nbins,min,max,tit); // NNLOPS internal QCD vars
   HistV h_STXS_nnlops_qcd2 = makeHistos(2,"STXS_nnlo_qcd",Nbins,min,max,tit); // NNLO and Powheg vars for NNLOPS
   HistV h_STXS_wg1qcd      = makeHistos(8,"STXS_wg1qcd",Nbins,min,max,tit); // WG1 propsed scheme for ggF

   Nbins=80; min=-4; max=4; tit=";#it{y_{H}}";
   TH1F *h_yH = new TH1F("yH",tit,Nbins,min,max);
   HistV h_yH_pdf4lhc = makeHistos(30,"yH_pdf4lhc",Nbins,min,max,tit);
   HistV h_yH_nnpdf30 = makeHistos(100,"yH_nnpdf30",Nbins,min,max,tit);
   HistV h_yH_aS = makeHistos({"yH_aSup","yH_aSdn"},Nbins,min,max,tit);
   // various QCD variations
   HistV h_yH_qcd         = makeHistos(8,"yH_qcd",Nbins,min,max,tit); // Default Powheg QCD variations (NLO)
   HistV h_yH_nnlops_qcd  = makeHistos(26,"yH_nnlops_qcd",Nbins,min,max,tit); // NNLOPS internal QCD vars
   HistV h_yH_nnlops_qcd2 = makeHistos(2,"yH_nnlo_qcd",Nbins,min,max,tit); // NNLO and Powheg vars for NNLOPS
   HistV h_yH_wg1qcd      = makeHistos(8,"yH_wg1qcd",Nbins,min,max,tit); // WG1 propsed scheme for ggF
   HistV h_yH_other       = makeHistos(8,"yH_other",Nbins,min,max,tit); // WG1 propsed scheme for ggF
   
   // Initialise the application:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );
   
   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );

   xAOD::HiggsWeightTool *higgsMCtool = new xAOD::HiggsWeightTool( "HiggsWeightTool" );
   higgsMCtool->setProperty( "OutputLevel", MSG::DEBUG ).ignore();
   higgsMCtool->setProperty( "RequireFinite", true ).ignore();
   higgsMCtool->setProperty( "WeightCutOff", weightMax ).ignore();
   if (forceNNLOPS) higgsMCtool->setProperty("ForceNNLOPS",true).ignore();
   if (forceVBF) higgsMCtool->setProperty("ForceVBF",true).ignore();
   if (forceVH) higgsMCtool->setProperty("ForceVH",true).ignore();
   higgsMCtool->initialize().ignore();

   for (TString fn:files) {
   
     // Open the input file:
     std::unique_ptr< ::TFile > ifile( ::TFile::Open( fn, "READ" ) );
     if( ! ifile.get() ) {
       ::Error( APP_NAME, "Couldn't open file: %s", fn.Data() );
       return 1;
     }
     ::Info( APP_NAME, "Opened file: %s", fn.Data() );
     
     // Connect the TEvent object to it:
     RETURN_CHECK( APP_NAME, event.readFrom( ifile.get() ) );
     
     // Loop over a few events:
     const ::Long64_t entries = event.getEntries();
     for ( ::Long64_t entry = 0; entry < entries; ++entry ) {
       
       // Get the current entry:
       event.getEntry( entry );
       const xAOD::EventInfo *evtInfo;
       RETURN_CHECK( APP_NAME, event.retrieve( evtInfo, "EventInfo" ) );
     
       TLorentzVector h;
       h.SetPtEtaPhiM(gRandom->Gaus(0,40),0,0,125);
       int HTXS_Njets30=gRandom->Poisson(0.9), HTXS_Stage1=0, HTXS_index=0;
       double HTXS_pTH=0;
       // All input files used for test have the HTXS content
       if (evtInfo->isAvailable<int>("HTXS_Njets_pTjet30")) {
	 HTXS_Njets30 = evtInfo->auxdata<int>("HTXS_Njets_pTjet30");
	 HTXS_Stage1 = evtInfo->auxdata<int>("HTXS_Stage1_Category_pTjet30");
	 HTXS_index  = evtInfo->auxdata<int>("HTXS_Stage1_FineIndex_pTjet30");
	 HTXS_pTH  = evtInfo->auxdata<float>("HTXS_Higgs_pt");
	 double HTXS_etaH = evtInfo->auxdata<float>("HTXS_Higgs_eta");
	 double HTXS_phiH = evtInfo->auxdata<float>("HTXS_Higgs_phi");
	 double HTXS_mH   = evtInfo->auxdata<float>("HTXS_Higgs_m");

	 h.SetPtEtaPhiM(HTXS_pTH,HTXS_etaH,HTXS_phiH,HTXS_mH);
	 h*=1e-3; // convert to GeV
       } else HTXS_pTH=h.Pt()*1000;

       // Access all Higgs weights
       xAOD::HiggsWeights hw = higgsMCtool->getHiggsWeights(HTXS_Njets30,HTXS_pTH,HTXS_Stage1);
       // on of my test files lack PDF info..
       
       double n = hw.nominal;
       h_wnom1->Fill(n); h_wnom2->Fill(n);
       bool bad=false;
       for (double q:hw.qcd_nnlops) 
	 if (std::abs(q-n)/n>1) bad=true;
       if (bad) {
	 //printf("Event %llu pT = %.3f, %i jets\n",entry,h.Pt(),HTXS_Njets30);
	 //for (double q:hw.qcd_nnlops) printf("%6.2f",(q-n)/n); printf("\n");
       }

       // 1. Nominal histograms
       h_pTH  -> Fill(h.Pt(),hw.nominal);
       h_Njets-> Fill(HTXS_Njets30,hw.nominal);
       h_yH   -> Fill(h.Rapidity(),hw.nominal);
       h_STXS -> Fill(HTXS_index,hw.nominal);

       // 2. PDF and alphaS uncertainties
       bool doPDF = hw.pdf4lhc_unc.size()==30;
       if (doPDF) {
	 if (std::abs(hw.alphaS_up)>10000) {
	   // Debugging event with NAN values for PDF wegiht in Paul Thompson's file:
	   // /afs/cern.ch/work/t/thompson/public/group.phys-higgs.345097.PowhegPythia8EvtGen_NNLOPS_nnlo_30_ggH125_mumu.evgen.EVNT.e5732.002.TRUTH1_EXT0/group.phys-higgs.10609360.EXT0._000030.DAOD_TRUTH1.mc15.pool.root
	   printf("Found event with crazy weight. Dumping info below. Will ignore it.");
	   hw.print();
	   continue;
	 }
	 fillHistos(h_pTH_pdf4lhc,h.Pt(),hw.pdf4lhc_unc);
	 fillHistos(h_pTH_aS,h.Pt(),{hw.alphaS_up,hw.alphaS_dn});
	 fillHistos(h_yH_pdf4lhc,h.Rapidity(),hw.pdf4lhc_unc);
	 fillHistos(h_yH_aS,h.Rapidity(),{hw.alphaS_up,hw.alphaS_dn});
	 fillHistos(h_Njets_pdf4lhc,HTXS_Njets30,hw.pdf4lhc_unc);
	 fillHistos(h_Njets_aS,HTXS_Njets30,{hw.alphaS_up,hw.alphaS_dn});
	 fillHistos(h_STXS_pdf4lhc,HTXS_index,hw.pdf4lhc_unc);
	 fillHistos(h_STXS_aS,HTXS_index,{hw.alphaS_up,hw.alphaS_dn});
       }
       if (hw.nnpdf30_unc.size()==100) {
	 fillHistos(h_pTH_nnpdf30,h.Pt(),hw.nnpdf30_unc);
	 fillHistos(h_yH_nnpdf30,h.Rapidity(),hw.nnpdf30_unc);
	 fillHistos(h_Njets_nnpdf30,HTXS_Njets30,hw.nnpdf30_unc);
	 fillHistos(h_STXS_nnpdf30,HTXS_index,hw.nnpdf30_unc);
       }

       ////////////////
       // 3.  QCD uncertainties
       //
       // 3.a Default Powheg scale variations.
       //     Note: For ggF, these are the pure Powheg MiNLO prime uncertainties
       //           without considering the NNLO correction. I.e. don't use for NNLOPS.
       //           Don't treat all these as NPs!! Take envelope. Or perhaps carefully chose 1 or 2 of them as NPs.
       fillHistos(h_pTH_qcd,h.Pt(),hw.qcd);
       fillHistos(h_yH_qcd,h.Rapidity(),hw.qcd);
       fillHistos(h_Njets_qcd,HTXS_Njets30,hw.qcd);
       fillHistos(h_STXS_qcd,HTXS_index,hw.qcd);

       // QCD variations for (NNLOPS) ggF
       //
       // 3.b The 26 variations around the nominal from NNLOPS (3 scales x {d,n,u} => 3^3=27 points, one is nom). 
       // This should give an NNLO accurate normalization uncertainty (8-11%)
       //     Note: The NNLOPS paper takes the envelope of all these variations 
       fillHistos(h_pTH_nnlops_qcd,h.Pt(),hw.qcd_nnlops);
       fillHistos(h_yH_nnlops_qcd,h.Rapidity(),hw.qcd_nnlops);
       fillHistos(h_Njets_nnlops_qcd,HTXS_Njets30,hw.qcd_nnlops);
       fillHistos(h_STXS_nnlops_qcd,HTXS_index,hw.qcd_nnlops);

       // 3.c Selecting two of the 26 as NPs (i.e. uncertainty sources to be treated as uncorrelated)
       //     Taking the NNLO uncertainty (nnloDn-PowNomNom) as one source and the extreme Powheg varation 
       //     (nnloNom-PowDnDn) as an uncorrelation source (similar to >=1 in ST: affects high pT)
       NumV nnlops_2np_qcd={hw.qcd_nnlops_nnlo,hw.qcd_nnlops_pow};
       fillHistos(h_pTH_nnlops_qcd2,h.Pt(),nnlops_2np_qcd);
       fillHistos(h_yH_nnlops_qcd2,h.Rapidity(),nnlops_2np_qcd);
       fillHistos(h_Njets_nnlops_qcd2,HTXS_Njets30,nnlops_2np_qcd);
       fillHistos(h_STXS_nnlops_qcd2,HTXS_index,nnlops_2np_qcd);

       // 3.d WG1 propsed uncertainty scheme. Recommended.
       //NumV wg1_qcd={hw.qcd_wg1_mu,hw.qcd_wg1_res,hw.qcd_wg1_mig01,hw.qcd_wg1_mig12,hw.qcd_wg1_pTH,hw.qcd_wg1_qm};
       NumV wg1_qcd=hw.ggF_qcd_wg1();
       fillHistos(h_pTH_wg1qcd,h.Pt(),wg1_qcd);
       fillHistos(h_yH_wg1qcd,h.Rapidity(),wg1_qcd);
       fillHistos(h_Njets_wg1qcd,HTXS_Njets30,wg1_qcd);
       fillHistos(h_STXS_wg1qcd,HTXS_index,wg1_qcd);

       fillHistos(h_pTH_qcd_stxs,h.Pt(),hw.ggF_qcd_stxs);

       // 4. Other weights
       fillHistos(h_yH_other,h.Rapidity(),
		  {hw.weight0,hw.nnpdf30_nnlo,hw.pdf4lhc_nnlo,hw.nnpdf30_nlo,hw.pdf4lhc_nlo,
		      hw.mmht2014nlo,hw.ct14nlo,hw.ct10nlo});

       yH=h.Rapidity(); pTH=h.Pt(); wnom=hw.nominal; wnlo=hw.weight0; nj=HTXS_Njets30; fineSTXS=HTXS_index;
       tree->Fill();

       // Print stuff to the screen for the first event in each file
       if ( entry == 0 ) {
	 hw.print();
       }
       
       // Give some feedback of where we are:
       if( (entry+1) % 10000 == 0 ) ::Info( APP_NAME, "Processed %5llu/%5llu events",entry+1,entries);
       
     } // for each entry
   } // for each file

   higgsMCtool->finalize().ignore();

   of->cd();
   of->Write();
   printf("\nProduced %s\n\n",of->GetName());
   of->Close();
   
   // Return gracefully:
   return 0;
}

