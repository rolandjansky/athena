/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <math.h>
#include <cmath>

#include "TrigMuonBackExtrapolator/BackExtrapolator.h"

/*
///////////////////////////////////// LUTS //////////////////////////////////////////
// Version 3.0 - 2012 Data Tuning (for data and MC) - N.Romptis/S.Giagu

//Barrel
double data_Barrel_Param[2][2][2]; 
data_Barrel_Param[0][0][0] = -0.344175;   data_Barrel_Param[0][0][1] = -0.007004; 
data_Barrel_Param[0][1][0] =  0.389134;   data_Barrel_Param[0][1][1] =  0.005503; 
data_Barrel_Param[1][0][0] = -0.394867;   data_Barrel_Param[1][0][1] = -0.005019; 
data_Barrel_Param[1][1][0] =  0.361533;   data_Barrel_Param[1][1][1] =  0.006671; 
double data_Barrel_Sigmas[2][2][2][2]; 
data_Barrel_Sigmas[0][0][0][0] =  0.019819;   data_Barrel_Sigmas[0][0][0][1] =  0.009023; 
data_Barrel_Sigmas[0][1][0][0] =  0.019706;   data_Barrel_Sigmas[0][1][0][1] =  0.008985; 
data_Barrel_Sigmas[1][0][0][0] =  0.025500;   data_Barrel_Sigmas[1][0][0][1] =  0.008895; 
data_Barrel_Sigmas[1][1][0][0] =  0.025495;   data_Barrel_Sigmas[1][1][0][1] =  0.008971; 
data_Barrel_Sigmas[0][0][1][0] =  0.157201;  data_Barrel_Sigmas[0][0][1][1] =  0.000833; 
data_Barrel_Sigmas[0][1][1][0] =  0.153530;  data_Barrel_Sigmas[0][1][1][1] =  0.001045; 
data_Barrel_Sigmas[1][0][1][0] =  0.152441;  data_Barrel_Sigmas[1][0][1][1] =  0.001100; 
data_Barrel_Sigmas[1][1][1][0] =  0.183810;  data_Barrel_Sigmas[1][1][1][1] = -0.000077; 

//Endcaps - TriggerST
double data_Endcap_TriggerST_Param[4][12][2][2][2][2];
 data_Endcap_TriggerST_Param[0][0][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][0][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][0][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][0][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][1][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][1][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][1][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][1][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][2][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][2][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][2][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][2][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][3][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][3][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][3][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][3][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][4][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][4][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][4][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][4][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][5][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][5][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][5][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][5][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][6][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][6][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][6][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][6][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][7][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][7][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][7][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][7][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][8][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][8][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][8][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][8][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][9][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][9][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][9][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][9][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][10][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][10][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][10][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][10][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][11][0][0][0][0] =     0.39008528; data_Endcap_TriggerST_Param[0][11][0][0][0][1] =    -0.00008796;  data_Endcap_TriggerST_Param[0][11][0][0][1][0] =    -0.23731600; data_Endcap_TriggerST_Param[0][11][0][0][1][1] =    -0.00697019;
 data_Endcap_TriggerST_Param[0][0][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][0][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][0][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][0][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][1][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][1][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][1][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][1][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][2][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][2][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][2][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][2][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][3][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][3][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][3][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][3][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][4][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][4][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][4][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][4][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][5][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][5][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][5][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][5][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][6][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][6][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][6][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][6][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][7][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][7][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][7][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][7][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][8][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][8][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][8][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][8][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][9][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][9][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][9][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][9][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][10][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][10][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][10][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][10][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][11][0][1][0][0] =    -0.41645430; data_Endcap_TriggerST_Param[0][11][0][1][0][1] =     0.00196946;  data_Endcap_TriggerST_Param[0][11][0][1][1][0] =     0.24404074; data_Endcap_TriggerST_Param[0][11][0][1][1][1] =     0.00364261;
 data_Endcap_TriggerST_Param[0][0][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][0][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][0][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][0][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][1][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][1][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][1][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][1][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][2][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][2][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][2][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][2][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][3][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][3][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][3][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][3][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][4][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][4][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][4][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][4][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][5][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][5][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][5][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][5][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][6][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][6][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][6][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][6][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][7][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][7][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][7][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][7][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][8][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][8][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][8][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][8][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][9][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][9][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][9][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][9][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][10][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][10][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][10][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][10][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][11][1][0][0][0] =     0.38674998; data_Endcap_TriggerST_Param[0][11][1][0][0][1] =    -0.00083805;  data_Endcap_TriggerST_Param[0][11][1][0][1][0] =    -0.24427115; data_Endcap_TriggerST_Param[0][11][1][0][1][1] =    -0.00569835;
 data_Endcap_TriggerST_Param[0][0][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][0][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][0][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][0][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][1][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][1][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][1][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][1][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][2][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][2][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][2][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][2][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][3][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][3][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][3][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][3][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][4][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][4][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][4][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][4][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][5][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][5][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][5][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][5][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][6][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][6][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][6][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][6][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][7][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][7][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][7][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][7][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][8][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][8][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][8][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][8][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][9][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][9][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][9][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][9][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][10][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][10][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][10][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][10][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[0][11][1][1][0][0] =    -0.44981688; data_Endcap_TriggerST_Param[0][11][1][1][0][1] =     0.00247323;  data_Endcap_TriggerST_Param[0][11][1][1][1][0] =     0.36769134; data_Endcap_TriggerST_Param[0][11][1][1][1][1] =     0.00051971;
 data_Endcap_TriggerST_Param[1][0][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][0][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][0][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][0][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][1][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][1][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][1][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][1][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][2][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][2][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][2][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][2][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][3][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][3][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][3][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][3][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][4][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][4][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][4][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][4][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][5][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][5][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][5][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][5][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][6][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][6][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][6][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][6][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][7][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][7][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][7][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][7][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][8][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][8][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][8][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][8][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][9][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][9][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][9][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][9][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][10][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][10][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][10][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][10][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][11][0][0][0][0] =     0.12434421; data_Endcap_TriggerST_Param[1][11][0][0][0][1] =     0.00301339;  data_Endcap_TriggerST_Param[1][11][0][0][1][0] =    -0.08611453; data_Endcap_TriggerST_Param[1][11][0][0][1][1] =    -0.00994466;
 data_Endcap_TriggerST_Param[1][0][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][0][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][0][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][0][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][1][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][1][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][1][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][1][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][2][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][2][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][2][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][2][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][3][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][3][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][3][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][3][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][4][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][4][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][4][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][4][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][5][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][5][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][5][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][5][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][6][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][6][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][6][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][6][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][7][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][7][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][7][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][7][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][8][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][8][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][8][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][8][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][9][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][9][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][9][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][9][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][10][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][10][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][10][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][10][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][11][0][1][0][0] =    -0.15175075; data_Endcap_TriggerST_Param[1][11][0][1][0][1] =    -0.00214425;  data_Endcap_TriggerST_Param[1][11][0][1][1][0] =     0.04845307; data_Endcap_TriggerST_Param[1][11][0][1][1][1] =     0.00765097;
 data_Endcap_TriggerST_Param[1][0][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][0][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][0][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][0][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][1][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][1][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][1][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][1][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][2][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][2][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][2][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][2][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][3][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][3][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][3][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][3][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][4][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][4][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][4][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][4][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][5][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][5][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][5][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][5][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][6][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][6][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][6][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][6][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][7][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][7][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][7][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][7][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][8][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][8][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][8][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][8][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][9][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][9][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][9][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][9][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][10][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][10][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][10][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][10][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][11][1][0][0][0] =     0.24444920; data_Endcap_TriggerST_Param[1][11][1][0][0][1] =     0.00041680;  data_Endcap_TriggerST_Param[1][11][1][0][1][0] =    -0.14596133; data_Endcap_TriggerST_Param[1][11][1][0][1][1] =    -0.00769274;
 data_Endcap_TriggerST_Param[1][0][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][0][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][0][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][0][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][1][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][1][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][1][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][1][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][2][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][2][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][2][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][2][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][3][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][3][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][3][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][3][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][4][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][4][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][4][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][4][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][5][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][5][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][5][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][5][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][6][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][6][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][6][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][6][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][7][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][7][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][7][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][7][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][8][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][8][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][8][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][8][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][9][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][9][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][9][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][9][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][10][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][10][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][10][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][10][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[1][11][1][1][0][0] =    -0.27022220; data_Endcap_TriggerST_Param[1][11][1][1][0][1] =     0.00099873;  data_Endcap_TriggerST_Param[1][11][1][1][1][0] =     0.32536867; data_Endcap_TriggerST_Param[1][11][1][1][1][1] =     0.00131456;
 data_Endcap_TriggerST_Param[2][0][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][0][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][0][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][0][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][1][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][1][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][1][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][1][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][2][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][2][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][2][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][2][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][3][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][3][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][3][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][3][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][4][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][4][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][4][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][4][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][5][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][5][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][5][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][5][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][6][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][6][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][6][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][6][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][7][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][7][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][7][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][7][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][8][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][8][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][8][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][8][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][9][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][9][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][9][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][9][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][10][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][10][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][10][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][10][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][11][0][0][0][0] =     0.34728334; data_Endcap_TriggerST_Param[2][11][0][0][0][1] =    -0.00001011;  data_Endcap_TriggerST_Param[2][11][0][0][1][0] =    -0.22728005; data_Endcap_TriggerST_Param[2][11][0][0][1][1] =    -0.00112712;
 data_Endcap_TriggerST_Param[2][0][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][0][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][0][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][0][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][1][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][1][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][1][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][1][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][2][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][2][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][2][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][2][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][3][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][3][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][3][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][3][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][4][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][4][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][4][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][4][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][5][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][5][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][5][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][5][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][6][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][6][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][6][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][6][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][7][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][7][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][7][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][7][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][8][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][8][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][8][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][8][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][9][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][9][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][9][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][9][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][10][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][10][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][10][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][10][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][11][0][1][0][0] =    -0.32401417; data_Endcap_TriggerST_Param[2][11][0][1][0][1] =    -0.00136715;  data_Endcap_TriggerST_Param[2][11][0][1][1][0] =     0.14903215; data_Endcap_TriggerST_Param[2][11][0][1][1][1] =     0.00117954;
 data_Endcap_TriggerST_Param[2][0][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][0][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][0][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][0][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][1][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][1][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][1][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][1][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][2][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][2][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][2][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][2][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][3][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][3][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][3][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][3][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][4][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][4][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][4][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][4][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][5][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][5][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][5][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][5][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][6][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][6][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][6][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][6][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][7][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][7][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][7][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][7][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][8][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][8][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][8][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][8][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][9][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][9][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][9][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][9][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][10][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][10][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][10][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][10][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][11][1][0][0][0] =     0.45776655; data_Endcap_TriggerST_Param[2][11][1][0][0][1] =    -0.00218530;  data_Endcap_TriggerST_Param[2][11][1][0][1][0] =    -0.18954463; data_Endcap_TriggerST_Param[2][11][1][0][1][1] =    -0.00189962;
 data_Endcap_TriggerST_Param[2][0][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][0][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][0][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][0][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][1][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][1][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][1][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][1][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][2][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][2][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][2][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][2][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][3][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][3][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][3][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][3][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][4][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][4][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][4][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][4][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][5][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][5][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][5][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][5][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][6][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][6][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][6][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][6][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][7][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][7][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][7][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][7][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][8][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][8][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][8][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][8][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][9][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][9][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][9][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][9][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][10][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][10][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][10][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][10][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[2][11][1][1][0][0] =    -0.39464600; data_Endcap_TriggerST_Param[2][11][1][1][0][1] =     0.00224427;  data_Endcap_TriggerST_Param[2][11][1][1][1][0] =     0.26039947; data_Endcap_TriggerST_Param[2][11][1][1][1][1] =    -0.00294014;
 data_Endcap_TriggerST_Param[3][0][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][0][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][0][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][0][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][1][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][1][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][1][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][1][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][2][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][2][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][2][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][2][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][3][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][3][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][3][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][3][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][4][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][4][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][4][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][4][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][5][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][5][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][5][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][5][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][6][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][6][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][6][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][6][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][7][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][7][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][7][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][7][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][8][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][8][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][8][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][8][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][9][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][9][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][9][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][9][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][10][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][10][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][10][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][10][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][11][0][0][0][0] =     0.57829530; data_Endcap_TriggerST_Param[3][11][0][0][0][1] =    -0.00135664;  data_Endcap_TriggerST_Param[3][11][0][0][1][0] =    -0.20128576; data_Endcap_TriggerST_Param[3][11][0][0][1][1] =    -0.00019486;
 data_Endcap_TriggerST_Param[3][0][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][0][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][0][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][0][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][1][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][1][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][1][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][1][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][2][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][2][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][2][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][2][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][3][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][3][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][3][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][3][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][4][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][4][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][4][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][4][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][5][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][5][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][5][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][5][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][6][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][6][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][6][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][6][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][7][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][7][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][7][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][7][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][8][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][8][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][8][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][8][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][9][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][9][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][9][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][9][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][10][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][10][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][10][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][10][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][11][0][1][0][0] =    -0.55326371; data_Endcap_TriggerST_Param[3][11][0][1][0][1] =     0.00203642;  data_Endcap_TriggerST_Param[3][11][0][1][1][0] =     0.16990056; data_Endcap_TriggerST_Param[3][11][0][1][1][1] =    -0.00111816;
 data_Endcap_TriggerST_Param[3][0][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][0][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][0][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][0][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][1][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][1][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][1][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][1][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][2][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][2][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][2][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][2][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][3][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][3][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][3][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][3][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][4][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][4][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][4][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][4][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][5][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][5][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][5][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][5][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][6][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][6][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][6][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][6][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][7][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][7][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][7][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][7][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][8][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][8][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][8][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][8][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][9][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][9][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][9][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][9][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][10][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][10][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][10][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][10][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][11][1][0][0][0] =     0.52335774; data_Endcap_TriggerST_Param[3][11][1][0][0][1] =    -0.00018286;  data_Endcap_TriggerST_Param[3][11][1][0][1][0] =    -0.17730148; data_Endcap_TriggerST_Param[3][11][1][0][1][1] =    -0.00056622;
 data_Endcap_TriggerST_Param[3][0][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][0][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][0][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][0][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][1][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][1][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][1][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][1][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][2][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][2][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][2][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][2][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][3][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][3][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][3][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][3][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][4][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][4][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][4][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][4][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][5][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][5][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][5][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][5][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][6][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][6][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][6][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][6][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][7][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][7][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][7][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][7][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][8][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][8][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][8][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][8][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][9][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][9][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][9][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][9][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][10][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][10][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][10][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][10][1][1][1][1] =    -0.00164896;
 data_Endcap_TriggerST_Param[3][11][1][1][0][0] =    -0.60069544; data_Endcap_TriggerST_Param[3][11][1][1][0][1] =     0.00269249;  data_Endcap_TriggerST_Param[3][11][1][1][1][0] =     0.19942259; data_Endcap_TriggerST_Param[3][11][1][1][1][1] =    -0.00164896;

double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2];
 data_Endcap_TriggerST_Sigmas[0][0][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][0][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][0][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][0][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][1][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][1][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][1][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][1][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][2][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][2][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][2][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][2][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][3][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][3][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][3][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][3][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][4][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][4][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][4][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][4][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][5][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][5][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][5][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][5][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][6][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][6][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][6][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][6][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][7][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][7][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][7][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][7][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][8][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][8][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][8][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][8][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][9][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][9][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][9][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][9][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][10][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][10][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][10][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][10][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][11][0][0][0][0] =     0.13281234; data_Endcap_TriggerST_Sigmas[0][11][0][0][0][1] =     0.00198762;  data_Endcap_TriggerST_Sigmas[0][11][0][0][1][0] =     0.33323734; data_Endcap_TriggerST_Sigmas[0][11][0][0][1][1] =    -0.00040997;
 data_Endcap_TriggerST_Sigmas[0][0][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][0][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][0][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][0][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][1][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][1][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][1][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][1][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][2][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][2][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][2][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][2][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][3][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][3][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][3][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][3][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][4][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][4][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][4][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][4][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][5][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][5][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][5][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][5][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][6][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][6][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][6][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][6][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][7][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][7][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][7][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][7][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][8][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][8][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][8][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][8][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][9][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][9][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][9][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][9][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][10][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][10][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][10][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][10][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][11][0][1][0][0] =     0.10881599; data_Endcap_TriggerST_Sigmas[0][11][0][1][0][1] =     0.00246415;  data_Endcap_TriggerST_Sigmas[0][11][0][1][1][0] =     0.36239266; data_Endcap_TriggerST_Sigmas[0][11][0][1][1][1] =    -0.00039195;
 data_Endcap_TriggerST_Sigmas[0][0][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][0][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][0][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][0][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][1][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][1][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][1][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][1][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][2][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][2][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][2][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][2][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][3][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][3][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][3][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][3][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][4][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][4][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][4][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][4][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][5][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][5][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][5][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][5][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][6][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][6][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][6][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][6][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][7][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][7][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][7][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][7][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][8][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][8][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][8][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][8][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][9][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][9][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][9][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][9][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][10][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][10][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][10][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][10][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][11][1][0][0][0] =     0.12531138; data_Endcap_TriggerST_Sigmas[0][11][1][0][0][1] =     0.00201682;  data_Endcap_TriggerST_Sigmas[0][11][1][0][1][0] =     0.37857238; data_Endcap_TriggerST_Sigmas[0][11][1][0][1][1] =    -0.00118625;
 data_Endcap_TriggerST_Sigmas[0][0][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][0][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][0][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][0][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][1][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][1][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][1][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][1][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][2][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][2][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][2][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][2][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][3][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][3][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][3][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][3][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][4][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][4][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][4][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][4][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][5][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][5][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][5][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][5][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][6][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][6][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][6][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][6][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][7][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][7][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][7][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][7][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][8][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][8][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][8][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][8][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][9][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][9][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][9][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][9][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][10][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][10][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][10][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][10][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[0][11][1][1][0][0] =     0.11425225; data_Endcap_TriggerST_Sigmas[0][11][1][1][0][1] =     0.00245888;  data_Endcap_TriggerST_Sigmas[0][11][1][1][1][0] =     0.33168570; data_Endcap_TriggerST_Sigmas[0][11][1][1][1][1] =     0.00033995;
 data_Endcap_TriggerST_Sigmas[1][0][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][0][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][0][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][0][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][1][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][1][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][1][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][1][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][2][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][2][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][2][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][2][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][3][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][3][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][3][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][3][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][4][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][4][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][4][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][4][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][5][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][5][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][5][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][5][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][6][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][6][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][6][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][6][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][7][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][7][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][7][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][7][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][8][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][8][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][8][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][8][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][9][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][9][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][9][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][9][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][10][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][10][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][10][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][10][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][11][0][0][0][0] =     0.12856069; data_Endcap_TriggerST_Sigmas[1][11][0][0][0][1] =     0.00119065;  data_Endcap_TriggerST_Sigmas[1][11][0][0][1][0] =     0.23895755; data_Endcap_TriggerST_Sigmas[1][11][0][0][1][1] =     0.00183274;
 data_Endcap_TriggerST_Sigmas[1][0][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][0][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][0][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][0][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][1][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][1][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][1][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][1][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][2][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][2][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][2][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][2][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][3][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][3][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][3][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][3][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][4][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][4][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][4][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][4][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][5][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][5][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][5][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][5][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][6][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][6][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][6][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][6][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][7][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][7][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][7][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][7][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][8][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][8][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][8][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][8][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][9][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][9][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][9][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][9][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][10][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][10][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][10][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][10][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][11][0][1][0][0] =     0.15032808; data_Endcap_TriggerST_Sigmas[1][11][0][1][0][1] =     0.00148648;  data_Endcap_TriggerST_Sigmas[1][11][0][1][1][0] =     0.22652846; data_Endcap_TriggerST_Sigmas[1][11][0][1][1][1] =     0.00305960;
 data_Endcap_TriggerST_Sigmas[1][0][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][0][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][0][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][0][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][1][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][1][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][1][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][1][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][2][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][2][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][2][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][2][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][3][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][3][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][3][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][3][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][4][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][4][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][4][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][4][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][5][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][5][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][5][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][5][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][6][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][6][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][6][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][6][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][7][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][7][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][7][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][7][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][8][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][8][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][8][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][8][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][9][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][9][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][9][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][9][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][10][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][10][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][10][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][10][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][11][1][0][0][0] =     0.13699295; data_Endcap_TriggerST_Sigmas[1][11][1][0][0][1] =     0.00169010;  data_Endcap_TriggerST_Sigmas[1][11][1][0][1][0] =     0.36554785; data_Endcap_TriggerST_Sigmas[1][11][1][0][1][1] =    -0.00035728;
 data_Endcap_TriggerST_Sigmas[1][0][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][0][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][0][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][0][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][1][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][1][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][1][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][1][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][2][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][2][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][2][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][2][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][3][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][3][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][3][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][3][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][4][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][4][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][4][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][4][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][5][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][5][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][5][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][5][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][6][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][6][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][6][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][6][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][7][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][7][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][7][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][7][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][8][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][8][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][8][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][8][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][9][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][9][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][9][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][9][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][10][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][10][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][10][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][10][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[1][11][1][1][0][0] =     0.15657617; data_Endcap_TriggerST_Sigmas[1][11][1][1][0][1] =     0.00057384;  data_Endcap_TriggerST_Sigmas[1][11][1][1][1][0] =     0.36567692; data_Endcap_TriggerST_Sigmas[1][11][1][1][1][1] =    -0.00075383;
 data_Endcap_TriggerST_Sigmas[2][0][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][0][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][0][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][0][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][1][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][1][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][1][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][1][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][2][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][2][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][2][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][2][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][3][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][3][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][3][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][3][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][4][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][4][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][4][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][4][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][5][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][5][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][5][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][5][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][6][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][6][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][6][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][6][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][7][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][7][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][7][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][7][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][8][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][8][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][8][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][8][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][9][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][9][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][9][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][9][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][10][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][10][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][10][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][10][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][11][0][0][0][0] =     0.08280142; data_Endcap_TriggerST_Sigmas[2][11][0][0][0][1] =     0.00379700;  data_Endcap_TriggerST_Sigmas[2][11][0][0][1][0] =     0.12491069; data_Endcap_TriggerST_Sigmas[2][11][0][0][1][1] =     0.00149252;
 data_Endcap_TriggerST_Sigmas[2][0][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][0][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][0][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][0][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][1][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][1][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][1][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][1][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][2][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][2][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][2][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][2][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][3][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][3][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][3][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][3][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][4][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][4][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][4][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][4][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][5][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][5][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][5][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][5][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][6][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][6][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][6][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][6][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][7][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][7][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][7][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][7][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][8][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][8][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][8][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][8][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][9][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][9][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][9][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][9][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][10][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][10][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][10][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][10][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][11][0][1][0][0] =     0.13244076; data_Endcap_TriggerST_Sigmas[2][11][0][1][0][1] =     0.00212945;  data_Endcap_TriggerST_Sigmas[2][11][0][1][1][0] =     0.09367422; data_Endcap_TriggerST_Sigmas[2][11][0][1][1][1] =     0.00172546;
 data_Endcap_TriggerST_Sigmas[2][0][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][0][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][0][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][0][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][1][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][1][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][1][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][1][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][2][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][2][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][2][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][2][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][3][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][3][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][3][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][3][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][4][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][4][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][4][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][4][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][5][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][5][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][5][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][5][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][6][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][6][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][6][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][6][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][7][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][7][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][7][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][7][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][8][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][8][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][8][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][8][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][9][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][9][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][9][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][9][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][10][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][10][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][10][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][10][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][11][1][0][0][0] =     0.14640195; data_Endcap_TriggerST_Sigmas[2][11][1][0][0][1] =     0.00135310;  data_Endcap_TriggerST_Sigmas[2][11][1][0][1][0] =     0.13339166; data_Endcap_TriggerST_Sigmas[2][11][1][0][1][1] =     0.00046171;
 data_Endcap_TriggerST_Sigmas[2][0][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][0][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][0][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][0][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][1][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][1][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][1][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][1][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][2][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][2][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][2][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][2][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][3][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][3][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][3][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][3][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][4][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][4][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][4][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][4][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][5][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][5][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][5][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][5][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][6][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][6][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][6][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][6][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][7][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][7][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][7][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][7][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][8][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][8][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][8][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][8][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][9][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][9][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][9][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][9][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][10][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][10][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][10][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][10][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[2][11][1][1][0][0] =    -0.01163394; data_Endcap_TriggerST_Sigmas[2][11][1][1][0][1] =     0.00739889;  data_Endcap_TriggerST_Sigmas[2][11][1][1][1][0] =     0.15848626; data_Endcap_TriggerST_Sigmas[2][11][1][1][1][1] =     0.00146797;
 data_Endcap_TriggerST_Sigmas[3][0][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][0][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][0][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][0][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][1][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][1][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][1][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][1][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][2][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][2][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][2][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][2][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][3][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][3][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][3][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][3][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][4][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][4][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][4][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][4][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][5][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][5][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][5][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][5][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][6][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][6][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][6][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][6][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][7][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][7][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][7][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][7][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][8][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][8][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][8][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][8][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][9][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][9][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][9][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][9][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][10][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][10][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][10][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][10][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][11][0][0][0][0] =     0.10413306; data_Endcap_TriggerST_Sigmas[3][11][0][0][0][1] =     0.00334968;  data_Endcap_TriggerST_Sigmas[3][11][0][0][1][0] =     0.15075245; data_Endcap_TriggerST_Sigmas[3][11][0][0][1][1] =     0.00026428;
 data_Endcap_TriggerST_Sigmas[3][0][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][0][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][0][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][0][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][1][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][1][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][1][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][1][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][2][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][2][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][2][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][2][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][3][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][3][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][3][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][3][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][4][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][4][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][4][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][4][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][5][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][5][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][5][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][5][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][6][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][6][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][6][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][6][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][7][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][7][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][7][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][7][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][8][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][8][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][8][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][8][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][9][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][9][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][9][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][9][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][10][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][10][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][10][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][10][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][11][0][1][0][0] =     0.08034411; data_Endcap_TriggerST_Sigmas[3][11][0][1][0][1] =     0.00396750;  data_Endcap_TriggerST_Sigmas[3][11][0][1][1][0] =     0.15655036; data_Endcap_TriggerST_Sigmas[3][11][0][1][1][1] =     0.00000012;
 data_Endcap_TriggerST_Sigmas[3][0][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][0][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][0][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][0][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][1][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][1][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][1][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][1][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][2][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][2][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][2][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][2][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][3][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][3][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][3][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][3][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][4][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][4][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][4][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][4][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][5][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][5][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][5][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][5][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][6][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][6][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][6][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][6][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][7][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][7][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][7][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][7][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][8][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][8][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][8][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][8][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][9][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][9][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][9][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][9][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][10][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][10][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][10][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][10][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][11][1][0][0][0] =     0.09289853; data_Endcap_TriggerST_Sigmas[3][11][1][0][0][1] =     0.00358122;  data_Endcap_TriggerST_Sigmas[3][11][1][0][1][0] =     0.13419320; data_Endcap_TriggerST_Sigmas[3][11][1][0][1][1] =     0.00075152;
 data_Endcap_TriggerST_Sigmas[3][0][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][0][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][0][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][0][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][1][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][1][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][1][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][1][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][2][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][2][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][2][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][2][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][3][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][3][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][3][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][3][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][4][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][4][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][4][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][4][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][5][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][5][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][5][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][5][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][6][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][6][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][6][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][6][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][7][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][7][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][7][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][7][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][8][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][8][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][8][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][8][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][9][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][9][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][9][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][9][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][10][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][10][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][10][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][10][1][1][1][1] =    -0.00003769;
 data_Endcap_TriggerST_Sigmas[3][11][1][1][0][0] =     0.10216539; data_Endcap_TriggerST_Sigmas[3][11][1][1][0][1] =     0.00344601;  data_Endcap_TriggerST_Sigmas[3][11][1][1][1][0] =     0.15786379; data_Endcap_TriggerST_Sigmas[3][11][1][1][1][1] =    -0.00003769;

//Endcaps - InnerST
 double data_Endcap_InnerST_Param[4][12][2][2][2];
double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2];

 data_Endcap_InnerST_PhiSigmas[0][0][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][0][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][0][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][0][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][0][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][0][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][0][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][0][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][1][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][1][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][1][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][1][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][1][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][1][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][1][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][1][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][2][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][2][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][2][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][2][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][2][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][2][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][2][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][2][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][3][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][3][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][3][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][3][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][3][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][3][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][3][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][3][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][4][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][4][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][4][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][4][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][4][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][4][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][4][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][4][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][5][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][5][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][5][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][5][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][5][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][5][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][5][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][5][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][6][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][6][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][6][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][6][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][6][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][6][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][6][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][6][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][7][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][7][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][7][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][7][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][7][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][7][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][7][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][7][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][8][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][8][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][8][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][8][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][8][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][8][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][8][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][8][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][9][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][9][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][9][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][9][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][9][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][9][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][9][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][9][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][10][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][10][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][10][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][10][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][10][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][10][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][10][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][10][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[0][11][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[0][11][0][0][1]=0.00549303; 
 data_Endcap_InnerST_PhiSigmas[2][11][0][0][0]=0.07225676; data_Endcap_InnerST_PhiSigmas[2][11][0][0][1]=0.00549303; 
  data_Endcap_InnerST_Param[0][11][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[0][11][0][0][1]=-0.00504776; 
 data_Endcap_InnerST_Param[2][11][0][0][0]=-0.24234227; data_Endcap_InnerST_Param[2][11][0][0][1]=-0.00504776; 
  data_Endcap_InnerST_PhiSigmas[1][0][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][0][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][0][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][0][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][0][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][0][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][0][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][0][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][1][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][1][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][1][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][1][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][1][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][1][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][1][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][1][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][2][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][2][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][2][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][2][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][2][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][2][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][2][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][2][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][3][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][3][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][3][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][3][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][3][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][3][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][3][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][3][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][4][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][4][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][4][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][4][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][4][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][4][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][4][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][4][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][5][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][5][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][5][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][5][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][5][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][5][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][5][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][5][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][6][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][6][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][6][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][6][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][6][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][6][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][6][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][6][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][7][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][7][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][7][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][7][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][7][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][7][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][7][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][7][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][8][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][8][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][8][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][8][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][8][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][8][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][8][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][8][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][9][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][9][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][9][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][9][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][9][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][9][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][9][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][9][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][10][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][10][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][10][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][10][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][10][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][10][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][10][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][10][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[1][11][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[1][11][0][0][1]=0.00766950; 
 data_Endcap_InnerST_PhiSigmas[3][11][0][0][0]=0.04284341; data_Endcap_InnerST_PhiSigmas[3][11][0][0][1]=0.00766950; 
  data_Endcap_InnerST_Param[1][11][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[1][11][0][0][1]=-0.00276037; 
 data_Endcap_InnerST_Param[3][11][0][0][0]=-0.20331495; data_Endcap_InnerST_Param[3][11][0][0][1]=-0.00276037; 
  data_Endcap_InnerST_PhiSigmas[0][0][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][0][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][0][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][0][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][0][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][0][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][0][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][0][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][1][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][1][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][1][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][1][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][1][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][1][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][1][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][1][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][2][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][2][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][2][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][2][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][2][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][2][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][2][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][2][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][3][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][3][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][3][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][3][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][3][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][3][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][3][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][3][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][4][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][4][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][4][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][4][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][4][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][4][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][4][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][4][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][5][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][5][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][5][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][5][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][5][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][5][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][5][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][5][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][6][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][6][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][6][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][6][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][6][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][6][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][6][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][6][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][7][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][7][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][7][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][7][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][7][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][7][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][7][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][7][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][8][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][8][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][8][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][8][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][8][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][8][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][8][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][8][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][9][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][9][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][9][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][9][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][9][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][9][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][9][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][9][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][10][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][10][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][10][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][10][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][10][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][10][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][10][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][10][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[0][11][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[0][11][0][1][1]=0.00433204; 
 data_Endcap_InnerST_PhiSigmas[2][11][0][1][0]=0.10706679; data_Endcap_InnerST_PhiSigmas[2][11][0][1][1]=0.00433204; 
  data_Endcap_InnerST_Param[0][11][0][1][0]=0.25237369; data_Endcap_InnerST_Param[0][11][0][1][1]=0.00264936; 
 data_Endcap_InnerST_Param[2][11][0][1][0]=0.25237369; data_Endcap_InnerST_Param[2][11][0][1][1]=0.00264936; 
  data_Endcap_InnerST_PhiSigmas[1][0][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][0][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][0][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][0][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][0][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][0][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][0][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][0][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][1][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][1][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][1][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][1][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][1][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][1][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][1][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][1][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][2][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][2][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][2][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][2][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][2][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][2][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][2][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][2][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][3][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][3][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][3][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][3][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][3][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][3][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][3][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][3][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][4][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][4][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][4][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][4][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][4][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][4][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][4][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][4][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][5][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][5][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][5][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][5][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][5][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][5][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][5][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][5][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][6][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][6][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][6][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][6][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][6][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][6][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][6][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][6][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][7][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][7][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][7][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][7][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][7][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][7][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][7][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][7][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][8][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][8][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][8][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][8][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][8][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][8][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][8][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][8][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][9][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][9][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][9][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][9][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][9][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][9][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][9][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][9][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][10][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][10][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][10][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][10][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][10][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][10][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][10][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][10][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[1][11][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[1][11][0][1][1]=0.00753565; 
 data_Endcap_InnerST_PhiSigmas[3][11][0][1][0]=0.03874343; data_Endcap_InnerST_PhiSigmas[3][11][0][1][1]=0.00753565; 
  data_Endcap_InnerST_Param[1][11][0][1][0]=0.22437839; data_Endcap_InnerST_Param[1][11][0][1][1]=-0.00015519; 
 data_Endcap_InnerST_Param[3][11][0][1][0]=0.22437839; data_Endcap_InnerST_Param[3][11][0][1][1]=-0.00015519; 
  data_Endcap_InnerST_PhiSigmas[0][0][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][0][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][0][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][0][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][0][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][0][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][0][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][0][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][1][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][1][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][1][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][1][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][1][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][1][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][1][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][1][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][2][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][2][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][2][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][2][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][2][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][2][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][2][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][2][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][3][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][3][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][3][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][3][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][3][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][3][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][3][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][3][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][4][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][4][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][4][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][4][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][4][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][4][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][4][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][4][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][5][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][5][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][5][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][5][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][5][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][5][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][5][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][5][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][6][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][6][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][6][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][6][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][6][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][6][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][6][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][6][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][7][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][7][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][7][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][7][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][7][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][7][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][7][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][7][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][8][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][8][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][8][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][8][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][8][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][8][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][8][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][8][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][9][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][9][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][9][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][9][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][9][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][9][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][9][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][9][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][10][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][10][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][10][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][10][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][10][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][10][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][10][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][10][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[0][11][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[0][11][1][0][1]=0.00468037; 
 data_Endcap_InnerST_PhiSigmas[2][11][1][0][0]=0.09298708; data_Endcap_InnerST_PhiSigmas[2][11][1][0][1]=0.00468037; 
  data_Endcap_InnerST_Param[0][11][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[0][11][1][0][1]=-0.00447655; 
 data_Endcap_InnerST_Param[2][11][1][0][0]=-0.23349855; data_Endcap_InnerST_Param[2][11][1][0][1]=-0.00447655; 
  data_Endcap_InnerST_PhiSigmas[1][0][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][0][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][0][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][0][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][0][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][0][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][0][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][0][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][1][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][1][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][1][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][1][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][1][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][1][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][1][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][1][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][2][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][2][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][2][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][2][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][2][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][2][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][2][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][2][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][3][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][3][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][3][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][3][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][3][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][3][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][3][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][3][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][4][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][4][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][4][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][4][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][4][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][4][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][4][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][4][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][5][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][5][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][5][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][5][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][5][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][5][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][5][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][5][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][6][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][6][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][6][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][6][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][6][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][6][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][6][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][6][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][7][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][7][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][7][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][7][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][7][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][7][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][7][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][7][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][8][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][8][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][8][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][8][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][8][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][8][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][8][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][8][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][9][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][9][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][9][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][9][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][9][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][9][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][9][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][9][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][10][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][10][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][10][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][10][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][10][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][10][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][10][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][10][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[1][11][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[1][11][1][0][1]=0.00666912; 
 data_Endcap_InnerST_PhiSigmas[3][11][1][0][0]=0.04069992; data_Endcap_InnerST_PhiSigmas[3][11][1][0][1]=0.00666912; 
  data_Endcap_InnerST_Param[1][11][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[1][11][1][0][1]=-0.00089065; 
 data_Endcap_InnerST_Param[3][11][1][0][0]=-0.20728864; data_Endcap_InnerST_Param[3][11][1][0][1]=-0.00089065; 
  data_Endcap_InnerST_PhiSigmas[0][0][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][0][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][0][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][0][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][0][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][0][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][0][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][0][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][1][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][1][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][1][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][1][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][1][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][1][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][1][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][1][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][2][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][2][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][2][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][2][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][2][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][2][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][2][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][2][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][3][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][3][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][3][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][3][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][3][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][3][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][3][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][3][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][4][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][4][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][4][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][4][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][4][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][4][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][4][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][4][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][5][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][5][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][5][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][5][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][5][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][5][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][5][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][5][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][6][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][6][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][6][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][6][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][6][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][6][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][6][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][6][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][7][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][7][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][7][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][7][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][7][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][7][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][7][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][7][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][8][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][8][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][8][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][8][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][8][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][8][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][8][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][8][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][9][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][9][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][9][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][9][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][9][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][9][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][9][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][9][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][10][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][10][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][10][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][10][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][10][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][10][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][10][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][10][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[0][11][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[0][11][1][1][1]=0.00470356; 
 data_Endcap_InnerST_PhiSigmas[2][11][1][1][0]=0.09291424; data_Endcap_InnerST_PhiSigmas[2][11][1][1][1]=0.00470356; 
  data_Endcap_InnerST_Param[0][11][1][1][0]=0.25992437; data_Endcap_InnerST_Param[0][11][1][1][1]=0.00204247; 
 data_Endcap_InnerST_Param[2][11][1][1][0]=0.25992437; data_Endcap_InnerST_Param[2][11][1][1][1]=0.00204247; 
  data_Endcap_InnerST_PhiSigmas[1][0][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][0][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][0][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][0][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][0][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][0][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][0][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][0][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][1][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][1][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][1][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][1][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][1][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][1][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][1][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][1][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][2][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][2][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][2][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][2][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][2][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][2][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][2][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][2][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][3][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][3][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][3][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][3][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][3][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][3][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][3][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][3][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][4][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][4][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][4][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][4][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][4][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][4][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][4][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][4][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][5][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][5][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][5][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][5][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][5][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][5][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][5][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][5][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][6][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][6][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][6][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][6][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][6][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][6][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][6][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][6][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][7][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][7][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][7][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][7][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][7][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][7][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][7][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][7][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][8][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][8][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][8][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][8][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][8][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][8][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][8][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][8][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][9][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][9][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][9][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][9][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][9][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][9][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][9][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][9][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][10][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][10][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][10][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][10][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][10][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][10][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][10][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][10][1][1][1]=0.00096603; 
  data_Endcap_InnerST_PhiSigmas[1][11][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[1][11][1][1][1]=0.00709018; 
 data_Endcap_InnerST_PhiSigmas[3][11][1][1][0]=0.03121386; data_Endcap_InnerST_PhiSigmas[3][11][1][1][1]=0.00709018; 
  data_Endcap_InnerST_Param[1][11][1][1][0]=0.21324215; data_Endcap_InnerST_Param[1][11][1][1][1]=0.00096603; 
 data_Endcap_InnerST_Param[3][11][1][1][0]=0.21324215; data_Endcap_InnerST_Param[3][11][1][1][1]=0.00096603; 

double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2]; 
 data_Endcap_InnerST_EtaSigmas[0][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[0][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[1][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[0][1][0][0][1]=0.00622663; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[1][1][0][0][1]=0.00622663; 
  data_Endcap_InnerST_EtaSigmas[0][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[0][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][0][0]=0.02511008; data_Endcap_InnerST_EtaSigmas[1][0][0][0][1]=0.00583879; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[0][1][0][0][1]=0.00622663; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][0][0]=0.02509277; data_Endcap_InnerST_EtaSigmas[1][1][0][0][1]=0.00622663; 
  data_Endcap_InnerST_EtaSigmas[0][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[0][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[1][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[0][1][0][1][1]=0.00660609; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[1][1][0][1][1]=0.00660609; 
  data_Endcap_InnerST_EtaSigmas[0][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[0][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[1][0][0][1][0]=0.02309201; data_Endcap_InnerST_EtaSigmas[1][0][0][1][1]=0.00597916; 
 data_Endcap_InnerST_EtaSigmas[0][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[0][1][0][1][1]=0.00660609; 
 data_Endcap_InnerST_EtaSigmas[1][1][0][1][0]=0.01454231; data_Endcap_InnerST_EtaSigmas[1][1][0][1][1]=0.00660609; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[0][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[1][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[0][1][1][0][1]=0.00644456; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[1][1][1][0][1]=0.00644456; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[0][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][0][0]=0.01991255; data_Endcap_InnerST_EtaSigmas[1][0][1][0][1]=0.00623431; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[0][1][1][0][1]=0.00644456; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][0][0]=0.01763423; data_Endcap_InnerST_EtaSigmas[1][1][1][0][1]=0.00644456; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[0][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[1][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[0][1][1][1][1]=0.00614263; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[1][1][1][1][1]=0.00614263; 
  data_Endcap_InnerST_EtaSigmas[0][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[0][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[1][0][1][1][0]=0.02863699; data_Endcap_InnerST_EtaSigmas[1][0][1][1][1]=0.00588306; 
 data_Endcap_InnerST_EtaSigmas[0][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[0][1][1][1][1]=0.00614263; 
 data_Endcap_InnerST_EtaSigmas[1][1][1][1][0]=0.02561595; data_Endcap_InnerST_EtaSigmas[1][1][1][1][1]=0.00614263;
*/

