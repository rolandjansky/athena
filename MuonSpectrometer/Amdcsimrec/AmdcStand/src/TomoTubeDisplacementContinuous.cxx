/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TomoTubeDisplacement.h"
#include "TomoTubeDisplacementContinuous.h"

#include<cmath>
#include "AmdcStand/AmdcsimrecStand.h"

void tomotubedisplacementcontinuous_(
                         int Iside_In,
		         int optionflag,
		         std::string TheStationName, int jff, int jzz, int MulLay,
                         double& /*szt_s*/    , double& szt_z        , double& szt_t        , 
                         double& Delta_szt_s_c, double& Delta_szt_z_c, double& Delta_szt_t_c
){

//Tomo data variables 
  double XtomoDela      = 0. ;
  int    XtomoNberTube  = 0  ;
  double XtomoZpit      = 0. ;
  double XtomoYpit      = 0. ;
  double XtomoDelz      = 0. ;
  double XtomoDely      = 0. ;
  int    XtomoNberLayer = 0  ;
  double nominalzpitch  = 0. ;
  double nominalypitch  = 0. ;

//Tomo reprocessed data variables
  double CosRotM1             = 0. ;
  double SinRot               = 0. ;
  double szt_z_tube           = 0. ;
  double szt_t_tube           = 0. ;  
  double szt_z_Rotation_Fixed = 0. ;
  double szt_t_Rotation_Fixed = 0. ;
  double szt_t_PitchY_Fixed   = 0. ;
  double szt_z_PitchZ_Fixed   = 0. ;  
  double Ratio_PicthZ         = 1. ; double Delta_PicthZ_Relative = 0. ;
  double Ratio_PicthY         = 1. ; double Delta_PicthY_Relative = 0. ;
  double Delta_ShiftZ         = 0. ;
  double Delta_ShiftY         = 0. ;

  Delta_szt_s_c = 0. ;
  Delta_szt_z_c = 0. ;
  Delta_szt_t_c = 0. ;

  double Delta_szt_z = 0. ;
  double Delta_szt_t = 0. ;

  double Delta_szt_z_n = 0. ;
  double Delta_szt_t_n = 0. ;
  double Delta_szt_z_p = 0. ;
  double Delta_szt_t_p = 0. ;

  for (int Iside = 0 ; Iside <2 ; Iside++){
    int Side = 2* Iside - 1 ; 

    int TomoGetDataStatus = 0 ; 
    tomogetdata_(
                TomoGetDataStatus,
		optionflag,
		TheStationName, jff, jzz, MulLay, Side ,
		XtomoDela , XtomoNberTube , XtomoZpit , XtomoYpit , XtomoDelz , XtomoDely , XtomoNberLayer,
		nominalzpitch , nominalypitch
               );
    if (TomoGetDataStatus==0) continue ;

    szt_z_tube = szt_z ;
    szt_t_tube = szt_t ;
    
    szt_z_PitchZ_Fixed = nominalzpitch/4;
    szt_t_PitchY_Fixed = 0. ; if ( MulLay==1 ) szt_t_PitchY_Fixed = ( XtomoNberLayer - 1 ) * nominalypitch ;// due to different layer nbering

    tomoprocessdata_(
                 XtomoDela , XtomoNberTube , XtomoZpit , XtomoYpit , XtomoDelz , XtomoDely ,
                 nominalzpitch , nominalypitch,
                 Ratio_PicthZ , Delta_PicthZ_Relative ,
                 Ratio_PicthY , Delta_PicthY_Relative ,
                 CosRotM1 ,  SinRot , Delta_ShiftZ , szt_z_Rotation_Fixed  ,
                                      Delta_ShiftY , szt_t_Rotation_Fixed
                );

    if (MulLay==1) Delta_PicthY_Relative = -Delta_PicthY_Relative ;// due to different layer nbering
    
    tomotubedisplacementbase_(
                Delta_szt_z , Delta_szt_t ,
                szt_z_tube,  szt_t_tube, 
                szt_z_PitchZ_Fixed , Ratio_PicthZ , Delta_PicthZ_Relative ,
                szt_t_PitchY_Fixed , Ratio_PicthY , Delta_PicthY_Relative ,
                CosRotM1 ,  SinRot , Delta_ShiftZ , szt_z_Rotation_Fixed  ,
                                     Delta_ShiftY , szt_t_Rotation_Fixed
               );
                            
    if (Side<0){
      Delta_szt_z_n = Delta_szt_z ;
      Delta_szt_t_n = Delta_szt_t ;
    }else{
      Delta_szt_z_p = Delta_szt_z ;
      Delta_szt_t_p = Delta_szt_t ;
    }  
                           
  }
  if (  Iside_In<0){
    Delta_szt_z_c = Delta_szt_z_n ;
    Delta_szt_t_c = Delta_szt_t_n ;
  }else{
    Delta_szt_z_c = Delta_szt_z_p ;
    Delta_szt_t_c = Delta_szt_t_p ;
  }  

}
