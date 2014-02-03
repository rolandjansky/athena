//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, Victoria Rojo, SUNY Albany, July 2006:
// Code to make hit (etahmo) plots
//
// Here we make hit plots for different types of muons
/////////////////////////////////////////////////////////////

void  MooHists::MooMakeHitPlots(Int_t matchKineMo, Int_t NumRealMuons, Int_t irecm, Double_t phi, Double_t eta) {

  // The index i indicates the muon type:
  //
  // [0] Real muons 
  // [1] All fake Muons
  // [2] Misreconstructed real muons that were classified as fakes
  // [3] Fakes due to random hits

  Int_t i;

  if(matchKineMo >= 0){ //only real muons
    i = 0;
    goto MakeHistos;
  }

  if(matchKineMo < 0 ){ //all fake muons
    i = 1;
    goto MakeHistos;
  }

 MakeHistos: // Here we fill the histograms  
  {
    // number of hits vs Eta

    nMdtHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_mdth)[irecm]);
    nPhiHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_phih)[irecm]);
    nStationsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_stati)[irecm]);
    nPrecEtaHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_mdth)[irecm]+(*Moore_cscetah)[irecm]);
    nTrigEtaHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_rpcetah)[irecm]+(*Moore_tgcetah)[irecm]);
     
    nmMdtHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_mdth)[irecm]);
    nmPhiHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_phih)[irecm]);
    nmStationsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_stati)[irecm]);
    nmEtaHitsVsEta_[i]->Fill(fabs(eta),(Float_t) (*Moore_etah)[irecm]);
    
    nMdtHitsEtaPhi_[i]->Fill(eta,phi,(Float_t) (*Moore_mdth)[irecm]);
    nPhiHitsEtaPhi_[i]->Fill(eta,phi,(Float_t) (*Moore_phih)[irecm]);
    nStationsEtaPhi_[i]->Fill(eta,phi,(Float_t) (*Moore_stati)[irecm]);
    nEtaHitsEtaPhi_[i]->Fill(eta,phi,(Float_t) (*Moore_etah)[irecm]);

    SectorVsPhi_[i]->Fill(phi,(MooPhiSector(phi)));

    //
    // look at hits vs. Phi in different eta regions
    //

    if(fabs(eta) < 1.0) { // in the barrel 
      nmMdtHitsVsPhi1_[i]->Fill(phi, (Float_t) (*Moore_mdth)[irecm]);
      nmPhiHitsVsPhi1_[i]->Fill(phi, (Float_t) (*Moore_phih)[irecm]);
      nmEtaHitsVsPhi1_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);
      
      // In the barrel there exists a gap at |eta| < 0.1 so we
      // want to look at the hits in this region seperately

      if(fabs(eta) < 0.1) { 
	nmEtaHitsVsPhi1a_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);
      }
    
      if(fabs(eta) >= 0.1) {
	nmEtaHitsVsPhi1b_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);
      }

      // Here we look at the number of hits in the  large
      // (non-overlapping) and small (non-overlapping) sectors, 
      // and overlapping regions of the barrel.
         
      if((MooPhiSector(phi)) == 1){
	nmEtaLargeSectorHitsVsEta_no_OL_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
      }
      if((MooPhiSector(phi)) == 2){
	nmEtaSmallSectorHitsVsEta_no_OL_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
      }
      if((MooPhiSector(phi)) == 3){
      nmEtaOverlapHitsVsEta_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]); 
      }
     
    }

    else if(fabs(eta) >=1.0 && fabs(eta)<1.95) { 
      
      // Look at hits in the endcap in the eta range
      // covered by the inner staion 
      
      nmMdtHitsVsPhi2_[i]->Fill(phi, (Float_t) (*Moore_mdth)[irecm]);
      nmPhiHitsVsPhi2_[i]->Fill(phi, (Float_t) (*Moore_phih)[irecm]);
      nmEtaHitsVsPhi2_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);

      if(fabs(eta) < 1.4) {
	
	// Region covered by the missing EEL/S 
	
	nmEtaHitsVsPhi2a_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);

	if((MooPhiSector(phi)) == 1){
	  nmEtaLargeSectorHitsVsEtaEC1_no_OL_[i]->Fill(fabs(eta), (Float_t) (*Moore_etah)[irecm]);
	}

	if((MooPhiSector(phi)) == 2){
	  nmEtaSmallSectorHitsVsEtaEC1_no_OL_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
	}
	 
	if((MooPhiSector(phi)) == 3){
	  nmEtaOverlapHitsVsEtaEC1_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
	}

      }

      if(fabs(eta) >= 1.4) {

	// Region not affected by the missing EEL/S 

	nmEtaHitsVsPhi1b_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);

	if((MooPhiSector(phi)) == 1){
	  nmEtaLargeSectorHitsVsEtaEC2_no_OL_[i]->Fill(fabs(eta), (Float_t) (*Moore_etah)[irecm]);
	}

	if((MooPhiSector(phi)) == 2){
	  nmEtaSmallSectorHitsVsEtaEC2_no_OL_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
	  // printf ("eta =  %f     Phi = %f \n",eta,phi);
	}
	 
	if((MooPhiSector(phi)) == 3){
	  nmEtaOverlapHitsVsEtaEC2_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
	}

      }
      
    }
    else if(fabs(eta) >=1.95 && fabs(eta)<2.4) {

      // eta region covered by the CSC's

      // printf ("eta =  %f     Phi = %f \n",eta,phi);
      
      nmMdtHitsVsPhi3_[i]->Fill(phi, (Float_t) (*Moore_mdth)[irecm]);
      nmPhiHitsVsPhi3_[i]->Fill(phi, (Float_t) (*Moore_phih)[irecm]);
      nmEtaHitsVsPhi3_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);

      if((MooPhiSector(phi)) == 1){
	nmEtaLargeSectorHitsVsEtaEC3_no_OL_[i]->Fill(fabs(eta), (Float_t) (*Moore_etah)[irecm]);
      }

      if((MooPhiSector(phi)) == 2){
	nmEtaSmallSectorHitsVsEtaEC3_no_OL_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
      }
      
      if((MooPhiSector(phi)) == 3){
	nmEtaOverlapHitsVsEtaEC3_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
      }

    }
    else if(fabs(eta) >=2.4) {

      // remaining region
      
      nmMdtHitsVsPhi4_[i]->Fill(phi, (Float_t) (*Moore_mdth)[irecm]);
      nmPhiHitsVsPhi4_[i]->Fill(phi, (Float_t) (*Moore_phih)[irecm]);
      nmEtaHitsVsPhi4_[i]->Fill(phi, (Float_t) (*Moore_etah)[irecm]);

      if((MooPhiSector(phi)) == 1){
	nmEtaLargeSectorHitsVsEtaEC4_no_OL_[i]->Fill(fabs(eta), (Float_t) (*Moore_etah)[irecm]);
      }
      if((MooPhiSector(phi)) == 2){
	nmEtaSmallSectorHitsVsEtaEC4_no_OL_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
      }      
      if((MooPhiSector(phi)) == 3){
	nmEtaOverlapHitsVsEtaEC4_[i]->Fill(eta, (Float_t) (*Moore_etah)[irecm]);
      }

    }
   
    // end of making plots

    if(matchKineMo < 0 && NumRealMuons ==0 && i!=0 && i!=2 && i!=3){ // fakes due to misreconstruced reals 
      i = 2;
      goto MakeHistos;
    }
    if(matchKineMo < 0 && NumRealMuons >= 1 && i!=0 && i!=2 && i!=3){ // fakes due to random hits
      i = 3;
      goto MakeHistos;
    }

  } //end of MakeHistos

}




