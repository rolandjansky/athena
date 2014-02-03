//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Jan 2006:
// Code to distinguish between real and fake muons
/////////////////////////////////////////////////////////////

Int_t MooHists::MooToKin_rassoc(Int_t irec) 
{
  //
  // for a given reconstructed Mo track find its matching generated track
  //
  for (Int_t ik=0; ik<Moore_nkine; ++ik) {
    //
    if(irec==(*Moore_rassociated)[ik]) return ik;
  }
  //
  // if we are here, it means we didn't find anything
  //
  return -1;

 }