///////////////////////////////////// CODE //////////////////////////////////////////
bool 
MuonBackExtrapolator::give_eta_phi_at_vertex(
             double pt, double eta, double phi,
             bool barrel,bool triggerST,bool /*aligned*/,bool dataset, double& extEta,
             double& sigmaEta,double& extPhi,double& sigmaPhi,double PT,
             double data_Barrel_Param[2][2][2],
             double data_Barrel_Sigmas[2][2][2][2],
             double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
             double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
             double data_Endcap_InnerST_Param[4][12][2][2][2],
             double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
             double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2])
{

    pt = retune_pt(pt, dataset);
    if(PT==0.) PT = pt;
    else PT = retune_pt(PT, dataset);
    
    int side = (eta      >  0.)? 0 : 1;
    int sign = (pt       >  0.)? 0 : 1;
    int ptbi = (fabs(pt) >= 6.)? 0 : 1;
        
    if (phi<0.) phi += 2.0*M_PI;

    
    
    // BARREL
    if (barrel) {
        double (*param)[2][2]     = 0;
        double (*sigmas)[2][2][2] = 0;

        param  = data_Barrel_Param;
        sigmas = data_Barrel_Sigmas;

        extEta = eta;
	
	float deltaPhi = param[side][sign][0]*(1./fabs(pt)) + param[side][sign][1];
	extPhi = phi - deltaPhi;
	if(extPhi>M_PI) extPhi -= 2*M_PI;
	
	sigmaEta = sigmas[side][sign][0][0]*(1./fabs(PT)) + sigmas[side][sign][0][1];
	
	sigmaPhi = sigmas[side][sign][1][0]*(1./fabs(PT)) + sigmas[side][sign][1][1];

        return true;
    }
    
    
    //ENDCAP
    int phib = 0;
    
    float Phi = phi;
    float MODULE = (2.*M_PI)/8.;
    while(Phi>MODULE) Phi -= MODULE;
    float CELL = MODULE/12.;
    while(Phi>CELL) {Phi -= CELL; phib++;}
    
    
    int etab1 = 0;
    if (fabs(eta) >= 1.2 && fabs(eta) < 1.5) {
        etab1 = 1;
    }
    if (fabs(eta) >= 1.5 && fabs(eta) < 2.0) {
        etab1 = 2;
    }
    if (fabs(eta) >= 2.0) {
        etab1 = 3;
    }
    
    
    int etab2 = 0;
    if (fabs(eta) >= 1.6) {
        etab2 = 1;
    }

    
    double deltaEta = 0.;
    double deltaPhi = 0.;
    
    
    
    if(triggerST) {
    
        double (*param)[12][2][2][2][2] = 0;
        double (*sigmas)[12][2][2][2][2] = 0;
    
        param =  data_Endcap_TriggerST_Param;
	sigmas = data_Endcap_TriggerST_Sigmas;
	
	deltaEta = param[etab1][phib][side][sign][0][0]*(1./fabs(pt))+param[etab1][phib][side][sign][0][1];
        deltaPhi = param[etab1][phib][side][sign][1][0]*(1./fabs(pt))+param[etab1][phib][side][sign][1][1];    
	sigmaEta = sigmas[etab1][phib][side][sign][0][0]*(1./fabs(PT)) + sigmas[etab1][phib][side][sign][0][1];
        sigmaPhi = sigmas[etab1][phib][side][sign][1][0]*(1./fabs(PT)) + sigmas[etab1][phib][side][sign][1][1];
    
    } else {
    
        double (*param)[12][2][2][2] = 0;
        double (*sigmaP)[12][2][2][2] = 0;
	double (*sigmaE)[2][2][2][2] = 0;

        param  = data_Endcap_InnerST_Param;
        sigmaP = data_Endcap_InnerST_PhiSigmas;
	sigmaE = data_Endcap_InnerST_EtaSigmas;
    
	deltaPhi =  (param[ptbi*2+etab2][phib][side][sign][0]*(1./fabs(pt))+ param[ptbi*2+etab2][phib][side][sign][1]);    
	sigmaEta = sigmaE[ptbi][etab2][side][sign][0]*(1./fabs(PT)) + sigmaE[ptbi][etab2][side][sign][1];
        sigmaPhi = sigmaP[ptbi*2+etab2][phib][side][sign][0]*(1./fabs(PT)) + sigmaP[ptbi*2+etab2][phib][side][sign][1];

    }
    

    
    extEta = eta - deltaEta; 
    extPhi = phi - deltaPhi;
    
    if(extPhi> M_PI) extPhi -= 2.0*M_PI;

    return true;
}



