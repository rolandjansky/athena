//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Jan 2006:
// Code to distinguish between real and fake muons
/////////////////////////////////////////////////////////////

Int_t MooHists::MooToKin_pTvar(Int_t irecm, Float_t chi2min, Float_t* chi2tk)
{
  //
  // for a given reconstructed Mo track find its matching generated
  // track require a chi-square test using the generated and reconstructed 5 track variables

  Int_t theTrack=-1;
  Float_t chi2cut = 1000;      // this needs to be tuned - may end up being pT dependent

  for (Int_t ik=0; ik<Moore_nkine; ++ik) {
    //
    Float_t pTMo = 1./(*Moore_ptig)[ik]/1000.; // generated momentum in GeV

    Float_t pTdiff = ((*Moore_ptir)[irecm]-(*Moore_ptiextrg)[ik])/sqrt((*Moore_covr55)[irecm]);
    //

    // calculate chi^2
    chi2tk[irecm] = pTdiff*pTdiff;
    // printf("ik/5 var diff %i %f %f\n", ik,pTdiff,chi2tk[irecm]);
    //
    // make cuts based on track pT and eta - use generated values of pT and eta
    //
    // for pT<50, eta: 0-1.5 chi2<25, eta: 1.5-2.0 chi2<40 and eta: 2.0- chi2<25
    //       >50  eta: 0-1.5 chi2<15, eta: 1.5-2.0 chi2<20 and eta: 2.0- chi2<10
    //
    // May 13, 2006
    // Use a flat cut across eta. Chi2<10
    //
    if(fabs((*Moore_etag)[ik])<1.5) {
      if(pTMo<50) {chi2cut = 10;}
      else{chi2cut=10;}
      
    }
    else {
      if(fabs((*Moore_etag)[ik])<2.0) { // eta>1.5 and <2.0

        if(pTMo<50) {chi2cut = 10;}
        else{chi2cut=10;}
        
      } // eta>1.5 and <2.0

      else {// eta>2.0
        if(pTMo<50) {chi2cut = 10;}
        else{chi2cut=10;}

      } // eta>2.0

    } // goes with eta<1.5
 

    //    chi2cut = 1000; // for testing purposes

    if(chi2tk[irecm] < chi2min && chi2tk[irecm]<chi2cut) { // chi2tk has to be a sensible value (chi2cut)
       theTrack = ik;
       chi2min = chi2tk[irecm];
    }

  } // loop over nkinemo

  chi2_pT[0]->Fill((*Moore_etar)[irecm],chi2min);
  if(theTrack>=0) return theTrack;
  //
  // if we are here, it means we didn't find anything
  //
  return -1;

}
