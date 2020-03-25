/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
// Moore Validation:
//
// Vivek Jain, SUNY Albany, Jan 2006:
// Code to distinguish between real and fake muons
/////////////////////////////////////////////////////////////

Int_t MooHists::MooToKin_rassoc_dist(Int_t irec) 
{
  //
  // for a given reconstructed Mo track find its matching generated
  // track require that the matching track be
  // within 100 mm for pT>50 GeV
  // within 400 mm for pT<50 GeV
  //
  for (Int_t ik=0; ik<Moore_nkine; ++ik) {
    Float_t pTMo = std::sin(*Moore_thetag)[ik]/((*Moore_qpig)[ik])/1000.;

    Int_t iAssoc = (*Moore_rassociated)[ik];

    if(irec==iAssoc && std::abs(pTMo) > 50 && ((*Moore_matchdg)[ik]) < 100) return ik;
    if(irec==iAssoc && std::abs(pTMo) <= 50 && ((*Moore_matchdg)[ik]) < 400) return ik;
  }
  //
  // if we are here, it means we didn't find anything
  //
  return -1;
 }
