//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Mar 2006:
// Code to suppress fakes based on number of Mdt/Phi hits as a function of eta
/////////////////////////////////////////////////////////////

Int_t MooHists::MooCheck_MdtPhiHits(Int_t irec) {

  // look at number of Mdt and Phi hits and throw out the track if it below the eta dependent threshold
  // no pT dependence for now
  //

  Float_t etaG1 = fabs((*Moore_etar)[irec]);

  //
  if(etaG1 < 1.0 && (*Moore_mdth)[irec]< 10) return 0;
  if(etaG1 > 1.0 && etaG1 < 1.4 && (*Moore_mdth)[irec]< 10) return 0;
  if(etaG1 > 1.4 && etaG1 < 2.0 && (*Moore_mdth)[irec]< 15) return 0;
  if(etaG1 > 2.0 && (*Moore_mdth)[irec]< 10) return 0;

  //

  /*
  if(etaG1 < 0.8 && (*Moore_phih)[irec]<10) return 0;
  if(etaG1 > 0.8 && etaG1 < 1.05 && (*Moore_phih)[irec] <10) return 0;
  if(etaG1 > 1.05 && etaG1 < 1.85 && (*Moore_phih)[irec] <7) return 0;
  if(etaG1 > 1.85 && etaG1 < 2.45 && (*Moore_phih)[irec] <3) return 0;
  if(etaG1 > 2.45 && (*Moore_mdth)[irec]< 3) return 0;
  */

  return 1;

}