bool 
MuonBackExtrapolator::give_eta_phi_at_tuned_vertex(
    double pt,double zetaId,double zetaMu,double eta,double phi, 
    bool barrel,bool triggerST,bool /*aligned*/, bool dataset,
    double& extEta,double& sigmaEta,double& extPhi,double& sigmaPhi,double PT,
             double data_Barrel_Param[2][2][2],
             double data_Barrel_Sigmas[2][2][2][2],
             double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
             double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
             double data_Endcap_InnerST_Param[4][12][2][2][2],
             double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
             double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2])
{

    pt = retune_pt(pt, dataset);
    if(PT==0.) PT = pt;
    else PT = retune_pt(PT, dataset);

    int side = (eta      >  0.)? 0 : 1;
    int sign = (pt       >  0.)? 0 : 1;
    int ptbi = (fabs(pt) >= 6.)? 0 : 1;    
    
    if (phi<0.) phi += 2.0*M_PI;
    
    
    // BARREL
    if (barrel) {
        double (*param)[2][2] = 0;
        double (*sigmas)[2][2][2] = 0;

        param  = data_Barrel_Param;
        sigmas = data_Barrel_Sigmas;
        
	extEta = MuonBackExtrapolator::retune_vtx(zetaId,zetaMu,eta);
	
	float deltaPhi = param[side][sign][0]*(1./fabs(pt)) + param[side][sign][1];
	extPhi = phi - deltaPhi;
	if(extPhi>M_PI) extPhi -= 2*M_PI;
	
	sigmaEta = sigmas[side][sign][0][0]*(1./fabs(PT)) + sigmas[side][sign][0][1];
	
	sigmaPhi = sigmas[side][sign][1][0]*(1./fabs(PT)) + sigmas[side][sign][1][1];

        return true;
    }
    
    
    //ENDCAP
    int phib = 0;
    
    float Phi = phi;
    float MODULE = (2.*M_PI)/8.;
    while(Phi>MODULE) Phi -= MODULE;
    float CELL = MODULE/12.;
    while(Phi>CELL) {Phi -= CELL; phib++;}
    
    
    eta = MuonBackExtrapolator::retune_vtx(zetaId,zetaMu,eta);
    
    int etab1 = 0;
    if (fabs(eta) >= 1.2 && fabs(eta) < 1.5) {
        etab1 = 1;
    }
    if (fabs(eta) >= 1.5 && fabs(eta) < 2.0) {
        etab1 = 2;
    }
    if (fabs(eta) >= 2.0) {
        etab1 = 3;
    }
    
    
    int etab2 = 0;
    if (fabs(eta) >= 1.6) {
        etab2 = 1;
    }

    
    double deltaEta = 0.;
    double deltaPhi = 0.;
    
    
    
    if(triggerST) {
    
        double (*param)[12][2][2][2][2] = 0;
        double (*sigmas)[12][2][2][2][2] = 0;

        param =  data_Endcap_TriggerST_Param;
	sigmas = data_Endcap_TriggerST_Sigmas;
	
	deltaEta = param[etab1][phib][side][sign][0][0]*(1./fabs(pt))+param[etab1][phib][side][sign][0][1];
        deltaPhi = param[etab1][phib][side][sign][1][0]*(1./fabs(pt))+param[etab1][phib][side][sign][1][1];    
	sigmaEta = sigmas[etab1][phib][side][sign][0][0]*(1./fabs(PT)) + sigmas[etab1][phib][side][sign][0][1];
        sigmaPhi = sigmas[etab1][phib][side][sign][1][0]*(1./fabs(PT)) + sigmas[etab1][phib][side][sign][1][1];
    
    } else {
    
        double (*param)[12][2][2][2] = 0;
        double (*sigmaP)[12][2][2][2] = 0;
	double (*sigmaE)[2][2][2][2] = 0;
    
        param  = data_Endcap_InnerST_Param;
	sigmaP = data_Endcap_InnerST_PhiSigmas;
	sigmaE = data_Endcap_InnerST_EtaSigmas;
	
	deltaPhi =  (param[ptbi*2+etab2][phib][side][sign][0]*(1./fabs(pt))+param[ptbi*2+etab2][phib][side][sign][1]);    
	sigmaEta = sigmaE[ptbi][etab2][side][sign][0]*(1./fabs(PT)) + sigmaE[ptbi][etab2][side][sign][1];
        sigmaPhi = sigmaP[ptbi*2+etab2][phib][side][sign][0]*(1./fabs(PT)) + sigmaP[ptbi*2+etab2][phib][side][sign][1];

    }
    

    
    extEta = eta - deltaEta; 
    extPhi = phi - deltaPhi;
    
    if(extPhi> M_PI) extPhi -= 2.0*M_PI;

    return true;
}



