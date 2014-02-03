//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Jan 2006:
// Code to distinguish between real and fake muons
/////////////////////////////////////////////////////////////

Int_t MooHists::MooToKin_etaPhi(Int_t irec)
{
  //
  // for a given reconstructed Mo track find its matching generated track
  // require that the matching track be within 50 mm
  //
  //

  Float_t dRmin = 999.;
  Int_t   iRmin = -1;

  for (Int_t ik=0; ik<Moore_nkine; ++ik) {

    Float_t deta = (*Moore_etar)[irec] - (*Moore_etag)[ik];    
    Float_t dphi = (*Moore_phir)[irec] - (*Moore_phig)[ik];    

    Float_t dR = sqrt(deta*deta + dphi*dphi);
    if (dR < dRmin) {
      dRmin = dR;
      iRmin = ik;
    }

  }

  // Check that the closest match is close enough (dR < 0.1)
  if (dRmin < 0.1) {
    return iRmin;
  }
  else {
    return -1;
  }
 }
