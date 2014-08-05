/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define PAUisEMPerf_cxx
#include "PAUisEMPerf.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>

#include "PhotonIDTool.h"

void PAUisEMPerf::Loop()
{
   if (fChain == 0) return;

   fChain->SetBranchStatus("*",0);  // disable all branches
   // truth
   fChain->SetBranchStatus("truth_ph_eta",1);
   fChain->SetBranchStatus("truth_ph_phi",1);
   fChain->SetBranchStatus("truth_ph_pt",1);
   fChain->SetBranchStatus("truth_ph_E",1);
   fChain->SetBranchStatus("truth_ph_isConv",1);
   fChain->SetBranchStatus("truth_ph_isBrem",1);
   fChain->SetBranchStatus("truth_ph_isFromHardProc",1);
   fChain->SetBranchStatus("truth_ph_iRecPhoton",1);
   // reconstructed
   fChain->SetBranchStatus("ph_pt",1);
   fChain->SetBranchStatus("ph_etaS2",1);
   fChain->SetBranchStatus("ph_E_clus",1);
   fChain->SetBranchStatus("ph_PDG",1);
   fChain->SetBranchStatus("ph_isBrem_MC",1);
   fChain->SetBranchStatus("ph_isPhotonFromHardProc_MC",1);
   fChain->SetBranchStatus("ph_isConv",1);
   fChain->SetBranchStatus("ph_shwr_EtHad1",1);
   fChain->SetBranchStatus("ph_shwr_EtHad",1);
   fChain->SetBranchStatus("ph_shwr_E277",1);
   fChain->SetBranchStatus("ph_shwr_E237",1);
   fChain->SetBranchStatus("ph_shwr_E233",1);
   fChain->SetBranchStatus("ph_shwr_weta2",1);
   fChain->SetBranchStatus("ph_shwr_f1",1);
   fChain->SetBranchStatus("ph_shwr_Emax1",1);
   fChain->SetBranchStatus("ph_shwr_Emax2",1);
   fChain->SetBranchStatus("ph_shwr_Emin",1);
   fChain->SetBranchStatus("ph_shwr_fracm",1);
   fChain->SetBranchStatus("ph_shwr_wtot",1);
   fChain->SetBranchStatus("ph_shwr_w1",1);

   Long64_t nentries = fChain->GetEntriesFast();
   //nentries = 100000;
   
   Long64_t nbytes = 0, nb = 0;

   //TString histfilename = "PAUisEMPerf_Signal_isConvReco.root";
   //TString histfilename = "PAUisEMPerf_Signal_isConvTrue.root";
   TString histfilename = "PAUisEMPerf_Signal_isConvTrue_AccTrue.root";

   int n_pt_bin   = 40;
   double pt_min  = 0.;
   double pt_max  = 100.;
   TString pt_xlabel = "E_{T} [GeV]";
   TString pt_ylabel = "Entries / 2.5 GeV";

   int n_eta_bin   = 25;
   double eta_min  = 0.;
   double eta_max  = 2.5;
   TString eta_xlabel = "|#eta|";
   TString eta_ylabel = "Entries / 0.1";

   TH1F* h_spectrum_pt_all_true = new TH1F("h_spectrum_pt_all_true","",n_pt_bin,pt_min,pt_max);
   h_spectrum_pt_all_true->Sumw2();
   h_spectrum_pt_all_true->GetXaxis()->SetTitle(pt_xlabel);
   h_spectrum_pt_all_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_spectrum_pt_unconv_true = new TH1F("h_spectrum_pt_unconv_true","",n_pt_bin,pt_min,pt_max);
   h_spectrum_pt_unconv_true->Sumw2();
   h_spectrum_pt_unconv_true->GetXaxis()->SetTitle(pt_xlabel);
   h_spectrum_pt_unconv_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_spectrum_pt_conv_true = new TH1F("h_spectrum_pt_conv_true","",n_pt_bin,pt_min,pt_max);
   h_spectrum_pt_conv_true->Sumw2();
   h_spectrum_pt_conv_true->GetXaxis()->SetTitle(pt_xlabel);
   h_spectrum_pt_conv_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_spectrum_pt_all_reco = new TH1F("h_spectrum_pt_all_reco","",n_pt_bin,pt_min,pt_max);
   h_spectrum_pt_all_reco->Sumw2();
   h_spectrum_pt_all_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_spectrum_pt_all_reco->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_spectrum_pt_unconv_reco = new TH1F("h_spectrum_pt_unconv_reco","",n_pt_bin,pt_min,pt_max);
   h_spectrum_pt_unconv_reco->Sumw2();
   h_spectrum_pt_unconv_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_spectrum_pt_unconv_reco->GetYaxis()->SetTitle(pt_ylabel);
   
   TH1F* h_spectrum_pt_conv_reco = new TH1F("h_spectrum_pt_conv_reco","",n_pt_bin,pt_min,pt_max);
   h_spectrum_pt_conv_reco->Sumw2();
   h_spectrum_pt_conv_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_spectrum_pt_conv_reco->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_passloose_pt_all = new TH1F("h_passloose_pt_all","",n_pt_bin,pt_min,pt_max);
   h_passloose_pt_all->Sumw2();
   h_passloose_pt_all->GetXaxis()->SetTitle(pt_xlabel);
   h_passloose_pt_all->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_passloose_pt_unconv = new TH1F("h_passloose_pt_unconv","",n_pt_bin,pt_min,pt_max);
   h_passloose_pt_unconv->Sumw2();
   h_passloose_pt_unconv->GetXaxis()->SetTitle(pt_xlabel);
   h_passloose_pt_unconv->GetYaxis()->SetTitle(pt_ylabel);
   
   TH1F* h_passloose_pt_conv = new TH1F("h_passloose_pt_conv","",n_pt_bin,pt_min,pt_max);
   h_passloose_pt_conv->Sumw2();
   h_passloose_pt_conv->GetXaxis()->SetTitle(pt_xlabel);
   h_passloose_pt_conv->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_passtight_pt_all = new TH1F("h_passtight_pt_all","",n_pt_bin,pt_min,pt_max);
   h_passtight_pt_all->Sumw2();
   h_passtight_pt_all->GetXaxis()->SetTitle(pt_xlabel);
   h_passtight_pt_all->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_passtight_pt_unconv = new TH1F("h_passtight_pt_unconv","",n_pt_bin,pt_min,pt_max);
   h_passtight_pt_unconv->Sumw2();
   h_passtight_pt_unconv->GetXaxis()->SetTitle(pt_xlabel);
   h_passtight_pt_unconv->GetYaxis()->SetTitle(pt_ylabel);
   
   TH1F* h_passtight_pt_conv = new TH1F("h_passtight_pt_conv","",n_pt_bin,pt_min,pt_max);
   h_passtight_pt_conv->Sumw2();
   h_passtight_pt_conv->GetXaxis()->SetTitle(pt_xlabel);
   h_passtight_pt_conv->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_spectrum_eta_all_true = new TH1F("h_spectrum_eta_all_true","",n_eta_bin,eta_min,eta_max);
   h_spectrum_eta_all_true->Sumw2();
   h_spectrum_eta_all_true->GetXaxis()->SetTitle(eta_xlabel);
   h_spectrum_eta_all_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_spectrum_eta_unconv_true = new TH1F("h_spectrum_eta_unconv_true","",n_eta_bin,eta_min,eta_max);
   h_spectrum_eta_unconv_true->Sumw2();
   h_spectrum_eta_unconv_true->GetXaxis()->SetTitle(eta_xlabel);
   h_spectrum_eta_unconv_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_spectrum_eta_conv_true = new TH1F("h_spectrum_eta_conv_true","",n_eta_bin,eta_min,eta_max);
   h_spectrum_eta_conv_true->Sumw2();
   h_spectrum_eta_conv_true->GetXaxis()->SetTitle(eta_xlabel);
   h_spectrum_eta_conv_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_spectrum_eta_all_reco = new TH1F("h_spectrum_eta_all_reco","",n_eta_bin,eta_min,eta_max);
   h_spectrum_eta_all_reco->Sumw2();
   h_spectrum_eta_all_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_spectrum_eta_all_reco->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_spectrum_eta_unconv_reco = new TH1F("h_spectrum_eta_unconv_reco","",n_eta_bin,eta_min,eta_max);
   h_spectrum_eta_unconv_reco->Sumw2();
   h_spectrum_eta_unconv_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_spectrum_eta_unconv_reco->GetYaxis()->SetTitle(eta_ylabel);
   
   TH1F* h_spectrum_eta_conv_reco = new TH1F("h_spectrum_eta_conv_reco","",n_eta_bin,eta_min,eta_max);
   h_spectrum_eta_conv_reco->Sumw2();
   h_spectrum_eta_conv_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_spectrum_eta_conv_reco->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_passloose_eta_all = new TH1F("h_passloose_eta_all","",n_eta_bin,eta_min,eta_max);
   h_passloose_eta_all->Sumw2();
   h_passloose_eta_all->GetXaxis()->SetTitle(eta_xlabel);
   h_passloose_eta_all->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_passloose_eta_unconv = new TH1F("h_passloose_eta_unconv","",n_eta_bin,eta_min,eta_max);
   h_passloose_eta_unconv->Sumw2();
   h_passloose_eta_unconv->GetXaxis()->SetTitle(eta_xlabel);
   h_passloose_eta_unconv->GetYaxis()->SetTitle(eta_ylabel);
   
   TH1F* h_passloose_eta_conv = new TH1F("h_passloose_eta_conv","",n_eta_bin,eta_min,eta_max);
   h_passloose_eta_conv->Sumw2();
   h_passloose_eta_conv->GetXaxis()->SetTitle(eta_xlabel);
   h_passloose_eta_conv->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_passtight_eta_all = new TH1F("h_passtight_eta_all","",n_eta_bin,eta_min,eta_max);
   h_passtight_eta_all->Sumw2();
   h_passtight_eta_all->GetXaxis()->SetTitle(eta_xlabel);
   h_passtight_eta_all->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_passtight_eta_unconv = new TH1F("h_passtight_eta_unconv","",n_eta_bin,eta_min,eta_max);
   h_passtight_eta_unconv->Sumw2();
   h_passtight_eta_unconv->GetXaxis()->SetTitle(eta_xlabel);
   h_passtight_eta_unconv->GetYaxis()->SetTitle(eta_ylabel);
   
   TH1F* h_passtight_eta_conv = new TH1F("h_passtight_eta_conv","",n_eta_bin,eta_min,eta_max);
   h_passtight_eta_conv->Sumw2();
   h_passtight_eta_conv->GetXaxis()->SetTitle(eta_xlabel);
   h_passtight_eta_conv->GetYaxis()->SetTitle(eta_ylabel);

   int nphotonstrue = 0;
   int nunconvtrue  = 0;
   int nconvtrue    = 0;

   int nphotons     = 0;
   int nconv        = 0;
   int nunconv      = 0;

   int nloose       = 0;
   int nlooseconv   = 0;
   int nlooseunconv = 0;

   int ntight       = 0;
   int ntightconv   = 0;
   int ntightunconv = 0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      
      if ( jentry%100000==0 && jentry!=0 ) std::cout << "--- " << jentry <<  " events processed..." << std::endl; 

      for (unsigned int iph=0; iph<truth_ph_iRecPhoton->size(); iph++) { // loop on truth photons
	
	// keep only signal photons (prompt)
        if ((*truth_ph_isFromHardProc)[iph]!=1) continue;

	// get reconstructed object index
	int i = (*truth_ph_iRecPhoton)[iph]; 

	// true photons: 
        // - keep only photons in acceptance, using true eta
	// - keep only photons above threshold, using true pt
	double etatrue = fabs((*truth_ph_eta)[iph]);
	bool etaCut_true = ( etatrue<1.37 || ( 1.52<etatrue && etatrue<2.37 ) ); 
	bool ptCut_true  = ( (*truth_ph_pt)[iph]>20000. );
	bool isConv_true = (*truth_ph_isConv)[iph];
	
	// count true photons
	if ( etaCut_true ) {
	  h_spectrum_pt_all_true->Fill( (*truth_ph_pt)[iph]/1000. ); // fill pt spectrum even under the threshold
	  if (isConv_true) {
	    h_spectrum_pt_conv_true->Fill( (*truth_ph_pt)[iph]/1000. ); 
	  } else {
	    h_spectrum_pt_unconv_true->Fill( (*truth_ph_pt)[iph]/1000. ); 
	  }
	  if ( ptCut_true ) {
	    nphotonstrue++;
	    h_spectrum_eta_all_true->Fill( etatrue ); // fill ete spectrum only for object above the threshold
	    if (isConv_true) {
	      nconvtrue++;
	      h_spectrum_eta_conv_true->Fill( etatrue ); 
	    } else {
	      nunconvtrue++;
	      h_spectrum_eta_unconv_true->Fill( etatrue ); 
	    }
	  }
	}

	// keep only reconstructed photons
	if (i<0) continue;

	// retrieve reconstructed variables
        double eta2 = fabs( (*ph_etaS2)[i] );
        double e    = (*ph_E_clus)[i];
        double et   = cosh(eta2)!=0. ? e/cosh(eta2) : 0.;

	// cache shower shape values
        double rhad    = (*ph_shwr_EtHad)[i]/et;
        double rhad1   = (*ph_shwr_EtHad1)[i]/et;
	double e277    = (*ph_shwr_E277)[i];
	double e237    = (*ph_shwr_E237)[i];
	double e233    = (*ph_shwr_E233)[i];
        double reta    = e277 != 0. ? e237/e277 : 0.;
        double rphi    = e237 != 0. ? e233/e237 : 0.;
        double weta2   = (*ph_shwr_weta2)[i];
        double w1      = (*ph_shwr_w1)[i];
        double wtot    = (*ph_shwr_wtot)[i];
        double fside   = (*ph_shwr_fracm)[i];
	double emax2   = (*ph_shwr_Emax2)[i];
	double emax1   = (*ph_shwr_Emax1)[i];
	double emin2   = (*ph_shwr_Emin)[i];
        double deltae  = emax2-emin2;
        double eratio  = (emax1+emax2)==0. ? 0 : (emax1-emax2)/(emax1+emax2);

	// compute isEM cuts using default constructor
	PhotonIDTool GammaSelection = PhotonIDTool( et, 
						    eta2,
						    (*ph_shwr_EtHad1)[i],
						    (*ph_shwr_EtHad)[i] ,
						    (*ph_shwr_E277)[i] ,
						    (*ph_shwr_E237)[i] ,
						    (*ph_shwr_E233)[i] ,
						    (*ph_shwr_weta2)[i] ,
						    (*ph_shwr_f1)[i] ,
						    (*ph_shwr_Emax1)[i] ,
						    (*ph_shwr_Emax2)[i] ,
						    (*ph_shwr_Emin)[i] ,
						    (*ph_shwr_fracm)[i] ,
						    (*ph_shwr_wtot)[i] ,
						    (*ph_shwr_w1)[i] ,
						    (*ph_isConv)[i] );
	
	//bool isLooseOld   = GammaSelection.PhotonCutsLooseElectron();
	bool isLoose      = GammaSelection.PhotonCutsLoose(1);
	//bool isLooseNew   = GammaSelection.PhotonCutsLoose(2);
	bool isTight      = GammaSelection.PhotonCutsTight(3);
	unsigned int isEM = GammaSelection.isEM(1,3);

	//if (isLoose != isLooseOld) printf("%d %d \n",isLooseOld,isLoose);
	//if (isLoose != isLooseNew) printf("%d %d %d\n",isLooseOld,isLoose,isLooseNew); // the SHOULD be different!

	// alternative constructor (useful to apply fudge factors)
	PhotonIDTool GammaSelectionAlt = PhotonIDTool( et, 
						       eta2,
						       rhad1,
						       rhad,
						       e277,
						       reta,
						       rphi,
						       weta2,
						       (*ph_shwr_f1)[i] ,
						       fside,
						       wtot,
						       w1,
						       deltae,
						       eratio,
						       (*ph_isConv)[i] );

	//bool isLooseOldAlt   = GammaSelectionAlt.PhotonCutsLooseElectron();
	bool isLooseAlt      = GammaSelectionAlt.PhotonCutsLoose(1);
	bool isTightAlt      = GammaSelectionAlt.PhotonCutsTight(3);
	unsigned int isEMAlt = GammaSelectionAlt.isEM(1,3);

	if (isLoose != isLooseAlt || isTight != isTightAlt)
	  printf("%d %d | %d %d | %d %d \n",isLoose,isLooseAlt,isTight,isTightAlt,isEM,isEMAlt);

	// reconstructed photons:
        // - keep only photons in acceptance, using reco eta
	// - keep only photons above threshold, using reco Et
	
	//bool etaCut_reco = ( eta2<1.37 || ( 1.52<eta2 && eta2<2.37 ) ); 
	//bool ptCut_reco  = ( et>20000. );	
	bool etaCut_reco = etaCut_true; 
	bool ptCut_reco  = ptCut_true;
	
	//bool isConv_reco = ( (*ph_isConv)[i] );
	bool isConv_reco = isConv_true;
	
	//double eta_fill = eta2;
	//double pt_fill  = et/1000.;
	double eta_fill = etatrue;
	double pt_fill  =  (*truth_ph_pt)[iph]/1000.;

	// count reco photons
	if ( etaCut_reco ) {

	  h_spectrum_pt_all_reco->Fill(pt_fill); // fill pt spectrum even under the threshold
	  if (isConv_reco) {
	    h_spectrum_pt_conv_reco->Fill(pt_fill); 
	  } else {
	    h_spectrum_pt_unconv_reco->Fill(pt_fill); 
	  }

	  if (isLoose) {
	    h_passloose_pt_all->Fill(pt_fill);
	    if (isConv_reco) {
	      h_passloose_pt_conv->Fill(pt_fill);
	    } else {
	      h_passloose_pt_unconv->Fill(pt_fill);
	    }
	  }

	  if (isTight) {
	    h_passtight_pt_all->Fill(pt_fill);
	    if (isConv_reco) {
	      h_passtight_pt_conv->Fill(pt_fill);
	    } else {
	      h_passtight_pt_unconv->Fill(pt_fill);
	    }
	  }

	  if ( ptCut_reco ) {

	    nphotons++;
	    h_spectrum_eta_all_reco->Fill(eta_fill); // fill ete spectrum only for object above the threshold
	    if (isConv_reco) {
	      nconv++;
	      h_spectrum_eta_conv_reco->Fill(eta_fill);
	    } else {
	      nunconv++;
	      h_spectrum_eta_unconv_reco->Fill(eta_fill);
	    }

	    if (isLoose) {
	      nloose++;
	      h_passloose_eta_all->Fill(eta_fill);
	      if (isConv_reco) {
		nlooseconv++;
		h_passloose_eta_conv->Fill(eta_fill);
	      } else {
		nlooseunconv++;
		h_passloose_eta_unconv->Fill(eta_fill);
	      }
	    }

	    if (isTight) {
	      ntight++;
	      h_passtight_eta_all->Fill(eta_fill);
	      if (isConv_reco) {
		ntightconv++;
		h_passtight_eta_conv->Fill(eta_fill);
	      } else {
		ntightunconv++;
		h_passtight_eta_unconv->Fill(eta_fill);
	      }
	    }
	  
	  }
	}
      }
   } 

   double eff, eeff;

   printf("\nN(true photons)   = %d\n",nphotonstrue);
   printf("N(true unconv)    = %d (%5.2f %%)\n",nunconvtrue,100.*(double)nunconvtrue/(double)nphotonstrue);
   printf("N(true conv)      = %d (%5.2f %%)\n\n",nconvtrue,100.*(double)nconvtrue/(double)nphotonstrue);

   printf("N(reco photons)   = %d\n",nphotons);
   printf("N(reco unconv)    = %d (%5.2f %%)\n",nunconv,100.*(double)nunconv/(double)nphotons);
   printf("N(reco conv)      = %d (%5.2f %%)\n\n",nconv,100.*(double)nconv/(double)nphotons);

   eff  = (double) nphotons / (double) nphotonstrue;
   eeff = sqrt(eff*(1-eff)/nphotonstrue);
   printf("E(reco all)       = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) nunconv / (double) nunconvtrue;
   eeff = sqrt(eff*(1-eff)/nunconvtrue);
   printf("E(reco unconv)    = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) nconv / (double) nconvtrue;
   eeff = sqrt(eff*(1-eff)/nconvtrue);
   printf("E(reco conv)      = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);

   printf("\nALL PHOTONS\n\n");

   printf("N(loose photons)  = %d\n",nloose);
   eff  = (double) nloose / (double) nphotons;
   eeff = sqrt(eff*(1-eff)/nphotons);
   printf("E(loose vs reco)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) nloose / (double) nphotonstrue;
   eeff = sqrt(eff*(1-eff)/nphotonstrue);
   printf("E(loose vs true)  = %5.2f +/- %3.2f \n\n",100.*eff,100.*eeff);

   printf("N(tight photons)  = %d\n",ntight);
   eff  = (double) ntight / (double) nphotons;
   eeff = sqrt(eff*(1-eff)/nphotons);
   printf("E(tight vs reco)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) ntight / (double) nphotonstrue;
   eeff = sqrt(eff*(1-eff)/nphotonstrue);
   printf("E(tight vs true)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);

   printf("\nUNCONVERTED PHOTONS\n\n");

   printf("N(loose unconv)   = %d\n",nlooseunconv);
   eff  = (double) nlooseunconv / (double) nunconv;
   eeff = sqrt(eff*(1-eff)/nunconv);
   printf("E(loose vs reco)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) nlooseunconv / (double) nunconvtrue;
   eeff = sqrt(eff*(1-eff)/nunconvtrue);
   printf("E(loose vs true)  = %5.2f +/- %3.2f \n\n",100.*eff,100.*eeff);

   printf("N(tight unconv)   = %d\n",ntightunconv);
   eff  = (double) ntightunconv / (double) nunconv;
   eeff = sqrt(eff*(1-eff)/nunconv);
   printf("E(tight vs reco)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) ntightunconv / (double) nunconvtrue;
   eeff = sqrt(eff*(1-eff)/nunconvtrue);
   printf("E(tight vs true)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);

   printf("\nCONVERTED PHOTONS\n\n");

   printf("N(loose conv)     = %d\n",nlooseconv);
   eff  = (double) nlooseconv / (double) nconv;
   eeff = sqrt(eff*(1-eff)/nconv);
   printf("E(loose vs reco)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) nlooseconv / (double) nconvtrue;
   eeff = sqrt(eff*(1-eff)/nconvtrue);
   printf("E(loose vs true)  = %5.2f +/- %3.2f \n\n",100.*eff,100.*eeff);

   printf("N(tight conv)     = %d\n",ntightconv);
   eff  = (double) ntightconv / (double) nconv;
   eeff = sqrt(eff*(1-eff)/nconv);
   printf("E(tight vs reco)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);
   eff  = (double) ntightconv / (double) nconvtrue;
   eeff = sqrt(eff*(1-eff)/nconvtrue);
   printf("E(tight vs true)  = %5.2f +/- %3.2f \n",100.*eff,100.*eeff);

   pt_ylabel  = "Photon efficiency";
   eta_ylabel = "Photon efficiency";

   TH1F* h_eff_loose_pt_all_true = new TH1F("h_eff_loose_pt_all_true","",n_pt_bin,pt_min,pt_max);
   h_eff_loose_pt_all_true->Sumw2();
   h_eff_loose_pt_all_true->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_loose_pt_all_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_loose_pt_unconv_true = new TH1F("h_eff_loose_pt_unconv_true","",n_pt_bin,pt_min,pt_max);
   h_eff_loose_pt_unconv_true->Sumw2();
   h_eff_loose_pt_unconv_true->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_loose_pt_unconv_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_loose_pt_conv_true = new TH1F("h_eff_loose_pt_conv_true","",n_pt_bin,pt_min,pt_max);
   h_eff_loose_pt_conv_true->Sumw2();
   h_eff_loose_pt_conv_true->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_loose_pt_conv_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_loose_pt_all_reco = new TH1F("h_eff_loose_pt_all_reco","",n_pt_bin,pt_min,pt_max);
   h_eff_loose_pt_all_reco->Sumw2();
   h_eff_loose_pt_all_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_loose_pt_all_reco->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_loose_pt_unconv_reco = new TH1F("h_eff_loose_pt_unconv_reco","",n_pt_bin,pt_min,pt_max);
   h_eff_loose_pt_unconv_reco->Sumw2();
   h_eff_loose_pt_unconv_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_loose_pt_unconv_reco->GetYaxis()->SetTitle(pt_ylabel);
   
   TH1F* h_eff_loose_pt_conv_reco = new TH1F("h_eff_loose_pt_conv_reco","",n_pt_bin,pt_min,pt_max);
   h_eff_loose_pt_conv_reco->Sumw2();
   h_eff_loose_pt_conv_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_loose_pt_conv_reco->GetYaxis()->SetTitle(pt_ylabel);

   h_eff_loose_pt_all_true->Divide(h_passloose_pt_all,h_spectrum_pt_all_true,1,1,"B");
   h_eff_loose_pt_unconv_true->Divide(h_passloose_pt_unconv,h_spectrum_pt_unconv_true,1,1,"B");
   h_eff_loose_pt_conv_true->Divide(h_passloose_pt_conv,h_spectrum_pt_conv_true,1,1,"B");

   h_eff_loose_pt_all_reco->Divide(h_passloose_pt_all,h_spectrum_pt_all_reco,1,1,"B");
   h_eff_loose_pt_unconv_reco->Divide(h_passloose_pt_unconv,h_spectrum_pt_unconv_reco,1,1,"B");
   h_eff_loose_pt_conv_reco->Divide(h_passloose_pt_conv,h_spectrum_pt_conv_reco,1,1,"B");

   TH1F* h_eff_tight_pt_all_true = new TH1F("h_eff_tight_pt_all_true","",n_pt_bin,pt_min,pt_max);
   h_eff_tight_pt_all_true->Sumw2();
   h_eff_tight_pt_all_true->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_tight_pt_all_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_tight_pt_unconv_true = new TH1F("h_eff_tight_pt_unconv_true","",n_pt_bin,pt_min,pt_max);
   h_eff_tight_pt_unconv_true->Sumw2();
   h_eff_tight_pt_unconv_true->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_tight_pt_unconv_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_tight_pt_conv_true = new TH1F("h_eff_tight_pt_conv_true","",n_pt_bin,pt_min,pt_max);
   h_eff_tight_pt_conv_true->Sumw2();
   h_eff_tight_pt_conv_true->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_tight_pt_conv_true->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_tight_pt_all_reco = new TH1F("h_eff_tight_pt_all_reco","",n_pt_bin,pt_min,pt_max);
   h_eff_tight_pt_all_reco->Sumw2();
   h_eff_tight_pt_all_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_tight_pt_all_reco->GetYaxis()->SetTitle(pt_ylabel);

   TH1F* h_eff_tight_pt_unconv_reco = new TH1F("h_eff_tight_pt_unconv_reco","",n_pt_bin,pt_min,pt_max);
   h_eff_tight_pt_unconv_reco->Sumw2();
   h_eff_tight_pt_unconv_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_tight_pt_unconv_reco->GetYaxis()->SetTitle(pt_ylabel);
   
   TH1F* h_eff_tight_pt_conv_reco = new TH1F("h_eff_tight_pt_conv_reco","",n_pt_bin,pt_min,pt_max);
   h_eff_tight_pt_conv_reco->Sumw2();
   h_eff_tight_pt_conv_reco->GetXaxis()->SetTitle(pt_xlabel);
   h_eff_tight_pt_conv_reco->GetYaxis()->SetTitle(pt_ylabel);

   h_eff_tight_pt_all_true->Divide(h_passtight_pt_all,h_spectrum_pt_all_true,1,1,"B");
   h_eff_tight_pt_unconv_true->Divide(h_passtight_pt_unconv,h_spectrum_pt_unconv_true,1,1,"B");
   h_eff_tight_pt_conv_true->Divide(h_passtight_pt_conv,h_spectrum_pt_conv_true,1,1,"B");

   h_eff_tight_pt_all_reco->Divide(h_passtight_pt_all,h_spectrum_pt_all_reco,1,1,"B");
   h_eff_tight_pt_unconv_reco->Divide(h_passtight_pt_unconv,h_spectrum_pt_unconv_reco,1,1,"B");
   h_eff_tight_pt_conv_reco->Divide(h_passtight_pt_conv,h_spectrum_pt_conv_reco,1,1,"B");

   TH1F* h_eff_loose_eta_all_true = new TH1F("h_eff_loose_eta_all_true","",n_eta_bin,eta_min,eta_max);
   h_eff_loose_eta_all_true->Sumw2();
   h_eff_loose_eta_all_true->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_loose_eta_all_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_loose_eta_unconv_true = new TH1F("h_eff_loose_eta_unconv_true","",n_eta_bin,eta_min,eta_max);
   h_eff_loose_eta_unconv_true->Sumw2();
   h_eff_loose_eta_unconv_true->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_loose_eta_unconv_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_loose_eta_conv_true = new TH1F("h_eff_loose_eta_conv_true","",n_eta_bin,eta_min,eta_max);
   h_eff_loose_eta_conv_true->Sumw2();
   h_eff_loose_eta_conv_true->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_loose_eta_conv_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_loose_eta_all_reco = new TH1F("h_eff_loose_eta_all_reco","",n_eta_bin,eta_min,eta_max);
   h_eff_loose_eta_all_reco->Sumw2();
   h_eff_loose_eta_all_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_loose_eta_all_reco->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_loose_eta_unconv_reco = new TH1F("h_eff_loose_eta_unconv_reco","",n_eta_bin,eta_min,eta_max);
   h_eff_loose_eta_unconv_reco->Sumw2();
   h_eff_loose_eta_unconv_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_loose_eta_unconv_reco->GetYaxis()->SetTitle(eta_ylabel);
   
   TH1F* h_eff_loose_eta_conv_reco = new TH1F("h_eff_loose_eta_conv_reco","",n_eta_bin,eta_min,eta_max);
   h_eff_loose_eta_conv_reco->Sumw2();
   h_eff_loose_eta_conv_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_loose_eta_conv_reco->GetYaxis()->SetTitle(eta_ylabel);

   h_eff_loose_eta_all_true->Divide(h_passloose_eta_all,h_spectrum_eta_all_true,1,1,"B");
   h_eff_loose_eta_unconv_true->Divide(h_passloose_eta_unconv,h_spectrum_eta_unconv_true,1,1,"B");
   h_eff_loose_eta_conv_true->Divide(h_passloose_eta_conv,h_spectrum_eta_conv_true,1,1,"B");

   h_eff_loose_eta_all_reco->Divide(h_passloose_eta_all,h_spectrum_eta_all_reco,1,1,"B");
   h_eff_loose_eta_unconv_reco->Divide(h_passloose_eta_unconv,h_spectrum_eta_unconv_reco,1,1,"B");
   h_eff_loose_eta_conv_reco->Divide(h_passloose_eta_conv,h_spectrum_eta_conv_reco,1,1,"B");

   TH1F* h_eff_tight_eta_all_true = new TH1F("h_eff_tight_eta_all_true","",n_eta_bin,eta_min,eta_max);
   h_eff_tight_eta_all_true->Sumw2();
   h_eff_tight_eta_all_true->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_tight_eta_all_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_tight_eta_unconv_true = new TH1F("h_eff_tight_eta_unconv_true","",n_eta_bin,eta_min,eta_max);
   h_eff_tight_eta_unconv_true->Sumw2();
   h_eff_tight_eta_unconv_true->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_tight_eta_unconv_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_tight_eta_conv_true = new TH1F("h_eff_tight_eta_conv_true","",n_eta_bin,eta_min,eta_max);
   h_eff_tight_eta_conv_true->Sumw2();
   h_eff_tight_eta_conv_true->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_tight_eta_conv_true->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_tight_eta_all_reco = new TH1F("h_eff_tight_eta_all_reco","",n_eta_bin,eta_min,eta_max);
   h_eff_tight_eta_all_reco->Sumw2();
   h_eff_tight_eta_all_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_tight_eta_all_reco->GetYaxis()->SetTitle(eta_ylabel);

   TH1F* h_eff_tight_eta_unconv_reco = new TH1F("h_eff_tight_eta_unconv_reco","",n_eta_bin,eta_min,eta_max);
   h_eff_tight_eta_unconv_reco->Sumw2();
   h_eff_tight_eta_unconv_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_tight_eta_unconv_reco->GetYaxis()->SetTitle(eta_ylabel);
   
   TH1F* h_eff_tight_eta_conv_reco = new TH1F("h_eff_tight_eta_conv_reco","",n_eta_bin,eta_min,eta_max);
   h_eff_tight_eta_conv_reco->Sumw2();
   h_eff_tight_eta_conv_reco->GetXaxis()->SetTitle(eta_xlabel);
   h_eff_tight_eta_conv_reco->GetYaxis()->SetTitle(eta_ylabel);

   h_eff_tight_eta_all_true->Divide(h_passtight_eta_all,h_spectrum_eta_all_true,1,1,"B");
   h_eff_tight_eta_unconv_true->Divide(h_passtight_eta_unconv,h_spectrum_eta_unconv_true,1,1,"B");
   h_eff_tight_eta_conv_true->Divide(h_passtight_eta_conv,h_spectrum_eta_conv_true,1,1,"B");

   h_eff_tight_eta_all_reco->Divide(h_passtight_eta_all,h_spectrum_eta_all_reco,1,1,"B");
   h_eff_tight_eta_unconv_reco->Divide(h_passtight_eta_unconv,h_spectrum_eta_unconv_reco,1,1,"B");
   h_eff_tight_eta_conv_reco->Divide(h_passtight_eta_conv,h_spectrum_eta_conv_reco,1,1,"B");

   TFile histfile(histfilename,"RECREATE");
   histfile.cd();
   
   h_spectrum_pt_all_true->Write();
   h_spectrum_pt_unconv_true->Write();
   h_spectrum_pt_conv_true->Write();
   h_spectrum_pt_all_reco->Write();
   h_spectrum_pt_unconv_reco->Write();
   h_spectrum_pt_conv_reco->Write();
   h_passloose_pt_all->Write();
   h_passloose_pt_unconv->Write();
   h_passloose_pt_conv->Write();
   h_passtight_pt_all->Write();
   h_passtight_pt_unconv->Write();
   h_passtight_pt_conv->Write();

   h_spectrum_eta_all_true->Write();
   h_spectrum_eta_unconv_true->Write();
   h_spectrum_eta_conv_true->Write();
   h_spectrum_eta_all_reco->Write();
   h_spectrum_eta_unconv_reco->Write();
   h_spectrum_eta_conv_reco->Write();
   h_passloose_eta_all->Write();
   h_passloose_eta_unconv->Write();
   h_passloose_eta_conv->Write();
   h_passtight_eta_all->Write();
   h_passtight_eta_unconv->Write();
   h_passtight_eta_conv->Write();

   h_eff_loose_pt_all_true->Write();
   h_eff_loose_pt_unconv_true->Write();
   h_eff_loose_pt_conv_true->Write();
   h_eff_loose_pt_all_reco->Write();
   h_eff_loose_pt_unconv_reco->Write();
   h_eff_loose_pt_conv_reco->Write();
   h_eff_tight_pt_all_true->Write();
   h_eff_tight_pt_unconv_true->Write();
   h_eff_tight_pt_conv_true->Write();
   h_eff_tight_pt_all_reco->Write();
   h_eff_tight_pt_unconv_reco->Write();
   h_eff_tight_pt_conv_reco->Write();

   h_eff_loose_eta_all_true->Write();
   h_eff_loose_eta_unconv_true->Write();
   h_eff_loose_eta_conv_true->Write();
   h_eff_loose_eta_all_reco->Write();
   h_eff_loose_eta_unconv_reco->Write();
   h_eff_loose_eta_conv_reco->Write();
   h_eff_tight_eta_all_true->Write();
   h_eff_tight_eta_unconv_true->Write();
   h_eff_tight_eta_conv_true->Write();
   h_eff_tight_eta_all_reco->Write();
   h_eff_tight_eta_unconv_reco->Write();
   h_eff_tight_eta_conv_reco->Write();

   histfile.Close();

}