bool MuonBackExtrapolator::idTrack_in_loose_window (double pt,int address,
                         double radius,double etaMu,double phiMu,double etaId,
	                 double phiId,bool aligned,bool dataset,double winPt,double weight,
             double data_Barrel_Param[2][2][2],
             double data_Barrel_Sigmas[2][2][2][2],
             double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
             double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
             double data_Endcap_InnerST_Param[4][12][2][2][2],
             double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
             double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2])
{
    weight = fabs(weight);
    //if (weight>1.) weight = 1.;
    
    if(fabs(winPt) > 40.) winPt = 40.;
    if(fabs(winPt) < 4. ) winPt = 4.;
    
    double etaExtr  = 0.;
    double sigmaEta = 0.;
    double phiExtr  = 0.;
    double sigmaPhi = 0.;
    
    if(!MuonBackExtrapolator::give_eta_phi_at_vertex( pt, etaMu, phiMu, 
                               (address!=-1), (radius <=10.), aligned, dataset, 
                                etaExtr, sigmaEta, phiExtr, sigmaPhi, winPt,
             data_Barrel_Param,
             data_Barrel_Sigmas,
             data_Endcap_TriggerST_Param,
             data_Endcap_TriggerST_Sigmas,
             data_Endcap_InnerST_Param,
             data_Endcap_InnerST_PhiSigmas,
             data_Endcap_InnerST_EtaSigmas))
        return false;
	
    double deta = 100.;
    double dphi = 100.;
    
    if ( etaId *  etaExtr >= 0. ) deta = etaExtr - etaId;  
    else deta = fabs(etaExtr)+ fabs(etaId);
    
    if (phiExtr < 0.) phiExtr += 2.0*M_PI;
    if (phiId   < 0.) phiId   += 2.0*M_PI;
    dphi = fabs(phiExtr - phiId);
    if (dphi>M_PI) dphi = fabs(dphi-2.0*M_PI);
    
   
    double WinDeta = (radius<=10.)? 3.7*weight*sigmaEta : 3.0*weight*sigmaEta;
    double WinDphi = (radius<=10.)? 3.7*weight*sigmaPhi : 3.0*weight*sigmaPhi;
    if(radius>10.&&address==-1) WinDphi *= 2.5;

        
    if ( std::abs(deta) < WinDeta && std::abs(dphi) < WinDphi) return true;
    
    return false;
}



