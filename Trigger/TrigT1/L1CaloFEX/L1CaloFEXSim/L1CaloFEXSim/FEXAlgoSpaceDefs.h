/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           FEXAlgoSpaceDefs.h  -
//                              -------------------
//     begin                : 08 02 2021
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/

#ifndef FEXAlgoSpaceDefs_H
#define FEXAlgoSpaceDefs_H

namespace LVL1 {

  //Doxygen class description below:
  /** The FEXAlgoSpaceDefs class defines the size of a single jFEX algorithm window, for use in jFEXFPGA
  */

  class FEXAlgoSpaceDefs {

  public:

    constexpr static int jFEX_wide_algoSpace_width = 45;
    constexpr static int jFEX_thin_algoSpace_width = 24;
    constexpr static int jFEX_algoSpace_height = 32; 
    constexpr static int jFEX_FCAL_start = 700000; 
 
//Array breakdown of jFEX_wide_algoSpace_width matric to indicate different eta and phi regions

//A side (jFEX Module 5):

    //Eta space for core area 
    constexpr static int jFEX_algoSpace_A_EMB_eta = 8;
    constexpr static int jFEX_algoSpace_A_EMIE_eta = 17;
    constexpr static int jFEX_algoSpace_A_FCAL_start_eta = 21; 
    constexpr static int jFEX_algoSpace_A_FCAL_end_eta = 33;
 
    //Eta space for core and overlaps, EM only
    constexpr static int jFEX_algoSpace_A_lowerEM_eta = 0;
    constexpr static int jFEX_algoSpace_A_upperEM_eta = 33;
    
    //Eta space for core and overlaps, FCAL 2 (33-40) and 3 (41-44)
    constexpr static int jFEX_algoSpace_A_lowerFCAL_eta = 33;
    constexpr static int jFEX_algoSpace_A_upperFCAL2_eta = 41;
    constexpr static int jFEX_algoSpace_A_upperFCAL_eta = 45;

//C side (jFEX module 0):
//there are more values on the C side as the matrix cannot be flipped in the bitwise framework.

   //Eta space for core area 
    constexpr static int jFEX_algoSpace_C_EMB_start_eta = 28;
    constexpr static int jFEX_algoSpace_C_EMB_end_eta = 37;
    constexpr static int jFEX_algoSpace_C_EMIE_start_eta = 24;
    constexpr static int jFEX_algoSpace_C_EMIE_end_eta = 28;
    constexpr static int jFEX_algoSpace_C_FCAL_start_eta = 12;
    constexpr static int jFEX_algoSpace_C_FCAL_end_eta = 24;
 
   //Eta space for core and overlaps, EM only
    constexpr static int jFEX_algoSpace_C_lowerEM_eta = 12;
    constexpr static int jFEX_algoSpace_C_upperEM_eta = 45;
    
    //Eta space for core and overlaps, FCAL 2 (4-11) and 3 (0-3)
    constexpr static int jFEX_algoSpace_C_lowerFCAL_eta = 0;
    constexpr static int jFEX_algoSpace_C_lowerFCAL2_eta = 4;
    constexpr static int jFEX_algoSpace_C_upperFCAL_eta = 12;

    //First and second FCAL 1st layer  eta bins
    constexpr static int jFEX_algoSpace_FCAL1_2nd =  22;
    constexpr static int jFEX_algoSpace_A_FCAL1_1st =  21;
    constexpr static int jFEX_algoSpace_C_FCAL1_1st =  23;
    
    //Lowest/highest  eta for jFEX Electrons
    constexpr static int jFEX_algoSpace_A_FwdEl_start =  14;
    constexpr static int jFEX_algoSpace_C_FwdEl_start =  30;

