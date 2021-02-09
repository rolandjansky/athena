/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  Loop over HEC wheels to store 
//  Sampling fractions as functions of Pseudorapidity
//  (for 100 GeV EM particles)
//

void store_eta (
     Char_t const *G4Ver = "10.1",          //  Geant4 version
     Char_t const *PhLis = "FTFP_BERT_ATL", //  Physics list
     Char_t const *Dir   = "/eos/atlas/atlascerngroupdisk/proj-simul/G4Run3/SamplingFractions/HEC")
{
//   
//
   Int_t NHEC  = 2;
//
   Int_t iPart = 0;
   Int_t IEner = 100;   
//   
   for (Int_t iHEC=0; iHEC<NHEC; iHEC++)
         samfr_etaX ( iHEC, iPart,IEner, G4Ver,PhLis, Dir );
//   
//
}
////////////////////////////////////////////////////////////////////////
