/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SUSYPhysValMonitoring/rel17_SmearingClass.h"

/** scale parameters **/
const double SmearingClass::scale_CB[4] = {1.0000, 0.9933, 1.0037, 1.0033};
const double SmearingClass::scaleSyst_CB[4] = {0.0006 , 0.0013, 0.0021, 0.0022};

/** Smearing Parameters rel.16 (Muid) **/
const double SmearingClass::ID_MS[4]       = {0.0002,0.0018,0.0006,0.0};
const double SmearingClass::ID_AL[4]       = {0.000255,0.00053,0.000819,0.001177};
const double SmearingClass::ID_ALTAN[4]    = {0,0,0,0.0000593};
const double SmearingClass::MS_MS[4]       = {0.02428,0.0678,0.0362,0.0237};
const double SmearingClass::MS_AL[4]       = {0.000191,0.00019,0.00012,0.000669};
/** Error for Smearing Parameters rel.16 (Muid) **/
const double SmearingClass::E_ID_MS[4]       = {0.0037,0.0062,0.0057,0.0071};
const double SmearingClass::E_ID_AL[4]       = {0.000037,0.00013,0.000071,0.000050};
const double SmearingClass::E_ID_ALTAN[4]    = {0,0,0,0.0000032};
const double SmearingClass::E_MS_MS[4]       = {0.00088,0.0027,0.0019,0.0045};
const double SmearingClass::E_MS_AL[4]       = {0.000028,0.00012,0.00011,0.000091};
/** sist error **/
const double SmearingClass::S_ID_MS[4]       = {0,0.0002,0,0};
const double SmearingClass::S_ID_AL[4]       = {0.000015,0.00014,0.000144,0.00014};
const double SmearingClass::S_ID_ALTAN[4]    = {0,0,0,0.0000072};
const double SmearingClass::S_MS_MS[4]       = {0.00019,0.0016,0.0028,0.0108};
const double SmearingClass::S_MS_AL[4]       = {0.000009,0.00004,0.00008,0.000167};

/** MC Parameters rel.16 (Muid) **/
const double SmearingClass::MC_ID_MS[4]    = {0.016,0.026,0.034,0.048};
const double SmearingClass::MC_ID_AL[4]    = {0.00029,0.0003,0.00042,0.00094};
const double SmearingClass::MC_ID_ALTAN[4] = {0,0,0,0.000063};
const double SmearingClass::MC_MS_CALO[4]  = {0.23,0,0,0.17};
const double SmearingClass::MC_MS_MS[4]    = {0.027,0.054,0.033,0.027};
const double SmearingClass::MC_MS_AL[4]    = {0.00013,0.00023,0.000093,0.000074};
/***********************************/
/** correlations                                   rho12, rho13, rho14,rho23,  rho24, rho34 **/
const double SmearingClass::CorrMat[4][6] = { {-0.937, 0.481,-0.095,-0.455, 0.077, 0.566}, 
					      {-0.650,-0.043,-0.767,-0.189, 0.470,-0.241}, 
					      { 0.077,-0.199,-0.553, 0.057,-0.345,-0.394},
					      {-0.326,-0.435,-0.301,-0.018,-0.375,-0.246}};

 


/** correlations                                   rho12, rho13, rho14,rho23,  rho24, rho34 **/
const double SmearingClass::CorrMatTan[4][6] = { {-0.937, 0.481,-0.095,-0.455, 0.077, 0.566}, 
					         {-0.650,-0.043,-0.767,-0.189, 0.470,-0.241}, 
					         { 0.077,-0.199,-0.553, 0.057,-0.345,-0.394},
					         {-0.326,-0.435,-0.301,-0.018,-0.375,-0.246}};

