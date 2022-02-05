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
    
    //Eta space for core and overlaps, FCAL 2 and 3
    constexpr static int jFEX_algoSpace_A_lowerFCAL_eta = 33;
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
    
    //Eta space for core and overlaps, FCAL 2 and 3
    constexpr static int jFEX_algoSpace_C_lowerFCAL_eta = 0;
    constexpr static int jFEX_algoSpace_C_upperFCAL_eta = 12;
    
//Phi space breakdown
    constexpr static int jFEX_algoSpace_EMB_start_phi =  8;    
    constexpr static int jFEX_algoSpace_EMB_end_phi  = 24; 
    constexpr static int jFEX_algoSpace_EMIE_start_phi =  4;  
    constexpr static int jFEX_algoSpace_EMIE_end_phi =  12;
    constexpr static int jFEX_algoSpace_FCAL_start_phi =  2;
    constexpr static int jFEX_algoSpace_FCAL_end_phi =  6;
  

   //define constants needed by gFEX Jet algorithm
    constexpr static int centralNphi = 32;
    constexpr static int forwardNphi = 16;

    constexpr static int n_partial = 4;
    constexpr static int ABCrows = 32;
    constexpr static int ABcolumns = 12;
    constexpr static int Ccolumns = 7;
    constexpr static int ABcolumnsEng = 6;
    constexpr static bool ENABLE_INTER_AB = true;
    constexpr static bool ENABLE_INTER_C = true;
    constexpr static int gJetTOBfib = 4;
    constexpr static int BTOBFIB = 6;
    //define constants needed by gFEX JwoJ algorithm
    constexpr static unsigned int aFPGA_A = 48;
    constexpr static unsigned int bFPGA_A = 48;
    constexpr static unsigned int aFPGA_B = 52;
    constexpr static unsigned int bFPGA_B = 52;
    constexpr static int gblockThreshold = 125;//*200 MeV = 25 GeV


  };

} // end of namespace

#endif