    //Phi space breakdown
    constexpr static int jFEX_algoSpace_EMB_start_phi =  8;    
    constexpr static int jFEX_algoSpace_EMB_end_phi  = 24; 
    constexpr static int jFEX_algoSpace_EMIE_start_phi =  4;  
    constexpr static int jFEX_algoSpace_EMIE_end_phi =  12;
    constexpr static int jFEX_algoSpace_FCAL_start_phi =  2;
    constexpr static int jFEX_algoSpace_FCAL_end_phi =  6;
    
// JFEX bit shifting in the TOB words
    // jFEX Taus (jTau)
      // Data locations within word
      static const int jTau_isoBit = 21;
      static const int jTau_etBit  = 10;
      static const int jTau_etaBit = 5;
      static const int jTau_phiBit = 1;
      static const int jTau_satBit = 0;

      // jFEX Forward Electrons (jEM)
      // Data locations within word
      static const int jEM_resBit  = 27;
      static const int jEM_emf2Bit = 25;
      static const int jEM_emf1Bit = 23;
      static const int jEM_isoBit  = 21;
      static const int jEM_etBit   = 10;
      static const int jEM_etaBit  = 5;
      static const int jEM_phiBit  = 1;
      static const int jEM_satBit  = 0;
      
    // jFEX SRJets (jJ)
      // Data locations within word
      static const int jJ_resBit = 21;
      static const int jJ_etBit  = 10;
      static const int jJ_etaBit = 5;
      static const int jJ_phiBit = 1;
      static const int jJ_satBit = 0;
      
    // jFEX LRJets (jLJ)
      // Data locations within word
      static const int jLJ_resBit = 23;
      static const int jLJ_etBit  = 10;
      static const int jLJ_etaBit = 5;
      static const int jLJ_phiBit = 1;
      static const int jLJ_satBit = 0; 
      
    // jFEX MET (jXE)
      // Data locations within word
      static const int jXE_ResBit   = 31;
      static const int jXE_Ey_Bit   = 16;
      static const int jXE_Ex_Bit   = 1;
      static const int jXE_SatBit   = 0;
      
    // jFEX SumET (jTE)
      // Data locations within word
      static const int jTE_Sat_upperBit = 31;
      static const int jTE_Et_upperBit  = 16;
      static const int jTE_Et_lowerBit  = 1; 
      static const int jTE_Sat_lowerBit = 0; 
      
    // jFEX SRJet Et Calibration (Calculated by performancy group, Elena Michelle Villhauer's QT)
    constexpr static int SRJ_Calib_params[6][9] =
    {   //<20  <30  <40  <50  <65  <80 <110 <150 <inf  GeV
        { 320, 278, 250, 237, 220, 209, 200, 185, 170 },// jFEX 0  FCal  
        { 376, 328, 297, 278, 261, 250, 232, 220, 200 },// jFEX 1  Central  
        { 320, 284, 261, 250, 237, 224, 213, 200, 185 },// jFEX 2  Central   
        { 320, 284, 266, 246, 237, 220, 213, 200, 182 },// jFEX 3  Central  
        { 376, 328, 297, 278, 261, 246, 237, 224, 200 },// jFEX 4  Central  
        { 320, 278, 250, 237, 220, 209, 200, 185, 170 } // jFEX 5  FCal 
    }; 

  

   //define constants needed by gFEX Jet algorithm
    constexpr static int centralNphi = 32;
    constexpr static int forwardNphi = 16;
    constexpr static int centralNeta = 12;
    constexpr static int forwardNeta = 8;
    constexpr static int totalNeta = 40;

    constexpr static int n_partial = 4;
    constexpr static int ABCrows = 32;
    constexpr static int ABcolumns = 12;
    constexpr static int ABcolumnsEng = 6;
    constexpr static bool ENABLE_INTER_AB = true;
    constexpr static bool ENABLE_INTER_C = true;
    constexpr static int gJetTOBfib = 4;
    constexpr static int BTOBFIB = 6;
    constexpr static bool ENABLE_PUC = true;
    constexpr static bool APPLY_TRUNC = false;
    //define constants needed by gFEX JwoJ algorithm
    constexpr static bool ENABLE_JWOJ_C = false;


  };

} // end of namespace

#endif
