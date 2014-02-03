//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Mar 2006:
// Code to suppress fakes based on number of Mdt/Phi hits as a function of eta
//
// Victoria Rojo, SUNY Albany, June-July 2006:
// Code to suppress fakes based on number of Eta/Phi hits as a function of eta
/////////////////////////////////////////////////////////////

Int_t MooHists::MooCheck_etahmo(Int_t irec) {

  // look at number of Eta and Phi hits and throw out the track if it below the eta dependent threshold
  // no pT dependence for now
  //

  Float_t etaG1 = fabs((*Moore_etar)[irec]);
  //Float_t etaG1 = etagmo[matchKineMo];
  // Float_t phiG1 = phigmo[matchKineMo];
  Float_t phiG1 = (*Moore_phir)[irec];


  if(etaG1 < 0.1 && (*Moore_etah)[irec]< 10) return 0;

  if(etaG1 >= 0.1 && etaG1 < 1){
    if((MooPhiSector(phiG1)) == 1){
      if((*Moore_etah)[irec]<12) return 0;
    }

    if((MooPhiSector(phiG1)) == 2){
      if((*Moore_etah)[irec]<12) return 0;
    }

    if((MooPhiSector(phiG1)) == 3){
      if((*Moore_etah)[irec]<13) return 0;
    }
  }
  
  if(etaG1 >= 1.0 && etaG1 < 1.4 && (*Moore_etah)[irec]< 10) return 0;
  if(etaG1 >= 1.4 && etaG1 < 1.95 && (*Moore_etah)[irec]< 14) return 0;
  if(etaG1 >= 1.95 && etaG1 < 2.4 && (*Moore_etah)[irec]< 10) return 0;
  if(etaG1 >= 2.4 && (*Moore_etah)[irec]< 10) return 0;



  return 1;

}