bool 
MuonBackExtrapolator::idTrack_in_tight_window (double pt,int address,double radius,double zetaId,
                         double zetaMu,double etaMu, double phiMu,double etaId,
			 double phiId,bool aligned, bool dataset, double winPt,double weight,
             double data_Barrel_Param[2][2][2],
             double data_Barrel_Sigmas[2][2][2][2],
             double data_Endcap_TriggerST_Param[4][12][2][2][2][2],
             double data_Endcap_TriggerST_Sigmas[4][12][2][2][2][2],
             double data_Endcap_InnerST_Param[4][12][2][2][2],
             double data_Endcap_InnerST_PhiSigmas[4][12][2][2][2],
             double data_Endcap_InnerST_EtaSigmas[2][2][2][2][2])
{
    weight = fabs(weight);
    //if (weight>1.) weight = 1.;
    
    if(fabs(winPt) > 40.) winPt = 40.;
    if(fabs(winPt) < 4. ) winPt = 4.;
    
    double etaLooseExtr  = 0.;
    double sigmaLooseEta = 0.;
    double phiLooseExtr  = 0.;
    double sigmaLoosePhi = 0.;
    
    if(!MuonBackExtrapolator::give_eta_phi_at_tuned_vertex( pt, zetaId, zetaMu,
               etaMu, phiMu, (address!=-1), (radius <=10.), aligned, dataset, 
               etaLooseExtr, sigmaLooseEta, phiLooseExtr, sigmaLoosePhi, winPt,
             data_Barrel_Param,
             data_Barrel_Sigmas,
             data_Endcap_TriggerST_Param,
             data_Endcap_TriggerST_Sigmas,
             data_Endcap_InnerST_Param,
             data_Endcap_InnerST_PhiSigmas,
             data_Endcap_InnerST_EtaSigmas))
        return false;

    double etaTightExtr  = 0.;
    double sigmaTightEta = 0.;
    double phiTightExtr  = 0.;
    double sigmaTightPhi = 0.;

    if(!MuonBackExtrapolator::give_eta_phi_at_tuned_vertex( pt, zetaId, zetaMu,
               etaMu, phiMu, (address!=-1), (radius <=10.), aligned, dataset, 
               etaTightExtr, sigmaTightEta, phiTightExtr, sigmaTightPhi, 0.,
             data_Barrel_Param,
             data_Barrel_Sigmas,
             data_Endcap_TriggerST_Param,
             data_Endcap_TriggerST_Sigmas,
             data_Endcap_InnerST_Param,
             data_Endcap_InnerST_PhiSigmas,
             data_Endcap_InnerST_EtaSigmas))
        return false;

	
    //double detaLoose = 100.;
    //double dphiLoose = 100.;
    double detaTight = 100.;
    double dphiTight = 100.;
    
    
    //if ( etaId *  etaLooseExtr >= 0. ) detaLoose = etaLooseExtr - etaId;  
    //else detaLoose = fabs(etaLooseExtr)+ fabs(etaId);
    
    if ( etaId *  etaTightExtr >= 0. ) detaTight = etaTightExtr - etaId;  
    else detaTight = fabs(etaTightExtr)+ fabs(etaId);
    
    
    if (phiLooseExtr < 0.) phiLooseExtr += 2.0*M_PI;
    if (phiTightExtr < 0.) phiTightExtr += 2.0*M_PI;
    if (phiId   < 0.) phiId   += 2.0*M_PI;
    
    //dphiLoose = fabs(phiLooseExtr - phiId);
    //if (dphiLoose>M_PI) dphiLoose = fabs(dphiLoose-2.0*M_PI);
    
    dphiTight = fabs(phiTightExtr - phiId);
    if (dphiTight>M_PI) dphiTight = fabs(dphiTight-2.0*M_PI);
    
    
//    double WinDetaLoose = (radius<=10.)? 3.7*weight*sigmaLooseEta : 3.0*weight*sigmaLooseEta;
//    double WinDphiLoose = (radius<=10.)? 3.7*weight*sigmaLoosePhi : 3.0*weight*sigmaLoosePhi;
    
    
    double WinDetaTight = (radius<=10.)? 3.7*weight*sigmaTightEta : 3.0*weight*sigmaTightEta;
    double WinDphiTight = (radius<=10.)? 3.7*weight*sigmaTightPhi : 3.0*weight*sigmaTightPhi;
    
    if(radius>10.&&address==-1) {
        WinDphiTight *= 2.5;
//        WinDphiLoose *= 2.5;
    }
    
    if(address!=-1) {
        double mul = 1.;
	if(fabs(pt)>6.) mul = 1.+ (2./34.)*(fabs(pt)-6.);
	
        if ( fabs(detaTight) < WinDetaTight*mul && 
	     fabs(dphiTight) < WinDphiTight*mul) return true;
    } else {
        if (radius<=10.) {
	    double mul = 1. + (3./34.)*(fabs(pt)-6.);
	    if ( fabs(detaTight) < WinDetaTight*1.1 && 
	         fabs(dphiTight) < WinDphiTight*mul) return true;
	} else {
	    double mul = 1. + (1./34.)*(fabs(pt)-6.);
	    
            if ( fabs(detaTight) < WinDetaTight*1.2 && 
	         fabs(dphiTight) < fabs(WinDphiTight)*mul) return true;
        }
    }
    
    return false;
}


double
MuonBackExtrapolator::retune_vtx(double IdZeta, double MuonZeta, double eta) 
{
    if (MuonZeta==0.) return eta;
    if(fabs(MuonZeta)<2000.) IdZeta/=10.;
    double theta  = atan(exp(-fabs(eta)))*2.;
    double radius = tan(theta)*fabs(MuonZeta);
    double zeta   = MuonZeta - IdZeta;
    double signZ  = zeta/fabs(zeta);
    theta = atan2(radius,fabs(zeta));
    return (-log(tan(theta/2.)))*signZ;
}

double
MuonBackExtrapolator::retune_pt(double pt, bool /*dataset*/) 
{
    double tuned_pt = pt;
    
    if(pt < 0.)
    {
        if(pt <= -40.)  tuned_pt = -40.;
	if(pt >= -4. )  tuned_pt = -4.;
    }
    else
    {
        if(pt >= 40.)  tuned_pt = 40.;
	if(pt <= 4. )  tuned_pt = 4.;
    }
    
    return tuned_pt;
}
