// $Id$
#include <TFile.h>
#include <TError.h>
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
  if (hw.size()!=ws.size()) fatal("Bad input to fillHistos");
  for (size_t i=0;i<hw.size();++i) hw[i]->Fill(x,ws[i]);
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
   TString ofn("higgsWeightTest_histo.root");
   for (int i=1;i<argc;++i) {
     TString arg(argv[i]);
     if      (arg=="--forceNNLOPS") forceNNLOPS=true;
     else if (arg=="--forceVBF"   ) forceVBF=true;
     else if (arg=="--forceVH"    ) forceVH=true;
     else if (arg=="--output") ofn=argv[++i];
     else if (arg.Contains(".root")) files.push_back(arg);
     else std::runtime_error(TString("Cannot intepret argument: "+arg).Data());
   }

   if (files.size()==0) {
     ::Error( APP_NAME, "Usage: %s <xAOD file1> [xAOD file2]...", APP_NAME );
     return 1;
   }

   // output file
   TFile *of = new TFile(ofn,"RECREATE");
   Str ptTit = ";#it{p}_{T} [GeV]";
   int Nbins=25; double min=0, max=250;

   TH1F *h_pTH = new TH1F("pTH",ptTit,Nbins,min,max);
   HistV h_pTH_pdf4lhc = makeHistos(30,"pTH_pdf4lhc",Nbins,min,max,ptTit);
   HistV h_pTH_aS = makeHistos({"pTH_aSup","pTH_aSdn"},Nbins,min,max,ptTit);

   Nbins=10; min=-0.5; max=9.5; Str tit="#it{N}_{jets}";
   TH1F *h_Njets = new TH1F("Njets30",tit,Nbins,min,max);
   HistV h_Njets_pdf4lhc = makeHistos(30,"Njets30_pdf4lhc",Nbins,min,max,tit);
   HistV h_Njets_aS = makeHistos({"Njets30_aSup","Njets30_aSdn"},Nbins,min,max,tit);

   Nbins=52; min=1; max=53; tit="STXS fine index";
   TH1F *h_STXS = new TH1F("STXS",tit,Nbins,min,max);
   HistV h_STXS_pdf4lhc = makeHistos(30,"STXS_pdf4lhc",Nbins,min,max,tit);
   HistV h_STXS_aS = makeHistos({"STXS_aSup","STXS_aSdn"},Nbins,min,max,tit);

   Nbins=60; min=-3; max=3; tit="#it{y_{H}}";
   TH1F *h_yH = new TH1F("yH",tit,Nbins,min,max);
   HistV h_yH_pdf4lhc = makeHistos(30,"yH_pdf4lhc",Nbins,min,max,tit);
   HistV h_yH_aS = makeHistos({"yH_aSup","yH_aSdn"},Nbins,min,max,tit);
   
   // Initialise the application:
   RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );
   
   // Create a TEvent object:
   xAOD::TEvent event( xAOD::TEvent::kBranchAccess );

   xAOD::HiggsWeightTool *higgsMCtool = new xAOD::HiggsWeightTool( "HiggsWeightTool" );
   higgsMCtool->setProperty( "OutputLevel", MSG::DEBUG ).ignore();
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
       std::vector<float> weights = evtInfo->mcEventWeights();

       const xAOD::TruthEventContainer *tevts;
       RETURN_CHECK( APP_NAME, event.retrieve( tevts, "TruthEvents" ) );

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
       bool doPDF = hw.pdf4lhc.size()==30;

       h_pTH  -> Fill(h.Pt(),hw.nominal);
       h_Njets-> Fill(HTXS_Njets30,hw.nominal);
       h_yH   -> Fill(h.Rapidity(),hw.nominal);
       h_STXS -> Fill(HTXS_index,hw.nominal);

       if (doPDF) {
	 fillHistos(h_pTH_pdf4lhc,h.Pt(),hw.pdf4lhc);
	 fillHistos(h_pTH_aS,h.Pt(),{hw.alphaS_up,hw.alphaS_dn});
	 fillHistos(h_yH_pdf4lhc,h.Rapidity(),hw.pdf4lhc);
	 fillHistos(h_yH_aS,h.Rapidity(),{hw.alphaS_up,hw.alphaS_dn});
	 fillHistos(h_Njets_pdf4lhc,HTXS_Njets30,hw.pdf4lhc);
	 fillHistos(h_Njets_aS,HTXS_Njets30,{hw.alphaS_up,hw.alphaS_dn});
	 fillHistos(h_STXS_pdf4lhc,HTXS_index,hw.pdf4lhc);
	 fillHistos(h_STXS_aS,HTXS_index,{hw.alphaS_up,hw.alphaS_dn});
       }

       // Print stuff to the screen for the first event in each file
       if ( entry == 0 ) {
	 ::Info(APP_NAME,"There are %lu weights in EventInfo and %lu in TruthEvents",
		weights.size(),tevts->at(0)->weights().size());
	 std::vector<float> ws = tevts->at(0)->weights();
	 for (size_t i=0;i<10;++i)
	   ::Info(APP_NAME,"Weight %lu %.3f and %.3f. %lu weights and %lu names",
		  i,weights[i],ws[i],ws.size(),higgsMCtool->getWeightNames().size());
	 hw.print();
       }
       
       // Give some feedback of where we are:
       if( (entry+1) % 10000 == 0 ) ::Info( APP_NAME, "Processed %5llu/%5llu events",entry+1,entries);
       
     } // for each entry
   } // for each file

   of->cd();
   of->Write();
   printf("\nProduced %s\n\n",of->GetName());
   of->Close();
   
   // Return gracefully:
   return 0;
}

