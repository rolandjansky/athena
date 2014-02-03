//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Jan 2006:
// Code to distinguish between real and fake muons
/////////////////////////////////////////////////////////////

Int_t MooHists::MooToKin_5var(Int_t irecm, Float_t chi2min, Float_t* chi2tk)
{
  //
  // for a given reconstructed Mo track find its matching generated
  // track require a chi-square test using the generated and reconstructed 5 track variables

  Int_t theTrack=-1;
  Float_t chi2Cut = 1000;      // this needs to be tuned - may end up being pT dependent

  for (Int_t ik=0; ik<Moore_nkine; ++ik) {
    //
    //    Float_t pTMo = 1./(*Moore_ptig)[ik]/1000.; // generated momentum

    Float_t pTdiff = ((*Moore_ptir)[irecm]-(*Moore_ptiextrg)[ik])/sqrt((*Moore_covr55)[irecm]);
    Float_t a0diff = ((*Moore_a0r)[irecm]-(*Moore_a0extrg)[ik])/sqrt((*Moore_covr11)[irecm]);
    Float_t z0diff = ((*Moore_z0r)[irecm]-(*Moore_z0extrg)[ik])/sqrt((*Moore_covr22)[irecm]);
    Float_t phidiff = ((*Moore_phir)[irecm]-(*Moore_phiextrg)[ik])/sqrt((*Moore_covr33)[irecm]);
    Float_t cthdiff = ((*Moore_cotthr)[irecm]-(*Moore_cthextrg)[ik])/sqrt((*Moore_covr44)[irecm]);
    //
    // calculate chi^2
    chi2tk[irecm] = pTdiff*pTdiff + a0diff*a0diff + z0diff*z0diff + phidiff*phidiff + cthdiff*cthdiff;
    //
    chi2_5var[0]->Fill(fabs((*Moore_etar)[irecm]), chi2tk[irecm]);
    chi2_5var1[0]->Fill(chi2tk[irecm]);
    chi2_pT[0]->Fill((*Moore_etar)[irecm],pTdiff*pTdiff);
    chi2_a0[0]->Fill((*Moore_etar)[irecm],a0diff*a0diff);
    chi2_z0[0]->Fill((*Moore_etar)[irecm],z0diff*z0diff);
    chi2_phi[0]->Fill((*Moore_etar)[irecm],phidiff*phidiff);
    chi2_cth[0]->Fill((*Moore_etar)[irecm],cthdiff*cthdiff);
    // if(nmuonmo>1) printf("ik/5 var diff %i %f %f %f %f %f %f\n", ik,pTdiff,a0diff,z0diff,phidiff,cthdiff,chi2tk[irecm]);
    //
    if(chi2tk[irecm] < chi2min && chi2tk[irecm]<chi2Cut) { // chi2tk has to be a sensible value (chi2Cut)
       theTrack = ik;
       chi2min = chi2tk[irecm];
    }

  }
  if(theTrack>=0) return theTrack;
  //
  // if we are here, it means we didn't find anything
  //
  return -1;
 }
