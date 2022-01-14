/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TomoTubeDisplacement.h"

#include<cmath>
#include "AmdcStand/AmdcsimrecStand.h"

void tomotubedisplacement_(
		         int& optionflag,
		         const std::string& TheStationName, int& jff, int& jzz, int& MulLay, int& JLay, int& Tube,
		         double& Delta_szt_z_n , double& Delta_szt_t_n , 
		         double& Delta_szt_z_p , double& Delta_szt_t_p
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

  double Delta_szt_z = 0. ;
  double Delta_szt_t = 0. ;

  Delta_szt_z_n = 0. ;
  Delta_szt_t_n = 0. ;
  Delta_szt_z_p = 0. ;
  Delta_szt_t_p = 0. ;

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
    if (TomoGetDataStatus!=1) continue ;

    szt_z_tube = ( Tube - 1 ) * nominalzpitch ; if ( JLay%2==0 ) szt_z_tube += nominalzpitch/2;
    szt_t_tube = ( JLay - 1 ) * nominalypitch ;
    
    szt_z_PitchZ_Fixed = 0. ; if ( JLay%2==0 ) szt_z_PitchZ_Fixed += nominalzpitch/2;
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

}

void tomotubedisplacementbase_(
     double& Delta_szt_z ,  double& Delta_szt_t ,
     double& szt_z, double& szt_t, 
     double& szt_z_PitchZ_Fixed , double& Ratio_PicthZ , double& Delta_PicthZ_Relative ,
     double& szt_t_PitchY_Fixed , double& Ratio_PicthY , double& Delta_PicthY_Relative ,
     double& CosRotM1 , double& SinRot , double& Delta_ShiftZ , double& szt_z_Rotation_Fixed ,
                                         double& Delta_ShiftY , double& szt_t_Rotation_Fixed
)
{

  Delta_szt_z  = ( szt_z - szt_z_PitchZ_Fixed ) * Delta_PicthZ_Relative ;
    
  Delta_szt_z += Delta_ShiftZ  
    + CosRotM1 * ( szt_z * Ratio_PicthZ - szt_z_Rotation_Fixed )
    + SinRot   * ( szt_t * Ratio_PicthY - szt_t_Rotation_Fixed ) ;
    
  Delta_szt_t  = ( szt_t - szt_t_PitchY_Fixed ) * Delta_PicthY_Relative ;

  Delta_szt_t +=  Delta_ShiftY  
    + CosRotM1 * ( szt_t * Ratio_PicthY - szt_t_Rotation_Fixed )
    - SinRot   * ( szt_z * Ratio_PicthZ - szt_z_Rotation_Fixed ) ;
            
}


void tomoprocessdata_(
    double& XtomoDela , int& XtomoNberTube, double& XtomoZpit, double& XtomoYpit, double& XtomoDelz, double& XtomoDely,
    double& nominalzpitch, double& nominalypitch,
    double& Ratio_PicthZ , double& Delta_PicthZ_Relative ,
    double& Ratio_PicthY , double& Delta_PicthY_Relative ,
    double& CosRotM1 , double& SinRot , double& Delta_ShiftZ , double& szt_z_Rotation_Fixed ,
                                        double& Delta_ShiftY , double& szt_t_Rotation_Fixed
){

  CosRotM1 = cos(XtomoDela) - 1. ;
  SinRot   = sin(XtomoDela);

  szt_z_Rotation_Fixed = XtomoNberTube * XtomoZpit/2 ;
  szt_t_Rotation_Fixed = 0. ;
 
  Ratio_PicthZ = XtomoZpit/nominalzpitch ; Delta_PicthZ_Relative = Ratio_PicthZ - 1. ;
  Ratio_PicthY = XtomoYpit/nominalypitch ; Delta_PicthY_Relative = Ratio_PicthY - 1. ;

  Delta_ShiftZ = XtomoDelz ;
  Delta_ShiftY = XtomoDely ;

}

