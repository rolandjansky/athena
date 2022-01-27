/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCSTAND_TOMOTUBEDISPLACEMENT_H
#define AMDCSTAND_TOMOTUBEDISPLACEMENT_H

#include <string>

void tomotubedisplacement_(
		         int& optionflag,
		         const std::string& TheStationName, int& jff, int& jzz, int& MulLay, int& JLay, int& Tube,
		         double& Delta_szt_z_n , double& Delta_szt_t_n , 
		         double& Delta_szt_z_p , double& Delta_szt_t_p
		        );

void tomotubedisplacementbase_(
     double& Delta_szt_z ,  double& Delta_szt_t ,
     double& szt_z, double& szt_t, 
     double& szt_z_PitchZ_Fixed , double& Ratio_PicthZ , double& Delta_PicthZ_Relative ,
     double& szt_t_PitchY_Fixed , double& Ratio_PicthY , double& Delta_PicthY_Relative ,
     double& CosRotM1 , double& SinRot , double& Delta_ShiftZ , double& szt_z_Rotation_Fixed ,
                                         double& Delta_ShiftY , double& szt_t_Rotation_Fixed
);

void tomoprocessdata_(
    double& XtomoDela , int& XtomoNberTube, double& XtomoZpit, double& XtomoYpit, double& XtomoDelz, double& XtomoDely,
    double& nominalzpitch , double& nominalypitch ,
    double& Ratio_PicthZ , double& Delta_PicthZ_Relative ,
    double& Ratio_PicthY , double& Delta_PicthY_Relative ,
    double& CosRotM1 , double& SinRot , double& Delta_ShiftZ , double& szt_z_Rotation_Fixed ,
                                        double& Delta_ShiftY , double& szt_t_Rotation_Fixed
);

void tomogetdata_(
    int& Status,
    int& optionflag,
    const std::string& TheStationName, int& jff, int& jzz, int& MulLay, int& Side,
    double& XtomoDela , int& XtomoNberTube, double& XtomoZpit, double& XtomoYpit, double& XtomoDelz, double& XtomoDely, int& XtomoNberLayer,
    double& nominalzpitch, double& nominalypitch
);

#endif