void tomogetdata_(
    int& Status,
    int& optionflag,
    const std::string& TheStationName, int& jff, int& jzz, int& MulLay, int& Side,
    double& XtomoDela , int& XtomoNberTube, double& XtomoZpit, double& XtomoYpit, double& XtomoDelz, double& XtomoDely, int& XtomoNberLayer,
    double& nominalzpitch, double& nominalypitch
){

  Status = 0 ;

//Take X tomo data
  AmdcsimrecStand aAmdcsimrecStand;
  int XtomoEntryRank = aAmdcsimrecStand.GetXtomoEntryRank(TheStationName,jff,jzz) ;
  if ( XtomoEntryRank == -1 ) return ; 
  
  
  int XtomoPassed = aAmdcsimrecStand.GetXtomoPassed    (XtomoEntryRank) ;
  XtomoNberLayer  = aAmdcsimrecStand.GetXtomoNberLayer (XtomoEntryRank) ;
  if(MulLay==1){
    if ( Side <0 ){
    //for negative s side
      XtomoDela     = aAmdcsimrecStand.GetXtomoML1NDela  (XtomoEntryRank) ;
      XtomoNberTube = aAmdcsimrecStand.GetXtomoNberTube1 (XtomoEntryRank) ;
      XtomoZpit     = aAmdcsimrecStand.GetXtomoML1NZpit  (XtomoEntryRank) ;
      XtomoYpit     = aAmdcsimrecStand.GetXtomoML1NYpit  (XtomoEntryRank) ;
      XtomoDelz     = aAmdcsimrecStand.GetXtomoML1NDelz  (XtomoEntryRank) ;
      XtomoDely     = aAmdcsimrecStand.GetXtomoML1NDely  (XtomoEntryRank) ;      
    }else{
    //for positive s side
      XtomoDela     = aAmdcsimrecStand.GetXtomoML1PDela  (XtomoEntryRank) ;
      XtomoNberTube = aAmdcsimrecStand.GetXtomoNberTube1 (XtomoEntryRank) ;
      XtomoZpit     = aAmdcsimrecStand.GetXtomoML1PZpit  (XtomoEntryRank) ;
      XtomoYpit     = aAmdcsimrecStand.GetXtomoML1PYpit  (XtomoEntryRank) ;
      XtomoDelz     = aAmdcsimrecStand.GetXtomoML1PDelz  (XtomoEntryRank) ;
      XtomoDely     = aAmdcsimrecStand.GetXtomoML1PDely  (XtomoEntryRank) ;
    }
  }

  if(MulLay==2){
    if ( Side <0 ){
    //for negative side
      XtomoDela     = aAmdcsimrecStand.GetXtomoML2NDela  (XtomoEntryRank) ;
      XtomoNberTube = aAmdcsimrecStand.GetXtomoNberTube2 (XtomoEntryRank) ;
      XtomoZpit     = aAmdcsimrecStand.GetXtomoML2NZpit  (XtomoEntryRank) ;
      XtomoYpit     = aAmdcsimrecStand.GetXtomoML2NYpit  (XtomoEntryRank) ;
      XtomoDelz     = aAmdcsimrecStand.GetXtomoML2NDelz  (XtomoEntryRank) ;
      XtomoDely     = aAmdcsimrecStand.GetXtomoML2NDely  (XtomoEntryRank) ;      
    }else{
    //for positive s side
      XtomoDela     = aAmdcsimrecStand.GetXtomoML2PDela  (XtomoEntryRank) ;
      XtomoNberTube = aAmdcsimrecStand.GetXtomoNberTube2 (XtomoEntryRank) ;
      XtomoZpit     = aAmdcsimrecStand.GetXtomoML2PZpit  (XtomoEntryRank) ;
      XtomoYpit     = aAmdcsimrecStand.GetXtomoML2PYpit  (XtomoEntryRank) ;
      XtomoDelz     = aAmdcsimrecStand.GetXtomoML2PDelz  (XtomoEntryRank) ;
      XtomoDely     = aAmdcsimrecStand.GetXtomoML2PDely  (XtomoEntryRank) ;      
    }
  }
  
//Pedestrian way
  double NominalZpitchL = 0. ;
  double NominalYpitchL = 0. ;
  int CurrentMuLay = 0 ;
  int jtyp = aAmdcsimrecStand.GetJtyp(TheStationName);
  int jgeo = aAmdcsimrecStand.IGEO(jtyp ,jff, jzz) ;
  int NobjTot = aAmdcsimrecStand.NOBJ(jtyp,jgeo) ;
  for (int jobj = 1 ; jobj <= NobjTot; jobj++){
    int jtec = aAmdcsimrecStand.ITEC(jtyp,jgeo,jobj) ;
    if ( aAmdcsimrecStand.TechnologyName(jtec) != "MDT" ) continue ;
    CurrentMuLay += 1 ;
    if ( CurrentMuLay == MulLay) {
      int jsta = aAmdcsimrecStand.ISTA(jtyp,jgeo,jobj) ;
      NominalZpitchL = aAmdcsimrecStand.STAPP(jtec,jsta) ;
      NominalYpitchL = aAmdcsimrecStand.STATT(jtec,jsta,2) - aAmdcsimrecStand.STATT(jtec,jsta,1) ;
//    NominalZpitchL=30.035;
//    NominalYpitchL=26.011;
//    NominalYpitchL=26.0111;
      break;
    }
  }
  
 //Re-signe nominal pitches due to different layer nbering
  nominalzpitch = XtomoZpit/std::abs( XtomoZpit)*NominalZpitchL ;
  nominalypitch = XtomoYpit/std::abs( XtomoYpit)*NominalYpitchL ;

 //choose what kind of xtomo information should be included  
 
  //apply to all stations
  if (optionflag==0){;}
 
 //apply only to stations passed xtomo 
  if (optionflag==1){     if(XtomoPassed==0) return; }
 
 // do not apply xtomo delta alpha correction
  if (optionflag==2){ XtomoDela=0; }

  //do not apply xtomo delta y correction
  if (optionflag==3){ XtomoDely=0; }

  // do not apply xtomo delta z correction
  if (optionflag==4) { XtomoDelz=0; }	

  //do not apply xtomo y pitch correction
  if (optionflag==5) { XtomoYpit=nominalypitch; }

  //do not apply xtomo z pitch correction
  if (optionflag==6) { XtomoZpit=nominalzpitch; }

  Status = 1 ;
  
}
